/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientBuilding.h
 *  PURPOSE:     Physical object entity class
 *
 *****************************************************************************/

class CClientBuilding;

#pragma once

#include <game/CBuilding.h>

class CClientBuilding : public CClientEntity
{
    DECLARE_CLASS(CClientBuilding, CClientEntity)
    friend class CClientBuildingManager;

public:
    CClientBuilding(class CClientManager* pManager, ElementID ID, uint16_t usModelId, const CVector &pos, const CVector &rot, uint8_t interior);
    ~CClientBuilding();

    void Unlink();
    void GetPosition(CVector& vecPosition) const override { vecPosition = m_vPos; };
    void SetPosition(const CVector& vecPosition) override;

    void GetRotationRadians(CVector& vecOutRadians) const override { vecOutRadians = m_vRot; };
    void SetRotationRadians(const CVector& vecRadians) override;

    CBuilding*     GetBuildingEntity() const { return m_pBuilding; };
    CEntity*       GetGameEntity() override { return m_pBuilding; };
    const CEntity* GetGameEntity() const override { return m_pBuilding; };

    bool SetMatrix(const CMatrix& matrix) override;

    void SetInterior(uint8_t ucInterior) override;

    uint16_t GetModel() const noexcept { return m_usModelId; };
    void     SetModel(uint16_t ulModel);

    eClientEntityType GetType() const { return CCLIENTBUILDING; }

    void SetUsesCollision(bool state);

    void Create();
    void Destroy();

    bool IsValid() const noexcept { return m_pBuilding != nullptr; };

    
    CClientBuilding* GetLowLodBuilding() const noexcept { return m_pLowBuilding; };
    bool SetLowLodBuilding(CClientBuilding* pLod = nullptr);
    bool IsLod() const noexcept { return m_pHighBuilding != nullptr; };


private:
    CClientBuilding* GetHighLodBuilding() const { return m_pHighBuilding; }; 
    void SetHighLodBuilding(CClientBuilding* pHighBuilding = nullptr) { m_pHighBuilding = pHighBuilding; };

    void Recreate()
    {
        Destroy();
        Create();
    };

private:
    CClientBuildingManager* m_pBuildingManager;

    CBuilding* m_pBuilding;
    uint16_t   m_usModelId;
    CVector    m_vPos;
    CVector    m_vRot;
    uint8_t    m_interior;
    bool       m_usesCollision;

    CClientBuilding* m_pHighBuilding;
    CClientBuilding* m_pLowBuilding;
};
