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
    __asm
    {
        push    dwSoundBankSlotID
        push    dwSoundBankID
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }
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
    __asm
    {
        push    dwSoundBankSlotID
        push    dwSoundBankID
        mov     ecx, dwThis
        call    dwFunc
    }
    // clang-format on
}

namespace
{
    constexpr DWORD NUM_AudioHardwareBankLoaderOffset = 0xD98;

    constexpr DWORD NUM_AudioHardwareNumChannelsOffset = 0x8E;
    constexpr DWORD NUM_AudioHardwareFreqScalingOffset = 0x310;

    constexpr DWORD NUM_BankLoaderSlotsOffset = 0x00;
    constexpr DWORD NUM_BankLoaderBankLkupsOffset = 0x04;
    constexpr DWORD NUM_BankLoaderSlotCountOffset = 0x0C;
    constexpr DWORD NUM_BankLoaderBankLkupCntOffset = 0x0E;
    constexpr DWORD NUM_BankLoaderBufferSizeOffset = 0x18;
    constexpr DWORD NUM_BankLoaderBufferOffset = 0x1C;

    constexpr DWORD NUM_BankSlotSize = 0x12D4;
    constexpr DWORD NUM_BankSlotOffsetBytesOffset = 0x00;
    constexpr DWORD NUM_BankSlotNumBytesOffset = 0x04;
    constexpr DWORD NUM_BankSlotBankIdOffset = 0x10;
    constexpr DWORD NUM_BankSlotNumSoundsOffset = 0x12;
    constexpr DWORD NUM_BankSlotSoundsArrayOffset = 0x14;

    constexpr DWORD NUM_BankSlotItemSize = 0x0C;
    constexpr DWORD NUM_BankSlotItemBufferOffsetOffset = 0x00;
    constexpr DWORD NUM_BankSlotItemLoopOffsetOffset = 0x04;
    constexpr DWORD NUM_BankSlotItemSampleFreqOffset = 0x08;

    constexpr DWORD NUM_BankLookupSize = 0x0C;
    constexpr DWORD NUM_BankLookupNumBytesOffset = 0x08;

    constexpr uint NUM_MaxBankSounds = 400;

    const BYTE* GetBankLoader()
    {
        const BYTE* pAudioHardware = reinterpret_cast<const BYTE*>(CLASS_CAEAudioHardware);
        return *reinterpret_cast<BYTE* const*>(pAudioHardware + NUM_AudioHardwareBankLoaderOffset);
    }

    const BYTE* GetBankSlot(uint usBankSlot)
    {
        const BYTE* pBankLoader = GetBankLoader();
        if (!pBankLoader)
            return nullptr;

        const ushort usSlotCount = *reinterpret_cast<const ushort*>(pBankLoader + NUM_BankLoaderSlotCountOffset);
        if (usBankSlot >= usSlotCount)
            return nullptr;

        const BYTE* pBankSlots = *reinterpret_cast<BYTE* const*>(pBankLoader + NUM_BankLoaderSlotsOffset);
        if (!pBankSlots)
            return nullptr;

        return pBankSlots + usBankSlot * NUM_BankSlotSize;
    }

    const BYTE* GetBankSlotData(const BYTE* pBankSlot)
    {
        const BYTE* pBankLoader = GetBankLoader();
        if (!pBankLoader)
            return nullptr;

        const BYTE* pBuffer = *reinterpret_cast<BYTE* const*>(pBankLoader + NUM_BankLoaderBufferOffset);
        if (!pBuffer)
            return nullptr;

        const uint uiBufferSize = *reinterpret_cast<const uint*>(pBankLoader + NUM_BankLoaderBufferSizeOffset);
        const uint uiOffsetBytes = *reinterpret_cast<const uint*>(pBankSlot + NUM_BankSlotOffsetBytesOffset);
        const uint uiNumBytes = *reinterpret_cast<const uint*>(pBankSlot + NUM_BankSlotNumBytesOffset);

        if (uiOffsetBytes + uiNumBytes > uiBufferSize || uiNumBytes == 0)
            return nullptr;

        return pBuffer + uiOffsetBytes;
    }

    const BYTE* GetBankSlotItem(const BYTE* pBankSlot, uint usIndex)
    {
        return pBankSlot + NUM_BankSlotSoundsArrayOffset + usIndex * NUM_BankSlotItemSize;
    }

    uint GetBankPcmSize(const BYTE* pBankSlot)
    {
        const BYTE* pBankLoader = GetBankLoader();
        if (!pBankLoader)
            return 0;

        const BYTE* pBankLkups = *reinterpret_cast<BYTE* const*>(pBankLoader + NUM_BankLoaderBankLkupsOffset);
        if (!pBankLkups)
            return 0;

        const ushort usBankLkupCnt = *reinterpret_cast<const ushort*>(pBankLoader + NUM_BankLoaderBankLkupCntOffset);
        const short  sBankId = *reinterpret_cast<const short*>(pBankSlot + NUM_BankSlotBankIdOffset);
        if (sBankId < 0 || sBankId >= static_cast<short>(usBankLkupCnt))
            return 0;

        return *reinterpret_cast<const uint*>(pBankLkups + sBankId * NUM_BankLookupSize + NUM_BankLookupNumBytesOffset);
    }
}

bool CAEAudioHardwareSA::GetLoadedSoundInfo(unsigned short usBankSlot, unsigned short usIndex, void*& pOutPcmData, unsigned int& uiOutPcmSize,
                                            unsigned int& uiOutSampleRate, int& iOutLoopStartOffset) const
{
    pOutPcmData = nullptr;
    uiOutPcmSize = 0;
    uiOutSampleRate = 0;
    iOutLoopStartOffset = -1;

    const BYTE* pBankSlot = GetBankSlot(usBankSlot);
    if (!pBankSlot)
        return false;

    const short sNumSounds = *reinterpret_cast<const short*>(pBankSlot + NUM_BankSlotNumSoundsOffset);
    if (sNumSounds < 0 || usIndex >= static_cast<uint>(sNumSounds) || usIndex >= NUM_MaxBankSounds)
        return false;

    const BYTE* pSlotData = GetBankSlotData(pBankSlot);
    if (!pSlotData)
        return false;

    const uint uiNumBytes = *reinterpret_cast<const uint*>(pBankSlot + NUM_BankSlotNumBytesOffset);

    const BYTE* pItem = GetBankSlotItem(pBankSlot, usIndex);
    const uint  uiBufferOffset = *reinterpret_cast<const uint*>(pItem + NUM_BankSlotItemBufferOffsetOffset);

    uint uiSize = 0;
    if (usIndex + 1 < static_cast<uint>(sNumSounds))
    {
        const uint uiNextOffset = *reinterpret_cast<const uint*>(GetBankSlotItem(pBankSlot, usIndex + 1) + NUM_BankSlotItemBufferOffsetOffset);
        if (uiNextOffset <= uiBufferOffset)
            return false;
        uiSize = uiNextOffset - uiBufferOffset;
    }
    else
    {
        const uint uiBankPcmSize = GetBankPcmSize(pBankSlot);
        if (uiBankPcmSize > 0)
        {
            if (uiBufferOffset >= uiBankPcmSize)
                return false;
            uiSize = uiBankPcmSize - uiBufferOffset;
        }
        else
        {
            if (uiBufferOffset >= uiNumBytes)
                return false;
            uiSize = uiNumBytes - uiBufferOffset;
        }
    }

    const BYTE* pPcmData = pSlotData + uiBufferOffset;
    if (pPcmData + uiSize > pSlotData + uiNumBytes)
        return false;

    pOutPcmData = const_cast<BYTE*>(pPcmData);
    uiOutPcmSize = uiSize;
    uiOutSampleRate = *reinterpret_cast<const ushort*>(pItem + NUM_BankSlotItemSampleFreqOffset);
    iOutLoopStartOffset = *reinterpret_cast<const int*>(pItem + NUM_BankSlotItemLoopOffsetOffset);
    return true;
}

void CAEAudioHardwareSA::GetChannelFrequencyScalingFactors(float* pOutFactors, unsigned int uiMax) const
{
    if (!pOutFactors || uiMax == 0)
        return;

    const BYTE*  pBase = reinterpret_cast<const BYTE*>(m_pInterface);
    const ushort usNumChannels = *reinterpret_cast<const ushort*>(pBase + NUM_AudioHardwareNumChannelsOffset);
    const uint   uiCount = std::min<uint>(uiMax, usNumChannels);
    const float* pFactors = reinterpret_cast<const float*>(pBase + NUM_AudioHardwareFreqScalingOffset);

    for (uint i = 0; i < uiCount; ++i)
        pOutFactors[i] = pFactors[i];
}

bool CAEAudioHardwareSA::PatchSoundBuffer(unsigned short usBankSlot, unsigned short usIndex, const void* pPcmData, unsigned int uiDataSize)
{
    void* pPcmDataDst = nullptr;
    uint  uiPcmSize = 0;
    uint  uiSampleRate = 0;
    int   iLoopStartOffset = -1;

    if (!GetLoadedSoundInfo(usBankSlot, usIndex, pPcmDataDst, uiPcmSize, uiSampleRate, iLoopStartOffset))
        return false;

    if (!pPcmData || uiDataSize > uiPcmSize)
        return false;

    memcpy(pPcmDataDst, pPcmData, uiDataSize);
    if (uiDataSize < uiPcmSize)
    {
        BYTE* pPad = static_cast<BYTE*>(pPcmDataDst) + uiDataSize;
        uint  uiPadSize = uiPcmSize - uiDataSize;
        if (iLoopStartOffset >= 0)
        {
            while (uiPadSize > 0)
            {
                const uint uiCopy = std::min(uiPadSize, uiDataSize);
                memcpy(pPad, pPcmData, uiCopy);
                pPad += uiCopy;
                uiPadSize -= uiCopy;
            }
        }
        else
        {
            memset(pPad, 0, uiPadSize);
        }
    }

    return true;
}
