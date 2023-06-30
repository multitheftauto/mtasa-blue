/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CAEAudioHardwareSA.cpp
 *  PURPOSE:     Audio hardware
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CAEAudioHardwareSA.h"

CAEAudioHardwareSA::CAEAudioHardwareSA(CAEAudioHardwareSAInterface* pInterface)
{
    m_pInterface = pInterface;
}

bool CAEAudioHardwareSA::IsSoundBankLoaded(short wSoundBankID, short wSoundBankSlotID)
{
    DWORD dwSoundBankID = wSoundBankID;
    DWORD dwSoundBankSlotID = wSoundBankSlotID;
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = 0x4D88C0;
    bool  bReturn = false;
    _asm
    {
        push    dwSoundBankSlotID
        push    dwSoundBankID
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

void CAEAudioHardwareSA::LoadSoundBank(short wSoundBankID, short wSoundBankSlotID)
{
    DWORD dwSoundBankID = wSoundBankID;
    DWORD dwSoundBankSlotID = wSoundBankSlotID;
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = 0x4D88A0;
    _asm
    {
        push    dwSoundBankSlotID
        push    dwSoundBankID
        mov     ecx, dwThis
        call    dwFunc
    }
}
