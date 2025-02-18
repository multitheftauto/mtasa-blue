/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CBuildingsPoolSA.h
 *  PURPOSE:     Buildings pool class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
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

    CBuilding* AddBuilding(CClientBuilding*, uint16_t modelId, CVector* vPos, CVector* vRot, uint8_t interior);
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

    std::unique_ptr<std::array<std::pair<bool, CBuildingSAInterface>, MAX_BUILDINGS>> m_pOriginalBuildingsBackup;
};
