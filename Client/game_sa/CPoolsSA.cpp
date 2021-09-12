/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPoolsSA.cpp
 *  PURPOSE:     Game entity pools
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

extern bool g_bVehiclePointerInvalid;

CPoolsSA::CPoolsSA()
{
    DEBUG_TRACE("CPoolsSA::CPoolsSA()");
    m_ppPedPoolInterface = (CPoolSAInterface<CPedSAInterface>**)0xB74490;
    m_ppObjectPoolInterface = (CPoolSAInterface<CObjectSAInterface>**)0xB7449C;
    m_ppVehiclePoolInterface = (CPoolSAInterface<CVehicleSAInterface>**)0xB74494;

    m_bGetVehicleEnabled = true;
    m_ulBuildingCount = 0;

    MemSetFast(&Buildings, 0, sizeof(CBuilding*) * MAX_BUILDINGS);

    EntryInfoNodePool = new CEntryInfoNodePoolSA();
    PointerNodeDoubleLinkPool = new CPointerNodeDoubleLinkPoolSA();
    PointerNodeSingleLinkPool = new CPointerNodeSingleLinkPoolSA();
}

CPoolsSA::~CPoolsSA()
{
    // Make sure we've deleted all vehicles, objects, peds and buildings
    DeleteAllVehicles();
    DeleteAllPeds();
    DeleteAllObjects();
    DeleteAllBuildings();

    if (EntryInfoNodePool)
        delete EntryInfoNodePool;

    if (PointerNodeDoubleLinkPool)
        delete PointerNodeDoubleLinkPool;

    if (PointerNodeSingleLinkPool)
        delete PointerNodeSingleLinkPool;
}

void CPoolsSA::DeleteAllBuildings()
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
inline bool CPoolsSA::AddVehicleToPool(CClientVehicle* pClientVehicle, CVehicleSA* pVehicle)
{
    DEBUG_TRACE("inline bool CPoolsSA::AddVehicleToPool ( CClientVehicle* pClientVehicle, CVehicleSA* pVehicle )");

    // Grab the interface
    CVehicleSAInterface* pInterface = pVehicle->GetVehicleInterface();

    if (!pInterface)
    {
        return false;
    }
    else
    {
        DWORD dwElementIndexInPool = GetVehiclePoolIndex((std::uint8_t*)pInterface);
        if (dwElementIndexInPool >= MAX_VEHICLES)
        {
            return false;
        }

        m_vehiclePool.arrayOfClientEntities[dwElementIndexInPool] = {pVehicle, (CClientEntity*)pClientVehicle};
        ++m_vehiclePool.ulCount;
    }

    return true;
}

CVehicle* CPoolsSA::AddVehicle(CClientVehicle* pClientVehicle, eVehicleTypes eVehicleType, unsigned char ucVariation, unsigned char ucVariation2)
{
    DEBUG_TRACE("CVehicle* CPoolsSA::AddVehicle ( CClientVehicle* pClientVehicle, eVehicleTypes eVehicleType )");
    CVehicleSA* pVehicle = NULL;

    if (m_vehiclePool.ulCount < MAX_VEHICLES)
    {
        auto vehicleClass = static_cast<VehicleClass>(pGame->GetModelInfo(eVehicleType)->GetVehicleType());

        switch (vehicleClass)
        {
            case VehicleClass::BOAT:
                pVehicle = new CBoatSA(eVehicleType, ucVariation, ucVariation2);
                break;
            case VehicleClass::BMX:
            case VehicleClass::BIKE:
                pVehicle = new CBikeSA(eVehicleType, ucVariation, ucVariation2);
                break;
            default:
                pVehicle = new CVehicleSA(eVehicleType, ucVariation, ucVariation2);
                break;
        }

        if (!AddVehicleToPool(pClientVehicle, pVehicle))
        {
            delete pVehicle;
            pVehicle = NULL;
        }
    }

    return pVehicle;
}

CVehicle* CPoolsSA::AddVehicle(CClientVehicle* pClientVehicle, DWORD* pGameInterface)
{
    DEBUG_TRACE("CVehicle* CPoolsSA::AddVehicle ( CClientVehicle* pClientVehicle, DWORD* pGameInterface )");
    CVehicleSA* pVehicle = NULL;

    if (m_vehiclePool.ulCount < MAX_VEHICLES)
    {
        CVehicleSAInterface* pInterface = reinterpret_cast<CVehicleSAInterface*>(pGameInterface);

        if (pInterface)
        {
            DWORD dwElementIndexInPool = GetVehiclePoolIndex((std::uint8_t*)pInterface);
            if (dwElementIndexInPool >= MAX_VEHICLES)
            {
                return nullptr;
            }
            pVehicle = m_vehiclePool.arrayOfClientEntities[dwElementIndexInPool].pEntity;

            if (pVehicle)
            {
                return pVehicle;
            }
            else
            {
                switch ((VehicleClass)pInterface->m_vehicleClass)
                {
                    case VehicleClass::BOAT:
                        pVehicle = new CBoatSA(reinterpret_cast<CBoatSAInterface*>(pInterface));
                        break;
                    case VehicleClass::BMX:
                    case VehicleClass::BIKE:
                        pVehicle = new CBikeSA(reinterpret_cast<CBikeSAInterface*>(pInterface));
                        break;
                    default:
                        pVehicle = new CVehicleSA(pInterface);
                        break;
                }

                if (!AddVehicleToPool(pClientVehicle, pVehicle))
                {
                    delete pVehicle;
                    pVehicle = NULL;
                }
            }
        }
    }

    return pVehicle;
}

void CPoolsSA::RemoveVehicle(CVehicle* pVehicle, bool bDelete)
{
    DEBUG_TRACE("void CPoolsSA::RemoveVehicle ( CVehicle * pVehicle, bool bDelete )");

    static bool bIsDeletingVehicleAlready = false;

    if (!bIsDeletingVehicleAlready)
    {
        assert(NULL != pVehicle);

        bIsDeletingVehicleAlready = true;

        CVehicleSAInterface* pInterface = pVehicle->GetVehicleInterface();

        DWORD dwElementIndexInPool = GetVehiclePoolIndex((std::uint8_t*)pInterface);
        if (dwElementIndexInPool >= MAX_VEHICLES)
        {
            return;
        }

        CVehicleSA* pVehicleSA = m_vehiclePool.arrayOfClientEntities[dwElementIndexInPool].pEntity;
        m_vehiclePool.arrayOfClientEntities[dwElementIndexInPool] = {nullptr, nullptr};

        // Delete it from memory
        delete pVehicleSA;

        // Decrease the count of elements in the pool
        --m_vehiclePool.ulCount;

        bIsDeletingVehicleAlready = false;
    }
}

SClientEntity<CVehicleSA>* CPoolsSA::GetVehicle(DWORD* pGameInterface)
{
    DEBUG_TRACE("SClientEntity<CVehicleSA>* CPoolsSA::GetVehicle ( DWORD* pGameInterface )");

    if (m_bGetVehicleEnabled)
    {
        CVehicleSAInterface* pInterface = reinterpret_cast<CVehicleSAInterface*>(pGameInterface);

        if (pInterface)
        {
            DWORD dwElementIndexInPool = GetVehiclePoolIndex((std::uint8_t*)pInterface);

            if (dwElementIndexInPool < MAX_VEHICLES)
            {
                return &m_vehiclePool.arrayOfClientEntities[dwElementIndexInPool];
            }
        }
    }
    return nullptr;
}

DWORD CPoolsSA::GetVehicleRef(CVehicle* pVehicle)
{
    DEBUG_TRACE("DWORD CPoolsSA::GetVehicleRef ( CVehicle* pVehicle )");

    DWORD       dwRef = 0;
    CVehicleSA* pVehicleSA = dynamic_cast<CVehicleSA*>(pVehicle);
    if (pVehicleSA)
    {
        CVehicleSAInterface* pInterface = pVehicleSA->GetVehicleInterface();
        DWORD                dwFunc = FUNC_GetVehicleRef;
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

DWORD CPoolsSA::GetVehicleRef(DWORD* pGameInterface)
{
    DEBUG_TRACE("DWORD CPoolsSA::GetVehicleRef ( DWORD* pGameInterface )");

    DWORD                dwRef = 0;
    CVehicleSAInterface* pInterface = reinterpret_cast<CVehicleSAInterface*>(pGameInterface);
    if (pInterface)
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

CVehicle* CPoolsSA::GetVehicleFromRef(DWORD dwGameRef)
{
    DEBUG_TRACE("CVehicle* CPoolsSA::GetVehicleFromRef ( DWORD dwGameRef )");

    DWORD dwReturn;
    DWORD dwFunction = FUNC_GetVehicle;

    _asm {
        mov     ecx, dword ptr ds : [CLASS_CPool_Vehicle]
        push    dwGameRef
        call    dwFunction
        add     esp, 0x4
        mov     dwReturn, eax
    }

    CVehicleSAInterface* pInterface = (CVehicleSAInterface*)dwReturn;
    if (pInterface)
    {
        DWORD       dwElementIndexInPool = dwGameRef >> 8;
        CVehicleSA* pVehicle = m_vehiclePool.arrayOfClientEntities[dwElementIndexInPool].pEntity;

        if (pVehicle)
        {
            return pVehicle;
        }
    }

    return NULL;
}

void CPoolsSA::DeleteAllVehicles()
{
    DEBUG_TRACE("void CPoolsSA::DeleteAllVehicles ( )");

    while (m_vehiclePool.ulCount > 0)
    {
        CVehicleSA* pVehicle = m_vehiclePool.arrayOfClientEntities[m_vehiclePool.ulCount - 1].pEntity;

        RemoveVehicle(pVehicle);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                     OBJECTS POOL                                     //
//////////////////////////////////////////////////////////////////////////////////////////
inline bool CPoolsSA::AddObjectToPool(CClientObject* pClientObject, CObjectSA* pObject)
{
    DEBUG_TRACE("inline bool CPoolsSA::AddObjectToPool ( CClientObject* pClientObject, CObjectSA* pObject )");

    // Grab the new object interface
    CObjectSAInterface* pInterface = pObject->GetObjectInterface();

    if (!pInterface)
    {
        return false;
    }
    else
    {
        DWORD dwElementIndexInPool = GetObjectPoolIndex((std::uint8_t*)pInterface);
        if (dwElementIndexInPool >= MAX_OBJECTS)
        {
            return false;
        }

        m_objectPool.arrayOfClientEntities[dwElementIndexInPool] = {pObject, (CClientEntity*)pClientObject};

        // Increase the count of objects
        ++m_objectPool.ulCount;
    }

    return true;
}

CObject* CPoolsSA::AddObject(CClientObject* pClientObject, DWORD dwModelID, bool bLowLod, bool bBreakingDisabled)
{
    DEBUG_TRACE("CObject * CPoolsSA::AddObject ( CClientObject* pClientObject, DWORD dwModelID, bool bLowLod, bool bBreakingDisabled )");

    CObjectSA* pObject = NULL;

    if (m_objectPool.ulCount < MAX_OBJECTS)
    {
        pObject = new CObjectSA(dwModelID, bBreakingDisabled);

        if (pObject && AddObjectToPool(pClientObject, pObject))
        {
            if (bLowLod)
            {
                pObject->m_pInterface->bUsesCollision = 0;
                pObject->m_pInterface->bDontCastShadowsOn = 1;
                // Set super hacky flag to indicate this is a special low lod object
                pObject->m_pInterface->SetIsLowLodEntity();
            }
            else
                pObject->m_pInterface->SetIsHighLodEntity();
        }
        else
        {
            delete pObject;
            pObject = NULL;
        }
    }

    return pObject;
}

void CPoolsSA::RemoveObject(CObject* pObject, bool bDelete)
{
    DEBUG_TRACE("void CPoolsSA::RemoveObject ( CObject* pObject, bool bDelete )");

    static bool bIsDeletingObjectAlready = false;

    if (!bIsDeletingObjectAlready)
    {
        assert(NULL != pObject);

        bIsDeletingObjectAlready = true;

        CObjectSAInterface* pInterface = pObject->GetObjectInterface();

        DWORD dwElementIndexInPool = GetObjectPoolIndex((std::uint8_t*)pInterface);
        if (dwElementIndexInPool >= MAX_OBJECTS)
        {
            return;
        }

        CObjectSA* pObjectSA = m_objectPool.arrayOfClientEntities[dwElementIndexInPool].pEntity;
        m_objectPool.arrayOfClientEntities[dwElementIndexInPool] = {nullptr, nullptr};

        // Delete it from memory
        delete pObjectSA;

        // Decrease the count of elements in the pool
        --m_objectPool.ulCount;

        bIsDeletingObjectAlready = false;
    }
}

SClientEntity<CObjectSA>* CPoolsSA::GetObject(DWORD* pGameInterface)
{
    DEBUG_TRACE("CObject* CPoolsSA::GetObject( DWORD* pGameInterface )");

    CObjectSAInterface* pInterface = reinterpret_cast<CObjectSAInterface*>(pGameInterface);

    if (pInterface)
    {
        DWORD dwElementIndexInPool = GetObjectPoolIndex((std::uint8_t*)pInterface);

        if (dwElementIndexInPool < MAX_OBJECTS)
        {
            return &m_objectPool.arrayOfClientEntities[dwElementIndexInPool];
        }
    }
    return nullptr;
}

DWORD CPoolsSA::GetObjectRef(CObject* pObject)
{
    DEBUG_TRACE("DWORD CPoolsSA::GetObjectRef ( CObject* pObject )");

    DWORD      dwRef = 0;
    CObjectSA* pObjectSA = dynamic_cast<CObjectSA*>(pObject);
    if (pObjectSA)
    {
        CObjectSAInterface* pInterface = pObjectSA->GetObjectInterface();
        DWORD               dwFunc = FUNC_GetObjectRef;
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

DWORD CPoolsSA::GetObjectRef(DWORD* pGameInterface)
{
    DEBUG_TRACE("DWORD CPoolsSA::GetObjectRef ( DWORD* pGameInterface )");

    DWORD               dwRef = 0;
    CObjectSAInterface* pInterface = reinterpret_cast<CObjectSAInterface*>(pGameInterface);
    if (pInterface)
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

CObject* CPoolsSA::GetObjectFromRef(DWORD dwGameRef)
{
    DEBUG_TRACE("CObject* CPoolsSA::GetObjectFromRef ( DWORD dwGameRef )");

    DWORD dwReturn;
    DWORD dwFunction = FUNC_GetObject;

    _asm {
        mov     ecx, dword ptr ds : [CLASS_CPool_Object]
        push    dwGameRef
        call    dwFunction
        add     esp, 0x4
        mov     dwReturn, eax
    }

    CObjectSAInterface* pInterface = (CObjectSAInterface*)dwReturn;

    if (pInterface)
    {
        DWORD      dwElementIndexInPool = dwGameRef >> 8;
        CObjectSA* pObject = m_objectPool.arrayOfClientEntities[dwElementIndexInPool].pEntity;

        if (pObject)
        {
            return pObject;
        }
    }

    return NULL;
}

CObject* CPoolsSA::GetObjectFromIndex(std::uint32_t elementIndexInPool)
{
    if (elementIndexInPool >= MAX_OBJECTS)
        return nullptr;
    auto objectPool = *m_ppObjectPoolInterface;
    if (objectPool->IsEmpty(elementIndexInPool))
        return nullptr;
    CObjectSAInterface* pInterface = objectPool->GetObject(elementIndexInPool);
    if (pInterface)
    {
        CObjectSA* pObject = m_objectPool.arrayOfClientEntities[elementIndexInPool].pEntity;
        if (pObject)
            return pObject;
    }
    return nullptr;
}

void CPoolsSA::DeleteAllObjects()
{
    DEBUG_TRACE("void CPoolsSA::DeleteAllObjects ( )");

    while (m_objectPool.ulCount > 0)
    {
        CObjectSA* pObject = m_objectPool.arrayOfClientEntities[m_objectPool.ulCount - 1].pEntity;

        RemoveObject(pObject);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                       PEDS POOL                                      //
//////////////////////////////////////////////////////////////////////////////////////////
inline bool CPoolsSA::AddPedToPool(CClientPed* pClientPed, CPedSA* pPed)
{
    DEBUG_TRACE("inline bool CPoolsSA::AddPedToPool ( CClientPed* pClientPed, CPedSA* pPed )");

    // Grab the ped interface
    CPedSAInterface* pInterface = pPed->GetPedInterface();

    if (!pInterface)
    {
        return false;
    }
    else
    {
        DWORD dwElementIndexInPool = GetPedPoolIndex((std::uint8_t*)pInterface);
        if (dwElementIndexInPool >= MAX_PEDS)
        {
            return false;
        }

        m_pedPool.arrayOfClientEntities[dwElementIndexInPool] = {pPed, (CClientEntity*)pClientPed};
        ++m_pedPool.ulCount;
    }

    return true;
}

CPed* CPoolsSA::AddPed(CClientPed* pClientPed, ePedModel ePedType)
{
    DEBUG_TRACE("CPed* CPoolsSA::AddPed ( CClientPed* pClientPed, ePedModel ePedType )");

    CPedSA* pPed = NULL;
    if (m_pedPool.ulCount < MAX_PEDS)
    {
        pPed = new CPlayerPedSA(ePedType);
        if (!AddPedToPool(pClientPed, pPed))
        {
            delete pPed;
            pPed = NULL;
        }
    }
    return pPed;
}

CPed* CPoolsSA::AddPed(CClientPed* pClientPed, DWORD* pGameInterface)
{
    DEBUG_TRACE("CPed* CPoolsSA::AddPed ( CClientPed* pClientPed, DWORD* pGameInterface )");

    CPedSA* pPed = NULL;

    if (m_pedPool.ulCount < MAX_PEDS)
    {
        CPlayerPedSAInterface* pInterface = reinterpret_cast<CPlayerPedSAInterface*>(pGameInterface);
        if (pInterface)
        {
            // Extract the element index from the handle
            DWORD dwElementIndexInPool = GetPedPoolIndex((std::uint8_t*)pInterface);

            if (dwElementIndexInPool < MAX_PEDS)
            {
                SClientEntity<CPedSA>* pPedEntity = &m_pedPool.arrayOfClientEntities[dwElementIndexInPool];
                if (pPedEntity && pPedEntity->pEntity)
                {
                    m_pedPool.arrayOfClientEntities[dwElementIndexInPool] = {pPedEntity->pEntity, (CClientEntity*)pClientPed};
                }
                else
                {
                    // Create it
                    pPed = new CPlayerPedSA(pInterface);
                    if (!AddPedToPool(pClientPed, pPed))
                    {
                        delete pPed;
                        pPed = NULL;
                    }
                }
            }
        }
    }

    return pPed;
}

CPed* CPoolsSA::AddCivilianPed(DWORD* pGameInterface)
{
    DEBUG_TRACE("CPed* CPoolsSA::AddCivilianPed ( DWORD* pGameInterface )");

    CPedSA* pPed = NULL;

    if (m_pedPool.ulCount < MAX_PEDS)
    {
        CPedSAInterface* pInterface = reinterpret_cast<CPedSAInterface*>(pGameInterface);
        if (pInterface)
        {
            // Extract the element index from the handle
            DWORD dwElementIndexInPool = GetPedPoolIndex((std::uint8_t*)pInterface);
            if (dwElementIndexInPool >= MAX_PEDS)
            {
                return nullptr;
            }
            CPedSA* pPed = m_pedPool.arrayOfClientEntities[dwElementIndexInPool].pEntity;
            if (pPed)
            {
                return pPed;
            }
            else
            {
                // create new  ped here
            }
        }
    }

    return pPed;
}

void CPoolsSA::RemovePed(CPed* pPed, bool bDelete)
{
    DEBUG_TRACE("void CPoolsSA::RemovePed( CPed* pPed, bool bDelete )");

    static bool bIsDeletingPedAlready = false;            // to prevent delete being called twice

    if (!bIsDeletingPedAlready)
    {
        assert(nullptr != pPed);

        bIsDeletingPedAlready = true;

        CPedSAInterface* pInterface = pPed->GetPedInterface();

        DWORD dwElementIndexInPool = GetPedPoolIndex((std::uint8_t*)pInterface);
        if (dwElementIndexInPool >= MAX_PEDS)
        {
            return;
        }

        CPedSA* pPed = m_pedPool.arrayOfClientEntities[dwElementIndexInPool].pEntity;
        m_pedPool.arrayOfClientEntities[dwElementIndexInPool] = {nullptr, nullptr};

        // Delete the element from memory
        switch (pPed->GetType())
        {
            case PLAYER_PED:
            {
                CPlayerPedSA* pPlayerPed = dynamic_cast<CPlayerPedSA*>(pPed);
                if (pPlayerPed)
                {
                    if (!bDelete)
                        pPlayerPed->SetDoNotRemoveFromGameWhenDeleted(true);
                }

                delete pPlayerPed;

                break;
            }

            default:
            {
                CCivilianPedSA* pCivPed = dynamic_cast<CCivilianPedSA*>(pPed);
                if (pCivPed)
                {
                    if (!bDelete)
                        pCivPed->SetDoNotRemoveFromGameWhenDeleted(true);
                }

                delete pCivPed;
            }
        }

        // Decrease the count of elements in the pool
        --m_pedPool.ulCount;

        bIsDeletingPedAlready = false;
    }
}

SClientEntity<CPedSA>* CPoolsSA::GetPed(DWORD* pGameInterface)
{
    DEBUG_TRACE("SClientEntity<T>* CPoolsSA::GetPed ( DWORD* pGameInterface )");

    CPedSAInterface* pInterface = reinterpret_cast<CPedSAInterface*>(pGameInterface);

    // 0x00400000 is used for bad player pointers some places in GTA
    if (pInterface && pGameInterface != (DWORD*)0x00400000)
    {
        // Extract the element index from the handle
        DWORD dwElementIndexInPool = GetPedPoolIndex((std::uint8_t*)pInterface);

        if (dwElementIndexInPool < MAX_PEDS)
        {
            return &m_pedPool.arrayOfClientEntities[dwElementIndexInPool];
        }
    }
    return nullptr;
}

DWORD CPoolsSA::GetPedRef(CPed* pPed)
{
    DEBUG_TRACE("DWORD CPoolsSA::GetPedRef ( CPed* pPed )");

    DWORD   dwRef = 0;
    CPedSA* pPedSA = dynamic_cast<CPedSA*>(pPed);
    if (pPedSA)
    {
        CPedSAInterface* pInterface = pPedSA->GetPedInterface();
        DWORD            dwFunc = FUNC_GetPedRef;
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

DWORD CPoolsSA::GetPedRef(DWORD* pGameInterface)
{
    DEBUG_TRACE("DWORD CPoolsSA::GetPedRef ( DWORD* pGameInterface )");

    DWORD            dwRef = 0;
    CPedSAInterface* pInterface = reinterpret_cast<CPedSAInterface*>(pGameInterface);
    if (pInterface)
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

CPed* CPoolsSA::GetPedFromRef(DWORD dwGameRef)
{
    DEBUG_TRACE("CPed* CPoolsSA::GetPedFromRef ( DWORD dwGameRef )");

    CPedSAInterface* pInterface = this->GetPedInterface(dwGameRef);
    if (pInterface)
    {
        // Extract the element index from the handle
        DWORD   dwElementIndexInPool = dwGameRef >> 8;
        CPedSA* pPed = m_pedPool.arrayOfClientEntities[dwElementIndexInPool].pEntity;
        if (pPed)
        {
            return pPed;
        }
    }
    return NULL;
}

CPedSAInterface* CPoolsSA::GetPedInterface(DWORD dwGameRef)
{
    DEBUG_TRACE("CPedSAInterface* CPoolsSA::GetPedInterface ( DWORD dwGameRef )");

    DWORD dwReturn;
    DWORD dwFunction = FUNC_GetPed;

    _asm {
        mov     ecx, dword ptr ds : [CLASS_CPool_Ped]
        push    dwGameRef
        call    dwFunction
        add     esp, 0x4
        mov     dwReturn, eax
    }

    CPedSAInterface* pInterface = (CPedSAInterface*)dwReturn;
    return pInterface;
}

void CPoolsSA::DeleteAllPeds()
{
    DEBUG_TRACE("void CPoolsSA::DeleteAllPeds ( )");

    while (m_pedPool.ulCount > 0)
    {
        CPedSA* pPed = m_pedPool.arrayOfClientEntities[m_pedPool.ulCount - 1].pEntity;
        RemovePed(pPed);
    }
}

CEntity* CPoolsSA::GetEntity(DWORD* pGameInterface)
{
    if (pGameInterface)
    {
        auto pTheObjectEntity = GetObject(pGameInterface);
        if (pTheObjectEntity)
        {
            return pTheObjectEntity->pEntity;
        }

        auto pTheVehicleEntity = GetVehicle(pGameInterface);
        if (pTheVehicleEntity)
        {
            return pTheVehicleEntity->pEntity;
        }

        auto pThePedEntity = GetPed(pGameInterface);
        if (pThePedEntity)
        {
            return pThePedEntity->pEntity;
        }
    }
    return NULL;
}

CClientEntity* CPoolsSA::GetClientEntity(DWORD* pGameInterface)
{
    if (pGameInterface)
    {
        auto pTheObjectEntity = GetObject(pGameInterface);
        if (pTheObjectEntity)
        {
            return pTheObjectEntity->pClientEntity;
        }

        auto pTheVehicleEntity = GetVehicle(pGameInterface);
        if (pTheVehicleEntity)
        {
            return pTheVehicleEntity->pClientEntity;
        }

        auto pThePedEntity = GetPed(pGameInterface);
        if (pThePedEntity)
        {
            return pThePedEntity->pClientEntity;
        }
    }
    return NULL;
}

CBuilding* CPoolsSA::AddBuilding(DWORD dwModelID)
{
    DEBUG_TRACE("CBuilding * CPoolsSA::AddBuilding ( DWORD dwModelID )");

    if (m_ulBuildingCount <= MAX_BUILDINGS)
    {
        for (int i = 0; i < MAX_BUILDINGS; i++)
        {
            if (Buildings[i] == 0)
            {
                CBuildingSA* pBuilding = new CBuildingSA(dwModelID);
                Buildings[i] = pBuilding;
                pBuilding->SetArrayID(i);
                m_ulBuildingCount++;

                return pBuilding;
            }
        }
    }
    return NULL;
}

CVehicle* CPoolsSA::AddTrain(CClientVehicle* pClientVehicle, CVector* vecPosition, DWORD dwModels[], int iSize, bool bDirection, uchar ucTrackId)
{
    DEBUG_TRACE("CVehicle* CPoolsSA::AddTrain ( CClientVehicle* pClientVehicle, CVector * vecPosition, DWORD dwModels[], int iSize, bool bDirection )");

    // clean the existing array
    MemSetFast((void*)VAR_TrainModelArray, 0, 32 * sizeof(DWORD));

    // now load the models we're going to use and add them to the array
    for (int i = 0; i < iSize; i++)
    {
        if (dwModels[i] == 449 || dwModels[i] == 537 || dwModels[i] == 538 || dwModels[i] == 569 || dwModels[i] == 590 || dwModels[i] == 570)
        {
            MemPutFast<DWORD>(VAR_TrainModelArray + i * 4, dwModels[i]);
        }
    }

    CVehicleSAInterface* pTrainBeginning = NULL;
    CVehicleSAInterface* pTrainEnd = NULL;

    float fX = vecPosition->fX;
    float fY = vecPosition->fY;
    float fZ = vecPosition->fZ;

    // Disable GetVehicle because CreateMissionTrain calls it before our CVehicleSA instance is inited
    m_bGetVehicleEnabled = false;

    // Find closest track node
    float fRailDistance;
    int   iNodeId = pGame->GetWorld()->FindClosestRailTrackNode(*vecPosition, ucTrackId, fRailDistance);
    int   iDesiredTrackId = ucTrackId;

    DWORD dwFunc = FUNC_CTrain_CreateMissionTrain;
    _asm
    {
        push    0            // place as close to point as possible (rather than at node)? (maybe) (actually seems to have an effect on the speed, so changed from
                             // 1 to 0)
                             push    iDesiredTrackId            // track ID
                             push    iNodeId            // node to start at (-1 for closest node)
                             lea     ecx, pTrainEnd
                             push    ecx            // end of train
                             lea     ecx, pTrainBeginning
                             push    ecx            // begining of train
                             push    0            // train type (always use 0 as thats where we're writing to)
                             push    bDirection            // direction
                             push    fZ            // z
                             push    fY            // y
                             push    fX            // x
                             call    dwFunc
                             add     esp, 0x28
    }

    // Enable GetVehicle
    m_bGetVehicleEnabled = true;

    CVehicleSA* trainHead = NULL;
    if (pTrainBeginning)
    {
        DWORD vehicleIndex = 0;

        if (m_vehiclePool.ulCount < MAX_VEHICLES)
        {
            trainHead = new CVehicleSA(pTrainBeginning);
            if (!AddVehicleToPool(pClientVehicle, trainHead))
            {
                delete trainHead;
                trainHead = NULL;
            }
            else
                ++vehicleIndex;
        }

        CVehicleSA* carriage = trainHead;

        while (carriage)
        {
            if (m_vehiclePool.ulCount < MAX_VEHICLES)
            {
                CVehicleSAInterface* vehCarriage = carriage->GetNextCarriageInTrain();
                if (vehCarriage)
                {
                    carriage = new CVehicleSA(vehCarriage);
                    if (!AddVehicleToPool(pClientVehicle, carriage))
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
    trainHead->SetMoveSpeed(&vec);

    return trainHead;
}

DWORD CPoolsSA::GetPedPoolIndex(std::uint8_t* pInterface)
{
    DWORD         dwAlignedSize = 1988;
    std::uint8_t* pTheObjects = (std::uint8_t*)(*m_ppPedPoolInterface)->m_pObjects;
    DWORD         dwMaxIndex = MAX_PEDS - 1;
    if (pInterface < pTheObjects || pInterface > pTheObjects + (dwMaxIndex * dwAlignedSize))
    {
        return MAX_PEDS;
    }
    return ((pInterface - pTheObjects) / dwAlignedSize);
}

DWORD CPoolsSA::GetVehiclePoolIndex(std::uint8_t* pInterface)
{
    DWORD         dwAlignedSize = 2584;
    std::uint8_t* pTheObjects = (std::uint8_t*)(*m_ppVehiclePoolInterface)->m_pObjects;
    DWORD         dwMaxIndex = MAX_VEHICLES - 1;
    if (pInterface < pTheObjects || pInterface > pTheObjects + (dwMaxIndex * dwAlignedSize))
    {
        return MAX_VEHICLES;
    }
    return ((pInterface - pTheObjects) / dwAlignedSize);
}

DWORD CPoolsSA::GetObjectPoolIndex(std::uint8_t* pInterface)
{
    DWORD         dwAlignedSize = 412;
    std::uint8_t* pTheObjects = (std::uint8_t*)(*m_ppObjectPoolInterface)->m_pObjects;
    DWORD         dwMaxIndex = MAX_OBJECTS - 1;
    if (pInterface < pTheObjects || pInterface > pTheObjects + (dwMaxIndex * dwAlignedSize))
    {
        return MAX_OBJECTS;
    }
    return ((pInterface - pTheObjects) / dwAlignedSize);
}

uint CPoolsSA::GetModelIdFromClump(RpClump* pRpClump)
{
    // Search our pools for a match
    for (uint i = 0; i < m_pedPool.ulCount; i++)
    {
        CEntitySA* pEntitySA = m_pedPool.arrayOfClientEntities[i].pEntity;
        if (pEntitySA)
        {
            if (pEntitySA->GetRpClump() == pRpClump)
            {
                return pEntitySA->GetModelIndex();
            }
        }
    }

    for (uint i = 0; i < m_vehiclePool.ulCount; i++)
    {
        CEntitySA* pEntitySA = m_vehiclePool.arrayOfClientEntities[i].pEntity;
        if (pEntitySA)
        {
            if (pEntitySA->GetRpClump() == pRpClump)
            {
                return pEntitySA->GetModelIndex();
            }
        }
    }

    for (uint i = 0; i < m_objectPool.ulCount; i++)
    {
        CEntitySA* pEntitySA = m_objectPool.arrayOfClientEntities[i].pEntity;
        if (pEntitySA)
        {
            if (pEntitySA->GetRpClump() == pRpClump)
            {
                return pEntitySA->GetModelIndex();
            }
        }
    }

    // Finally search model info array
    CBaseModelInfoSAInterface** ppModelInfo = (CBaseModelInfoSAInterface**)ARRAY_ModelInfo;

    unsigned int NUMBER_OF_MODELS = pGame->GetBaseIDforTXD();

    for (uint i = 1; i < NUMBER_OF_MODELS; i++)
    {
        CBaseModelInfoSAInterface* m_pInterface = ppModelInfo[i];
        if (m_pInterface && m_pInterface->pRwObject == (RwObject*)pRpClump)
        {
            return i;
        }
    }
    return 0;
}

int CPoolsSA::GetPoolDefaultCapacity(ePools pool)
{
    switch (pool)
    {
        case BUILDING_POOL:
            return 13000;
        case PED_POOL:
            return 140;
        case OBJECT_POOL:
            return 350;            // Modded to 700   @ CGameSA.cpp
        case DUMMY_POOL:
            return 2500;
        case VEHICLE_POOL:
            return 110;
        case COL_MODEL_POOL:
            return 10150;            // Modded to 12000  @ CGameSA.cpp
        case TASK_POOL:
            return 500;            // Modded to 5000   @ CGameSA.cpp
        case EVENT_POOL:
            return 200;            // Modded to 5000   @ CGameSA.cpp
        case TASK_ALLOCATOR_POOL:
            return 16;
        case PED_INTELLIGENCE_POOL:
            return 140;
        case PED_ATTRACTOR_POOL:
            return 64;
        case ENTRY_INFO_NODE_POOL:
            return 500;            // Modded to 4096   @ CGameSA.cpp
        case NODE_ROUTE_POOL:
            return 64;
        case PATROL_ROUTE_POOL:
            return 32;
        case POINT_ROUTE_POOL:
            return 64;
        case POINTER_DOUBLE_LINK_POOL:
            return 3200;            // Modded to 8000   @ CGameSA.cpp
        case POINTER_SINGLE_LINK_POOL:
            return 70000;
        case ENV_MAP_MATERIAL_POOL:
            return 4096;            // Modded to 16000   @ CGameSA.cpp
        case ENV_MAP_ATOMIC_POOL:
            return 1024;            // Modded to 8000    @ CGameSA.cpp
        case SPEC_MAP_MATERIAL_POOL:
            return 4096;            // Modded to 16000   @ CGameSA.cpp
    }
    return 0;
}

int CPoolsSA::GetPoolCapacity(ePools pool)
{
    DWORD iPtr = NULL;
    DWORD cPtr = NULL;
    switch (pool)
    {
        case BUILDING_POOL:
            iPtr = 0x55105F;
            break;
        case PED_POOL:
            iPtr = 0x550FF2;
            break;
        case OBJECT_POOL:
            iPtr = 0x551097;
            break;
        case DUMMY_POOL:
            iPtr = 0x5510CF;
            break;
        case VEHICLE_POOL:
            cPtr = 0x55102A;
            break;
        case COL_MODEL_POOL:
            iPtr = 0x551107;
            break;
        case TASK_POOL:
            iPtr = 0x55113F;
            break;
        case EVENT_POOL:
            iPtr = 0x551177;
            break;
        case TASK_ALLOCATOR_POOL:
            cPtr = 0x55124E;
            break;
        case PED_INTELLIGENCE_POOL:
            iPtr = 0x551283;
            break;
        case PED_ATTRACTOR_POOL:
            cPtr = 0x5512BC;
            break;
        case ENTRY_INFO_NODE_POOL:
            iPtr = 0x550FBA;
            break;
        case NODE_ROUTE_POOL:
            cPtr = 0x551219;
            break;
        case PATROL_ROUTE_POOL:
            cPtr = 0x5511E4;
            break;
        case POINT_ROUTE_POOL:
            cPtr = 0x5511AF;
            break;
        case POINTER_DOUBLE_LINK_POOL:
            iPtr = 0x550F82;
            break;
        case POINTER_SINGLE_LINK_POOL:
            iPtr = 0x550F46;
            break;
        case ENV_MAP_MATERIAL_POOL:
            iPtr = 0x5DA08E;
            break;
        case ENV_MAP_ATOMIC_POOL:
            iPtr = 0x5DA0CA;
            break;
        case SPEC_MAP_MATERIAL_POOL:
            iPtr = 0x5DA106;
            break;
    }
    if (iPtr)
        return *(int*)iPtr;

    if (cPtr)
        return *(char*)cPtr;

    return 0;
}

// Must be called before CPools::Initialise()
void CPoolsSA::SetPoolCapacity(ePools pool, int iValue)
{
    DWORD iPtr = NULL;
    DWORD cPtr = NULL;
    switch (pool)
    {
        case BUILDING_POOL:
            iPtr = 0x55105F;
            break;
        case PED_POOL:
            iPtr = 0x550FF2;
            break;
        case OBJECT_POOL:
            iPtr = 0x551097;
            break;
        case DUMMY_POOL:
            iPtr = 0x5510CF;
            break;
        case VEHICLE_POOL:
            iPtr = 0x55102A;
            break;
        case COL_MODEL_POOL:
            iPtr = 0x551107;
            break;
        case TASK_POOL:
            iPtr = 0x55113F;
            break;
        case EVENT_POOL:
            iPtr = 0x551177;
            break;
        case TASK_ALLOCATOR_POOL:
            cPtr = 0x55124E;
            break;            // 0 - 127
        case PED_INTELLIGENCE_POOL:
            iPtr = 0x551283;
            break;
        case PED_ATTRACTOR_POOL:
            cPtr = 0x5512BB;
            break;            // 0 - 127
        case ENTRY_INFO_NODE_POOL:
            iPtr = 0x550FBA;
            break;
        case NODE_ROUTE_POOL:
            cPtr = 0x551218;
            break;            // 0 - 127
        case PATROL_ROUTE_POOL:
            cPtr = 0x5511E4;
            break;            // 0 - 127
        case POINT_ROUTE_POOL:
            cPtr = 0x5511AF;
            break;            // 0 - 127
        case POINTER_DOUBLE_LINK_POOL:
            iPtr = 0x550F82;
            break;
        case POINTER_SINGLE_LINK_POOL:
            iPtr = 0x550F46;
            break;
        case ENV_MAP_MATERIAL_POOL:
            iPtr = 0x5DA08E;
            break;
        case ENV_MAP_ATOMIC_POOL:
            iPtr = 0x5DA0CA;
            break;
        case SPEC_MAP_MATERIAL_POOL:
            iPtr = 0x5DA106;
            break;
    }
    if (iPtr)
        MemPut<int>(iPtr, iValue);

    if (cPtr)
        MemPut<char>(cPtr, iValue);
}

int CPoolsSA::GetNumberOfUsedSpaces(ePools pool)
{
    DWORD dwFunc = NULL;
    DWORD dwThis = NULL;
    switch (pool)
    {
        case BUILDING_POOL:
            dwFunc = FUNC_CBuildingPool_GetNoOfUsedSpaces;
            dwThis = CLASS_CBuildingPool;
            break;
        case PED_POOL:
            dwFunc = FUNC_CPedPool_GetNoOfUsedSpaces;
            dwThis = CLASS_CPedPool;
            break;
        case OBJECT_POOL:
            dwFunc = FUNC_CObjectPool_GetNoOfUsedSpaces;
            dwThis = CLASS_CObjectPool;
            break;
        case DUMMY_POOL:
            dwFunc = FUNC_CDummyPool_GetNoOfUsedSpaces;
            dwThis = CLASS_CDummyPool;
            break;
        case VEHICLE_POOL:
            dwFunc = FUNC_CVehiclePool_GetNoOfUsedSpaces;
            dwThis = CLASS_CVehiclePool;
            break;
        case COL_MODEL_POOL:
            dwFunc = FUNC_CColModelPool_GetNoOfUsedSpaces;
            dwThis = CLASS_CColModelPool;
            break;
        case TASK_POOL:
            dwFunc = FUNC_CTaskPool_GetNoOfUsedSpaces;
            dwThis = CLASS_CTaskPool;
            break;
        case EVENT_POOL:
            dwFunc = FUNC_CEventPool_GetNoOfUsedSpaces;
            dwThis = CLASS_CEventPool;
            break;
        case TASK_ALLOCATOR_POOL:
            dwFunc = FUNC_CTaskAllocatorPool_GetNoOfUsedSpaces;
            dwThis = CLASS_CTaskAllocatorPool;
            break;
        case PED_INTELLIGENCE_POOL:
            dwFunc = FUNC_CPedIntelligencePool_GetNoOfUsedSpaces;
            dwThis = CLASS_CPedIntelligencePool;
            break;
        case PED_ATTRACTOR_POOL:
            dwFunc = FUNC_CPedAttractorPool_GetNoOfUsedSpaces;
            dwThis = CLASS_CPedAttractorPool;
            break;
        case ENTRY_INFO_NODE_POOL:
            dwFunc = FUNC_CEntryInfoNodePool_GetNoOfUsedSpaces;
            dwThis = CLASS_CEntryInfoNodePool;
            break;
        case NODE_ROUTE_POOL:
            dwFunc = FUNC_CNodeRoutePool_GetNoOfUsedSpaces;
            dwThis = CLASS_CNodeRoutePool;
            break;
        case PATROL_ROUTE_POOL:
            dwFunc = FUNC_CPatrolRoutePool_GetNoOfUsedSpaces;
            dwThis = CLASS_CPatrolRoutePool;
            break;
        case POINT_ROUTE_POOL:
            dwFunc = FUNC_CPointRoutePool_GetNoOfUsedSpaces;
            dwThis = CLASS_CPointRoutePool;
            break;
        case POINTER_DOUBLE_LINK_POOL:
            dwFunc = FUNC_CPtrNodeDoubleLinkPool_GetNoOfUsedSpaces;
            dwThis = CLASS_CPtrNodeDoubleLinkPool;
            break;
        case POINTER_SINGLE_LINK_POOL:
            dwFunc = FUNC_CPtrNodeSingleLinkPool_GetNoOfUsedSpaces;
            dwThis = CLASS_CPtrNodeSingleLinkPool;
            break;
        default:
            return -1;
    }

    int iOut = -2;
    if (*(DWORD*)dwThis != NULL)
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

CEntryInfoNodePool* CPoolsSA::GetEntryInfoNodePool()
{
    return EntryInfoNodePool;
}

int CEntryInfoNodePoolSA::GetNumberOfUsedSpaces()
{
    DWORD dwFunc = FUNC_CEntryInfoNodePool_GetNoOfUsedSpaces;
    int   iOut = 0;
    _asm
    {
        mov     ecx, CLASS_CEntryInfoNodePool
        mov     ecx, [ecx]
        call    dwFunc
        mov     iOut, eax
    }

    return iOut;
}

CPointerNodeDoubleLinkPool* CPoolsSA::GetPointerNodeDoubleLinkPool()
{
    return PointerNodeDoubleLinkPool;
}

int CPointerNodeDoubleLinkPoolSA::GetNumberOfUsedSpaces()
{
    DWORD dwFunc = FUNC_CPtrNodeDoubleLinkPool_GetNoOfUsedSpaces;
    int   iOut = 0;
    _asm
    {
        mov     ecx, CLASS_CPtrNodeDoubleLinkPool
        mov     ecx, [ecx]
        call    dwFunc
        mov     iOut, eax
    }

    return iOut;
}

CPointerNodeSingleLinkPool* CPoolsSA::GetPointerNodeSingleLinkPool()
{
    return PointerNodeSingleLinkPool;
}

void CPoolsSA::InvalidateLocalPlayerClientEntity()
{
    m_pedPool.arrayOfClientEntities[0] = {m_pedPool.arrayOfClientEntities[0].pEntity, nullptr};
}

int CPointerNodeSingleLinkPoolSA::GetNumberOfUsedSpaces()
{
    DWORD dwFunc = FUNC_CPtrNodeSingleLinkPool_GetNoOfUsedSpaces;
    int   iOut = 0;
    _asm
    {
        mov     ecx, CLASS_CPtrNodeSingleLinkPool
        mov     ecx, [ecx]
        call    dwFunc
        mov     iOut, eax
    }

    return iOut;
}
