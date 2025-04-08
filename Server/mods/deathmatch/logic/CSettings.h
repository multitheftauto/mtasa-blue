/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CSettings.h
 *  PURPOSE:     XML-based variable settings class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

class CSettings;

#pragma once

#include "CClient.h"
#include "CXMLConfig.h"
#include "CLogger.h"

class CResourceManager;

class CSettings
{
public:
    enum AccessType
    {
        Private,
        Protected,
        Public
    };
    CSettings(CResourceManager* pResourceManager);
    ~CSettings();

    CXMLNode* Get(const char* szLocalResource, const char* szSetting, bool& bDeleteNode);
    bool      Set(const char* szLocalResource, const char* szSetting, const char* szContent);

private:
    enum SettingStatus
    {
        NoAccess,
        NotFound,
        Found
    };

    CXMLNode* Get(CXMLNode* pSource, CXMLNode* pStorage, const char* szSourceResource, const char* szLocalResource, const char* szSetting, bool& bDeleteNode,
                  SettingStatus& eSetting, CXMLNode* pMultiresultParentNode = NULL);

    CXMLNode*             CreateSetting(CXMLNode* pDst, const char* szSetting, const char* szContent);
    CSettings::AccessType GetAccessType(char cCharacter);
    bool                  HasPrefix(char cCharacter);
    const char*           GetResourceName(const char* szSetting, char* szBuffer, unsigned int uiLength);
    bool                  HasResourceName(const char* szSetting);
    const char*           GetName(const char* szSetting, unsigned int uiResourceLength = -1);

    SString   m_strPath;
    CXMLFile* m_pFile;
    CXMLNode* m_pNodeGlobalSettings;

    CResourceManager* m_pResourceManager;
};
