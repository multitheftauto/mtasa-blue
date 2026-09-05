/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAEAudioHardwareSA.h
 *  PURPOSE:     Audio hardware header
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CAEAudioHardware.h>

#define FUNC_CAEAudioHardware__IsSoundBankLoaded 0x4D88C0
#define FUNC_CAEAudioHardware__LoadSoundBank     0x4D88A0
#define FUNC_CAEAudioHardware__Terminate         0x4D97A0
#define FUNC_CAEAudioHardware__Initialise        0x4D9930

#define CLASS_CAEAudioHardware 0xB5F8B8

class CAEAudioHardwareSAInterface
{
};

class CAEAudioHardwareSA : public CAEAudioHardware
{
public:
    CAEAudioHardwareSA(CAEAudioHardwareSAInterface* pInterface);
    bool IsSoundBankLoaded(short wSoundBankID, short wSoundBankSlotID);
    void LoadSoundBank(short wSoundBankID, short wSoundBankSlotID);
    void Terminate();
    bool Initialise();

private:
    CAEAudioHardwareSAInterface* m_pInterface;
};
