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

#include "CParticleManagerSA.h"

class CParticleEmitterBaseDataSAInterfaceVTBL
{
public:
    uint32 SCALAR_DELETING_DESTRUCTOR;
    uint32 LoadData_LOD;
    uint32 ReadTextures;
    uint32 AllocateParticleEmitterManager;
    uint32 Unknown1;
    uint32 RenderAll;
    uint32 FindParticleFx_InParticleStorageList;
};

class CParticleStorageListSAInterface
{
public:
    class CParticleStorageSAInterface* pHead;
    uint32 pad1;
    uint32 pad2;
};

// FX_PRIM_BASE_DATA section in effects.fxp
class CParticleEmitterBaseDataSAInterface
{
public:
    CParticleEmitterBaseDataSAInterface* vtbl;
    uint8 pad1;
    uint8 ucSrcBlendID; // D3DBLEND ; do not use that in here because we need
    uint8 ucDestBlendID; // D3DBLEND ; byte and D3DBLEND is forced DWORD
    uint8 bUseAlpha;
    uint32* pCompressedMatrix; // sizeof = 0x18
    RwTexture* pTextures[4];
    uint32 pad2;
    CParticleStorageListSAInterface ParticleStorageList;
    CParticleManagerSAInterface particleManager;
};
C_ASSERT(sizeof(CParticleEmitterBaseDataSAInterface) == 0x40);