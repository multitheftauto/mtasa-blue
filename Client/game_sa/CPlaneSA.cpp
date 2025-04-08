/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPlaneSA.cpp
 *  PURPOSE:     Plane vehicle entity
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlaneSA.h"

CPlaneSA::CPlaneSA(CPlaneSAInterface* pInterface)
{
    SetInterface(pInterface);
    Init();
}
