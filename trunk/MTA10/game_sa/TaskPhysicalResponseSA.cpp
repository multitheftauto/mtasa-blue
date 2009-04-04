/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/TaskPhysicalResponseSA.cpp
*  PURPOSE:		Physical response game tasks
*  DEVELOPERS:	Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

// ##############################################################################
// ## Name:    CTaskSimpleChoking                                  
// ## Purpose: Make the ped choke
// ##############################################################################

CTaskSimpleChokingSA::CTaskSimpleChokingSA ( CPed* pAttacker, bool bIsTearGas )
{
    CPedSA* pAttackerSA = dynamic_cast < CPedSA* > ( pAttacker );

    DWORD dwFunc = FUNC_CTaskSimpleChoking__Constructor;
    DWORD dwIsTearGas = bIsTearGas;

    // Grab the GTA class for the attacker if any
    CPedSAInterface* pAttackerInterface = NULL;
    if ( pAttackerSA ) pAttackerInterface = pAttackerSA->GetPedInterface ();

    this->CreateTaskInterface ( sizeof ( CTaskSimpleChokingSAInterface ) );
    DWORD dwThisInterface = (DWORD)this->GetInterface();
    _asm
    {
        mov		ecx, dwThisInterface
        push    ebx
        push    bIsTearGas
        push    pAttackerInterface
        call    dwFunc
        pop     ebx
    }
}


CPed* CTaskSimpleChokingSA::GetAttacker ( void )
{
    CTaskSimpleChokingSAInterface * internalInterface = (CTaskSimpleChokingSAInterface*)this->GetInterface();
    return pGame->GetPools ()->GetPed ( (DWORD*) internalInterface->m_pAttacker );
}


unsigned int CTaskSimpleChokingSA::GetTimeRemaining ( void )
{
    CTaskSimpleChokingSAInterface * internalInterface = (CTaskSimpleChokingSAInterface*)this->GetInterface();
    return internalInterface->m_nTimeRemaining;
}


unsigned int CTaskSimpleChokingSA::GetTimeStarted ( void )
{
    CTaskSimpleChokingSAInterface * internalInterface = (CTaskSimpleChokingSAInterface*)this->GetInterface();
    return internalInterface->m_nTimeStarted;
}


bool CTaskSimpleChokingSA::IsTeargas ( void )
{
    CTaskSimpleChokingSAInterface * internalInterface = (CTaskSimpleChokingSAInterface*)this->GetInterface();
    return internalInterface->m_bIsTeargas ? true : false;
}


bool CTaskSimpleChokingSA::IsFinished ( void )
{
    CTaskSimpleChokingSAInterface * internalInterface = (CTaskSimpleChokingSAInterface*)this->GetInterface();
    return internalInterface->m_bIsFinished;
}


void CTaskSimpleChokingSA::UpdateChoke ( CPed* pPed, CPed* pAttacker, bool bIsTearGas )
{
    // Get game interfaces
    CPedSA* pPedSA = dynamic_cast < CPedSA* > ( pPed );
	if ( !pPedSA ) return;

    CPedSAInterface* pPedInterface = pPedSA->GetPedInterface ();

    CPedSAInterface* pAttackerInterface = NULL;
    if ( pAttacker )
    {
        CPedSA* pAttackerSA = dynamic_cast < CPedSA* > ( pAttacker );
		if ( pAttackerSA )
			pAttackerInterface = pAttackerSA->GetPedInterface ();
    }

    // Call the func
    DWORD dwThisInterface = (DWORD)this->GetInterface();
    DWORD dwFunc = FUNC_CTaskSimpleChoking__UpdateChoke;
    _asm
    {
        mov         ecx, dwThisInterface
        push        bIsTearGas
        push        pAttackerInterface
        push        pPedInterface
        call        dwFunc
    }
}

