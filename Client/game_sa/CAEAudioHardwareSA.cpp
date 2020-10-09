/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAEAudioHardwareSA.cpp
 *  PURPOSE:     Audio hardware
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CAEAudioHardwareSA::CAEAudioHardwareSA(CAEAudioHardwareSAInterface* pInterface)
{
    m_pInterface = pInterface;
}

bool CAEAudioHardwareSA::IsSoundBankLoaded(short wSoundBankID, short wSoundBankSlotID)
{
    return ((bool(__thiscall*)(void*, short, short))FUNC_CAEAudioHardware__IsSoundBankLoaded)((void*)m_pInterface, wSoundBankID, wSoundBankSlotID);
}

bool CAEAudioHardwareSA::LoadSoundBank(short wSoundBankID, short wSoundBankSlotID)
{
    return ((bool(__thiscall*)(void*, short, short))FUNC_CAEAudioHardware__LoadSoundBank)((void*)m_pInterface, wSoundBankID, wSoundBankSlotID);
}
