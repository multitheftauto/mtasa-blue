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
#include "CModelInfoSA.h"
#include "CRenderWareExtensionSA.h"
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
#include "CIPLSectorSA.h"

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

    // This function expects the entities to free themselves from this;
    // so do not decrease the count here!
    void    Clear( void )
    {
        for ( unsigned int n = 0; n < GetMax(); n++ )
        {
            if ( m_flags[n] & 0x80 )
                continue;

            delete GetOffset( n );
        }
    }

    ~CPool( void )
    {
        //Clear();

        if ( GetMax() > 0 )
        {
            if ( m_poolActive )
            {
                delete [] (unsigned char*)m_pool;
                delete [] m_flags;
            }

            m_pool = NULL;
            m_flags = NULL;
            m_max = 0;
            m_lastUsed = 0;
        }
    }

    inline type*    Allocate( void )
    {
        unsigned int n = ++m_lastUsed;
        unsigned int max = GetMax();

        for ( unsigned int i = 0; i < 2; i++ )
        {
            for ( ; n < max; n++ )
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

            // Try another iteration starting from the beginning.
            // This ensures that there are no errors in the allocation system.
            n = 0;
            max = m_lastUsed;
            m_lastUsed = 0xFFFFFFFF;
        }

        return NULL;
    }

    inline type*    Get( unsigned int id )
    {
        return ( (id < GetMax()) && !(m_flags[id] & 0x80) ) ? GetOffset( id ) : NULL;
    }

    unsigned int    GetIndex( type *entity ) const
    {
        return ((unsigned int)entity - (unsigned int)m_pool) / size;
    }

    bool    IsValid( type *entity ) const
    {
        return entity >= m_pool && GetIndex( entity ) < GetMax();
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

    bool    IsAnySlotFree( void ) const
    {
        for ( unsigned int n = m_lastUsed + 1; n < GetMax(); n++ )
        {
            if ( m_flags[n] & 0x80 )
                return true;
        }

        return false;
    }

    bool    Full( void ) const
    {
        // We do not have to count all of the slots.
        return !IsAnySlotFree();
    }

    unsigned int    GetCount( void ) const
    {
        // Count all occupied slots in this pool
        unsigned int count = 0;

        for ( unsigned int n = 0; n < GetMax(); n++ )
        {
            if ( !( m_flags[n] & 0x80 ) )
                count++;
        }

        return count;
    }

    unsigned int    GetMax( void ) const
    {
        // We enable compiler optimizations by using the constant template variable.
        // Using the dynamic variable here may enable support for hacks (which dynamically extend pool size).
        // Why should we enable that kind of support?
        return max;
    }

    template <typename callbackType>
    inline void     ForAllActiveEntries( callbackType& cb, unsigned int startIndex = 0 )
    {
        for ( unsigned int n = startIndex; n < GetMax(); n++ )
        {
            if ( !( m_flags[n] & 0x80 ) )
                cb.OnEntry( GetOffset( n ), n );
        }
    }

    template <typename callbackType>
    inline void     ForAllActiveReverse( callbackType& cb, unsigned int stopIndex = 0 )
    {
        for ( unsigned int n = GetMax() - 1; n >= stopIndex; n++ )
        {
            if ( !( m_flags[n] & 0x80 ) )
                cb.OnEntry( GetOffset( n ), n );
        }
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

typedef CPool <CPtrNodeSingleSA <void>, 100000> CPtrNodeSinglePool;
typedef CPool <CPtrNodeDoubleSA <void>, 200000> CPtrNodeDoublePool;
typedef CPool <CEntryInfoSA, 100000> CEntryInfoPool; // info for every entity in the world

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

typedef CPool <CQuadTreeNodeSAInterface <void>, 400> CQuadTreeNodePool;

// They have to be defined somewhere!
extern CVehicleComponentInfoPool** ppVehicleComponentInfoPool;
extern CColModelPool** ppColModelPool;

extern CPtrNodeSinglePool** ppPtrNodeSinglePool;
extern CPtrNodeDoublePool** ppPtrNodeDoublePool;
extern CEntryInfoPool** ppEntryInfoPool;

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

extern CQuadTreeNodePool** ppQuadTreeNodePool;

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

// Export a namespace, since it avoids the class environment at native level (to speed up look-up).
// I noticed that this file causes issues with Visual Studio IntelliSense, hopefully this helps solve it.
namespace Pools
{
    // Vehicles pool
    CVehicleSA*             GetVehicle                  ( unsigned long id );
    CVehicleSA*             GetVehicle                  ( CVehicleSAInterface *entity );
    unsigned int            GetVehicleRef               ( CVehicle *veh );
    unsigned int            GetVehicleRef               ( CVehicleSAInterface *veh );
    CVehicleSA*             GetVehicleFromRef           ( unsigned int index );

    // Objects pool
    CObjectSA*              GetObject                   ( unsigned long ulID );
    CObjectSA*              GetObject                   ( CObjectSAInterface *entity );
    unsigned int            GetObjectRef                ( CObject *obj );
    unsigned int            GetObjectRef                ( CObjectSAInterface *entity );
    CObjectSA*              GetObjectFromRef            ( unsigned int index );

    // Peds pool
    CPedSA*                 GetPed                      ( unsigned long ulID );
    CPedSA*                 GetPed                      ( CPedSAInterface *entity );
    unsigned int            GetPedRef                   ( CPed *ped );
    unsigned int            GetPedRef                   ( CPedSAInterface *ped );
    CPedSA*                 GetPedFromRef               ( unsigned int index );
    CPedSAInterface*        GetPedInterface             ( unsigned int index );

    // Common Utilities
    CEntitySA*              GetEntity                   ( CEntitySAInterface *entity );

    // Pool inline functions.
    // These should be used instead of the globals.
    inline CPtrNodeSinglePool*&    GetPtrNodeSinglePool         ( void )        { return *(CPtrNodeSinglePool**)CLASS_CPtrNodeSingleLinkPool; }
    inline CPtrNodeDoublePool*&    GetPtrNodeDoublePool         ( void )        { return *(CPtrNodeDoublePool**)CLASS_CPtrNodeDoubleLinkPool; }
    inline CEntryInfoPool*&        GetEntryInfoPool             ( void )        { return *(CEntryInfoPool**)CLASS_CEntryInfoNodePool; }

    inline CVehiclePool*&          GetVehiclePool               ( void )        { return *(CVehiclePool**)CLASS_CVehiclePool; }
    inline CPedPool*&              GetPedPool                   ( void )        { return *(CPedPool**)CLASS_CPedPool; }
    inline CBuildingPool*&         GetBuildingPool              ( void )        { return *(CBuildingPool**)CLASS_CBuildingPool; }
    inline CObjectPool*&           GetObjectPool                ( void )        { return *(CObjectPool**)CLASS_CObjectPool; }

    inline CDummyPool*&            GetDummyPool                 ( void )        { return *(CDummyPool**)CLASS_CDummyPool; }

    inline CColModelPool*&         GetColModelPool              ( void )        { return *(CColModelPool**)CLASS_CColModelPool; }

    inline CTaskPool*&             GetTaskPool                  ( void )        { return *(CTaskPool**)CLASS_CTaskPool; }
    inline CEventPool*&            GetEventPool                 ( void )        { return *(CEventPool**)CLASS_CEventPool; }
 
    inline CPointRoutePool*&       GetPointRoutePool            ( void )        { return *(CPointRoutePool**)CLASS_CPointRoutePool; }
    inline CPatrolRoutePool*&      GetPatrolRoutePool           ( void )        { return *(CPatrolRoutePool**)CLASS_CPatrolRoutePool; }
    inline CNodeRoutePool*&        GetNodeRoutePool             ( void )        { return *(CNodeRoutePool**)CLASS_CNodeRoutePool; }

    inline CTaskAllocatorPool*&    GetTaskAllocatorPool         ( void )        { return *(CTaskAllocatorPool**)CLASS_CTaskAllocatorPool; }
    inline CPedIntelligencePool*&  GetPedIntelligencePool       ( void )        { return *(CPedIntelligencePool**)CLASS_CPedIntelligencePool; }
    inline CPedAttractorPool*&     GetPedAttractorPool          ( void )        { return *(CPedAttractorPool**)CLASS_CPedAttractorPool; }
};

class CPoolsSA : public CPools
{
public:
                            CPoolsSA            ( );
                            ~CPoolsSA           ( );

    
    // Vehicles pool
    CVehicleSA*             AddVehicle          ( modelId_t model, unsigned char ucVariation, unsigned char ucVariation2 );
    CVehicleSA*             AddVehicle          ( DWORD* pGameInterface );
    void                    RemoveVehicle       ( CVehicle* pVehicle, bool bDelete = true );
    void                    RemoveVehicle       ( unsigned long ulID, bool bDelete = true );
    CVehicleSA*             GetVehicle          ( unsigned long ulID )                                  { return Pools::GetVehicle( ulID ); }
    CVehicleSA*             GetVehicle          ( void *entity )                                        { return Pools::GetVehicle( (CVehicleSAInterface*)entity ); }
    DWORD                   GetVehicleRef       ( CVehicle* pVehicle )                                  { return Pools::GetVehicleRef( pVehicle ); }
    DWORD                   GetVehicleRef       ( DWORD* pGameInterface )                               { return Pools::GetVehicleRef( (CVehicleSAInterface*)pGameInterface ); }
    CVehicleSA*             GetVehicleFromRef   ( DWORD dwGameRef )                                     { return Pools::GetVehicleFromRef( dwGameRef ); }
    inline unsigned long    GetVehicleCount     ( )                                                     { return Pools::GetVehiclePool()->GetCount(); }
    void                    DeleteAllVehicles   ( );

    // Objects pool
    CObjectSA*              AddObject           ( modelId_t dwModelID, bool bLowLod, bool bBreakable );
    void                    RemoveObject        ( CObject* pObject, bool bDelete = true );
    void                    RemoveObject        ( unsigned long ulID, bool bDelete = true );
    CObjectSA*              GetObject           ( unsigned long ulID )                                  { return Pools::GetObject( ulID ); }
    CObjectSA*              GetObject           ( void *entity )                                        { return Pools::GetObject( (CObjectSAInterface*)entity ); }
    DWORD                   GetObjectRef        ( CObject* pObject )                                    { return Pools::GetObjectRef( pObject ); }
    DWORD                   GetObjectRef        ( DWORD* pGameInterface )                               { return Pools::GetObjectRef( (CObjectSAInterface*)pGameInterface ); }
    CObjectSA*              GetObjectFromRef    ( DWORD dwGameRef )                                     { return Pools::GetObjectFromRef( dwGameRef ); }
    inline unsigned long    GetObjectCount      ( )                                                     { return Pools::GetObjectPool()->GetCount(); }
    void                    DeleteAllObjects    ( );

    // Peds pool
    CPedSA*                 AddPed              ( modelId_t model );
    CPedSA*                 AddPed              ( DWORD* pGameInterface );
    CPedSA*                 AddCivilianPed      ( DWORD* pGameInterface );
    void                    RemovePed           ( CPed* ped, bool bDelete = true );
    void                    RemovePed           ( unsigned long ulID, bool bDelete = true);
    CPedSA*                 GetPed              ( unsigned long ulID )                                  { return Pools::GetPed( ulID ); }
    CPedSA*                 GetPed              ( void *entity )                                        { return Pools::GetPed( (CPedSAInterface*)entity ); }
    DWORD                   GetPedRef           ( CPed* pPed )                                          { return Pools::GetPedRef( pPed ); }
    DWORD                   GetPedRef           ( DWORD* pGameInterface )                               { return Pools::GetPedRef( (CPedSAInterface*)pGameInterface ); }
    CPedSA*                 GetPedFromRef       ( DWORD dwGameRef )                                     { return Pools::GetPedFromRef( dwGameRef ); }
    CPedSAInterface*        GetPedInterface     ( DWORD dwGameRef )                                     { return Pools::GetPedInterface( dwGameRef ); } // game_sa specific
    inline unsigned long    GetPedCount         ( )                                                     { return Pools::GetPedPool()->GetCount(); }
    void                    DeleteAllPeds       ( );

    CEntitySA*              GetEntity           ( void *pGameInterface )                                { return Pools::GetEntity( (CEntitySAInterface*)pGameInterface ); }

    // Others
    CBuildingSA*            AddBuilding         ( DWORD dwModelID );
    void                    DeleteAllBuildings  ( );
    CVehicleSA*             AddTrain            ( CVector* vecPosition, DWORD dwModels[], int iSize, bool bDirection, unsigned char ucTrackID = 0xFF );

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

// Maintenance globals.
// Can be used instead of the CPools API.
// Is used by the classes to register themselves.
extern CVehicleSA *mtaVehicles[MAX_VEHICLES];
extern CObjectSA *mtaObjects[MAX_OBJECTS];
extern CPedSA *mtaPeds[MAX_PEDS];

#endif
