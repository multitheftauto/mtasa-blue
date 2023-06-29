/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CQuadBikeSA.cpp
 *  PURPOSE:     Quad bike vehicle entity
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CQuadBikeSA.h"

CQuadBikeSA::CQuadBikeSA(CQuadBikeSAInterface* pInterface)
{
    SetInterface(pInterface);
    Init();
}
