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

// TODO: Document CMeshExtension sometime

class CBreakableInstanceSAInterface
{
public:
    RwMatrix matrix;
    CVector vecVelocity;
    uint8 pad1[2];
    uint16 pad2;
    uint32 pad3;
    RwTexture* pTexture;
    uint8 pad4[4];
    uint32 pad5[2];
    CVector vecUnk;
    uint32 pad6;
};

class CBreakableInfoSAInterface
{
public:
    uint8 pad1;
    uint8 bActive;
    uint8 bSparksOnImpact;
    uint8 bDoStream;
    uint8 ucBreakableCount;
    CBreakableInstance* pInst;
    uint32 pad2[2];
    CVector vecUnk;
};
