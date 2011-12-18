/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CAccessControlListRight.cpp
*  PURPOSE:     Access control list rights class
*  DEVELOPERS:  Oliver Brown <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CAccessControlListRight::CAccessControlListRight ( const char* szRightName, ERightType eRightType, bool bAccess, CAccessControlListManager* pACLManager )
{
    m_szRightName[0] = '\0';
    snprintf ( m_szRightName, MAX_ACL_RIGHT_NAME_LENGTH, "%s", szRightName );
    m_szRightName[MAX_ACL_RIGHT_NAME_LENGTH-1] = '\0';
    m_uiNameHash = HashString ( m_szRightName );

    m_eRightType = eRightType;
    m_bAccess = bAccess;
    m_pACLManager = pACLManager;
}


///////////////////////////////////////////////////////////////
//
// CAccessControlListRight::WriteToXMLNode
//
// Write right subnode
//
///////////////////////////////////////////////////////////////
void CAccessControlListRight::WriteToXMLNode ( CXMLNode* pNode )
{
    assert ( pNode );

    SString strRightFullName = CAclRightName ( m_eRightType, m_szRightName ).GetFullName ();
    SString strAccess = m_bAccess ? "true" : "false";

    CXMLNode* pRightNode = pNode->CreateSubNode ( "right" );
    pRightNode->GetAttributes ().Create ( "name" )->SetValue ( strRightFullName );
    pRightNode->GetAttributes ().Create ( "access" )->SetValue ( strAccess );

    // Create tha extra attributes
    for ( std::map < SString, SString >::reverse_iterator iter = m_ExtraAttributeMap.rbegin () ; iter != m_ExtraAttributeMap.rend () ; ++iter )
    {
        pRightNode->GetAttributes ().Create ( iter->first )->SetValue ( iter->second );
    }
}


///////////////////////////////////////////////////////////////
//
// CAccessControlListRight::SetAttributeValue
//
// Set right attribute by name
//
///////////////////////////////////////////////////////////////
void CAccessControlListRight::SetAttributeValue ( const SString& strAttributeName, const SString& strAttributeValue )
{
    if ( strAttributeName == "access" )
    {
        m_bAccess = StringToBool ( strAttributeValue );
    }
    else
    if ( strAttributeName == "name" )
    {
    }
    else
    {
        MapSet ( m_ExtraAttributeMap, strAttributeName, strAttributeValue );
    }
    OnChange ();
}


///////////////////////////////////////////////////////////////
//
// CAccessControlListRight::GetAttributeValue
//
// Get right attribute by name
//
///////////////////////////////////////////////////////////////
SString CAccessControlListRight::GetAttributeValue ( const SString& strAttributeName )
{
    if ( strAttributeName == "access" )
    {
        return m_bAccess ? "true" : "false";
    }
    else
    if ( strAttributeName == "name" )
    {
        return m_szRightName;
    }
    else
    {
        SString* pResult = MapFind ( m_ExtraAttributeMap, strAttributeName );
        return pResult ? *pResult : "";
    }
}
