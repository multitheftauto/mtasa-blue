/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CBlipManager.cpp
*  PURPOSE:     Blip entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CBlipManager::CBlipManager ( void )
{
    // Init
    m_bRemoveFromList = true;
}


CBlip* CBlipManager::Create ( CElement* pParent, CXMLNode* pNode )
{
    // Create the blip
    CBlip* pBlip = new CBlip ( pParent, pNode, this );

    // Is the id valid?
    if ( pBlip->GetID () == INVALID_ELEMENT_ID )
    {
        delete pBlip;
        return NULL;
    }

    // Return the created blip
    return pBlip;
}


CBlip* CBlipManager::CreateFromXML ( CElement* pParent, CXMLNode& Node, CLuaMain* pLuaMain, CEvents* pEvents )
{
    // Create the blip
    CBlip* pBlip = new CBlip ( pParent, &Node, this );

    // Is the id valid?
    if ( pBlip->GetID () == INVALID_ELEMENT_ID ||
         !pBlip->LoadFromCustomData ( pLuaMain, pEvents ) )
    {
        delete pBlip;
        return NULL;
    }

    // Return the created blip
    return pBlip;
}


void CBlipManager::DeleteAll ( void )
{
    // Delete all our blips
    m_bRemoveFromList = false;
    list < CBlip* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        delete *iter;
    }
    m_bRemoveFromList = true;

    // Clear the list
    m_List.clear ();
}


bool CBlipManager::Exists ( CBlip* pBlip )
{
    // Search the list for the blip
    list < CBlip* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        // Ids match? Return true
        if ( *iter == pBlip )
        {
            return true;
        }
    }

    // Doesn't exist
    return false;
}
