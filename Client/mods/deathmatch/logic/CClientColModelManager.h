/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientColModelManager.cpp
 *  PURPOSE:     Model collision entity manager class
 *
 *****************************************************************************/

#pragma once

#include <list>
#include "CClientColModel.h"

class CClientColModelManager
{
    friend class CClientColModel;

public:
    CClientColModelManager(class CClientManager* pManager);
    ~CClientColModelManager();

    void RemoveAll();
    bool Exists(CClientColModel* pCol);

    CClientColModel* GetElementThatReplaced(unsigned short usModel, CClientColModel* pDontSearch = NULL);

    static bool IsReplacableModel(unsigned short usModel);
    bool        RestoreModel(unsigned short usModel);

    std::list<CClientColModel*>::const_iterator IterBegin() { return m_List.begin(); }
    std::list<CClientColModel*>::const_iterator IterEnd() { return m_List.end(); }

private:
    void AddToList(CClientColModel* pCol) { m_List.push_back(pCol); }
    void RemoveFromList(CClientColModel* pCol);

    std::list<CClientColModel*> m_List;
    bool                        m_bRemoveFromList;
};
