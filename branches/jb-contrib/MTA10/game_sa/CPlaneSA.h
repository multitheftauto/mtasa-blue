/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPlaneSA.h
*  PURPOSE:     Header file for plane vehicle entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_PLANE
#define __CGAMESA_PLANE

#include <game/CPlane.h>
#include "CAutomobileSA.h"

class CPlaneSAInterface : public CAutomobileSAInterface
{
    uint32 pad1[8];
    float fMaxAltitude;
    uint32 pad2;
    float fMinAltitude;
    uint32 pad3[6];
    float fLandingGearPos; // [0.0, 1.0]
    uint32 pad4;
    class CParticleFx** ppSpecialPlaneParticles; // may be array of CParticleFx* , needs more checking
    uint32 pad5[4];
    class CParticleFx* pThrustersParticles[4];
    class CParticleFx* pEngineFireParticle;
    class CFxSystem* pFxSystem; // check its function
    uint8 pad6[4];
};
C_ASSERT(sizeof(CPlaneSAInterface) == 0xA04);

class CPlaneSA : public virtual CPlane, public virtual CAutomobileSA
{
private:
    CPlaneSAInterface        * internalInterface;
public:
    CPlaneSA( CPlaneSAInterface * plane );
    CPlaneSA( eVehicleTypes dwModelID, unsigned char ucVariation, unsigned char ucVariation2 );
};

#endif
