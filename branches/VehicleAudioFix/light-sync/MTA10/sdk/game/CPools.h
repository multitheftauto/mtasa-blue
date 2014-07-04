/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CPools.h
*  PURPOSE:     Game pool interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_POOLS
#define __CGAME_POOLS

#include "Common.h"

#include "CAutomobile.h"
#include "CBoat.h"
#include "CBike.h"
#include "CBuilding.h"
#include "CObject.h"
#include "CPed.h"
#include "CVehicle.h"

enum ePools {
    BUILDING_POOL = 0,
    PED_POOL,
    OBJECT_POOL,
    DUMMY_POOL,
    VEHICLE_POOL,
    COL_MODEL_POOL,
    TASK_POOL,
    EVENT_POOL,
    TASK_ALLOCATOR_POOL,
    PED_INTELLIGENCE_POOL,
    PED_ATTRACTOR_POOL,
    ENTRY_INFO_NODE_POOL,
    NODE_ROUTE_POOL,
    PATROL_ROUTE_POOL,
    POINT_ROUTE_POOL,
    POINTER_DOUBLE_LINK_POOL,
    POINTER_SINGLE_LINK_POOL,
    MAX_POOLS
};

class CEntryInfoNodePool
{
public:
    virtual int             GetNumberOfUsedSpaces   ( ) = 0;
};

class CPointerNodeSingleLinkPool
{
public:
    virtual int             GetNumberOfUsedSpaces   ( ) = 0;
};

class CPointerNodeDoubleLinkPool
{
public:
    virtual int             GetNumberOfUsedSpaces   ( ) = 0;
};

class CPools
{
public:
    // Vehicles pool
    virtual CVehicle*       AddVehicle              ( eVehicleTypes eVehicleType ) = 0;
    virtual CVehicle*       AddVehicle              ( DWORD* pGameInterface ) = 0;
    virtual void            RemoveVehicle           ( CVehicle* pVehicle, bool bDelete = true ) = 0;
    virtual void            RemoveVehicle           ( unsigned long ulID, bool bDelete = true ) = 0;
    virtual CVehicle*       GetVehicle              ( unsigned long ulID ) = 0;
    virtual CVehicle*       GetVehicle              ( DWORD* pGameInterface ) = 0;
    virtual DWORD           GetVehicleRef           ( CVehicle* pVehicle ) = 0;
    virtual DWORD           GetVehicleRef           ( DWORD* pGameInterface ) = 0;
    virtual CVehicle*       GetVehicleFromRef       ( DWORD dwGameRef ) = 0;
    virtual unsigned long   GetVehicleCount         ( ) = 0;

    // Objects pool
    virtual CObject*        AddObject               ( DWORD dwModelID ) = 0;
    virtual void            RemoveObject            ( CObject* pObject, bool bDelete = true ) = 0;
    virtual void            RemoveObject            ( unsigned long ulID, bool bDelete = true ) = 0;
    virtual CObject*        GetObject               ( unsigned long ulID ) = 0;
    virtual CObject*        GetObject               ( DWORD* pGameInterface ) = 0;
    virtual DWORD           GetObjectRef            ( CObject* pObject ) = 0;
    virtual DWORD           GetObjectRef            ( DWORD* pGameInterface ) = 0;
    virtual CObject*        GetObjectFromRef        ( DWORD dwGameRef ) = 0;
    virtual unsigned long   GetObjectCount          ( ) = 0;

    // Peds pool
    virtual CPed*           AddPed                  ( ePedModel ePedType ) = 0;
    virtual CPed*           AddPed                  ( DWORD* pGameInterface ) = 0;
    virtual CPed*           AddCivilianPed          ( DWORD* pGameInterface ) = 0;
    virtual void            RemovePed               ( CPed* pPed, bool bDelete = true ) = 0;
    virtual void            RemovePed               ( unsigned long ulID, bool bDelete = true ) = 0;
    virtual CPed*           GetPed                  ( unsigned long ulID ) = 0;
    virtual CPed*           GetPed                  ( DWORD* pGameInterface ) = 0; // not sure we really want this here
    virtual DWORD           GetPedRef               ( CPed* pPed ) = 0;
    virtual DWORD           GetPedRef               ( DWORD* pGameInterface ) = 0;
    virtual CPed*           GetPedFromRef           ( DWORD dwGameRef ) = 0;
    virtual unsigned long   GetPedCount             ( ) = 0;

    // Others
    virtual CBuilding*      AddBuilding             ( DWORD dwModelID ) = 0;
    virtual CVehicle*       AddTrain                ( CVector* vecPosition, DWORD dwModels[], int iSize, bool iDirection ) = 0;
    virtual CEntity*        GetEntity               ( DWORD* pGameInterface ) = 0;

    virtual int             GetNumberOfUsedSpaces   ( ePools pool ) = 0;
    virtual void            DumpPoolsStatus         ( ) = 0;

    virtual int             GetPoolDefaultCapacity  ( ePools pool ) = 0;
    virtual int             GetPoolCapacity         ( ePools pool ) = 0;
    virtual void            SetPoolCapacity         ( ePools pool, int iValue ) = 0;

    virtual CEntryInfoNodePool*             GetEntryInfoNodePool            ( ) = 0;
    virtual CPointerNodeSingleLinkPool*     GetPointerNodeSingleLinkPool    ( ) = 0;
    virtual CPointerNodeDoubleLinkPool*     GetPointerNodeDoubleLinkPool    ( ) = 0;
};

#endif
