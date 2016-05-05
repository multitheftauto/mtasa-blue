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


CBlip* CBlipManager::CreateFromXML ( CElement* pParent, CXMLNode& Node, CEvents* pEvents )
{
    // Create the blip
    CBlip* pBlip = new CBlip ( pParent, &Node, this );

    // Is the id valid?
    if ( pBlip->GetID () == INVALID_ELEMENT_ID ||
         !pBlip->LoadFromCustomData ( pEvents ) )
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
    DeletePointersAndClearList ( m_List );
}


bool CBlipManager::Exists ( CBlip* pBlip )
{
    return ListContains ( m_List, pBlip );
}
