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

class CFxSystemSAInterface
{
public:
    uint32 pad1;
    uint32 pad2;
    uint32 uiFxSystemHash;
    uint32 pad3[3];
    uint16 pad4;
    uint8 pad5;
    uint8 uiParticleEmitterManagerCount;
    uint32 pad6[4];
};
// C_ASSERT(sizeof(CFxSystemSAInterface) == 0x???); find correct size
