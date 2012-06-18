/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CParticleSystemManagerSA.h
*  PURPOSE:     Header file for particle system manager class
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#include "CRenderWareSA.h"

class CParticleStorageSAInterface
{
public:
    uint32 pad1;
    class CParticleStorageSAInterface* pNext;
    float fDuration;
    float fBlur;
    CVector vecUnk;
    CVector vecDirection;
    class CParticleFxSAInterface* pParticleFx;
    RwColor colour;
    uint8 pad2[5];
    uint8 ucBrightness;
    uint8 pad3[3];
};
// C_ASSERT(sizeof(CParticleStorageSAInterface) == 0x???); // needs checking

