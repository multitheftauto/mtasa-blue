/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CEventGunShot.h
*  PURPOSE:		Event gunshot interface
*  DEVELOPERS:	Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CEventGunShot_H
#define __CEventGunShot_H

class CEventGunShotSAInterface;

class CEventGunShot
{
public:
    virtual CEventGunShotSAInterface *          GetInterface        ( void ) = 0;

    virtual void                                Destroy             ( bool bDestroyInterface ) = 0;
};

#endif