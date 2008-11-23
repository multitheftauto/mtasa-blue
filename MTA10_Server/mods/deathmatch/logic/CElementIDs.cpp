/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CElementGroup.cpp
*  PURPOSE:     Static element array management class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using namespace std;

CStack < ElementID > CElementIDs::m_UniqueIDs;
CElement* CElementIDs::m_Elements [MAX_SERVER_ELEMENTS];

void CElementIDs::Initialize ( void )
{
    memset ( m_Elements, 0, sizeof ( m_Elements ) );
}


ElementID CElementIDs::PopUniqueID ( CElement* pElement )
{
    // Grab the ID and check that we had more left
    ElementID ID = m_UniqueIDs.Pop ();
    if ( ID != INVALID_ELEMENT_ID )
    {
        m_Elements [ID] = pElement;
    }

    return ID;
}


void CElementIDs::PushUniqueID ( ElementID ID )
{
    // Push the ID back and NULL the entity there
    if ( ID != INVALID_ELEMENT_ID )
    {
        m_UniqueIDs.Push ( ID );
        m_Elements [ID] = NULL;
    }
}


void CElementIDs::PushUniqueID ( CElement* pElement )
{
    PushUniqueID ( pElement->GetID () );
}


CElement* CElementIDs::GetElement ( ElementID ID )
{
    // Return the element with the given ID
    if ( ID != INVALID_ELEMENT_ID && ID < MAX_SERVER_ELEMENTS )
        return m_Elements [ID];

    return NULL;
}
