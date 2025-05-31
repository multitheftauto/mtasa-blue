/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CGroups.h
 *  PURPOSE:     Entity (dummy) group manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CDummy.h"
#include "CElement.h"
#include <list>

class CGroups
{
    friend class CDummy;

public:
    CGroups() {}
    ~CGroups() { DeleteAll(); }

    CDummy* Create(CElement* pParent);
    CDummy* CreateFromXML(CElement* pParent, CXMLNode& Node, CEvents* pEvents);

    void Delete(CDummy* pDummy);
    void DeleteAll();

    std::list<CDummy*>::const_iterator IterBegin() { return m_List.begin(); }
    std::list<CDummy*>::const_iterator IterEnd() { return m_List.end(); }

private:
    void AddToList(CDummy* pDummy) { m_List.push_back(pDummy); };
    void RemoveFromList(CDummy* pDummy);

    std::list<CDummy*> m_List;
};
