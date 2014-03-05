/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPoolsSA.cpp
*  PURPOSE:     Game entity pools
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Alberto Alonso <rydencillo@gmail.com>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define FUNC_InitGamePools  0x00550F10

CPtrNodeSinglePool      **ppPtrNodeSinglePool = (CPtrNodeSinglePool**)CLASS_CPtrNodeSingleLinkPool;
CPtrNodeDoublePool      **ppPtrNodeDoublePool = (CPtrNodeDoublePool**)CLASS_CPtrNodeDoubleLinkPool;
CEntryInfoPool          **ppEntryInfoPool = (CEntryInfoPool**)CLASS_CEntryInfoNodePool;

CVehiclePool            **ppVehiclePool = (CVehiclePool**)CLASS_CVehiclePool;
CPedPool                **ppPedPool = (CPedPool**)CLASS_CPedPool;
CBuildingPool           **ppBuildingPool = (CBuildingPool**)CLASS_CBuildingPool;
CObjectPool             **ppObjectPool = (CObjectPool**)CLASS_CObjectPool;

CDummyPool              **ppDummyPool = (CDummyPool**)CLASS_CDummyPool;

CColModelPool           **ppColModelPool = (CColModelPool**)CLASS_CColModelPool;

CTaskPool               **ppTaskPool = (CTaskPool**)CLASS_CTaskPool;
CEventPool              **ppEventPool = (CEventPool**)CLASS_CEventPool;

CPointRoutePool         **ppPointRoutePool = (CPointRoutePool**)CLASS_CPointRoutePool;
CPatrolRoutePool        **ppPatrolRoutePool = (CPatrolRoutePool**)CLASS_CPatrolRoutePool;
CNodeRoutePool          **ppNodeRoutePool = (CNodeRoutePool**)CLASS_CNodeRoutePool;

CTaskAllocatorPool      **ppTaskAllocatorPool = (CTaskAllocatorPool**)CLASS_CTaskAllocatorPool;
CPedIntelligencePool    **ppPedIntelligencePool = (CPedIntelligencePool**)CLASS_CPedIntelligencePool;
CPedAttractorPool       **ppPedAttractorPool = (CPedAttractorPool**)CLASS_CPedAttractorPool;

CPoolsSA::CPoolsSA( void )
{
    DEBUG_TRACE("CPoolsSA::CPoolsSA( void )");

    // Do not let GTA:SA init pools!
    MemPut <unsigned char> ( (unsigned char*)FUNC_InitGamePools, 0xC3 );

    *ppPtrNodeSinglePool = new CPtrNodeSinglePool();
    *ppPtrNodeDoublePool = new CPtrNodeDoublePool();
    *ppEntryInfoPool = new CEntryInfoPool();

    *ppVehiclePool = new CVehiclePool();
    *ppPedPool = new CPedPool();
    *ppBuildingPool = new CBuildingPool();
    *ppObjectPool = new CObjectPool();

    *ppDummyPool = new CDummyPool();
    *ppColModelPool = new CColModelPool();

    *ppTaskPool = new CTaskPool();
    *ppEventPool = new CEventPool();

    *ppPointRoutePool = new CPointRoutePool();
    *ppPatrolRoutePool = new CPatrolRoutePool();
    *ppNodeRoutePool = new CNodeRoutePool();

    *ppTaskAllocatorPool = new CTaskAllocatorPool();
    *ppPedIntelligencePool = new CPedIntelligencePool();
    *ppPedAttractorPool = new CPedAttractorPool();

    memset( mtaVehicles, 0, sizeof( mtaVehicles ) );
    memset( mtaObjects, 0, sizeof( mtaObjects ) );
    memset( mtaPeds, 0, sizeof( mtaPeds ) );

    EntryInfoNodePool = new CEntryInfoNodePoolSA();
    PointerNodeDoubleLinkPool = new CPointerNodeDoubleLinkPoolSA();
    PointerNodeSingleLinkPool = new CPointerNodeSingleLinkPoolSA();
}

CPoolsSA::~CPoolsSA ( void )
{
    // Make sure we've deleted all vehicles, objects, peds and buildings
    DeleteAllVehicles ();
    DeleteAllPeds ();
    DeleteAllObjects ();

    if ( EntryInfoNodePool )
        delete EntryInfoNodePool;

    if ( PointerNodeDoubleLinkPool )
        delete PointerNodeDoubleLinkPool;

    if ( PointerNodeSingleLinkPool )
        delete PointerNodeSingleLinkPool;

    delete *ppPtrNodeSinglePool;
    delete *ppPtrNodeDoublePool;
    delete *ppEntryInfoPool;

    // Seems to be crashy
#if TODO
    delete *ppVehiclePool;
    delete *ppPedPool;
    delete *ppBuildingPool;
    delete *ppObjectPool;

    delete *ppDummyPool;
#endif
    delete *ppColModelPool;

    delete *ppTaskPool;
    delete *ppEventPool;

    delete *ppPointRoutePool;
    delete *ppPatrolRoutePool;
    delete *ppNodeRoutePool;

    delete *ppTaskAllocatorPool;
    delete *ppPedIntelligencePool;
    delete *ppPedAttractorPool;
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                    VEHICLES POOL                                     //
//////////////////////////////////////////////////////////////////////////////////////////
CVehicleSA *mtaVehicles[MAX_VEHICLES];

CVehicleSA* CPoolsSA::AddVehicle( modelId_t model, unsigned char ucVariation, unsigned char ucVariation2 )
{
    DEBUG_TRACE("CVehicle* CPoolsSA::AddVehicle ( eVehicleTypes eVehicleType )");

    if ( (*ppVehiclePool)->Full() )
        return NULL;

    return new CVehicleSA ( model, ucVariation, ucVariation2 );
}

CVehicleSA* CPoolsSA::AddVehicle ( DWORD* pGameInterface )
{
    DEBUG_TRACE("CVehicle* CPoolsSA::AddVehicle ( DWORD* pGameInterface )");
    
    return new CVehicleSA ( (CVehicleSAInterface*)pGameInterface );
}

void CPoolsSA::RemoveVehicle ( unsigned long ulID, bool )
{
    DEBUG_TRACE("void CPoolsSA::RemoveVehicle ( unsigned long ulID, bool )");

    if ( ulID >= MAX_VEHICLES )
        return;

    // Delete it from memory
    delete mtaVehicles[ulID];
}

void CPoolsSA::RemoveVehicle ( CVehicle* pVehicle, bool bDelete )
{
    DEBUG_TRACE("void CPoolsSA::RemoveVehicle ( CVehicle * pVehicle, bool bDelete )");

    assert ( NULL != pVehicle );

    CVehicleSA* pVehicleSA = dynamic_cast < CVehicleSA* > ( pVehicle );
    if ( pVehicleSA )
    {
        RemoveVehicle ( pVehicleSA->GetPoolIndex(), bDelete );
    }
}

CVehicleSA* Pools::GetVehicle ( unsigned long ulID )
{
    DEBUG_TRACE("CVehicleSA* Pools::GetVehicle ( unsigned long ulID )");

    if ( ulID >= MAX_VEHICLES )
        return NULL;

    return mtaVehicles[ulID];
}

CVehicleSA* Pools::GetVehicle ( CVehicleSAInterface* pGameInterface )
{
    DEBUG_TRACE("CVehicleSA* Pools::GetVehicle ( CVehicleSAInterface* pGameInterface )");

    unsigned int id = Pools::GetVehiclePool()->GetIndex( pGameInterface );
    
    if ( id > MAX_VEHICLES-1 )
        return NULL;

    return mtaVehicles[id];
}

unsigned int Pools::GetVehicleRef ( CVehicle* pVehicle )
{
    DEBUG_TRACE("unsigned int Pools::GetVehicleRef ( CVehicle* pVehicle )");

    return pVehicle->GetPoolIndex();
}

unsigned int Pools::GetVehicleRef ( CVehicleSAInterface* pGameInterface )
{
    DEBUG_TRACE("unsigned int Pools::GetVehicleRef ( CVehicleSAInterface* pGameInterface )");

    return Pools::GetVehiclePool()->GetIndex( pGameInterface );
}

CVehicleSA* Pools::GetVehicleFromRef ( unsigned int dwGameRef )
{
    DEBUG_TRACE("CVehicleSA* Pools::GetVehicleFromRef ( unsigned int dwGameRef )");

    if ( dwGameRef >= MAX_VEHICLES )
        return NULL;

    return mtaVehicles[dwGameRef];
}

void CPoolsSA::DeleteAllVehicles ( )
{
    DEBUG_TRACE("void CPoolsSA::DeleteAllVehicles ( )");

    // TODO: not done yet
    Pools::GetVehiclePool()->Clear();

    memset( mtaVehicles, 0, sizeof( mtaVehicles ) );
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                     OBJECTS POOL                                     //
//////////////////////////////////////////////////////////////////////////////////////////
CObjectSA *mtaObjects[MAX_OBJECTS];

CObjectSA* CPoolsSA::AddObject( modelId_t dwModelID, bool bLowLod, bool bBreakable )
{
    DEBUG_TRACE("CObject * CPoolsSA::AddObject ( DWORD dwModelID )");

    if ( Pools::GetObjectPool()->Full() )
        return NULL;

    CObjectSA *pObject = new CObjectSA ( dwModelID, bBreakable );

    if ( bLowLod )
    {
        pObject->m_pInterface->bUsesCollision = 0;
        pObject->m_pInterface->bDontCastShadowsOn = 1; 
        // Set super hacky flag to indicate this is a special low lod object
        pObject->m_pInterface->SetIsLowLodEntity ();
    }
    else
        pObject->m_pInterface->SetIsHighLodEntity ();

    return pObject;
}

void CPoolsSA::RemoveObject ( unsigned long ulID, bool )
{
    DEBUG_TRACE("void CPoolsSA::RemoveObject ( unsigned long ulID, bool )");

    if ( ulID >= MAX_OBJECTS )
        return;

    // Delete it from memory
    delete mtaObjects[ulID];
}

void CPoolsSA::RemoveObject ( CObject* pObject, bool bDelete )
{
    DEBUG_TRACE("void CPoolsSA::RemoveObject ( CObject* pObject, bool bDelete )");

    assert ( NULL != pObject );

    CObjectSA* pObjectSA = dynamic_cast < CObjectSA* > ( pObject );
    if ( pObjectSA )
    {
        RemoveObject ( pObjectSA->GetPoolIndex(), bDelete );
    }
}

CObjectSA* Pools::GetObject ( unsigned long ulID )
{
    DEBUG_TRACE("CObjectSA* Pools::GetObject ( unsigned long ulID )");

    if ( ulID >= MAX_OBJECTS )
        return NULL;

    return mtaObjects[ulID];
}

CObjectSA* Pools::GetObject ( CObjectSAInterface* pGameInterface )
{
    DEBUG_TRACE("CObjectSA* Pools::GetObject ( CObjectSAInterface* pGameInterface )");

    unsigned int id = Pools::GetObjectPool()->GetIndex( pGameInterface );

    if ( id > MAX_OBJECTS-1 )
        return NULL;

    return mtaObjects[ id ];
}

unsigned int Pools::GetObjectRef ( CObject* pObject )
{
    DEBUG_TRACE("unsigned int Pools::GetObjectRef ( CObject* pObject )");

    return pObject->GetPoolIndex();
}

unsigned int Pools::GetObjectRef ( CObjectSAInterface* pGameInterface )
{
    DEBUG_TRACE("unsigned int Pools::GetObjectRef ( CObjectSAInterface* pGameInterface )");

    return Pools::GetObjectPool()->GetIndex( pGameInterface );
}

CObjectSA* Pools::GetObjectFromRef ( unsigned int dwGameRef )
{
    DEBUG_TRACE("CObjectSA* Pools::GetObjectFromRef ( unsigned int dwGameRef )");

    if ( dwGameRef >= MAX_OBJECTS )
        return NULL;

    return mtaObjects[dwGameRef];
}

void CPoolsSA::DeleteAllObjects ( )
{
    DEBUG_TRACE("void CPoolsSA::DeleteAllObjects ( )");

#if 0
    Pools::GetObjectPool()->Clear();

    memset( mtaObjects, 0, sizeof( mtaObjects ) );
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                       PEDS POOL                                      //
//////////////////////////////////////////////////////////////////////////////////////////
CPedSA *mtaPeds[MAX_PEDS];

CPedSA* CPoolsSA::AddPed( modelId_t model )
{
    DEBUG_TRACE("CPed* CPoolsSA::AddPed ( ePedModel ePedType )");

    if ( Pools::GetPedPool()->Full() )
        return NULL;

    return new CPlayerPedSA ( model );
}

CPedSA* CPoolsSA::AddPed ( DWORD* pGameInterface )
{
    DEBUG_TRACE("CPed* CPoolsSA::AddPed ( DWORD* pGameInterface )");

    return new CPlayerPedSA( (CPlayerPedSAInterface*)pGameInterface );
}

CPedSA* CPoolsSA::AddCivilianPed ( DWORD* pGameInterface )
{
    DEBUG_TRACE("CPed* CPoolsSA::AddCivilianPed ( DWORD* pGameInterface )");

    if ( Pools::GetPedPool()->Full() )
        return NULL;

    return new CCivilianPedSA ( (CCivilianPedSAInterface*)pGameInterface );
}

void CPoolsSA::RemovePed ( unsigned long ulID, bool bDelete )
{
    DEBUG_TRACE("void CPoolsSA::RemovePed ( unsigned long ulID, bool bDelete )");

    if ( ulID >= MAX_PEDS )
        return;

    CPedSA *pPedSA = mtaPeds[ulID];

    if ( !pPedSA )
        return;

    // Delete the element from memory
    switch ( pPedSA->GetType () )
    {
        case PLAYER_PED:
        {
            CPlayerPedSA* pPlayerPed = dynamic_cast < CPlayerPedSA* > ( pPedSA );
            if ( pPlayerPed )
            {
                if ( ! bDelete )
                    pPlayerPed->SetDoNotRemoveFromGameWhenDeleted ( true );
            }

            delete pPlayerPed;

            break;
        }

        default:
        {
            CCivilianPedSA* pCivPed = dynamic_cast < CCivilianPedSA* > ( pPedSA );
            if ( pCivPed )
            {
                if ( ! bDelete )
                    pCivPed->SetDoNotRemoveFromGameWhenDeleted ( true );
            }

            delete pCivPed;

            break;
        }
    }
}

void CPoolsSA::RemovePed ( CPed* pPed, bool bDelete )
{
    DEBUG_TRACE("void CPoolsSA::RemovePed ( CPed* pPed, bool bDelete )");

    assert ( NULL != pPed );

    CPedSA* pPedSA = dynamic_cast < CPedSA* > ( pPed );
    if ( pPedSA )
    {
        RemovePed ( pPedSA->GetPoolIndex(), bDelete );
    }
}

CPedSA* Pools::GetPed ( unsigned long ulID )
{
    DEBUG_TRACE("CPedSA* Pools::GetPed ( unsigned long ulID )");

    if ( ulID >= MAX_PEDS )
        return NULL;

    return mtaPeds[ulID];
}

CPedSA* Pools::GetPed ( CPedSAInterface* pGameInterface )
{
    DEBUG_TRACE("CPedSA* Pools::GetPed ( CPedSAInterface* pGameInterface )");

    // 0x00400000 is used for bad player pointers some places in GTA
    // The_GTA: where exactly does this occur? is it important?
    if ( pGameInterface && (DWORD*)pGameInterface != (DWORD*)0x00400000 )
    {
        unsigned int id = Pools::GetPedPool()->GetIndex( pGameInterface );

        if ( id < MAX_PEDS )
            return mtaPeds[ id ];
    }

    return NULL;
}

unsigned int Pools::GetPedRef ( CPed* pPed )
{
    DEBUG_TRACE("unsigned int Pools::GetPedRef ( CPed* pPed )");

    return pPed->GetPoolIndex();
}

unsigned int Pools::GetPedRef ( CPedSAInterface* pGameInterface )
{
    DEBUG_TRACE("unsigned int Pools::GetPedRef ( CPedSAInterface* pGameInterface )");

    return Pools::GetPedPool()->GetIndex( pGameInterface );
}

CPedSA* Pools::GetPedFromRef ( unsigned int dwGameRef )
{
    DEBUG_TRACE("CPedSA* Pools::GetPedFromRef ( unsigned int dwGameRef )");

    if ( dwGameRef >= MAX_PEDS )
        return NULL;

    return mtaPeds[dwGameRef];
}

CPedSAInterface* Pools::GetPedInterface ( unsigned int dwGameRef )
{
    DEBUG_TRACE("CPedSAInterface* Pools::GetPedInterface ( unsigned int dwGameRef )");

    return Pools::GetPedPool()->Get( dwGameRef );
}

void CPoolsSA::DeleteAllPeds ( )
{
    DEBUG_TRACE("void CPoolsSA::DeleteAllPeds ( )");

#if 0
    Pools::GetPedPool()->Clear();

    memset( mtaPeds, 0, sizeof( mtaPeds ) );
#endif
}

CEntitySA* Pools::GetEntity ( CEntitySAInterface *pGameInterface )
{
    if ( pGameInterface )
    {
        CEntitySAInterface *pEntityInterface = pGameInterface;

        switch ( pEntityInterface->nType )
        {
        case ENTITY_TYPE_PED:           return GetPed ( (CPedSAInterface*)pGameInterface );
        case ENTITY_TYPE_VEHICLE:       return GetVehicle ( (CVehicleSAInterface*)pGameInterface );
        case ENTITY_TYPE_OBJECT:        return GetObject ( (CObjectSAInterface*)pGameInterface );
        }
    }

    return NULL;
}

CBuildingSA* CPoolsSA::AddBuilding ( DWORD dwModelID )
{
    DEBUG_TRACE("CBuilding * CPoolsSA::AddBuilding ( DWORD dwModelID )");

#if 0
    if(m_ulBuildingCount <= MAX_BUILDINGS)
    {
        for(int i = 0;i<MAX_BUILDINGS;i++)
        {
            if(Buildings[i] == 0)
            {
                CBuildingSA * pBuilding = new CBuildingSA(dwModelID);
                Buildings[i] = pBuilding;
                pBuilding->SetArrayID ( i );
                m_ulBuildingCount++;

                return pBuilding;
            }
        }
    }
#endif

    return NULL;
}

CVehicleSA* CPoolsSA::AddTrain ( CVector * vecPosition, DWORD dwModels[], int iSize, bool bDirection, uchar ucTrackId )
{
    DEBUG_TRACE("CVehicle* CPoolsSA::AddTrain ( CVector * vecPosition, DWORD dwModels[], int iSize, bool bDirection )");

    // clean the existing array
    MemSetFast ( (void *)VAR_TrainModelArray, 0, 32 * sizeof(DWORD) );

    // now load the models we're going to use and add them to the array
    for ( int i = 0; i < iSize; i++ )
    {
        if ( dwModels[i] == 449 || dwModels[i] == 537 || 
            dwModels[i] == 538 || dwModels[i] == 569 || 
            dwModels[i] == 590 || dwModels[i] == 570 )
        {
            MemPutFast < DWORD > ( VAR_TrainModelArray + i * 4, dwModels[i] );
        }
    }

    CVehicleSAInterface* pTrainBeginning = NULL;
    CVehicleSAInterface* pTrainEnd = NULL;

    float fX = vecPosition->fX;
    float fY = vecPosition->fY;
    float fZ = vecPosition->fZ;

    // Disable GetVehicle because CreateMissionTrain calls it before our CVehicleSA instance is inited
    //m_bGetVehicleEnabled = false;

    // Find closest track node
    float fRailDistance;
    int iNodeId = pGame->GetWorld ()->FindClosestRailTrackNode ( *vecPosition, ucTrackId, fRailDistance );
    int iDesiredTrackId = ucTrackId;

    DWORD dwFunc = FUNC_CTrain_CreateMissionTrain;
    _asm
    {
        push    0 // place as close to point as possible (rather than at node)? (maybe) (actually seems to have an effect on the speed, so changed from 1 to 0)
        push    iDesiredTrackId // track ID
        push    iNodeId // node to start at (-1 for closest node)
        lea     ecx, pTrainEnd
        push    ecx // end of train
        lea     ecx, pTrainBeginning 
        push    ecx // begining of train
        push    0 // train type (always use 0 as thats where we're writing to)
        push    bDirection // direction 
        push    fZ // z
        push    fY // y
        push    fX // x
        call    dwFunc
        add     esp, 0x28   
    }

    // Enable GetVehicle
    //m_bGetVehicleEnabled = true;

    CVehicleSA * trainHead = NULL;
    if ( pTrainBeginning )
    {
        DWORD vehicleIndex = 0;

        if ( !(*ppVehiclePool)->Full() )
        {
            trainHead = new CVehicleSA ( pTrainBeginning );
        }

        CVehicleSA * carriage = trainHead;
        
        while ( carriage )
        {
            if ( !(*ppVehiclePool)->Full() )
            {
                CVehicleSAInterface* vehCarriage = carriage->GetNextCarriageInTrain ();
                if ( vehCarriage )
                {
                    carriage = new CVehicleSA ( vehCarriage );
                }
                else
                    carriage = NULL;
            }
        }
    }

    // Stops the train from moving at ludacrist speeds right after creation
    // due to some glitch in the node finding in CreateMissionTrain
    CVector vec(0, 0, 0);
    trainHead->SetMoveSpeed ( &vec );

    return trainHead;
}


char szOutput[1024];

void CPoolsSA::DumpPoolsStatus ()
{
    char*  poolNames[] = {"Buildings", "Peds", "Objects", "Dummies", "Vehicles", "ColModels", 
        "Tasks", "Events", "TaskAllocators", "PedIntelligences", "PedAttractors", 
        "EntryInfoNodes", "NodeRoutes", "PatrolRoutes", "PointRoutes", 
        "PointerNodeDoubleLinks", "PointerNodeSingleLinks" };

    int poolSizes[] = {13000,140,350,2500,110,10150,500,200,16,140,64,500,64,32,64,3200,70000};
    int iPosition = 0;
    char percent = '%';
    iPosition += snprintf ( szOutput, 1024, "-----------------\n" );
    for ( int i = 0; i < MAX_POOLS; i++ )
    {
        int usedSpaces = GetNumberOfUsedSpaces ( (ePools)i );
        iPosition += snprintf ( szOutput + iPosition, 1024 - iPosition, "%s: %d (%d) (%.2f%c)\n", poolNames[i], usedSpaces, poolSizes[i], ((float)usedSpaces / (float)poolSizes[i] * 100), percent  );
    }
    #ifdef MTA_DEBUG
    OutputDebugString ( szOutput );
    #endif
}


int CPoolsSA::GetPoolDefaultCapacity ( ePools pool )
{
    switch ( pool )
    {
        case BUILDING_POOL:             return 13000;
        case PED_POOL:                  return 140;
        case OBJECT_POOL:               return 350;          // Modded to 700   @ CGameSA.cpp
        case DUMMY_POOL:                return 2500;
        case VEHICLE_POOL:              return 110;
        case COL_MODEL_POOL:            return 10150;        // Modded to 12000  @ CGameSA.cpp
        case TASK_POOL:                 return 500;          // Modded to 5000   @ CGameSA.cpp
        case EVENT_POOL:                return 200;          // Modded to 5000   @ CGameSA.cpp
        case TASK_ALLOCATOR_POOL:       return 16;
        case PED_INTELLIGENCE_POOL:     return 140;
        case PED_ATTRACTOR_POOL:        return 64;
        case ENTRY_INFO_NODE_POOL:      return 500;          // Modded to 4096   @ CMultiplayerSA.cpp
        case NODE_ROUTE_POOL:           return 64;
        case PATROL_ROUTE_POOL:         return 32;
        case POINT_ROUTE_POOL:          return 64;
        case POINTER_DOUBLE_LINK_POOL:  return 3200;         // Modded to 8000   @ CMultiplayerSA.cpp
        case POINTER_SINGLE_LINK_POOL:  return 70000;
        case ENV_MAP_MATERIAL_POOL:     return 4096;         // Modded to 16000   @ CGameSA.cpp
        case ENV_MAP_ATOMIC_POOL:       return 1024;         // Modded to 8000    @ CGameSA.cpp
        case SPEC_MAP_MATERIAL_POOL:    return 4096;         // Modded to 16000   @ CGameSA.cpp
    }
    return 0;
}


int CPoolsSA::GetPoolCapacity ( ePools pool )
{
    DWORD iPtr = NULL;
    DWORD cPtr = NULL;
    switch ( pool )
    {
        case BUILDING_POOL:             iPtr = 0x55105F; break;
        case PED_POOL:                  iPtr = 0x550FF2; break;
        case OBJECT_POOL:               iPtr = 0x551097; break;
        case DUMMY_POOL:                iPtr = 0x5510CF; break;
        case VEHICLE_POOL:              cPtr = 0x55102A; break;
        case COL_MODEL_POOL:            iPtr = 0x551107; break;
        case TASK_POOL:                 iPtr = 0x55113F; break;
        case EVENT_POOL:                iPtr = 0x551177; break;
        case TASK_ALLOCATOR_POOL:       cPtr = 0x55124E; break;
        case PED_INTELLIGENCE_POOL:     iPtr = 0x551283; break;
        case PED_ATTRACTOR_POOL:        cPtr = 0x5512BC; break;
        case ENTRY_INFO_NODE_POOL:      iPtr = 0x550FBA; break;
        case NODE_ROUTE_POOL:           cPtr = 0x551219; break;
        case PATROL_ROUTE_POOL:         cPtr = 0x5511E4; break;
        case POINT_ROUTE_POOL:          cPtr = 0x5511AF; break;
        case POINTER_DOUBLE_LINK_POOL:  iPtr = 0x550F82; break;
        case POINTER_SINGLE_LINK_POOL:  iPtr = 0x550F46; break;
        case ENV_MAP_MATERIAL_POOL:     iPtr = 0x5DA08E; break;
        case ENV_MAP_ATOMIC_POOL:       iPtr = 0x5DA0CA; break;
        case SPEC_MAP_MATERIAL_POOL:    iPtr = 0x5DA106; break;
    }
    if ( iPtr )
        return *(int*)iPtr;

    if ( cPtr )
        return *(char*)cPtr;

    return 0;
}


// Must be called before CPools::Initialise()
// The_GTA: do we really need this?
// Changing pool capacity is dangerous; letting API access can lead to crashes (i.e. TXD pool hardcode limit to 5000)
void CPoolsSA::SetPoolCapacity ( ePools pool, int iValue )
{
    DWORD iPtr = NULL;
    DWORD cPtr = NULL;
    switch ( pool )
    {
        case BUILDING_POOL:             iPtr = 0x55105F; break;
        case PED_POOL:                  iPtr = 0x550FF2; break;
        case OBJECT_POOL:               iPtr = 0x551097; break;
        case DUMMY_POOL:                iPtr = 0x5510CF; break;
        case VEHICLE_POOL:              iPtr = 0x55102A; break;
        case COL_MODEL_POOL:            iPtr = 0x551107; break;
        case TASK_POOL:                 iPtr = 0x55113F; break;
        case EVENT_POOL:                iPtr = 0x551177; break;
        case TASK_ALLOCATOR_POOL:       cPtr = 0x55124E; break;     // 0 - 127
        case PED_INTELLIGENCE_POOL:     iPtr = 0x551283; break;
        case PED_ATTRACTOR_POOL:        cPtr = 0x5512BB; break;     // 0 - 127
        case ENTRY_INFO_NODE_POOL:      iPtr = 0x550FBA; break;
        case NODE_ROUTE_POOL:           cPtr = 0x551218; break;     // 0 - 127
        case PATROL_ROUTE_POOL:         cPtr = 0x5511E4; break;     // 0 - 127
        case POINT_ROUTE_POOL:          cPtr = 0x5511AF; break;     // 0 - 127
        case POINTER_DOUBLE_LINK_POOL:  iPtr = 0x550F82; break;
        case POINTER_SINGLE_LINK_POOL:  iPtr = 0x550F46; break;
        case ENV_MAP_MATERIAL_POOL:     iPtr = 0x5DA08E; break;
        case ENV_MAP_ATOMIC_POOL:       iPtr = 0x5DA0CA; break;
        case SPEC_MAP_MATERIAL_POOL:    iPtr = 0x5DA106; break;
    }
    if ( iPtr )
        MemPut < int > ( iPtr, iValue );

    if ( cPtr )
        MemPut < char > ( cPtr, iValue );
}


int CPoolsSA::GetNumberOfUsedSpaces ( ePools pool )
{
    switch ( pool )
    {
    case BUILDING_POOL:                 return (*ppBuildingPool)->GetCount();
    case PED_POOL:                      return (*ppPedPool)->GetCount();
    case OBJECT_POOL:                   return (*ppObjectPool)->GetCount();
    case DUMMY_POOL:                    return (*ppDummyPool)->GetCount();
    case VEHICLE_POOL:                  return (*ppVehiclePool)->GetCount();
    case COL_MODEL_POOL:                return (*ppColModelPool)->GetCount();
    case TASK_POOL:                     return (*ppTaskPool)->GetCount();
    case EVENT_POOL:                    return (*ppEventPool)->GetCount();
    case TASK_ALLOCATOR_POOL:           return (*ppTaskAllocatorPool)->GetCount();
    case PED_INTELLIGENCE_POOL:         return (*ppPedIntelligencePool)->GetCount();
    case PED_ATTRACTOR_POOL:            return (*ppPedAttractorPool)->GetCount();
    case ENTRY_INFO_NODE_POOL:          return (*ppEntryInfoPool)->GetCount();
    case NODE_ROUTE_POOL:               return (*ppNodeRoutePool)->GetCount();
    case PATROL_ROUTE_POOL:             return (*ppPatrolRoutePool)->GetCount();
    case POINT_ROUTE_POOL:              return (*ppPointRoutePool)->GetCount();
    case POINTER_DOUBLE_LINK_POOL:      return (*ppPtrNodeDoublePool)->GetCount();
    case POINTER_SINGLE_LINK_POOL:      return (*ppPtrNodeSinglePool)->GetCount();
    case ENV_MAP_MATERIAL_POOL:         return (*ppEnvMapMaterialPool)->GetCount();
    case ENV_MAP_ATOMIC_POOL:           return (*ppEnvMapAtomicPool)->GetCount();
    case SPEC_MAP_MATERIAL_POOL:        return (*ppSpecMapMaterialPool)->GetCount();
    }

    return -1;
}

CEntryInfoNodePool * CPoolsSA::GetEntryInfoNodePool ( void )
{
    return EntryInfoNodePool;
}

int CEntryInfoNodePoolSA::GetNumberOfUsedSpaces ( void )
{
    return (*ppEntryInfoPool)->GetCount();
}

CPointerNodeDoubleLinkPool * CPoolsSA::GetPointerNodeDoubleLinkPool ( void )
{
    return PointerNodeDoubleLinkPool;
}

int CPointerNodeDoubleLinkPoolSA::GetNumberOfUsedSpaces ( void )
{
    return (*ppPtrNodeDoublePool)->GetCount();
}

CPointerNodeSingleLinkPool * CPoolsSA::GetPointerNodeSingleLinkPool ( void )
{
    return PointerNodeSingleLinkPool;
}

int CPointerNodeSingleLinkPoolSA::GetNumberOfUsedSpaces ( void )
{
    return (*ppPtrNodeSinglePool)->GetCount();
}
