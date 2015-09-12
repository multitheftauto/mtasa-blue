/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CElementDeleter.cpp
*  PURPOSE:     Element lazy deletion manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame * g_pGame;

void CElementDeleter::Delete ( class CElement* pElement, bool bUnlink, bool bUpdatePerPlayerEntities )
{
    if ( pElement )
    {
        if ( !IsBeingDeleted ( pElement ) )
        {
            // Flag it as being deleted
            pElement->SetIsBeingDeleted ( true );

            // We have to call RemoveAllPlayers here so it's not too late for it
            // to trigger onPlayerTeamChange with a valid element as previous team
            if ( pElement->GetType ( ) == CElement::TEAM )
            {
                ( ( CTeam* ) pElement )->RemoveAllPlayers ( );
            }

            // Before we do anything, fire the on-destroy event
            CLuaArguments Arguments;
            pElement->CallEvent ( "onElementDestroy", Arguments );

            // Add it to our list
            if ( !IsBeingDeleted ( pElement ) )
            {
                m_List.push_back ( pElement );
            }

            // unlink it from the tree/managers
            pElement->ClearChildren ();
            pElement->SetParentObject ( NULL, bUpdatePerPlayerEntities );

            if ( bUnlink )
                pElement->Unlink ();
        }
    }
}


void CElementDeleter::DoDeleteAll ( void )
{
    // This depends on CElementDeleter::Unreference() being called in ~CElement() 
    while ( !m_List.empty () )
        delete *m_List.begin ();
}

void CElementDeleter::Unreference ( CElement* pElement )
{
    m_List.remove ( pElement );
}

bool CElementDeleter::IsBeingDeleted ( CElement* pElement )
{
    return ListContains ( m_List, pElement );
}

void CElementDeleter::CleanUpForVM ( CLuaMain* pLuaMain )
{
    CElementListType::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end () ; iter++ )
        (*iter)->DeleteEvents ( pLuaMain, false );
}
