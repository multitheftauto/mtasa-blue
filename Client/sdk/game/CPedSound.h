/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPedSound.h
 *  PURPOSE:     Header file for ped sound interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CPedSoundSAInterface;

class CPedSound
{
public:
    virtual CPedSoundSAInterface* GetInterface() = 0;

    virtual short GetVoiceTypeID() = 0;
    virtual short GetVoiceID() = 0;
    virtual void  SetVoiceTypeID(short sVoiceType) = 0;
    virtual void  SetVoiceID(short sVoiceID) = 0;

    virtual bool IsSpeechDisabled() = 0;
    virtual void EnablePedSpeech() = 0;
    virtual void DisablePedSpeech(bool bStopCurrent) = 0;
};
