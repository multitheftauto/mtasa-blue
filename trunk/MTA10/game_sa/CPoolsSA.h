/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPoolsSA.h
*  PURPOSE:     Header file for game entity pools class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#ifndef __CGAMESA_POOLS
#define __CGAMESA_POOLS

#include <game/CPools.h>
#include "CPedSA.h"
#include "CVehicleSA.h"
#include "CObjectSA.h"
#include "CBuildingSA.h"

#define FUNC_GetVehicle						0x54fff0
#define FUNC_GetVehicleRef					0x54ffc0
//#define FUNC_GetVehicleCount				0x429510
#define FUNC_GetPed							0x54ff90
#define FUNC_GetPedRef						0x54ff60
//#define FUNC_GetPedCount					0x4A7440
#define FUNC_GetObject						0x550050
#define FUNC_GetObjectRef					0x550020
//#define FUNC_GetObjectCount					0x4A74D0

#define CLASS_CPool_Vehicle					0xB74494
#define CLASS_CPool_Ped						0xB74490
#define CLASS_CPool_Object					0xB7449C

#define CLASS_CBuildingPool                 0xb74498
#define CLASS_CPedPool                      0xb74490
#define CLASS_CObjectPool                   0xb7449c
#define CLASS_CDummyPool                    0xb744a0
#define CLASS_CVehiclePool                  0xb74494
#define CLASS_CColModelPool                 0xb744a4
#define CLASS_CTaskPool                     0xb744a8
#define CLASS_CEventPool                    0xb744ac
#define CLASS_CTaskAllocatorPool            0xb744bc
#define CLASS_CPedIntelligencePool          0xb744c0
#define CLASS_CPedAttractorPool             0xb744c4
#define CLASS_CEntryInfoNodePool            0xb7448c
#define CLASS_CNodeRoutePool                0xb744b8
#define CLASS_CPatrolRoutePool              0xb744b4
#define CLASS_CPointRoutePool               0xb744b0
#define CLASS_CPtrNodeDoubleLinkPool        0xB74488
#define CLASS_CPtrNodeSingleLinkPool        0xB74484


#define FUNC_CBuildingPool_GetNoOfUsedSpaces                0x550620
#define FUNC_CPedPool_GetNoOfUsedSpaces                     0x5504A0
#define FUNC_CObjectPool_GetNoOfUsedSpaces                  0x54F6B0
#define FUNC_CDummyPool_GetNoOfUsedSpaces                   0x5507A0
#define FUNC_CVehiclePool_GetNoOfUsedSpaces                 0x42CCF0
#define FUNC_CColModelPool_GetNoOfUsedSpaces                0x550870
#define FUNC_CTaskPool_GetNoOfUsedSpaces                    0x550940
#define FUNC_CEventPool_GetNoOfUsedSpaces                   0x550A10
#define FUNC_CTaskAllocatorPool_GetNoOfUsedSpaces           0x550d50
#define FUNC_CPedIntelligencePool_GetNoOfUsedSpaces         0x550E20
#define FUNC_CPedAttractorPool_GetNoOfUsedSpaces            0x550ef0
#define FUNC_CEntryInfoNodePool_GetNoOfUsedSpaces           0x5503d0
#define FUNC_CNodeRoutePool_GetNoOfUsedSpaces               0x550c80
#define FUNC_CPatrolRoutePool_GetNoOfUsedSpaces             0x550bb0
#define FUNC_CPointRoutePool_GetNoOfUsedSpaces              0x550ae0
#define FUNC_CPtrNodeSingleLinkPool_GetNoOfUsedSpaces       0x550230
#define FUNC_CPtrNodeDoubleLinkPool_GetNoOfUsedSpaces       0x550300

#define FUNC_CTrain_CreateMissionTrain                      0x6F7550
#define VAR_TrainModelArray                                 0x8D44F8

class CEntryInfoNodePoolSA : public CEntryInfoNodePool
{
public:
    int             GetNumberOfUsedSpaces ( void );
};

class CPointerNodeDoubleLinkPoolSA : public CPointerNodeDoubleLinkPool
{
public:
    int             GetNumberOfUsedSpaces ( void );
};

class CPointerNodeSingleLinkPoolSA : public CPointerNodeSingleLinkPool
{
public:
    int             GetNumberOfUsedSpaces ( void );
};

class CPoolsSA : public CPools
{
private:
	CVehicleSA		* Vehicles[MAX_VEHICLES];
	DWORD			dwVehicleCount;

	CPedSA			* Peds[MAX_PEDS];
	DWORD			dwPedCount;

	CObjectSA		* Objects[MAX_OBJECTS];
	DWORD			dwObjectCount;

	CBuildingSA		* Buildings[MAX_BUILDINGS];
	DWORD			dwBuildingCount;

    bool            m_bGetVehicleEnabled;

    CEntryInfoNodePool * EntryInfoNodePool;
    CPointerNodeDoubleLinkPool * PointerNodeDoubleLinkPool;
    CPointerNodeSingleLinkPool * PointerNodeSingleLinkPool;

public:
					CPoolsSA::CPoolsSA();
                    ~CPoolsSA ( void );

    void            DeleteAllVehicles   ( void );
    void            DeleteAllPeds       ( void );
    void            DeleteAllObjects    ( void );
    void            DeleteAllBuildings  ( void );

	CVehicle		* GetVehicle ( DWORD ID );
	CVehicle		* GetVehicle ( CVehicleSAInterface * VehicleInterface );
	CVehicle		* GetVehicleInterface ( DWORD ID );
	DWORD			GetVehicleRef ( CVehicle * vehicle );
	DWORD			GetVehicleCount (  );
	CObject			* AddObject ( DWORD ID );
	VOID			RemoveObject ( CObject * object );
	CObject			* GetObject ( DWORD ID );
	DWORD			GetObjectRef ( CObject * object );
	CObject *		GetObject ( CObjectSAInterface * objectInterface );
	DWORD			GetObjectCount (  );
	CPed			* GetPed ( DWORD ID );
	CPed			* GetPed ( DWORD * pedInterface );
	CPedSAInterface	* GetPedInterface ( DWORD ID );
	DWORD			GetPedRef ( CPed * ped );
	DWORD			GetPedRef ( DWORD internalInterface );
	DWORD			GetPedCount (  );

    int             GetNumberOfUsedSpaces ( ePools pools );
    void            DumpPoolsStatus ();

	CVehicle		* AddVehicle ( eVehicleTypes vehicleType );
	CPed			* AddPed ( ePedModel pedType );
	CPed			* AddPed ( CPedSAInterface * ped );
	CBuilding		* AddBuilding ( DWORD dwModelID );
    CVehicle        * AddTrain ( CVector * vecPosition, DWORD dwModels[], int iSize, bool bDirection );
    
	CPed *			CreateCivilianPed ( DWORD * pedInterface );

	VOID			RemovePed ( CPed * ped, bool bDelete );
	VOID			RemoveVehicle ( CVehicle * vehicle );
	VOID			RemovePed ( DWORD ID );

    // stuff that really maybe should be elsewhere or not, perhaps
    CEntryInfoNodePool*            GetEntryInfoNodePool ( void );
    CPointerNodeSingleLinkPool*    GetPointerNodeSingleLinkPool ( void );
    CPointerNodeDoubleLinkPool*    GetPointerNodeDoubleLinkPool ( void );
};

#endif