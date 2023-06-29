/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CPlaneSA.cpp
 *  PURPOSE:     Plane vehicle entity
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlaneSA.h"

CPlaneSA::CPlaneSA(CPlaneSAInterface* pInterface)
{
    SetInterface(pInterface);
    Init();
}
