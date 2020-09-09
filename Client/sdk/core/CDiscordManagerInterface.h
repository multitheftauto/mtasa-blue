/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/core/CDiscordManagerInterface.h
 *
 *****************************************************************************/

//
// Some enums are redefined so the interface won't require discord's header
//

#pragma once

#include <functional>

enum EDiscordActivityT
{
    EDiscordActivityT_Playing,
    EDiscordActivityT_Streaming,
    EDiscordActivityT_Listening,
    EDiscordActivityT_Watching
};

enum EDiscordRes
{
    DiscordRes_Ok = 0,
    DiscordRes_ServiceUnavailable = 1,
    DiscordRes_InvalidVersion = 2,
    DiscordRes_LockFailed = 3,
    DiscordRes_InternalError = 4,
    DiscordRes_InvalidPayload = 5,
    DiscordRes_InvalidCommand = 6,
    DiscordRes_InvalidPermissions = 7,
    DiscordRes_NotFetched = 8,
    DiscordRes_NotFound = 9,
    DiscordRes_Conflict = 10,
    DiscordRes_InvalidSecret = 11,
    DiscordRes_InvalidJoinSecret = 12,
    DiscordRes_NoEligibleActivity = 13,
    DiscordRes_InvalidInvite = 14,
    DiscordRes_NotAuthenticated = 15,
    DiscordRes_InvalidAccessToken = 16,
    DiscordRes_ApplicationMismatch = 17,
    DiscordRes_InvalidDataUrl = 18,
    DiscordRes_InvalidBase64 = 19,
    DiscordRes_NotFiltered = 20,
    DiscordRes_LobbyFull = 21,
    DiscordRes_InvalidLobbySecret = 22,
    DiscordRes_InvalidFilename = 23,
    DiscordRes_InvalidFileSize = 24,
    DiscordRes_InvalidEntitlement = 25,
    DiscordRes_NotInstalled = 26,
    DiscordRes_NotRunning = 27,
    DiscordRes_InsufficientBuffer = 28,
    DiscordRes_PurchaseCanceled = 29,
    DiscordRes_InvalidGuild = 30,
    DiscordRes_InvalidEvent = 31,
    DiscordRes_InvalidChannel = 32,
    DiscordRes_InvalidOrigin = 33,
    DiscordRes_RateLimited = 34,
    DiscordRes_OAuth2Error = 35,
    DiscordRes_SelectChannelTimeout = 36,
    DiscordRes_GetGuildTimeout = 37,
    DiscordRes_SelectVoiceForceRequired = 38,
    DiscordRes_CaptureShortcutAlreadyListening = 39,
    DiscordRes_UnauthorizedForAchievement = 40,
    DiscordRes_InvalidGiftCode = 41,
    DiscordRes_PurchaseError = 42,
    DiscordRes_TransactionAborted = 43,
};

struct SDiscordActivity
{
    SDiscordActivity()
        : m_activityType(EDiscordActivityT_Playing),
          m_startTimestamp(0),
          m_endTimestamp(0),
          m_name(""),
          m_state(""),
          m_details(""),
          m_joinSecret(""),
          m_spectateSecret("")
    {
    }

    int64             m_startTimestamp;
    int64             m_endTimestamp;
    const char*       m_name;
    const char*       m_state;
    const char*       m_details;
    const char*       m_joinSecret;
    const char*       m_spectateSecret;
    EDiscordActivityT m_activityType;
};

class CDiscordManagerInterface
{
public:
    virtual void UpdateActivity(SDiscordActivity& activity, std::function<void(EDiscordRes)> callback) = 0;
    virtual void SetType(EDiscordActivityT type, std::function<void(EDiscordRes)> callback) = 0;
    virtual void SetName(char const* name, std::function<void(EDiscordRes)> callback) = 0;
    virtual void SetState(char const* state, std::function<void(EDiscordRes)> callback) = 0;
    virtual void SetDetails(char const* details, std::function<void(EDiscordRes)> callback) = 0;
    virtual void SetStartEndTimestamp(int64 start, int64 end, std::function<void(EDiscordRes)> callback) = 0;
    virtual void SetJoinParameters(const char* joinSecret, const char* partyId, uint partySize, uint partyMax, std::function<void(EDiscordRes)> callback) = 0;
    virtual void SetSpectateSecret(const char* spectateSecret, std::function<void(EDiscordRes)> callback) = 0;
    virtual void RegisterPlay(bool connected) = 0;
    virtual void Disconnect() = 0;

    virtual SString GetJoinSecret() = 0;
};
