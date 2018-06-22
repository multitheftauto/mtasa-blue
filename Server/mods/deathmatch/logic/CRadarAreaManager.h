/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CRadarAreaManager.h
 *  PURPOSE:     Radar area entity manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CRadarArea.h"
#include <list>

class CRadarAreaManager
{
    friend class CRadarArea;

public:
    CRadarAreaManager(void);
    ~CRadarAreaManager(void);

    CRadarArea* Create(CElement* pParent, CXMLNode* pNode);
    CRadarArea* CreateFromXML(CElement* pParent, CXMLNode& Node, CEvents* pEvents);
    void        DeleteAll(void);

    unsigned int Count(void) { return static_cast<unsigned int>(m_List.size()); };
    bool         Exists(CRadarArea* pArea);

    list<CRadarArea*>::const_iterator IterBegin(void) { return m_List.begin(); };
    list<CRadarArea*>::const_iterator IterEnd(void) { return m_List.end(); };

private:
    void AddToList(CRadarArea* pArea) { m_List.push_back(pArea); };
    void RemoveFromList(CRadarArea* pArea);

    list<CRadarArea*> m_List;
};
