/*****************************************************************************
 *
 *  PROJECT:        Multi Theft Auto v1.0
 *  LICENSE:        See LICENSE in the top level directory
 *  FILE:        game_sa/CAudioEngineSA.h
 *  PURPOSE:        Header file for audio manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CAudioEngine.h>

#define FUNC_ReportFrontendAudioEvent           0x506EA0
#define FUNC_PreloadBeatTrack                   0x507F40
#define FUNC_PlayPreloadedBeatTrack             0x507180

#define FUNC_SetEffectsMasterVolume             0x506E10
#define FUNC_SetMusicMasterVolume               0x506DE0

#define FUNC_ReportMissionAudioEvent_Vehicle    0x507390
#define FUNC_ReportMissionAudioEvent_Vector     0x507340
#define FUNC_PauseAllSounds                     0x507430
#define FUNC_ResumeAllSounds                    0x507440
#define FUNC_StopRadio                          0x506F70
#define FUNC_StartRadio2                        0x507DC0

#define VAR_AudioEventVolumes                   0xBD00F8 // m_pAudioEventVolumes

#define CLASS_CAudioEngine                      0xB6BC90
#define CLASS_AERadioTrackManager               0x8CB6F8
#define CLASS_AECutsceneTrackManager            0x8AE554
#define FUNC_CAudioEngine_ReportBulletHit                   0x506ec0
#define FUNC_CAudioEngine_ReportWeaponEvent                 0x506f40

#define FUNC_CAESound__Stop                     0x4EF1C0

class CAudioEngineSAInterface
{
};

class CAEAudioEntityVTBL
{
};

class CAEAudioEntity
{
public:
    CAEAudioEntityVTBL* vtbl;
    CEntitySAInterface* pEntity;
    char                m_tempSound[0x74];            // CAESound
};
static_assert(sizeof(CAEAudioEntity) == 0x7C, "Invalid size for CAEAudioEntity");

class CAESound
{
public:
    ushort              usGroup;                        // +0
    ushort              usIndex;                        // +2
    CAEAudioEntity*     pAudioEntity;                   // +4
    CEntitySAInterface* pGameEntity;                    // +8    Either a player or NULL
    unsigned int        m_dwEvent;                      // +12
    float               m_fMaxVolume;                   // +16
    float               m_fVolume;                      // +20
    float               m_fSoundDistance;               // +24
    float               m_fSpeed;                       // +28
    float               unk1;                           // +32
    CVector             m_vCurrPosn;                    // +36
    CVector             m_vPrevPosn;                    // +48
    int                 m_dwLastFrameUpdate;            // +60
    int                 m_dwCurrTimeUpdate;             // +64
    int                 m_dwPrevTimeUpdate;             // +68
    float               m_fCurrCamDist;                 // +72
    float               m_fPrevCamDist;                 // +76
    float               m_fTimeScale;                   // +80
    char                unk2;                           // +84 = 31488
    char                unk3;                           // +85 = 1005
    union
    {
        unsigned short m_wEnvironmentFlags;
        struct
        {
            unsigned short m_bFrontEnd : 1;
            unsigned short m_bUncancellable : 1;
            unsigned short m_bRequestUpdates : 1;
            unsigned short m_bPlayPhysically : 1;
            unsigned short m_bUnpausable : 1;
            unsigned short m_bStartPercentage : 1;
            unsigned short m_bMusicMastered : 1;
            unsigned short m_bLifespanTiedToPhysicalEntity : 1;
            unsigned short m_bUndackable : 1;
            unsigned short m_bUncompressable : 1;
            unsigned short m_bRolledOff : 1;
            unsigned short m_bSmoothDucking : 1;
            unsigned short m_bForcedFront : 1;
        };
    };
    unsigned short m_wIsUsed;                         // +88
    short          unk4;                              // +90 = 1005
    short          m_wCurrentPlayPosition;            // +92
    short          unk5;                              // +94 = 0
    float          m_fFinalVolume;                    // +96
    float          m_fFrequency;                      // +100
    short          m_wPlayingState;                   // +104
    char           unk6[2];                           // +106
    float          m_fSoundHeadRoom;                  // +108
    short          m_wSoundLength;                    // +112
    short          unk8;                              // +114
};
static_assert(sizeof(CAESound) == 0x74, "Invalid size for CAESound");

class CAudioEngineSA : public CAudioEngine
{
public:
    CAudioEngineSA(CAudioEngineSAInterface* pInterface);

    void          PlayFrontEndSound(DWORD dwSound);
    void          PlayBeatTrack(short iTrack);
    void          SetEffectsMasterVolume(BYTE bVolume);            // 64 = max volume
    void          SetMusicMasterVolume(BYTE bVolume);
    void          ClearMissionAudio(int slot = 1);
    void          PreloadMissionAudio(unsigned short usAudioEvent, int slot = 1);
    unsigned char GetMissionAudioLoadingStatus(int slot = 1);
    bool          IsMissionAudioSampleFinished(int slot = 1);
    void          AttachMissionAudioToPhysical(CPhysical* physical, int slot = 1);
    void          SetMissionAudioPosition(CVector* position, int slot = 1);
    bool          PlayLoadedMissionAudio(int slot = 1);
    void          PauseAllSound(bool bPaused);
    void          StopRadio();
    void          StartRadio(unsigned int station);
    void          PauseAmbientSounds(bool bPaused);
    void          SetAmbientSoundEnabled(eAmbientSoundType eType, bool bEnabled);
    bool          IsAmbientSoundEnabled(eAmbientSoundType eType);
    void          ResetAmbientSounds();
    void          SetWorldSoundEnabled(uint uiGroup, uint uiIndex, bool bEnabled, bool bImmediate);
    bool          IsWorldSoundEnabled(uint uiGroup, uint uiIndex);
    void          ResetWorldSounds();
    void          SetWorldSoundHandler(WorldSoundHandler* pHandler);
    void          ReportBulletHit(CEntity* pEntity, unsigned char ucSurfaceType, CVector* pvecPosition, float f_2);
    void          ReportWeaponEvent(int iEvent, eWeaponType weaponType, CPhysical* pPhysical);

    void UpdateAmbientSoundSettings();
    bool OnWorldSound(CAESound* pAESound);

private:
    bool               m_bRadioOn;
    bool               m_bRadioMuted;
    unsigned char      m_ucRadioChannel;
    bool               m_bAmbientSoundsPaused;
    bool               m_bAmbientGeneralEnabled;
    bool               m_bAmbientGunfireEnabled;
    CRanges            m_DisabledWorldSounds;
    WorldSoundHandler* m_pWorldSoundHandler;

    CAudioEngineSAInterface* m_pInterface;
};
