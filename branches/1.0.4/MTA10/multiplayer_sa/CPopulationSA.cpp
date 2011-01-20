/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/CPopulationSA.cpp
*  PURPOSE:     Ped world population class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

CPedSAInterface     * pPedStorage;
CPopulationSA       * pSingleton;
DWORD               pedVtable;

VOID HOOK_EndOf_CPopulation__Add();
VOID HOOK_CPopulation__RemovePed();

CivilianAddHandler      * m_pCivilianAddHandler;
CivilianRemoveHandler   * m_pCivilianRemoveHandler;

CPopulationSA::CPopulationSA()
{
    dwPedCount = 0;
    HookInstall(HOOKPOS_EndOf_CPopulation__Add, (DWORD)HOOK_EndOf_CPopulation__Add, 6);
    HookInstall(HOOKPOS_CPopulation__RemovePed, (DWORD)HOOK_CPopulation__RemovePed, 6);
    pSingleton = this;
    m_pCivilianRemoveHandler = NULL;
    m_pCivilianAddHandler = NULL;
}

VOID CPopulationSA::AddPed ( CCivilianPed * ped )
{
    CCivilianPedSA* pPedSA = dynamic_cast < CCivilianPedSA* > ( ped );
    if ( !pPedSA ) return;

    CEntitySAInterface* pPedSAInterface = pPedSA->GetInterface ();

    list < CCivilianPedSA* > ::iterator iter;
    for ( iter = peds.begin (); iter != peds.end (); iter++ )
    {
        if ( (*iter)->GetInterface() == pPedSAInterface )
        {
            return;
        }
    }

    peds.push_back ( pPedSA );
    dwPedCount ++;
}

VOID CPopulationSA::AddPed ( CPedSAInterface * ped )
{ 
    list < CCivilianPedSA* > ::iterator iter;
    for ( iter = peds.begin (); iter != peds.end (); iter++ )
    {
        if ( (*iter)->GetInterface() == ped )
        {
            return;
        }
    }

    //_asm int 3
    CCivilianPedSA * pedSA = dynamic_cast < CCivilianPedSA* > ( pGameInterface->GetPools()->AddCivilianPed((DWORD *)ped ) );
    if ( !pedSA ) return;

    char szDebug[255] = {'\0'};
    DWORD dwPedInterface = (DWORD)pedSA->GetInterface();
    sprintf ( szDebug, "Civ ped added (%d) (0x%X -> 0x%X)\n", dwPedCount+1, ped, dwPedInterface);
    //OutputDebugString ( szDebug );

    if ( m_pCivilianAddHandler )
        m_pCivilianAddHandler ( pedSA );

    peds.push_back (pedSA);
    dwPedCount ++;
}

VOID CPopulationSA::RemovePed ( CCivilianPed * ped )
{
    if ( !ped ) return;

    CCivilianPedSA* pPedSA = dynamic_cast < CCivilianPedSA* > ( ped );
    if ( !pPedSA ) return;

    ped->SetDoNotRemoveFromGameWhenDeleted ( true );
    pGameInterface->GetPools()->RemovePed ( (CPed*)ped );
    if ( !peds.empty () ) peds.remove ( pPedSA );
    dwPedCount--;
}

VOID CPopulationSA::RemovePed ( CPedSAInterface * ped )
{
    list < CCivilianPedSA* > ::iterator iter;
    for ( iter = peds.begin (); iter != peds.end (); iter++ )
    {
        if ( (*iter)->GetInterface() == ped )
        {
            char szDebug[255] = {'\0'};
            sprintf ( szDebug, "Civ ped removed (%d)\n", dwPedCount - 1);
            pGameInterface->GetPools()->RemovePed ( (CPed *)(CCivilianPed *)(*iter), false );
            //OutputDebugString ( szDebug );
            
            if ( m_pCivilianRemoveHandler )
                m_pCivilianRemoveHandler ( (*iter) );

            peds.erase ( iter );
            dwPedCount--;
            return;
        }
    }
    //OutputDebugString ( "Tried to remove Civ Ped, but Civ Ped not found!\n" );
}

DWORD CPopulationSA::GetPedCount ( )
{
    return dwPedCount;
}

CCivilianPed * CPopulationSA::GetFirstPed ()
{
    if ( peds.size () > 0 )
    {
        pedIter = peds.begin ();
        return *peds.begin ();
    }
    else
    {
        return NULL;
    }
}

CCivilianPed * CPopulationSA::GetNextPed ()
{
    ( pedIter )++;
    if ( pedIter != peds.end () )
    {
        return *pedIter;
    }
    else
    {
        return NULL;
    }
}

void CPopulationSA::SetCivilianAddHandler ( CivilianAddHandler * pCivilianAddHandler )
{
    m_pCivilianAddHandler = pCivilianAddHandler;
}

void CPopulationSA::SetCivilianRemoveHandler ( CivilianRemoveHandler * pCivilianRemoveHandler )
{
    m_pCivilianRemoveHandler = pCivilianRemoveHandler;
}

VOID _declspec(naked) HOOK_EndOf_CPopulation__Add()
{
    _asm
    {
        mov     pPedStorage, eax
        pushad
    }

    FUNCTION_PROLOG
    pSingleton->AddPed ( pPedStorage );
    FUNCTION_EPILOG

    _asm
    {
        popad
        add     esp, 0x3C
        retn
    }
}

VOID _declspec(naked) HOOK_CPopulation__RemovePed()
{
    /*
    00610F20  /$ 56             PUSH ESI
    00610F21  |. 8B7424 08      MOV ESI,DWORD PTR SS:[ESP+8]
    00610F25  |. 56             PUSH ESI
    */

    _asm
    {
        
        push    esi
        mov     esi, [esp+8]
        push    esi
        mov     pPedStorage, esi
        mov     ecx, [esi]
        mov     pedVtable, ecx
        pushad
    }

    if ( pedVtable == VTBL_CPlayerPed )
    {
        _asm
        {
            popad
            pop     esi
            pop     esi
            retn
        }
    }

    FUNCTION_PROLOG
    pSingleton->RemovePed(pPedStorage);
    FUNCTION_EPILOG

    _asm
    {
        popad
        mov     ecx, HOOKPOS_CPopulation__RemovePed
        add     ecx, 6
        jmp     ecx
    }
}
