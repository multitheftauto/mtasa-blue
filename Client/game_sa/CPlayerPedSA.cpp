/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPlayerPedSA.cpp
 *  PURPOSE:     Player ped entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <core/CCoreInterface.h>
#include <multiplayer/CMultiplayer.h>
#include "CAnimBlendAssocGroupSA.h"
#include "CAnimManagerSA.h"
#include "CGameSA.h"
#include "CPlayerInfoSA.h"
#include "CPlayerPedSA.h"
#include "CWorldSA.h"
#include "CProjectileInfoSA.h"

extern CCoreInterface* g_pCore;
extern CGameSA*        pGame;

class CPedClothesDesc;

static CPedClothesDesc*    pLocalClothes = 0;
static CWantedSAInterface* pLocalWanted = 0;
static std::set<SString>   ms_DoneAnimBlockRefMap;

CPlayerPedSA::CPlayerPedSA(unsigned int nModelIndex)
{
    // based on CPlayerPed::SetupPlayerPed (R*)
    DWORD CPedOperatorNew = FUNC_CPedOperatorNew;
    DWORD CPlayerPedConstructor = FUNC_CPlayerPedConstructor;

    DWORD dwPedPointer = 0;
    _asm
    {
        push    SIZEOF_CPLAYERPED
        call    CPedOperatorNew
        add     esp, 4

        mov     dwPedPointer, eax

        mov     ecx, eax
        push    0 // set to 0 and they'll behave like AI peds
        push    1
        call    CPlayerPedConstructor
    }

    SetInterface((CEntitySAInterface*)dwPedPointer);

    Init();            // init our interfaces
    CPoolsSA* pools = (CPoolsSA*)pGame->GetPools();
    CWorldSA* world = (CWorldSA*)pGame->GetWorld();

    SetModelIndex(nModelIndex);
    BeingDeleted = false;
    DoNotRemoveFromGame = false;
    SetType(PLAYER_PED);

    // Allocate a player data struct and set it as the players
    m_bIsLocal = false;
    m_pData = new CPlayerPedDataSAInterface;

    // Copy the local player data so we're defaulted to something good
    CPlayerPedSA* pLocalPlayerSA = dynamic_cast<CPlayerPedSA*>(pools->GetPedFromRef((DWORD)1));
    if (pLocalPlayerSA)
        MemCpyFast(m_pData, ((CPlayerPedSAInterface*)pLocalPlayerSA->GetInterface())->pPlayerData, sizeof(CPlayerPedDataSAInterface));

    // Replace the player ped data in our ped interface with the one we just created
    GetPlayerPedInterface()->pPlayerData = m_pData;

    // Set default stuff
    m_pData->m_bRenderWeapon = true;
    m_pData->m_Wanted = pLocalWanted;
    m_pData->m_fTimeCanRun = 1000.0f;

    // Clothes pointers or we'll crash later (TODO: Wrap up with some cloth classes and make it unique per player)
    m_pData->m_pClothes = pLocalClothes;

    // Not sure why was this here (svn blame reports that this line came from the old SVN),
    // but it's causing a bug in what the just streamed-in players that are in the air are
    // processed as if they would be standing on some surface, screwing velocity calculations
    // for players floating in air (using superman script, for example) because GTA:SA will
    // try to apply the floor friction to their velocity.
    // SetIsStanding ( true );

    GetPlayerPedInterface()->pedFlags.bCanBeShotInVehicle = true;
    GetPlayerPedInterface()->pedFlags.bTestForShotInVehicle = true;
    // Stop remote players targeting eachother, this also stops the local player targeting them (needs to be fixed)
    GetPlayerPedInterface()->pedFlags.bNeverEverTargetThisPed = true;
    GetPlayerPedInterface()->pedFlags.bIsLanding = false;
    GetPlayerPedInterface()->fRotationSpeed = 7.5;
    m_pInterface->bStreamingDontDelete = true;
    m_pInterface->bDontStream = true;
    world->Add(m_pInterface, CPlayerPed_Constructor);
}

CPlayerPedSA::CPlayerPedSA(CPlayerPedSAInterface* pPlayer)
{
    // based on CPlayerPed::SetupPlayerPed (R*)
    SetInterface((CEntitySAInterface*)pPlayer);

    Init();
    CPoolsSA* pools = (CPoolsSA*)pGame->GetPools();
    SetType(PLAYER_PED);
    BeingDeleted = false;

    m_bIsLocal = true;
    DoNotRemoveFromGame = true;
    m_pData = GetPlayerPedInterface()->pPlayerData;
    m_pWanted = NULL;

    GetPlayerPedInterface()->pedFlags.bCanBeShotInVehicle = true;
    GetPlayerPedInterface()->pedFlags.bTestForShotInVehicle = true;
    GetPlayerPedInterface()->pedFlags.bIsLanding = false;
    GetPlayerPedInterface()->fRotationSpeed = 7.5;

    pLocalClothes = m_pData->m_pClothes;
    pLocalWanted = m_pData->m_Wanted;

    GetPlayerPedInterface()->pedFlags.bCanBeShotInVehicle = true;
    // Something resets this, constantly
    GetPlayerPedInterface()->pedFlags.bTestForShotInVehicle = true;
    // Stop remote players targeting the local (need to stop them targeting eachother too)
    GetPlayerPedInterface()->pedFlags.bNeverEverTargetThisPed = true;
}

CPlayerPedSA::~CPlayerPedSA()
{
    if (!BeingDeleted && DoNotRemoveFromGame == false)
    {
        DWORD dwInterface = (DWORD)m_pInterface;

        if ((DWORD)GetInterface()->vtbl != VTBL_CPlaceable)
        {
            CWorldSA* world = (CWorldSA*)pGame->GetWorld();
            pGame->GetProjectileInfo()->RemoveEntityReferences(this);
            world->Remove(m_pInterface, CPlayerPed_Destructor);

            DWORD dwThis = (DWORD)m_pInterface;
            DWORD dwFunc = m_pInterface->vtbl->SCALAR_DELETING_DESTRUCTOR;            // we use the vtbl so we can be type independent
            _asm
            {
                mov     ecx, dwThis
                push    1            // delete too
                call    dwFunc
            }
        }
        BeingDeleted = true;
        ((CPoolsSA*)pGame->GetPools())->RemovePed((CPed*)(CPedSA*)this, false);
    }

    // Delete the player data
    if (!m_bIsLocal)
    {
        delete m_pData;
    }
}

CWanted* CPlayerPedSA::GetWanted()
{
    return m_pWanted;
}

void CPlayerPedSA::SetInitialState()
{
    DWORD dwUnknown = 1;
    DWORD dwFunction = FUNC_SetInitialState;
    DWORD dwThis = (DWORD)m_pInterface;
    _asm
    {
        push    dwUnknown
        mov     ecx, dwThis
        call    dwFunction
    }

    // Avoid direction locks for respawning after a jump
    GetPlayerPedInterface()->pedFlags.bIsLanding = false;
    // Avoid direction locks for respawning after a sprint stop
    GetPlayerPedInterface()->fRotationSpeed = 7.5;
    // This seems to also be causing some movement / direction locks
    GetPlayerPedInterface()->pedFlags.bStayInSamePlace = false;
}

eMoveAnim CPlayerPedSA::GetMoveAnim()
{
    CPedSAInterface* pedInterface = (CPedSAInterface*)GetInterface();
    return (eMoveAnim)pedInterface->iMoveAnimGroup;
}

// Helper for SetMoveAnim
bool IsBlendAssocGroupLoaded(int iGroup)
{
    CAnimBlendAssocGroupSAInterface* pBlendAssocGroup = *(CAnimBlendAssocGroupSAInterface**)0xB4EA34;
    pBlendAssocGroup += iGroup;
    return pBlendAssocGroup->pAnimBlock != NULL;
}

// Helper for SetMoveAnim
bool IsBlendAssocGroupValid(int iGroup)
{
    CAnimBlendAssocGroupSAInterface* pBlendAssocGroup = *(CAnimBlendAssocGroupSAInterface**)0xB4EA34;
    pBlendAssocGroup += iGroup;
    if (pBlendAssocGroup->pAnimBlock == NULL)
        return false;
    if (pBlendAssocGroup->pAssociationsArray == NULL)
        return false;
    int moveIds[] = {0, 1, 2, 3, 5};
    for (uint i = 0; i < NUMELMS(moveIds); i++)
    {
        int                                     iUseAnimId = moveIds[i] - pBlendAssocGroup->iIDOffset;
        CAnimBlendStaticAssociationSAInterface* pAssociation = pBlendAssocGroup->pAssociationsArray + iUseAnimId;
        if (pAssociation == NULL)
            return false;
        if (pAssociation->pAnimHeirarchy == NULL)
            return false;
    }
    return true;
}

void CPlayerPedSA::SetMoveAnim(eMoveAnim iAnimGroup)
{
    // Range check
    if (!IsValidMoveAnim(iAnimGroup))
        return;

    if (iAnimGroup == MOVE_DEFAULT)
    {
        m_iCustomMoveAnim = 0;
        return;
    }

    // Find which anim block to load
    SString strBlockName;
    switch (iAnimGroup)
    {
        case 55:
        case 58:
        case 61:
        case 64:
        case 67:
            strBlockName = "fat";
            break;

        case 56:
        case 59:
        case 62:
        case 65:
        case 68:
            strBlockName = "muscular";
            break;

        case 138:
            strBlockName = "skate";
            break;

        default:
            strBlockName = "ped";
            break;
    }

    // Need to load ?
    if (!IsBlendAssocGroupLoaded(iAnimGroup))
    {
        std::unique_ptr<CAnimBlock> pAnimBlock = pGame->GetAnimManager()->GetAnimationBlock(strBlockName);

        // Try load
        if (pAnimBlock && !pAnimBlock->IsLoaded())
        {
            pAnimBlock->Request(BLOCKING, true);
            MapInsert(ms_DoneAnimBlockRefMap, strBlockName);            // Request() adds a ref for us
        }

        // Load fail?
        if (!IsBlendAssocGroupLoaded(iAnimGroup))
            return;
    }

    // Check it all looks good
    if (!IsBlendAssocGroupValid(iAnimGroup))
        return;

    // Ensure we add a ref to this block, even if it wasn't loaded by us
    std::unique_ptr<CAnimBlock> pAnimBlock = pGame->GetAnimManager()->GetAnimationBlock(strBlockName);
    if (!pAnimBlock)
        return;
    if (!MapContains(ms_DoneAnimBlockRefMap, strBlockName))
    {
        MapInsert(ms_DoneAnimBlockRefMap, strBlockName);
        pAnimBlock->AddRef();
    }

    m_iCustomMoveAnim = iAnimGroup;

    // Set the the new move animation group now, although it does get updated through the hooks as well
    CPedSAInterface* pedInterface = (CPedSAInterface*)GetInterface();
    pedInterface->iMoveAnimGroup = (int)iAnimGroup;

    DWORD dwThis = (DWORD)pedInterface;
    DWORD dwFunc = FUNC_CPlayerPed_ReApplyMoveAnims;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

////////////////////////////////////////////////////////////////
//
// Hooks for CPlayerPedSA
//
////////////////////////////////////////////////////////////////

/*
    Summary of move anim types
    --------------------------

    Physique based anims with plain pose:
        MOVE_PLAYER = 54,
        MOVE_PLAYER_F,
        MOVE_PLAYER_M,

    Physique based anims with special pose:
        MOVE_ROCKET,
        MOVE_ROCKET_F,
        MOVE_ROCKET_M,
        MOVE_ARMED,
        MOVE_ARMED_F,
        MOVE_ARMED_M,
        MOVE_BBBAT,
        MOVE_BBBAT_F,
        MOVE_BBBAT_M,
        MOVE_CSAW,
        MOVE_CSAW_F,
        MOVE_CSAW_M,

    Non physique based pose:
        MOVE_JETPACK,

    Non physique based anims:
        MOVE_SNEAK,
        MOVE_MAN = 118,
        MOVE_SHUFFLE,
        MOVE_OLDMAN,
        MOVE_GANG1,
        MOVE_GANG2,
        MOVE_OLDFATMAN,
        MOVE_FATMAN,
        MOVE_JOGGER,
        MOVE_DRUNKMAN,
        MOVE_BLINDMAN,
        MOVE_SWAT,
        MOVE_WOMAN,
        MOVE_SHOPPING,
        MOVE_BUSYWOMAN,
        MOVE_SEXYWOMAN,
        MOVE_PRO,
        MOVE_OLDWOMAN,
        MOVE_FATWOMAN,
        MOVE_JOGWOMAN,
        MOVE_OLDFATWOMAN,
        MOVE_SKATE,
*/

bool IsAnimJetPack(int iAnim)
{
    return iAnim == MOVE_JETPACK;
}

// True if anim has Std/Fat/Muscular variants
bool IsAnimPhysiqueBased(int iAnim)
{
    return (iAnim >= MOVE_PLAYER) && (iAnim <= MOVE_CSAW_M);
}

// Special pose is MOVE_ROCKET, MOVE_ARMED, MOVE_BBBAT or MOVE_CSAW
bool IsAnimSpecialPose(int iAnim)
{
    return (iAnim >= MOVE_ROCKET) && (iAnim <= MOVE_CSAW_M);
}

// 0-Std  1-Fat  2-Muscular
int GetAnimPhysiqueIndex(int iAnim)
{
    return (iAnim - MOVE_PLAYER) % 3;
}

// Pose is MOVE_PLAYER, MOVE_ROCKET, MOVE_ARMED, MOVE_BBBAT or MOVE_CSAW
int GetAnimPose(int iAnim)
{
    return iAnim - GetAnimPhysiqueIndex(iAnim);
}

////////////////////////////////////////////////////////////////
//
// CPlayerPed_ProcessAnimGroups_Mid
//
// Stop GTA overriding our custom move anim
// Returns anim to use
//
////////////////////////////////////////////////////////////////
__declspec(noinline) int _cdecl OnCPlayerPed_ProcessAnimGroups_Mid(CPlayerPedSAInterface* pPlayerPedSAInterface, int iReqMoveAnim)
{
    SClientEntity<CPedSA>* pPedClientEntity = pGame->GetPools()->GetPed((DWORD*)pPlayerPedSAInterface);
    CPed*                  pPed = pPedClientEntity ? pPedClientEntity->pEntity : nullptr;

    if (pPed)
    {
        int iCustomAnim = pPed->GetCustomMoveAnim();
        if (iCustomAnim)
        {
            // If iReqMoveAnim is jetpack, use iReqMoveAnim
            if (IsAnimJetPack(iReqMoveAnim))
                return iReqMoveAnim;

            // If iCustomAnim is physique based without special pose, and iReqMoveAnim is physique based, then use pose from iReqMoveAnim and physique from
            // iCustomAnim
            if (IsAnimPhysiqueBased(iCustomAnim) && !IsAnimSpecialPose(iCustomAnim) && IsAnimPhysiqueBased(iReqMoveAnim))
            {
                int iPhysique = GetAnimPhysiqueIndex(iCustomAnim);
                int iPose = GetAnimPose(iReqMoveAnim);
                return iPose + iPhysique;
            }

            // Everything else, use iCustomAnim
            return iCustomAnim;
        }
    }

    return iReqMoveAnim;
}

// Hook info
#define HOOKPOS_CPlayerPed_ProcessAnimGroups_Mid        0x0609A44
#define HOOKSIZE_CPlayerPed_ProcessAnimGroups_Mid       6
DWORD RETURN_CPlayerPed_ProcessAnimGroups_Mid = 0x0609A4A;
void _declspec(naked) HOOK_CPlayerPed_ProcessAnimGroups_Mid()
{
    _asm
    {
        pushad
        push    eax
        push    esi
        call    OnCPlayerPed_ProcessAnimGroups_Mid
        mov     [esp+0],eax         // Put temp
        add     esp, 4*2
        popad

        mov     eax,[esp-32-4*2]    // Get temp
        cmp     [esi+4D4h], eax     // pPed->iMoveAnim

        jmp     RETURN_CPlayerPed_ProcessAnimGroups_Mid
    }
}

////////////////////////////////////////////////////////////////
//
// CClothes_GetDefaultPlayerMotionGroup
//
// Stop GTA overriding the physique of our custom move anim
// Returns MOVE_PLAYER, MOVE_PLAYER_F or MOVE_PLAYER_M
//
////////////////////////////////////////////////////////////////
__declspec(noinline) int _cdecl OnCClothes_GetDefaultPlayerMotionGroup(int iReqMoveAnim)
{
    CPed* pPed = g_pCore->GetMultiplayer()->GetContextSwitchedPed();

    if (pPed)
    {
        int iCustomAnim = pPed->GetCustomMoveAnim();
        if (iCustomAnim)
        {
            if (IsAnimPhysiqueBased(iCustomAnim))
            {
                int iPhysique = GetAnimPhysiqueIndex(iCustomAnim);
                int iBaseAnim = MOVE_PLAYER + iPhysique;
                return iBaseAnim;
            }
        }
    }

    return iReqMoveAnim;
}

// Hook info
#define HOOKPOS_CClothes_GetDefaultPlayerMotionGroup        0x05A81B0
#define HOOKSIZE_CClothes_GetDefaultPlayerMotionGroup       5
DWORD RETURN_CClothes_GetDefaultPlayerMotionGroup = 0x05A81B5;
void _declspec(naked) HOOK_CClothes_GetDefaultPlayerMotionGroup()
{
    _asm
    {
        mov     eax, 0x05A7FB0      // CClothes::GetPlayerMotionGroupToLoad
        call    eax

        pushad
        push    eax
        call    OnCClothes_GetDefaultPlayerMotionGroup
        mov     [esp+0],eax         // Put temp
        add     esp, 4*1
        popad

        mov     eax,[esp-32-4*1]    // Get temp
        jmp     RETURN_CClothes_GetDefaultPlayerMotionGroup
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CPlayerPedSA::StaticSetHooks()
{
    EZHookInstall(CPlayerPed_ProcessAnimGroups_Mid);
    EZHookInstall(CClothes_GetDefaultPlayerMotionGroup);
}
