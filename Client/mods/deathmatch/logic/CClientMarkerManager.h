/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientMarkerManager.h
 *  PURPOSE:     Marker entity manager class
 *
 *****************************************************************************/

#pragma once

#include "CClientMarker.h"
#include <list>

class CClientMarkerManager
{
    friend class CClientManager;
    friend class CClientMarker;
    friend class CClientGame;

public:
    unsigned int          Count() { return static_cast<unsigned int>(m_Markers.size()); };
    static CClientMarker* Get(ElementID ID);
    void                  DeleteAll();
    static bool           IsMarkerModel(unsigned short usModel);

private:
    CClientMarkerManager(class CClientManager* pManager);
    ~CClientMarkerManager();
    void DoPulse();
    void AddToList(CClientMarker* pCheckpoint) { m_Markers.push_back(pCheckpoint); };
    void RemoveFromList(CClientMarker* pCheckpoint)
    {
        if (m_bCanRemoveFromList)
            m_Markers.remove(pCheckpoint);
    };

    class CClientManager*     m_pManager;
    CFastList<CClientMarker*> m_Markers;
    bool                      m_bCanRemoveFromList;
};
