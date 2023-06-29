/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CEventListSA.h
 *  PURPOSE:     Header file for event list class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CEventList.h>

class CEventListSA : public CEventList
{
public:
    CEventDamage* GetEventDamage(CEventDamageSAInterface* pInterface);
};
