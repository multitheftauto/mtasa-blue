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
    ZERO_ON_NEW
                                                CAccessControlListManager   ( void );
    virtual                                     ~CAccessControlListManager  ( void );

    void                                        DoPulse                     ( void );
    bool                                        Load                        ( void );
    bool                                        Reload                      ( void );
    bool                                        Save                        ( void );

    class CAccessControlListGroup*              GetGroup                    ( const std::string& strGroupName );
    class CAccessControlList*                   GetACL                      ( const std::string& strACLName );

    bool                                        CanObjectUseRight           ( const std::string& strObjectName, CAccessControlListGroupObject::EObjectType, const std::string& strRightName, CAccessControlListRight::ERightType eRightType, bool bDefaultAccessRight );

    class CAccessControlListGroup*              AddGroup                    ( const std::string& strGroupName );
    class CAccessControlList*                   AddACL                      ( const std::string& strACLName );

    void                                        DeleteGroup                 ( class CAccessControlListGroup* pGroup );
    void                                        DeleteACL                   ( class CAccessControlList* pACL );

    void                                        ClearACLs                   ( void );
    void                                        ClearGroups                 ( void );

    CAccessControlListGroup*                    GetGroupFromScriptID        ( uint uiScriptID );
    CAccessControlList*                         GetACLFromScriptID          ( uint uiScriptID );

    list < class CAccessControlList* > ::const_iterator             ACL_Begin       ( void )    { return m_ACLs.begin (); };
    list < class CAccessControlList* > ::const_iterator             ACL_End         ( void )    { return m_ACLs.end (); };
    size_t                                                          ACL_Count       ( void )    { return m_ACLs.size (); };

    list < class CAccessControlListGroup* > ::const_iterator        Groups_Begin    ( void )    { return m_Groups.begin (); };
    list < class CAccessControlListGroup* > ::const_iterator        Groups_End      ( void )    { return m_Groups.end (); };
    size_t                                                          Groups_Count    ( void )    { return m_Groups.size (); };

    static std::string                          ExtractObjectName           ( const std::string& strObjectName,
                                                                              CAccessControlListGroupObject::EObjectType& eType );

    static std::string                          ExtractRightName            ( const std::string& strRightName,
                                                                              CAccessControlListRight::ERightType& eType );

    void                                        OnChange                    ( void );
    uint                                        GetGlobalRevision           ( void )        { return m_uiGlobalRevision; }
    std::vector < SString >                     GetObjectGroupNames         ( const SString& strObjectName, CAccessControlListGroupObject::EObjectType objectType );

private:
    void                                        ClearReadCache              ( void );
    bool                                        InternalCanObjectUseRight   ( const std::string& strObjectName, CAccessControlListGroupObject::EObjectType, const std::string& strRightName, CAccessControlListRight::ERightType eRightType, bool bDefaultAccessRight );
    void                                        RemoveACLDependencies       ( class CAccessControlList* pACL );

    list < class CAccessControlListGroup* >     m_Groups;
    list < class CAccessControlList* >          m_ACLs;

    CAccountManager*                            m_pAccountManager;

    CXMLFile*                                   m_pXML;
    CXMLNode*                                   m_pRootNode;

    bool                                        m_bReadCacheDirty;
    long long                                   m_llLastTimeReadCacheCleared;
    CFastHashMap < SString, bool >              m_ReadCacheMap;

    bool                                        m_bNeedsSave;
    bool                                        m_bAllowSave;
    CElapsedTime                                m_AutoSaveTimer;
    uint                                        m_uiGlobalRevision;
};

#endif

