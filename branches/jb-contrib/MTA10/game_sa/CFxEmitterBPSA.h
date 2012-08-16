/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        
*  PURPOSE:     
*  DEVELOPERS:  Nobody knows
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

class CFxEmitterBPSAInterfaceVTBL
{
public:
    uint32 SCALAR_DELETING_DESTRUCTOR;
    uint32 LoadDataLOD;
    uint32 ReadTextures;
    uint32 AllocateFxEmitter;
    uint32 Update;
    uint32 Render;
    uint32 FindFxSystem;
};

class CFxEmitterBPSAInterface
{
public:
    CFxEmitterBPSAInterfaceVTBL* vtbl;
    uint8 pad1;
    uint8 ucSrcBlendID;
    uint8 ucDestBlendID;
    uint8 bAlphaOn;
    void* pCompressedMatrix;
    RwTexture* pTextures[4];
    uint32 pad2;
    LListHead ParticleList; // ???
    CParticleManager particleManager; //???
};
C_ASSERT(sizeof(CFxEmitterBPSAInterface) == 0x40);