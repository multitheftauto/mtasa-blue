/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPedModelInfoSA.cpp
*  PURPOSE:     Modelinfo for ped entities
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CPedModelInfoSA::SetMotionAnimGroup ( AssocGroupId animGroup )
{
	DWORD dwThis = (DWORD)m_pInterface;
	DWORD dwFunc = (DWORD)FUNC_SetMotionAnimGroup;
	_asm
	{
		mov		ecx, dwThis
		push	animGroup
		call	dwFunc
	}
}

/* Removed to CModelInfoSA due to interface problems
void CPedModelInfoSA::SetPedVoice ( eVoiceGens VoiceGen, char szVoiceBankFirst[], char szVoiceBankLast[] )
{
    DEBUG_TRACE("void CPedModelInfoSA::SetPedVoice ( eVoiceGens VoiceGen, char szVoiceBankFirst[], char szVoiceBankLast[] )\n");

    DWORD dwFunc = (DWORD)FUNC_CAEPedSpeechAudioEntity__GetVoice;
    DWORD dwBuffer, dwGen;
    short FirstVoice = 0, LastVoice = 0;

    // Get the appropriate FirstVoice index
    dwBuffer    = (DWORD)&szVoiceBankFirst[0];
    dwGen       = (DWORD)VoiceGen;
    _asm
    {
        push    dwBuffer
        push    dwGen
        call    dwFunc
        add		esp, 8
        mov     FirstVoice, ax
    }

    // Get the appropriate LastVoice index
    dwBuffer    = (DWORD)&szVoiceBankLast[0];
    dwGen       = (DWORD)VoiceGen;
    _asm
    {
        push    dwBuffer
        push    dwGen
        call    dwFunc
        add		esp, 8
        mov     LastVoice, ax
    }

    char buf[1024] = {0};
    _snprintf(buf,1023,"First voice: %u\nLast voice: %u\n", FirstVoice, LastVoice);
    OutputDebugString(buf);

    // Set appropriate voices
    GetPedInterface ()->FirstVoice = FirstVoice;
    GetPedInterface ()->LastVoice = LastVoice;
    GetPedInterface ()->NextVoice = FirstVoice;
}

void CPedModelInfoSA::SetPedAudioType ( const char *szPedAudioType )
{
    DEBUG_TRACE("void CPedModelInfoSA::SetPedAudioType ( const char *szPedAudioType )\n");

    DWORD dwFunc = (DWORD)FUNC_CAEPedSpeechAudioEntity__GetAudioPedType;
    DWORD dwBuffer = (DWORD)szPedAudioType;
    short pedAudioType = 0;

    _asm
    {
        push    dwBuffer
        call    dwFunc
        add     esp, 4
        mov     pedAudioType, ax
    }

    char buf[1024] = {0};
    _snprintf(buf,1023,"Audio type: %u\n", pedAudioType);
    OutputDebugString(buf);

    GetPedInterface ()->pedAudioType = pedAudioType;
}
*/
