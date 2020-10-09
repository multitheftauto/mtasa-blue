/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAEAudioHardwareSA.h
 *  PURPOSE:     Audio hardware header
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Common.h"
#include <game/CAEAudioHardware.h>

#define FUNC_CAEAudioHardware__IsSoundBankLoaded                            0x4D88C0
#define FUNC_CAEAudioHardware__LoadSoundBank                                0x4D88A0

#define CLASS_CAEAudioHardware                                              0xB5F8B8

struct CAEAudioHardwareSAInterface
{
    BYTE field_0;
    char m_bDisableEffectsLoading;
};

class CAEAudioHardwareSA : public CAEAudioHardware
{
public:
    CAEAudioHardwareSA(CAEAudioHardwareSAInterface* pInterface);
    bool IsSoundBankLoaded(short wSoundBankID, short wSoundBankSlotID);
    bool LoadSoundBank(short wSoundBankID, short wSoundBankSlotID);
    bool IsEffectsLoadingDisabled() const { return m_pInterface->m_bDisableEffectsLoading; }

private:
    CAEAudioHardwareSAInterface* m_pInterface;
};
