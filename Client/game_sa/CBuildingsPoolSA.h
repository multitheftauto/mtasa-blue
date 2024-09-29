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

    CBuilding* AddBuilding(CClientBuilding*, uint16_t modelId, CVector* vPos, CVector4D* vRot, uint8_t interior);
    void       RemoveBuilding(CBuilding* pBuilding);
    bool       HasFreeBuildingSlot();

    void RemoveAllBuildings() override;
    void RestoreAllBuildings() override;
    bool Resize(int size) override;
    int  GetSize() const override { return (*m_ppBuildingPoolInterface)->m_nSize; };

private:
    void RemoveBuildingFromWorld(CBuildingSAInterface* pBuilding);
    bool AddBuildingToPool(CClientBuilding* pClientBuilding, CBuildingSA* pBuilding);
    void UpdateIplEntrysPointers(uint32_t offset);
    void UpdateBackupLodPointers(uint32_t offset);
    void RemoveVehicleDamageLinks();
    void RemovePedsContactEnityLinks();

private:
    SVectorPoolData<CBuildingSA> m_buildingPool{MAX_BUILDINGS};
    CPoolSAInterface<CBuildingSAInterface>**           m_ppBuildingPoolInterface;

    typedef std::uint8_t building_buffer_t[sizeof(CBuildingSAInterface)];
    typedef std::array<std::pair<bool, building_buffer_t>, MAX_BUILDINGS> backup_array_t;

    std::unique_ptr<backup_array_t> m_pOriginalBuildingsBackup;
};
