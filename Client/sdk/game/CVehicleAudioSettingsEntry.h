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

#include "enums/VehicleSoundType.h"

class CVehicleAudioSettingsEntry
{
public:
    // Destructor
    virtual ~CVehicleAudioSettingsEntry(){}

    // Get
    virtual VehicleSoundType  GetSoundType() const noexcept = 0;
    virtual short             GetEngineOnSoundBankID() const noexcept = 0;
    virtual short             GetEngineOffSoundBankID() const noexcept = 0;
    virtual char              GetBassSetting() const noexcept = 0;
    virtual float             GetBassEq() const noexcept = 0;
    virtual float             GetFieldC() const noexcept = 0;
    virtual char              GetHornTon() const noexcept = 0;
    virtual float             GetHornHign() const noexcept = 0;
    virtual char              GetEngineUpgrade() const noexcept = 0;
    virtual char              GetDoorSound() const noexcept = 0;
    virtual char              GetRadioNum() const noexcept = 0;
    virtual char              GetRadioType() const noexcept = 0;
    virtual char              GetVehicleTypeForAudio() const noexcept = 0;
    virtual float             GetHornVolumeDelta() const noexcept = 0;

    // Set
    virtual void SetSoundType(VehicleSoundType value) noexcept = 0;
    virtual void SetEngineOnSoundBankID(short value) noexcept = 0;
    virtual void SetEngineOffSoundBankID(short value) noexcept = 0;
    virtual void SetBassSetting(char value) noexcept = 0;
    virtual void SetBassEq(float value) noexcept = 0;
    virtual void SetFieldC(float value) noexcept = 0;
    virtual void SetHornTon(char value) noexcept = 0;
    virtual void SetHornHign(float value) noexcept = 0;
    virtual void SetEngineUpgrade(char value) noexcept = 0;
    virtual void SetDoorSound(char value) noexcept = 0;
    virtual void SetRadioNum(char value) noexcept = 0;
    virtual void SetRadioType(char value) noexcept = 0;
    virtual void SetVehicleTypeForAudio(char value) noexcept = 0;
    virtual void SetHornVolumeDelta(float value) noexcept = 0;
};
