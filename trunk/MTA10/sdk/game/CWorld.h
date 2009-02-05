/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CWorld.h
*  PURPOSE:		Game world interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_WORLD
#define __CGAME_WORLD

#include "CEntity.h"
#include "CColPoint.h"

class CWorld
{
public:
	virtual VOID		Add ( CEntity * entity )=0;
	virtual VOID		Remove ( CEntity * entity )=0;
	//virtual bool		ProcessLineOfSight(CVector * vecStart, CVector * vecEnd, CColPoint * colCollision, CEntity * CollisionEntity)=0;
	virtual bool		ProcessLineOfSight(CVector * vecStart, CVector * vecEnd, CColPoint ** colCollision, CEntity ** CollisionEntity, bool bCheckBuildings = true, bool bCheckVehicles = true, bool bCheckPeds = true, bool bCheckObjects = true, bool bCheckDummies = true, bool bSeeThroughStuff = false, bool bIgnoreSomeObjectsForCamera = false, bool bShootThroughStuff = false)=0;
    // THIS FUNCTION IS INCOMPLETE AND SHOULD NOT BE USED ----------v
    virtual bool        TestLineSphere(CVector * vecStart, CVector * vecEnd, CVector * vecSphereCenter, float fSphereRadius, CColPoint ** colCollision )=0;
	virtual VOID		IgnoreEntity(CEntity * entity)=0;
	virtual BYTE		GetLevelFromPosition(CVector * vecPosition)=0;
	virtual FLOAT		FindGroundZForPosition(FLOAT fX, FLOAT fY)=0;
	virtual FLOAT		FindGroundZFor3DPosition(CVector * vecPosition)=0;
	virtual VOID		LoadMapAroundPoint(CVector * vecPosition, FLOAT fRadius)=0;
	virtual bool		IsLineOfSightClear ( CVector * vecStart, CVector * vecEnd, bool bCheckBuildings = true, bool bCheckVehicles = true, bool bCheckPeds = true, bool bCheckObjects = true, bool bCheckDummies = true, bool bSeeThroughStuff = false, bool bIgnoreSomeObjectsForCamera = false )=0;
    virtual bool        HasCollisionBeenLoaded ( CVector * vecPosition )=0;
    virtual DWORD       GetCurrentArea ( void )=0;
    virtual void        SetCurrentArea ( DWORD dwArea )=0;
};

#endif