/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPlayerTextManager.cpp
*  PURPOSE:     Player text display manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPlayerTextManager::CPlayerTextManager ( CPlayer * player )
{
    m_pPlayer = player;
}


CPlayerTextManager::~CPlayerTextManager ( void )
{
    // Remove us from all text display's lists
    // Note that RemoveObserver removes itself from the list so we just do this till there are 0 items left
    while ( m_displays.size () > 0 )
    {
        m_displays.front ()->RemoveObserver ( this );
    }

    // Delete the cloned queued items or we'll leak
    list < CTextItem* > ::iterator iter2 = m_highQueue.begin ();
    for ( ; iter2 != m_highQueue.end (); iter2++ )
    {
        delete *iter2;
    }

    for ( iter2 = m_mediumQueue.begin (); iter2 != m_mediumQueue.end (); iter2++ )
    {
        delete *iter2;
    }

    for ( iter2 = m_lowQueue.begin (); iter2 != m_lowQueue.end (); iter2++ )
    {
        delete *iter2;
    }
}


// bRemoved = true if the item is being removed
void CPlayerTextManager::Update ( CTextItem* pTextItem, bool bRemovedFromDisplay )
{
    // Check that the item isn't already on our list or we end up updating twice for no reason
    CTextItem* pQueueItem = GetTextItemOnQueue ( pTextItem );
    if ( pQueueItem )
    {
        // Update the item on the queue with the new one passed
        *pQueueItem = *pTextItem;

        // Make the queueitem deletable if we're supposed to remove it from display. So that it won't
        // continue to appear on the player screen if you add and remove before a Process is called.
        if ( bRemovedFromDisplay )
            pQueueItem->m_bDeletable = true;

        return;
    }

    // Clone the passed text item
    eTextPriority updatePriority = pTextItem->GetPriority ();
    CTextItem* pClonedItem = new CTextItem ( *pTextItem );
    if ( bRemovedFromDisplay )
        pClonedItem->m_bDeletable = true;   // we pretend that the text item has actually been deleted
                                            // as far as the player is concerned, it has been

    // Push it on the correct queue 
    switch (updatePriority)
    {
        case PRIORITY_LOW:
            m_lowQueue.push_back(pClonedItem);
            break;
        case PRIORITY_MEDIUM:
            m_mediumQueue.push_back(pClonedItem);
            break;
        case PRIORITY_HIGH:
            m_highQueue.push_back(pClonedItem);
            break;
        default:
            delete pClonedItem;
            return;
    }
}


// checks if this item is already on the queue and if so returns a pointer to the item
// this is not the same as the parameter as the item on the queue is a CLONE of the original
CTextItem* CPlayerTextManager::GetTextItemOnQueue ( CTextItem* textItem )
{
    list < CTextItem* > ::iterator iter = m_highQueue.begin ();
    for ( ; iter != m_highQueue.end (); iter++ )
    {
        if ( (*iter)->GetID() == textItem->GetID() )
        {
            return (*iter);
        }
    }

    for ( iter = m_mediumQueue.begin (); iter != m_mediumQueue.end (); iter++ )
    {
        if ( (*iter)->GetID() == textItem->GetID() )
        {
            return (*iter);
        }
    }

    for ( iter = m_lowQueue.begin (); iter != m_lowQueue.end (); iter++ )
    {
        if ( (*iter)->GetID() == textItem->GetID() )
        {
            return (*iter);
        }
    }

    // not found it
    return NULL;
}

CPlayer* CPlayerTextManager::GetPlayer ()
{
    return m_pPlayer;
}

void CPlayerTextManager::Process ()
{
    // Pop some item off the queue
    CTextItem* textItem = NULL;
    if ( m_highQueue.size() != 0 )
    {
        textItem = m_highQueue.front();
        m_highQueue.pop_front();
    }
    else if ( m_mediumQueue.size() != 0 )
    {
        textItem = m_mediumQueue.front();
        m_mediumQueue.pop_front();
    }
    else if ( m_lowQueue.size() != 0 )
    {
        textItem = m_lowQueue.front();
        m_lowQueue.pop_front();
    }

    if ( textItem )
    {
        // Tell the player
        m_pPlayer->Send ( CServerTextItemPacket( textItem->m_ulUniqueId, textItem->m_bDeletable, textItem->m_vecPosition.fX, 
                                                 textItem->m_vecPosition.fY, textItem->m_fScale, 
                                                 textItem->m_Color, textItem->m_ucFormat, textItem->m_ucShadowAlpha, textItem->m_strText ) );

        // Delete the text item created on the queue
        delete textItem;
    }
}
