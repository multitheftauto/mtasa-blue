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
    -515899393, -134217729, -1, -1, 33554431, -1, -1, -1, -14337, -1, -1, -129, -1, 1073741823, -1, -1, -1, -8387585, -1, -1, 
    -1, -1, -1, -513, -1, -1, -1, -1, -1, -1, -1, -1, -1, -9, -1, -1, -1, -1, -1, -1, 
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -25, -1, -1, -1, -1, -1, -1, -1, -1, 34080767, 
    2113536, -4825600, -5, -1, -3145729, -1, -16777217, -33, -1, -1, -1, -1, -201326593, -1, -1, -1, -1, -1, -1, -1, 
    1073741823, -133121, -1, -1, -65, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
    -1, 1073741823, -64, -1, -1, -1, -1, -1, 134217727, 0, -64, -1, -1, -1, -1, -1, -1, -1, -536870913, -131069, 
    -1, -1, -1, -1, -1, -1, -1, -1, -16384, -1, -1, -1, -1, -1, -1, -1, -1, 524287, -128, -1, 
    -1, -1, -1, -1, -1, -1, -1, 134217727, -524288, -1, -1, -1, -1, -1, -1, 245759, -256, -1, -4097, -1, 
    -1, -1, -1, 1073741823, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32768, -1, -1, -1, -1, -1, 
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -8388607, -1, -1, -1, -1, -1, -1, -1, -1, 
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -961, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
    -1, -1, -1, -1, -1, -1, -1, -2096129, -1, -1, -1, -1, -1, -1, -1, -1, -897, -1, -1, -1, 
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
    -1, -1, -1921, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1040187393, -1, -1, -1, -1, 
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -201326593, -1, -1, -1, 
    -1, -1, -1, -1, -1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
    -1, -1, -1, 268435455, -4194304, -1, -1, -241, -1, -1, -1, -1, -1, -1, 7, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, -32768, -1, -1, -1, -1, -671088643, -1, -1, -66060289, -13, -1793, 
    -32257, -245761, -1, -1, -513, 16252911, 0, 0, 0, -131072, 33554431, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
    -1, -1, -1, -1, 8388607, 0, 0, 0, 0, 0, 0, -256, -1, -1, -1, -1, -1, -1, -1, -1, 
    -1, -1, -1, -1, -1, -1, -268435449, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
    -1, 92274687, -65536, -2097153, -1, 595591167, 1, 0, -16777216, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
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
