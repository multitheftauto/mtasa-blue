/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CCheckpoints.h
*  PURPOSE:     Checkpoint entity manager interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_CHECKPOINTS
#define __CGAME_CHECKPOINTS

#include <CVector.h>
#include "CCheckpoint.h"

class CCheckpoints
{
public:
    virtual CCheckpoint     * CreateCheckpoint(DWORD Identifier, WORD wType, CVector * vecPosition, CVector * vecPointDir, FLOAT fSize, FLOAT fPulseFraction, const SColor color)=0;
    virtual CCheckpoint     * FindFreeMarker()=0;
};

#endif
