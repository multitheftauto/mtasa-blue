/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CAudioEngineSA.h
*  PURPOSE:		Header file for audio manager class
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_AUDIOENGINE
#define __CGAMESA_AUDIOENGINE

#include "Common.h"
#include <game/CAudioEngine.h>
#include "CVehicleSA.h"


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

class CAudioEngineSAInterface
{
};

class CAEAudioEntityVTBL
{

};

class CAEAudioEntity
{
public:
    CAEAudioEntityVTBL * vtbl;
    CEntitySAInterface * pEntity;
};

class CAESound
{
public:
    ushort  usGroup;    //+0
    ushort  usIndex;    //+2
    CAEAudioEntity*         pAudioEntity;   //+4
    CEntitySAInterface*     pGameEntity;    //+8    Either a player or NULL
    int     a;          //+c  = 3
    float   b;          //+10 = -1.f
    float   c;          //+14 = -50.f
    float   fVolume;    //+18 = 1.f
    float   fPitch;     //+1c = 1.f
    uint    d;          //+20
    CVector vec1;       //+24 = Position of player?
    CVector vec2;       //+30 = Position of player?
    int     e;          //+3c = 1,621         set from framecounter, no set pos if non zero
    int     f;          //+40 = 300           time in milliseconds
    int     g;          //+44 = 300           set from framecounter
    float   h;          //+48 = 2997.3567f    start something?
    float   j;          //+4c = 2997.3567f    current something?
    float   k;          //+50 = 1.0f
    ushort  l;          //+54 = 31488
    ushort  m;          //+56 = 1005
    int     n;          //+58 = 1
    int     o;          //+5C = 0
    float   p;          //+60 = -100.f
    float   q;          //+64 = 1.f
    ushort  r;          //+68 = 0         (1 on stop)
    ushort  s;          //+6a = 114       
};

class CAudioEngineSA : public CAudioEngine
{
public:
                                CAudioEngineSA                  ( CAudioEngineSAInterface * pInterface );

    void                        PlayFrontEndSound               ( DWORD dwSound );
    void                        PlayBeatTrack                   ( short iTrack );
    void                        SetEffectsMasterVolume          ( BYTE bVolume ); // 64 = max volume
    void                        SetMusicMasterVolume            ( BYTE bVolume );
    void                        ClearMissionAudio               ( int slot = 1 );
    void                        PreloadMissionAudio             ( unsigned short usAudioEvent, int slot = 1 );
    unsigned char               GetMissionAudioLoadingStatus    ( int slot = 1 );
    bool                        IsMissionAudioSampleFinished    ( int slot = 1 );
    void                        AttachMissionAudioToPhysical    ( CPhysical * physical, int slot = 1 );
    void                        SetMissionAudioPosition         ( CVector * position, int slot = 1 );
    bool                        PlayLoadedMissionAudio          ( int slot = 1 );
    void                        PauseAllSound                   ( bool bPaused );
    void                        StopRadio                       ( void );
    void                        StartRadio                      ( unsigned char ucStation );
    void                        RetuneRadio                     ( unsigned char ucStation );
    bool                        IsVehicleRadioActive            ( void );
    void                        PauseAmbientSounds              ( bool bPaused );
    void                        SetAmbientSoundEnabled          ( eAmbientSoundType eType, bool bEnabled );
    bool                        IsAmbientSoundEnabled           ( eAmbientSoundType eType );
    void                        ResetAmbientSounds              ( void );
    void                        SetWorldSoundEnabled            ( uint uiGroup, uint uiIndex, bool bEnabled );
    bool                        IsWorldSoundEnabled             ( uint uiGroup, uint uiIndex );
    void                        ResetWorldSounds                ( void );
    void                        SetWorldSoundHandler            ( WorldSoundHandler * pHandler );
    void                        ReportBulletHit                 ( CEntity * pEntity, unsigned char ucSurfaceType, CVector * pvecPosition, float f_2 );
    void                        ReportWeaponEvent               ( int iEvent, eWeaponType weaponType, CPhysical * pPhysical );

    void                        UpdateAmbientSoundSettings      ( void );
    bool                        OnWorldSound                    ( CAESound* pAESound );

private:
    bool            m_bRadioOn;
    bool            m_bRadioMuted;
    unsigned char   m_ucRadioChannel;
    bool            m_bAmbientSoundsPaused;
    bool            m_bAmbientGeneralEnabled;
    bool            m_bAmbientGunfireEnabled;
    CRanges         m_DisabledWorldSounds;
    WorldSoundHandler* m_pWorldSoundHandler;

    CAudioEngineSAInterface *   m_pInterface;
};

#endif