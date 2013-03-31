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
#include "CRenderWareExtensionSA.h"
#include "CTextureManagerSA.h"
#include "CModelInfoSA.h"
#include "CAtomicModelInfoSA.h"
#include "CClumpModelInfoSA.h"
#include "CVehicleModelInfoSA.h"
#include "CPedModelInfoSA.h"
#include "CPedSA.h"
#include "CVehicleSA.h"
#include "CObjectSA.h"
#include "CDummySA.h"
#include "CBuildingSA.h"
#include "CPtrNodeSA.h"
#include "CEntryInfoSA.h"
#include "CRouteSA.h"
#include "CTaskAllocatorSA.h"
#include "CTaskManagementSystemSA.h"
#include "CEventSA.h"
#include "CPedIntelligenceSA.h"

template <class type, int max, const size_t size = sizeof(type)>
class CPool
{
public:
    CPool( void )
    {
        m_pool = (type*)new unsigned char[ size * max ];
        m_flags = new unsigned char[ max ];

        m_poolActive = true;

        // Reset all flags
        memset(m_flags, 0x80, max);

        m_max = max;
        m_lastUsed = 0xFFFFFFFF;
    }

    inline type*    GetOffset( unsigned int id )
    {
        return (type*)( (unsigned int)m_pool + size * id );
    }

    void    Clear( void )
    {
        unsigned int n;

        for (n=0; n<m_max; n++)
        {
            if ( m_flags[n] & 0x80 )
                continue;

            delete GetOffset( n );
        }

#ifdef _DEBUG
        if ( GetCount() != 0 )
            __asm int 3
#endif
    }

    ~CPool( void )
    {
        Clear();

        delete [] (unsigned char*)m_pool;
        delete [] m_flags;
    }

    inline type*    Allocate( void )
    {
        // The original code did two iterations, but it is not required
        for ( unsigned int n = ++m_lastUsed; n < m_max; n++ )
        {
            // If slot is used, we skip
            if ( !( m_flags[n] & 0x80 ) )
                continue;

            // Mark slot as used
            m_flags[n] &= ~0x80;

            // Next iteration we start from next slot
            m_lastUsed = n;
            return GetOffset( n );
        }

        return NULL;
    }

    inline type*    Get( unsigned int id )
    {
        return ( (id < m_max) && !(m_flags[id] & 0x80) ) ? GetOffset( id ) : NULL;
    }

    unsigned int    GetIndex( type *entity )
    {
        return ((unsigned int)entity - (unsigned int)m_pool) / size;
    }

    bool    IsValid( type *entity )
    {
        return entity >= m_pool && GetIndex( entity ) < m_max;
    }

    void    Free( unsigned int id )
    {
        if ( id >= m_max )
            return;

        if ( m_flags[id] & 0x80 )
            return;

        m_flags[id] |= 0x80;
        
        if ( id < m_lastUsed )
            m_lastUsed = id - 1;
    }

    void    Free( type *entity )
    {
        // For compatibility with MSVCRT
        if ( entity == NULL )
            return;

        Free( GetIndex( entity ) );
    }

    bool    IsAnySlotFree( void )
    {
        for ( unsigned int n = m_lastUsed + 1; n < m_max; n++ )
        {
            if ( m_flags[n] & 0x80 )
                return true;
        }

        return false;
    }

    bool    Full( void )
    {
        // We do not have to count all of the slots.
        return !IsAnySlotFree();
    }

    unsigned int    GetCount( void )
    {
        // Count all occupied slots in this pool
        unsigned int count = 0;

        for ( unsigned int n = 0; n < m_max; n++ )
        {
            if ( !( m_flags[n] & 0x80 ) )
                count++;
        }

        return count;
    }

    unsigned int    GetMax( void )
    {
        return m_max;
    }

    type*           m_pool;
    unsigned char*  m_flags;
    unsigned int    m_max;
    unsigned int    m_lastUsed;
    bool            m_poolActive;
};

class CPedAttractorSA
{
public:
    BYTE            m_pad[196];
};

class CEnvMapMaterialSA
{
public:
    BYTE            m_pad[12];
};

class CEnvMapAtomicSA
{
public:
    BYTE            m_pad[12];
};

class CSpecMapMaterialSA
{
public:
    BYTE            m_pad[8];
};

#define MAX_DUMMIES     4000
#define MAX_IPL         256

#ifdef GAME_INTERFACE_SEPERATION

// Rockstar's inheritance trick; keep these chains updated!
#define MAX_VEHICLE_SIZE ( max(sizeof(CHeliSAInterface),max(sizeof(CTrainSAInterface),max(sizeof(CAutomobileSAInterface),max(sizeof(CBikeSAInterface),max(sizeof(CBicycleSAInterface),max(sizeof(CPlaneSAInterface),max(sizeof(CBoatSAInterface),max(sizeof(CAutomobileTrailerSAInterface),max(sizeof(CQuadBikeSAInterface),sizeof(CMonsterTruckSAInterface)))))))))) )

#define MAX_PED_SIZE ( max(sizeof(CPedSAInterface),max(sizeof(CPlayerPedSAInterface),sizeof(CCivilianPedSAInterface))) )

#define MAX_OBJECT_SIZE ( max(sizeof(CObjectSAInterface),sizeof(CProjectileSAInterface)) )

#else //GAME_INTERFACE_SEPERATION

#define MAX_VEHICLE_SIZE    2584
#define MAX_PED_SIZE        1988
#define MAX_OBJECT_SIZE     412

#endif //GAME_INTERFACE_SEPERATION

typedef CPool <CVehicleComponentInfoSAInterface, 500> CVehicleComponentInfoPool;
typedef CPool <CColModelSAInterface, 20000> CColModelPool;

typedef CPool <CPtrNodeSingleSA, 100000> CPtrNodeSinglePool;
typedef CPool <CPtrNodeDoubleSA, 200000> CPtrNodeDoublePool;
typedef CPool <CEntryInfoSA, 100000> CEntryInfoPool; // info for every entity in the world

typedef CPool <CTxdInstanceSA, MAX_TXD> CTxdPool;

typedef CPool <CVehicleSAInterface, MAX_VEHICLES, MAX_VEHICLE_SIZE> CVehiclePool;
typedef CPool <CPedSAInterface, MAX_PEDS, MAX_PED_SIZE> CPedPool;
typedef CPool <CObjectSAInterface, MAX_OBJECTS, MAX_OBJECT_SIZE> CObjectPool;

typedef CPool <CBuildingSAInterface, MAX_BUILDINGS> CBuildingPool;
typedef CPool <CDummySAInterface, MAX_DUMMIES> CDummyPool;

typedef CPool <CTaskSAInterface, MAX_TASKS, 128> CTaskPool;
typedef CPool <CEventSAInterface, 5000> CEventPool;
typedef CPool <CPointRouteSA, 64> CPointRoutePool;
typedef CPool <CPatrolRouteSA, 32> CPatrolRoutePool;
typedef CPool <CNodeRouteSA, 64> CNodeRoutePool;
typedef CPool <CTaskAllocatorSA, 16> CTaskAllocatorPool;

typedef CPool <CPedIntelligenceSAInterface, MAX_PEDS> CPedIntelligencePool;
typedef CPool <CPedAttractorSA, 64> CPedAttractorPool;

typedef CPool <CColFileSA, 255> CColFilePool;
typedef CPool <CIPLFileSA, MAX_IPL> CIPLFilePool;

typedef CPool <CEnvMapMaterialSA, 16000> CEnvMapMaterialPool;
typedef CPool <CEnvMapAtomicSA, 4000> CEnvMapAtomicPool;
typedef CPool <CSpecMapMaterialSA, 16000> CSpecMapMaterialPool;

// They have to be defined somewhere!
extern CVehicleComponentInfoPool** ppVehicleComponentInfoPool;
extern CColModelPool** ppColModelPool;

extern CPtrNodeSinglePool** ppPtrNodeSinglePool;
extern CPtrNodeDoublePool** ppPtrNodeDoublePool;
extern CEntryInfoPool** ppEntryInfoPool;

extern CTxdPool** ppTxdPool;

extern CVehiclePool** ppVehiclePool;
extern CPedPool** ppPedPool;
extern CObjectPool** ppObjectPool;

extern CBuildingPool** ppBuildingPool;
extern CDummyPool** ppDummyPool;

extern CTaskPool** ppTaskPool;
extern CEventPool** ppEventPool;
extern CPointRoutePool** ppPointRoutePool;
extern CPatrolRoutePool** ppPatrolRoutePool;
extern CNodeRoutePool** ppNodeRoutePool;
extern CTaskAllocatorPool** ppTaskAllocatorPool;

extern CPedIntelligencePool** ppPedIntelligencePool;
extern CPedAttractorPool** ppPedAttractorPool;

extern CColFilePool** ppColFilePool;
extern CIPLFilePool** ppIPLFilePool;

extern CEnvMapMaterialPool** ppEnvMapMaterialPool;
extern CEnvMapAtomicPool** ppEnvMapAtomicPool;
extern CSpecMapMaterialPool** ppSpecMapMaterialPool;

#ifdef MTA_INTERFACES_WITH_POOLS

// MTA pools; lets use the trick ourselves, shall we? :P
// Do not forget to extend this chain once new interfaces are spotted!
#define MAX_MTA_VEHICLE_SIZE ( max(sizeof(CVehicleSA),max(sizeof(CTrainSA),max(sizeof(CPlaneSA),max(sizeof(CHeliSA),max(sizeof(CBikeSA),max(sizeof(CBicycleSA),max(sizeof(CAutomobileTrailerSA),max(sizeof(CBoatSA),max(sizeof(CQuadBikeSA),sizeof(CMonsterTruckSA)))))))))) )

#define MAX_MTA_PED_SIZE ( max(sizeof(CPedSA),max(sizeof(CPlayerPedSA),sizeof(CCivilianPedSA))) )

#define MAX_MTA_OBJECT_SIZE ( max(sizeof(CObjectSA),sizeof(CProjectileSA)) )

// this thing doesnt work! compiler error [out of memory/too complex]
//#define MAX_MTA_TASK_SIZE ( max(MAX_PHYSICAL_TASK_SIZE, max(MAX_CAR_TASK_SIZE, max(MAX_CAR_UTIL_TASK_SIZE, max(MAX_GOTO_TASK_SIZE, max(MAX_ACTION_TASK_SIZE, max(sizeof(CTaskSimpleDuckSA), max(sizeof(CTaskSimpleSA), max(sizeof(CTaskComplexSA), max(MAX_BASIC_TASK_SIZE, MAX_ATTACK_TASK_SIZE))))))))) )

typedef CPool <CVehicleSA, MAX_VEHICLES, MAX_MTA_VEHICLE_SIZE> CMTAVehiclePool;
typedef CPool <CPedSA, MAX_PEDS, MAX_MTA_PED_SIZE> CMTAPedPool;
typedef CPool <CObjectSA, MAX_OBJECTS, MAX_MTA_OBJECT_SIZE> CMTAObjectPool;
typedef CPool <CTaskSA, MAX_TASKS, 128> CMTATaskPool; // we align the tasks, please keep this size-value up-to-date!
typedef CPool <CPlayerPedDataSAInterface, MAX_PEDS> CMTAPlayerDataPool;
typedef CPool <dynamicObjectData, MAX_OBJECTS> CMTAObjectDataPool;

extern CMTAVehiclePool *mtaVehiclePool;
extern CMTAPedPool *mtaPedPool;
extern CMTAObjectPool *mtaObjectPool;
extern CMTATaskPool *mtaTaskPool;
extern CMTAPlayerDataPool *mtaPlayerDataPool;
extern CMTAObjectDataPool *mtaObjectDataPool;

#endif //MTA_INTERFACES_WITH_POOLS

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
    CVehicle*               AddVehicle          ( eVehicleTypes eVehicleType, unsigned char ucVariation, unsigned char ucVariation2 );
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
    CObject*                AddObject           ( DWORD dwModelID, bool bLowLod, bool bBreakable );
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
#define CLASS_CColFilePool                  0x965560
#define CLASS_CIPLFilePool                  0x8E3FB0


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
