/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientEffectManager.h
 *  PURPOSE:     Effect manager
 *
 *****************************************************************************/

#pragma once

#include "CClientEffect.h"

#include <game/CFxSystem.h>
#include <game/CFxManager.h>

class CClientEffectManager
{
    friend class CClientManager;

public:
    CClientEffect* Create(const SString& strEffectName, const CVector& vecPosition, ElementID ID, bool bSoundEnable);
    void           SAEffectDestroyed(void* pFxSAInterface);
    void           DeleteAll();

    unsigned int          Count() { return m_Effects.size(); };
    static CClientEffect* Get(ElementID ID);
    void                  AddToList(CClientEffect* pEffect) { m_Effects.push_back(pEffect); }
    void                  RemoveFromList(CClientEffect* pEffect);

private:
    CClientEffectManager(class CClientManager* pManager);
    ~CClientEffectManager();
    CClientEffect* Get(void* pFxSA);

    class CClientManager*     m_pManager;
    bool                      m_bCanRemoveFromList;
    std::list<CClientEffect*> m_Effects;
};
