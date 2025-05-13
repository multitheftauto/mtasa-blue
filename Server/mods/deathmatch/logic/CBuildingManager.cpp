/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CBuildingManager.cpp
 *  PURPOSE:     Building entity manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CBuildingManager.h"
#include "Utils.h"

constexpr float WORLD_DISTANCE_FROM_CENTER = 3000.0f;

CBuildingManager::CBuildingManager()
{
}

CBuildingManager::~CBuildingManager()
{
    DeleteAll();
}

CBuilding* CBuildingManager::Create(CElement* pParent)
{
    CBuilding* pBuilding = new CBuilding(pParent, this);

    if (pBuilding->GetID() == INVALID_ELEMENT_ID)
    {
        delete pBuilding;
        return nullptr;
    }

    return pBuilding;
}

CBuilding* CBuildingManager::CreateFromXML(CElement* pParent, CXMLNode& Node, CEvents* pEvents)
{
    CBuilding* pBuilding = new CBuilding(pParent, this);

    if (pBuilding->GetID() == INVALID_ELEMENT_ID || !pBuilding->LoadFromCustomData(pEvents, Node))
    {
        delete pBuilding;
        return nullptr;
    }

    return pBuilding;
}

void CBuildingManager::DeleteAll()
{
    // Delete all objects, make sure they dont remove themselves from our list (would make this damn slow)
    DeletePointersAndClearList(m_List);
}

bool CBuildingManager::IsValidModel(unsigned long ulObjectModel)
{
    // We are using the same list
    return CObjectManager::IsValidModel(ulObjectModel);
}

void CBuildingManager::RemoveFromList(CBuilding* pBuilding)
{
    m_List.remove(pBuilding);
}

bool CBuildingManager::IsValidPosition(const CVector& pos) noexcept
{
    return (pos.fX >= -WORLD_DISTANCE_FROM_CENTER && pos.fX <= WORLD_DISTANCE_FROM_CENTER && pos.fY >= -WORLD_DISTANCE_FROM_CENTER &&
            pos.fY <= WORLD_DISTANCE_FROM_CENTER);
}
