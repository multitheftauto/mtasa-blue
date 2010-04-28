/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CElementArray.cpp
*  PURPOSE:     Element array class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*****************************************************************************/

#include <StdInc.h>

using namespace std;

CClientEntity* CElementIDs::m_Elements [MAX_SERVER_ELEMENTS + MAX_CLIENT_ELEMENTS];
CStack < ElementID, MAX_CLIENT_ELEMENTS, INVALID_ELEMENT_ID > CElementIDs::m_ClientStack;

void CElementIDs::Initialize ( void )
{
    memset ( m_Elements, 0, sizeof ( m_Elements ) );
}


CClientEntity* CElementIDs::GetElement ( ElementID ID )
{
    if ( ID < MAX_SERVER_ELEMENTS + MAX_CLIENT_ELEMENTS )
    {
        return m_Elements [ID];
    }

/*
#ifdef MTA_DEBUG
    assert ( 0 );
#endif
*/
    return NULL;
}


void CElementIDs::SetElement ( ElementID ID, CClientEntity* pEntity )
{
    if ( ID < MAX_SERVER_ELEMENTS + MAX_CLIENT_ELEMENTS )
        m_Elements [ID] = pEntity;
#ifdef MTA_DEBUG
    else
        assert ( 0 );
#endif
}


ElementID CElementIDs::PopClientID ( void )
{
    // Pop an unique ID
    ElementID ID = m_ClientStack.Pop ();
    if ( ID != INVALID_ELEMENT_ID )
    {
        // Make it at the beginning after server range ends
        return ID + MAX_SERVER_ELEMENTS;
    }

    // Return it
    return INVALID_ELEMENT_ID;
}


void CElementIDs::PushClientID ( ElementID ID )
{
    // Not invalid?
    if ( ID != INVALID_ELEMENT_ID )
    {
        // It's in the server element ID range, put it down to client
        ID -= MAX_SERVER_ELEMENTS;
        m_ClientStack.Push ( ID );
    }
}
