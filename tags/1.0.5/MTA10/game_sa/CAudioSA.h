/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CAudioSA.h
*  PURPOSE:     Header file for audio manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_AUDIO
#define __CGAMESA_AUDIO

#include "Common.h"
#include <game/CAudio.h>
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
class CAudioSA : public CAudio
{
public:
    CAudioSA ();
    VOID PlayFrontEndSound(DWORD dwSound);
    VOID PlayBeatTrack ( short iTrack );
    VOID SetEffectsMasterVolume ( BYTE bVolume );
    VOID SetMusicMasterVolume ( BYTE bVolume );
    VOID ClearMissionAudio ( int slot = 1);
    VOID PreloadMissionAudio ( unsigned short usAudioEvent, int slot = 1 );
    unsigned char GetMissionAudioLoadingStatus ( int slot = 1 );
    bool IsMissionAudioSampleFinished ( int slot = 1 );
    VOID AttachMissionAudioToPhysical ( CPhysical * physical, int slot = 1 );
    VOID SetMissionAudioPosition ( CVector * position, int slot = 1 );
    bool PlayLoadedMissionAudio ( int slot = 1 );
    VOID PauseAllSound ( bool bPaused );
    VOID StopRadio();
    VOID StartRadio( unsigned int station );

    bool            m_bRadioOn;
    bool            m_bRadioMuted;
    unsigned char   m_ucRadioChannel;
};

#endif
