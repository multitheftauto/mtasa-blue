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
    // CAEAudioHardware::IsSoundBankLoaded
    return ((bool(__thiscall*)(CAEAudioHardwareSAInterface*, unsigned short, short))FUNC_CAEAudioHardware__IsSoundBankLoaded)(m_pInterface, wSoundBankID,
                                                                                                                              wSoundBankSlotID);
}

void CAEAudioHardwareSA::LoadSoundBank(short wSoundBankID, short wSoundBankSlotID)
{
    // CAEAudioHardware::LoadSoundBank
    ((void(__thiscall*)(CAEAudioHardwareSAInterface*, unsigned short, short))FUNC_CAEAudioHardware__LoadSoundBank)(m_pInterface, wSoundBankID,
                                                                                                                   wSoundBankSlotID);
}
