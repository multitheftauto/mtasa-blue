/*****************************************************************************
 *
 *  PROJECT:        Multi Theft Auto v1.0
 *  LICENSE:        See LICENSE in the top level directory
 *  FILE:        sdk/game/CAEAudioHardware.h
 *  PURPOSE:        Game audio hardware interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

enum eBankSlot : short
{
    BANKSLOT_FRONTEND_GAME = 0,
    BANKSLOT_FRONTEND_MENU = 1,
    BANKSLOT_COLLISIONS = 2,
    BANKSLOT_BULLET_SOUNDS = 3,
    BANKSLOT_EXPLOSIONS = 4,
    BANKSLOT_WEAPONS = 5,
    BANKSLOT_WEATHER_RAIN = 6,
    BANKSLOT_STREAM_ENGINE_1 = 7,
    BANKSLOT_STREAM_ENGINE_2 = 8,
    BANKSLOT_STREAM_ENGINE_3 = 9,
    BANKSLOT_STREAM_ENGINE_4 = 10,
    BANKSLOT_STREAM_ENGINE_5 = 11,
    BANKSLOT_STREAM_ENGINE_6 = 12,
    BANKSLOT_STREAM_ENGINE_7 = 13,
    BANKSLOT_STREAM_ENGINE_8 = 14,
    BANKSLOT_STREAM_ENGINE_9 = 15,
    BANKSLOT_STREAM_ENGINE_10 = 16,
    BANKSLOT_HORNS = 17,
    BANKSLOT_HELICOPTER = 18,
    BANKSLOT_VEHICLE_EXTRAS = 19,
    BANKSLOT_SPEECH_0 = 20,
    BANKSLOT_SPEECH_1 = 21,
    BANKSLOT_SPEECH_2 = 22,
    BANKSLOT_SPEECH_3 = 23,
    BANKSLOT_SPEECH_4 = 24,
    BANKSLOT_PLAYER_SPEECH = 25,
    BANKSLOT_SCRIPT_SPEECH_0 = 26,
    BANKSLOT_SCRIPT_SPEECH_1 = 27,
    BANKSLOT_SCRIPT_SPEECH_2 = 28,
    BANKSLOT_SCRIPT_SPEECH_3 = 29,
    BANKSLOT_AMBIENT_RESIDENT = 30,
    BANKSLOT_DOORS = 31,
    BANKSLOT_WATER = 32,
    BANKSLOT_33 = 33,
    BANKSLOT_34 = 34,
    BANKSLOT_35 = 35,
    BANKSLOT_36 = 36,
    BANKSLOT_37 = 37,
    BANKSLOT_38 = 38,
    BANKSLOT_39 = 39,
    BANKSLOT_ENGINE_RESIDENT = 40,
    BANKSLOT_FEET_RESIDENT = 41,
    BANKSLOT_BULLET_TRAIL = 42,
    BANKSLOT_43 = 43,
    BANKSLOT_44 = 44
};

class CAEAudioHardware
{
public:
    virtual bool IsSoundBankLoaded(short wSoundBankID, short wSoundBankSlotID) = 0;
    virtual void LoadSoundBank(short wSoundBankID, short wSoundBankSlotID) = 0;
};
