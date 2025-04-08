/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CAccessControlListManager.cpp
 *  PURPOSE:     Access control list manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CAccessControlListManager.h"
#include "CLogger.h"
#include "CGame.h"
#include "Utils.h"
#include "CIdArray.h"

CAccessControlListManager::CAccessControlListManager() : CXMLConfig(NULL)
{
    m_pXML = NULL;
    m_pRootNode = NULL;
    m_llLastTimeReadCacheCleared = 0;
    m_bReadCacheDirty = false;
    m_bNeedsSave = false;
    m_uiGlobalRevision = 1;
}

CAccessControlListManager::~CAccessControlListManager()
{
    if (m_bNeedsSave)
        Save();

    // Clear the ACL stuff
    ClearACLs();
    ClearGroups();

    // Delete the XML
    if (m_pXML)
    {
        delete m_pXML;
        m_pXML = NULL;
    }
}

bool CAccessControlListManager::Load()
{
    m_bAllowSave = true;

    // Eventually destroy the previously loaded xml
    if (m_pXML)
    {
        delete m_pXML;
    }

    // Load the XML
    m_pXML = g_pServerInterface->GetXML()->CreateXML(GetFileName().c_str());
    if (!m_pXML)
    {
        CLogger::ErrorPrintf("Error loading Access Control List file\n");
        return false;
    }

    // Parse it
    if (!m_pXML->Parse())
    {
        SString strParseErrorDesc;
        m_pXML->GetLastError(strParseErrorDesc);
        CLogger::ErrorPrintf("Error parsing %s - %s\n", *ExtractFilename(GetFileName()), *strParseErrorDesc);
        return false;
    }

    // Grab the XML root node
    m_pRootNode = m_pXML->GetRootNode();
    if (!m_pRootNode)
    {
        CLogger::ErrorPrintf("Missing root node ('ACL')\n");
        return false;
    }

    // Clear previous ACL stuff
    ClearACLs();
    ClearGroups();
    ClearReadCache();

    // load the acl's
    CXMLNode*    pSubNode = NULL;
    unsigned int uiSubNodesCount = m_pRootNode->GetSubNodeCount();
    for (unsigned int i = 0; i < uiSubNodesCount; i++)
    {
        pSubNode = m_pRootNode->GetSubNode(i);
        if (!pSubNode)
            continue;

        if (pSubNode->GetTagName().compare("acl") == 0)
        {
            CXMLAttribute* pAttribute = pSubNode->GetAttributes().Find("name");
            if (pAttribute)
            {
                CAccessControlList* pACL = AddACL(pAttribute->GetValue().c_str());

                CXMLNode*    pSubSubNode = NULL;
                unsigned int uiSubSubNodesCount = pSubNode->GetSubNodeCount();
                for (unsigned int j = 0; j < uiSubSubNodesCount; j++)
                {
                    // If this subnode doesn't exist, return to the for loop and continue it
                    pSubSubNode = pSubNode->GetSubNode(j);
                    if (!pSubSubNode)
                        continue;

                    // Check that this subsub node is named "right"
                    if (pSubSubNode->GetTagName().compare("right") == 0)
                    {
                        // Grab the name and the access attributes
                        CXMLAttribute* pNameAttribute = pSubSubNode->GetAttributes().Find("name");
                        CXMLAttribute* pAccessAttribute = pSubSubNode->GetAttributes().Find("access");
                        if (pNameAttribute && pAccessAttribute)
                        {
                            // See if the access attribute is true or false
                            bool        bAccess = false;
                            std::string strAccess = pAccessAttribute->GetValue();

                            if (stricmp(strAccess.c_str(), "true") == 0 || stricmp(strAccess.c_str(), "yes") == 0 || strcmp(strAccess.c_str(), "1") == 0)
                            {
                                bAccess = true;
                            }

                            // Grab the name of the 'right' name
                            const char* szRightName = pNameAttribute->GetValue().c_str();

                            // Create the rights control list
                            CAccessControlListRight* pRight = NULL;
                            if (StringBeginsWith(szRightName, "command."))
                            {
                                pRight = pACL->AddRight(&szRightName[8], CAccessControlListRight::RIGHT_TYPE_COMMAND, bAccess);
                            }
                            else if (StringBeginsWith(szRightName, "function."))
                            {
                                pRight = pACL->AddRight(&szRightName[9], CAccessControlListRight::RIGHT_TYPE_FUNCTION, bAccess);
                            }
                            else if (StringBeginsWith(szRightName, "resource."))
                            {
                                pRight = pACL->AddRight(&szRightName[9], CAccessControlListRight::RIGHT_TYPE_RESOURCE, bAccess);
                            }
                            else if (StringBeginsWith(szRightName, "general."))
                            {
                                pRight = pACL->AddRight(&szRightName[8], CAccessControlListRight::RIGHT_TYPE_GENERAL, bAccess);
                            }
                            else
                                continue;

                            // Set all the extra attributes
                            for (uint i = 0; i < pSubSubNode->GetAttributes().Count(); i++)
                            {
                                CXMLAttribute* pAttribute = pSubSubNode->GetAttributes().Get(i);
                                pRight->SetAttributeValue(pAttribute->GetName(), pAttribute->GetValue());
                            }
                        }
                    }
                }
            }
        }
    }

    // Load the groups
    pSubNode = NULL;
    uiSubNodesCount = m_pRootNode->GetSubNodeCount();
    for (unsigned int i = 0; i < uiSubNodesCount; i++)
    {
        pSubNode = m_pRootNode->GetSubNode(i);
        if (!pSubNode)
            continue;

        if (pSubNode->GetTagName().compare("group") == 0)
        {
            CXMLAttribute* pAttribute = pSubNode->GetAttributes().Find("name");
            if (pAttribute)
            {
                CAccessControlListGroup* pGroup = AddGroup(pAttribute->GetValue().c_str());

                CXMLNode*    pSubSubNode = NULL;
                unsigned int uiSubSubNodesCount = pSubNode->GetSubNodeCount();
                for (unsigned int j = 0; j < uiSubSubNodesCount; j++)
                {
                    pSubSubNode = pSubNode->GetSubNode(j);
                    if (!pSubSubNode)
                        continue;

                    if (pSubSubNode->GetTagName().compare("object") == 0)
                    {
                        CXMLAttribute* pSubAttribute = pSubSubNode->GetAttributes().Find("name");
                        if (pSubAttribute)
                        {
                            const char* szAccountName = pSubAttribute->GetValue().c_str();

                            if (StringBeginsWith(szAccountName, "user."))
                            {
                                pGroup->AddObject(&szAccountName[5], CAccessControlListGroupObject::OBJECT_TYPE_USER);
                            }
                            else if (StringBeginsWith(szAccountName, "resource."))
                            {
                                pGroup->AddObject(&szAccountName[9], CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE);
                            }
                        }
                    }
                    else if (pSubSubNode->GetTagName().compare("acl") == 0)
                    {
                        CXMLAttribute* pSubAttribute = pSubSubNode->GetAttributes().Find("name");
                        if (pSubAttribute)
                        {
                            CAccessControlList* pACL = GetACL(pSubAttribute->GetValue().c_str());
                            if (pACL)
                            {
                                pGroup->AddACL(pACL);
                            }
                        }
                    }
                }
            }
        }
    }

    m_bNeedsSave = false;
    return true;
}

bool CAccessControlListManager::Reload()
{
    // Flush any pending saves - This is ok because reloading is for loading manual changes to acl.xml
    // and manual changes are subject to being overwritten by server actions at any time.
    if (m_bNeedsSave)
        Save();

    return Load();
}

bool CAccessControlListManager::Save()
{
    // Only allow save after a load was attempted
    if (!m_bAllowSave)
        return false;

    m_bNeedsSave = false;
    m_AutoSaveTimer.Reset();

    // We have it loaded?
    if (m_pXML)
    {
        // Clear it
        m_pXML->Clear();

        // Create a root node
        CXMLNode* pNode = m_pXML->CreateRootNode("acl");
        if (pNode)
        {
            // Loop through the groups writing them to the ACL
            std::list<CAccessControlListGroup*>::iterator iter = m_Groups.begin();
            for (; iter != m_Groups.end(); iter++)
            {
                (*iter)->WriteToXMLNode(pNode);
            }

            // Loop through the ACLs writing them to the ACL file
            std::list<CAccessControlList*>::iterator iterACL = m_ACLs.begin();
            for (; iterACL != m_ACLs.end(); iterACL++)
            {
                (*iterACL)->WriteToXMLNode(pNode);
            }

            // Save the XML
            if (m_pXML->Write())
                return true;
            CLogger::ErrorPrintf("Error saving '%s'\n", GetFileName().c_str());
        }
    }

    return false;
}

CAccessControlListGroup* CAccessControlListManager::GetGroup(const char* szGroupName)
{
    // Loop through the list and find the group with a matching name
    list<CAccessControlListGroup*>::iterator iter = m_Groups.begin();
    for (; iter != m_Groups.end(); iter++)
    {
        if (strcmp(szGroupName, (*iter)->GetGroupName()) == 0)
        {
            return *iter;
        }
    }

    return NULL;
}

CAccessControlList* CAccessControlListManager::GetACL(const char* szACLName)
{
    // Loop through the list and find the ACL with a matching name
    list<CAccessControlList*>::iterator iter = m_ACLs.begin();
    for (; iter != m_ACLs.end(); iter++)
    {
        if (strcmp(szACLName, (*iter)->GetName()) == 0)
        {
            return *iter;
        }
    }

    return NULL;
}

void CAccessControlListManager::DoPulse()
{
    // Clear cache every 12 hours or if dirty
    if (m_bReadCacheDirty || GetTickCount64_() - m_llLastTimeReadCacheCleared > 1000 * 60 * 60 * 12)
        ClearReadCache();

    // Save when needed, but no more than once a second
    if (m_AutoSaveTimer.Get() > 1000 && m_bNeedsSave)
        Save();
}

void CAccessControlListManager::ClearReadCache()
{
    m_bReadCacheDirty = false;
    m_llLastTimeReadCacheCleared = GetTickCount64_();
    m_ReadCacheMap.clear();
    m_uiGlobalRevision++;
}

bool CAccessControlListManager::CanObjectUseRight(const char* szObjectName, CAccessControlListGroupObject::EObjectType eObjectType, const char* szRightName,
                                                  CAccessControlListRight::ERightType eRightType, bool bDefaultAccessRight)
{
    // Clear cache if required
    if (m_bReadCacheDirty)
        ClearReadCache();

    // Make unique key for this query
    SString strKey("%s %s %d %d %d", szObjectName, szRightName, eObjectType, eRightType, bDefaultAccessRight);
    // Check if this query has been done before
    bool* pResult = MapFind(m_ReadCacheMap, strKey);
    if (!pResult)
    {
        // If not, do query now and add result to the cache
        bool bResult = InternalCanObjectUseRight(szObjectName, eObjectType, szRightName, eRightType, bDefaultAccessRight);
        MapSet(m_ReadCacheMap, strKey, bResult);
        return bResult;
    }
    // Return cached result
    return *pResult;
}

bool CAccessControlListManager::InternalCanObjectUseRight(const char* szObjectName, CAccessControlListGroupObject::EObjectType eObjectType,
                                                          const char* szRightName, CAccessControlListRight::ERightType eRightType, bool bDefaultAccessRight)
{
    // This is set to true if we were explicitly denied access by an ACL
    bool bDenied = false;

    // Look through the groups
    list<CAccessControlListGroup*>::iterator group = m_Groups.begin();
    for (; group != m_Groups.end(); group++)
    {
        // Look for a group that has our user/resource in it
        if ((*group)->FindObjectMatch(szObjectName, eObjectType))
        {
            // Look through its access lists for our 'right' name
            list<CAccessControlList*>::iterator acl = (*group)->IterBeginACL();
            for (; acl != (*group)->IterEndACL(); acl++)
            {
                // Grab the right with this name
                CAccessControlListRight* pRight = (*acl)->GetRight(szRightName, eRightType);
                if (pRight)
                {
                    // If he has access, return that he can use this object. Otherwize keep looking
                    // for an ACL that gives him permission to do so.
                    if (pRight->GetRightAccess())
                    {
                        return true;
                    }
                    else
                    {
                        bDenied = true;
                    }
                }
            }
        }
    }

    // An ACL denied us access and no ACL gave us access. No access given.
    if (bDenied)
        return false;

    // Otherwize if nothing specified, return the default right
    return bDefaultAccessRight;
}

CAccessControlListGroup* CAccessControlListManager::AddGroup(const char* szGroupName)
{
    // Grab the group with that name already. Only add it if it doesn't exist already
    CAccessControlListGroup* pGroup = GetGroup(szGroupName);
    if (!pGroup)
    {
        // Create it and put it back in our list
        pGroup = new CAccessControlListGroup(szGroupName);
        m_Groups.push_back(pGroup);
        OnChange();
    }

    return pGroup;
}

CAccessControlList* CAccessControlListManager::AddACL(const char* szACLName)
{
    // Grab the ACL with that name. Only add it if we don't already have it
    CAccessControlList* pACL = GetACL(szACLName);
    if (!pACL)
    {
        // Create it and put it back in our list
        pACL = new CAccessControlList(szACLName, this);
        m_ACLs.push_back(pACL);
        OnChange();
    }

    return pACL;
}

void CAccessControlListManager::DeleteGroup(class CAccessControlListGroup* pGroup)
{
    assert(pGroup);

    // Delete the class and remove it from the list
    delete pGroup;
    m_Groups.remove(pGroup);
    OnChange();
}

void CAccessControlListManager::DeleteACL(class CAccessControlList* pACL)
{
    assert(pACL);

    // Remove its dependencies among the groups
    RemoveACLDependencies(pACL);

    // Delete the class and remove it from the list
    delete pACL;
    m_ACLs.remove(pACL);
    OnChange();
}

void CAccessControlListManager::ClearACLs()
{
    // Return true if it exists
    list<CAccessControlList*>::iterator iter = m_ACLs.begin();
    for (; iter != m_ACLs.end(); iter++)
    {
        delete *iter;
    }

    // Clear the list
    m_ACLs.clear();
    OnChange();
}

void CAccessControlListManager::ClearGroups()
{
    // Delete all the ACLs
    list<CAccessControlListGroup*>::iterator iter = m_Groups.begin();
    for (; iter != m_Groups.end(); iter++)
    {
        delete *iter;
    }

    // Clear the list
    m_Groups.clear();
    OnChange();
}

CAccessControlList* CAccessControlListManager::GetACLFromScriptID(uint uiScriptID)
{
    CAccessControlList* pACL = (CAccessControlList*)CIdArray::FindEntry(uiScriptID, EIdClass::ACL);
    dassert(!pACL || ListContains(m_ACLs, pACL));
    return pACL;
}

CAccessControlListGroup* CAccessControlListManager::GetGroupFromScriptID(uint uiScriptID)
{
    CAccessControlListGroup* pGroup = (CAccessControlListGroup*)CIdArray::FindEntry(uiScriptID, EIdClass::ACL_GROUP);
    dassert(!pGroup || ListContains(m_Groups, pGroup));
    return pGroup;
}

void CAccessControlListManager::RemoveACLDependencies(class CAccessControlList* pACL)
{
    // Remove all groups depending on the given ACL
    list<CAccessControlListGroup*>::iterator iter = m_Groups.begin();
    for (; iter != m_Groups.end(); iter++)
    {
        (*iter)->RemoveACL(pACL);
    }
    OnChange();
}

const char* CAccessControlListManager::ExtractObjectName(const char* szObjectName, CAccessControlListGroupObject::EObjectType& eType)
{
    // Pick the correct type based on what the string starts with.
    // Return where the name of the right begins.
    if (StringBeginsWith(szObjectName, "user."))
    {
        eType = CAccessControlListGroupObject::OBJECT_TYPE_USER;
        return szObjectName + 5;
    }
    else if (StringBeginsWith(szObjectName, "resource."))
    {
        eType = CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE;
        return szObjectName + 9;
    }

    // Failed
    return NULL;
}

const char* CAccessControlListManager::ExtractRightName(const char* szRightName, CAccessControlListRight::ERightType& eType)
{
    // Pick the correct type based on what the string starts with.
    // Return where the name of the object begins.
    if (StringBeginsWith(szRightName, "command."))
    {
        eType = CAccessControlListRight::RIGHT_TYPE_COMMAND;
        return szRightName + 8;
    }
    else if (StringBeginsWith(szRightName, "function."))
    {
        eType = CAccessControlListRight::RIGHT_TYPE_FUNCTION;
        return szRightName + 9;
    }
    else if (StringBeginsWith(szRightName, "resource."))
    {
        eType = CAccessControlListRight::RIGHT_TYPE_RESOURCE;
        return szRightName + 9;
    }
    else if (StringBeginsWith(szRightName, "general."))
    {
        eType = CAccessControlListRight::RIGHT_TYPE_GENERAL;
        return szRightName + 8;
    }

    // Failed
    return NULL;
}

//
// Get list of group names the supplied object belongs to
//
std::vector<SString> CAccessControlListManager::GetObjectGroupNames(const SString& strObjectName, CAccessControlListGroupObject::EObjectType objectType)
{
    std::vector<SString> groupNameList;
    for (auto pGroup : m_Groups)
        for (auto itero = pGroup->IterBeginObjects(); itero != pGroup->IterEndObjects(); itero++)
            if ((*itero)->GetObjectType() == objectType)
                if (strObjectName == (*itero)->GetObjectName() || strcmp((*itero)->GetObjectName(), "*") == 0)
                    groupNameList.push_back(pGroup->GetGroupName());
    return groupNameList;
}

//
// Called when an item within the ACL is modified
//
void CAccessControlListManager::OnChange()
{
    m_bReadCacheDirty = true;
    m_bNeedsSave = true;
    m_uiGlobalRevision++;
}
