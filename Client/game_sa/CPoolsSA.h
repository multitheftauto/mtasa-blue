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

#define INVALID_POOL_ARRAY_ID 0xFFFFFFFF

class CClientEntity;

// size of tPoolObjectFlags is 1 byte only
union tPoolObjectFlags
{
    struct
    {
        unsigned char nId : 7;
        bool          bEmpty : 1;
    };

private:
    unsigned char nValue;
};

template <class A, class B = A>
class CPoolSAInterface
{
public:
    // m_pObjects contains all interfaces. 140 maximum for ped objects.
    B*                m_pObjects;
    tPoolObjectFlags* m_byteMap;
    int               m_nSize;
    int               m_nFirstFree;
    bool              m_bOwnsAllocations;
    bool              field_11;

    // Default constructor for statically allocated pools
    CPoolSAInterface()
    {
        // Remember to call CPool::Init to fill in the fields!
        m_pObjects = nullptr;
        m_byteMap = nullptr;
        m_nSize = 0;
        m_bOwnsAllocations = false;
    }

    bool IsEmpty(std::int32_t objectIndex) { return m_byteMap[objectIndex].bEmpty; }
    B*   GetObject(std::int32_t objectIndex) { return &m_pObjects[objectIndex]; }
};

class CPoolsSA : public CPools
{
public:
    CPoolsSA();
    ~CPoolsSA();

    // Vehicles pool
    CVehicle* AddVehicle(CClientVehicle* pClientVehicle, eVehicleTypes eVehicleType, unsigned char ucVariation, unsigned char ucVariation2);

private:
    bool AddVehicleToPool(CClientVehicle* pClientVehicle, CVehicleSA* pVehicle);

public:
    void                       RemoveVehicle(CVehicle* pVehicle, bool bDelete = true);
    SClientEntity<CVehicleSA>* GetVehicle(DWORD* pGameInterface);
    unsigned long              GetVehicleCount()
    {
        return m_vehiclePool.ulCount;
        ;
    }
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
    CPed* AddPed(CClientPed* pClientPed, ePedModel ePedType);
    CPed* AddPed(CClientPed* pClientPed, DWORD* pGameInterface);

private:
    bool AddPedToPool(CClientPed* pClientPed, CPedSA* pPed);

public:
    void RemovePed(CPed* ped, bool bDelete = true);
    SClientEntity<CPedSA>* GetPed(DWORD* pGameInterface);
    CPed*            GetPedFromRef(DWORD dwGameRef);
    CPedSAInterface* GetPedInterface(DWORD dwGameRef);            // game_sa specific
    unsigned long    GetPedCount() { return m_pedPool.ulCount; }
    void             DeleteAllPeds();

    CEntity*       GetEntity(DWORD* pGameInterface);
    CClientEntity* GetClientEntity(DWORD* pGameInterface);
    uint           GetModelIdFromClump(RpClump* pRpClump);

    // Others
    CVehicle*  AddTrain(CClientVehicle* pClientVehicle, CVector* vecPosition, DWORD dwModels[], int iSize, bool bDirection, uchar ucTrackId = 0xFF);

    DWORD GetPedPoolIndex(std::uint8_t* pInterface);
    DWORD GetVehiclePoolIndex(std::uint8_t* pInterfacee);
    DWORD GetObjectPoolIndex(std::uint8_t* pInterface);

    int  GetNumberOfUsedSpaces(ePools pools);
    int  GetPoolDefaultCapacity(ePools pool);
    int  GetPoolCapacity(ePools pool);
    void SetPoolCapacity(ePools pool, int iValue);

    void ResetPedPoolCount() { m_pedPool.ulCount = 0; }
    void InvalidateLocalPlayerClientEntity();

private:
    // Generic container for pools
    template <class T, class I, unsigned long MAX>
    struct SPoolData
    {
        std::array<SClientEntity<T>, MAX> arrayOfClientEntities;
        unsigned long                     ulCount;

    private:
        friend class CPoolsSA;

        SPoolData() : ulCount(0UL)
        {
            for (unsigned int i = 0; i < MAX; ++i)
            {
                arrayOfClientEntities[i] = {nullptr, nullptr};
            }
        }
    };

    // Pools
    typedef SPoolData<CVehicleSA, CVehicleSAInterface, MAX_VEHICLES> vehiclePool_t;
    typedef SPoolData<CPedSA, CPedSAInterface, MAX_PEDS>             pedPool_t;
    typedef SPoolData<CObjectSA, CObjectSAInterface, MAX_OBJECTS>    objectPool_t;
    vehiclePool_t                                                    m_vehiclePool;
    pedPool_t                                                        m_pedPool;
    objectPool_t                                                     m_objectPool;
    CPoolSAInterface<CPedSAInterface>**                              m_ppPedPoolInterface;
    CPoolSAInterface<CObjectSAInterface>**                           m_ppObjectPoolInterface;
    CPoolSAInterface<CVehicleSAInterface>**                          m_ppVehiclePoolInterface;

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
