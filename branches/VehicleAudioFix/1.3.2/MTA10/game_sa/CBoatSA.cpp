/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CBoatSA.cpp
*  PURPOSE:     Boat vehicle entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CBoatSA::CBoatSA( CBoatSAInterface * boat )
{
    DEBUG_TRACE("CBoatSA::CBoatSA( CBoatSAInterface * boat )");
    this->m_pInterface = boat;
}

CBoatSA::CBoatSA( eVehicleTypes dwModelID, unsigned char ucVariation, unsigned char ucVariation2 ):CVehicleSA( dwModelID, ucVariation, ucVariation2 )
{
    DEBUG_TRACE("CBoatSA::CBoatSA( eVehicleTypes dwModelID ):CVehicleSA( dwModelID )");
    /*if(this->internalInterface)
    {
        // create the actual vehicle
        DWORD dwFunc = FUNC_CBoatContructor;
        DWORD dwThis = (DWORD)this->internalInterface;
        _asm
        {
            mov     ecx, dwThis
            push    MISSION_VEHICLE
            push    dwModelID
            call    dwFunc
        }

        this->SetEntityStatus(STATUS_ABANDONED); // so it actually shows up in the world

        pGame->GetWorld()->Add((CEntitySA *)this);
    }   */
}
