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

enum eAudioLookupIndex { FEET = 0, GENRL, PAIN_A, SCRIPT, SPC_EA, SPC_FA, SPC_GA, SPC_NA, SPC_PA };

class CAudioContainer
{
public:
    virtual bool GetAudioData ( eAudioLookupIndex lookupIndex, int bankIndex, int audioIndex, void*& pMemory, unsigned int& length ) = 0;

};

#endif
