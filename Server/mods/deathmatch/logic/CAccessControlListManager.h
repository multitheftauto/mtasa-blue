/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CAccessControlListManager.h
 *  PURPOSE:     Access control list manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

class CAccessControlListManager;

#pragma once

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
    CAccessControlListManager();
    virtual ~CAccessControlListManager();

    void DoPulse();
    bool Load();
    bool Reload();
    bool Save();

    class CAccessControlListGroup* GetGroup(const char* szGroupName);
    class CAccessControlList*      GetACL(const char* szACLName);

    bool CanObjectUseRight(const char* szObjectName, CAccessControlListGroupObject::EObjectType, const char* szRightName,
                           CAccessControlListRight::ERightType eRightType, bool bDefaultAccessRight);

    class CAccessControlListGroup* AddGroup(const char* szGroupName);
    class CAccessControlList*      AddACL(const char* szACLName);

    void DeleteGroup(class CAccessControlListGroup* pGroup);
    void DeleteACL(class CAccessControlList* pACL);

    void ClearACLs();
    void ClearGroups();

    CAccessControlListGroup* GetGroupFromScriptID(uint uiScriptID);
    CAccessControlList*      GetACLFromScriptID(uint uiScriptID);

    std::list<class CAccessControlList*>::const_iterator ACL_Begin() { return m_ACLs.begin(); };
    std::list<class CAccessControlList*>::const_iterator ACL_End() { return m_ACLs.end(); };
    size_t                                               ACL_Count() { return m_ACLs.size(); };

    std::list<class CAccessControlListGroup*>::const_iterator Groups_Begin() { return m_Groups.begin(); };
    std::list<class CAccessControlListGroup*>::const_iterator Groups_End() { return m_Groups.end(); };
    size_t                                                    Groups_Count() { return m_Groups.size(); };

    static const char* ExtractObjectName(const char* szObjectName, CAccessControlListGroupObject::EObjectType& eType);

    static const char* ExtractRightName(const char* szRightName, CAccessControlListRight::ERightType& eType);

    void                 OnChange();
    uint                 GetGlobalRevision() { return m_uiGlobalRevision; }
    std::vector<SString> GetObjectGroupNames(const SString& strObjectName, CAccessControlListGroupObject::EObjectType objectType);

private:
    void ClearReadCache();
    bool InternalCanObjectUseRight(const char* szObjectName, CAccessControlListGroupObject::EObjectType, const char* szRightName,
                                   CAccessControlListRight::ERightType eRightType, bool bDefaultAccessRight);
    void RemoveACLDependencies(class CAccessControlList* pACL);

    std::list<class CAccessControlListGroup*> m_Groups;
    std::list<class CAccessControlList*>      m_ACLs;

    CAccountManager* m_pAccountManager;

    CXMLFile* m_pXML;
    CXMLNode* m_pRootNode;

    bool                        m_bReadCacheDirty;
    long long                   m_llLastTimeReadCacheCleared;
    CFastHashMap<SString, bool> m_ReadCacheMap;

    bool         m_bNeedsSave;
    bool         m_bAllowSave;
    CElapsedTime m_AutoSaveTimer;
    uint         m_uiGlobalRevision;
};
