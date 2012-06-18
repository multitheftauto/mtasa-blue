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

// FX_PRIM_BASE_DATA section in effects.fxp
// move this into its own file later
class CParticleEmitterBaseDataSAInterface
{
public:
    uint32* vtbl;
    uint8 pad1;
    uint8 ucSrcBlendID; // D3DBLEND ; do not use that because we need
    uint8 ucDestBlendID; // D3DBLEND ; byte and D3DBLEND is forced DWORD
    uint8 bUseAlpha;
    uint32* pCompressedMatrix;
    RwTexture* pTextures[4];
    uint32 pad2[7];
    uint16 usLodStart;
    uint16 usLodEnd;
    uint32 pad3;
};
C_ASSERT(sizeof(CParticleEmitterBaseDataSAInterface) == 0x40);