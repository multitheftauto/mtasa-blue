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

#ifndef __CPedSound_H
#define __CPedSound_H

class CPedSoundSAInterface;

class CPedSound
{
public:
    virtual CPedSoundSAInterface*   GetInterface                ( void ) = 0;

    virtual short                   GetVoiceTypeID              ( void ) = 0;
    virtual short                   GetVoiceID                  ( void ) = 0;
    virtual void                    SetVoiceTypeID              ( short sVoiceType ) = 0;
    virtual void                    SetVoiceID                  ( short sVoiceID ) = 0;
    
    virtual bool                    IsSpeechDisabled            ( void ) = 0;
    virtual void                    EnablePedSpeech             ( void ) = 0;
    virtual void                    DisablePedSpeech            ( bool bStopCurrent ) = 0;
};

#endif