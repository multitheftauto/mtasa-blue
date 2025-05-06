/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CElementGroup.cpp
 *  PURPOSE:     Static element array management class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CElementIDs.h"

CStack<ElementID, MAX_SERVER_ELEMENTS - 2>  CElementIDs::m_UniqueIDs;
SFixedArray<CElement*, MAX_SERVER_ELEMENTS> CElementIDs::m_Elements;

void CElementIDs::Initialize()
{
    memset(&m_Elements[0], 0, sizeof(m_Elements));
}

ElementID CElementIDs::PopUniqueID(CElement* pElement)
{
    // Grab the ID and check that we had more left
    ElementID ID;
    if (m_UniqueIDs.Pop(ID) && ID != INVALID_ELEMENT_ID)
    {
        assert(ID < MAX_SERVER_ELEMENTS);
        m_Elements[ID.Value()] = pElement;
    }

    return ID;
}

void CElementIDs::PushUniqueID(ElementID ID)
{
    // Push the ID back and NULL the entity there
    if (ID != INVALID_ELEMENT_ID && ID < MAX_SERVER_ELEMENTS)
    {
        m_UniqueIDs.Push(ID);
        m_Elements[ID.Value()] = NULL;
    }
}

void CElementIDs::PushUniqueID(CElement* pElement)
{
    PushUniqueID(pElement->GetID());
}

CElement* CElementIDs::GetElement(ElementID ID)
{
    // Return the element with the given ID
    if (ID != INVALID_ELEMENT_ID && ID < MAX_SERVER_ELEMENTS)
    {
        CElement* pElement = m_Elements[ID.Value()];
        if (pElement && !pElement->IsBeingDeleted())
            return pElement;
    }

    return NULL;
}
