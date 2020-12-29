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

#include "Common.h"
#include "CAudioEngineSA.h"
#include <game/CAEVehicleAudioEntity.h>

#define FUNC_CAEVehicleAudioEntity__RequestBankSlot                                    0x4F4D10
#define FUNC_CAEVehicleAudioEntity__StoppedUsingBankSlot                               0x4F4DF0
#define FUNC_CAEVehicleAudioEntity__DoesBankSlotContainThisBank                        0x4F4E30
#define FUNC_CAEVehicleAudioEntity__DemandBankSlot                                     0x4F4E60
#define FUNC_CAEVehicleAudioEntity__StaticService                                      0x4F4EC0
#define FUNC_CAEVehicleAudioEntity__StaticGetPlayerVehicleAudioSettingsForRadio        0x4F4ED0
#define FUNC_CAEVehicleAudioEntity__EnableHelicoptors                                  0x4F4EE0
#define FUNC_CAEVehicleAudioEntity__DisableHelicoptors                                 0x4F4EF0
#define FUNC_CAEVehicleAudioEntity__GetVehicleTypeForAudio                             0x4F4F00
#define FUNC_CAEVehicleAudioEntity__IsAccInhibited                                     0x4F4F70
#define FUNC_CAEVehicleAudioEntity__IsAccInhibitedBackwards                            0x4F4FC0
#define FUNC_CAEVehicleAudioEntity__IsAccInhibitedForLowSpeed                          0x4F4FF0
#define FUNC_CAEVehicleAudioEntity__IsAccInhibitedForTime                              0x4F5020
#define FUNC_CAEVehicleAudioEntity__InhibitAccForTime                                  0x4F5030
#define FUNC_CAEVehicleAudioEntity__IsCrzInhibitedForTime                              0x4F5050
#define FUNC_CAEVehicleAudioEntity__InhibitCrzForTime                                  0x4F5060
#define FUNC_CAEVehicleAudioEntity__GetAccelAndBrake                                   0x4F5080
#define FUNC_CAEVehicleAudioEntity__GetVolumeForDummyIdle                              0x4F51F0
#define FUNC_CAEVehicleAudioEntity__GetFrequencyForDummyIdle                           0x4F5310
#define FUNC_CAEVehicleAudioEntity__GetVolumeForDummyRev                               0x4F53D0
#define FUNC_CAEVehicleAudioEntity__GetFrequencyForDummyRev                            0x4F54F0
#define FUNC_CAEVehicleAudioEntity__CancelVehicleEngineSound                           0x4F55C0
#define FUNC_CAEVehicleAudioEntity__UpdateVehicleEngineSound                           0x4F56D0
#define FUNC_CAEVehicleAudioEntity__JustGotInVehicleAsDriver                           0x4F5700
#define FUNC_CAEVehicleAudioEntity__TurnOnRadioForVehicle                              0x4F5B20
#define FUNC_CAEVehicleAudioEntity__TurnOffRadioForVehicle                             0x4F5B60
#define FUNC_CAEVehicleAudioEntity__PlayerAboutToExitVehicleAsDriver                   0x4F5BA0
#define FUNC_CAEVehicleAudioEntity__DisableHelicoptor                                  0x4F5BF0
#define FUNC_CAEVehicleAudioEntity__EnableHelicoptor                                   0x4F5C00
#define FUNC_CAEVehicleAudioEntity__GetVehicleAudioSettings                            0x4F5C10
#define FUNC_CAEVehicleAudioEntity__CopHeli                                            0x4F5C40
#define FUNC_CAEVehicleAudioEntity__GetFreqForIdle                                     0x4F5C60
#define FUNC_CAEVehicleAudioEntity__GetVolForPlayerEngineSound                         0x4F5D00
#define FUNC_CAEVehicleAudioEntity__JustFinishedAccelerationLoop                       0x4F5E50
#define FUNC_CAEVehicleAudioEntity__UpdateGasPedalAudio                                0x4F5EB0
#define FUNC_CAEVehicleAudioEntity__GetVehicleDriveWheelSkidValue                      0x4F5F30
#define FUNC_CAEVehicleAudioEntity__GetVehicleNonDriveWheelSkidValue                   0x4F6000
#define FUNC_CAEVehicleAudioEntity__GetBaseVolumeForBicycleTyre                        0x4F60B0
#define FUNC_CAEVehicleAudioEntity__GetFlyingMetalVolume                               0x4F6150
#define FUNC_CAEVehicleAudioEntity__GetHornState                                       0x4F61E0
#define FUNC_CAEVehicleAudioEntity__GetSirenState                                      0x4F62A0
#define FUNC_CAEVehicleAudioEntity__StopGenericEngineSound                             0x4F6320
#define FUNC_CAEVehicleAudioEntity__CAEVehicleAudioEntity                              0x4F63E0
#define FUNC_CAEVehicleAudioEntity__AddAudioEvent                                      0x4F6420
#define FUNC_CAEVehicleAudioEntity__AddAudioEvent_1                                    0x4F7580 // (renamed)
#define FUNC_CAEVehicleAudioEntity__Initialise                                         0x4F7670
#define FUNC_CAEVehicleAudioEntity__RequestNewPlayerCarEngineSound                     0x4F7A50
#define FUNC_CAEVehicleAudioEntity__StartVehicleEngineSound                            0x4F7F20
#define FUNC_CAEVehicleAudioEntity__GetFreqForPlayerEngineSound                        0x4F8070
#define FUNC_CAEVehicleAudioEntity__PlaySkidSound                                      0x4F8360
#define FUNC_CAEVehicleAudioEntity__PlayRoadNoiseSound                                 0x4F84D0
#define FUNC_CAEVehicleAudioEntity__PlayFlatTyreSound                                  0x4F8650
#define FUNC_CAEVehicleAudioEntity__PlayReverseSound                                   0x4F87D0
#define FUNC_CAEVehicleAudioEntity__ProcessVehicleFlatTyre                             0x4F8940
#define FUNC_CAEVehicleAudioEntity__ProcessVehicleRoadNoise                            0x4F8B00
#define FUNC_CAEVehicleAudioEntity__ProcessReverseGear                                 0x4F8DF0
#define FUNC_CAEVehicleAudioEntity__ProcessVehicleSkidding                             0x4F8F10
#define FUNC_CAEVehicleAudioEntity__ProcessRainOnVehicle                               0x4F92C0
#define FUNC_CAEVehicleAudioEntity__PlayAircraftSound                                  0x4F93C0
#define FUNC_CAEVehicleAudioEntity__GetAircraftNearPosition                            0x4F96A0
#define FUNC_CAEVehicleAudioEntity__PlayBicycleSound                                   0x4F9710
#define FUNC_CAEVehicleAudioEntity__PlayHornOrSiren                                    0x4F99D0
#define FUNC_CAEVehicleAudioEntity__UpdateBoatSound                                    0x4F9E90
#define FUNC_CAEVehicleAudioEntity__ProcessBoatMovingOverWater                         0x4FA0C0
#define FUNC_CAEVehicleAudioEntity__UpdateTrainSound                                   0x4FA1C0
#define FUNC_CAEVehicleAudioEntity__ProcessTrainTrackSound                             0x4FA3F0
#define FUNC_CAEVehicleAudioEntity__PlayTrainBrakeSound                                0x4FA630
#define FUNC_CAEVehicleAudioEntity__ProcessDummyRCPlane                                0x4FA7C0
#define FUNC_CAEVehicleAudioEntity__ProcessDummyRCHeli                                 0x4FAA80
#define FUNC_CAEVehicleAudioEntity__UpdateGenericVehicleSound                          0x4FAD40
#define FUNC_CAEVehicleAudioEntity__ProcessEngineDamage                                0x4FAE20
#define FUNC_CAEVehicleAudioEntity__ProcessNitro                                       0x4FB070
#define FUNC_CAEVehicleAudioEntity__ProcessMovingParts                                 0x4FB260
#define FUNC_CAEVehicleAudioEntity__UpdateParameters                                   0x4FB6C0
#define FUNC_CAEVehicleAudioEntity__Terminate                                          0x4FB8C0
#define FUNC_CAEVehicleAudioEntity__ProcessPlayerVehicleEngine                         0x4FBB10
#define FUNC_CAEVehicleAudioEntity__ProcessDummyStateTransition                        0x4FCA10
#define FUNC_CAEVehicleAudioEntity__JustGotOutOfVehicleAsDriver                        0x4FCF40
#define FUNC_CAEVehicleAudioEntity__JustWreckedVehicle                                 0x4FD0B0
#define FUNC_CAEVehicleAudioEntity__ProcessPlayerProp                                  0x4FD290
#define FUNC_CAEVehicleAudioEntity__ProcessDummyProp                                   0x4FD8F0
#define FUNC_CAEVehicleAudioEntity__ProcessAIProp                                      0x4FDFD0
#define FUNC_CAEVehicleAudioEntity__ProcessPlayerHeli                                  0x4FE420
#define FUNC_CAEVehicleAudioEntity__ProcessDummyHeli                                   0x4FE940
#define FUNC_CAEVehicleAudioEntity__ProcessAIHeli                                      0x4FEE20
#define FUNC_CAEVehicleAudioEntity__ProcessPlayerSeaPlane                              0x4FF320
#define FUNC_CAEVehicleAudioEntity__ProcessDummySeaPlane                               0x4FF7C0
#define FUNC_CAEVehicleAudioEntity__ProcessGenericJet                                  0x4FF900
#define FUNC_CAEVehicleAudioEntity__ProcessDummyBicycle                                0x4FFDC0
#define FUNC_CAEVehicleAudioEntity__ProcessPlayerBicycle                               0x500040
#define FUNC_CAEVehicleAudioEntity__ProcessVehicleSirenAlarmHorn                       0x5002C0
#define FUNC_CAEVehicleAudioEntity__ProcessBoatEngine                                  0x5003A0
#define FUNC_CAEVehicleAudioEntity__ProcessDummyTrainEngine                            0x500710
#define FUNC_CAEVehicleAudioEntity__ProcessPlayerTrainBrakes                           0x500AB0
#define FUNC_CAEVehicleAudioEntity__ProcessPlayerCombine                               0x500CE0
#define FUNC_CAEVehicleAudioEntity__ProcessDummyRCCar                                  0x500DC0
#define FUNC_CAEVehicleAudioEntity__ProcessDummyHovercraft                             0x500F50
#define FUNC_CAEVehicleAudioEntity__ProcessDummyGolfCart                               0x501270
#define FUNC_CAEVehicleAudioEntity__ProcessDummyVehicleEngine                          0x501480
#define FUNC_CAEVehicleAudioEntity__ProcessPlayerJet                                   0x501650
#define FUNC_CAEVehicleAudioEntity__ProcessDummyJet                                    0x501960
#define FUNC_CAEVehicleAudioEntity__ProcessSpecialVehicle                              0x501AB0
#define FUNC_CAEVehicleAudioEntity__ProcessAircraft                                    0x501C50
#define FUNC_CAEVehicleAudioEntity__ProcessVehicle                                     0x501E10
#define FUNC_CAEVehicleAudioEntity__Service                                            0x502280

#define VAR_CAEVehicleAudioEntity__s_pPlayerDriver                                     0xB6B990

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
