/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CWorld.h
*  PURPOSE:     Game world interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_WORLD
#define __CGAME_WORLD

#include "CEntity.h"
#include "CColPoint.h"

struct SLineOfSightFlags
{
    SLineOfSightFlags ( void )
        : bCheckBuildings ( true )
        , bCheckVehicles ( true )
        , bCheckPeds ( true )
        , bCheckObjects ( true )
        , bCheckDummies ( true )
        , bSeeThroughStuff ( false )
        , bIgnoreSomeObjectsForCamera ( false )
        , bShootThroughStuff ( false )
    {}
    bool bCheckBuildings;
    bool bCheckVehicles;
    bool bCheckPeds;
    bool bCheckObjects;
    bool bCheckDummies;
    bool bSeeThroughStuff;
    bool bIgnoreSomeObjectsForCamera;
    bool bShootThroughStuff;            // not used for IsLineOfSightClear
};

struct SLineOfSightBuildingResult
{
    SLineOfSightBuildingResult ( void )
        : bValid ( false )
    {}
    bool bValid;
    ushort usModelID;
    CVector vecPosition;
    CVector vecRotation;
};


class CWorld
{
public:
    virtual void        Add                         ( CEntity * entity ) = 0;
    virtual void        Remove                      ( CEntity * entity ) = 0;
    virtual bool        ProcessLineOfSight          ( const CVector * vecStart, const CVector * vecEnd, CColPoint ** colCollision, CEntity ** CollisionEntity, const SLineOfSightFlags flags = SLineOfSightFlags(), SLineOfSightBuildingResult* pBuildingResult = NULL ) = 0;
    // THIS FUNCTION IS INCOMPLETE AND SHOULD NOT BE USED ----------v
    virtual bool        TestLineSphere              ( CVector * vecStart, CVector * vecEnd, CVector * vecSphereCenter, float fSphereRadius, CColPoint ** colCollision ) = 0;
    virtual void        IgnoreEntity                ( CEntity * entity ) = 0;
    virtual BYTE        GetLevelFromPosition        ( CVector * vecPosition ) = 0;
    virtual float       FindGroundZForPosition      ( float fX, float fY ) = 0;
    virtual float       FindGroundZFor3DPosition    ( CVector * vecPosition ) = 0;
    virtual void        LoadMapAroundPoint          ( CVector * vecPosition, float fRadius ) = 0;
    virtual bool        IsLineOfSightClear          ( const CVector * vecStart, const CVector * vecEnd, const SLineOfSightFlags flags = SLineOfSightFlags() ) = 0;
    virtual bool        HasCollisionBeenLoaded      ( CVector * vecPosition ) = 0;
    virtual DWORD       GetCurrentArea              ( void ) = 0;
    virtual void        SetCurrentArea              ( DWORD dwArea ) = 0;
    virtual void        SetJetpackMaxHeight         ( float fHeight ) = 0;
    virtual float       GetJetpackMaxHeight         ( void ) = 0;
    virtual void        SetAircraftMaxHeight        ( float fHeight ) = 0;
    virtual float       GetAircraftMaxHeight        ( void ) = 0;
};

#endif
