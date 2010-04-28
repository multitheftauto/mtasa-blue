/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CCarEnterExitSA.cpp
*  PURPOSE:     Car enter and exit handler
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

bool CCarEnterExitSA::GetNearestCarDoor ( CPed * pPed, CVehicle * pVehicle, CVector * pVector, int * pDoor )
{
    DWORD dwFunc = FUNC_GetNearestCarDoor;
    bool bReturn = false;

    CPedSA* pPedSA = dynamic_cast < CPedSA* > ( pPed );
    CVehicleSA* pVehicleSA = dynamic_cast < CVehicleSA* > ( pVehicle );

    if ( pPedSA && pVehicleSA )
    {
        CPedSAInterface * pPedInterface = pPedSA->GetPedInterface ();
        CVehicleSAInterface * pVehicleInterface = pVehicleSA->GetVehicleInterface ();
        _asm
        {
            push    pDoor
            push    pVector
            push    pVehicleInterface
            push    pPedInterface
            call    dwFunc
            add     esp, 0x10
            mov     bReturn, al
        }
    }

    return bReturn;
}

bool CCarEnterExitSA::GetNearestCarPassengerDoor ( CPed * pPed, CVehicle * pVehicle, CVector * pVector, int * pDoor, bool bUnknown, bool bUnknown2, bool bCheckIfRoomToGetIn )
{
    DWORD dwFunc = FUNC_GetNearestCarPassengerDoor;
    bool bReturn = false;

    CPedSA* pPedSA = dynamic_cast < CPedSA* > ( pPed );
    CVehicleSA* pVehicleSA = dynamic_cast < CVehicleSA* > ( pVehicle );

    if ( pPedSA && pVehicleSA )
    {
        CPedSAInterface * pPedInterface = pPedSA->GetPedInterface ();
        CVehicleSAInterface * pVehicleInterface = pVehicleSA->GetVehicleInterface();
        _asm
        {
            push    ebx
            xor     ebx, ebx
            mov     bl, bCheckIfRoomToGetIn
            push    ebx
            mov     bl, bUnknown2
            push    ebx
            mov     bl, bUnknown
            push    ebx
            push    pDoor 
            push    pVector
            push    pVehicleInterface
            push    pPedInterface
            call    dwFunc
            add     esp, 0x1C
            mov     bReturn, al
            pop     ebx
        }
    }

    return bReturn;
}
