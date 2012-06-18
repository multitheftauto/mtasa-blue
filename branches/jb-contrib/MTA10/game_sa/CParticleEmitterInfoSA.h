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

class CParticleEmitterInfoSAInterface
{
public:
    uint32 infoCount;
    uint32* pEmitterInfoData;
    uint8 pad1[4];
    uint32 pad2[4];
};

