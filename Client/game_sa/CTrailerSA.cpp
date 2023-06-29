/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CTrailerSA.cpp
 *  PURPOSE:     Trailer vehicle entity
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CTrailerSA.h"

CTrailerSA::CTrailerSA(CTrailerSAInterface* pInterface)
{
    SetInterface(pInterface);
    Init();
}
