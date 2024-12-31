/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPoolsSA.h
 *  PURPOSE:     Header file for game entity pools class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <game/CPools.h>
#include "CPedSA.h"
#include "CVehicleSA.h"
#include "CObjectSA.h"
#include "CBuildingSA.h"
#include "CBuildingsPoolSA.h"
#include "CDummyPoolSA.h"
#include "CTxdPoolSA.h"

#define INVALID_POOL_ARRAY_ID 0xFFFFFFFF

class CClientEntity;

class CPoolsSA : public CPools
{
public:
    CPoolsSA();
    ~CPoolsSA();

    // Vehicles pool
    CVehicle* AddVehicle(CClientVehicle* pClientVehicle, std::uint16_t model, std::uint8_t variation, std::uint8_t variation2) noexcept;

private:
    bool AddVehicleToPool(CClientVehicle* pClientVehicle, CVehicleSA* pVehicle);

public:
    void                       RemoveVehicle(CVehicle* pVehicle, bool bDelete = true);
    SClientEntity<CVehicleSA>* GetVehicle(DWORD* pGameInterface);
    SClientEntity<CVehicleSA>* GetVehicle(size_t pos) { return &m_vehiclePool.arrayOfClientEntities[pos]; };
    unsigned long              GetVehicleCount() { return m_vehiclePool.ulCount; };
    void DeleteAllVehicles();

    // Objects pool
    CObject* AddObject(CClientObject* pClientObject, DWORD dwModelID, bool bLowLod, bool bBreakingDisabled);

private:
    bool AddObjectToPool(CClientObject* pClientObject, CObjectSA* pObject);

public:
    void                      RemoveObject(CObject* pObject, bool bDelete = true);
    SClientEntity<CObjectSA>* GetObject(DWORD* pGameInterface);
    CObject*                  GetObjectFromIndex(std::uint32_t elementIndexInPool);
    unsigned long             GetObjectCount() { return m_objectPool.ulCount; }
    void                      DeleteAllObjects();

    // Peds pool
    CPed* AddPed(CClientPed* pClientPed, unsigned int nModelIndex);
    CPed* AddPed(CClientPed* pClientPed, DWORD* pGameInterface);

private:
    bool AddPedToPool(CClientPed* pClientPed, CPedSA* pPed);

public:
    void                   RemovePed(CPed* ped, bool bDelete = true);
    SClientEntity<CPedSA>* GetPed(DWORD* pGameInterface);
    SClientEntity<CPedSA>* GetPed(size_t pos) { return &m_pedPool.arrayOfClientEntities[pos]; };
    CPed*                  GetPedFromRef(DWORD dwGameRef);
    CPedSAInterface*       GetPedInterface(DWORD dwGameRef);            // game_sa specific
    unsigned long          GetPedCount() { return m_pedPool.ulCount; }
    void                   DeleteAllPeds();

    CEntity*       GetEntity(DWORD* pGameInterface);
    CClientEntity* GetClientEntity(DWORD* pGameInterface);
    uint           GetModelIdFromClump(RpClump* pRpClump);

    // Others
    CVehicle* AddTrain(CClientVehicle* pClientVehicle, const CVector& vecPosition, std::vector<DWORD> models, bool bDirection,
                       std::uint8_t ucTrackId = 255) noexcept;

    DWORD GetPedPoolIndex(std::uint8_t* pInterface);
    DWORD GetVehiclePoolIndex(std::uint8_t* pInterfacee);
    DWORD GetObjectPoolIndex(std::uint8_t* pInterface);

    int  GetNumberOfUsedSpaces(ePools pools);
    int  GetPoolDefaultCapacity(ePools pool);
    int  GetPoolDefaultModdedCapacity(ePools pool);
    int  GetPoolCapacity(ePools pool);
    void SetPoolCapacity(ePools pool, int iValue);

    void ResetPedPoolCount() { m_pedPool.ulCount = 0; }
    void InvalidateLocalPlayerClientEntity();

    CBuildingsPool& GetBuildingsPool() noexcept override { return m_BuildingsPool; };
    CDummyPool&     GetDummyPool() noexcept { return m_DummyPool; };
    CTxdPool&       GetTxdPool() noexcept { return m_TxdPool; };

private:
    // Pools
    SPoolData<CVehicleSA, CVehicleSAInterface, MAX_VEHICLES>    m_vehiclePool;
    SPoolData<CPedSA, CPedSAInterface, MAX_PEDS>                m_pedPool;
    SPoolData<CObjectSA, CObjectSAInterface, MAX_OBJECTS>       m_objectPool;

    CPoolSAInterface<CPedSAInterface>**              m_ppPedPoolInterface;
    CPoolSAInterface<CObjectSAInterface>**           m_ppObjectPoolInterface;
    CPoolSAInterface<CVehicleSAInterface>**          m_ppVehiclePoolInterface;

    CBuildingsPoolSA m_BuildingsPool;
    CDummyPoolSA     m_DummyPool;
    CTxdPoolSA       m_TxdPool;

    bool m_bGetVehicleEnabled;
};

#define FUNC_GetPed 0x54ff90

#define CLASS_CPool_Vehicle 0xB74494
#define CLASS_CPool_Ped 0xB74490
#define CLASS_CPool_Object 0xB7449C

#define CLASS_CBuildingPool 0xb74498
#define CLASS_CPedPool 0xb74490
#define CLASS_CObjectPool 0xb7449c
#define CLASS_CDummyPool 0xb744a0
#define CLASS_CVehiclePool 0xb74494
#define CLASS_CColModelPool 0xb744a4
#define CLASS_CTaskPool 0xb744a8
#define CLASS_CEventPool 0xb744ac
#define CLASS_CTaskAllocatorPool 0xb744bc
#define CLASS_CPedIntelligencePool 0xb744c0
#define CLASS_CPedAttractorPool 0xb744c4
#define CLASS_CEntryInfoNodePool 0xb7448c
#define CLASS_CNodeRoutePool 0xb744b8
#define CLASS_CPatrolRoutePool 0xb744b4
#define CLASS_CPointRoutePool 0xb744b0
#define CLASS_CPtrNodeDoubleLinkPool 0xB74488
#define CLASS_CPtrNodeSingleLinkPool 0xB74484

#define FUNC_CBuildingPool_GetNoOfUsedSpaces 0x550620
#define FUNC_CPedPool_GetNoOfUsedSpaces 0x5504A0
#define FUNC_CObjectPool_GetNoOfUsedSpaces 0x54F6B0
#define FUNC_CDummyPool_GetNoOfUsedSpaces 0x5507A0
#define FUNC_CVehiclePool_GetNoOfUsedSpaces 0x42CCF0
#define FUNC_CColModelPool_GetNoOfUsedSpaces 0x550870
#define FUNC_CTaskPool_GetNoOfUsedSpaces 0x550940
#define FUNC_CEventPool_GetNoOfUsedSpaces 0x550A10
#define FUNC_CTaskAllocatorPool_GetNoOfUsedSpaces 0x550d50
#define FUNC_CPedIntelligencePool_GetNoOfUsedSpaces 0x550E20
#define FUNC_CPedAttractorPool_GetNoOfUsedSpaces 0x550ef0
#define FUNC_CEntryInfoNodePool_GetNoOfUsedSpaces 0x5503d0
#define FUNC_CNodeRoutePool_GetNoOfUsedSpaces 0x550c80
#define FUNC_CPatrolRoutePool_GetNoOfUsedSpaces 0x550bb0
#define FUNC_CPointRoutePool_GetNoOfUsedSpaces 0x550ae0
#define FUNC_CPtrNodeSingleLinkPool_GetNoOfUsedSpaces 0x550230
#define FUNC_CPtrNodeDoubleLinkPool_GetNoOfUsedSpaces 0x550300

#define FUNC_CTrain_CreateMissionTrain 0x6F7550
#define VAR_TrainModelArray 0x8D44F8
