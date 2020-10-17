/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientCivilianManager.h
 *  PURPOSE:     Civilian ped manager class
 *
 *****************************************************************************/

class CClientCivilianManager;

#pragma once

#include "CClientCivilian.h"

#include <game/CCivilianPed.h>

#include <list>

class CClientCivilianManager
{
    friend class CClientManager;
    friend class CClientCivilian;

public:
    CClientCivilian* Create(int iPedModel, ElementID ID);
    CClientCivilian* Create(CCivilianPed* pPed, ElementID ID);
    void             DeleteAll();

    unsigned int                   Count() { return static_cast<unsigned int>(m_Civilians.size()); };
    static CClientCivilian*        Get(ElementID ID);
    static inline CClientCivilian* Get(CCivilianPed* pPed) { return reinterpret_cast<CClientCivilian*>(pPed->GetStoredPointer()); };

    static bool IsValidModel(unsigned int ulCivilianModel);

    std::list<CClientCivilian*>::iterator         IterGet(CClientCivilian* pCivilian);
    std::list<CClientCivilian*>::reverse_iterator IterGetReverse(CClientCivilian* pCivilian);
    std::list<CClientCivilian*>::iterator         IterBegin() { return m_Civilians.begin(); };
    std::list<CClientCivilian*>::iterator         IterEnd() { return m_Civilians.end(); };
    std::list<CClientCivilian*>::reverse_iterator IterReverseBegin() { return m_Civilians.rbegin(); };
    std::list<CClientCivilian*>::reverse_iterator IterReverseEnd() { return m_Civilians.rend(); };

private:
    CClientCivilianManager(class CClientManager* pManager);
    ~CClientCivilianManager();

    void AddToList(CClientCivilian* pCivilian) { m_Civilians.push_back(pCivilian); };
    void RemoveFromList(CClientCivilian* pCivilian);

    class CClientManager*       m_pManager;
    bool                        m_bCanRemoveFromList;
    std::list<CClientCivilian*> m_Civilians;
};
