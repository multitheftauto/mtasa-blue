/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CAccessControlList.cpp
*  PURPOSE:     Access control list class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*               Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CAccessControlList::CAccessControlList ( const char* szACLName, CAccessControlListManager* pACLManager )
{
    m_szACLName[0] = '\0';
    _snprintf ( m_szACLName, MAX_ACL_NAME_LENGTH, "%s", szACLName );
    m_szACLName[MAX_ACL_NAME_LENGTH-1] = '\0';

    m_pACLManager = pACLManager;
}


CAccessControlList::~CAccessControlList ( void )
{
    list < CAccessControlListRight* > ::iterator iter = m_Rights.begin ();
    for ( ; iter != m_Rights.end (); iter++ )
    {
        delete *iter;
    }

    m_Rights.clear ();
}


CAccessControlListRight* CAccessControlList::AddRight ( const char* szRightName, CAccessControlListRight::ERightType eRightType, bool bAccess )
{
    CAccessControlListRight* pRight = GetRight ( szRightName, eRightType );

    if ( !pRight )
    {
        pRight = new CAccessControlListRight ( szRightName, eRightType, bAccess, m_pACLManager );
        m_Rights.push_back ( pRight );
    }

    return pRight;
}


CAccessControlListRight* CAccessControlList::GetRight ( const char* szRightName, CAccessControlListRight::ERightType eRightType )
{
    unsigned int uiHash = HashString ( szRightName );

    list < CAccessControlListRight* > ::iterator iter = m_Rights.begin ();
    for ( ; iter != m_Rights.end (); iter++ )
    {
        if ( (*iter)->GetRightNameHash () == uiHash && eRightType == (*iter)->GetRightType () )
        {
            return *iter;
        }
    }

    return NULL;
}


bool CAccessControlList::RemoveRight ( const char* szRightName, CAccessControlListRight::ERightType eRightType )
{
    unsigned int uiHash = HashString ( szRightName );

    list < CAccessControlListRight* > ::iterator iter = m_Rights.begin ();
    for ( ; iter != m_Rights.end (); iter++ )
    {
        CAccessControlListRight* pACLRight = *iter;
        if ( pACLRight->GetRightNameHash () == uiHash && eRightType == pACLRight->GetRightType () )
        {
            m_Rights.remove ( pACLRight );
            delete pACLRight;
            return true;
        }
    }

    return false;
}


void CAccessControlList::WriteToXMLNode ( CXMLNode* pNode )
{
    assert ( pNode );

    // Create the subnode for this
    CXMLNode* pSubNode = pNode->CreateSubNode ( "acl" );
    assert ( pSubNode );

    // Create attribute for the name and set it
    CXMLAttribute* pAttribute = pSubNode->GetAttributes ().Create ( "name" );
    pAttribute->SetValue ( m_szACLName );

    // Loop through each right and write it to the ACL
    list < CAccessControlListRight* > ::iterator iter = m_Rights.begin ();
    for ( ; iter != m_Rights.end (); iter++ )
    {
        CAccessControlListRight* pRight = *iter;

        // Find out the right type string
        char szRightType [255];
        switch ( pRight->GetRightType () )
        {
            case CAccessControlListRight::RIGHT_TYPE_COMMAND:
                strcpy ( szRightType, "command" );
                break;

            case CAccessControlListRight::RIGHT_TYPE_FUNCTION:
                strcpy ( szRightType, "function" );
                break;

            case CAccessControlListRight::RIGHT_TYPE_GENERAL:
                strcpy ( szRightType, "general" );
                break;

            case CAccessControlListRight::RIGHT_TYPE_RESOURCE:
                strcpy ( szRightType, "resource" );
                break;

            default:
                strcpy ( szRightType, "error" );
                break;
        }

        // Append a dot append the name of the node
        strcat ( szRightType, "." );
        strncat ( szRightType, pRight->GetRightName (), 255 );

        // Create the subnode for this object and write the name attribute we generated
        CXMLNode* pRightNode = pSubNode->CreateSubNode ( "right" );
        pAttribute = pRightNode->GetAttributes ().Create ( "name" );
        pAttribute->SetValue ( szRightType );

        // Create the access attribute
        pAttribute = pRightNode->GetAttributes ().Create ( "access" );
        if ( pRight->GetRightAccess () )
            pAttribute->SetValue ( "true" );
        else
            pAttribute->SetValue ( "false" );
    }
}

