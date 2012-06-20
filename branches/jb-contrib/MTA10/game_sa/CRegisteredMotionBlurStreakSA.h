/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CPathFind.h
*  PURPOSE:     Path finder interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#include <CVector.h>

#define NUM_STREAKS 3
 
class CRegisteredMotionBlurStreakSAInterface
{
public:
        class CEntity* pParent;
        uint8 R, G, B, A;
        CVector FirstPoints[NUM_STREAKS];       // The actual coordinates of one side of the streaks
        CVector SecondPoints[NUM_STREAKS];      // Other side
        uint8    bUsed[NUM_STREAKS];
};
