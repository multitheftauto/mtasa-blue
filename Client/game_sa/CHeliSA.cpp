/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CHeliSA.cpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CHeliSA.h"

CHeliSA::CHeliSA(CHeliSAInterface* pInterface)
{
    SetInterface(pInterface);
    Init();
}
