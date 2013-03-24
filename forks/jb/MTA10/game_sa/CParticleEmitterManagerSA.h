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

class CParticleEmitterManagerSAInterfaceVTBL
{
public:
    uint32 SCALAR_DELETING_DESTRUCTOR;
    uint32 Init;
    uint32 Unknown2;
    uint32 Unknown3;
    // These call CParticleManager::ActivateParticleEmitter
    uint32 Unknown4;
    uint32 AddParticleEffect;
};

class CParticleEmitterManagerSAInterface
{
public:
    uint32* vtbl;
    class CParticleEmitterBaseDataSAInterface *pParticleEmitter;
    class CParticleFxSAInterface* pParticleFx;
    uint8 bActive;
    uint8 pad1[3];
    uint32 pad2;
};
