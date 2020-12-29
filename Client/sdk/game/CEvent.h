/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CEvent.h
 *  PURPOSE:     Game event interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CEventSAInterface;

class CEvent
{
public:
    virtual CEventSAInterface* GetInterface() = 0;
};
