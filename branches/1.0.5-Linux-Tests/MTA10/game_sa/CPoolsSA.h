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
*               Alberto Alonso <rydencillo@gmail.com>
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

#include <google/dense_hash_map>

class CEntryInfoNodePoolSA : public CEntryInfoNodePool
{
public:
    int             GetNumberOfUsedSpaces       ( );
};

class CPointerNodeDoubleLinkPoolSA : public CPointerNodeDoubleLinkPool
{
public:
    int             GetNumberOfUsedSpaces       ( );
};

class CPointerNodeSingleLinkPoolSA : public CPointerNodeSingleLinkPool
{
public:
    int             GetNumberOfUsedSpaces       ( );
};

class CPoolsSA : public CPools
{
public:
                            CPoolsSA            ( );
                            ~CPoolsSA           ( );

    
    // Vehicles pool
    CVehicle*               AddVehicle          ( eVehicleTypes eVehicleType );
    CVehicle*               AddVehicle          ( DWORD* pGameInterface );
private:
    bool                    AddVehicleToPool    ( CVehicleSA* pVehicle );
public:
    void                    RemoveVehicle       ( CVehicle* pVehicle, bool bDelete = true );
    void                    RemoveVehicle       ( unsigned long ulID, bool bDelete = true );
    CVehicle*               GetVehicle          ( unsigned long ulID );
    CVehicle*               GetVehicle          ( DWORD* pGameInterface );
    DWORD                   GetVehicleRef       ( CVehicle* pVehicle );
    DWORD                   GetVehicleRef       ( DWORD* pGameInterface );
    CVehicle*               GetVehicleFromRef   ( DWORD dwGameRef );
    inline unsigned long    GetVehicleCount     ( ) { return m_vehiclePool.ulCount;; }
    void                    DeleteAllVehicles   ( );

    // Objects pool
    CObject*                AddObject           ( DWORD dwModelID );
    CObject*                AddObject           ( DWORD* pGameInterface );
private:
    bool                    AddObjectToPool     ( CObjectSA* pObject );
public:
    void                    RemoveObject        ( CObject* pObject, bool bDelete = true );
    void                    RemoveObject        ( unsigned long ulID, bool bDelete = true );
    CObject*                GetObject           ( unsigned long ulID );
    CObject*                GetObject           ( DWORD* pGameInterface );
    DWORD                   GetObjectRef        ( CObject* pObject );
    DWORD                   GetObjectRef        ( DWORD* pGameInterface );
    CObject*                GetObjectFromRef    ( DWORD dwGameRef );
    inline unsigned long    GetObjectCount      ( ) { return m_objectPool.ulCount; }
    void                    DeleteAllObjects    ( );

    // Peds pool
    CPed*                   AddPed              ( ePedModel ePedType );
    CPed*                   AddPed              ( DWORD* pGameInterface );
    CPed*                   AddCivilianPed      ( DWORD* pGameInterface );
private:
    bool                    AddPedToPool        ( CPedSA* pPed );
public:
    void                    RemovePed           ( CPed* ped, bool bDelete = true );
    void                    RemovePed           ( unsigned long ulID, bool bDelete = true);
    CPed*                   GetPed              ( unsigned long ulID );
    CPed*                   GetPed              ( DWORD* pGameInterface );
    DWORD                   GetPedRef           ( CPed* pPed );
    DWORD                   GetPedRef           ( DWORD* pGameInterface );
    CPed*                   GetPedFromRef       ( DWORD dwGameRef );
    CPedSAInterface*        GetPedInterface     ( DWORD dwGameRef ); // game_sa specific
    inline unsigned long    GetPedCount         ( ) { return m_pedPool.ulCount; }
    void                    DeleteAllPeds       ( );

    CEntity*                GetEntity           ( DWORD* pGameInterface );

    // Others
    CBuilding*              AddBuilding         ( DWORD dwModelID );
    void                    DeleteAllBuildings  ( );
    CVehicle*               AddTrain            ( CVector* vecPosition, DWORD dwModels[], int iSize, bool bDirection );

    int                     GetNumberOfUsedSpaces   ( ePools pools );
    void                    DumpPoolsStatus         ( );

    int                     GetPoolDefaultCapacity  ( ePools pool );
    int                     GetPoolCapacity         ( ePools pool );
    void                    SetPoolCapacity         ( ePools pool, int iValue );

    // stuff that really maybe should be elsewhere or not, perhaps
    CEntryInfoNodePool*             GetEntryInfoNodePool            ( );
    CPointerNodeSingleLinkPool*     GetPointerNodeSingleLinkPool    ( );
    CPointerNodeDoubleLinkPool*     GetPointerNodeDoubleLinkPool    ( );


private:
    // Generic container for pools
    template < class T, class I, unsigned long MAX >
    struct SPoolData
    {
        typedef         google::dense_hash_map < I*, T* >  mapType;
        mapType         map;
        T*              array [ MAX ];
        unsigned long   ulCount;

    private:
        friend class CPoolsSA;

        SPoolData ( )
            : map ( MAX ), ulCount ( 0UL )
        {
            for ( unsigned int i = 0; i < MAX; ++i )
            {
                array [ i ] = NULL;
            }
        }
    };

    // Pools
    typedef SPoolData < CVehicleSA, CVehicleSAInterface, MAX_VEHICLES > vehiclePool_t;
    typedef SPoolData < CPedSA, CPedSAInterface, MAX_PEDS > pedPool_t;
    typedef SPoolData < CObjectSA, CObjectSAInterface, MAX_OBJECTS > objectPool_t;
    vehiclePool_t   m_vehiclePool;
    pedPool_t       m_pedPool;
    objectPool_t    m_objectPool;

    CBuildingSA*    Buildings [ MAX_BUILDINGS ];
    unsigned long   m_ulBuildingCount;

    bool            m_bGetVehicleEnabled;

    CEntryInfoNodePool*             EntryInfoNodePool;
    CPointerNodeDoubleLinkPool*     PointerNodeDoubleLinkPool;
    CPointerNodeSingleLinkPool*     PointerNodeSingleLinkPool;
};




#define FUNC_GetVehicle                     0x54fff0
#define FUNC_GetVehicleRef                  0x54ffc0
//#define FUNC_GetVehicleCount              0x429510
#define FUNC_GetPed                         0x54ff90
#define FUNC_GetPedRef                      0x54ff60
//#define FUNC_GetPedCount                  0x4A7440
#define FUNC_GetObject                      0x550050
#define FUNC_GetObjectRef                   0x550020
//#define FUNC_GetObjectCount                   0x4A74D0

#define CLASS_CPool_Vehicle                 0xB74494
#define CLASS_CPool_Ped                     0xB74490
#define CLASS_CPool_Object                  0xB7449C

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

#endif
