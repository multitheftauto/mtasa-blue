/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CQuadBikeSA.cpp
*  PURPOSE:     Quad bike vehicle entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CQuadBikeSA::CQuadBikeSA( CQuadBikeSAInterface * quadbike ):CAutomobileSA( quadbike )
{
	DEBUG_TRACE("CQuadBikeSA::CQuadBikeSA( CQuadBikeSAInterface * quadbike )");
	this->m_pInterface = quadbike;
}

CQuadBikeSA::CQuadBikeSA( eVehicleTypes dwModelID ):CAutomobileSA( dwModelID )
{
	DEBUG_TRACE("CQuadBikeSA::CQuadBikeSA( eVehicleTypes dwModelID ):CVehicleSA( dwModelID )");
}