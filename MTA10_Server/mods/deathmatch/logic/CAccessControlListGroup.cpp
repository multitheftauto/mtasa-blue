/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CAccessControlListGroup.cpp
*  PURPOSE:     Access control list group definition class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*               aru <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CAccessControlListGroup::CAccessControlListGroup ( const char* szGroupName )
{
    _snprintf ( m_szGroupName, 256, "%s", szGroupName );
    m_szGroupName[255] = '\0';
}


CAccessControlListGroup::~CAccessControlListGroup ( void )
{
    ObjectList::iterator iter = m_Objects.begin ();
    for ( ; iter != m_Objects.end (); iter++ )
    {
        delete *iter;
    }

    m_Objects.clear ();
    m_ObjectsById.clear ();
}


CAccessControlListGroupObject* CAccessControlListGroup::AddObject ( const char* szObjectName, CAccessControlListGroupObject::EObjectType eObjectType )
{
    unsigned int uiHash = CAccessControlListGroupObject::GenerateHashId ( szObjectName, eObjectType );
    ObjectMap::const_iterator iter = m_ObjectsById.find ( uiHash );

    if ( iter != m_ObjectsById.end() )
    {
        return iter->second;
    }

    CAccessControlListGroupObject* pObject = new CAccessControlListGroupObject ( szObjectName, eObjectType );
    m_Objects.push_back ( pObject );
    m_ObjectsById [ pObject->GetObjectHashId() ] = pObject;

    return pObject;
}


bool CAccessControlListGroup::FindObjectMatch ( const char* szObjectName, CAccessControlListGroupObject::EObjectType eObjectType )
{
    unsigned int uiHash = CAccessControlListGroupObject::GenerateHashId ( szObjectName, eObjectType );

    // Look through the list for a matching name. If we find one, return true.
    ObjectMap::const_iterator iterFind = m_ObjectsById.find ( uiHash );
    if ( iterFind != m_ObjectsById.end() )
    {
        return true;
    }

    // Loop through our list again for wildchar finding
    char strName [256];
    strName[255] = '\0';
    ObjectList::iterator iter = m_Objects.begin ();
    for ( ; iter != m_Objects.end (); iter++ )
    {
        // Matching type (resource/user)
        if ( eObjectType == (*iter)->GetObjectType () )
        {
            // Grab object name and string length
            const char* szName = (*iter)->GetObjectName ();
            int iLen = strlen ( szName );

            // Long enough string and this is a wildchar entry?
            if ( iLen > 0 && szName [iLen - 1] == '*' )
            {
                // Copy the namestring and remove it's wildchar character
                strncpy ( strName, szName, 255 );
                strName [iLen - 1] = '\0';

                // Does the st
                if ( StringBeginsWith ( szObjectName, strName ) )
                {
                    return true;
                }
            }
        }
    }

    return false;
}


bool CAccessControlListGroup::RemoveObject ( const char* szObjectName, CAccessControlListGroupObject::EObjectType eObjectType )
{
    unsigned int uiHash = CAccessControlListGroupObject::GenerateHashId ( szObjectName, eObjectType );

    // Try to find a match and delete it
    ObjectMap::iterator iter = m_ObjectsById.find ( uiHash );
    if ( iter != m_ObjectsById.end() )
    {
        
        // Delete, remove from list and return true
        delete ( iter->second );
        m_Objects.remove ( iter->second );

        m_ObjectsById.erase( iter );

        return true;
    }

    return false;
}


bool CAccessControlListGroup::AddACL ( CAccessControlList* pACL )
{
    if ( !IsACLPresent ( pACL ) )
    {
        m_ACLs.push_back ( pACL );
        return true;
    }

    return false;
}


bool CAccessControlListGroup::IsACLPresent ( CAccessControlList* pACL )
{
    ACLsList::iterator iter = m_ACLs.begin ();
    for ( ; iter != m_ACLs.end (); iter++ )
    {
        if ( *iter == pACL )
        {
            return true;
        }
    }

    return false;
}


CAccessControlList* CAccessControlListGroup::GetACL ( const char* szACLName )
{
    ACLsList::iterator iter = m_ACLs.begin ();
    for ( ; iter != m_ACLs.end (); iter++ )
    {
        if ( strcmp ( szACLName, (*iter)->GetName () ) == 0 )
        {
            return *iter;
        }
    }

    return NULL;
}


void CAccessControlListGroup::RemoveACL ( class CAccessControlList* pACL )
{
    m_ACLs.remove ( pACL );
}


void CAccessControlListGroup::WriteToXMLNode ( CXMLNode* pNode )
{
    assert ( pNode );

    // Create the subnode for this
    CXMLNode* pSubNode = pNode->CreateSubNode ( "group" );
    assert ( pSubNode );

    // Create attribute for the name and set it
    CXMLAttribute* pAttribute = pSubNode->GetAttributes ().Create ( "name" );
    pAttribute->SetValue ( m_szGroupName );

    // Write the ACL's this group use
    ACLsList::iterator iterACL = m_ACLs.begin ();
    for ( ; iterACL != m_ACLs.end (); iterACL++ )
    {
        CAccessControlList* pACL = *iterACL;

        // Create the subnode for this object and write the name attribute we generated
        CXMLNode* pObjectNode = pSubNode->CreateSubNode ( "acl" );
        pAttribute = pObjectNode->GetAttributes ().Create ( "name" );
        pAttribute->SetValue ( pACL->GetName () );
    }

    // Write every object
    ObjectList::iterator iter = m_Objects.begin ();
    for ( ; iter != m_Objects.end (); iter++ )
    {
        CAccessControlListGroupObject* pObject = *iter;

        // Find out the object type string
        char szObjectType [255];
        switch ( pObject->GetObjectType () )
        {
            case CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE:
                strcpy ( szObjectType, "resource" );
                break;

            case CAccessControlListGroupObject::OBJECT_TYPE_USER:
                strcpy ( szObjectType, "user" );
                break;

            default:
                strcpy ( szObjectType, "error" );
                break;
        }

        // Append a dot append the name of the node
        strcat ( szObjectType, "." );
        strncat ( szObjectType, pObject->GetObjectName (), 255 );

        // Create the subnode for this object and write the name attribute we generated
        CXMLNode* pObjectNode = pSubNode->CreateSubNode ( "object" );
        pAttribute = pObjectNode->GetAttributes ().Create ( "name" );
        pAttribute->SetValue ( szObjectType );
    }
}
