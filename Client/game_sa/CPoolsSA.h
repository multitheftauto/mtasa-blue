/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CPoolsSA.h
 *  PURPOSE:     Header file for game entity pools class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
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
    CPed* AddPed(CClientPed* pClientPed, unsigned int nModelIndex);
    CPed* AddPed(CClientPed* pClientPed, DWORD* pGameInterface);

private:
    bool AddPedToPool(CClientPed* pClientPed, CPedSA* pPed);

public:
    void                   RemovePed(CPed* ped, bool bDelete = true);
    SClientEntity<CPedSA>* GetPed(DWORD* pGameInterface);
    CPed*                  GetPedFromRef(DWORD dwGameRef);
    CPedSAInterface*       GetPedInterface(DWORD dwGameRef);            // game_sa specific
    unsigned long          GetPedCount() { return m_pedPool.ulCount; }
    void                   DeleteAllPeds();

    CEntity*       GetEntity(DWORD* pGameInterface);
    CClientEntity* GetClientEntity(DWORD* pGameInterface);
    uint           GetModelIdFromClump(RpClump* pRpClump);

    // Others
    CVehicle* AddTrain(CClientVehicle* pClientVehicle, CVector* vecPosition, DWORD dwModels[], int iSize, bool bDirection, uchar ucTrackId = 0xFF);

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
