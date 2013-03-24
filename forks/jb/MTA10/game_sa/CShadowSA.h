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

class CShadowSAInterface
{
public:
    CVector vecCenter;
    float fSpreadXMin;
    float fSpreadYMin;
    float fSpreadXMax;
    float fSpreadYMax;
    float fRangeZ;
    float fOffset;
    class RwTexture* pTexture;
    class CShadowDataSAInterface* pShadowData;
    uint16 usIntensity;
    uint8 ucColorScheme;
    uint8 ucR;
    uint8 ucG;
    uint8 ucB;
    uint16 usFlags;
    uint32 pad1;
};

