/*********************************************7********************************
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

#define FUNC_CParticleHeader__constructor                               0x4AA0D0
// Allocate itself on DWORD boundary via SimpleBuffer::getAlignedData
#define FUNC_CParticleHeader__alloc                                     0x4AA100
#define FUNC_CParticleHeader__GetBoundingSphere                         0x4AA1F0
#define FUNC_CParticleHeader__SetBoundingSphere                         0x4AA200
// These perform operations on every pParticleEmitterBaseData they have in array
#define FUNC_CParticleHeader__Unknown1                                  0x4AA130
#define FUNC_CParticleHeader__Render                                    0x4AA160
#define FUNC_CParticleHeader__Unknown2                                  0x4AA1B0

// The part !!ABOVE!! FX_PRIM_EMITTER_DATA in effects.fxp
class CParticleHeaderSAInterface
{
public:
    uint32 pad1; // 0
    uint32 pad2; // 4
    uint32 uiParticleNameHash; // 8
    float fLength; // 12
    float fIntervalMin; // 16
    float fLength2; // 20
    uint16 usCullDistance; // 24
    uint8 ucPlayMode; // 26
    uint8 ucParticleEmitterBaseDataCount; // 27
    class CParticleEmitterBaseDataSAInterface* pParticleEmitterBaseDataArray; // 28 // allocated array of CParticleEmitterBaseData pointers
    RwSphere* pBoundingSphere; // 32
};
C_ASSERT(sizeof(CParticleHeaderSAInterface) == 0x24);