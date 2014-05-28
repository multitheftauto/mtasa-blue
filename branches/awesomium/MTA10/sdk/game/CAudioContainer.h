/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CAudioContainer.h
*  PURPOSE:     Audio container reader interface
*  DEVELOPERS:  Jusonex <jusonex96@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CAUDIOCONTAINER_H
#define __CAUDIOCONTAINER_H

#include "Common.h"

enum eAudioLookupIndex { AUDIO_LOOKUP_FEET = 0, AUDIO_LOOKUP_GENRL, AUDIO_LOOKUP_PAIN_A, AUDIO_LOOKUP_SCRIPT, AUDIO_LOOKUP_SPC_EA,
    AUDIO_LOOKUP_SPC_FA, AUDIO_LOOKUP_SPC_GA, AUDIO_LOOKUP_SPC_NA, AUDIO_LOOKUP_SPC_PA, AUDIO_LOOKUP_RADIO };

enum eRadioStreamIndex { 
    RADIO_STREAM_POLICE, RADIO_STREAM_ADVERTS, RADIO_STREAM_AMBIENCE, RADIO_STREAM_BEATS, RADIO_STREAM_PLAYBACK, RADIO_STREAM_KROSE, RADIO_STREAM_KDST,
    RADIO_STREAM_CUTSCENE, RADIO_STREAM_BOUNCE, RADIO_STREAM_SFUR, RADIO_STREAM_RADIO_LOS_SANTOS, RADIO_STREAM_RADIO_X, RADIO_STREAM_CSR,
    RADIO_STREAM_KJAH, RADIO_STREAM_MASTER_SOUNDS, RADIO_STREAM_WCTR };

class CAudioContainer
{
public:
    virtual bool GetAudioData ( eAudioLookupIndex lookupIndex, int bankIndex, int audioIndex, void*& pMemory, unsigned int& length ) = 0;
    virtual bool ValidateContainer ( eAudioLookupIndex lookupIndex ) = 0;

    virtual bool GetRadioAudioData(eRadioStreamIndex streamIndex, int trackIndex, void*& pMemory, unsigned int& length) = 0;
};

#endif
