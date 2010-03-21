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

CStack < unsigned long, MAX_XML, INVALID_XML_ID > CXMLArray::m_IDStack;
CXMLCommon* CXMLArray::m_Elements [MAX_XML];

void CXMLArray::Initialize ( void )
{
    // Initialize entry array
    memset ( m_Elements, 0, sizeof ( m_Elements ) );
}


unsigned long CXMLArray::PopUniqueID ( CXMLCommon* pEntry )
{
    // Grab the ID and check that we had more left
    unsigned long ulID = m_IDStack.Pop ();
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
        m_IDStack.Push ( ulID );
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

