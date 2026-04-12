/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAEAudioHardwareSA.cpp
 *  PURPOSE:     Audio hardware
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
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
    DWORD dwFunc = FUNC_CAEAudioHardware__IsSoundBankLoaded;
    bool  bReturn = false;
    // clang-format off
    using func_t = decltype(bReturn) (__thiscall*)(decltype(dwThis), decltype(dwSoundBankID), decltype(dwSoundBankSlotID));
bReturn =     reinterpret_cast<func_t>(dwFunc)(dwThis, dwSoundBankID, dwSoundBankSlotID);
    // clang-format on
    return bReturn;
}

void CAEAudioHardwareSA::LoadSoundBank(short wSoundBankID, short wSoundBankSlotID)
{
    DWORD dwSoundBankID = wSoundBankID;
    DWORD dwSoundBankSlotID = wSoundBankSlotID;
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CAEAudioHardware__LoadSoundBank;
    // clang-format off
    using func_t = void (__thiscall*)(decltype(dwThis), decltype(dwSoundBankID), decltype(dwSoundBankSlotID));
    reinterpret_cast<func_t>(dwFunc)(dwThis, dwSoundBankID, dwSoundBankSlotID);
    // clang-format on
}
