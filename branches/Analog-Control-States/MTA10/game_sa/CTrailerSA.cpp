/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTrailerSA.cpp
*  PURPOSE:     Trailer vehicle entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CTrailerSA::CTrailerSA( CTrailerSAInterface * trailer ):CAutomobileSA( trailer )
{
    DEBUG_TRACE("CTrailerSA::CTrailerSA( CTrailerSAInterface * trailer )");
    this->m_pInterface = trailer;
}

CTrailerSA::CTrailerSA( eVehicleTypes dwModelID, unsigned char ucVariation, unsigned char ucVariation2 ):CAutomobileSA( dwModelID, ucVariation, ucVariation2 )
{
    DEBUG_TRACE("CTrailerSA::CTrailerSA( eVehicleTypes dwModelID ):CVehicleSA( dwModelID )");
}
