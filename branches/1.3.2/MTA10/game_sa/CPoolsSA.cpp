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
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPoolsSA::CPoolsSA()
{
    DEBUG_TRACE("CPoolsSA::CPoolsSA()");
    
    m_bGetVehicleEnabled = true;
    m_ulBuildingCount= 0;

    MemSetFast (&Buildings,0,sizeof(CBuilding *) * MAX_BUILDINGS);

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
    DeleteAllBuildings ();

    if ( EntryInfoNodePool )
        delete EntryInfoNodePool;

    if ( PointerNodeDoubleLinkPool )
        delete PointerNodeDoubleLinkPool;

    if ( PointerNodeSingleLinkPool )
        delete PointerNodeSingleLinkPool;
}

void CPoolsSA::DeleteAllBuildings ( void )
{
    /*
    for ( int i = 0; i < MAX_BUILDINGS; i++ )
    {
        if ( Buildings [i] )
        {
            RemoveBuilding ( Buildings [i] );
        }
    }
    */
}


//////////////////////////////////////////////////////////////////////////////////////////
//                                    VEHICLES POOL                                     //
//////////////////////////////////////////////////////////////////////////////////////////
inline bool CPoolsSA::AddVehicleToPool ( CVehicleSA* pVehicle )
{
    DEBUG_TRACE("inline bool CPoolsSA::AddVehicleToPool ( CVehicleSA* pVehicle )");

    // We always add to the end of the pool array because
    // it's ensured that there won't be empty spaces in the
    // middle of the array.
    unsigned long ulNewPos = m_vehiclePool.ulCount;

    // Grab the interface
    CVehicleSAInterface* pInterface = pVehicle->GetVehicleInterface ();

    if ( ! pInterface )
    {
        return false;
    }
    else
    {
        // Add it to the pool array
        m_vehiclePool.array [ ulNewPos ] = pVehicle;
        pVehicle->SetArrayID ( ulNewPos );

        // Add it to the pool map
        m_vehiclePool.map.insert ( vehiclePool_t::mapType::value_type ( pInterface, pVehicle ) );

        // Increase the count of vehicles
        ++m_vehiclePool.ulCount;
    }

    return true;
}

CVehicle* CPoolsSA::AddVehicle ( eVehicleTypes eVehicleType, unsigned char ucVariation, unsigned char ucVariation2 )
{
    DEBUG_TRACE("CVehicle* CPoolsSA::AddVehicle ( eVehicleTypes eVehicleType )");
    CVehicleSA* pVehicle = NULL;

    if ( m_vehiclePool.ulCount < MAX_VEHICLES )
    {
        pVehicle = new CVehicleSA ( eVehicleType, ucVariation, ucVariation2 );
        if ( ! AddVehicleToPool ( pVehicle ) )
        {
            delete pVehicle;
            pVehicle = NULL;
        }
    }

    return pVehicle;
}

CVehicle* CPoolsSA::AddVehicle ( DWORD* pGameInterface )
{
    DEBUG_TRACE("CVehicle* CPoolsSA::AddVehicle ( DWORD* pGameInterface )");
    CVehicleSA* pVehicle = NULL;

    if ( m_vehiclePool.ulCount < MAX_VEHICLES )
    {
        CVehicleSAInterface* pInterface = reinterpret_cast < CVehicleSAInterface* > ( pGameInterface );
        if ( pInterface )
        {
            // Make sure that it's not already in the pool
            vehiclePool_t::mapType::iterator iter = m_vehiclePool.map.find ( pInterface );
            if ( iter != m_vehiclePool.map.end () )
            {
                pVehicle = (*iter).second;
            }
            else
            {
                // Create it
                pVehicle = new CVehicleSA ( pInterface );
                if ( ! AddVehicleToPool ( pVehicle ) )
                {
                    delete pVehicle;
                    pVehicle = NULL;
                }
            }
        }
    }

    return pVehicle;
}

void CPoolsSA::RemoveVehicle ( unsigned long ulID, bool )
{
    DEBUG_TRACE("void CPoolsSA::RemoveVehicle ( unsigned long ulID, bool )");

    static bool bIsDeletingVehicleAlready = false; // to prevent delete being called twice
    if ( !bIsDeletingVehicleAlready ) 
    {
        bIsDeletingVehicleAlready = true;

        CVehicleSA* pVehicleSA = m_vehiclePool.array [ ulID ];
        assert ( NULL != pVehicleSA );

        // Pop the element to remove from the pool array
        if ( ulID != m_vehiclePool.ulCount - 1 )
        {
            // We are removing an intermediate position of
            // the array. Move the last element to the just
            // deleted element position to not allow empty
            // spaces on it.
            m_vehiclePool.array [ ulID ] = m_vehiclePool.array [ m_vehiclePool.ulCount - 1 ];
            m_vehiclePool.array [ ulID ]->SetArrayID ( ulID );
        }
        m_vehiclePool.array [ m_vehiclePool.ulCount - 1 ] = NULL;

        // Unlink the element to remove from the pool map
        vehiclePool_t::mapType::iterator iter = m_vehiclePool.map.find ( pVehicleSA->GetVehicleInterface () );
        if ( iter != m_vehiclePool.map.end () )
        {
            m_vehiclePool.map.erase ( iter );
        }

        // Delete it from memory
        delete pVehicleSA;

        // Decrease the count of elements in the pool
        --m_vehiclePool.ulCount;

        bIsDeletingVehicleAlready = false;
    }
}

void CPoolsSA::RemoveVehicle ( CVehicle* pVehicle, bool bDelete )
{
    DEBUG_TRACE("void CPoolsSA::RemoveVehicle ( CVehicle * pVehicle, bool bDelete )");

    assert ( NULL != pVehicle );

    CVehicleSA* pVehicleSA = dynamic_cast < CVehicleSA* > ( pVehicle );
    if ( pVehicleSA )
    {
        RemoveVehicle ( pVehicleSA->GetArrayID (), bDelete );
    }
}

CVehicle* CPoolsSA::GetVehicle ( unsigned long ulID )
{
    DEBUG_TRACE("CVehicle* CPoolsSA::GetVehicle ( unsigned long ulID )");

    assert ( ulID < MAX_VEHICLES );

    return m_vehiclePool.array [ ulID ];
}

CVehicle* CPoolsSA::GetVehicle ( DWORD* pGameInterface )
{
    DEBUG_TRACE("CVehicle* CPoolsSA::GetVehicle ( DWORD* pGameInterface )");

    assert ( pGameInterface );

    if ( m_bGetVehicleEnabled )
    {
        CVehicleSAInterface* pInterface = reinterpret_cast < CVehicleSAInterface* > ( pGameInterface );

        // Lookup in the pool map for the vehicle related to this interface.
        vehiclePool_t::mapType::iterator iter = m_vehiclePool.map.find ( pInterface );
        if ( iter != m_vehiclePool.map.end () )
        {
            return (*iter).second;
        }
    }

    return NULL;
}

DWORD CPoolsSA::GetVehicleRef ( CVehicle* pVehicle )
{
    DEBUG_TRACE("DWORD CPoolsSA::GetVehicleRef ( CVehicle* pVehicle )");

    DWORD dwRef = 0;
    CVehicleSA* pVehicleSA = dynamic_cast < CVehicleSA * > ( pVehicle );
    if ( pVehicleSA )
    {
        CVehicleSAInterface* pInterface = pVehicleSA->GetVehicleInterface ();
        DWORD dwFunc = FUNC_GetVehicleRef;
        _asm
        {
            push    pInterface
            call    dwFunc
            add     esp, 0x4
            mov     dwRef, eax
        }
    }

    return dwRef;
}

DWORD CPoolsSA::GetVehicleRef ( DWORD* pGameInterface )
{
    DEBUG_TRACE("DWORD CPoolsSA::GetVehicleRef ( DWORD* pGameInterface )");

    DWORD dwRef = 0;
    CVehicleSAInterface* pInterface = reinterpret_cast < CVehicleSAInterface * > ( pGameInterface );
    if ( pInterface )
    {
        DWORD dwFunc = FUNC_GetVehicleRef;
        _asm
        {
            push    pInterface
            call    dwFunc
            add     esp, 0x4
            mov     dwRef, eax
        }
    }

    return dwRef;
}

CVehicle* CPoolsSA::GetVehicleFromRef ( DWORD dwGameRef )
{
    DEBUG_TRACE("CVehicle* CPoolsSA::GetVehicleFromRef ( DWORD dwGameRef )");

    DWORD dwReturn;
    DWORD dwFunction = FUNC_GetVehicle;

    _asm
    {
        mov     ecx, dword ptr ds:[CLASS_CPool_Vehicle]
        push    dwGameRef
        call    dwFunction
        add     esp, 0x4
        mov     dwReturn, eax
    }

    CVehicleSAInterface* pInterface = (CVehicleSAInterface*)dwReturn;
    if ( pInterface )
    {
        // We have a special slot in vehicles GTA interface pointing to
        // our game_sa instance for it.
        return pInterface->m_pVehicle;
    }

    return NULL;
}

void CPoolsSA::DeleteAllVehicles ( )
{
    DEBUG_TRACE("void CPoolsSA::DeleteAllVehicles ( )");

    while ( m_vehiclePool.ulCount > 0 )
        RemoveVehicle ( m_vehiclePool.ulCount - 1 );
    m_vehiclePool.map.clear ();
}









//////////////////////////////////////////////////////////////////////////////////////////
//                                     OBJECTS POOL                                     //
//////////////////////////////////////////////////////////////////////////////////////////
inline bool CPoolsSA::AddObjectToPool ( CObjectSA* pObject )
{
    DEBUG_TRACE("inline bool CPoolsSA::AddObjectToPool ( CObjectSA* pObject )");

    // We always add to the end of the pool array because
    // it's ensured that there won't be empty spaces in the
    // middle of the array.
    unsigned long ulNewPos = m_objectPool.ulCount;

    // Grab the new object interface
    CObjectSAInterface* pInterface = pObject->GetObjectInterface ();

    if ( ! pInterface )
    {
        return false;
    }
    else
    {
        // Add it to the pool array
        m_objectPool.array [ ulNewPos ] = pObject;
        pObject->SetArrayID ( ulNewPos );

        // Add it to the pool map
        m_objectPool.map.insert ( objectPool_t::mapType::value_type ( pInterface, pObject ) );

        // Increase the count of objects
        ++m_objectPool.ulCount;
    }

    return true;
}

CObject* CPoolsSA::AddObject ( DWORD dwModelID, bool bLowLod, bool bBreakable )
{
    DEBUG_TRACE("CObject * CPoolsSA::AddObject ( DWORD dwModelID )");

    CObjectSA* pObject = NULL;

    if ( m_objectPool.ulCount < MAX_OBJECTS )
    {
        pObject = new CObjectSA ( dwModelID, bBreakable );

        if ( bLowLod )
        {
            pObject->m_pInterface->bUsesCollision = 0;
            pObject->m_pInterface->bDontCastShadowsOn = 1; 
            // Set super hacky flag to indicate this is a special low lod object
            pObject->m_pInterface->SetIsLowLodEntity ();
        }
        else
            pObject->m_pInterface->SetIsHighLodEntity ();


        if ( ! AddObjectToPool ( pObject ) )
        {
            delete pObject;
            pObject = NULL;
        }
    }

    return pObject;
}

void CPoolsSA::RemoveObject ( unsigned long ulID, bool )
{
    DEBUG_TRACE("void CPoolsSA::RemoveObject ( unsigned long ulID, bool )");

    static bool bIsDeletingObjectAlready = false; // to prevent delete being called twice
    if ( !bIsDeletingObjectAlready ) 
    {
        bIsDeletingObjectAlready = true;

        CObjectSA* pObjectSA = m_objectPool.array [ ulID ];
        assert ( NULL != pObjectSA );

        // Pop the element to remove from the pool array
        if ( ulID != m_objectPool.ulCount - 1 )
        {
            // We are removing an intermediate position of
            // the array. Move the last element to the just
            // deleted element position to not allow empty
            // spaces on it.
            m_objectPool.array [ ulID ] = m_objectPool.array [ m_objectPool.ulCount - 1 ];
            m_objectPool.array [ ulID ]->SetArrayID ( ulID );
        }
        m_objectPool.array [ m_objectPool.ulCount - 1 ] = NULL;

        // Unlink the element to remove from the pool map
        objectPool_t::mapType::iterator iter = m_objectPool.map.find ( pObjectSA->GetObjectInterface () );
        if ( iter != m_objectPool.map.end () )
        {
            m_objectPool.map.erase ( iter );
        }

        // Delete it from memory
        delete pObjectSA;

        // Decrease the count of elements in the pool
        --m_objectPool.ulCount;

        bIsDeletingObjectAlready = false;
    }
}

void CPoolsSA::RemoveObject ( CObject* pObject, bool bDelete )
{
    DEBUG_TRACE("void CPoolsSA::RemoveObject ( CObject* pObject, bool bDelete )");

    assert ( NULL != pObject );

    CObjectSA* pObjectSA = dynamic_cast < CObjectSA* > ( pObject );
    if ( pObjectSA )
    {
        RemoveObject ( pObjectSA->GetArrayID (), bDelete );
    }
}

CObject* CPoolsSA::GetObject ( unsigned long ulID )
{
    DEBUG_TRACE("CObject* CPoolsSA::GetObject ( unsigned long ulID )");

    assert ( ulID < MAX_OBJECTS );

    return m_objectPool.array [ ulID ];
}

CObject* CPoolsSA::GetObject ( DWORD* pGameInterface )
{
    DEBUG_TRACE("CObject* CPoolsSA::GetObject ( DWORD* pGameInterface )");

    CObjectSAInterface* pInterface = reinterpret_cast < CObjectSAInterface* > ( pGameInterface );

    if ( pInterface )
    {
        // Lookup in the pool map for the object related to this interface.
        objectPool_t::mapType::iterator iter = m_objectPool.map.find ( pInterface );
        if ( iter != m_objectPool.map.end () )
        {
            return (*iter).second;
        }
    }

    return NULL;
}

DWORD CPoolsSA::GetObjectRef ( CObject* pObject )
{
    DEBUG_TRACE("DWORD CPoolsSA::GetObjectRef ( CObject* pObject )");

    DWORD dwRef = 0;
    CObjectSA* pObjectSA = dynamic_cast < CObjectSA * > ( pObject );
    if ( pObjectSA )
    {
        CObjectSAInterface* pInterface = pObjectSA->GetObjectInterface ();
        DWORD dwFunc = FUNC_GetObjectRef;
        _asm
        {
            push    pInterface
            call    dwFunc
            add     esp, 0x4
            mov     dwRef, eax
        }
    }

    return dwRef;
}

DWORD CPoolsSA::GetObjectRef ( DWORD* pGameInterface )
{
    DEBUG_TRACE("DWORD CPoolsSA::GetObjectRef ( DWORD* pGameInterface )");

    DWORD dwRef = 0;
    CObjectSAInterface* pInterface = reinterpret_cast < CObjectSAInterface * > ( pGameInterface );
    if ( pInterface )
    {
        DWORD dwFunc = FUNC_GetObjectRef;
        _asm
        {
            push    pInterface
            call    dwFunc
            add     esp, 0x4
            mov     dwRef, eax
        }
    }

    return dwRef;
}

CObject* CPoolsSA::GetObjectFromRef ( DWORD dwGameRef )
{
    DEBUG_TRACE("CObject* CPoolsSA::GetObjectFromRef ( DWORD dwGameRef )");

    DWORD dwReturn;
    DWORD dwFunction = FUNC_GetObject;

    _asm
    {
        mov     ecx, dword ptr ds:[CLASS_CPool_Object]
        push    dwGameRef
        call    dwFunction
        add     esp, 0x4
        mov     dwReturn, eax
    }

    CObjectSAInterface* pInterface = (CObjectSAInterface*)dwReturn;
    if ( pInterface )
    {
        // Lookup in the pool map for this GTA interface.
        objectPool_t::mapType::iterator iter = m_objectPool.map.find ( pInterface );
        if ( iter != m_objectPool.map.end () )
        {
            return (*iter).second;
        }
    }

    return NULL;
}

void CPoolsSA::DeleteAllObjects ( )
{
    DEBUG_TRACE("void CPoolsSA::DeleteAllObjects ( )");

    while ( m_objectPool.ulCount > 0 )
        RemoveObject ( m_objectPool.ulCount - 1 );
    m_objectPool.map.clear ();
}









//////////////////////////////////////////////////////////////////////////////////////////
//                                       PEDS POOL                                      //
//////////////////////////////////////////////////////////////////////////////////////////
inline bool CPoolsSA::AddPedToPool ( CPedSA* pPed )
{
    DEBUG_TRACE("inline bool CPoolsSA::AddPedToPool ( CPedSA* pPed )");

    // We always add to the end of the pool array because
    // it's ensured that there won't be empty spaces in the
    // middle of the array.
    unsigned long ulNewPos = m_pedPool.ulCount;

    // Grab the ped interface
    CPedSAInterface* pInterface = pPed->GetPedInterface ();

    if ( ! pInterface )
    {
        return false;
    }
    else
    {
        // Add it to the pool array
        m_pedPool.array [ ulNewPos ] = pPed;
        pPed->SetArrayID ( ulNewPos );

        // Add it to the pool map
        m_pedPool.map.insert ( pedPool_t::mapType::value_type ( pInterface, pPed ) );

        // Increase the count of peds
        ++m_pedPool.ulCount;
    }

    return true;
}

CPed* CPoolsSA::AddPed ( ePedModel ePedType )
{
    DEBUG_TRACE("CPed* CPoolsSA::AddPed ( ePedModel ePedType )");

    CPedSA* pPed = NULL;

    if ( m_pedPool.ulCount < MAX_PEDS )
    {
        pPed = new CPlayerPedSA ( ePedType );
        if ( ! AddPedToPool ( pPed ) )
        {
            delete pPed;
            pPed = NULL;
        }
    }

    return pPed;
}

CPed* CPoolsSA::AddPed ( DWORD* pGameInterface )
{
    DEBUG_TRACE("CPed* CPoolsSA::AddPed ( DWORD* pGameInterface )");

    CPedSA* pPed = NULL;

    if ( m_pedPool.ulCount < MAX_PEDS )
    {
        CPlayerPedSAInterface* pInterface = reinterpret_cast < CPlayerPedSAInterface* > ( pGameInterface );
        if ( pInterface )
        {
            // Make sure that it's not already in the peds pool
            pedPool_t::mapType::iterator iter = m_pedPool.map.find ( pInterface );
            if ( iter != m_pedPool.map.end () )
            {
                pPed = (*iter).second;
            }
            else
            {
                // Create it
                pPed = new CPlayerPedSA ( pInterface );
                if ( ! AddPedToPool ( pPed ) )
                {
                    delete pPed;
                    pPed = NULL;
                }
            }
        }
    }

    return pPed;
}

CPed* CPoolsSA::AddCivilianPed ( DWORD* pGameInterface )
{
    DEBUG_TRACE("CPed* CPoolsSA::AddCivilianPed ( DWORD* pGameInterface )");

    CPedSA* pPed = NULL;

    if ( m_pedPool.ulCount < MAX_PEDS )
    {
        CPedSAInterface* pInterface = reinterpret_cast < CPedSAInterface* > ( pGameInterface );
        if ( pInterface )
        {
            // Make sure that it's not already in the peds pool
            pedPool_t::mapType::iterator iter = m_pedPool.map.find ( pInterface );
            if ( iter != m_pedPool.map.end () )
            {
                pPed = (*iter).second;
            }
            else
            {
                // Create it
                pPed = new CCivilianPedSA ( pInterface );
                if ( ! AddPedToPool ( pPed ) )
                {
                    delete pPed;
                    pPed = NULL;
                }
            }
        }
    }

    return pPed;
}

void CPoolsSA::RemovePed ( unsigned long ulID, bool bDelete )
{
    DEBUG_TRACE("void CPoolsSA::RemovePed ( unsigned long ulID, bool bDelete )");

    static bool bIsDeletingPedAlready = false; // to prevent delete being called twice
    if ( !bIsDeletingPedAlready ) 
    {
        bIsDeletingPedAlready = true;

        CPedSA* pPedSA = m_pedPool.array [ ulID ];
        assert ( NULL != pPedSA );

        // Pop the element to remove from the pool array
        if ( ulID != m_pedPool.ulCount - 1 )
        {
            // We are removing an intermediate position of
            // the array. Move the last element to the just
            // deleted element position to not allow empty
            // spaces on it.
            m_pedPool.array [ ulID ] = m_pedPool.array [ m_pedPool.ulCount - 1 ];
            m_pedPool.array [ ulID ]->SetArrayID ( ulID );
        }
        m_pedPool.array [ m_pedPool.ulCount - 1 ] = NULL;

        // Unlink the element to remove from the pool map
        pedPool_t::mapType::iterator iter = m_pedPool.map.find ( pPedSA->GetPedInterface () );
        if ( iter != m_pedPool.map.end () )
        {
            m_pedPool.map.erase ( iter );
        }


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
            }
        }

        // Decrease the count of elements in the pool
        --m_pedPool.ulCount;

        bIsDeletingPedAlready = false;
    }
}

void CPoolsSA::RemovePed ( CPed* pPed, bool bDelete )
{
    DEBUG_TRACE("void CPoolsSA::RemovePed ( CPed* pPed, bool bDelete )");

    assert ( NULL != pPed );

    CPedSA* pPedSA = dynamic_cast < CPedSA* > ( pPed );
    if ( pPedSA )
    {
        RemovePed ( pPedSA->GetArrayID (), bDelete );
    }
}

CPed* CPoolsSA::GetPed ( unsigned long ulID )
{
    DEBUG_TRACE("CPed* CPoolsSA::GetPed ( unsigned long ulID )");

    assert ( ulID < MAX_PEDS );

    return m_pedPool.array [ ulID ];
}

CPed* CPoolsSA::GetPed ( DWORD* pGameInterface )
{
    DEBUG_TRACE("CPed* CPoolsSA::GetPed ( DWORD* pGameInterface )");

    CPedSAInterface* pInterface = reinterpret_cast < CPedSAInterface* > ( pGameInterface );

    // 0x00400000 is used for bad player pointers some places in GTA
    if ( pInterface && pGameInterface != (DWORD*)0x00400000 )
    {
        // Lookup in the pool map for the ped related to this interface.
        pedPool_t::mapType::iterator iter = m_pedPool.map.find ( pInterface );
        if ( iter != m_pedPool.map.end () )
        {
            return (*iter).second;
        }
    }

    return NULL;
}

DWORD CPoolsSA::GetPedRef ( CPed* pPed )
{
    DEBUG_TRACE("DWORD CPoolsSA::GetPedRef ( CPed* pPed )");

    DWORD dwRef = 0;
    CPedSA* pPedSA = dynamic_cast < CPedSA * > ( pPed );
    if ( pPedSA )
    {
        CPedSAInterface* pInterface = pPedSA->GetPedInterface ();
        DWORD dwFunc = FUNC_GetPedRef;
        _asm
        {
            push    pInterface
            call    dwFunc
            add     esp, 0x4
            mov     dwRef, eax
        }
    }

    return dwRef;
}

DWORD CPoolsSA::GetPedRef ( DWORD* pGameInterface )
{
    DEBUG_TRACE("DWORD CPoolsSA::GetPedRef ( DWORD* pGameInterface )");

    DWORD dwRef = 0;
    CPedSAInterface* pInterface = reinterpret_cast < CPedSAInterface * > ( pGameInterface );
    if ( pInterface )
    {
        DWORD dwFunc = FUNC_GetPedRef;
        _asm
        {
            push    pInterface
            call    dwFunc
            add     esp, 0x4
            mov     dwRef, eax
        }
    }

    return dwRef;
}

CPed* CPoolsSA::GetPedFromRef ( DWORD dwGameRef )
{
    DEBUG_TRACE("CPed* CPoolsSA::GetPedFromRef ( DWORD dwGameRef )");

    CPedSAInterface* pInterface = this->GetPedInterface ( dwGameRef );
    if ( pInterface )
    {
        // Lookup in the pool map for this GTA interface.
        pedPool_t::mapType::iterator iter = m_pedPool.map.find ( pInterface );
        if ( iter != m_pedPool.map.end () )
        {
            return (*iter).second;
        }
    }

    return NULL;
}

CPedSAInterface* CPoolsSA::GetPedInterface ( DWORD dwGameRef )
{
    DEBUG_TRACE("CPedSAInterface* CPoolsSA::GetPedInterface ( DWORD dwGameRef )");

    DWORD dwReturn;
    DWORD dwFunction = FUNC_GetPed;

    _asm
    {
        mov     ecx, dword ptr ds:[CLASS_CPool_Ped]
        push    dwGameRef
        call    dwFunction
        add     esp, 0x4
        mov     dwReturn, eax
    }

    CPedSAInterface* pInterface = (CPedSAInterface*)dwReturn;
    return pInterface;
}

void CPoolsSA::DeleteAllPeds ( )
{
    DEBUG_TRACE("void CPoolsSA::DeleteAllPeds ( )");

    while ( m_pedPool.ulCount > 0 )
        RemovePed ( m_pedPool.ulCount - 1 );
    m_pedPool.map.clear ();
}

CEntity * CPoolsSA::GetEntity ( DWORD* pGameInterface )
{
    if ( pGameInterface )
    {
        CEntitySAInterface * pEntityInterface = ( CEntitySAInterface * ) ( pGameInterface );
        switch ( pEntityInterface->nType )
        {
            case ENTITY_TYPE_PED: return ( CEntity * ) GetPed ( pGameInterface ); break;
            case ENTITY_TYPE_VEHICLE: return ( CEntity * ) GetVehicle ( pGameInterface ); break;
            case ENTITY_TYPE_OBJECT: return ( CEntity * ) GetObject ( pGameInterface ); break;
            default: break;
        }
    }
    return NULL;
}


CBuilding * CPoolsSA::AddBuilding ( DWORD dwModelID )
{
    DEBUG_TRACE("CBuilding * CPoolsSA::AddBuilding ( DWORD dwModelID )");
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
    return NULL;
}


CVehicle* CPoolsSA::AddTrain ( CVector * vecPosition, DWORD dwModels[], int iSize, bool bDirection )
{
    DEBUG_TRACE("CVehicle* CPoolsSA::AddTrain ( CVector * vecPosition, DWORD dwModels[], int iSize, bool bDirection )");

    // clean the existing array
    MemSetFast ( (void *)VAR_TrainModelArray, 0, 32 * sizeof(DWORD) );

    // now load the models we're going to use and add them to the array
    for ( int i = 0; i < iSize; i++ )
    {
        if ( dwModels[i] == 449 || dwModels[i] == 537 || 
            dwModels[i] == 538 || dwModels[i] == 569 || 
            dwModels[i] == 590 )
        {
            MemPutFast < DWORD > ( VAR_TrainModelArray + i * 4, dwModels[i] );
        }
    }

    CVehicleSAInterface * trainBegining;
    CVehicleSAInterface * trainEnd;

    float fX = vecPosition->fX;
    float fY = vecPosition->fY;
    float fZ = vecPosition->fZ;

    // Disable GetVehicle because CreateMissionTrain calls it before our CVehicleSA instance is inited
    m_bGetVehicleEnabled = false;

    DWORD dwFunc = FUNC_CTrain_CreateMissionTrain;
    _asm
    {
        push    0 // place as close to point as possible (rather than at node)? (maybe) (actually seems to have an effect on the speed, so changed from 1 to 0)
        push    0 // start finding closest from here 
        push    -1 // node to start at (-1 for closest node)
        lea     ecx, trainEnd
        push    ecx // end of train
        lea     ecx, trainBegining 
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
    m_bGetVehicleEnabled = true;

    CVehicleSA * trainHead = NULL;
    if ( trainBegining )
    {
        DWORD vehicleIndex = 0;

        if ( m_vehiclePool.ulCount < MAX_VEHICLES )
        {
            trainHead = new CVehicleSA ( trainBegining );
            if ( ! AddVehicleToPool ( trainHead ) )
            {
                delete trainHead;
                trainHead = NULL;
            }
            else
                ++vehicleIndex;
        }

        CVehicleSA * carriage = trainHead;
        
        while ( carriage )
        {
            if ( m_vehiclePool.ulCount < MAX_VEHICLES)
            {
                CVehicleSAInterface* vehCarriage = carriage->GetNextCarriageInTrain ();
                if ( vehCarriage )
                {
                    carriage = new CVehicleSA ( vehCarriage );
                    if ( ! AddVehicleToPool ( carriage ) )
                    {
                        delete carriage;
                        carriage = NULL;
                    }
                    else
                        ++vehicleIndex;
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
    DWORD dwFunc = NULL;
    DWORD dwThis = NULL;
    switch ( pool )
    {
        case BUILDING_POOL: dwFunc = FUNC_CBuildingPool_GetNoOfUsedSpaces; dwThis = CLASS_CBuildingPool; break;
        case PED_POOL: dwFunc = FUNC_CPedPool_GetNoOfUsedSpaces; dwThis = CLASS_CPedPool; break;
        case OBJECT_POOL: dwFunc = FUNC_CObjectPool_GetNoOfUsedSpaces; dwThis = CLASS_CObjectPool; break;
        case DUMMY_POOL: dwFunc = FUNC_CDummyPool_GetNoOfUsedSpaces; dwThis = CLASS_CDummyPool; break;
        case VEHICLE_POOL: dwFunc = FUNC_CVehiclePool_GetNoOfUsedSpaces; dwThis = CLASS_CVehiclePool; break;
        case COL_MODEL_POOL: dwFunc = FUNC_CColModelPool_GetNoOfUsedSpaces; dwThis = CLASS_CColModelPool; break;
        case TASK_POOL: dwFunc = FUNC_CTaskPool_GetNoOfUsedSpaces; dwThis = CLASS_CTaskPool; break;
        case EVENT_POOL: dwFunc = FUNC_CEventPool_GetNoOfUsedSpaces; dwThis = CLASS_CEventPool; break;
        case TASK_ALLOCATOR_POOL: dwFunc = FUNC_CTaskAllocatorPool_GetNoOfUsedSpaces; dwThis = CLASS_CTaskAllocatorPool; break;
        case PED_INTELLIGENCE_POOL: dwFunc = FUNC_CPedIntelligencePool_GetNoOfUsedSpaces; dwThis = CLASS_CPedIntelligencePool; break;
        case PED_ATTRACTOR_POOL: dwFunc = FUNC_CPedAttractorPool_GetNoOfUsedSpaces; dwThis = CLASS_CPedAttractorPool; break;
        case ENTRY_INFO_NODE_POOL: dwFunc = FUNC_CEntryInfoNodePool_GetNoOfUsedSpaces; dwThis = CLASS_CEntryInfoNodePool; break;
        case NODE_ROUTE_POOL: dwFunc = FUNC_CNodeRoutePool_GetNoOfUsedSpaces; dwThis = CLASS_CNodeRoutePool; break;
        case PATROL_ROUTE_POOL: dwFunc = FUNC_CPatrolRoutePool_GetNoOfUsedSpaces; dwThis = CLASS_CPatrolRoutePool; break;
        case POINT_ROUTE_POOL: dwFunc = FUNC_CPointRoutePool_GetNoOfUsedSpaces; dwThis = CLASS_CPointRoutePool; break;
        case POINTER_DOUBLE_LINK_POOL: dwFunc = FUNC_CPtrNodeDoubleLinkPool_GetNoOfUsedSpaces; dwThis = CLASS_CPtrNodeDoubleLinkPool; break;
        case POINTER_SINGLE_LINK_POOL: dwFunc = FUNC_CPtrNodeSingleLinkPool_GetNoOfUsedSpaces; dwThis = CLASS_CPtrNodeSingleLinkPool; break;
        default: return -1;
    }

    int iOut = -2;
    if ( *(DWORD *)dwThis != NULL )
    {
        _asm
        {
            mov     ecx, dwThis
            mov     ecx, [ecx]
            call    dwFunc
            mov     iOut, eax

        }
    }

    return iOut;
}

CEntryInfoNodePool * CPoolsSA::GetEntryInfoNodePool ( void )
{
    return EntryInfoNodePool;
}

int CEntryInfoNodePoolSA::GetNumberOfUsedSpaces ( void )
{
    DWORD dwFunc = FUNC_CEntryInfoNodePool_GetNoOfUsedSpaces;
    int iOut = 0;
    _asm
    {
        mov     ecx, CLASS_CEntryInfoNodePool
        mov     ecx, [ecx]
        call    dwFunc
        mov     iOut, eax
    }

    return iOut;
}

CPointerNodeDoubleLinkPool * CPoolsSA::GetPointerNodeDoubleLinkPool ( void )
{
    return PointerNodeDoubleLinkPool;
}

int CPointerNodeDoubleLinkPoolSA::GetNumberOfUsedSpaces ( void )
{
    DWORD dwFunc = FUNC_CPtrNodeDoubleLinkPool_GetNoOfUsedSpaces;
    int iOut = 0;
    _asm
    {
        mov     ecx, CLASS_CPtrNodeDoubleLinkPool
        mov     ecx, [ecx]
        call    dwFunc
        mov     iOut, eax
    }

    return iOut;
}

CPointerNodeSingleLinkPool * CPoolsSA::GetPointerNodeSingleLinkPool ( void )
{
    return PointerNodeSingleLinkPool;
}

int CPointerNodeSingleLinkPoolSA::GetNumberOfUsedSpaces ( void )
{
    DWORD dwFunc = FUNC_CPtrNodeSingleLinkPool_GetNoOfUsedSpaces;
    int iOut = 0;
    _asm
    {
        mov     ecx, CLASS_CPtrNodeSingleLinkPool
        mov     ecx, [ecx]
        call    dwFunc
        mov     iOut, eax
    }

    return iOut;
}
