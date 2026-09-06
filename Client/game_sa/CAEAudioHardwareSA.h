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

    bool GetLoadedSoundInfo(unsigned short usBankSlot, unsigned short usIndex, void*& pOutPcmData, unsigned int& uiOutPcmSize, unsigned int& uiOutSampleRate,
                            int& iOutLoopStartOffset) const override;
    bool PatchSoundBuffer(unsigned short usBankSlot, unsigned short usIndex, const void* pPcmData, unsigned int uiDataSize) override;
    void GetChannelFrequencyScalingFactors(float* pOutFactors, unsigned int uiMax) const override;

private:
    CAEAudioHardwareSAInterface* m_pInterface;
};
