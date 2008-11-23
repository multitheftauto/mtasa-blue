/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CObjectManager.cpp
*  PURPOSE:     Object entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Alberto Alonso <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

static unsigned int g_uiValidObjectModels[] = {
    2, 0, 0, 0, 0, 0, 0, 0, 0, -4096, -1053185, 4194303, 16383, 0, 0, 0, 0, 0, 0, -128,
    -515899393, -134217729, -1, -1, 33554431, -1, -1, -1, -14337, -1, -33, -129, -1, -1, -1, -1, -1, -8387585, -1, -1,
    -1, -16385, -1, -1, -1, -1, -1, -1, -1, -33, -1, -771751937, -1, -9, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -25, -1, -1, -1, -1, -1, -1, -65537, -2147483648, 34079999,
    2113536, -4825600, -5, -1, -3145729, -1, -16777217, -35, -1, -1, -1, -1, -201326593, -1, -1, -1, -1, -1, -257, -1,
    1073741823, -133122, -1, -1, -65, -1, -1, -1, -1, -1, -1, -2146435073, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 1073741823, -64, -1, -1, -1, -1, -2635777, 134086663, 0, -64, -1, -1, -1, -1, -1, -1, -1, -536870927, -131069,
    -1, -1, -1, -1, -1, -1, -1, -1, -16384, -1, -33554433, -1, -1, -1, -1, -1, -1610612737, 524285, -128, -1,
    2080309247, -1, -1, -1114113, -1, -1, -1, 66977343, -524288, -1, -1, -1, -1, -2031617, -1, 114687, -256, -1, -4097, -1,
    -4097, -1, -1, 1010827263, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32768, -1, -1, -1, -1, -1,
    2147483647, -33554434, -1, -1, -49153, -1148191169, 2147483647, -100781080, -262145, -57, -134217729, -8388607, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1048577, -1, -449, -1017, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1835009, -2049, -1, -1, -1, -1, -1, -1, -8193, -1, -536870913, -1, -1, -1, -1, -1, -87041, -1, -1,
    -1, -1, -1, -1, -209860, -1023, -8388609, -2096385, -1, -1048577, -1, -1, -1, -1, -1, -1, -897, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1610612737, -1025, -28673, -1, -1, -1, -1537, -1, -1, -13, -1, -1,
    -1, -1, -1985, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1056964609, -1, -1, -1, -1,
    -1, -1, -1, -2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -236716037, -1, -1, -1,
    -1, -1, -1, -1, -536870913, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -2097153, -2109441, -1, 201326591, -4194304, -1, -1, -241, -1, -1, -1, -1, -1, -1, 7, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, -32768, -1, -1, -1, -2, -671096835, -1, -8388609, -66061441, -13, -1793,
    -32257, -247809, -1, -1, -513, 16252911, 0, 0, 0, -131072, 33554383, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, 8388607, 0, 0, 0, 0, 0, 0, -256, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -268435449, -1, -1, -2049, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 92274627, -65536, -2097153, -268435457, 591191935, 1, 0, -16777216, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
};


CObjectManager::CObjectManager ( void )
{
    // Init
    m_bRemoveFromList = true;
}


CObjectManager::~CObjectManager ( void )
{
    // Delete all our objects
    DeleteAll ();
}


CObject* CObjectManager::Create ( CElement* pParent, CXMLNode* pNode )
{
    // Create the object
    CObject* pObject = new CObject ( pParent, pNode, this );

    // Valid object id?
    if ( pObject->GetID () == INVALID_ELEMENT_ID )
    {
        delete pObject;
        return NULL;
    }

    // Return the object
    return pObject;
}


CObject* CObjectManager::CreateFromXML ( CElement* pParent, CXMLNode& Node, CLuaMain* pLuaMain, CEvents* pEvents )
{
    // Create the object
    CObject* pObject = new CObject ( pParent, &Node, this );

    // Valid object id and load it from xml
    if ( pObject->GetID () == INVALID_ELEMENT_ID ||
         !pObject->LoadFromCustomData ( pLuaMain, pEvents ) )
    {
        delete pObject;
        return NULL;
    }

    // Return the object
    return pObject;
}


void CObjectManager::DeleteAll ( void )
{
    // Delete all objects, make sure they dont remove themselves from our list (would make this damn slow)
    m_bRemoveFromList = false;
    list < CObject* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        delete *iter;
    }
    m_bRemoveFromList = true;

    // Clear the list
    m_List.clear ();
}


bool CObjectManager::Exists ( CObject* pObject )
{
    // Find the object in the list
    list < CObject* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        if ( *iter == pObject )
        {
            return true;
        }
    }

    // Doesn't exist
    return false;
}


bool CObjectManager::IsValidModel ( unsigned long ulObjectModel )
{
    bool bIsValid = false;

    if ( ulObjectModel < 20001 )
    {
        unsigned int uiChunk = ulObjectModel / ( sizeof(unsigned int)*8 );
        unsigned int shift = ulObjectModel - ( uiChunk * sizeof(unsigned int)*8 );
        unsigned int bit = 1 << shift;
        bIsValid = !!( g_uiValidObjectModels[uiChunk] & bit );
    }

    return bIsValid;
}


void CObjectManager::RemoveFromList ( CObject* pObject )
{
    if ( m_bRemoveFromList && !m_List.empty() )
    {
        m_List.remove ( pObject );
    }
}
