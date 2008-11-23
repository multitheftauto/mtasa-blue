/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CParticleSA.h
*  PURPOSE:     Header file for particle entity class
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_PARTICLE
#define __CGAMESA_PARTICLE

#include <game/CParticle.h>

class CParticleSAInterface
{

};

class CParticleSA : public CParticle
{
private:
	CParticleSAInterface		* internalInterface;
public:
};

#endif