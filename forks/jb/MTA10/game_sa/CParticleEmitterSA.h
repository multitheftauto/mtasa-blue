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

#include "CParticleActivatorSA.h"

// TODO: Add VTBLs and all Activators
class CParticleEmitterSAInterfaceVTBL
{
public:
    uint32 SCALAR_DELETING_DESTRUCTOR;
    uint32 ReadData;
    uint32 Activate;
};

// <Particle Emitter Base Class>
class CParticleEmitterSAInterface
{
public:
    CParticleEmitterSAInterfaceVTBL* vtbl;
    uint16 usID; // get ID<->Name mapping @ CParticleSystemManagerSA.h
    uint8 ucTimemodePrt;
    uint8 pad1;
    CParticleActivatorSAInterface Activator;
};
C_ASSERT(sizeof(CParticleEmitterSAInterface) == 0x18);
