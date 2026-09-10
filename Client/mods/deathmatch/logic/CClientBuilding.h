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
    CClientBuilding(class CClientManager* pManager, ElementID ID, uint16_t usModelId, const CVector& pos, const CVector& rot, uint8_t interior,
                    uint16_t logicalModel = 0xFFFF);
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
    void SetDimension(unsigned short usDimension) override;

    // Buildings aren't a CClientStreamElement, so unlike objects/peds/vehicles they get no
    // automatic dimension-based streaming; this is what the manager calls on every building
    // whenever the local player's dimension changes, and what our own SetDimension calls to
    // re-evaluate this one building right after its own dimension changes.
    void RelateDimension(unsigned short usDimension);

    uint16_t GetModel() const noexcept { return m_usModelId; };
    uint16_t GetLogicalModel() const noexcept { return m_logicalModel != 0xFFFF ? m_logicalModel : m_usModelId; }
    void     SetLogicalModel(std::uint16_t logicalModel) noexcept { m_logicalModel = logicalModel; }
    void     SetModel(uint16_t ulModel, uint16_t logicalModel = 0xFFFF);

    eClientEntityType GetType() const { return CCLIENTBUILDING; }

    bool GetUsesCollision() const noexcept { return m_usesCollision; }
    void SetUsesCollision(bool state);

    unsigned char GetAlpha() const noexcept { return m_ucAlpha; }
    void          SetAlpha(unsigned char ucAlpha);

    void Create();
    void Destroy();

    bool IsValid() const noexcept { return m_pBuilding != nullptr; };

    CClientBuilding* GetLowLodBuilding() const noexcept { return m_pLowBuilding; };
    bool             SetLowLodBuilding(CClientBuilding* pLod = nullptr);
    bool             IsLod() const noexcept { return m_pHighBuilding != nullptr; };

    float GetDistanceFromCentreOfMassToBaseOfModel();

private:
    CClientBuilding* GetHighLodBuilding() const { return m_pHighBuilding; };
    void             SetHighLodBuilding(CClientBuilding* pHighBuilding = nullptr) { m_pHighBuilding = pHighBuilding; };

    void Recreate()
    {
        Destroy();
        Create();
    };

private:
    CClientBuildingManager* m_pBuildingManager;

    CBuilding*    m_pBuilding;
    uint16_t      m_usModelId;
    uint16_t      m_logicalModel = 0xFFFF;
    CVector       m_vPos;
    CVector       m_vRot;
    uint8_t       m_interior;
    bool          m_usesCollision;
    unsigned char m_ucAlpha;

    CClientBuilding* m_pHighBuilding;
    CClientBuilding* m_pLowBuilding;
};
