/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CElementDeleter.h
 *  PURPOSE:     Element lazy deletion manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CElement.h"
#include <list>
#include <vector>

class CElementDeleter
{
public:
    ~CElementDeleter() { DoDeleteAll(); };

    void Delete(class CElement* pElement, bool bUnlink = true, bool bUpdatePerPlayerEntities = true);
    void DeleteTree(CElement* rootElement, bool unlink = true, bool updatePerPlayerEntities = true);
    void DoDeleteAll();

    bool IsBeingDeleted(CElement* element) const;
    void Unreference(CElement* element);
    void CleanUpForVM(CLuaMain* luaMain);

private:
    void CollectTreeElements(CElement* element, std::vector<CElement*>& elements);
    CElementListType m_List;
};
