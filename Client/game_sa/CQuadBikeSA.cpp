/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CQuadBikeSA.cpp
 *  PURPOSE:     Quad bike vehicle entity
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CQuadBikeSA.h"

CQuadBikeSA::CQuadBikeSA(CQuadBikeSAInterface* pInterface)
{
    SetInterface(pInterface);
    Init();
}
