/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPointLightsManager.h
 *  PURPOSE:     PointLights entity manager class header
 *
 *****************************************************************************/

#pragma once

#include "CClientPointLights.h"
#include <list>
// class CClientPointLights;

class CClientPointLightsManager
{
    friend class CClientManager;
    friend class CClientPointLights;
    friend class CClientSearchLight;

public:
    CClientPointLightsManager(CClientManager* pManager);
    ~CClientPointLightsManager();

    CClientPointLights* Create(ElementID ID);

    void Delete(CClientPointLights* pLight);
    void DeleteAll();

    std::list<CClientPointLights*>::const_iterator IterBegin() { return m_List.begin(); };
    std::list<CClientPointLights*>::const_iterator IterEnd() { return m_List.end(); };

    static CClientPointLights* Get(ElementID ID);

    unsigned short GetDimension() { return m_usDimension; };
    void           SetDimension(unsigned short usDimension);

    void DoPulse();
    void RenderHeliLightHandler();

    static bool IsLightsLimitReached();

private:
    void AddToList(CClientPointLights* pLight) { m_List.push_back(pLight); };
    void RemoveFromList(CClientPointLights* pLight);

    void AddToSearchLightList(CClientSearchLight* pLight) { m_SearchLightList.push_back(pLight); }
    void RemoveSearchLightFromList(CClientSearchLight* pLight);

private:
    CClientManager* m_pManager;

    std::list<CClientPointLights*> m_List;
    std::list<CClientSearchLight*> m_SearchLightList;
    bool                           m_bDontRemoveFromList;
    unsigned short                 m_usDimension;
};
