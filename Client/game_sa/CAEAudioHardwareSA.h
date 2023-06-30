/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CAEAudioHardwareSA.h
 *  PURPOSE:     Audio hardware header
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CAEAudioHardware.h>

class CAEAudioHardwareSAInterface {};

class CAEAudioHardwareSA : public CAEAudioHardware
{
public:
    CAEAudioHardwareSA(CAEAudioHardwareSAInterface* pInterface);
    bool IsSoundBankLoaded(short wSoundBankID, short wSoundBankSlotID);
    void LoadSoundBank(short wSoundBankID, short wSoundBankSlotID);

private:
    CAEAudioHardwareSAInterface* m_pInterface;
};
