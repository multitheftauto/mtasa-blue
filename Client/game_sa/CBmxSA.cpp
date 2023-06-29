/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CBmxSA.cpp
 *  PURPOSE:     Bmx bike vehicle entity
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CBmxSA.h"

CBmxSA::CBmxSA(CBmxSAInterface* pInterface)
{
    SetInterface(pInterface);
    Init();
}
