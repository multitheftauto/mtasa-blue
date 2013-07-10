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
    AUDIO_LOOKUP_SPC_FA, AUDIO_LOOKUP_SPC_GA, AUDIO_LOOKUP_SPC_NA, AUDIO_LOOKUP_SPC_PA };

class CAudioContainer
{
public:
    virtual bool GetAudioData ( eAudioLookupIndex lookupIndex, int bankIndex, int audioIndex, void*& pMemory, unsigned int& length ) = 0;

};

#endif
