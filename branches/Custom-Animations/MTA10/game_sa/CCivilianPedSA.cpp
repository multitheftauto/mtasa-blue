/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CCivilianPedSA.cpp
*  PURPOSE:     Civilian ped entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

/**
 * Constructor for CCivilianPedSA
 */
CCivilianPedSA::CCivilianPedSA( ePedModel pedType )
{
    DEBUG_TRACE("CCivilianPedSA::CCivilianPedSA( ePedModel pedType )");
    // based on CCivilianPed::SetupPlayerPed (R*)
    DWORD CPedOperatorNew = FUNC_CPedOperatorNew;
    DWORD CCivilianPedConstructor = FUNC_CCivilianPedConstructor;

    DWORD dwPedPointer = 0;
    _asm
    {
        push    SIZEOF_CCIVILIANPED
        call    CPedOperatorNew
        add     esp, 4

        mov     dwPedPointer, eax

        mov     ecx, eax
        push    0 // set to 0 and they'll behave like AI peds
        push    1 // ped type
        call    CCivilianPedConstructor
    }
    this->SetInterface((CEntitySAInterface *)dwPedPointer);
    this->Init(); // init our interfaces 
    CPoolsSA * pools = (CPoolsSA *)pGame->GetPools();
    this->internalID =  pools->GetPedRef ( (DWORD *)this->GetInterface () );
    CWorldSA * world = (CWorldSA *)pGame->GetWorld();
    world->Add(this->GetInterface());
    this->SetModelIndex(pedType);
    this->BeingDeleted = FALSE;
    this->DoNotRemoveFromGame = FALSE;
    this->SetType ( CIVILIAN_PED );

    this->m_pInterface->bStreamingDontDelete = true;
    this->m_pInterface->bDontStream = true;
}

CCivilianPedSA::~CCivilianPedSA( )
{
    DEBUG_TRACE("CCivilianPedSA::~CCivilianPedSA( )");
    if(!this->BeingDeleted && DoNotRemoveFromGame == false)
    {
        DWORD dwInterface = (DWORD)this->GetInterface();
        
        if ( (DWORD)this->GetInterface()->vtbl != VTBL_CPlaceable )
        {
            CWorldSA * world = (CWorldSA *)pGame->GetWorld();
            world->Remove(this->GetInterface());
        
            DWORD dwThis = (DWORD)this->GetInterface();
            DWORD dwFunc = this->GetInterface()->vtbl->SCALAR_DELETING_DESTRUCTOR; // we use the vtbl so we can be type independent
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
}

CCivilianPedSA::CCivilianPedSA( CPedSAInterface * ped )
{
    DEBUG_TRACE("CCivilianPedSA::CCivilianPedSA( CPedSAInterface * ped )");

    this->SetInterface((CEntitySAInterface *)ped);
    this->Init();
    this->SetType ( CIVILIAN_PED );
}

/**
 * Gets information on the player's wanted status
 * @return Pointer to a CWanted class containing the wanted information for the PlayerPed.
 */
//CWanted   * CCivilianPedSA::GetWanted (  )
//{
    //return internalInterface->Wanted;
//}

/**
 * Gets the current weapon type that the playerped is using
 * @return DWORD containing the current weapon type
 * \todo Check this is the weapon type, not the actual weapon ID (or whatever)
 */
/*DWORD CCivilianPedSA::GetCurrentWeaponType (  )
{
    return internalInterface->CurrentWeapon;
}*/

/** 
 * Gets the time the last shot was fired by the playerped
 * @return DWORD containing a system time value
 */
/*DWORD CCivilianPedSA::GetLastShotTime (  )
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
BOOL CCivilianPedSA::IsStationaryOnFoot (  )
{
    return internalInterface->StationaryOnFoot;
}*/

/**
 * Resets the played ped to their initial state
 * \note This also resets some global values, such as game speed
 */
/*
VOID CCivilianPedSA::ResetToInitialState (  )
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
VOID CCivilianPedSA::ClearWeaponTarget (  )
{
    DWORD dwFunction = FUNC_ClearWeaponTarget;
    DWORD dwThis = (DWORD)internalInterface;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunction
    }
}*/
