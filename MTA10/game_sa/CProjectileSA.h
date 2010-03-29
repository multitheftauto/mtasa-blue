/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CProjectileSA.h
*  PURPOSE:     Header file for projectile entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_PROJECTILE
#define __CGAMESA_PROJECTILE

#include <game/CProjectile.h>
#include "CObjectSA.h"

class CProjectileSAInterface : public CObjectSAInterface // entirely inherited from CObject
{

};

class CProjectileSA : public virtual CProjectile, public virtual CObjectSA
{
private:
    CProjectileSAInterface          * internalInterface;
    class CProjectileInfo         * projectileInfo;
public:
                    CProjectileSA(class CProjectileSAInterface * projectileInterface);
                    ~CProjectileSA( );
    void            Destroy();
    
    void            SetProjectileInfo ( CProjectileInfo* pProjectileInfo ) { projectileInfo = pProjectileInfo; }
};

#endif
