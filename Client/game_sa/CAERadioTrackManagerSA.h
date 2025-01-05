/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAERadioTrackManagerSA.cpp
 *  PURPOSE:     Header file for audio entity radio track manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CAERadioTrackManager.h>

#define FUNC_GetCurrentRadioStationID   0x4E83F0
#define FUNC_IsVehicleRadioActive       0x4E9800
#define FUNC_GetRadioStationName        0x4E9E10
#define FUNC_IsRadioOn                  0x4E8350
#define FUNC_SetBassSetting             0x4E82F0
#define FUNC_Reset                      0x4E7F80
#define FUNC_StartRadio                 0x4EB3C0

#define CLASS_CAERadioTrackManager      0x8CB6F8

enum class eRadioTrackMode
{
    RADIO_STARTING,
    RADIO_WAITING_TO_PLAY,
    RADIO_PLAYING,
    RADIO_STOPPING,
    RADIO_STOPPING_SILENCED,
    RADIO_STOPPING_CHANNELS_STOPPED,
    RADIO_WAITING_TO_STOP,
    RADIO_STOPPED
};

struct tRadioSettings
{
    std::int32_t djIndex[4];
    std::int32_t currentTrackId;
    std::int32_t prevTrackId;
    std::int32_t trackPlayTime;
    std::int32_t trackLengthInMS;
    std::uint8_t trackFlags[2];
    std::uint8_t currentRadioStation;
    std::uint8_t field_27;
    std::uint8_t bassSet;
    float        bassGain;
    std::uint8_t trackTypes[4];
    std::uint8_t currentTrackType;
    std::uint8_t prevTrackType;
    std::int8_t  trackIndexes[10];
};
static_assert(sizeof(tRadioSettings) == 0x3C, "Invalid size of tRadioSettings struct!");

struct tRadioState
{
    std::int32_t elapsed[3];
    std::int32_t timeInPauseModeInMS;
    std::int32_t timeInMS;
    std::int32_t trackPlayTime;
    std::int32_t trackQueue[3];
    std::uint8_t trackTypes[3];
    std::uint8_t gameMonthDay;
    std::uint8_t gameClockHours;
};
static_assert(sizeof(tRadioState) == 0x2C, "Invalid size of tRadioState struct!");

class CAERadioTrackManagerSAInterface
{
public:
    bool            isInitialised;
    bool            displayStationName;
    std::uint8_t    field_2;
    bool            enableInPauseMode;
    bool            bassEnhance;
    bool            pauseMode;
    bool            retuneJustStarted;
    bool            autoSelect;
    std::uint8_t    tracksInARow[14];
    std::uint8_t    gameMonthDay;
    std::uint8_t    gameClockHours;
    std::int32_t    listenItems[14];
    std::uint32_t   timeRadioStationReturned;
    std::uint32_t   timeToDisplayRadioName;
    std::uint32_t   savedTimeInMS;
    std::uint32_t   retuneStartedTime;
    std::uint8_t    field_60[4];
    std::int32_t    hwClientHandle;
    eRadioTrackMode trackMode;
    std::int32_t    stationsListed;
    std::int32_t    stationsListDown;
    std::int32_t    savedRadioStationId;
    std::int32_t    radioStationMenuRequest;
    std::int32_t    radioStationScriptRequest;
    float           volume1;
    float           volume2;
    tRadioSettings  requestedSettings;
    tRadioSettings  activeSettings;
    tRadioState     radioState[13];
    std::uint8_t    field_33C[12];
    std::uint8_t    field_348[32];
    std::uint32_t   field_368;
    std::uint8_t    userTrackPlayMode;
    std::uint8_t    field_36D[3];
};
static_assert(sizeof(CAERadioTrackManagerSAInterface) == 0x370, "Invalid size of CAERadioTrackManagerSAInterface class!");

class CAERadioTrackManagerSA : public CAERadioTrackManager
{
public:
    CAERadioTrackManagerSAInterface* GetInterface() const noexcept { return reinterpret_cast<CAERadioTrackManagerSAInterface*>(CLASS_CAERadioTrackManager); }

    BYTE  GetCurrentRadioStationID();
    BYTE  IsVehicleRadioActive();
    char* GetRadioStationName(BYTE bStationID);
    bool  IsRadioOn();
    void  SetBassSetting(DWORD dwBass);
    void  Reset();
    void  StartRadio(BYTE bStationID, BYTE bUnknown);
    bool IsStationLoading() const;
};
