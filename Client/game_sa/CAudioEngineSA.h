/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CAudioEngineSA.h
 *  PURPOSE:     Header file for audio manager class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CAudioEngine.h>

#define VAR_AudioEventVolumes                   0xBD00F8 // m_pAudioEventVolumes

#define CLASS_CAudioEngine                      0xB6BC90
#define CLASS_AECutsceneTrackManager            0x8AE554

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
        ushort m_wEnvironmentFlags;
        struct
        {
            ushort m_bFrontEnd : 1;
            ushort m_bUncancellable : 1;
            ushort m_bRequestUpdates : 1;
            ushort m_bPlayPhysically : 1;
            ushort m_bUnpausable : 1;
            ushort m_bStartPercentage : 1;
            ushort m_bMusicMastered : 1;
            ushort m_bLifespanTiedToPhysicalEntity : 1;
            ushort m_bUndackable : 1;
            ushort m_bUncompressable : 1;
            ushort m_bRolledOff : 1;
            ushort m_bSmoothDucking : 1;
            ushort m_bForcedFront : 1;
        };
    };
    ushort m_wIsUsed;                         // +88
    short  unk4;                              // +90 = 1005
    short  m_wCurrentPlayPosition;            // +92
    short  unk5;                              // +94 = 0
    float  m_fFinalVolume;                    // +96
    float  m_fFrequency;                      // +100
    short  m_wPlayingState;                   // +104
    char   unk6[2];                           // +106
    float  m_fSoundHeadRoom;                  // +108
    short  m_wSoundLength;                    // +112
    short  unk8;                              // +114
};
static_assert(sizeof(CAESound) == 0x74, "Invalid size for CAESound");

class CAudioEngineSA : public CAudioEngine
{
public:
    CAudioEngineSA(CAudioEngineSAInterface* pInterface);

    void  PlayFrontEndSound(DWORD dwSound);
    void  PlayBeatTrack(short iTrack);
    void  SetEffectsMasterVolume(BYTE bVolume);            // 64 = max volume
    void  SetMusicMasterVolume(BYTE bVolume);
    void  ClearMissionAudio(int slot = 1);
    void  PreloadMissionAudio(unsigned short usAudioEvent, int slot = 1);
    uchar GetMissionAudioLoadingStatus(int slot = 1);
    bool  IsMissionAudioSampleFinished(int slot = 1);
    void  AttachMissionAudioToPhysical(CPhysical* physical, int slot = 1);
    void  SetMissionAudioPosition(CVector* position, int slot = 1);
    bool  PlayLoadedMissionAudio(int slot = 1);
    void  PauseAllSound(bool bPaused);
    void  StopRadio();
    void  StartRadio(unsigned int station);
    void  PauseAmbientSounds(bool bPaused);
    void  SetAmbientSoundEnabled(eAmbientSoundType eType, bool bEnabled);
    bool  IsAmbientSoundEnabled(eAmbientSoundType eType);
    void  ResetAmbientSounds();
    void  SetWorldSoundEnabled(uint uiGroup, uint uiIndex, bool bEnabled, bool bImmediate);
    bool  IsWorldSoundEnabled(uint uiGroup, uint uiIndex);
    void  ResetWorldSounds();
    void  SetWorldSoundHandler(WorldSoundHandler* pHandler);
    void  ReportBulletHit(CEntity* pEntity, unsigned char ucSurfaceType, CVector* pvecPosition, float f_2);
    void  ReportWeaponEvent(int iEvent, eWeaponType weaponType, CPhysical* pPhysical);

    void UpdateAmbientSoundSettings();
    bool OnWorldSound(CAESound* pAESound);

private:
    bool               m_bRadioOn;
    bool               m_bRadioMuted;
    uchar              m_ucRadioChannel;
    bool               m_bAmbientSoundsPaused;
    bool               m_bAmbientGeneralEnabled;
    bool               m_bAmbientGunfireEnabled;
    CRanges            m_DisabledWorldSounds;
    WorldSoundHandler* m_pWorldSoundHandler;

    CAudioEngineSAInterface* m_pInterface;
};
