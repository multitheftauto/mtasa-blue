/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CBuildingManager.h
 *  PURPOSE:     Building entity manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CBuilding.h"
#include <list>

using std::list;

class CBuildingManager
{
    friend class CBuilding;
public:
    CBuildingManager();
    ~CBuildingManager();

    CBuilding* Create(CElement* pParent);
    CBuilding* CreateFromXML(CElement* pParent, CXMLNode& Node, CEvents* pEvents);
    void     DeleteAll();

    unsigned int Count() { return static_cast<unsigned int>(m_List.size()); };
    bool         Exists(CBuilding* pObject) const;

    using CBuildingListType = CFastList<CBuilding*>;
    CBuildingListType::const_iterator IterBegin() const { return m_List.begin(); };
    CBuildingListType::const_iterator IterEnd() const { return m_List.end(); };

    static bool IsValidModel(unsigned long ulObjectModel);
    static bool IsValidPosition(const CVector& pos) noexcept;

private:
    void AddToList(CBuilding* pObject) { m_List.push_back(pObject); };
    void RemoveFromList(CBuilding* pObject);

    CBuildingListType m_List;
};
