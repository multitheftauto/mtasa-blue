/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientRadarMarkerManager.h
 *  PURPOSE:     Radar marker entity manager class header
 *
 *****************************************************************************/

class CClientRadarMarkerManager;

#pragma once

#include "CClientRadarMarker.h"
#include <list>

class CClientRadarMarkerManager
{
    friend class CClientRadarMarker;

public:
    CClientRadarMarkerManager(class CClientManager* pManager);
    ~CClientRadarMarkerManager();

    void DoPulse();

    void DeleteAll();

    static CClientRadarMarker* Get(ElementID ID);

    unsigned short GetDimension() { return m_usDimension; }
    void           SetDimension(unsigned short usDimension);

    std::list<CClientRadarMarker*>::const_iterator IterBegin() { return m_Markers.begin(); };
    std::list<CClientRadarMarker*>::const_iterator IterEnd() { return m_Markers.end(); };

    bool        Exists(CClientRadarMarker* pMarker);
    static bool IsValidIcon(unsigned long ulIcon) noexcept { return ulIcon <= RADAR_MARKER_LIMIT; }

private:
    void AddToList(CClientRadarMarker* pMarker) { m_Markers.push_back(pMarker); };
    void RemoveFromList(CClientRadarMarker* pMarker);

    void        OrderMarkers();
    static bool CompareOrderingIndex(CClientRadarMarker* p1, CClientRadarMarker* p2);

    class CClientManager*          m_pManager;
    bool                           m_bCanRemoveFromList;
    std::list<CClientRadarMarker*> m_Markers;

    unsigned short m_usDimension;
    bool           m_bOrderOnPulse;
    CVector        m_vecCameraPosition;
};
