/*****************************************************************************
 *
 *  PROJECT:        Multi Theft Auto v1.0
 *  LICENSE:        See LICENSE in the top level directory
 *  FILE:           sdk/game/CAEVehicleAudioEntity.h
 *  PURPOSE:        Vehicle audio interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CAEVehicleAudioEntity
{
public:
    virtual void JustGotInVehicleAsDriver() = 0;
    virtual void JustGotOutOfVehicleAsDriver() = 0;
    virtual void TurnOnRadioForVehicle() = 0;

    virtual void SetVehicleSoundType(enum eVehicleAudioType type) = 0;

    virtual unsigned int SetEngineAccelerationSoundBank(unsigned int val) = 0;
    virtual unsigned int GetEngineAccelerationSoundBank() const = 0;

    virtual unsigned int SetEngineDeaccelerationSoundBank(unsigned int val) = 0;
};
