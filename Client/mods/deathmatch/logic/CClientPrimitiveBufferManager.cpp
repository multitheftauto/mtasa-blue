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
    DeleteAll();
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

CClientPrimitiveBuffer* CClientPrimitiveBufferManager::Create()
{
    CClientPrimitiveBuffer* pPrimitiveBuffer = new CClientPrimitiveBuffer(m_pManager, INVALID_ELEMENT_ID);
    AddToList(pPrimitiveBuffer);
    return pPrimitiveBuffer;
}

void CClientPrimitiveBufferManager::Delete(CClientPrimitiveBuffer* pPrimitiveBuffer)
{
    RemoveFromList(pPrimitiveBuffer);
    delete pPrimitiveBuffer;
}

void CClientPrimitiveBufferManager::DeleteAll()
{
    list<CClientPrimitiveBuffer*>::const_iterator iter = m_List.begin();
    for (; iter != m_List.end(); iter++)
    {
        delete *iter;
    }

    // Clear the list
    m_List.clear();
}

CClientPrimitiveBuffer* CClientPrimitiveBufferManager::Get(ElementID ID)
{
    // Grab the element with the given id. Check its type.
    CClientEntity* pEntity = CElementIDs::GetElement(ID);
    if (pEntity && pEntity->GetType() == CCLIENTPRIMITIVEBUFFER)
    {
        return static_cast<CClientPrimitiveBuffer*>(pEntity);
    }

    return NULL;
}

void CClientPrimitiveBufferManager::RemoveFromList(CClientPrimitiveBuffer* pPrimitiveBuffer)
{
    if (!m_List.empty())
        m_List.remove(pPrimitiveBuffer);
}
