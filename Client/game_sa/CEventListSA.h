/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CEventListSA.h
 *  PURPOSE:     Header file for event list class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CEventList.h>

class CEventListSA : public CEventList
{
public:
    CEventDamage* GetEventDamage(CEventDamageSAInterface* pInterface);
};
