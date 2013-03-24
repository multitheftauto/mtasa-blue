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

// calls particle emitters' VTBL + 0x8(CParticleEmitter::Activate)
#define FUNC_CParticleManagerSA__ActivateParticleEmitter                    0x4A4960

class CParticleManagerSAInterface
{
public:
    uint32 count;
    /***
     *  ACTUAL; Particle Emitter
     *              - (at ctor) calls Particle Activator
     *  Particle Emitters have base in CParticleEmitterSAInterface* and extends it.
     */
    class CParticleEmitterSAInterface** pParticleEmitters; // array of CParticleEmitterSAInterface*
    uint32 pad2;
    uint16 usLodStart;
    uint16 usLodEnd;
    uint8 bHasFlatParticleEmitter;
    uint8 bHasHeatHazeParticleEmitter;
    uint8 pad6;
    uint8 pad7;
};
C_ASSERT(sizeof(CParticleManagerSAInterface) == 0x14);
