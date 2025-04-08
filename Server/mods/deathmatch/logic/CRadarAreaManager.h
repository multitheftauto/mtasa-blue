/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CRadarAreaManager.h
 *  PURPOSE:     Radar area entity manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CRadarArea.h"
#include <list>

class CRadarAreaManager
{
    friend class CRadarArea;

public:
    CRadarAreaManager();
    ~CRadarAreaManager();

    CRadarArea* Create(CElement* pParent);
    CRadarArea* CreateFromXML(CElement* pParent, CXMLNode& Node, CEvents* pEvents);
    void        DeleteAll();

    unsigned int Count() { return static_cast<unsigned int>(m_List.size()); };
    bool         Exists(CRadarArea* pArea);

    std::list<CRadarArea*>::const_iterator IterBegin() { return m_List.begin(); };
    std::list<CRadarArea*>::const_iterator IterEnd() { return m_List.end(); };

private:
    void AddToList(CRadarArea* pArea) { m_List.push_back(pArea); };
    void RemoveFromList(CRadarArea* pArea);

    std::list<CRadarArea*> m_List;
};
