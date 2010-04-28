/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPlaneSA.cpp
*  PURPOSE:     Plane vehicle entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPlaneSA::CPlaneSA( CPlaneSAInterface * plane ):CAutomobileSA( plane )
{
    DEBUG_TRACE("CPlaneSA::CPlaneSA( CPlaneSAInterface * plane )");
    this->m_pInterface = plane;
}

CPlaneSA::CPlaneSA( eVehicleTypes dwModelID ):CAutomobileSA( dwModelID )
{
    DEBUG_TRACE("CPlaneSA::CPlaneSA( eVehicleTypes dwModelID ):CVehicleSA( dwModelID )");
}
