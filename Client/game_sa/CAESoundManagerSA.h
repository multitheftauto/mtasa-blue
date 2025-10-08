/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAESoundManagerSA.h
 *  PURPOSE:     Header file for audio engine sound manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <game/CAESoundManager.h>
#include "CAudioEngineSA.h"

#define CLASS_CAESoundManager                           0xB62CB0

class CAESoundManagerSAInterface
{
    int16_t  m_wNumAvailableChannels;                 // + 0x0000 // = CAEAudioHardware::GetNumAvailableChannels(...), [10, 300]
    int16_t  m_wChannel;                              // + 0x0002 // = CAEAudioHardware::AllocateChannels(...), could be -1
    CAESound m_aSound[300];                           // + 0x0004
    int16_t* m_aChannelSoundTable;                    // + 0x87F4 // = new short[m_wNumAvailableChannels]
    int16_t* m_aChannelSoundPosition;                 // + 0x87F8 // = new short[m_wNumAvailableChannels]
    int16_t* m_aChannelSoundUncancellable;            // + 0x87FC // = new short[m_wNumAvailableChannels]
    int16_t  m_wSoundLength[300];                     // + 0x8800 // = -1 (0xFFFF) when initialized
    int16_t  m_wSoundLoopStartTime[300];              // + 0x8A58 // = -1 (0xFFFF) when initialized
    uint32_t m_uiUpdateTime;                          // + 0x8CB0
    int8_t   m_bPauseTimeInUse;                       // + 0x8CB4
    int8_t   m_bPaused;                               // + 0x8CB5
    int8_t   field_8CB6;
    int8_t   field_8CB7;
    uint32_t m_uiPauseUpdateTime;            // + 0x8CB8
};
static_assert(sizeof(CAESoundManagerSAInterface) == 0x8CBC, "Invalid size for CAESoundManagerSAInterface");

class CAESoundManagerSA : public CAESoundManager
{
public:
    CAESoundManagerSA(CAESoundManagerSAInterface* pInterface);

public:
    virtual void CancelSoundsInBankSlot(uint uiGroup, uint uiIndex) override;

private:
    CAESoundManagerSAInterface* m_pInterface;
};
