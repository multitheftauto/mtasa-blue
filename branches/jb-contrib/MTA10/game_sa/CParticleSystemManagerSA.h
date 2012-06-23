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

#ifndef __CGAMESA_PARTICLESYSTEMMANAGER
#define __CGAMESA_PARTICLESYSTEMMANAGER

#include <game/CParticleSystemManager.h>



#define CLASS_CParticleSystemMgr                                0xA9AE00

// <Particle System Root>
class CParticleSystemManagerSAInterface
{

};
C_ASSERT(sizeof(CParticleSystemManagerSAInterface) == 0x80);

class CParticleSystemManagerSA : public CParticleSystemManager
{
public:
private:
    CParticleSystemManagerSAInterface* pInterface;
};

#endif