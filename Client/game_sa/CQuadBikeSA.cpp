/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CQuadBikeSA.cpp
 *  PURPOSE:     Quad bike vehicle entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CQuadBikeSA.h"

CQuadBikeSA::CQuadBikeSA(CQuadBikeSAInterface* quadbike) : CAutomobileSA(quadbike)
{
    m_pInterface = quadbike;
}

CQuadBikeSA::CQuadBikeSA(eVehicleTypes dwModelID, unsigned char ucVariation, unsigned char ucVariation2) : CAutomobileSA(dwModelID, ucVariation, ucVariation2)
{
}
