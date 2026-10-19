/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CBuildingsPoolSA.h
 *  PURPOSE:     Buildings pool class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <algorithm>
#include <limits>
#include <game/CBuildingsPool.h>
#include <CVector.h>
#include "CPoolSAInterface.h"
#include "CBuildingSA.h"

class CBuildingsPoolSA : public CBuildingsPool
{
public:
    // The largest size every allocation in Resize can still express: m_nSize is an int, and both
    // MemSA::malloc_struct and the wrapper vector multiply it by their element size in 32 bits
    static constexpr std::size_t MAX_CAPACITY = std::min<std::size_t>({
        static_cast<std::size_t>(std::numeric_limits<int>::max()),
        std::numeric_limits<std::size_t>::max() / sizeof(CBuildingSAInterface),
        std::numeric_limits<std::size_t>::max() / sizeof(SClientEntity<CBuildingSA>),
    });

    CBuildingsPoolSA();
    ~CBuildingsPoolSA() = default;

    CBuilding* AddBuilding(CClientBuilding*, uint16_t modelId, CVector* vPos, CVector* vRot, uint8_t interior);
    void       RemoveBuilding(CBuilding* pBuilding);
    bool       HasFreeBuildingSlot();

    void           RemoveAllWithBackup() override;
    void           RestoreBackup() override;
    bool           Resize(int size) override;
    int            GetSize() const override { return (m_ppBuildingPoolInterface && *m_ppBuildingPoolInterface) ? (*m_ppBuildingPoolInterface)->m_nSize : 0; };
    CClientEntity* GetClientBuilding(CBuildingSAInterface* pGameInterface) const noexcept;
    CEntity*       GetBuilding(CBuildingSAInterface* pGameInterface) const noexcept;

private:
    void RemoveBuildingFromWorld(CBuildingSAInterface* pBuilding);
    bool AddBuildingToPool(CClientBuilding* pClientBuilding, CBuildingSA* pBuilding);
    void UpdateIplEntityArrayPointers(uint32_t offset, std::uintptr_t oldPoolStart, std::uintptr_t oldPoolEnd);
    void UpdateBackupLodPointers(uint32_t offset, std::uintptr_t oldPoolStart, std::uintptr_t oldPoolEnd);
    void UpdateObjectLods(uint32_t offset, std::uintptr_t oldPoolStart, std::uintptr_t oldPoolEnd);
    void RemoveVehicleDamageLinks();
    void RemovePedsContactEnityLinks();
    void RemoveObjectEntityLinks();
    void PurgeStaleSectorEntries(void* oldPool, int poolSize);

private:
    SVectorPoolData<CBuildingSA>             m_buildingPool{MAX_BUILDINGS};
    CPoolSAInterface<CBuildingSAInterface>** m_ppBuildingPoolInterface;

    using building_buffer_t = std::uint8_t[sizeof(CBuildingSAInterface)];
    using backup_entry_t = std::pair<bool, building_buffer_t>;
    using backup_container_t = std::vector<backup_entry_t>;

    std::unique_ptr<backup_container_t> m_pOriginalBuildingsBackup;

    std::unordered_map<size_t, CMatrix_Padded> m_buildingMatrix{};

    // Set by RemoveAllWithBackup after sweeping stale entity links (vehicle damage,
    // ped contact, object entity refs). Cleared by Resize to skip a redundant pass
    // when SetBuildingPoolSize calls both in the same remove/resize cycle.
    bool m_bLinkSweepsDone{false};
};
