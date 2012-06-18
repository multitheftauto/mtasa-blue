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

#define FUNC_CGlass__global_ctor                    0x854EE0
#define FUNC_CGlass__global_dtor                    0x856BA0
#define FUNC_CGlass__constructor                    0x71A8B0
#define FUNC_CGlass__Init                           0x71A8D0
#define FUNC_CGlass__Update                         0x71AA10
#define FUNC_CGlass__UpdateAll                      0x71B0D0
#define FUNC_CGlass__Render_One                     0x71B100
#define FUNC_CGlass__GeneratePanesForWindow         0x71B620
#define FUNC_CGlass__WindowRespondsToCollision      0x71BC40
#define FUNC_CGlass__ProcessGlassHitByBullet        0x71C0D0
#define FUNC_CGlass__WindowRespondsToExplosion      0x71C1A0
#define FUNC_CGlass__Render                         0x71CE20

// Pool Size: 45
#define VAR_CGlass__aGlassPanes                     0xC71BF8

class CGlassSAInterface
{
public:
    CMatrixEx matrix;
    uint32 pad1[3];
    CVector vecUnk;
    uint32 creationTime;
    uint32 pad2[2];
    uint8 pad3[4];
};
C_ASSERT(sizeof(CGlassSAInterface) == 0x70);
