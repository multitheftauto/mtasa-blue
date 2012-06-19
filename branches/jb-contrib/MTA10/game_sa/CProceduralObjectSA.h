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


class CProceduralObjectSAInterface
{
public:
    uint8 ucSurfaceIndex;
    uint8 pad1[3];
    uint16 usModelIndex;
    uint8 pad2[2];
    float fSpacing;
    uint32 pad3;
    float fMinDistSquared; // min dist per square unit
    float fMinRot;
    float fMaxRot;
    float fMinScale;
    float fMaxScale;
    float fMinScaleZ;
    float fMaxScaleZ;
    float fZOffMin;
    float fZOffMax;
    uint8 pad4[4];
    uint8 ucUseGrid;
    uint8 pad5[3];
};
