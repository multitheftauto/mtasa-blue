/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CEventListSA.cpp
 *  PURPOSE:     Event list
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CEventListSA.h"
#include "CEventDamageSA.h"

CEventDamage* CEventListSA::GetEventDamage(CEventDamageSAInterface* pInterface)
{
    return new CEventDamageSA(pInterface);
}
