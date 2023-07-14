/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CHandlingEntry.h
 *  PURPOSE:     Vehicle handling entry interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Common.h"

class CVehicleAudioSettingsEntry
{
public:
    // Destructor
    virtual ~CVehicleAudioSettingsEntry(){};

    // Use this to copy data from an another handling class to this
    virtual void Assign(const CVehicleAudioSettingsEntry* pData) = 0;

    // Get
    virtual eVehicleSoundType GetSoundType() = 0;
    virtual uchar             GetEngineOnSoundBankID() = 0;
    virtual uchar             GetEngineOffSoundBankID() = 0;
    virtual uchar             GetStereo() = 0;
    virtual float             GetUnk3() = 0;
    virtual float             GetUnk4() = 0;
    virtual uchar             GetHornTon() = 0;
    virtual float             GetHornHign() = 0;
    virtual uchar             GetUnk6() = 0;
    virtual uchar             GetDoorSound() = 0;
    virtual uchar             GetRadioNum() = 0;
    virtual uchar             GetRadioType() = 0;
    virtual uchar             GetVehicleTypeForAudio() = 0;
    virtual float             GetHornVolumeDelta() = 0;

    // Set
    virtual void SetSoundType(eVehicleSoundType value) = 0;
    virtual void SetEngineOnSoundBankID(uchar value) = 0;
    virtual void SetEngineOffSoundBankID(uchar value) = 0;
    virtual void SetStereo(uchar value) = 0;
    virtual void SetUnk3(float value) = 0;
    virtual void SetUnk4(float value) = 0;
    virtual void SetHornTon(uchar value) = 0;
    virtual void SetHornHign(float value) = 0;
    virtual void SetUnk6(uchar value) = 0;
    virtual void SetDoorSound(uchar value) = 0;
    virtual void SetRadioNum(uchar value) = 0;
    virtual void SetRadioType(uchar value) = 0;
    virtual void SetVehicleTypeForAudio(uchar value) = 0;
    virtual void SetHornVolumeDelta(float value) = 0;
};
