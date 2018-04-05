/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CElementDeleter.cpp
 *  PURPOSE:     Manager for elements to be destroyed
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

using std::list;

CElementDeleter::CElementDeleter()
{
    // Allow unrefernces
    m_bAllowUnreference = true;
}

void CElementDeleter::Delete(class CClientEntity* pElement)
{
    // Make sure we don't try to delete it twice
    if (pElement && !IsBeingDeleted(pElement))
    {
        // Before we do anything, fire the on-destroy event
        CLuaArguments Arguments;
        pElement->CallEvent("onClientElementDestroy", Arguments, true);

        // Add it to our list
        if (!pElement->IsBeingDeleted())
        {
            // Just to be clear, not a Lua event
            OnClientSpecialElementDestroy(pElement);
            m_List.push_back(pElement);
        }

        // Flag it as being deleted and unlink it from the tree/managers
        pElement->SetBeingDeleted(true);
        pElement->ClearChildren();
        pElement->SetParent(NULL);
        pElement->Unlink();
    }
}

void CElementDeleter::DeleteRecursive(class CClientEntity* pElement)
{
    // Gather a list over children (we can't use the list as it changes)
    list<CClientEntity*>            Children;
    CChildListType ::const_iterator iterCopy = pElement->IterBegin();
    for (; iterCopy != pElement->IterEnd(); ++iterCopy)
    {
        Children.push_back(*iterCopy);
    }

    // Call ourselves on each child of this to go as deep as possible and start deleting there
    list<CClientEntity*>::const_iterator iter = Children.begin();
    for (; iter != Children.end(); ++iter)
    {
        DeleteRecursive(*iter);
    }

    // At this point we're sure that this element has no more children left.
    // Add it to our list over deleting objects
    if (!pElement->IsBeingDeleted())
    {
        // Just to be clear, not a Lua event
        OnClientSpecialElementDestroy(pElement);
        m_List.push_back(pElement);
    }

    // Mark us as being deleted, unlink from parent and unlink from manager classes eventually
    pElement->SetBeingDeleted(true);
    pElement->SetParent(NULL);
    pElement->Unlink();
}

// If DeleteElementSpecial returns true then do not access pElement
// because element has been deleted
bool CElementDeleter::DeleteElementSpecial(CClientEntity* pElement)
{
    if (IS_IFP(pElement))
    {
        DeleteIFP(pElement);
        return true;
    }
    return false;
}

void CElementDeleter::DeleteIFP(CClientEntity* pElement)
{
    CClientIFP&        IFP = static_cast<CClientIFP&>(*pElement);
    const unsigned int u32BlockNameHash = IFP.GetBlockNameHash();

    auto it = std::find_if(m_vecIFPElements.begin(), m_vecIFPElements.end(),
                           [&u32BlockNameHash](std::shared_ptr<CClientIFP> const& pIFP) { return pIFP->GetBlockNameHash() == u32BlockNameHash; });
    if (it != m_vecIFPElements.end())
    {
        m_vecIFPElements.erase(it);
    }
}

void CElementDeleter::DoDeleteAll(void)
{
    // Make sure elements won't call us back and screw with our list (would crash)
    m_bAllowUnreference = false;

    // Delete all the elements
    list<CClientEntity*>::iterator iter = m_List.begin();
    while (iter != m_List.end())
    {
        CClientEntity* pEntity = *iter;

        if (!DeleteElementSpecial(pEntity))
        {
            // Delete the entity and put the next element in the list in the iterator
            delete pEntity;
        }
        iter = m_List.erase(iter);
    }

    // We can now allow unrefernecs again
    m_bAllowUnreference = true;
}

bool CElementDeleter::OnClientSpecialElementDestroy(CClientEntity* pElement)
{
    if (IS_IFP(pElement))
    {
        OnClientIFPElementDestroy(pElement);
        return true;
    }
    return false;
}

void CElementDeleter::OnClientIFPElementDestroy(CClientEntity* pElement)
{
    CClientIFP&        IFP = static_cast<CClientIFP&>(*pElement);
    const unsigned int u32BlockNameHash = IFP.GetBlockNameHash();

    std::shared_ptr<CClientIFP> pIFP = g_pClientGame->GetIFPPointerFromMap(u32BlockNameHash);
    if (pIFP)
    {
        // Remove IFP from map, so we can indicate that it does not exist
        g_pClientGame->RemoveIFPPointerFromMap(u32BlockNameHash);

        // Remove IFP animations from replaced animations of peds/players
        g_pClientGame->OnClientIFPUnload(pIFP);

        // keep a reference to shared_ptr CClientIFP in list, so it does not get
        // destroyed after exiting this function
        m_vecIFPElements.emplace_back(pIFP);
    }
}

bool CElementDeleter::IsBeingDeleted(CClientEntity* pElement)
{
    return m_List.Contains(pElement);
}

void CElementDeleter::Unreference(class CClientEntity* pElement)
{
    // If we allow unreferencing, remove this element from the to delete list.
    if (m_bAllowUnreference)
    {
        m_List.remove(pElement);
    }
}

void CElementDeleter::CleanUpForVM(CLuaMain* pLuaMain)
{
    list<CClientEntity*>::const_iterator iter = m_List.begin();
    for (; iter != m_List.end(); ++iter)
        (*iter)->DeleteEvents(pLuaMain, false);
}
