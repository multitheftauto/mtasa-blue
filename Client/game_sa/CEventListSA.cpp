/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CEventListSA.cpp
 *  PURPOSE:     Event list
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CEventListSA.h"
#include "CEventDamageSA.h"

CEventDamage* CEventListSA::GetEventDamage(CEventDamageSAInterface* pInterface)
{
    return new CEventDamageSA(pInterface);
}
