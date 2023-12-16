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
#define DISCORD_DISABLE_IO_THREAD
// Uncomment to use manuall refresh of discord connection
// (important) Don't forget to write same in discord-rpc.h

class CDiscordInterface
{
public:
    virtual ~CDiscordInterface() = default;
    virtual void UpdatePresence() = 0;
#ifdef DISCORD_DISABLE_IO_THREAD
    virtual void UpdatePresenceConnection() = 0;
#endif
    virtual void SetAssetLargeData(const char* szAsset, const char* szAssetText) = 0;
    virtual void SetAssetSmallData(const char* szAsset, const char* szAssetText) = 0;
    virtual void SetPresenceStartTimestamp(const unsigned long ulStart) = 0;
    virtual void SetPresenceEndTimestamp(const unsigned long ulEnd) = 0;
    virtual void SetPresencePartySize(int iSize, int iMax, bool bCustom) = 0;
    virtual void SetDiscordClientConnected(bool bConnected) = 0;
    virtual void SetDiscordUserID(const std::string& strUserID) = 0;

    virtual bool SetPresenceDetails(const char* szDetails, bool bCustom) = 0;
    virtual bool SetApplicationID(const char* szResourceName, const char* szAppID) = 0;
    virtual bool ResetDiscordData() = 0;
    virtual bool SetPresenceState(const char* szState, bool bCustom) = 0;
    virtual bool SetPresenceButtons(unsigned short int iIndex, const char* szName, const char* szUrl) = 0;
    virtual bool SetDiscordRPCEnabled(bool bEnabled) = 0;
    virtual bool IsDiscordRPCEnabled() const = 0;
    virtual bool IsDiscordCustomDetailsDisallowed() const = 0;
    virtual bool IsDiscordClientConnected() const = 0;

    virtual std::string GetDiscordResourceName() const = 0;
    virtual std::string GetDiscordUserID() const = 0;
};
