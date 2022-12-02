/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPlaneSA.cpp
 *  PURPOSE:     Plane vehicle entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlaneSA.h"

CPlaneSA::CPlaneSA(CPlaneSAInterface* plane) : CAutomobileSA(plane)
{
    m_pInterface = plane;
}

CPlaneSA::CPlaneSA(eVehicleTypes dwModelID, unsigned char ucVariation, unsigned char ucVariation2) : CAutomobileSA(dwModelID, ucVariation, ucVariation2)
{
}
