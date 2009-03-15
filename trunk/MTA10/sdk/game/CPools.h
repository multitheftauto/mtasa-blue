/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CPools.h
*  PURPOSE:		Game pool interface
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

#include <windows.h>

class CPedSAInterface; // not happy about this
class CVehicleSAInterface; // or this

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
    virtual int             GetNumberOfUsedSpaces ( void )=0;
};

class CPointerNodeSingleLinkPool
{
public:
    virtual int             GetNumberOfUsedSpaces ( void )=0;
};

class CPointerNodeDoubleLinkPool
{
public:
    virtual int             GetNumberOfUsedSpaces ( void )=0;
};

class CPools
{
public:
	virtual CVehicle		* GetVehicle ( DWORD ID )=0;
	virtual CVehicle		* GetVehicle ( CVehicleSAInterface * VehicleInterface )=0;
	virtual CVehicle		* GetVehicleInterface ( DWORD ID )=0;

	virtual DWORD			GetVehicleRef ( CVehicle * vehicle )=0;
	virtual DWORD			GetVehicleCount (  )=0;
	virtual CBuilding		* AddBuilding ( DWORD dwModelID )=0;
	virtual CObject			* AddObject ( DWORD ID )=0;
	virtual VOID			RemoveObject ( CObject * object )=0;
	virtual CObject			* GetObject ( DWORD ID )=0;
    virtual CObject         * GetObject ( CObjectSAInterface * ObjectInterface )=0;
	virtual DWORD			GetObjectRef ( CObject * object )=0;
	virtual DWORD			GetObjectCount (  )=0;
	virtual CPed			* GetPed ( DWORD ID )=0;
	virtual CPed			* GetPed ( DWORD * pedInterface )=0; // not sure we really want this here
	virtual DWORD			GetPedRef ( CPed * ped )=0;
	virtual DWORD			GetPedCount (  )=0;

    virtual int             GetNumberOfUsedSpaces ( ePools pool )=0;
    virtual void            DumpPoolsStatus ()=0;
	virtual CVehicle		* AddVehicle ( eVehicleTypes vehicleType )=0;
	virtual CPed			* AddPed ( ePedModel pedType )=0;
    virtual CVehicle        * AddTrain ( CVector * vecPosition, DWORD dwModels[], int iSize, bool bDirection ) = 0;

	virtual CPed *			CreateCivilianPed ( DWORD * pedInterface )=0; // or this (remove it if we possibly can)

	virtual VOID			RemovePed ( CPed * ped, bool bDelete = true )=0;
	virtual VOID			RemoveVehicle ( CVehicle * vehicle )=0;
	virtual VOID			RemovePed ( DWORD ID )=0;

    virtual CEntryInfoNodePool *            GetEntryInfoNodePool ( void )=0;
    virtual CPointerNodeSingleLinkPool *    GetPointerNodeSingleLinkPool ( void )=0;
    virtual CPointerNodeDoubleLinkPool *    GetPointerNodeDoubleLinkPool ( void )=0;
};

#endif