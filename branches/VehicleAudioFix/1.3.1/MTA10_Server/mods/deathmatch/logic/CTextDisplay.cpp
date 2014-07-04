/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CTextDisplay.cpp
*  PURPOSE:     Text display class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Peter <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CTextDisplay::CTextDisplay( void )
{
    m_uiScriptID = CIdArray::PopUniqueId ( this, EIdClass::TEXT_DISPLAY );
}


CTextDisplay::~CTextDisplay ( void )
{
    CIdArray::PushUniqueId ( this, EIdClass::TEXT_DISPLAY, m_uiScriptID );
    // Delete all our text items
    list < CTextItem* > ::iterator iter = m_contents.begin ();
    for ( ; iter != m_contents.end (); iter++ )
    {
        Remove ( *iter, false );
    }

    // Clear our text items
    m_contents.clear ();

    // Unreference us from the player text managers
    list < CPlayerTextManager* > ::iterator iter2 = m_observers.begin ();
    for ( ; iter2 != m_observers.end (); iter2++ )
    {
        (*iter2)->m_displays.remove ( this );
    }
}


void CTextDisplay::AddObserver ( CPlayer* pPlayer)
{
    AddObserver ( pPlayer->GetPlayerTextManager () );
}


void CTextDisplay::RemoveObserver ( CPlayer* pPlayer )
{
    RemoveObserver( pPlayer->GetPlayerTextManager () );
}

bool CTextDisplay::IsObserver ( CPlayer* pPlayer )
{
    list < CPlayerTextManager* > ::iterator iter = m_observers.begin ();
    for ( ; iter != m_observers.end (); iter++ )
    {
        if ( *iter == pPlayer -> GetPlayerTextManager () )
            return true;
    }

    return false;
}

void CTextDisplay::GetObservers ( lua_State* pLua )
{
    assert ( pLua );

    unsigned int uiIndex = 0;
    list < CPlayerTextManager* > ::iterator iter = m_observers.begin ();
    for ( ; iter != m_observers.end (); iter++ )
    {
        lua_pushnumber ( pLua, ++uiIndex );
        lua_pushelement ( pLua, (*iter) -> GetPlayer() );
        lua_settable ( pLua, -3 );
    }
}

void CTextDisplay::AddObserver ( CPlayerTextManager* pTextManager )
{
    // Remove them first if they're already added (easier than checking if they are and only marginally slower)
    m_observers.remove ( pTextManager );
    pTextManager->m_displays.remove ( this );

    // Add them
    m_observers.push_back ( pTextManager );
    pTextManager->m_displays.push_back ( this );

    // Inform the new observer of all the items in this display
    list < CTextItem* > ::iterator iter = m_contents.begin ();
    for ( ; iter != m_contents.end (); iter++ )
    {
        pTextManager->Update( *iter );
    }
}


void CTextDisplay::RemoveObserver ( CPlayerTextManager* pTextManager )
{
    // Remove the text manager from our list and us from the text manager's list
    m_observers.remove ( pTextManager );
    pTextManager->m_displays.remove ( this );

    // Inform the observer that all the items in this display have been deleted
    list < CTextItem* > ::iterator iter = m_contents.begin ();
    for ( ; iter != m_contents.end (); iter++ )
    {
        pTextManager->Update( *iter, true );
    }
}


// called when a CServerTextItem that this contains is changed
void CTextDisplay::Update ( CTextItem* pTextItem, bool bRemovedFromDisplay )
{
    // Remove it from our contents if it's getting removed from display
    if ( pTextItem->IsBeingDeleted () )
    {
        m_contents.remove ( pTextItem );
    }

    // Tell the player text managers to update
    list < CPlayerTextManager* > ::iterator iter = m_observers.begin ();
    for ( ; iter != m_observers.end (); iter++ )
    {
        (*iter)->Update ( pTextItem, bRemovedFromDisplay );
    }
}


void CTextDisplay::Add ( CTextItem* pTextItem )
{
    // Push the new item onto the content list
    m_contents.remove ( pTextItem );
    m_contents.push_back ( pTextItem );

    // Tell the text display to add us as a text display
    pTextItem->AddObserver ( this );

    // Inform the players
    Update ( pTextItem );
}


void CTextDisplay::Remove ( CTextItem* pTextItem, bool bRemoveFromList )
{
    // Remove it from our list and us from the text item's
    if ( bRemoveFromList )
    {
        m_contents.remove ( pTextItem );
    }

    pTextItem->RemoveObserver ( this );

    // Inform the players
    Update ( pTextItem, true );
}
