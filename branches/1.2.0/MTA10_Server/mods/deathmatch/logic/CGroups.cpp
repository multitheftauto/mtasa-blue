/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CGroups.cpp
*  PURPOSE:     Entity (dummy) group manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CDummy* CGroups::Create ( CElement* pParent )
{
    // Create and return it
    CDummy* pTemp = new CDummy ( this, pParent );
    return pTemp;
}


CDummy* CGroups::CreateFromXML ( CElement* pParent, CXMLNode& Node, CLuaMain* pLuaMain, CEvents* pEvents )
{
    // Create the item and load its custom data and events
    CDummy* pTemp = new CDummy ( this, pParent, &Node );
    pTemp->LoadFromCustomData ( pLuaMain, pEvents );

    // Return it
    return pTemp;
}


void CGroups::Delete ( CDummy* pDummy )
{
    delete pDummy;
}


void CGroups::DeleteAll ( void )
{
    // Delete all the elements
    m_bDontRemoveFromList = true;
    list < CDummy* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the list
    m_List.clear ();
    m_bDontRemoveFromList = false;
}


void CGroups::RemoveFromList ( CDummy* pDummy )
{
    if ( !m_bDontRemoveFromList )
    {
        if ( !m_List.empty() ) m_List.remove ( pDummy );
    }
}
