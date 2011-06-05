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

CStack <ElementID, MAX_SERVER_ELEMENTS - MAX_PLAYER_COUNT, MAX_PLAYER_COUNT > CElementIDs::m_UniqueIDs;
CElement* CElementIDs::m_Elements [MAX_SERVER_ELEMENTS];

void CElementIDs::Initialize ( void )
{
    memset ( m_Elements, 0, sizeof ( m_Elements ) );
}


ElementID CElementIDs::PopUniqueID ( CElement* pElement )
{
    // Grab the ID and check that we had more left
    ElementID ID;
    if ( m_UniqueIDs.Pop (ID) && ID != INVALID_ELEMENT_ID )
    {
        m_Elements [ID.Value()] = pElement;
    }

    return ID;
}

ElementID CElementIDs::PopPlayerID ( CPlayer* pPlayer )
{
    ElementID ID;
    if ( m_UniqueIDs.PopReserved (ID) && ID != INVALID_ELEMENT_ID )
    {
        m_Elements [ID.Value()] = pPlayer;
    }

    return ID;
}


void CElementIDs::PushUniqueID ( ElementID ID )
{
    // Push the ID back and NULL the entity there
    if ( ID != INVALID_ELEMENT_ID )
    {
        m_UniqueIDs.Push ( ID );
        m_Elements [ID.Value()] = NULL;
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
        return m_Elements [ID.Value()];

    return NULL;
}
