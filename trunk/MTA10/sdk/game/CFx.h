/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CFx.h
*  PURPOSE:     Game effects interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CFX
#define __CFX

#include "RenderWare.h"

class CVector;
class CEntity;
class CVehicle;

class CFx
{
public:
    virtual void                AddBlood                ( CVector & vecPosition, CVector & vecDirection, int iCount, float fBrightness ) = 0;
    virtual void                AddWood                 ( CVector & vecPosition, CVector & vecDirection, int iCount, float fBrightness ) = 0;
    virtual void                AddSparks               ( CVector & vecPosition, CVector & vecDirection, float fForce, int iCount, CVector vecAcrossLine, unsigned char ucBlurIf0, float fSpread, float fLife ) = 0;
    virtual void                AddTyreBurst            ( CVector & vecPosition, CVector & vecDirection ) = 0;
    virtual void                AddBulletImpact         ( CVector & vecPosition, CVector & vecDirection, int iSmokeSize, int iSparkCount, float fSmokeIntensity ) = 0;
    virtual void                AddPunchImpact          ( CVector & vecPosition, CVector & vecDirection, int i ) = 0;
    virtual void                AddDebris               ( CVector & vecPosition, RwColor & rwColor, float fScale, int iCount ) = 0;
    virtual void                AddGlass                ( CVector & vecPosition, RwColor & rwColor, float fScale, int iCount ) = 0;
    virtual void                TriggerWaterHydrant     ( CVector & vecPosition ) = 0;
    virtual void                TriggerGunshot          ( CEntity * pEntity, CVector & vecPosition, CVector & vecDirection, bool bIncludeSparks ) = 0;
    virtual void                TriggerTankFire         ( CVector & vecPosition, CVector & vecDirection ) = 0;
    virtual void                TriggerWaterSplash      ( CVector & vecPosition ) = 0;
    virtual void                TriggerBulletSplash     ( CVector & vecPosition ) = 0;
    virtual void                TriggerFootSplash       ( CVector & vecPosition ) = 0;
};

#endif
