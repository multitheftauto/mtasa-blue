/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CProjectileInfo.h
*  PURPOSE:		Projectile entity information interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_PROJECTILEINFO
#define __CGAME_PROJECTILEINFO

class CProjectile;
class CPlayerPed;
class CProjectileInfo
{
public:
    virtual bool					AddProjectile ( CEntity * creator, eWeaponType eWeapon, CVector vecOrigin, float fForce, CVector * target, CEntity * targetEntity )=0;
    virtual CProjectile *           GetProjectile ( DWORD ID ) = 0;
    virtual CProjectile *           GetProjectile ( void * projectilePointer )=0; //hack, don't use please
    virtual CProjectileInfo         * GetProjectileInfo ( void * projectileInfoInterface )=0; //don't use
    virtual void                    RemoveProjectile ( CProjectileInfo * pProjectileInfo, CProjectile * pProjectile )=0;
    virtual CProjectileInfo         * GetNextFreeProjectileInfo ( )=0;
    virtual CProjectileInfo         * GetProjectileInfo ( DWORD Index )=0;

    virtual CEntity*                GetTarget ( void ) = 0;
    virtual void                    SetTarget ( CEntity* pEntity ) = 0;

    virtual bool                    IsActive ( void ) = 0;
};

#endif