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


CDummy* CGroups::CreateFromXML ( CElement* pParent, CXMLNode& Node, CEvents* pEvents )
{
    // Create the item and load its custom data and events
    CDummy* pTemp = new CDummy ( this, pParent, &Node );
    pTemp->LoadFromCustomData ( pEvents );

    // Return it
    return pTemp;
}


void CGroups::Delete ( CDummy* pDummy )
{
    delete pDummy;
}


void CGroups::DeleteAll ( void )
{
    DeletePointersAndClearList ( m_List );
}


void CGroups::RemoveFromList ( CDummy* pDummy )
{
    m_List.remove ( pDummy );
}
