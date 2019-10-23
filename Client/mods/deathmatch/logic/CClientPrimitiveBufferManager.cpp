/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPrimitiveBufferManager.cpp
 *  PURPOSE:     Primitive buffer manager class
 *
 *****************************************************************************/

#include "StdInc.h"

using std::list;

CClientPrimitiveBufferManager::CClientPrimitiveBufferManager(CClientManager* pManager)
{
    m_pManager = pManager;
}

CClientPrimitiveBufferManager::~CClientPrimitiveBufferManager()
{
    RemoveAll();
}

CClientPrimitiveBuffer* CClientPrimitiveBufferManager::Get(unsigned long ulID)
{
    // Find the PrimitiveBuffer with the given id
    list<CClientPrimitiveBuffer*>::const_iterator iter = m_List.begin();
    for (; iter != m_List.end(); iter++)
    {
        if ((*iter)->GetID() == ulID)
        {
            return *iter;
        }
    }

    return NULL;
}

void CClientPrimitiveBufferManager::RemoveFromList(CClientPrimitiveBuffer* pPrimitiveBuffer)
{
    if (m_bCanRemoveFromList)
    {
        if (!m_List.empty())
        {
            m_List.remove(pPrimitiveBuffer);
        }
    }
}

void CClientPrimitiveBufferManager::AddToList(CClientPrimitiveBuffer* pPrimitiveBuffer)
{
    m_List.push_back(pPrimitiveBuffer);
}

void CClientPrimitiveBufferManager::RemoveAll()
{
    // Delete all the items in the list
    m_bCanRemoveFromList = false;
    list<CClientPrimitiveBuffer*>::iterator iter = m_List.begin();
    for (; iter != m_List.end(); iter++)
    {
        delete *iter;
    }

    // Clear the list
    m_List.clear();
    m_bCanRemoveFromList = true;
}
