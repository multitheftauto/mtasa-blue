/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPlayerPedSA.cpp
*  PURPOSE:     Player ped entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Alberto Alonso <rydencillo@gmail.com>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               aru <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

/**
 * Constructor for CPlayerPedSA
 */

static CPedClothesDesc* pLocalClothes = 0;
static CWantedSAInterface* pLocalWanted = 0;

CPlayerPedSA::CPlayerPedSA( ePedModel pedType )
{
    DEBUG_TRACE("CPlayerPedSA::CPlayerPedSA( ePedModel pedType )");
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

    this->SetInterface((CEntitySAInterface *)dwPedPointer);

    this->Init(); // init our interfaces 
    CPoolsSA * pools = (CPoolsSA *)pGame->GetPools ( );
    this->internalID =  pools->GetPedRef ( (DWORD *)this->GetInterface () );
    CWorldSA * world = (CWorldSA *)pGame->GetWorld();
    
    this->SetModelIndex(pedType);
    this->BeingDeleted = FALSE;
    this->DoNotRemoveFromGame = FALSE;
    this->SetType ( PLAYER_PED );

    // Allocate a player data struct and set it as the players
    m_bIsLocal = false;
    m_pData = new CPlayerPedDataSAInterface;

    // Copy the local player data so we're defaulted to something good
    CPlayerPedSA* pLocalPlayerSA = dynamic_cast < CPlayerPedSA* > ( pools->GetPedFromRef ( (DWORD)1 ) );
    if ( pLocalPlayerSA )
        memcpy ( m_pData, ((CPlayerPedSAInterface*)pLocalPlayerSA->GetInterface ())->pPlayerData, sizeof ( CPlayerPedDataSAInterface ) );

    // Replace the player ped data in our ped interface with the one we just created
    GetPlayerPedInterface ()->pPlayerData = m_pData;

    // Set default stuff
    m_pData->m_bRenderWeapon = true;
    m_pData->m_Wanted = pLocalWanted;
    m_pData->m_fSprintEnergy = 1000.0f;

    // Clothes pointers or we'll crash later (TODO: Wrap up with some cloth classes and make it unique per player)
    m_pData->m_pClothes = pLocalClothes;

    // Not sure why was this here (svn blame reports that this line came from the old SVN),
    // but it's causing a bug in what the just streamed-in players that are in the air are
    // processed as if they would be standing on some surface, screwing velocity calculations
    // for players floating in air (using superman script, for example) because GTA:SA will
    // try to apply the floor friction to their velocity.
    //SetIsStanding ( true );

    GetPlayerPedInterface ()->pedFlags.bCanBeShotInVehicle = true;
    GetPlayerPedInterface ()->pedFlags.bTestForShotInVehicle = true;
    // Stop remote players targeting eachother, this also stops the local player targeting them (needs to be fixed)
    GetPlayerPedInterface ()->pedFlags.bNeverEverTargetThisPed = true;
    GetPlayerPedInterface ()->pedFlags.bIsLanding = false;
    GetPlayerPedInterface ()->fRotationSpeed = 7.5;
    m_pInterface->bStreamingDontDelete = true;
    m_pInterface->bDontStream = true;
    world->Add ( m_pInterface );
}


CPlayerPedSA::CPlayerPedSA ( CPlayerPedSAInterface * pPlayer )
{
    DEBUG_TRACE("CPlayerPedSA::CPlayerPedSA( CPedSAInterface * ped )");
    // based on CPlayerPed::SetupPlayerPed (R*)
    this->SetInterface((CEntitySAInterface *)pPlayer);

    this->Init();
    CPoolsSA * pools = (CPoolsSA *)pGame->GetPools();
    this->internalID =  pools->GetPedRef ( (DWORD *)this->GetInterface () );
    this->SetType ( PLAYER_PED );

    m_bIsLocal = true;
    m_pData = GetPlayerPedInterface ()->pPlayerData;
    m_pWanted = NULL;

    GetPlayerPedInterface ()->pedFlags.bCanBeShotInVehicle = true;
    GetPlayerPedInterface ()->pedFlags.bTestForShotInVehicle = true;    
    GetPlayerPedInterface ()->pedFlags.bIsLanding = false;
    GetPlayerPedInterface ()->fRotationSpeed = 7.5;


    pLocalClothes = m_pData->m_pClothes;
    pLocalWanted = m_pData->m_Wanted;

    GetPlayerPedInterface ()->pedFlags.bCanBeShotInVehicle = true;
    // Something resets this, constantly
    GetPlayerPedInterface ()->pedFlags.bTestForShotInVehicle = true;
    // Stop remote players targeting the local (need to stop them targeting eachother too)
    GetPlayerPedInterface ()->pedFlags.bNeverEverTargetThisPed = true;
}


CPlayerPedSA::~CPlayerPedSA ( void )
{
    DEBUG_TRACE("CPlayerPedSA::~CPlayerPedSA( )");
    if(!this->BeingDeleted && DoNotRemoveFromGame == false)
    {
        DWORD dwInterface = (DWORD) m_pInterface;
        
        if ( (DWORD)this->GetInterface()->vtbl != VTBL_CPlaceable )
        {
            CWorldSA * world = (CWorldSA *)pGame->GetWorld();
            world->Remove ( m_pInterface );
        
            DWORD dwThis = (DWORD) m_pInterface;
            DWORD dwFunc = m_pInterface->vtbl->SCALAR_DELETING_DESTRUCTOR; // we use the vtbl so we can be type independent
            _asm    
            {
                mov     ecx, dwThis
                push    1           //delete too
                call    dwFunc
            }
        }
        this->BeingDeleted = true;
        ((CPoolsSA *)pGame->GetPools())->RemovePed((CPed *)(CPedSA *)this, false);
    }

    // Delete the player data
    if ( !m_bIsLocal )
    {
        delete m_pData;
    }
}


CWanted* CPlayerPedSA::GetWanted ( void )
{
    return m_pWanted;
}


float CPlayerPedSA::GetSprintEnergy ( void )
{
    /*
    OutputDebugString("GetSprintEnergy HACK\n");

    m_pData->bCanBeDamaged = true;
    m_pData->m_bRenderWeapon = true;
    m_pData->m_bDontAllowWeaponChange = true;

    ((CPedSAInterface*)GetInterface())->pedFlags.bUpdateAnimHeading = true;
    ((CPedSAInterface*)GetInterface())->pedFlags.bHeadStuckInCollision = true;
    ((CPedSAInterface*)GetInterface())->pedFlags.bDonePositionOutOfCollision = true;
    ((CPedSAInterface*)GetInterface())->pedFlags.bIsRestoringGun = true;

    RebuildPlayer ();
    */

    return m_pData->m_fSprintEnergy;
}


void CPlayerPedSA::SetSprintEnergy ( float fSprintEnergy )
{
    m_pData->m_fSprintEnergy = fSprintEnergy;
}


void CPlayerPedSA::SetInitialState ( void )
{
    DWORD dwUnknown = 1;
    DWORD dwFunction = FUNC_SetInitialState;
    DWORD dwThis = (DWORD) m_pInterface;
    _asm
    {
        push    dwUnknown
        mov     ecx, dwThis
        call    dwFunction
    }

    // Avoid direction locks for respawning after a jump
    GetPlayerPedInterface ()->pedFlags.bIsLanding = false;
    // Avoid direction locks for respawning after a sprint stop
    GetPlayerPedInterface ()->fRotationSpeed = 7.5;
    // This seems to also be causing some movement / direction locks
    GetPlayerPedInterface()->pedFlags.bStayInSamePlace = false;
}

eMoveAnim CPlayerPedSA::GetMoveAnim ( void )
{
    CPedSAInterface *pedInterface = ( CPedSAInterface * ) this->GetInterface();
    return (eMoveAnim)pedInterface->iMoveAnimGroup;
}

void CPlayerPedSA::SetMoveAnim ( eMoveAnim iAnimGroup )
{
    // Set the the new move animation group
    CPedSAInterface *pedInterface = ( CPedSAInterface * ) this->GetInterface();
    pedInterface->iMoveAnimGroup = (int)iAnimGroup;

    DWORD dwThis = ( DWORD ) pedInterface;
    DWORD dwFunc = FUNC_CPlayerPed_ReApplyMoveAnims;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

/**
 * Gets information on the player's wanted status
 * @return Pointer to a CWanted class containing the wanted information for the PlayerPed.
 */
//CWanted   * CPlayerPedSA::GetWanted (  )
//{
    //return internalInterface->Wanted;
//}

/**
 * Gets the current weapon type that the playerped is using
 * @return DWORD containing the current weapon type
 * \todo Check this is the weapon type, not the actual weapon ID (or whatever)
 */
/*DWORD CPlayerPedSA::GetCurrentWeaponType (  )
{
    return internalInterface->CurrentWeapon;
}*/

/** 
 * Gets the time the last shot was fired by the playerped
 * @return DWORD containing a system time value
 */
/*DWORD CPlayerPedSA::GetLastShotTime (  )
{
    return internalInterface->LastShotTime;
}
*/
/**
 * Checks if the player is stationary on foot
 * \todo Does this duplicate?
 * @return BOOL TRUE if the player is stationary on foot, FALSE otherwise
 */
/*
BOOL CPlayerPedSA::IsStationaryOnFoot (  )
{
    return internalInterface->StationaryOnFoot;
}*/

/**
 * Resets the played ped to their initial state
 * \note This also resets some global values, such as game speed
 */
/*
VOID CPlayerPedSA::ResetToInitialState (  )
{
    DWORD dwFunction = FUNC_SetInitialState;
    DWORD dwThis = (DWORD)internalInterface;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunction
    }
}*/

/**
 * Clears information related to the player targeting
 */
/*
VOID CPlayerPedSA::ClearWeaponTarget (  )
{
    DWORD dwFunction = FUNC_ClearWeaponTarget;
    DWORD dwThis = (DWORD)internalInterface;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunction
    }
}*/
