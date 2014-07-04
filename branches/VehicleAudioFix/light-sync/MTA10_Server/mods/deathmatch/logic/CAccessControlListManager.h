/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CAccessControlListManager.h
*  PURPOSE:     Access control list manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CAccessControlListManager;

#ifndef __CACCESSCONTROLLISTMANAGER_H
#define __CACCESSCONTROLLISTMANAGER_H

#include <list>
#include "CAccessControlListRight.h"
#include "CAccessControlListGroup.h"
#include "CAccessControlList.h"
#include "CXMLConfig.h"
#include "CAccountManager.h"

class CAccessControlListManager : public CXMLConfig
{
public:
                                                CAccessControlListManager   ( void );
    virtual                                     ~CAccessControlListManager  ( void );

    void                                        DoPulse                     ( void );
    bool                                        Load                        ( const char* szFilename );
    bool                                        Save                        ( const char* szFilename );

    class CAccessControlListGroup*              GetGroup                    ( const char* szGroupName );
    class CAccessControlList*                   GetACL                      ( const char* szACLName );

    bool                                        CanObjectUseRight           ( const char* szObjectName, CAccessControlListGroupObject::EObjectType, const char* szRightName, CAccessControlListRight::ERightType eRightType, bool bDefaultAccessRight );

    class CAccessControlListGroup*              AddGroup                    ( const char* szGroupName );
    class CAccessControlList*                   AddACL                      ( const char* szACLName );

    void                                        DeleteGroup                 ( class CAccessControlListGroup* pGroup );
    void                                        DeleteACL                   ( class CAccessControlList* pACL );

    void                                        ClearACLs                   ( void );
    void                                        ClearGroups                 ( void );

    bool                                        VerifyGroup                 ( class CAccessControlListGroup* pGroup );
    bool                                        VerifyACL                   ( class CAccessControlList* pACL );

    list < class CAccessControlList* > ::const_iterator             ACL_Begin       ( void )    { return m_ACLs.begin (); };
    list < class CAccessControlList* > ::const_iterator             ACL_End         ( void )    { return m_ACLs.end (); };
    size_t                                                          ACL_Count       ( void )    { return m_ACLs.size (); };

    list < class CAccessControlListGroup* > ::const_iterator        Groups_Begin    ( void )    { return m_Groups.begin (); };
    list < class CAccessControlListGroup* > ::const_iterator        Groups_End      ( void )    { return m_Groups.end (); };
    size_t                                                          Groups_Count    ( void )    { return m_Groups.size (); };

    static const char*                          ExtractObjectName           ( const char* szObjectName,
                                                                              CAccessControlListGroupObject::EObjectType& eType );

    static const char*                          ExtractRightName            ( const char* szRightName,
                                                                              CAccessControlListRight::ERightType& eType );

private:
    void                                        ClearCache                  ( void );
    bool                                        InternalCanObjectUseRight   ( const char* szObjectName, CAccessControlListGroupObject::EObjectType, const char* szRightName, CAccessControlListRight::ERightType eRightType, bool bDefaultAccessRight );
    void                                        RemoveACLDependencies       ( class CAccessControlList* pACL );

    list < class CAccessControlListGroup* >     m_Groups;
    list < class CAccessControlList* >          m_ACLs;

    CAccountManager*                            m_pAccountManager;

    CXMLFile*                                   m_pXML;
    CXMLNode*                                   m_pRootNode;

    long long                                   m_llLastTimeCacheCleared;
    std::map < SString, bool >                  m_ACLCacheMap;
};

#endif

