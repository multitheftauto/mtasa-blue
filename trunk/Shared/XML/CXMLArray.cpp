/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        xml/CXMLArrayImpl.cpp
*  PURPOSE:     XML array class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using namespace std;

unsigned long CXMLArray::m_ulStackPosition;
unsigned long CXMLArray::m_ulIDStack [MAX_XML];
CXMLCommon* CXMLArray::m_Elements [MAX_XML];

void CXMLArray::Initialize ( void )
{
    // Initialize entry array
    memset ( m_Elements, 0, sizeof ( m_Elements ) );

    // Initialize the stack with unique ids
    m_ulStackPosition = MAX_XML;
    for ( unsigned long i = 0; i < MAX_XML; i++ )
    {
        m_ulIDStack [i] = i;
    }
}


unsigned long CXMLArray::PopUniqueID ( CXMLCommon* pEntry )
{
    // Grab the ID and check that we had more left
    unsigned long ulID = PopStack ();
    if ( ulID != INVALID_XML_ID &&
         ulID < MAX_XML )
    {
        m_Elements [ulID] = pEntry;
        return MAX_SERVER_ELEMENTS + MAX_CLIENT_ELEMENTS + ulID;
    }
    
     return INVALID_XML_ID;
}


void CXMLArray::PushUniqueID ( unsigned long ulID )
{
    // Push the ID back and NULL the entity there
    if ( ulID != INVALID_XML_ID &&
         ulID >= MAX_SERVER_ELEMENTS + MAX_CLIENT_ELEMENTS &&
         ulID < MAX_SERVER_ELEMENTS + MAX_CLIENT_ELEMENTS + MAX_XML )
    {
        ulID -= MAX_SERVER_ELEMENTS + MAX_CLIENT_ELEMENTS;
        PushStack ( ulID );
        m_Elements [ulID] = NULL;
    }
}


void CXMLArray::PushUniqueID ( CXMLCommon* pEntry )
{
    PushUniqueID ( pEntry->GetID () );
}


CXMLCommon* CXMLArray::GetEntry ( unsigned long ulID )
{
    // Return the element with the given ID
    if ( ulID != INVALID_XML_ID &&
         ulID >= MAX_SERVER_ELEMENTS + MAX_CLIENT_ELEMENTS &&
         ulID < MAX_SERVER_ELEMENTS + MAX_CLIENT_ELEMENTS + MAX_XML )
        return m_Elements [ulID - (MAX_SERVER_ELEMENTS + MAX_CLIENT_ELEMENTS)];

    return NULL;
}


unsigned long CXMLArray::PopStack ( void )
{
    // Got any items? Pop off and return the first item
    if ( m_ulStackPosition > 0 )
    {
        --m_ulStackPosition;
        return m_ulIDStack [m_ulStackPosition];
    }

    // No IDs left
    return INVALID_XML_ID;
}


void CXMLArray::PushStack ( unsigned long ulID )
{
    if ( m_ulStackPosition < MAX_XML )
    {
        m_ulIDStack [m_ulStackPosition] = ulID;
        ++m_ulStackPosition;
    }
}
