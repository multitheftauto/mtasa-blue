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

#include "CRenderWareSA.h"

class CRegisteredMotionBlurStreakSAInterface
{
public:
    class CEntity* pEntity;
    RwColor colour;
    CVector vecStart;
    CVector vecUnk1;
    CVector vecUnk2;
    CVector vecEnd;
    CVector vecUnk3;
    CVector vecUnk4;
    uint8 ucExists;
    uint8 pad1[3];
};

