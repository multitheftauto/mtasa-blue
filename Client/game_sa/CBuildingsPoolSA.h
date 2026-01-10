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

#include <game/CBuildingsPool.h>
#include <CVector.h>
#include "CPoolSAInterface.h"
#include "CBuildingSA.h"

class CBuildingsPoolSA : public CBuildingsPool
{
public:
    CBuildingsPoolSA();
    ~CBuildingsPoolSA() = default;

    CBuilding* AddBuilding(CClientBuilding*, uint16_t modelId, CVector* vPos, CVector* vRot, uint8_t interior, bool anim);
    void       RemoveBuilding(CBuilding* pBuilding);
    bool       HasFreeBuildingSlot();

    void RemoveAllWithBackup() override;
    void RestoreBackup() override;
    bool Resize(int size) override;
    int  GetSize() const override { return (*m_ppBuildingPoolInterface)->m_nSize; };
    CClientEntity* GetClientBuilding(CBuildingSAInterface* pGameInterface) const noexcept;

private:
    void RemoveBuildingFromWorld(CBuildingSAInterface* pBuilding);
    bool AddBuildingToPool(CClientBuilding* pClientBuilding, CBuildingSA* pBuilding);
    void UpdateIplEntrysPointers(uint32_t offset);
    void UpdateBackupLodPointers(uint32_t offset);
    void RemoveVehicleDamageLinks();
    void RemovePedsContactEnityLinks();

private:
    SVectorPoolData<CBuildingSA>             m_buildingPool{MAX_BUILDINGS};
    CPoolSAInterface<CBuildingSAInterface>** m_ppBuildingPoolInterface;

    using building_buffer_t = std::uint8_t[sizeof(CBuildingSAInterface)];
    using backup_array_t = std::array<std::pair<bool, building_buffer_t>, MAX_BUILDINGS>;

    std::unique_ptr<backup_array_t> m_pOriginalBuildingsBackup;
};
