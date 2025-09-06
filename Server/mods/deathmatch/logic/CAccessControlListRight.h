/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CAccessControlListRight.h
 *  PURPOSE:     Access control list rights class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

class CAccessControlListRight;

#pragma once

#define MAX_ACL_RIGHT_NAME_LENGTH 256

class CAccessControlListRight
{
public:
    enum ERightType
    {
        RIGHT_TYPE_COMMAND,
        RIGHT_TYPE_FUNCTION,
        RIGHT_TYPE_RESOURCE,
        RIGHT_TYPE_GENERAL
    };

public:
    CAccessControlListRight(const char* szRightName, ERightType eRightType, bool bAccess, class CAccessControlListManager* pACLManager);
    virtual ~CAccessControlListRight() { OnChange(); }

    void WriteToXMLNode(CXMLNode* pNode);

    const char*  GetRightName() { return m_strRightName; };
    unsigned int GetRightNameHash() { return m_uiNameHash; };
    ERightType   GetRightType() { return m_eRightType; };

    bool GetRightAccess() { return m_bAccess; };
    void SetRightAccess(bool bAccess)
    {
        m_bAccess = bAccess;
        OnChange();
    };

    void    SetAttributeValue(const SString& strAttributeName, const SString& strAttributeValue);
    SString GetAttributeValue(const SString& strAttributeName);

private:
    void OnChange();

    SString                          m_strRightName;
    unsigned int                     m_uiNameHash;
    ERightType                       m_eRightType;
    bool                             m_bAccess;
    class CAccessControlListManager* m_pACLManager;
    std::map<SString, SString>       m_ExtraAttributeMap;
};
