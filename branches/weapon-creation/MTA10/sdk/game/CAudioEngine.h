/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CAudio.h
*  PURPOSE:		Game audio interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_AUDIOENGINE
#define __CGAME_AUDIOENGINE

#include <windows.h>
#include "CVehicle.h"

enum eSurfaceType
{
    SURFACE_TYPE_WATER_SHALLOW=39,
};

enum eWeaponEventType
{
    WEAPON_EVENT_FIRE=145,
    WEAPON_EVENT_RELOAD,
    WEAPON_EVENT_RELOAD_2,
    WEAPON_EVENT_PROJECTILE,
};

enum eAmbientSoundType
{
    AMBIENT_SOUND_GENERAL,
    AMBIENT_SOUND_GUNFIRE,
};

class CAudioEngine
{
public:
	virtual void                        PlayFrontEndSound               ( DWORD dwSound ) = 0;
	virtual void                        PlayBeatTrack                   ( short iTrack ) = 0;
	virtual void                        SetEffectsMasterVolume          ( BYTE bVolume ) = 0;
	virtual void                        SetMusicMasterVolume            ( BYTE bVolume ) = 0;
	virtual void                        ClearMissionAudio               ( int slot = 1 ) = 0;
	virtual void                        PreloadMissionAudio             ( unsigned short usAudioEvent, int slot = 1 ) = 0;
	virtual unsigned char               GetMissionAudioLoadingStatus    ( int slot = 1 ) = 0;
	virtual bool                        IsMissionAudioSampleFinished    ( int slot = 1 ) = 0;
	virtual void                        AttachMissionAudioToPhysical    ( CPhysical * physical, int slot = 1 ) = 0;
	virtual void                        SetMissionAudioPosition         ( CVector * position, int slot = 1 ) = 0;
	virtual bool                        PlayLoadedMissionAudio          ( int slot = 1 ) = 0;
    virtual void                        PauseAllSound                   ( bool bPaused ) = 0;
	virtual void                        StopRadio                       ( void ) = 0;
    virtual void                        StartRadio                      ( unsigned int station ) = 0;

    virtual void                        PauseAmbientSounds ( bool bPaused )=0;
    virtual VOID                        SetAmbientSoundEnabled ( eAmbientSoundType eType, bool bEnabled )=0;
    virtual bool                        IsAmbientSoundEnabled ( eAmbientSoundType eType )=0;
    virtual void                        ResetAmbientSounds ( void )=0;

    virtual void                        ReportBulletHit                 ( CEntity * pEntity, unsigned char ucSurfaceType, CVector * pvecPosition, float f_2 ) = 0;
    virtual void                        ReportWeaponEvent               ( int iEvent, eWeaponType weaponType, CPhysical * pPhysical ) = 0;
};

#endif