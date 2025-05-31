/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CElementGroup.cpp
 *  PURPOSE:     Element group class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

// This class stores a number of elements in a flat list. This is
// purely to allow actions to be done on the list as a whole,
// mainly the deletion of every element, but possibly also the
// hiding and showing of them on demand. Each element can belong in
// only one CElementGroup at a time (though this will probably not
// be enforced at this level).

#include "StdInc.h"
#include "CElementGroup.h"
#include "packets/CEntityRemovePacket.h"
#include "CGame.h"
#include "CMapManager.h"
#include "CPerPlayerEntity.h"

extern CGame* g_pGame;

CElementGroup::~CElementGroup()
{
    // Delete all the elements
    if (!m_elements.empty())
    {
        if (!g_pGame->IsBeingDeleted())
        {
            CEntityRemovePacket removePacket;

            for (CFastList<CElement*>::iterator iter = m_elements.begin(); iter != m_elements.end(); iter++)
            {
                CElement* pElement = (*iter);

                switch (pElement->GetType())
                {
                    case CElement::DATABASE_CONNECTION:
                    case CElement::SCRIPTFILE:
                        continue;
                    default:
                        break;
                }
                // Tell everyone to destroy it if this is not a per-player entity
                if (pElement->IsPerPlayerEntity())
                {
                    // Unsync it (will destroy it for those that know about it)
                    CPerPlayerEntity* pEntity = static_cast<CPerPlayerEntity*>(pElement);
                    pEntity->Sync(false);
                }
                else
                {
                    // Tell everyone to destroy it
                    removePacket.Add(pElement);
                }
            }

            g_pGame->GetPlayerManager()->BroadcastOnlyJoined(removePacket);
        }

        CElementDeleter* deleter = g_pGame->GetElementDeleter();
        CElement*        pElement = NULL;
        for (CFastList<CElement*>::iterator iter = m_elements.begin(); iter != m_elements.end(); iter++)
        {
            pElement = *iter;
            pElement->SetElementGroup(NULL);
            pElement->DeleteAllEvents();
            deleter->Delete(pElement, true, false);
        }

        // Do this at the end for speed
        g_pGame->GetMapManager()->GetRootElement()->UpdatePerPlayerEntities();
    }
}

void CElementGroup::Add(class CElement* element)
{
    m_elements.push_back(element);
    element->SetElementGroup(this);
}

void CElementGroup::Remove(class CElement* element)
{
    m_elements.remove(element);
}

unsigned int CElementGroup::GetCount()
{
    return m_elements.size();
}
