/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPedManager.cpp
 *  PURPOSE:     Ped entity manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPedManager.h"
#include "CPed.h"
#include "Utils.h"

CPedManager::CPedManager()
{
}

CPedManager::~CPedManager()
{
    DeleteAll();
}

CPed* CPedManager::Create(unsigned short usModel, CElement* pParent)
{
    CPed* const pPed = new CPed(this, pParent, usModel);

    if (pPed->GetID() == INVALID_ELEMENT_ID)
    {
        delete pPed;
        return nullptr;
    }

    return pPed;
}

CPed* CPedManager::CreateFromXML(CElement* pParent, CXMLNode& Node, CEvents* pEvents)
{
    CPed* const pPed = new CPed(this, pParent, 400);

    if (pPed->GetID() == INVALID_ELEMENT_ID || !pPed->LoadFromCustomData(pEvents, Node))
    {
        delete pPed;
        return nullptr;
    }

    pPed->SetSpawned(true);

    if (pPed->GetHealth() > 0)
    {
        pPed->SetIsDead(false);
    }

    return pPed;
}

void CPedManager::DeleteAll()
{
    // Delete all items
    DeletePointersAndClearList(m_List);
}

void CPedManager::RemoveFromList(CPed* pPed)
{
    m_List.remove(pPed);
}

bool CPedManager::Exists(CPed* pPed)
{
    return ListContains(m_List, pPed);
}

bool CPedManager::IsValidModel(unsigned short usModel)
{
    return true;
}
