/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CPools.h
 *  PURPOSE:     Game pool interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Common.h"
#include "CBuildingsPool.h"
#include "CDummyPool.h"
#include "CTxdPool.h"
#include "CPtrNodeSingleLinkPool.h"

class CClientEntity;
class CEntity;
class CEntitySAInterface;
class CObject;
class CObjectSA;
class CPed;
class CPedSA;
class CBuilding;
class CBuildingSA;
class CVector;
class CVehicle;
class CVehicleSA;
struct RpClump;

enum ePools
{
    BUILDING_POOL = 0,
    PED_POOL,
    OBJECT_POOL,
    DUMMY_POOL,
    VEHICLE_POOL,
    COL_MODEL_POOL,
    TASK_POOL,
    EVENT_POOL,
    TASK_ALLOCATOR_POOL,
    PED_INTELLIGENCE_POOL,
    PED_ATTRACTOR_POOL,
    ENTRY_INFO_NODE_POOL,
    NODE_ROUTE_POOL,
    PATROL_ROUTE_POOL,
    POINT_ROUTE_POOL,
    POINTER_DOUBLE_LINK_POOL,
    POINTER_SINGLE_LINK_POOL,
    ENV_MAP_MATERIAL_POOL,
    ENV_MAP_ATOMIC_POOL,
    SPEC_MAP_MATERIAL_POOL,
    MAX_POOLS
};

template <class T>
struct SClientEntity
{
    T*             pEntity;
    CClientEntity* pClientEntity;
};

class CPools
{
public:
    // Vehicles pool
    virtual CVehicle* AddVehicle(class CClientVehicle* pClientVehicle, std::uint16_t model, std::uint8_t variation, std::uint8_t variation2) noexcept = 0;
    virtual void      RemoveVehicle(CVehicle* pVehicle, bool bDelete = true) = 0;

    virtual SClientEntity<CVehicleSA>* GetVehicle(DWORD* pGameInterface) = 0;
    virtual SClientEntity<CVehicleSA>* GetVehicle(size_t pos) = 0;
    virtual unsigned long              GetVehicleCount() = 0;

    // Objects pool
    virtual CObject* AddObject(class CClientObject* pClientObject, DWORD dwModelID, bool bLowLod, bool bBreakingDisabled) = 0;
    virtual void     RemoveObject(CObject* pObject, bool bDelete = true) = 0;

    virtual SClientEntity<CObjectSA>* GetObject(DWORD* pGameInterface) = 0;
    virtual CObject*                  GetObjectFromIndex(std::uint32_t elementIndexInPool) = 0;
    virtual unsigned long             GetObjectCount() = 0;

    // Peds pool
    virtual CPed* AddPed(class CClientPed* pClientPed, unsigned int nModelIndex) = 0;
    virtual CPed* AddPed(class CClientPed* pClientPed, DWORD* pGameInterface) = 0;
    virtual void  RemovePed(CPed* pPed, bool bDelete = true) = 0;

    virtual SClientEntity<CPedSA>* GetPed(DWORD* pGameInterface) = 0;            // not sure we really want this here
    virtual SClientEntity<CPedSA>* GetPed(size_t pos) = 0;
    virtual CPed*                  GetPedFromRef(DWORD dwGameRef) = 0;
    virtual unsigned long          GetPedCount() = 0;

    // Others
    virtual CVehicle* AddTrain(class CClientVehicle* pClientVehicle, const CVector& vecPosition, std::vector<DWORD> models, bool iDirection,
                               std::uint8_t ucTrackId = 255) noexcept = 0;

    virtual CEntity*       GetEntity(DWORD* pGameInterface) = 0;
    virtual CClientEntity* GetClientEntity(DWORD* pGameInterface) = 0;
    virtual uint           GetModelIdFromClump(RpClump* pRpClump) = 0;

    virtual int  GetNumberOfUsedSpaces(ePools pool) = 0;
    virtual int  GetPoolDefaultCapacity(ePools pool) = 0;
    virtual int  GetPoolDefaultModdedCapacity(ePools pool) = 0;
    virtual int  GetPoolCapacity(ePools pool) = 0;
    virtual void SetPoolCapacity(ePools pool, int iValue) = 0;

    virtual void ResetPedPoolCount() = 0;
    virtual void InvalidateLocalPlayerClientEntity() = 0;

    virtual CBuildingsPool& GetBuildingsPool() noexcept = 0;
    virtual CDummyPool&     GetDummyPool() noexcept = 0;
    virtual CTxdPool&       GetTxdPool() noexcept = 0;
    virtual CPtrNodeSingleLinkPool& GetPtrNodeSingleLinkPool() noexcept = 0;
};
