/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResourceChecker.h
 *  PURPOSE:     Resource file content checker/validator/upgrader
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <unzip.h>
#ifdef WIN32
    #include <iowin32.h>
#else
    #include <ioapi.h>
#endif
#include <zip.h>

class CResource;

namespace ECheckerMode
{
    enum ECheckerModeType
    {
        NONE,
        UPGRADE,
        WARNINGS,
    };
};
using ECheckerMode::ECheckerModeType;

namespace ECheckerWhat
{
    enum ECheckerWhatType
    {
        NONE,
        REMOVED,
        REPLACED,
        MODIFIED,
    };
};
using ECheckerWhat::ECheckerWhatType;

class CResourceChecker
{
public:
    void LogUpgradeWarnings(CResource* pResource, const std::string& strResourceZip, CMtaVersion& strOutReqClientVersion, CMtaVersion& strOutReqServerVersion,
                            SString& strOutReqClientReason, SString& strOutReqServerReason);
    void ApplyUpgradeModifications(CResource* pResource, const std::string& strResourceZip);

protected:
    void CheckResourceForIssues(CResource* pResource, const std::string& strResourceZip);
    void CheckFileForIssues(const std::string& strPath, const std::string& strFileName, const std::string& strResourceName, bool bScript, bool bClient,
                            bool bMeta);
    void CheckPngFileForIssues(const std::string& strPath, const std::string& strFileName, const std::string& strResourceName);
    void CheckRwFileForIssues(const std::string& strPath, const std::string& strFileName, const std::string& strResourceName);
    void CheckMetaFileForIssues(const std::string& strPath, const std::string& strFileName, const std::string& strResourceName);
    void CheckMetaSourceForIssues(CXMLNode* pRootNode, const std::string& strFileName, const std::string& strResourceName, ECheckerModeType checkerMode,
                                  bool* pbOutHasChanged = NULL);
    void CheckLuaFileForIssues(const std::string& strPath, const std::string& strFileName, const std::string& strResourceName, bool bClientScript);
    bool CheckLuaDeobfuscateRequirements(const std::string& strFileContents, const std::string& strFileName, const std::string& strResourceName,
                                         bool bClientScript);
    void CheckLuaSourceForIssues(std::string strLuaSource, const std::string& strFileName, const std::string& strResourceName, bool bClientScript,
                                 bool bCompiledScript, ECheckerModeType checkerMode, std::string* pstrOutResult = NULL);
    long FindLuaIdentifier(const char* szLuaSource, long* plOutLength, long* plLineNumber);
    bool UpgradeLuaFunctionName(const std::string& strFunctionName, bool bClientScript, std::string& strOutUpgraded);
    void IssueLuaFunctionNameWarnings(const std::string& strFunctionName, const std::string& strFileName, const std::string& strResourceName,
                                      bool bClientScript, unsigned long ulLineNumber);
    ECheckerWhatType GetLuaFunctionNameUpgradeInfo(const std::string& strFunctionName, bool bClientScript, std::string& strOutHow, CMtaVersion& strOutVersion);
    int              ReplaceFilesInZIP(const std::string& strOrigZip, const std::string& strTempZip, const std::vector<std::string>& pathInArchiveList,
                                       const std::vector<std::string>& upgradedFullPathList);
    bool             RenameBackupFile(const std::string& strOrigFilename, const std::string& strBakAppend);
    void             CheckVersionRequirements(const std::string& strIdentifierName, bool bClientScript);

    bool                     m_bUpgradeScripts;
    unsigned long            m_ulDeprecatedWarningCount;
    std::vector<std::string> m_upgradedFullPathList;
    CMtaVersion              m_strMinClientFromMetaXml;
    CMtaVersion              m_strMinServerFromMetaXml;
    CMtaVersion              m_strReqClientVersion;
    CMtaVersion              m_strReqServerVersion;
    SString                  m_strReqClientReason;
    SString                  m_strReqServerReason;
};
