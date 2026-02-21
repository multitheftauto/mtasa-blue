/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CElementDeleter.cpp
 *  PURPOSE:     Element lazy deletion manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CElementDeleter.h"
#include "CGame.h"

extern CGame* g_pGame;

void CElementDeleter::Delete(class CElement* pElement, bool bUnlink, bool bUpdatePerPlayerEntities)
{
    if (pElement)
    {
        if (!IsBeingDeleted(pElement))
        {
            // Before we do anything, fire the on-destroy event
            CLuaArguments Arguments;
            pElement->CallEvent("onElementDestroy", Arguments);

            // Add it to our list
            if (!pElement->IsBeingDeleted())
            {
                m_List.push_back(pElement);
            }

            g_pGame->GetPlayerManager()->ClearElementData(pElement);

            // Flag it as being deleted and unlink it from the tree/managers
            pElement->SetIsBeingDeleted(true);
            pElement->ClearChildren();
            pElement->SetParentObject(NULL, bUpdatePerPlayerEntities);

            if (bUnlink)
                pElement->Unlink();
        }
    }
}

void CElementDeleter::DoDeleteAll()
{
    // This depends on CElementDeleter::Unreference() being called in ~CElement()
    while (!m_List.empty())
        delete *m_List.begin();
}

void CElementDeleter::Unreference(CElement* element)
{
    m_List.remove(element);
}

bool CElementDeleter::IsBeingDeleted(CElement* element) const
{
    return ListContains(m_List, element);
}

void CElementDeleter::DeleteTree(CElement* rootElement, bool unlink, bool updatePerPlayerEntities)
{
    if (!rootElement || IsBeingDeleted(rootElement))
        return;

    std::vector<CElement*> elementsToDelete;
    CollectTreeElements(rootElement, elementsToDelete);

    // Fire destroy events for all elements
    for (auto* element : elementsToDelete)
    {
        if (IsBeingDeleted(element))
            continue;

        CLuaArguments arguments;
        element->CallEvent("onElementDestroy", arguments);

        if (!element->IsBeingDeleted())
            m_List.push_back(element);

        g_pGame->GetPlayerManager()->ClearElementData(element);
        element->SetIsBeingDeleted(true);
    }

    // Clear children and unlink elements
    for (auto* element : elementsToDelete)
    {
        element->ClearChildren();
        element->SetParentObject(nullptr, false);
        
        if (unlink)
            element->Unlink();
    }

    if (updatePerPlayerEntities && rootElement)
        rootElement->UpdatePerPlayerEntities();
}

void CElementDeleter::CollectTreeElements(CElement* element, std::vector<CElement*>& elements)
{
    if (!element || IsBeingDeleted(element))
        return;

    elements.push_back(element);

    for (auto iter = element->IterBegin(); iter != element->IterEnd(); ++iter)
    {
        CollectTreeElements(*iter, elements);
    }
}

void CElementDeleter::CleanUpForVM(CLuaMain* luaMain)
{
    for (auto* element : m_List)
        element->DeleteEvents(luaMain, false);
}
