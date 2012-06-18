/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CFxSA.h
*  PURPOSE:		Game effects handling
*  DEVELOPERS:	Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

// TODO: Add VTBLs and all Activators

// <Particle Emitter Base Class>
class CParticleEmitterSAInterface
{
public:
    uint32* vtbl;
    uint16 usID; // get ID<->Name mapping @ CParticleSystemManagerSA.h
    uint16 pad1;
};
C_ASSERT(sizeof(CParticleEmitterSAInterface) == 0x8);