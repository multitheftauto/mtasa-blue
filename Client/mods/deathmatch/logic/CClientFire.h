/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClientFire.h
 *  PURPOSE:     Fire Manipulation Class
 *  DEVELOPERS:  Fedor Sinev
 *
 *****************************************************************************/

#pragma once

#include "CClientEntity.h"

class CClientFire : public CClientEntity
{
public:
    CClientFire(CClientManager* pManager, ElementID ID, CVector& vecPosition, float fSize = 1.8, bool bSilent = false);
    ~CClientFire(void);

    eClientEntityType GetType() const { return CCLIENTFIRE; };
    void              Unlink();

    // GetInterface points to CFireInterace which is empty, this function is a dud. Added for consistency
    CFireInterface* GetInterface() { m_pFire->GetInterface(); };

    void GetPosition(CVector& vecPosition) const { vecPosition = *m_pFire->GetPosition(); };
    void SetPosition(const CVector& vecPosition) { m_pFire->SetPosition((CVector)vecPosition); };

    void     SetTarget(CEntity* Entity) { m_pFire->SetTarget(Entity); };
    CEntity* GetCreator() { m_pFire->GetCreator(); };
    CEntity* GetEntityOnFire() { m_pFire->GetEntityOnFire(); };

    void  SetSilent(bool bSilent) { m_pFire->SetSilent(bSilent); };
    void  SetStrength(float fStrength) { m_pFire->SetStrength(fStrength); };
    float GetStrength() { m_pFire->GetStrength(); };

    void Ignite() { m_pFire->Ignite(); };            // should re-ignite a position, or reset data
    void Extinguish() { m_pFire->Extinguish(); };
    bool IsFireIgnited() { m_pFire->IsIgnited(); };
    bool IsBeingExtinguished() { m_pFire->IsBeingExtinguished(); };

    void          SetTimeToBurnOut(unsigned long ulTime) { m_pFire->SetTimeToBurnOut(ulTime); };
    unsigned long GetTimeToBurnOut() { m_pFire->GetTimeToBurnOut(); };

    void SetNumOfGenerationsAllowed(char chNum) { m_pFire->SetNumGenerationsAllowed(chNum); };

private:
    CFire*              m_pFire;
    CClientFireManager* m_pManager;

    friend CClientFireManager;
};
