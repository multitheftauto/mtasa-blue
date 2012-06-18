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

#define FUNC_CParticleInfo__Init					0x4AB270
#define FUNC_CParticleInfo__Set						0x4AB290				
// ^duplicate @ 004AB2D0

class CParticleInfoSAInterface
{
public:
    float r;
    float g;
    float b;
    float a;
    float fSize;
    float pad1;
    float fDurationFactor;
};
// C_ASSERT(sizeof(CParticleInfoSAInterface) == 0x1C); // NEEDS CHECKING
