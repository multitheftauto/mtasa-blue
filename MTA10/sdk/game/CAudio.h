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

#ifndef __CGAME_AUDIO
#define __CGAME_AUDIO

#include <windows.h>
#include "CVehicle.h"

class CAudio
{
public:
	virtual VOID PlayFrontEndSound(DWORD dwSound)=0;
	virtual VOID PlayBeatTrack ( short iTrack )=0;
	virtual VOID SetEffectsMasterVolume ( BYTE bVolume )=0; // 64 = max volume
	virtual VOID SetMusicMasterVolume ( BYTE bVolume )=0;
	virtual VOID ClearMissionAudio ( int slot = 1)=0;
	virtual VOID PreloadMissionAudio ( unsigned short usAudioEvent, int slot = 1 )=0;
	virtual unsigned char GetMissionAudioLoadingStatus ( int slot = 1 )=0;
	virtual bool IsMissionAudioSampleFinished ( int slot = 1 )=0;
	virtual VOID AttachMissionAudioToPhysical ( CPhysical * physical, int slot = 1 )=0;
	virtual VOID SetMissionAudioPosition ( CVector * position, int slot = 1 )=0;
	virtual bool PlayLoadedMissionAudio ( int slot = 1 )=0;
    virtual VOID PauseAllSound ( bool bPaused )=0;
	virtual VOID StopRadio()=0;
    virtual VOID StartRadio( unsigned int station )=0;
};

#endif