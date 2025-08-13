/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CTrainSA.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CTrainSA.h"

CTrainSA::CTrainSA(CTrainSAInterface* pInterface)
{
    SetInterface(pInterface);
    Init();
}
