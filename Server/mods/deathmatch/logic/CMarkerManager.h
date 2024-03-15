/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CMarkerManager.h
 *  PURPOSE:     Marker entity manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CMarkerManager;

#pragma once

#include "CColManager.h"
#include "CMarker.h"
#include <list>

class CMarker;

class CMarkerManager
{
    friend class CMarker;

public:
    CMarkerManager(CColManager* pColManager);
    ~CMarkerManager() { DeleteAll(); };

    CMarker* Create(CElement* pParent);
    CMarker* CreateFromXML(CElement* pParent, CXMLNode& Node, CEvents* pEvents);
    void     DeleteAll();

    std::uint32_t Count() { return static_cast<std::uint32_t>(m_Markers.size()); };
    bool         Exists(CMarker* pMarker);

    std::list<CMarker*>::const_iterator IterBegin() { return m_Markers.begin(); };
    std::list<CMarker*>::const_iterator IterEnd() { return m_Markers.end(); };

    static int           StringToType(const char* szString);
    static bool          TypeToString(std::uint32_t uiType, char* szString);
    static std::uint8_t StringToIcon(const char* szString);
    static bool          IconToString(std::uint8_t ucIcon, char* szString);

private:
    void AddToList(CMarker* pMarker) { m_Markers.push_back(pMarker); };
    void RemoveFromList(CMarker* pMarker);

    CColManager*        m_pColManager;
    std::list<CMarker*> m_Markers;
};
