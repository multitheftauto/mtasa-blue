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

class CClientDummy : public CClientEntity
{
    DECLARE_CLASS(CClientDummy, CClientEntity)
public:
    CClientDummy(class CClientManager* pManager, ElementID ID, const char* szTypeName);
    ~CClientDummy(void);

    void Unlink(void);

    eClientEntityType GetType(void) const { return CCLIENTDUMMY; }
    const CVector&    GetPosition(void) { return m_vecPosition; };
    void              GetPosition(CVector& vecPosition) const { vecPosition = m_vecPosition; };
    void              SetPosition(const CVector& vecPosition) { m_vecPosition = vecPosition; };

private:
    class CClientGroups* m_pGroups;
    CVector              m_vecPosition;
};
