/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CEventList.h
 *  PURPOSE:     Event list interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CEventDamage;
class CEventDamageSAInterface;

class CEventList
{
public:
    virtual CEventDamage* GetEventDamage(CEventDamageSAInterface* pInterface) = 0;
};
