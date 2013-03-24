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

#include "CRenderWareSA.h"

#define FUNC_CParticleInfo__Init					0x4AB270
#define FUNC_CParticleInfo__Set						0x4AB290				
// ^duplicate @ 004AB2D0

class CParticleInfoSAInterface
{
public:
    RwColorFloat colour; /** Range: [0.0, 1.0] */
    float fSize; /** Range: [0.0, 1.0] */
    float pad1; /** Range: [0.0, 1.0] */
    float fDurationFactor;
};
// C_ASSERT(sizeof(CParticleInfoSAInterface) == 0x1C); // NEEDS CHECKING

class CParticleInfo
{
private:
    CParticleInfoSAInterface* pInterface;
public:

};
