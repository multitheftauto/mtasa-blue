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

// The part !!ABOVE!! FX_PRIM_EMITTER_DATA in effects.fxp
class CParticleHeaderSAInterface
{
public:
    uint32 pad1;
    uint32 pad2;
    class CParticleFxSAInterface* pParticleFx;
    float fLength;
    float fIntervalMin;
    float fLength2;
    uint16 usCullDistance;
    uint8 ucPlayMode;
    uint8 ucParticleEmittersCount;
    uint32 pParticleEmitters; // TODO: Clarify
    RwSphere* pBoundingSphere;
};
