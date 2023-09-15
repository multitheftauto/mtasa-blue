/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CBmxSA.cpp
 *  PURPOSE:     Bmx bike vehicle entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CBmxSA.h"

CBmxSA::CBmxSA(CBmxSAInterface* pInterface)
{
    SetInterface(pInterface);
    Init();
}
