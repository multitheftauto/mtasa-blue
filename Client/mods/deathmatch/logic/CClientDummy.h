/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientDummy.h
 *  PURPOSE:     Dummy entity class
 *
 *****************************************************************************/

#pragma once

#include "CClientEntity.h"

class CClientDummy final : public CClientEntity
{
    DECLARE_CLASS(CClientDummy, CClientEntity)
public:
    CClientDummy(class CClientManager* pManager, ElementID ID, const char* szTypeName);
    ~CClientDummy();

    void Unlink();

    eClientEntityType GetType() const { return CCLIENTDUMMY; }
    const CVector&    GetPosition() { return m_vecPosition; };
    void              GetPosition(CVector& vecPosition) const { vecPosition = m_vecPosition; };
    const CVector&    GetRotationRadians() { return m_vecRotation; };
    void              GetRotationRadians(CVector& vecRotation) const { vecRotation = m_vecRotation; };
    void              GetRotationDegrees(CVector& vecRotation);
    
    void              SetPosition(const CVector& vecPosition) { m_vecPosition = vecPosition; };
    void              SetRotationRadians(const CVector& vecRotation) { m_vecRotation = vecRotation; };
    void              SetRotationDegrees(const CVector& vecRotation);

private:
    class CClientGroups* m_pGroups;
    CVector              m_vecPosition;
    CVector              m_vecRotation;
};
