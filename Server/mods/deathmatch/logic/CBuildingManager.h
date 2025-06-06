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
using CBuildingListType = CFastList<CBuilding*>;

class CBuildingManager
{
    friend class CBuilding;

public:
    CBuildingManager();
    ~CBuildingManager();

    CBuilding* Create(CElement* pParent);
    CBuilding* CreateFromXML(CElement* pParent, CXMLNode& Node, CEvents* pEvents);
    void       DeleteAll();

    CBuildingListType::const_iterator IterBegin() const noexcept { return m_List.begin(); }
    CBuildingListType::const_iterator IterEnd() const noexcept { return m_List.end(); }

    static bool IsValidModel(unsigned long ulObjectModel);
    static bool IsValidPosition(const CVector& pos) noexcept;

private:
    void AddToList(CBuilding* pBuilding) { m_List.push_back(pBuilding); };
    void RemoveFromList(CBuilding* pBuilding);

    CBuildingListType m_List;
};
