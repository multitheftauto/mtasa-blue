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
    snprintf ( m_szACLName, MAX_ACL_NAME_LENGTH, "%s", szACLName );
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
    OnChange ();
}


CAccessControlListRight* CAccessControlList::AddRight ( const char* szRightName, CAccessControlListRight::ERightType eRightType, bool bAccess )
{
    CAccessControlListRight* pRight = GetRight ( szRightName, eRightType );

    if ( !pRight )
    {
        pRight = new CAccessControlListRight ( szRightName, eRightType, bAccess, m_pACLManager );
        m_Rights.push_back ( pRight );
        OnChange ();
    }

    return pRight;
}


CAccessControlListRight* CAccessControlList::GetRight ( const char* szRightName, CAccessControlListRight::ERightType eRightType, bool bAllowWildcardMatch )
{
    static unsigned int uiStarHash = HashString ( "*" );
    unsigned int uiHash = HashString ( szRightName );
    CAccessControlListRight* pWildcardMatch = NULL;

    list < CAccessControlListRight* > ::iterator iter = m_Rights.begin ();
    for ( ; iter != m_Rights.end (); iter++ )
    {
        CAccessControlListRight* pACLRight = *iter;
        if ( eRightType == pACLRight->GetRightType () )
        {
            if ( pACLRight->GetRightNameHash () == uiHash && SStringX ( szRightName ) == pACLRight->GetRightName () )
            {
                // Exact match
                return pACLRight;
            }
            else
            if ( bAllowWildcardMatch && pACLRight->GetRightNameHash () == uiStarHash )
            {
                // Save first wildcard match to be used if no exact matches
                if ( !pWildcardMatch )
                    pWildcardMatch = pACLRight;
            }
        }
    }

    return pWildcardMatch;
}


bool CAccessControlList::RemoveRight ( const char* szRightName, CAccessControlListRight::ERightType eRightType )
{
    unsigned int uiHash = HashString ( szRightName );

    list < CAccessControlListRight* > ::iterator iter = m_Rights.begin ();
    for ( ; iter != m_Rights.end (); iter++ )
    {
        CAccessControlListRight* pACLRight = *iter;
        if ( pACLRight->GetRightNameHash () == uiHash && eRightType == pACLRight->GetRightType ()
             && SStringX ( szRightName ) == pACLRight->GetRightName () )
        {
            m_Rights.remove ( pACLRight );
            delete pACLRight;
            OnChange ();
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
        pRight->WriteToXMLNode ( pSubNode );
    }
}


void CAccessControlList::OnChange ( void )
{
    g_pGame->GetACLManager ()->OnChange ();
}


bool CAccessControlList::CanBeModifiedByScript ( void )
{
    // If this isn't horrible, I don't know what is
    return !SStringX ( GetName () ).BeginsWith ( "autoACL_" );
}
