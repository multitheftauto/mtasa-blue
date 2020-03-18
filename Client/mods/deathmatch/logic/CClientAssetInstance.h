/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientAssetModel.h
 *  PURPOSE:     ASset model class header
 *
 *****************************************************************************/

#pragma once

class CClientAssetInstanceManager;

class CClientAssetInstance : public CClientEntity
{
    DECLARE_CLASS(CClientAssetInstance, CClientEntity)

public:
    CClientAssetInstance(class CClientManager* pManager, ElementID ID);
    ~CClientAssetInstance();

    void Unlink();

    eClientEntityType GetType() const { return CCLIENTASSETINSTANCE; };

    void GetPosition(CVector& vecPosition) const { vecPosition = m_vecPosition; };
    void SetPosition(const CVector& vecPosition) { m_vecPosition = vecPosition; };

    void DoPulse();

protected:
    CVector                      m_vecPosition;
    CClientAssetInstanceManager* m_pAssetInstanceManager;
};
