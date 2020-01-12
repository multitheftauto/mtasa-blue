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
    for (auto& iter : m_List)
    {
        if (iter->GetID() == ulID)
            return iter.get();
    }

    return nullptr;
}

CClientPrimitiveBuffer* CClientPrimitiveBufferManager::Create()
{
    std::unique_ptr<CClientPrimitiveBuffer> pPrimitiveBuffer = std::make_unique<CClientPrimitiveBuffer>(m_pManager, INVALID_ELEMENT_ID);
    CClientPrimitiveBuffer*                 ptr = pPrimitiveBuffer.get();
    AddToList(std::move(pPrimitiveBuffer));
    return ptr;
}

void CClientPrimitiveBufferManager::Delete(CClientPrimitiveBuffer* pPrimitiveBuffer)
{
    RemoveFromList(pPrimitiveBuffer);
}

void CClientPrimitiveBufferManager::DeleteAll()
{
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

    return nullptr;
}

void CClientPrimitiveBufferManager::RemoveFromList(CClientPrimitiveBuffer* pPrimitiveBuffer)
{
    for (auto& iter : m_List)
    {
        if (iter.get() == pPrimitiveBuffer)
        {
            m_List.remove(iter);
            return;
        }
    }
}
