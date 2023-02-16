/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/core/CDiscordInterface.h
 *  PURPOSE:     Discord interface class
 *
 *****************************************************************************/

#pragma once

class CDiscordInterface
{
public:
    virtual ~CDiscordInterface() = default;
    virtual void UpdatePresence() = 0;
    virtual void SetPresenceState(const char* szState) = 0;
    virtual void SetPresenceDetails(const char* szDetails) = 0;
    virtual void SetPresenceStartTimestamp(const unsigned long ulStart) = 0;
};
