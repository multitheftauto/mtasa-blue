/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAEVehicleAudioEntitySA.h
 *  PURPOSE:     Vehicle audio entity header
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CAEVehicleAudioEntity.h>
#include "CAudioEngineSA.h"

#define FUNC_CAEVehicleAudioEntity__JustGotInVehicleAsDriver                           0x4F5700
#define FUNC_CAEVehicleAudioEntity__TurnOnRadioForVehicle                              0x4F5B20
#define FUNC_CAEVehicleAudioEntity__JustGotOutOfVehicleAsDriver                        0x4FCF40
#define FUNC_CAEVehicleAudioEntity__ProcessAIProp                                      0x4FDFD0
#define FUNC_CAEVehicleAudioEntity__ProcessAIHeli                                      0x4FEE20

struct tVehicleAudioSettings
{
    char  m_nVehicleSoundType;
    char  unk1;
    short m_wEngineOnSoundBankId;
    short m_wEngineOffSoundBankId;
    char  m_nStereo;
    char  unk2;
    int   unk3;
    int   unk4;
    char  m_bHornTon;
    char  unk5[3];
    float m_fHornHigh;
    char  m_nDoorSound;
    char  unk6;
    char  m_nRadioNum;
    char  m_nRadioType;
    char  unk7;
    char  unk8[3];
    float m_fHornVolumeDelta;
};
static_assert(sizeof(tVehicleAudioSettings) == 0x24, "Invalid size for tVehicleAudioSettings");

struct tVehicleSound
{
    int       m_dwIndex;
    CAESound* m_pSound;
};

class CAETwinLoopSoundEntity : CAEAudioEntity
{
    short           m_wBankSlotId;
    short           m_wSoundType[2];
    char            pad1[2];
    CAEAudioEntity* m_pBaseAudio;
    short           unk1;
    short           unk2;
    short           unk3;
    short           m_wPlayTimeMin;
    short           m_wPlayTimeMax;
    char            pad2[2];
    unsigned int    m_dwTimeToSwapSounds;
    bool            m_bPlayingFirstSound;
    char            pad3;
    short           m_wStartingPlayPercentage[2];
    short           unk4;
    CAESound*       m_apSounds[2];
};
static_assert(sizeof(CAETwinLoopSoundEntity) == 0xA8, "Invalid size for CAETwinLoopSoundEntity");

class CAEVehicleAudioEntitySAInterface : public CAEAudioEntity
{
public:
    void AddAudioEvent(int eventId, float volume)
    {
        ((void(__thiscall*)(CAEVehicleAudioEntitySAInterface*, int, float))0x4F6420)(this, eventId, volume);
    }

    short                  unk1;                                      // +124
    char                   unk2[2];                                   // +126
    tVehicleAudioSettings  m_nSettings;                               // +128
    bool                   m_bEnabled;                                // +164
    bool                   m_bPlayerDriver;                           // +165
    bool                   m_bPlayerPassenger;                        // +166
    bool                   m_bVehicleRadioPaused;                     // +167
    bool                   m_bSoundsStopped;                          // +168
    char                   m_nEngineState;                            // +169
    char                   unk3;                                      // +170
    char                   unk4;                                      // +171
    int                    unk5;                                      // +172
    bool                   m_bInhibitAccForLowSpeed;                  // +176
    char                   unk6;                                      // +177
    short                  m_wRainDropCounter;                        // +178
    short                  unk7;                                      // +180
    char                   pad1[2];                                   // +182
    int                    unk8;                                      // +184
    char                   unk9;                                      // +188
    bool                   m_bDisableHeliEngineSounds;                // +189
    char                   unk10;                                     // +190
    bool                   m_bSirenOrAlarmPlaying;                    // +191
    bool                   m_bHornPlaying;                            // +192
    char                   pad2[3];                                   // +193
    float                  m_fSirenVolume;                            // +196
    bool                   m_bModelWithSiren;                         // +200
    char                   pad3[3];                                   // +201
    unsigned int           m_dwBoatHitWaveLastPlayedTime;             // +204
    unsigned int           m_dwTimeToInhibitAcc;                      // +208
    unsigned int           m_dwTimeToInhibitCrz;                      // +212
    float                  m_fGeneralVehicleSoundVolume;              // +216
    short                  m_wEngineDecelerateSoundBankId;            // +220
    short                  m_wEngineAccelerateSoundBankId;            // +222
    short                  m_wEngineBankSlotId;                       // +224
    short                  unk11;                                     // +226
    tVehicleSound          m_aEngineSounds[12];                       // +228
    int                    unk12;                                     // +324
    short                  unk13;                                     // +328
    short                  unk14;                                     // +330
    short                  unk15;                                     // +332
    short                  unk16;                                     // +334
    int                    unk17;                                     // some time in ms (@0x4F5638)               // +336
    short                  unk18;                                     // +340
    short                  m_wSkidSoundType;                          // +342
    CAESound*              unk19;                                     // +344
    short                  m_wRoadNoiseSoundType;                     // +348
    char                   pad4[2];                                   // +350
    CAESound*              m_pRoadNoiseSound;                         // +352
    short                  m_wFlatTyreSoundType;                      // +356
    char                   pad5[2];                                   // +358
    CAESound*              m_pFlatTyreSound;                          // +360
    short                  m_wReverseGearSoundType;                   // +364
    char                   pad6[2];                                   // +366
    CAESound*              m_pReverseGearSound;                       // +368
    char                   pad7[4];                                   // +372
    CAESound*              m_pHornTonSound;                           // +376
    CAESound*              m_pSirenSound;                             // +380
    CAESound*              m_pPoliceSirenSound;                       // +384
    CAETwinLoopSoundEntity m_nTwinLoopSoundEntity;                    // +388
    float                  unk20;                                     // +556
    float                  unk21;                                     // +560
    float                  unk22;                                     // +564
    float                  unk23;                                     // +568
    float                  unk24;                                     // +572
    int                    unk25;                                     // +576
    bool                   m_bNitroSoundPresent;                      // +580
    char                   unk26;                                     // +581
    float                  unk27;                                     // +582
};
static_assert(sizeof(CAEVehicleAudioEntitySAInterface) == 0x24C, "Invalid size for CAEVehicleAudioEntitySAInterface");

class CAEVehicleAudioEntitySA : public CAEVehicleAudioEntity
{
public:
    CAEVehicleAudioEntitySA(CAEVehicleAudioEntitySAInterface* pInterface);
    void JustGotInVehicleAsDriver();
    void JustGotOutOfVehicleAsDriver();
    void TurnOnRadioForVehicle();
    void StopVehicleEngineSound(unsigned char ucSlot);

private:
    CAEVehicleAudioEntitySAInterface* m_pInterface;
};
