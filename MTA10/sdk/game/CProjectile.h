/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CProjectile.h
*  PURPOSE:		Projectile entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_PROJECTILE
#define __CGAME_PROJECTILE

#include "CObject.h"

class CProjectile: public virtual CObject
{
public:
    virtual                 ~CProjectile ( void ) {};

    virtual void            Destroy()=0;
    virtual void            SetProjectileInfo ( class CProjectileInfo* pProjectileInfo )=0;
};

#endif