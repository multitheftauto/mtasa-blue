/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CAudioSA.cpp
*  PURPOSE:     Audio manager
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

VOID CAudioSA::StopRadio()
{
    //DWORD dwFunc = FUNC_StopRadio;
    DWORD dwFunc = 0x4E9823; // Some function CAudio::StopRadio jumps to immediately

    _asm 
    {
        // This doesn't work anymore because we've
        // returned out the function.
        /*
        push    0
        push    0
        mov     ecx, CLASS_CAudioEngine
        call    dwFunc
        */

        // Push our arguments onto the stack and emulate a "call" instruction
        // by pushing the return position on the stack aswell.
        push        0
        push        0
        push        retpoint

        // Do what CAudio::StopRadio does. Mov the AERadioTrackManager class instance into 'ecx' (this)
        mov         ecx, CLASS_AERadioTrackManager  

        // Do what this global func we've removed does in the beginning.
        push        ebx  
        push        ebp  
        push        esi

        // Jump to behind the return code we've replaced.
        jmp         dwFunc

        retpoint:
    }
}

VOID CAudioSA::StartRadio(unsigned int station)
{
    DWORD dwFunc = 0x4DBEC3;
    DWORD dwFunc2 = 0x4EB3C3;
    _asm 
    {
        // We can't do this anymore as we've returned out StartRadio
        /*
        push    0
        push    station
        mov     ecx, CLASS_CAudioEngine
        call    dwFunc
        */

        // Push our arguments onto the stack
        push        0
        push        station

        // Call something, skip 3 bytes that we have our return instruction on (no arguments)
        mov         ecx,CLASS_AECutsceneTrackManager 
        mov         eax,dword ptr [ecx+8] 
        call        dwFunc

        // Check the return value, eventually skip
        test        al,al 
        jne         skip

        mov         eax,dword ptr [esp+4] 
        mov         ecx,dword ptr [esp] 

        // Push arguments to some other function
        push        0
        push        0
        push        eax  
        push        ecx  

        // Call it (emulate call instruction)
        mov         ecx,8CB6F8h 
        push        done
        push        ebx  
        mov         bl,byte ptr [esp+8] 
        jmp         dwFunc2

        // Pop our arguments back
        done:
        pop         eax
        pop         eax

        skip:
    }
}

// 43 = race one
// 32 = help
// 13 = camera take picture
VOID CAudioSA::PlayFrontEndSound(DWORD dwSound)
{
    if  ( *(DWORD *)VAR_AudioEventVolumes != 0 && dwSound <= 101 ) // may prevent a crash
    {
        DEBUG_TRACE("VOID CAudioSA::PlayFrontEndSound(DWORD dwSound)");
        DWORD dwFunc = FUNC_ReportFrontendAudioEvent;
        FLOAT fUnknown = 1.0f;
        _asm
        {
            push    fUnknown
            push    0
            push    dwSound
            mov     ecx, CLASS_CAudioEngine
            call    dwFunc
        }
    }

/*  DWORD dwAudioEntity = 0xB6BC90;

    DWORD dwFunc = 0x507290;
    _asm
    {
        push    1
        push    dwSound
        call    dwFunc
    }

        
    dwFunc = 0x5072B0;
    _asm
    {
        push    dwSound
        mov     ecx,dwAudioEntity
        call    dwFunc
    }*/
}

VOID CAudioSA::SetEffectsMasterVolume ( BYTE bVolume )
{
    DWORD dwFunc = FUNC_SetEffectsMasterVolume;
    DWORD dwVolume = bVolume;
    _asm
    {
        mov     ecx, CLASS_CAudioEngine
        push    dwVolume
        call    dwFunc
    }
}

VOID CAudioSA::SetMusicMasterVolume ( BYTE bVolume )
{
    DWORD dwFunc = FUNC_SetMusicMasterVolume;
    DWORD dwVolume = bVolume;
    _asm
    {
        mov     ecx, CLASS_CAudioEngine
        push    dwVolume
        call    dwFunc
    }
}

VOID CAudioSA::PlayBeatTrack ( short iTrack )
{
    if  ( *(DWORD *)VAR_AudioEventVolumes != 0 ) // may prevent a crash
    {
        DEBUG_TRACE("VOID CAudioSA::PlayBeatTrack ( short iTrack )");
        DWORD dwFunc = FUNC_PreloadBeatTrack;
        DWORD dwTrack = iTrack;
        _asm
        {
            mov     ecx, CLASS_CAudioEngine
            push    dwTrack
            call    dwFunc
        }

        dwFunc = FUNC_PlayPreloadedBeatTrack;
        _asm
        {
            mov     ecx, CLASS_CAudioEngine
            push    1
            call    dwFunc
        }
    }
}

VOID CAudioSA::ClearMissionAudio ( int slot )
{
    DWORD dwFunc = 0x5072F0; // CAudioEngine::ClearMissionAudio(unsigned char)
    _asm
    {
        mov     ecx, CLASS_CAudioEngine
        push    slot // sound bank slot?
        call    dwFunc
    }
}

bool CAudioSA::IsMissionAudioSampleFinished ( int slot )
{
    DWORD dwFunc = 0x5072C0; // CAudioEngine::IsMissionAudioSampleFinished
    bool cret = 0;
    _asm
    {
        mov     ecx, CLASS_CAudioEngine
        push    slot
        call    dwFunc
        mov     cret, al
    }
    return cret;
}

VOID CAudioSA::PreloadMissionAudio ( unsigned short usAudioEvent, int slot )
{
    DWORD dwFunc = 0x507290; // CAudioEngine__PreloadMissionAudio
    DWORD AudioEvent = usAudioEvent;
    _asm
    {
        mov     ecx, CLASS_CAudioEngine
        push    AudioEvent
        push    slot
        call    dwFunc
    }
}

unsigned char CAudioSA::GetMissionAudioLoadingStatus ( int slot )
{
    DWORD dwFunc = 0x5072A0; // get load status
    unsigned char cret = 0;
    _asm
    {
        mov     ecx, CLASS_CAudioEngine
        push    slot
        call    dwFunc
        mov     cret, al
    }
    return cret;
}

VOID CAudioSA::AttachMissionAudioToPhysical ( CPhysical * physical, int slot )
{
    CEntitySAInterface * entity = NULL;
    if ( physical )
    {
        CPhysicalSA* pPhysical = dynamic_cast < CPhysicalSA* > ( physical );
        if ( pPhysical )
            entity = pPhysical->GetInterface ();
    }
    
    DWORD dwFunc = 0x507330; // AttachMissionAudioToPhysical
    _asm
    {
        mov     ecx, CLASS_CAudioEngine
        push    entity
        push    slot
        call    dwFunc
    }
}

VOID CAudioSA::SetMissionAudioPosition ( CVector * position, int slot )
{
    DWORD dwFunc = 0x507300; // CAudioEngine__SetMissionAudioPosition
    _asm
    {
        mov     ecx, CLASS_CAudioEngine
        push    position
        push    slot
        call    dwFunc
    }
}

bool CAudioSA::PlayLoadedMissionAudio ( int slot )
{
    if ( GetMissionAudioLoadingStatus(slot) == 1 )
    {
        DWORD dwFunc = 0x5072B0; // CAudioEngine::PlayLoadedMissionAudio(unsigned char)
        _asm    
        {
            mov     ecx, CLASS_CAudioEngine
            push    slot
            call    dwFunc
        }
        return true;
    }
    return false;
}

VOID CAudioSA::PauseAllSound ( bool bPaused )
{
    if ( bPaused )
    {
        DWORD dwFunc = FUNC_PauseAllSounds;
        _asm    
        {
            mov     ecx, CLASS_CAudioEngine
            call    dwFunc
        }
    }
    else
    {
        DWORD dwFunc = FUNC_ResumeAllSounds;
        _asm    
        {
            mov     ecx, CLASS_CAudioEngine
            call    dwFunc
        }
    }
}
