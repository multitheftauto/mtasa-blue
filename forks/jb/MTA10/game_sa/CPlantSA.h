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

class CPlantSAInterface
{
public:
    uint16 usPlantSlotID;
    uint16 usModelIndex;
    uint16 UVOffset;
    uint8 ucRed;
    uint8 ucGreen;
    uint8 ucBlue;
    uint8 ucIntensity;
    uint8 ucVariation;
    uint8 ucAlpha;
    float fSclVarXY;
    float fSclVarZ;
    float fBendScl;
    float fWBendVar;
    float fDensity;
    uint32 pad1[23];
};
