/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CVehicleSA.cpp
 *  PURPOSE:     Vehicle base entity
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CAutomobileSA.h"
#include "CBikeSA.h"
#include "CCameraSA.h"
#include "CColModelSA.h"
#include "CFxManagerSA.h"
#include "CFxSystemSA.h"
#include "CGameSA.h"
#include "CProjectileInfoSA.h"
#include "CTrainSA.h"
#include "CPlaneSA.h"
#include "CVehicleSA.h"
#include "CBoatSA.h"
#include "CVisibilityPluginsSA.h"
#include "CWorldSA.h"
#include "gamesa_renderware.h"

extern CGameSA* pGame;

static BOOL m_bVehicleSunGlare = false;
_declspec(naked) void DoVehicleSunGlare(void* this_)
{
    _asm {
        mov eax, FUNC_CVehicle_DoSunGlare
        jmp eax
    }
}

void _declspec(naked) HOOK_Vehicle_PreRender(void)
{
    _asm {
        mov    ecx, m_bVehicleSunGlare
        cmp    ecx, 0
        jle    noglare
        mov    ecx, esi
        call DoVehicleSunGlare
    noglare:
        mov [esp+0D4h], edi
        push 6ABD04h
        retn
    }
}

namespace
{
    bool ClumpDumpCB(RpAtomic* pAtomic, void* data)
    {
        CVehicleSA* pVehicleSA = (CVehicleSA*)data;
        RwFrame*    pFrame = RpGetFrame(pAtomic);
        pVehicleSA->AddComponent(pFrame, false);
        // g_pCore->GetConsole()->Print ( SString ( "Atomic:%08x  Frame:%08x %s", pAtomic, pFrame, pFrame ? pFrame->szName : "" ) );
        // OutputDebugLine ( SString ( "Atomic:%08x  Frame:%08x %s", pAtomic, pFrame, pFrame ? pFrame->szName : "" ) );
        return true;
    }

    void ClumpDump(RpClump* pClump, CVehicleSA* pVehicleSA)
    {
        // get the clump's frame
        RwFrame* pFrame = RpGetFrame(pClump);
        // OutputDebugLine ( SStringX ( "--------------------------------" ) );
        // OutputDebugLine ( SString ( "Clump:%08x  Frame:%08x %s", pClump, pFrame, pFrame ? pFrame->szName : "" ) );
        // g_pCore->GetConsole()->Print ( SStringX ( "--------------------------------" ) );
        // g_pCore->GetConsole()->Print ( SString ( "Clump:%08x  Frame:%08x %s", pClump, pFrame, pFrame ? pFrame->szName : "" ) );
        // Do for all atomics
        RpClumpForAllAtomics(pClump, ClumpDumpCB, pVehicleSA);
    }

    // Recursive RwFrame children searching function
    void RwFrameDump(RwFrame* parent, CVehicleSA* pVehicleSA)
    {
        RwFrame* ret = parent->child;
        while (ret != NULL)
        {
            // recurse into the child
            if (ret->child != NULL)
            {
                RwFrameDump(ret, pVehicleSA);
            }
            // don't re-add, check ret for validity, if it has an empty string at this point it isn't a variant or it's already added
            if (pVehicleSA->IsComponentPresent(ret->szName) == false && ret->szName != "")
            {
                pVehicleSA->AddComponent(ret, true);
            }
            ret = ret->next;
        }
    }

    void VehicleDump(CVehicleSA* pVehicleSA)
    {
        // This grabs 90% of the frames
        ClumpDump(pVehicleSA->GetInterface()->m_pRwObject, pVehicleSA);
        // This grabs the rest which aren't always copied to the Model Info in the interface ( usually markers for things like NOS )
        RwFrameDump(RpGetFrame((RpClump*)pGame->GetModelInfo(pVehicleSA->GetModelIndex())->GetRwObject()), pVehicleSA);
    }

    RwObject* __cdecl GetAllAtomicObjectCB(struct RwObject* object, void* data)
    {
        std::vector<RwObject*>& result = *((std::vector<RwObject*>*)data);
        result.push_back(object);
        return object;
    }

    // Get all atomics for this frame (even if they are invisible)
    void GetAllAtomicObjects(RwFrame* frame, std::vector<RwObject*>& result) { RwFrameForAllObjects(frame, (void*)GetAllAtomicObjectCB, &result); }
}            // namespace

void CVehicleSA::Init()
{
    GetVehicleInterface()->bStreamingDontDelete = true;
    GetVehicleInterface()->bDontStream = true;
    BeingDeleted = false;

    // Store our CVehicleSA pointer in the vehicle's time of creation member (as it won't get modified later and as far as I know it isn't used for something
    // important)
    GetVehicleInterface()->m_pVehicle = this;

    CModelInfo* modelInfo = pGame->GetModelInfo(GetModelIndex());

    if (modelInfo != nullptr)
    {
        for (size_t i = 0; i < m_dummyPositions.size(); ++i)
        {
            m_dummyPositions[i] = modelInfo->GetVehicleDummyPosition((eVehicleDummies)i);
        }
    }

    // Unlock doors as they spawn randomly with locked doors
    LockDoors(false);

    // Reset the car countss to 0 so that this vehicle doesn't affect the population vehicles
    for (int i = 0; i < 5; i++)
    {
        MemPutFast<DWORD>(VARS_CarCounts + i * sizeof(DWORD), 0);
    }

    // only applicable for CAutomobile based vehicles (i.e. not bikes, trains or boats, but includes planes, helis etc)
    m_pDamageManager = new CDamageManagerSA(m_pInterface, (CDamageManagerSAInterface*)((DWORD)GetInterface() + 1440));

    m_pVehicleAudioEntity = new CAEVehicleAudioEntitySA(&GetVehicleInterface()->m_VehicleAudioEntity);

    // Replace the handling interface with our own to prevent handlig.cfg cheats and allow custom handling stuff.
    // We don't use SA's array because we want one handling per vehicle type and also allow custom handlings
    // per car later.
    /*CHandlingEntry* pEntry = pGame->GetHandlingManager ()->CreateHandlingData ();
    //CHandlingEntry* pEntry = pGame->GetHandlingManager ()->GetHandlingData ( dwModelID );
    pEntry->ApplyHandlingData ( pGame->GetHandlingManager ()->GetHandlingData ( dwModelID ) );  // We need to do that so vehicle handling wont get corrupted
    SetHandlingData ( pEntry );
    pEntry->Recalculate ();*/

    GetVehicleInterface()->m_nVehicleFlags.bVehicleCanBeTargetted = true;

    m_RGBColors[0] = CVehicleColor::GetRGBFromPaletteIndex(GetVehicleInterface()->m_colour1);
    m_RGBColors[1] = CVehicleColor::GetRGBFromPaletteIndex(GetVehicleInterface()->m_colour2);
    m_RGBColors[2] = CVehicleColor::GetRGBFromPaletteIndex(GetVehicleInterface()->m_colour3);
    m_RGBColors[3] = CVehicleColor::GetRGBFromPaletteIndex(GetVehicleInterface()->m_colour4);
    SetColor(m_RGBColors[0], m_RGBColors[1], m_RGBColors[2], m_RGBColors[3], 0);

    // Initialize doors depending on the vtable.
    DWORD dwOffset;
    DWORD dwFunc = ((CVehicleSAInterfaceVTBL*)GetVehicleInterface()->vtbl)->GetDoorAngleOpenRatio_;
    if (dwFunc == FUNC_CAutomobile__GetDoorAngleOpenRatio)
        dwOffset = 1464;
    else if (dwFunc == FUNC_CTrain__GetDoorAngleOpenRatio)
        dwOffset = 1496;
    else
        dwOffset = 0;            // Other vehicles don't have door information.

    if (dwOffset != 0)
    {
        for (unsigned int i = 0; i < sizeof(m_doors) / sizeof(m_doors[0]); ++i)
        {
            DWORD dwInterface = (DWORD)GetInterface();
            DWORD dwDoorAddress = dwInterface + 1464 + i * 24;
            m_doors[i].SetInterface((CDoorSAInterface*)dwDoorAddress);
        }
    }

    CopyGlobalSuspensionLinesToPrivate();
}

CVehicleSA::~CVehicleSA()
{
    if (!BeingDeleted)
    {
        if ((DWORD)m_pInterface->vtbl != VTBL_CPlaceable)
        {
            GetVehicleInterface()->m_pVehicle = nullptr;

            if (m_pDamageManager)
            {
                delete m_pDamageManager;
                m_pDamageManager = nullptr;
            }

            if (m_pVehicleAudioEntity)
            {
                delete m_pVehicleAudioEntity;
                m_pVehicleAudioEntity = nullptr;
            }

            if (m_pSuspensionLines)
            {
                delete[] m_pSuspensionLines;
                m_pSuspensionLines = nullptr;
            }

            DWORD dwThis = (DWORD)m_pInterface;
            DWORD dwFunc = 0x6D2460;            // CVehicle::ExtinguishCarFire
            _asm
            {
                mov     ecx, dwThis
                call    dwFunc
            }

            CWorldSA* pWorld = (CWorldSA*)pGame->GetWorld();
            pWorld->Remove(m_pInterface, CVehicle_Destructor);
            pWorld->RemoveReferencesToDeletedObject(m_pInterface);

            dwFunc = m_pInterface->vtbl->SCALAR_DELETING_DESTRUCTOR;            // we use the vtbl so we can be vehicle type independent
            _asm
            {
                mov     ecx, dwThis
                push    1           //delete too
                call    dwFunc
            }
        }
        BeingDeleted = true;
        ((CPoolsSA*)pGame->GetPools())->RemoveVehicle((CVehicle*)this);
    }
}

void CVehicleSA::SetMoveSpeed(CVector* vecMoveSpeed)
{
    DWORD dwFunc = FUNC_GetMoveSpeed;
    DWORD dwThis = (DWORD)GetInterface();
    DWORD dwReturn = 0;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     dwReturn, eax
    }
    MemCpyFast((void*)dwReturn, vecMoveSpeed, sizeof(CVector));

    // INACCURATE. Use Get/SetTrainSpeed instead of Get/SetMoveSpeed. (Causes issue #4829).
#if 0
    // In case of train: calculate on-rail speed
    WORD wModelID = GetModelIndex();
    if ( wModelID == 537 || wModelID == 538 || wModelID == 569 || wModelID == 570 || wModelID == 590 || wModelID == 449 )
    {
        if ( !IsDerailed () )
        {
            CVehicleSAInterface* pInterf = GetVehicleInterface ();

            // Find the rail node we are on
            DWORD dwNumNodes = ((DWORD *)ARRAY_NumRailTrackNodes) [ pInterf->m_ucRailTrackID ];
            SRailNodeSA* pNode = ( (SRailNodeSA **) ARRAY_RailTrackNodePointers ) [ pInterf->m_ucRailTrackID ];
            SRailNodeSA* pNodesEnd = &pNode [ dwNumNodes ];
            while ( (float)pNode->sRailDistance / 3.0f <= pInterf->m_fTrainRailDistance && pNode < pNodesEnd )
            {
                pNode++;
            }
            if ( pNode >= pNodesEnd )
                return;
            // Get the direction vector between the nodes the train is between
            CVector vecNode1 ( (float)(pNode - 1)->sX / 8.0f, (float)(pNode - 1)->sY / 8.0f, (float)(pNode - 1)->sZ / 8.0f );
            CVector vecNode2 ( (float)pNode->sX / 8.0f, (float)pNode->sY / 8.0f, (float)pNode->sZ / 8.0f );
            CVector vecDirection = vecNode2 - vecNode1;
            vecDirection.Normalize ();
            // Set the speed
            pInterf->m_fTrainSpeed = vecDirection.DotProduct ( vecMoveSpeed );
        }
    }
#endif
}

CTrainSAInterface* CVehicleSA::GetNextCarriageInTrain()
{
    return (CTrainSAInterface*)*(DWORD*)((DWORD)GetInterface() + 1492);
}

CVehicle* CVehicleSA::GetNextTrainCarriage()
{
    CTrainSAInterface* pVehicle = GetNextCarriageInTrain();
    if (pVehicle)
    {
        SClientEntity<CVehicleSA>* pVehicleClientEntity = pGame->GetPools()->GetVehicle((DWORD*)pVehicle);
        return pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
    }
    return nullptr;
}

bool CVehicleSA::AddProjectile(eWeaponType eWeapon, CVector vecOrigin, float fForce, CVector* target, CEntity* targetEntity)
{
    return ((CProjectileInfoSA*)pGame->GetProjectileInfo())->AddProjectile((CEntitySA*)this, eWeapon, vecOrigin, fForce, target, targetEntity);
}

void CVehicleSA::SetNextTrainCarriage(CVehicle* pNext)
{
    if (pNext)
    {
        CVehicleSA* pNextVehicle = dynamic_cast<CVehicleSA*>(pNext);
        if (pNextVehicle)
        {
            MemPutFast<DWORD>((DWORD)GetInterface() + 1492, (DWORD)pNextVehicle->GetInterface());
            if (pNextVehicle->GetPreviousTrainCarriage() != this)
                pNextVehicle->SetPreviousTrainCarriage(this);
        }
    }
    else
    {
        MemPutFast<DWORD>((DWORD)GetInterface() + 1492, NULL);
    }
}

CTrainSAInterface* CVehicleSA::GetPreviousCarriageInTrain()
{
    return (CTrainSAInterface*)*(DWORD*)((DWORD)GetInterface() + 1488);
}

void CVehicleSA::SetPreviousTrainCarriage(CVehicle* pPrevious)
{
    if (pPrevious)
    {
        CVehicleSA* pPreviousVehicle = dynamic_cast<CVehicleSA*>(pPrevious);
        if (pPreviousVehicle)
        {
            MemPutFast<DWORD>((DWORD)GetInterface() + 1488, (DWORD)pPreviousVehicle->GetInterface());
            if (pPreviousVehicle->GetNextTrainCarriage() != this)
                pPreviousVehicle->SetNextTrainCarriage(this);
        }
    }
    else
    {
        MemPutFast<DWORD>((DWORD)GetInterface() + 1488, NULL);
    }
}

CVehicle* CVehicleSA::GetPreviousTrainCarriage()
{
    CTrainSAInterface* pVehicle = GetPreviousCarriageInTrain();
    if (pVehicle)
    {
        SClientEntity<CVehicleSA>* pVehicleClientEntity = pGame->GetPools()->GetVehicle((DWORD*)pVehicle);
        return pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
    }
    return nullptr;
}

float CVehicleSA::GetDistanceToCarriage(CVehicle* pCarriage)
{
    auto pCarriageInterface = static_cast<CTrainSAInterface*>(pCarriage->GetVehicleInterface());
    if (pCarriageInterface->trainFlags.bClockwiseDirection)
    {
        CBoundingBox* pBoundingBox = pGame->GetModelInfo(pCarriage->GetModelIndex())->GetBoundingBox();
        return -(pBoundingBox->vecBoundMax.fY - pBoundingBox->vecBoundMin.fY);
    }
    else
    {
        CBoundingBox* pBoundingBox = pGame->GetModelInfo(GetModelIndex())->GetBoundingBox();
        return pBoundingBox->vecBoundMax.fY - pBoundingBox->vecBoundMin.fY;
    }
}

void CVehicleSA::AttachTrainCarriage(CVehicle* pCarriage)
{
    if (!pCarriage)
        return;

    CVehicleSA* pCarriageSA = dynamic_cast<CVehicleSA*>(pCarriage);
    if (!pCarriageSA)
        return;

    // Link both vehicles
    SetNextTrainCarriage(pCarriage);

    // Mark the carriage as non chain engine
    auto pCarriageInterface = static_cast<CTrainSAInterface*>(pCarriage->GetVehicleInterface());
    pCarriageInterface->trainFlags.bIsTheChainEngine = false;

    if (pCarriageInterface->trainFlags.bClockwiseDirection)
    {
        CBoundingBox* pBoundingBox = pGame->GetModelInfo(pCarriage->GetModelIndex())->GetBoundingBox();
        pCarriageInterface->m_fDistanceToNextCarriage = -(pBoundingBox->vecBoundMax.fY - pBoundingBox->vecBoundMin.fY);
    }
    else
    {
        CBoundingBox* pBoundingBox = pGame->GetModelInfo(GetModelIndex())->GetBoundingBox();
        pCarriageInterface->m_fDistanceToNextCarriage = pBoundingBox->vecBoundMax.fY - pBoundingBox->vecBoundMin.fY;
    }
}

void CVehicleSA::DetachTrainCarriage(CVehicle* pCarriage)
{
    SetNextTrainCarriage(NULL);

    if (pCarriage)
    {
        pCarriage->SetPreviousTrainCarriage(NULL);
        pCarriage->SetIsChainEngine(true);
    }
}

bool CVehicleSA::IsChainEngine()
{
    auto pInterface = static_cast<CTrainSAInterface*>(GetVehicleInterface());
    return pInterface->trainFlags.bIsTheChainEngine;
}

void CVehicleSA::SetIsChainEngine(bool bChainEngine)
{
    auto pInterface = static_cast<CTrainSAInterface*>(GetVehicleInterface());
    pInterface->trainFlags.bIsTheChainEngine = bChainEngine;
}

bool CVehicleSA::IsDerailed()
{
    auto pInterface = static_cast<CTrainSAInterface*>(GetVehicleInterface());
    return pInterface->trainFlags.bIsDerailed;
}

void CVehicleSA::SetDerailed(bool bDerailed)
{
    WORD wModelID = GetModelIndex();
    if (wModelID == 537 || wModelID == 538 || wModelID == 569 || wModelID == 570 || wModelID == 590 || wModelID == 449)
    {
        auto  pInterface = static_cast<CTrainSAInterface*>(GetVehicleInterface());
        DWORD dwThis = (DWORD)pInterface;
        if (bDerailed != pInterface->trainFlags.bIsDerailed)
        {
            if (bDerailed)
            {
                pInterface->trainFlags.bIsDerailed = true;
                // update the physical flags
                MemAndFast<DWORD>(dwThis + 64, (DWORD)0xFFFDFFFB);
                // what this does:
                // physicalFlags.bDisableCollisionForce = false (3rd flag)
                // physicalFlags.bDisableSimpleCollision = false (18th flag)
            }
            else
            {
                pInterface->trainFlags.bIsDerailed = false;
                // update the physical flags
                MemOrFast<DWORD>(dwThis + 64, (DWORD)0x20004);
                // what this does:
                // physicalFlags.bDisableCollisionForce = true (3rd flag)
                // physicalFlags.bDisableSimpleCollision = true (18th flag)

                // Recalculate the on-rail distance from the start node (train position parameter, m_fTrainRailDistance)
                DWORD dwFunc = FUNC_CTrain_FindPositionOnTrackFromCoors;
                _asm
                {
                    mov     ecx, dwThis
                        call    dwFunc
                }

                // Reset the speed
                static_cast<CTrainSAInterface*>(GetVehicleInterface())->m_fTrainSpeed = 0.0f;
            }
        }
    }
}

float CVehicleSA::GetTrainSpeed()
{
    auto pInterface = static_cast<CTrainSAInterface*>(GetVehicleInterface());
    return pInterface->m_fTrainSpeed;
}

void CVehicleSA::SetTrainSpeed(float fSpeed)
{
    auto pInterface = static_cast<CTrainSAInterface*>(GetVehicleInterface());
    pInterface->m_fTrainSpeed = fSpeed;
}

void CVehicleSA::SetPlaneRotorSpeed(float fSpeed)
{
    auto pInterface = static_cast<CPlaneSAInterface*>(GetInterface());
    pInterface->m_fPropSpeed = fSpeed;
}

bool CVehicleSA::SetVehicleWheelRotation(float fWheelRot1, float fWheelRot2, float fWheelRot3, float fWheelRot4) noexcept
{ 
    VehicleClass m_eVehicleType = static_cast<VehicleClass>(GetVehicleInterface()->m_vehicleSubClass);
    switch (m_eVehicleType)
    {
        case VehicleClass::AUTOMOBILE:
        case VehicleClass::MONSTER_TRUCK:
        case VehicleClass::QUAD:
        case VehicleClass::TRAILER:
        {
            auto pInterface = static_cast<CAutomobileSAInterface*>(GetInterface());
            pInterface->m_wheelRotation[0] = fWheelRot1;
            pInterface->m_wheelRotation[1] = fWheelRot2;
            pInterface->m_wheelRotation[2] = fWheelRot3;
            pInterface->m_wheelRotation[3] = fWheelRot4;
            return true;
        }
        case VehicleClass::BIKE:
        case VehicleClass::BMX:
        {
            auto pInterface = static_cast<CBikeSAInterface*>(GetInterface());
            pInterface->m_afWheelRotationX[0] = fWheelRot1;
            pInterface->m_afWheelRotationX[1] = fWheelRot2;
            return true;
        }
        default:
            return false;
    }
    return false;
}

float CVehicleSA::GetPlaneRotorSpeed() 
{
    auto pInterface = static_cast<CPlaneSAInterface*>(GetInterface());
    return pInterface->m_fPropSpeed;
}

bool CVehicleSA::GetTrainDirection()
{
    auto pInterface = static_cast<CTrainSAInterface*>(GetInterface());
    return pInterface->trainFlags.bClockwiseDirection;
}

void CVehicleSA::SetTrainDirection(bool bDirection)
{
    auto pInterface = static_cast<CTrainSAInterface*>(GetInterface());
    pInterface->trainFlags.bClockwiseDirection = bDirection;
}

BYTE CVehicleSA::GetRailTrack()
{
    auto pInterface = static_cast<CTrainSAInterface*>(GetVehicleInterface());
    return pInterface->m_ucRailTrackID;
}

void CVehicleSA::SetRailTrack(BYTE ucTrackID)
{
    if (ucTrackID >= NUM_RAILTRACKS)
        return;

    auto pInterf = static_cast<CTrainSAInterface*>(GetVehicleInterface());
    if (pInterf->m_ucRailTrackID != ucTrackID)
    {
        pInterf->m_ucRailTrackID = ucTrackID;
        if (!IsDerailed())
        {
            DWORD dwFunc = FUNC_CTrain_FindPositionOnTrackFromCoors;
            _asm
            {
                mov ecx, pInterf
                call dwFunc
            }
        }
    }
}

float CVehicleSA::GetTrainPosition()
{
    auto pInterface = static_cast<CTrainSAInterface*>(GetVehicleInterface());
    return pInterface->m_fTrainRailDistance;
}

void CVehicleSA::SetTrainPosition(float fPosition, bool bRecalcOnRailDistance)
{
    auto pInterface = static_cast<CTrainSAInterface*>(GetVehicleInterface());
    // if ( pInterface->m_fTrainRailDistance <= fPosition - 0.1 || pInterface->m_fTrainRailDistance >= fPosition + 0.1 )
    {
        pInterface->m_fTrainRailDistance = fPosition;
        if (bRecalcOnRailDistance && !IsDerailed())
        {
            DWORD dwFunc = FUNC_CTrain_FindPositionOnTrackFromCoors;
            _asm
            {
                mov ecx, pInterface
                call dwFunc
            }
        }
    }
}

CDoorSA* CVehicleSA::GetDoor(unsigned char ucDoor)
{
    if (ucDoor <= 5)
        return &m_doors[ucDoor];
    return 0;
}

void CVehicleSA::OpenDoor(unsigned char ucDoor, float fRatio, bool bMakeNoise)
{
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = ((CVehicleSAInterfaceVTBL*)GetVehicleInterface()->vtbl)->OpenDoor;

    // Grab the car node index for the given door id
    static int s_iCarNodeIndexes[6] = {0x10, 0x11, 0x0A, 0x08, 0x0B, 0x09};
    DWORD      dwIdx = s_iCarNodeIndexes[ucDoor];
    DWORD      dwDoor = ucDoor;
    DWORD      dwMakeNoise = bMakeNoise;

    _asm
    {
        mov     ecx, dwThis
        push    dwMakeNoise
        push    fRatio
        push    dwDoor
        push    dwIdx
        push    0
        call    dwFunc
    }
}

void CVehicleSA::SetSwingingDoorsAllowed(bool bAllowed)
{
    m_bSwingingDoorsAllowed = bAllowed;
}

bool CVehicleSA::AreSwingingDoorsAllowed() const
{
    return m_bSwingingDoorsAllowed;
}

bool CVehicleSA::AreDoorsLocked()
{
    return (GetVehicleInterface()->m_doorLock == DOOR_LOCK_LOCKED || GetVehicleInterface()->m_doorLock == DOOR_LOCK_COP_CAR ||
            GetVehicleInterface()->m_doorLock == DOOR_LOCK_LOCKED_PLAYER_INSIDE || GetVehicleInterface()->m_doorLock == DOOR_LOCK_SKIP_SHUT_DOORS ||
            GetVehicleInterface()->m_doorLock == DOOR_LOCK_LOCKOUT_PLAYER_ONLY);
}

void CVehicleSA::LockDoors(bool bLocked)
{
    bool bAreDoorsLocked = AreDoorsLocked();
    bool bAreDoorsUndamageable = AreDoorsUndamageable();

    if (bLocked && !bAreDoorsLocked)
    {
        if (bAreDoorsUndamageable)
            GetVehicleInterface()->m_doorLock = DOOR_LOCK_SKIP_SHUT_DOORS;
        else
            GetVehicleInterface()->m_doorLock = DOOR_LOCK_LOCKED;
    }
    else if (!bLocked && bAreDoorsLocked)
    {
        if (bAreDoorsUndamageable)
            GetVehicleInterface()->m_doorLock = DOOR_LOCK_UNLOCKED;
        else
            GetVehicleInterface()->m_doorLock = DOOR_LOCK_NOT_USED;
    }
}

void CVehicleSA::AddVehicleUpgrade(DWORD dwModelID)
{
    if (dwModelID >= 1000 && dwModelID <= 1193)
    {
        DWORD dwThis = (DWORD)m_pInterface;

        DWORD dwFunc = FUNC_CVehicle_AddVehicleUpgrade;
        _asm
        {
            mov     ecx, dwThis
            push    dwModelID
            call    dwFunc
        }
    }
}

void CVehicleSA::RemoveVehicleUpgrade(DWORD dwModelID)
{
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_RemoveVehicleUpgrade;

    _asm
    {
        mov     ecx, dwThis
        push    dwModelID
        call    dwFunc
    }

    // GTA SA only does this when CVehicle::ClearVehicleUpgradeFlags returns false.
    // In the case of hydraulics and nitro, this function does not return false and the upgrade is never removed from the array
    for (std::int16_t& upgrade : GetVehicleInterface()->m_upgrades)
    {
        if (upgrade == dwModelID)
        {
            upgrade = -1;
            break;
        }
    }
}

bool CVehicleSA::DoesSupportUpgrade(const SString& strFrameName)
{
    SVehicleFrame* pComponent = GetVehicleComponent(strFrameName);
    if (pComponent && pComponent->pFrame != NULL)
    {
        // Todo: enforce hierarchy
        return true;
    }

    return false;
}

DWORD CVehicleSA::GetBaseVehicleType()
{
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_GetBaseVehicleType;
    DWORD dwReturn = 0;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     dwReturn, eax

    }

    return dwReturn;
}

void CVehicleSA::SetBodyDirtLevel(float fDirtLevel)
{
    GetVehicleInterface()->nBodyDirtLevel = fDirtLevel;
}

float CVehicleSA::GetBodyDirtLevel()
{
    return GetVehicleInterface()->nBodyDirtLevel;
}

unsigned char CVehicleSA::GetCurrentGear()
{
    return GetVehicleInterface()->m_nCurrentGear;
}

float CVehicleSA::GetGasPedal()
{
    return GetVehicleInterface()->m_fGasPedal;
}

bool CVehicleSA::GetTowBarPos(CVector* pVector, CVehicle* pTrailer)
{
    CVehicleSAInterfaceVTBL* vehicleVTBL = (CVehicleSAInterfaceVTBL*)(m_pInterface->vtbl);
    DWORD                    dwThis = (DWORD)m_pInterface;
    DWORD                    dwFunc = vehicleVTBL->GetTowbarPos;
    bool                     bReturn = false;

    DWORD       dwTrailerInt = 0;
    CVehicleSA* pTrailerSA = dynamic_cast<CVehicleSA*>(pTrailer);
    if (pTrailerSA)
        dwTrailerInt = (DWORD)pTrailerSA->GetInterface();

    _asm
    {
        mov     ecx, dwThis
        push    dwTrailerInt
        push    1
        push    pVector
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}

bool CVehicleSA::GetTowHitchPos(CVector* pVector)
{
    CVehicleSAInterfaceVTBL* vehicleVTBL = (CVehicleSAInterfaceVTBL*)(m_pInterface->vtbl);
    DWORD                    dwThis = (DWORD)m_pInterface;
    DWORD                    dwFunc = vehicleVTBL->GetTowHitchPos;
    bool                     bReturn = false;

    _asm
    {
        mov     ecx, dwThis
        push    0
        push    1
        push    pVector
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}

bool CVehicleSA::IsUpsideDown()
{
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_IsUpsideDown;
    bool  bReturn = false;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}

void CVehicleSA::SetEngineOn(bool bEngineOn)
{
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwEngineOn = (DWORD)bEngineOn;
    DWORD dwFunc = FUNC_CVehicle_SetEngineOn;

    _asm
    {
        mov     ecx, dwThis
        push    dwEngineOn
        call    dwFunc
    }
}

CPed* CVehicleSA::GetDriver()
{
    CPoolsSA* pPools = (CPoolsSA*)pGame->GetPools();

    CPedSAInterface* pDriver = GetVehicleInterface()->pDriver;
    if (pDriver)
    {
        SClientEntity<CPedSA>* pPedClientEntity = pPools->GetPed((DWORD*)pDriver);
        if (pPedClientEntity)
        {
            return pPedClientEntity->pEntity;
        }
    }
    return nullptr;
}

CPed* CVehicleSA::GetPassenger(unsigned char ucSlot)
{
    CPoolsSA* pPools = (CPoolsSA*)pGame->GetPools();

    if (ucSlot < 8)
    {
        CPedSAInterface* pPassenger = GetVehicleInterface()->pPassengers[ucSlot];
        if (pPassenger)
        {
            SClientEntity<CPedSA>* pPedClientEntity = pPools->GetPed((DWORD*)pPassenger);
            if (pPedClientEntity)
            {
                return pPedClientEntity->pEntity;
            }
        }
    }

    return NULL;
}

bool CVehicleSA::IsBeingDriven()
{
    return GetVehicleInterface()->pDriver != NULL;
}

/**
 * \todo Implement for other vehicle types too and check
 */
void CVehicleSA::PlaceBikeOnRoadProperly()
{
    DWORD dwFunc = FUNC_Bike_PlaceOnRoadProperly;
    DWORD dwBike = (DWORD)GetInterface();

    _asm
    {
        mov     ecx, dwBike
        call    dwFunc
    }
}

void CVehicleSA::PlaceAutomobileOnRoadProperly()
{
    DWORD dwFunc = FUNC_Automobile_PlaceOnRoadProperly;
    DWORD dwAutomobile = (DWORD)GetInterface();

    _asm
    {
        mov     ecx, dwAutomobile
        call    dwFunc
    }
}

void CVehicleSA::SetColor(SharedUtil::SColor color1, SharedUtil::SColor color2, SharedUtil::SColor color3, SharedUtil::SColor color4, int)
{
    m_RGBColors[0] = color1;
    m_RGBColors[1] = color2;
    m_RGBColors[2] = color3;
    m_RGBColors[3] = color4;

    // Some colors result in black for unknown reason
    for (uint i = 0; i < NUMELMS(m_RGBColors); i++)
    {
        m_RGBColorsFixed[i] = m_RGBColors[i];
        const SharedUtil::SColor color = m_RGBColorsFixed[i];
        if (color == 0xFF00FF                // 255,   0, 255
            || color == 0x00FFFF             //   0,   0, 255
            || color == 0xFF00AF             // 255,   0, 175
            || color == 0xFFAF00             // 255, 175,   0
            || color == 0xB9FF00             // 185, 255,   0
            || color == 0x00FFC8             //   0, 255, 200
            || color == 0xFF3C00             // 255,  60,   0
            || color == 0x3CFF00)            //  60, 255,   0
            m_RGBColorsFixed[i].ulARGB |= 0x010101;
    }
}

void CVehicleSA::GetColor(SharedUtil::SColor* color1, SharedUtil::SColor* color2, SharedUtil::SColor* color3, SharedUtil::SColor* color4, bool bFixedForGTA)
{
    if (!bFixedForGTA)
    {
        *color1 = m_RGBColors[0];
        *color2 = m_RGBColors[1];
        *color3 = m_RGBColors[2];
        *color4 = m_RGBColors[3];
    }
    else
    {
        *color1 = m_RGBColorsFixed[0];
        *color2 = m_RGBColorsFixed[1];
        *color3 = m_RGBColorsFixed[2];
        *color4 = m_RGBColorsFixed[3];
    }
}

// works with firetrucks & tanks
void CVehicleSA::GetTurretRotation(float* fHorizontal, float* fVertical)
{
    // This is coded in asm because for some reason it was failing to compile
    // correctly with normal c++.
    DWORD vehicleInterface = (DWORD)GetInterface();
    float fHoriz = 0.0f;
    float fVert = 0.0f;
    _asm
    {
        mov     eax, vehicleInterface
        add     eax, 0x94C
        fld     [eax]
        fstp    fHoriz
        add     eax, 4
        fld     [eax]
        fstp    fVert
    }
    *fHorizontal = fHoriz;
    *fVertical = fVert;
}

void CVehicleSA::SetTurretRotation(float fHorizontal, float fVertical)
{
    //*(float *)(this->GetInterface() + 2380) = fHorizontal;
    //*(float *)(this->GetInterface() + 2384) = fVertical;
    DWORD vehicleInterface = (DWORD)GetInterface();
    _asm
    {
        mov     eax, vehicleInterface
        add     eax, 0x94C
        fld     fHorizontal
        fstp    [eax]
        add     eax, 4
        fld     fVertical
        fstp    [eax]
    }
}

bool CVehicleSA::IsSirenOrAlarmActive()
{
    return ((CVehicleSAInterface*)GetInterface())->m_nVehicleFlags.bSirenOrAlarm;
}

void CVehicleSA::SetSirenOrAlarmActive(bool bActive)
{
    ((CVehicleSAInterface*)GetInterface())->m_nVehicleFlags.bSirenOrAlarm = bActive;
}

float CVehicleSA::GetHealth()
{
    CVehicleSAInterface* vehicle = (CVehicleSAInterface*)GetInterface();
    return vehicle->m_nHealth;
}

void CVehicleSA::SetHealth(float fHealth)
{
    auto vehicle = static_cast<CAutomobileSAInterface*>(GetInterface());
    vehicle->m_nHealth = fHealth;
    if (fHealth >= 250.0f)
    {
        vehicle->m_fBurningTime = 0.0f;
        vehicle->m_delayedExplosionTimer = 0;
    }
}

// SHould be plane funcs

void CVehicleSA::SetLandingGearDown(bool bLandingGearDown)
{
    // This is the C code corresponding to the gta_sa
    // asm code at address 0x6CB39A.
    CVehicleSAInterface* pInterface = GetVehicleInterface();
    DWORD                dwThis = (DWORD)pInterface;
    float&               fPosition = *(float*)(dwThis + 0x9CC);
    float&               fTimeStep = *(float*)(0xB7CB5C);
    float&               flt_871904 = *(float*)(0x871904);

    if (IsLandingGearDown() != bLandingGearDown)
    {
        // The following code toggles the landing gear direction
        if (fPosition == 0.0f)
        {
            MemPutFast<DWORD>(dwThis + 0x5A5, 0x02020202);
            fPosition += (fTimeStep * flt_871904);
        }
        else
        {
            if (fPosition != 1.0f)
                fPosition *= -1.0f;
            else
                fPosition = (fTimeStep * flt_871904) - 1.0f;
        }
    }
}

float CVehicleSA::GetLandingGearPosition()
{
    DWORD dwThis = (DWORD)GetInterface();
    return *(float*)(dwThis + 2508);
}

void CVehicleSA::SetLandingGearPosition(float fPosition)
{
    DWORD dwThis = (DWORD)GetInterface();
    MemPutFast<float>(dwThis + 2508, fPosition);
}

bool CVehicleSA::IsLandingGearDown()
{
    DWORD dwThis = (DWORD)GetInterface();
    if (*(float*)(dwThis + 2508) <= 0.0f)
        return true;
    else
        return false;
}

void CVehicleSA::Fix()
{
    DWORD dwThis = (DWORD)GetInterface();

    DWORD       dwFunc = 0;
    CModelInfo* pModelInfo = pGame->GetModelInfo(GetModelIndex());
    if (pModelInfo)
    {
        if (pModelInfo->IsCar() || pModelInfo->IsMonsterTruck() || pModelInfo->IsTrailer())
            dwFunc = FUNC_CAutomobile__Fix;
        else if (pModelInfo->IsPlane())
            dwFunc = FUNC_CPlane__Fix;
        else if (pModelInfo->IsHeli())
            dwFunc = FUNC_CHeli__Fix;
        else if (pModelInfo->IsBike())
            dwFunc = FUNC_CBike_Fix;

        if (dwFunc)
        {
            _asm
            {
                mov     ecx, dwThis
                call    dwFunc
            }
        }
    }
}

CDamageManager* CVehicleSA::GetDamageManager()
{
    return m_pDamageManager;
}

void CVehicleSA::BlowUp(CEntity* pCreator, unsigned long ulUnknown)
{
    CVehicleSAInterfaceVTBL* vehicleVTBL = (CVehicleSAInterfaceVTBL*)(GetInterface()->vtbl);
    DWORD                    dwThis = (DWORD)m_pInterface;
    DWORD                    dwFunc = vehicleVTBL->BlowUpCar;

    DWORD dwCreator = (DWORD)pCreator;

    _asm
    {
        push        ulUnknown
        push        dwCreator
        mov         ecx, dwThis
        call        dwFunc
    }
}

void CVehicleSA::FadeOut(bool bFadeOut)
{
    CVehicleSAInterface* vehicle = (CVehicleSAInterface*)GetInterface();
    vehicle->bDistanceFade = bFadeOut;
    vehicle->m_nVehicleFlags.bFadeOut = bFadeOut;
}

bool CVehicleSA::IsFadingOut()
{
    CVehicleSAInterface* vehicle = (CVehicleSAInterface*)GetInterface();
    return vehicle->m_nVehicleFlags.bFadeOut;
}

void CVehicleSA::SetTowLink(CVehicle* pVehicle)
{
    // We can't use the vtable func, because it teleports the trailer parallel to the vehicle => make our own one (see #1655)

    CVehicleSA* towingVehicleSA = dynamic_cast<CVehicleSA*>(pVehicle);

    if (towingVehicleSA)
    {
        CVehicleSAInterface* trailerVehicle = GetVehicleInterface();
        CVehicleSAInterface* towingVehicle = towingVehicleSA->GetVehicleInterface();
        towingVehicle->m_trailerVehicle = trailerVehicle;
        trailerVehicle->m_towingVehicle = towingVehicle;

        // Set the trailer's status to "remote controlled"
        SetEntityStatus(eEntityStatus::STATUS_REMOTE_CONTROLLED);
    }
}

bool CVehicleSA::BreakTowLink()
{
    DWORD dwThis = (DWORD)GetInterface();

    CVehicleSAInterfaceVTBL* vehicleVTBL = (CVehicleSAInterfaceVTBL*)(GetInterface()->vtbl);
    DWORD                    dwFunc = vehicleVTBL->BreakTowLink;
    bool                     bReturn = false;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}

CVehicle* CVehicleSA::GetTowedVehicle()
{
    CVehicleSAInterface* trailerVehicle = GetVehicleInterface()->m_trailerVehicle;

    if (trailerVehicle)
    {
        SClientEntity<CVehicleSA>* pVehicleClientEntity = pGame->GetPools()->GetVehicle((DWORD*)trailerVehicle);
        return pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
    }

    return nullptr;
}

CVehicle* CVehicleSA::GetTowedByVehicle()
{
    CVehicleSAInterface* towingVehicle = GetVehicleInterface()->m_towingVehicle;

    if (towingVehicle)
    {
        SClientEntity<CVehicleSA>* pVehicleClientEntity = pGame->GetPools()->GetVehicle((DWORD*)towingVehicle);
        return pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
    }

    return nullptr;
}

void CVehicleSA::SetWinchType(eWinchType winchType)
{
    if (winchType < 4 && winchType != 2)
    {
        GetVehicleInterface()->WinchType = winchType;
    }
}

void CVehicleSA::PickupEntityWithWinch(CEntity* pEntity)
{
    CEntitySA* pEntitySA = dynamic_cast<CEntitySA*>(pEntity);

    if (pEntitySA)
    {
        DWORD dwFunc = FUNC_CVehicle_PickUpEntityWithWinch;
        DWORD dwThis = (DWORD)GetInterface();
        DWORD dwEntityInterface = (DWORD)pEntitySA->GetInterface();

        _asm
        {
            push    dwEntityInterface
            mov     ecx, dwThis
            call    dwFunc
        }
    }
}

void CVehicleSA::ReleasePickedUpEntityWithWinch()
{
    DWORD dwFunc = FUNC_CVehicle_ReleasePickedUpEntityWithWinch;
    DWORD dwThis = (DWORD)GetInterface();

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

void CVehicleSA::SetRopeHeightForHeli(float fRopeHeight)
{
    DWORD dwFunc = FUNC_CVehicle_SetRopeHeightForHeli;
    DWORD dwThis = (DWORD)GetInterface();

    _asm
    {
        push    fRopeHeight
        mov     ecx, dwThis
        call    dwFunc
    }
}

CPhysical* CVehicleSA::QueryPickedUpEntityWithWinch()
{
    DWORD dwFunc = FUNC_CVehicle_QueryPickedUpEntityWithWinch;
    DWORD dwThis = (DWORD)GetInterface();

    CPhysicalSAInterface* phys;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     phys, eax
    }

    if (phys)
    {
        CPools* pPools = pGame->GetPools();
        return reinterpret_cast<CPhysical*>(pPools->GetEntity((DWORD*)phys));
    }
    return nullptr;
}

void CVehicleSA::SetRemap(int iRemap)
{
    DWORD dwFunc = FUNC_CVehicle__SetRemap;
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    iRemap
        call    dwFunc
    }
}

int CVehicleSA::GetRemapIndex()
{
    DWORD dwFunc = FUNC_CVehicle__GetRemapIndex;
    DWORD dwThis = (DWORD)GetInterface();
    int   iReturn = 0;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     iReturn, eax
    }
    return iReturn;
}

void CVehicleSA::SetRemapTexDictionary(int iRemapTextureDictionary)
{
    DWORD dwFunc = FUNC_CVehicle__SetRemapTexDictionary;
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    iRemapTextureDictionary
        call    dwFunc
    }
}

bool CVehicleSA::IsSmokeTrailEnabled()
{
    return (*(unsigned char*)((DWORD)GetInterface() + 2560) == 1);
}

void CVehicleSA::SetSmokeTrailEnabled(bool bEnabled)
{
    MemPutFast<unsigned char>((DWORD)GetInterface() + 2560, (bEnabled) ? 1 : 0);
}

CHandlingEntry* CVehicleSA::GetHandlingData()
{
    return m_pHandlingData;
}

CFlyingHandlingEntry* CVehicleSA::GetFlyingHandlingData()
{
    return m_pFlyingHandlingData;
}

void CVehicleSA::SetHandlingData(CHandlingEntry* pHandling)
{
    // Store the handling and recalculate it
    m_pHandlingData = static_cast<CHandlingEntrySA*>(pHandling);
    GetVehicleInterface()->pHandlingData = m_pHandlingData->GetInterface();
    RecalculateHandling();
}

void CVehicleSA::SetFlyingHandlingData(CFlyingHandlingEntry* pFlyingHandling)
{
    if (!pFlyingHandling)
        return;
    m_pFlyingHandlingData = static_cast<CFlyingHandlingEntrySA*>(pFlyingHandling);
    GetVehicleInterface()->pFlyingHandlingData = m_pFlyingHandlingData->GetInterface();
}

void CVehicleSA::RecalculateHandling()
{
    if (!m_pHandlingData)
        return;

    m_pHandlingData->Recalculate();

    // Recalculate the suspension lines
    RecalculateSuspensionLines();

    // Put it in our interface
    CVehicleSAInterface* pInt = GetVehicleInterface();
    unsigned int         uiHandlingFlags = m_pHandlingData->GetInterface()->uiHandlingFlags;
    bool                 hydralicsInstalled = false, nitroInstalled = false;

    // We check whether the user has not set incorrect flags via handlingFlags in the case of nitro and hydraulics
    // If this happened, we need to correct it
    for (const std::int16_t& upgradeID : pInt->m_upgrades)
    {
        // Empty upgrades value is -1
        if (upgradeID < 0)
            continue;

        // If NOS is installed we need set the flag
        if ((upgradeID >= 1008 && upgradeID <= 1010))
        {
            if (!(uiHandlingFlags & HANDLING_NOS_Flag))
                uiHandlingFlags |= HANDLING_NOS_Flag;

            nitroInstalled = true;
        }

        // If hydraulics is installed we need set the flag
        if ((upgradeID == 1087))
        {
            if (!(uiHandlingFlags & HANDLING_Hydraulics_Flag))
                uiHandlingFlags |= HANDLING_Hydraulics_Flag;

            hydralicsInstalled = true;
        }
    }

    // If hydraulics isn't installed we need unset the flag
    if ((!hydralicsInstalled) && (uiHandlingFlags & HANDLING_Hydraulics_Flag))
        uiHandlingFlags &= ~HANDLING_Hydraulics_Flag;

    // If NOS isn't installed we need unset the flag
    if ((!nitroInstalled) && (uiHandlingFlags & HANDLING_NOS_Flag))
        uiHandlingFlags &= ~HANDLING_NOS_Flag;

    m_pHandlingData->SetHandlingFlags(uiHandlingFlags);

    pInt->dwHandlingFlags = uiHandlingFlags;
    pInt->m_fMass = m_pHandlingData->GetInterface()->fMass;
    pInt->m_fTurnMass = m_pHandlingData->GetInterface()->fTurnMass;            // * pGame->GetHandlingManager()->GetTurnMassMultiplier();
    pInt->m_vecCenterOfMass = m_pHandlingData->GetInterface()->vecCenterOfMass;
    pInt->m_fBuoyancyConstant = m_pHandlingData->GetInterface()->fUnknown2;
    /*if (m_pHandlingData->GetInterface()->fDragCoeff >= pGame->GetHandlingManager()->GetBasicDragCoeff())
        GetVehicleInterface ()->fDragCoeff = pGame->GetHandlingManager()->GetBasicDragCoeff();
    else*/
    // pInt->fDragCoeff = m_pHandlingData->GetInterface()->fDragCoeff / 1000 * pGame->GetHandlingManager()->GetDragMultiplier();
}

void CVehicleSA::BurstTyre(BYTE bTyre)
{
    CVehicleSAInterfaceVTBL* vehicleVTBL = (CVehicleSAInterfaceVTBL*)(m_pInterface->vtbl);
    DWORD                    dwThis = (DWORD)m_pInterface;
    DWORD                    dwFunc = vehicleVTBL->BurstTyre;
    _asm
    {
        mov         ecx, dwThis
        push        1 // not used
        push        bTyre
        call        dwFunc
    }
}

BYTE CVehicleSA::GetBikeWheelStatus(BYTE bWheel)
{
    if (bWheel == 0)
        return *(BYTE*)((DWORD)GetInterface() + 0x65C);
    if (bWheel == 1)
        return *(BYTE*)((DWORD)GetInterface() + 0x65D);
    return 0;
}

void CVehicleSA::SetBikeWheelStatus(BYTE bWheel, BYTE bStatus)
{
    if (bWheel == 0)
        *(BYTE*)((DWORD)GetInterface() + 0x65C) = bStatus;
    else if (bWheel == 1)
        *(BYTE*)((DWORD)GetInterface() + 0x65D) = bStatus;
}

bool CVehicleSA::IsWheelCollided(BYTE eWheelPosition)
{
    auto vehicle = static_cast<CAutomobileSAInterface*>(GetInterface());
    switch ((VehicleClass)vehicle->m_vehicleClass)
    {
        case VehicleClass::AUTOMOBILE:
            if (eWheelPosition < 4)
                return vehicle->m_wheelCollisionState[eWheelPosition] == 4.f;
            break;
        case VehicleClass::BIKE:
            if (eWheelPosition < 2)
                return *(float*)((DWORD)vehicle + 0x730 + eWheelPosition * 8) == 4.f || *(float*)((DWORD)vehicle + 0x734 + eWheelPosition * 8) == 4.f;
            break;
    }
    return false;
}

int CVehicleSA::GetWheelFrictionState(BYTE eWheelPosition)
{
    auto vehicle = static_cast<CAutomobileSAInterface*>(GetInterface());
    return vehicle->m_wheelFrictionState[eWheelPosition];
}

void CVehicleSA::SetTaxiLightOn(bool bLightOn)
{
    DWORD dwThis = (DWORD)GetInterface();
    DWORD dwState = (DWORD)bLightOn;
    DWORD dwFunc = FUNC_CAutomobile_SetTaxiLight;

    _asm
    {
        mov     ecx, dwThis
        push    dwState
        call    dwFunc
    }
}

void GetMatrixForGravity(const CVector& vecGravity, CMatrix& mat)
{
    // Calculates a basis where the z axis is the inverse of the gravity
    if (vecGravity.Length() > 0.0001f)
    {
        mat.vUp = -vecGravity;
        mat.vUp.Normalize();
        if (fabs(mat.vUp.fX) > 0.0001f || fabs(mat.vUp.fZ) > 0.0001f)
        {
            CVector y(0.0f, 1.0f, 0.0f);
            mat.vFront = vecGravity;
            mat.vFront.CrossProduct(&y);
            mat.vFront.CrossProduct(&vecGravity);
            mat.vFront.Normalize();
        }
        else
        {
            mat.vFront = CVector(0.0f, 0.0f, vecGravity.fY);
        }
        mat.vRight = mat.vFront;
        mat.vRight.CrossProduct(&mat.vUp);
    }
    else
    {
        // No gravity, use default axes
        mat.vRight = CVector(1.0f, 0.0f, 0.0f);
        mat.vFront = CVector(0.0f, 1.0f, 0.0f);
        mat.vUp = CVector(0.0f, 0.0f, 1.0f);
    }
}

void CVehicleSA::SetGravity(const CVector* pvecGravity)
{
    if (pGame->GetPools()->GetPedFromRef(1)->GetVehicle() == this)
    {
        // If this is the local player's vehicle, adjust the camera's position history.
        // This is to keep the automatic camera settling (which happens when driving while not moving the mouse)
        // nice and consistent while the gravity changes.
        CCam* pCam = pGame->GetCamera()->GetCam(pGame->GetCamera()->GetActiveCam());

        CMatrix matOld, matNew;
        GetMatrixForGravity(m_vecGravity, matOld);
        GetMatrixForGravity(*pvecGravity, matNew);

        CVector* pvecPosition = &m_pInterface->Placeable.matrix->vPos;

        matOld.Invert();
        pCam->GetTargetHistoryPos()[0] = matOld * (pCam->GetTargetHistoryPos()[0] - *pvecPosition);
        pCam->GetTargetHistoryPos()[0] = matNew * pCam->GetTargetHistoryPos()[0] + *pvecPosition;

        pCam->GetTargetHistoryPos()[1] = matOld * (pCam->GetTargetHistoryPos()[1] - *pvecPosition);
        pCam->GetTargetHistoryPos()[1] = matNew * pCam->GetTargetHistoryPos()[1] + *pvecPosition;
    }

    m_vecGravity = *pvecGravity;
}

bool CVehicleSA::SpawnFlyingComponent(const eCarNodes& nodeIndex, const eCarComponentCollisionTypes& collisionType, std::int32_t removalTime)
{
    if (nodeIndex == eCarNodes::NONE)
        return false;

    DWORD nodesOffset = OFFSET_CAutomobile_Nodes;
    RwFrame* defaultBikeChassisFrame = nullptr;

    // CBike, CBmx, CBoat and CTrain don't inherit CAutomobile so let's do it manually!
    switch (static_cast<VehicleClass>(GetVehicleInterface()->m_vehicleClass))
    {
        case VehicleClass::AUTOMOBILE:
        case VehicleClass::MONSTER_TRUCK:
        case VehicleClass::PLANE:
        case VehicleClass::HELI:
        case VehicleClass::TRAILER:
        case VehicleClass::QUAD:
        {
            nodesOffset = OFFSET_CAutomobile_Nodes;
            break;
        }
        case VehicleClass::TRAIN:
        {
            if (static_cast<eTrainNodes>(nodeIndex) >= eTrainNodes::NUM_NODES)
                return false;

            nodesOffset = OFFSET_CTrain_Nodes;
            break;
        }
        case VehicleClass::BIKE:
        case VehicleClass::BMX:
        {
            auto* bikeInterface = static_cast<CBikeSAInterface*>(GetVehicleInterface());
            if (!bikeInterface)
                return false;

            if (static_cast<eBikeNodes>(nodeIndex) >= eBikeNodes::NUM_NODES)
                return false;

            nodesOffset = OFFSET_CBike_Nodes;
            if (static_cast<eBikeNodes>(nodeIndex) != eBikeNodes::CHASSIS)
                break;

            // Set the correct "bike_chassis" frame for bikes
            defaultBikeChassisFrame = bikeInterface->m_apModelNodes[1];
            if (defaultBikeChassisFrame && std::strcmp(defaultBikeChassisFrame->szName, "chassis_dummy") == 0)
            {
                RwFrame* correctChassisFrame = RwFrameFindFrame(RpGetFrame(bikeInterface->m_pRwObject), "chassis");
                if (correctChassisFrame)
                    bikeInterface->m_apModelNodes[1] = correctChassisFrame;
            }
            break;
        }
        case VehicleClass::BOAT:
        {
            if (static_cast<eBoatNodes>(nodeIndex) >= eBoatNodes::NUM_NODES)
                return false;

            nodesOffset = OFFSET_CBoat_Nodes;
            break;
        }
        default:
            return false;
    }

    // Patch nodes array in CAutomobile::SpawnFlyingComponent
    MemPut(0x6A85B3, nodesOffset);
    MemPut(0x6A8631, nodesOffset);

    auto* componentObject = ((CObjectSAInterface * (__thiscall*)(CVehicleSAInterface*, int, int)) FUNC_CAutomobile__SpawnFlyingComponent)(GetVehicleInterface(), static_cast<int>(nodeIndex), static_cast<int>(collisionType));

    // Restore default nodes array in CAutomobile::SpawnFlyingComponent
    // CAutomobile::m_aCarNodes offset
    MemPut(0x6A85B3, 0x648);
    MemPut(0x6A8631, 0x648);

    // Restore default chassis frame for bikes
    if (static_cast<eBikeNodes>(nodeIndex) == eBikeNodes::CHASSIS && defaultBikeChassisFrame)
    {
        auto* bikeInterface = static_cast<CBikeSAInterface*>(GetVehicleInterface());
        if (bikeInterface && bikeInterface->m_apModelNodes)
            bikeInterface->m_apModelNodes[1] = defaultBikeChassisFrame;
    }

    if (removalTime <= -1 || !componentObject)
        return true;

    // Set double-sided
    componentObject->bBackfaceCulled = true;

    componentObject->uiObjectRemovalTime = pGame->GetSystemTime() + static_cast<std::uint32_t>(removalTime);
    return true;
}

void CVehicleSA::SetWheelVisibility(eWheelPosition wheel, bool bVisible)
{
    auto     vehicle = static_cast<CAutomobileSAInterface*>(GetInterface());
    RwFrame* pFrame = NULL;
    switch (wheel)
    {
        case FRONT_LEFT_WHEEL:
            pFrame = vehicle->m_aCarNodes[static_cast<std::size_t>(eCarNodes::WHEEL_LF)];
            break;
        case REAR_LEFT_WHEEL:
            pFrame = vehicle->m_aCarNodes[static_cast<std::size_t>(eCarNodes::WHEEL_LB)];
            break;
        case FRONT_RIGHT_WHEEL:
            pFrame = vehicle->m_aCarNodes[static_cast<std::size_t>(eCarNodes::WHEEL_RF)];
            break;
        case REAR_RIGHT_WHEEL:
            pFrame = vehicle->m_aCarNodes[static_cast<std::size_t>(eCarNodes::WHEEL_RB)];
            break;
        default:
            break;
    }

    if (pFrame)
    {
        DWORD     dw_GetCurrentAtomicObjectCB = 0x6a0750;
        RwObject* pObject = NULL;

        // Stop GetCurrentAtomicObjectCB from returning null for 'invisible' objects
        MemPutFast<BYTE>(0x6A0758, 0x90);
        MemPutFast<BYTE>(0x6A0759, 0x90);
        RwFrameForAllObjects(pFrame, (void*)dw_GetCurrentAtomicObjectCB, &pObject);
        MemPutFast<BYTE>(0x6A0758, 0x74);
        MemPutFast<BYTE>(0x6A0759, 0x06);

        if (pObject)
            pObject->flags = (bVisible) ? 4 : 0;
    }
}

CVector CVehicleSA::GetWheelPosition(eWheelPosition wheel)
{
    auto pInterface = static_cast<CAutomobileSAInterface*>(GetVehicleInterface());
    switch (wheel)
    {
        case FRONT_LEFT_WHEEL:
            return pInterface->m_wheelColPoint[FRONT_LEFT_WHEEL].Position;
        case FRONT_RIGHT_WHEEL:
            return pInterface->m_wheelColPoint[FRONT_RIGHT_WHEEL].Position;
        case REAR_LEFT_WHEEL:
            return pInterface->m_wheelColPoint[REAR_LEFT_WHEEL].Position;
        case REAR_RIGHT_WHEEL:
            return pInterface->m_wheelColPoint[REAR_RIGHT_WHEEL].Position;
    }
    return CVector();
}

bool CVehicleSA::IsHeliSearchLightVisible()
{
    // See CHeli::PreRender
    DWORD dwThis = (DWORD)GetInterface();
    return *(bool*)(dwThis + 2577);
}

void CVehicleSA::SetHeliSearchLightVisible(bool bVisible)
{
    // See CHeli::PreRender
    DWORD dwThis = (DWORD)GetInterface();
    MemPutFast<bool>(dwThis + 2577, bVisible);
}

CColModel* CVehicleSA::GetSpecialColModel()
{
    CVehicleSAInterface* vehicle = (CVehicleSAInterface*)GetInterface();
    if (vehicle->m_nSpecialColModel != 0xFF)
    {
        CColModelSAInterface* pSpecialColModels = (CColModelSAInterface*)VAR_CVehicle_SpecialColModels;
        CColModelSAInterface* pColModelInterface = &pSpecialColModels[vehicle->m_nSpecialColModel];
        if (pColModelInterface)
        {
            CColModel* pColModel = new CColModelSA(pColModelInterface);
            return pColModel;
        }
    }
    return NULL;
}

bool CVehicleSA::UpdateMovingCollision(float fAngle)
{
    // If we dont have a driver, use the local player for this function
    // It will check a few key-states which shouldn't make any difference as we've specified an angle.
    CVehicleSAInterface* vehicle = (CVehicleSAInterface*)GetInterface();
    CPedSAInterface*     pDriver = vehicle->pDriver;
    if (!pDriver)
    {
        CPed* pLocalPed = pGame->GetPools()->GetPedFromRef(1);
        if (pLocalPed)
            vehicle->pDriver = (CPedSAInterface*)pLocalPed->GetInterface();
    }

    bool  bReturn;
    DWORD dwThis = (DWORD)GetInterface();
    DWORD dwFunc = FUNC_CAutomobile__UpdateMovingCollision;
    _asm
    {
        mov     ecx, dwThis
        push    fAngle
        call    dwFunc
        mov     bReturn, al
    }

    // Restore our driver
    vehicle->pDriver = pDriver;

    return bReturn;
}

void* CVehicleSA::GetPrivateSuspensionLines()
{
    if (m_pSuspensionLines == NULL)
    {
        CModelInfo* pModelInfo = pGame->GetModelInfo(GetModelIndex());
        CColDataSA* pColData = pModelInfo->GetInterface()->pColModel->m_data;
        if (pModelInfo->IsMonsterTruck())
        {
            // Monster truck suspension data is 0x90 BYTES rather than 0x80 (some extra stuff I guess)
            m_pSuspensionLines = new BYTE[0x90];
        }
        else if (pModelInfo->IsBike())
        {
            // Bike Suspension data is 0x80 BYTES rather than 0x40 (Some extra stuff I guess)
            m_pSuspensionLines = new BYTE[0x80];
        }
        else
        {
            // CAutomobile allocates wheels * 32 (0x20)
            m_pSuspensionLines = new BYTE[pColData->m_numSuspensionLines * 0x20];
        }
    }

    return m_pSuspensionLines;
}

void CVehicleSA::CopyGlobalSuspensionLinesToPrivate()
{
    CModelInfo* pModelInfo = pGame->GetModelInfo(GetModelIndex());
    CColDataSA* pColData = pModelInfo->GetInterface()->pColModel->m_data;
    if (pModelInfo->IsMonsterTruck())
    {
        // Monster trucks are 0x90 bytes not 0x80
        if (pColData->m_suspensionLines)
            memcpy(GetPrivateSuspensionLines(), pColData->m_suspensionLines, 0x90);
    }
    else if (pModelInfo->IsBike())
    {
        // Bikes are 0x80 bytes not 0x40
        if (pColData->m_suspensionLines)
            memcpy(GetPrivateSuspensionLines(), pColData->m_suspensionLines, 0x80);
    }
    else
    {
        // CAutomobile allocates wheels * 32 (0x20)
        if (pColData->m_suspensionLines)
            memcpy(GetPrivateSuspensionLines(), pColData->m_suspensionLines, pColData->m_numSuspensionLines * 0x20);
    }
}

void CVehicleSA::RecalculateSuspensionLines()
{
    CHandlingEntry* pHandlingEntry = GetHandlingData();

    DWORD       dwModel = GetModelIndex();
    CModelInfo* pModelInfo = pGame->GetModelInfo(dwModel);
    if (pModelInfo && pModelInfo->IsMonsterTruck() || pModelInfo->IsCar())
    {
        CVehicleSAInterface* pInt = GetVehicleInterface();
        // Trains (Their trailers do as well!)
        if (pModelInfo->IsTrain() || dwModel == 571 || dwModel == 570 || dwModel == 569 || dwModel == 590)
            return;

        CVehicleSAInterfaceVTBL* pVtbl = reinterpret_cast<CVehicleSAInterfaceVTBL*>(pInt->vtbl);
        DWORD                    dwSetupSuspensionLines = pVtbl->SetupSuspensionLines;
        DWORD                    dwThis = (DWORD)pInt;
        _asm
        {
            mov ecx, dwThis
            call dwSetupSuspensionLines
        }

        CopyGlobalSuspensionLinesToPrivate();
    }
}

void CVehicleSA::GiveVehicleSirens(unsigned char ucSirenType, unsigned char ucSirenCount)
{
    m_tSirenInfo.m_bOverrideSirens = true;
    m_tSirenInfo.m_ucSirenType = ucSirenType;
    m_tSirenInfo.m_ucSirenCount = ucSirenCount;
}

void CVehicleSA::SetVehicleSirenPosition(unsigned char ucSirenID, CVector vecPos)
{
    m_tSirenInfo.m_tSirenInfo[ucSirenID].m_vecSirenPositions = vecPos;
}

void CVehicleSA::GetVehicleSirenPosition(unsigned char ucSirenID, CVector& vecPos)
{
    vecPos = m_tSirenInfo.m_tSirenInfo[ucSirenID].m_vecSirenPositions;
}

void CVehicleSA::SetVehicleFlags(bool bEnable360, bool bEnableRandomiser, bool bEnableLOSCheck, bool bEnableSilent)
{
    m_tSirenInfo.m_b360Flag = bEnable360;
    m_tSirenInfo.m_bDoLOSCheck = bEnableLOSCheck;
    m_tSirenInfo.m_bUseRandomiser = bEnableRandomiser;
    m_tSirenInfo.m_bSirenSilent = bEnableSilent;
}

void CVehicleSA::OnChangingPosition(const CVector& vecNewPosition)
{
    // Only apply to CAutomobile and down
    if (GetBaseVehicleType() == 0)
    {
        CVector vecDelta = vecNewPosition - m_pInterface->Placeable.matrix->vPos;
        if (vecDelta.LengthSquared() > 10 * 10)
        {
            // Reposition colpoints for big moves to avoid random spinning
            auto pInterface = static_cast<CAutomobileSAInterface*>(GetVehicleInterface());
            pInterface->m_wheelColPoint[FRONT_LEFT_WHEEL].Position += vecDelta;
            pInterface->m_wheelColPoint[REAR_LEFT_WHEEL].Position += vecDelta;
            pInterface->m_wheelColPoint[FRONT_RIGHT_WHEEL].Position += vecDelta;
            pInterface->m_wheelColPoint[REAR_RIGHT_WHEEL].Position += vecDelta;
        }
    }
}

void CVehicleSA::StaticSetHooks()
{
    // Setup vehicle sun glare hook
    HookInstall(FUNC_CAutomobile_OnVehiclePreRender, (DWORD)HOOK_Vehicle_PreRender, 5);
}

void CVehicleSA::SetVehiclesSunGlareEnabled(bool bEnabled)
{
    m_bVehicleSunGlare = bEnabled;
}

bool CVehicleSA::GetVehiclesSunGlareEnabled()
{
    return m_bVehicleSunGlare;
}

namespace
{
    void _MatrixConvertFromEulerAngles(CMatrix_Padded* matrixPadded, float fX, float fY, float fZ)
    {
        int iUnknown = 0;
        if (matrixPadded)
        {
            DWORD dwFunc = FUNC_CMatrix__ConvertFromEulerAngles;
            _asm
            {
                push    iUnknown
                push    fZ
                push    fY
                push    fX
                mov     ecx, matrixPadded
                call    dwFunc
            }
        }
    }
    void _MatrixConvertToEulerAngles(CMatrix_Padded* matrixPadded, float& fX, float& fY, float& fZ)
    {
        int iUnknown = 0;
        if (matrixPadded)
        {
            DWORD dwFunc = FUNC_CMatrix__ConvertToEulerAngles;
            _asm
            {
                push    iUnknown
                push    fZ
                push    fY
                push    fX
                mov     ecx, matrixPadded
                call    dwFunc
            }
        }
    }
}            // namespace

SVehicleFrame* CVehicleSA::GetVehicleComponent(const SString& vehicleComponent)
{
    return MapFind(m_ExtraFrames, vehicleComponent);
}

bool CVehicleSA::SetComponentRotation(const SString& vehicleComponent, const CVector& vecRotation)
{
    SVehicleFrame* pComponent = GetVehicleComponent(vehicleComponent);
    if (pComponent && pComponent->pFrame != NULL)
    {
        pGame->GetRenderWareSA()->RwMatrixSetRotation(pComponent->pFrame->modelling, vecRotation);
        return true;
    }
    return false;
}

bool CVehicleSA::GetComponentRotation(const SString& vehicleComponent, CVector& vecRotation)
{
    SVehicleFrame* pComponent = GetVehicleComponent(vehicleComponent);
    if (pComponent && pComponent->pFrame != NULL)
    {
        pGame->GetRenderWareSA()->RwMatrixGetRotation(pComponent->pFrame->modelling, vecRotation);
        return true;
    }
    return false;
}

bool CVehicleSA::SetComponentPosition(const SString& vehicleComponent, const CVector& vecPosition)
{
    SVehicleFrame* pComponent = GetVehicleComponent(vehicleComponent);
    if (pComponent && pComponent->pFrame != NULL)
    {
        pGame->GetRenderWareSA()->RwMatrixSetPosition(pComponent->pFrame->modelling, vecPosition);
        return true;
    }
    return false;
}

bool CVehicleSA::GetComponentPosition(const SString& vehicleComponent, CVector& vecPositionModelling)
{
    SVehicleFrame* pComponent = GetVehicleComponent(vehicleComponent);
    if (pComponent && pComponent->pFrame != NULL)
    {
        pGame->GetRenderWareSA()->RwMatrixGetPosition(pComponent->pFrame->modelling, vecPositionModelling);
        return true;
    }
    return false;
}

bool CVehicleSA::SetComponentScale(const SString& vehicleComponent, const CVector& vecScale)
{
    SVehicleFrame* pComponent = GetVehicleComponent(vehicleComponent);
    if (pComponent && pComponent->pFrame != NULL)
    {
        pGame->GetRenderWareSA()->RwMatrixSetScale(pComponent->pFrame->modelling, vecScale);
        return true;
    }
    return false;
}

bool CVehicleSA::GetComponentScale(const SString& vehicleComponent, CVector& vecScaleModelling)
{
    SVehicleFrame* pComponent = GetVehicleComponent(vehicleComponent);
    if (pComponent && pComponent->pFrame != NULL)
    {
        pGame->GetRenderWareSA()->RwMatrixGetScale(pComponent->pFrame->modelling, vecScaleModelling);
        return true;
    }
    return false;
}

bool CVehicleSA::IsComponentPresent(const SString& vehicleComponent)
{
    return GetVehicleComponent(vehicleComponent) != NULL;
}

bool CVehicleSA::GetComponentMatrix(const SString& vehicleComponent, CMatrix& matOutOrientation)
{
    SVehicleFrame* pComponent = GetVehicleComponent(vehicleComponent);
    // Check validty
    if (pComponent && pComponent->pFrame != NULL)
    {
        pGame->GetRenderWareSA()->RwMatrixToCMatrix(pComponent->pFrame->modelling, matOutOrientation);
        return true;
    }
    return false;
}

bool CVehicleSA::SetComponentMatrix(const SString& vehicleComponent, const CMatrix& matOrientation)
{
    SVehicleFrame* pComponent = GetVehicleComponent(vehicleComponent);
    // Check validty
    if (pComponent && pComponent->pFrame != NULL)
    {
        pGame->GetRenderWareSA()->CMatrixToRwMatrix(matOrientation, pComponent->pFrame->modelling);
        return true;
    }
    return false;
}

// Get transform from component parent to the model root
bool CVehicleSA::GetComponentParentToRootMatrix(const SString& vehicleComponent, CMatrix& matOutParentToRoot)
{
    SVehicleFrame* pComponent = GetVehicleComponent(vehicleComponent);
    // Check validty
    if (pComponent && pComponent->pFrame != NULL)
    {
        // Calc root to parent transform
        CMatrix matCombo;
        for (uint i = 0; i < pComponent->frameList.size(); i++)
        {
            RwFrame* pFrame = pComponent->frameList[i];
            if (pFrame)
            {
                CMatrix matFrame;
                pGame->GetRenderWareSA()->RwMatrixToCMatrix(pFrame->modelling, matFrame);
                matCombo = matFrame * matCombo;
            }
        }
        matOutParentToRoot = matCombo;
        return true;
    }
    return false;
}

void CVehicleSA::AddComponent(RwFrame* pFrame, bool bReadOnly)
{
    // if the frame is invalid we don't want to be here
    if (!pFrame)
        return;

    // if the frame already exists ignore it
    if (IsComponentPresent(pFrame->szName) || pFrame->szName == "")
        return;

    SString strName = pFrame->szName;
    // variants have no name field.
    if (strName == "")
    {
        // In MTA variant 255 means no variant
        if ((m_ucVariantCount == 0 && m_ucVariant == 255) || (m_ucVariantCount == 1 && m_ucVariant2 == 255))
            return;

        // name starts with extra
        strName = "extra_";

        // variants are extra_a - extra_f
        strName += 'a' + (m_ucVariantCount == 0 ? m_ucVariant : m_ucVariant2);

        // increment the variant count ( we assume that the first variant created is variant1 and the second is variant2 )
        m_ucVariantCount++;
    }

    // insert our new frame
    SVehicleFrame frame = SVehicleFrame(pFrame, bReadOnly);
    m_ExtraFrames.insert(std::pair<SString, SVehicleFrame>(strName, frame));
}

void CVehicleSA::FinalizeFramesList()
{
    // For each frame, make list of parent frames
    std::map<SString, SVehicleFrame>::iterator iter = m_ExtraFrames.begin();
    for (; iter != m_ExtraFrames.end(); ++iter)
    {
        SVehicleFrame& vehicleFrame = iter->second;
        dassert(vehicleFrame.frameList.empty());
        vehicleFrame.frameList.clear();

        // Get frame list from parent component down to the root
        RwFrame* pParent = (RwFrame*)vehicleFrame.pFrame->object.parent;
        for (; pParent && pParent != pParent->root; pParent = (RwFrame*)pParent->object.parent)
        {
            // Get parent frame by name from our list instead of the RwFrame structure
            SVehicleFrame* parentVehicleFrame = GetVehicleComponent(pParent->szName);
            if (parentVehicleFrame && parentVehicleFrame->pFrame != NULL)
            {
                vehicleFrame.frameList.insert(vehicleFrame.frameList.begin(), parentVehicleFrame->pFrame);
            }
        }
    }
}

void CVehicleSA::DumpVehicleFrames()
{
    // clear our rw frames list
    m_ExtraFrames.clear();

    // dump the frames
    VehicleDump(this);
    FinalizeFramesList();
}

bool CVehicleSA::SetComponentVisible(const SString& vehicleComponent, bool bRequestVisible)
{
    SVehicleFrame* pComponent = GetVehicleComponent(vehicleComponent);
    // Check validty
    if (pComponent && pComponent->pFrame != NULL && pComponent->bReadOnly == false)
    {
        RwFrame* pFrame = pComponent->pFrame;
        // Get all atomics for this component - Usually one, or two if there is a damaged version
        std::vector<RwObject*> atomicList;
        GetAllAtomicObjects(pFrame, atomicList);

        // Count number currently visible
        uint uiNumAtomicsCurrentlyVisible = 0;
        for (uint i = 0; i < atomicList.size(); i++)
            if (atomicList[i]->flags & 0x04)
                uiNumAtomicsCurrentlyVisible++;

        if (bRequestVisible && uiNumAtomicsCurrentlyVisible == 0)
        {
            // Make atomic (undamaged version) visible. TODO - Check if damaged version should be made visible instead
            for (uint i = 0; i < atomicList.size(); i++)
            {
                RwObject* pAtomic = atomicList[i];
                int       AtomicId = pGame->GetVisibilityPlugins()->GetAtomicId(pAtomic);

                if (!(AtomicId & ATOMIC_ID_FLAG_TWO_VERSIONS_DAMAGED))
                {
                    // Either only one version, or two versions and this is the undamaged one
                    pAtomic->flags |= 0x04;
                }
            }
        }
        else if (!bRequestVisible && uiNumAtomicsCurrentlyVisible > 0)
        {
            // Make all atomics invisible
            for (uint i = 0; i < atomicList.size(); i++)
                atomicList[i]->flags &= ~0x05;            // Mimic what GTA seems to do - Not sure what the bottom bit is for
        }
        return true;
    }
    return false;
}

bool CVehicleSA::GetComponentVisible(const SString& vehicleComponent, bool& bOutVisible)
{
    SVehicleFrame* pComponent = GetVehicleComponent(vehicleComponent);
    // Check validty
    if (pComponent && pComponent->pFrame != NULL && pComponent->bReadOnly == false)
    {
        RwFrame* pFrame = pComponent->pFrame;
        // Get all atomics for this component - Usually one, or two if there is a damaged version
        std::vector<RwObject*> atomicList;
        GetAllAtomicObjects(pFrame, atomicList);

        // Count number currently visible
        uint uiNumAtomicsCurrentlyVisible = 0;
        for (uint i = 0; i < atomicList.size(); i++)
            if (atomicList[i]->flags & 0x04)
                uiNumAtomicsCurrentlyVisible++;

        // Set result
        bOutVisible = (uiNumAtomicsCurrentlyVisible > 0);
        return true;
    }
    return false;
}

void CVehicleSA::SetNitroLevel(float fLevel)
{
    DWORD dwThis = (DWORD)GetInterface();
    MemPutFast<float>(dwThis + 0x8A4, fLevel);
}

float CVehicleSA::GetNitroLevel()
{
    DWORD dwThis = (DWORD)GetInterface();
    float fLevel = *(float*)(dwThis + 0x8A4);
    return fLevel;
}

// The following function is a reproduction of a part of
// CPlane::ProcessControlInputs (006CADD0). This allows
// the usage of setVehicleLandingGearDown on non-occupied
// planes. (Issue 0007608)
void CVehicleSA::UpdateLandingGearPosition()
{
    DWORD                dwThis = (DWORD)GetInterface();
    CVehicleSAInterface* pVehicle = (CVehicleSAInterface*)(dwThis);
    float&               fGearPosition = *(float*)(dwThis + 0x9CC);
    const float&         fTimeStep = *(float*)(0xB7CB5C);

    // Guide to fGearPosition
    //  1.0f = Landing gear is pulled in
    //  0.0f = Landing gear is pulled out
    // -1.0f to 0.0f -> Landing gear is being pulled out
    //  0.0f to 1.0f -> Landing gear is being pulled in

    if (fGearPosition < 0.0f)
    {
        fGearPosition = fTimeStep * 0.02f + fGearPosition;

        if (fGearPosition >= 0.0f)
        {
            // Set the position to 0.0f
            fGearPosition = 0.0f;

            // Remove Wheels
            m_pDamageManager->SetWheelStatus(FRONT_LEFT_WHEEL, 0);
            m_pDamageManager->SetWheelStatus(FRONT_RIGHT_WHEEL, 0);
            m_pDamageManager->SetWheelStatus(REAR_LEFT_WHEEL, 0);
            m_pDamageManager->SetWheelStatus(REAR_RIGHT_WHEEL, 0);

            // Update Air Resistance
            float fDragCoeff = GetHandlingData()->GetDragCoeff();
            pVehicle->m_fAirResistance = fDragCoeff / 1000.0f * 0.5f;
        }
    }
    else if (fGearPosition > 0.0f && fGearPosition < 1.0f)
    {
        // Pull in
        fGearPosition = fTimeStep * 0.02f + fGearPosition;
        if (fGearPosition >= 1.0f)
        {
            // Set the position to 1.0f
            fGearPosition = 1.0f;

            // C++ Representaion of CPlane::SetLandingGearDown (006CAC20)

            // Recreate Wheels
            m_pDamageManager->SetWheelStatus(FRONT_LEFT_WHEEL, 2);
            m_pDamageManager->SetWheelStatus(FRONT_RIGHT_WHEEL, 2);
            m_pDamageManager->SetWheelStatus(REAR_LEFT_WHEEL, 2);
            m_pDamageManager->SetWheelStatus(REAR_RIGHT_WHEEL, 2);

            // Update Air Resistance
            float fDragCoeff = GetHandlingData()->GetDragCoeff();

            const float& fFlyingHandlingGearUpR = pVehicle->pFlyingHandlingData->fGearUpR;
            pVehicle->m_fAirResistance = fDragCoeff / 1000.0f * 0.5f * fFlyingHandlingGearUpR;
        }
    }
}

bool CVehicleSA::GetDummyPosition(eVehicleDummies dummy, CVector& position) const
{
    if (dummy >= 0 && dummy < VEHICLE_DUMMY_COUNT)
    {
        position = m_dummyPositions[dummy];
        return true;
    }

    return false;
}

bool CVehicleSA::SetDummyPosition(eVehicleDummies dummy, const CVector& position)
{
    if (dummy < 0 || dummy >= VEHICLE_DUMMY_COUNT)
        return false;

    auto vehicle = reinterpret_cast<CVehicleSAInterface*>(m_pInterface);

    m_dummyPositions[dummy] = position;

    if (dummy == ENGINE)
    {
        if (vehicle->m_overheatParticle != nullptr)
            CFxSystemSA::SetPosition(vehicle->m_overheatParticle, position);

        if (vehicle->m_fireParticle != nullptr)
            CFxSystemSA::SetPosition(vehicle->m_fireParticle, position);
    }

    bool isAutomobileClass = static_cast<VehicleClass>(vehicle->m_vehicleClass) == VehicleClass::AUTOMOBILE;

    if (isAutomobileClass)
    {
        SetAutomobileDummyPosition(reinterpret_cast<CAutomobileSAInterface*>(m_pInterface), dummy, position);
    }

    return true;
}

//
// NOTE(botder): Move the code to CAutomobileSA::SetDummyPosition, when we start using CAutomobileSA
//
void CVehicleSA::SetAutomobileDummyPosition(CAutomobileSAInterface* automobile, eVehicleDummies dummy, const CVector& position)
{
    if (dummy == EXHAUST)
    {
        if (automobile->pNitroParticle[0] != nullptr)
            CFxSystemSA::SetPosition(automobile->pNitroParticle[0], position);
    }
    else if (dummy == EXHAUST_SECONDARY)
    {
        if (automobile->pNitroParticle[1] != nullptr)
            CFxSystemSA::SetPosition(automobile->pNitroParticle[1], position);
    }
}

// Change plate text of existing vehicle
bool CVehicleSA::SetPlateText(const SString& strText)
{
    CModelInfo* pModelInfo = pGame->GetModelInfo(GetModelIndex());
    if (!pModelInfo)
        return false;
    CVehicleModelInfoSAInterface* pVehicleModelInfo = (CVehicleModelInfoSAInterface*)pModelInfo->GetInterface();

    // Copy text
    strncpy(pVehicleModelInfo->plateText, *strText, 8);

    // Check if changeable
    if (!pVehicleModelInfo->pPlateMaterial)
        return false;

    // Release texture ref, if was custom before
    RwTexture*& pOldTexture = GetVehicleInterface()->m_pCustomPlateTexture;
    if (pOldTexture && pOldTexture != pVehicleModelInfo->pPlateMaterial->texture)
    {
        RwTextureDestroy(pOldTexture);
        pOldTexture = NULL;
    }

    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_CustomCarPlate_TextureCreate;
    bool  bReturn = false;
    _asm
    {
        mov     ecx, dwThis
        push    pVehicleModelInfo
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

bool CVehicleSA::SetWindowOpenFlagState(unsigned char ucWindow, bool bState)
{
    if (ucWindow > 6)
        return false;
    unsigned char ucWindows[7] = {0, 1, 8, 9, 10, 11, 18};
    ucWindow = ucWindows[ucWindow];

    DWORD dwThis = (DWORD)GetVehicleInterface();
    DWORD dwFunc;
    if (bState)
    {
        dwFunc = FUNC_CVehicle_SetWindowOpenFlag;
    }
    else
    {
        dwFunc = FUNC_CVehicle_ClearWindowOpenFlag;
    }
    bool bReturn = false;

    _asm
    {
        mov     ecx, dwThis
        push    ucWindow
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}
