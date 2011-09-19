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
