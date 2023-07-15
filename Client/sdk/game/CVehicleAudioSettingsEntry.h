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
    virtual short             GetEngineOnSoundBankID() = 0;
    virtual short             GetEngineOffSoundBankID() = 0;
    virtual char              GetStereo() = 0;
    virtual float             GetUnk3() = 0;
    virtual float             GetUnk4() = 0;
    virtual char              GetHornTon() = 0;
    virtual float             GetHornHign() = 0;
    virtual char              GetUnk6() = 0;
    virtual char              GetDoorSound() = 0;
    virtual char              GetRadioNum() = 0;
    virtual char              GetRadioType() = 0;
    virtual char              GetVehicleTypeForAudio() = 0;
    virtual float             GetHornVolumeDelta() = 0;

    // Set
    virtual void SetSoundType(eVehicleSoundType value) = 0;
    virtual void SetEngineOnSoundBankID(short value) = 0;
    virtual void SetEngineOffSoundBankID(short value) = 0;
    virtual void SetStereo(char value) = 0;
    virtual void SetUnk3(float value) = 0;
    virtual void SetUnk4(float value) = 0;
    virtual void SetHornTon(char value) = 0;
    virtual void SetHornHign(float value) = 0;
    virtual void SetUnk6(char value) = 0;
    virtual void SetDoorSound(char value) = 0;
    virtual void SetRadioNum(char value) = 0;
    virtual void SetRadioType(char value) = 0;
    virtual void SetVehicleTypeForAudio(char value) = 0;
    virtual void SetHornVolumeDelta(float value) = 0;
};
