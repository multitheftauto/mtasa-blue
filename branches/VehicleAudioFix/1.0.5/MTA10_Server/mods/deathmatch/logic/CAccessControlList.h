/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CAccessControlList.h
*  PURPOSE:     Access control list class
*  DEVELOPERS:  Oliver Brown <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CAccessControlList;

#ifndef __CACCESSCONTROLLIST_H
#define __CACCESSCONTROLLIST_H

#include <list>
#include "CAccessControlListRight.h"
#include <xml/CXMLNode.h>

#define MAX_ACL_NAME_LENGTH 256

class CAccessControlList
{
public:
                                                CAccessControlList          ( const char* szACLName, class CAccessControlListManager* pACLManager );
                                                ~CAccessControlList         ( void );

    inline const char*                          GetName                     ( void )        { return m_szACLName; };

    CAccessControlListRight*                    AddRight                    ( const char* szRightName, CAccessControlListRight::ERightType eRightType, bool bAccess );
    CAccessControlListRight*                    GetRight                    ( const char* szRightName, CAccessControlListRight::ERightType eRightType );
    bool                                        RemoveRight                 ( const char* szRightName, CAccessControlListRight::ERightType eRightType );

    void                                        WriteToXMLNode              ( CXMLNode* pNode );

    inline list < CAccessControlListRight* >  ::const_iterator  IterBegin   ( void ) { return m_Rights.begin (); };
    inline list < CAccessControlListRight* >  ::const_iterator  IterEnd     ( void ) { return m_Rights.end (); };

private:
    char                                        m_szACLName                 [ MAX_ACL_NAME_LENGTH ];
    list < CAccessControlListRight* >           m_Rights;

    class CAccessControlListManager*            m_pACLManager;

};

#endif
