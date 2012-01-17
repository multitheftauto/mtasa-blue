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
    ushort usLODModelID;
    CVector vecPosition;
    CVector vecRotation;
};

struct SBuildingRemoval
{
    SBuildingRemoval ( )
    {
        m_pBinaryRemoveList = new std::list < CEntitySAInterface * >;
        m_pDataRemoveList = new std::list < CEntitySAInterface * >;
        m_usModel = 0;
        m_vecPos = CVector( 0, 0, 0);
        m_fRadius = 0.0f;
    }

    ~SBuildingRemoval ( )
    {
        delete m_pBinaryRemoveList;
        delete m_pDataRemoveList;
    }

    void AddBinaryBuilding ( CEntitySAInterface * pInterface )
    {
        // Add to list of binary buildings for this removal
        m_pBinaryRemoveList->push_back ( pInterface );
    }
    void AddDataBuilding ( CEntitySAInterface * pInterface )
    {
        // Add to list of data buildings for this removal
        m_pDataRemoveList->push_back ( pInterface );
    }

    unsigned short m_usModel;
    CVector m_vecPos;
    float m_fRadius;
    std::list < CEntitySAInterface * > * m_pBinaryRemoveList;
    std::list < CEntitySAInterface * > * m_pDataRemoveList;
};
struct SIPLInst
{
    CVector     m_pPosition;
    CVector     m_pRotation;
    float       m_fRotationCont;
    WORD        m_nModelIndex;
    BYTE        m_nInterior;
    BYTE        m_bLOD;
};

struct sDataBuildingRemoval
{
    sDataBuildingRemoval ( CEntitySAInterface * pInterface, bool bData )
    {
        m_pInterface = pInterface;
        m_iCount = 0;
    }
    void AddCount ( )
    {
        m_iCount++;
    }
    void RemoveCount ( )
    {
        m_iCount--;
    }
    CEntitySAInterface * m_pInterface;
    int m_iCount;
};

enum eDebugCaller
{
    CEntity_SetMatrix,
    CEntity_SetOrientation,
    CEntity_FixBoatOrientation,
    BuildingRemoval,
    BuildingRemoval2,
    BuildingRemoval3,
    BuildingRemoval4,
    BuildingRemoval5,
    Building_Restore,
    Building_Restore2,
    BuildingRemovalReset,
    BuildingRemovalReset2,
    CVehicle_Destructor,
    CPlayerPed_Constructor,
    CPlayerPed_Destructor,
    CPickup_Constructor,
    CPickup_Destructor,
    CObject_Constructor,
    CObject_Destructor,
    CCivPed_Constructor,
    CCivPed_Destructor,
    CBuilding_Destructor,

};

class CWorld
{
public:
    virtual void        Add                         ( CEntity * entity, eDebugCaller CallerId ) = 0;
    virtual void        Remove                      ( CEntity * entity, eDebugCaller CallerId ) = 0;
    virtual void        Remove                      ( CEntitySAInterface * entityInterface, eDebugCaller CallerId ) = 0;
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
    virtual void        RemoveBuilding              ( unsigned short usModelToRemove, float fDistance, float fX, float fY, float fZ) = 0;
    virtual bool        IsRemovedModelInRadius      ( SIPLInst* pInst ) = 0;
    virtual bool        IsModelRemoved              ( unsigned short usModelID ) = 0;
    virtual void        ClearRemovedBuildingLists   ( void ) = 0;
    virtual bool        RestoreBuilding             ( unsigned short usModelToRestore, float fDistance, float fX, float fY, float fZ ) = 0;
    virtual SBuildingRemoval*   GetBuildingRemoval  ( CEntitySAInterface * pInterface ) = 0;
    virtual void        AddDataBuilding             ( CEntitySAInterface * pInterface ) = 0;
    virtual void        RemoveWorldBuildingFromLists( CEntitySAInterface * pInterface ) = 0;
    virtual bool        IsObjectRemoved             ( CEntitySAInterface * pInterface ) = 0;
    virtual bool        IsDataModelRemoved          ( unsigned short usModelID ) = 0;
};

#endif
