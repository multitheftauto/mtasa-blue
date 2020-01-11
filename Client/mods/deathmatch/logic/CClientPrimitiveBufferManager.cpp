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

std::shared_ptr<CClientPrimitiveBuffer> CClientPrimitiveBufferManager::Get(unsigned long ulID)
{
    // Find the PrimitiveBuffer with the given id
    for (auto& iter : m_List)
    {
        if (iter->GetID() == ulID)
            return std::move(iter);
    }

    return nullptr;
}

std::shared_ptr<CClientPrimitiveBuffer> CClientPrimitiveBufferManager::Create()
{
    std::shared_ptr<CClientPrimitiveBuffer> pPrimitiveBuffer = std::make_unique<CClientPrimitiveBuffer>(m_pManager, INVALID_ELEMENT_ID);
    AddToList(pPrimitiveBuffer);
    return std::move(pPrimitiveBuffer);
}

void CClientPrimitiveBufferManager::Delete(std::shared_ptr<CClientPrimitiveBuffer> pPrimitiveBuffer)
{
    RemoveFromList(std::move(pPrimitiveBuffer));
}

void CClientPrimitiveBufferManager::DeleteAll()
{
    // Clear the list
    m_List.clear();
}

std::shared_ptr<CClientPrimitiveBuffer> CClientPrimitiveBufferManager::Get(ElementID ID)
{
    // Grab the element with the given id. Check its type.
    CClientEntity* pEntity = CElementIDs::GetElement(ID);
    if (pEntity && pEntity->GetType() == CCLIENTPRIMITIVEBUFFER)
    {
        return std::shared_ptr<CClientPrimitiveBuffer>(static_cast<CClientPrimitiveBuffer*>(pEntity));
    }

    return nullptr;
}

void CClientPrimitiveBufferManager::RemoveFromList(std::shared_ptr<CClientPrimitiveBuffer> pPrimitiveBuffer)
{
    if (!m_List.empty())
        m_List.remove(std::move(pPrimitiveBuffer));
}
