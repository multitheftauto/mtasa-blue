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
    CClientBuilding(class CClientManager* pManager, ElementID ID, uint16_t usModelId, CVector pos, CVector4D rot, uint8_t interior);
    ~CClientBuilding();

    void Unlink(){};
    void GetPosition(CVector& vecPosition) const { vecPosition = m_vPos; };
    void SetPosition(const CVector& vecPosition) { m_vPos = vecPosition; };

    eClientEntityType GetType() const { return CCLIENTBUILDING; }

private:
    void Create();
    void Destroy();

private:
    CClientBuildingManager* m_pBuildingManager;

    CBuilding* m_pBuilding;
    uint16_t   m_usModelId;
    CVector    m_vPos;
    CVector4D  m_vRot;
    uint8_t    m_interior;
};
