/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CFireManager.h
 *  PURPOSE:     Fire entity manager interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CFire;
class CVector;

class CFireManager
{
public:
    virtual void ExtinguishPoint(CVector& vecPosition, float fRadius) = 0;
    // doesn't work, use below instead
    // virtual CFire                 * StartFire ( CEntity * entityTarget, CEntity * entityCreator, float fSize )=0;
    virtual CFire* StartFire(CVector& vecPosition, float fSize) = 0;
    virtual void   ExtinguishAllFires() = 0;
    virtual CFire* GetFire(DWORD ID) = 0;
};
