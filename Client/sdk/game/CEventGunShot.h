/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CEventGunShot.h
 *  PURPOSE:     Event gunshot interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CEventGunShotSAInterface;

class CEventGunShot
{
public:
    virtual CEventGunShotSAInterface* GetInterface() = 0;

    virtual void Destroy(bool bDestroyInterface) = 0;
};
