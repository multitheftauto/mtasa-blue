/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceChecker.cpp
*  PURPOSE:     Resource file content checker/validator/upgrader
*  DEVELOPERS:  ccw <chris@codewave.co.uk>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef CRESOURCECHECKER_H
#define CRESOURCECHECKER_H

#include <unzip.h>
#ifdef WIN32
#include <iowin32.h>
#else
#include <ioapi.h>
#endif
#include <zip.h>

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
    };
};
using ECheckerWhat::ECheckerWhatType;


class CResourceChecker
{
public:
    void        LogUpgradeWarnings              ( CResource* pResource, const string& strResourceZip, SString& strOutReqClientVersion, SString& strOutReqServerVersion, SString& strOutReqClientReason, SString& strOutReqServerReason );
    void        ApplyUpgradeModifications       ( CResource* pResource, const string& strResourceZip );

protected:
    void                CheckResourceForIssues          ( CResource* pResource, const string& strResourceZip );
    void                CheckFileForIssues              ( const string& strPath, const string& strFileName, const string& strResourceName, bool bScript, bool bClient, bool bMeta );
    void                CheckPngFileForIssues           ( const string& strPath, const string& strFileName, const string& strResourceName );
    void                CheckRwFileForIssues            ( const string& strPath, const string& strFileName, const string& strResourceName );
    void                CheckMetaFileForIssues          ( const string& strPath, const string& strFileName, const string& strResourceName );
    void                CheckMetaSourceForIssues        ( CXMLNode* pRootNode, const string& strFileName, const string& strResourceName, ECheckerModeType checkerMode, bool* pbOutHasChanged = NULL );
    void                CheckLuaFileForIssues           ( const string& strPath, const string& strFileName, const string& strResourceName, bool bClientScript );
    bool                CheckLuaDeobfuscateRequirements ( const string& strFileContents, const string& strFileName, const string& strResourceName, bool bClientScript );
    void                CheckLuaSourceForIssues         ( string strLuaSource, const string& strFileName, const string& strResourceName, bool bClientScript, bool bCompiledScript, ECheckerModeType checkerMode, string* pstrOutResult = NULL );
    long                FindLuaIdentifier               ( const char* szLuaSource, long* plOutLength, long* plLineNumber );
    bool                UpgradeLuaFunctionName          ( const string& strFunctionName, bool bClientScript, string& strOutUpgraded );
    void                IssueLuaFunctionNameWarnings    ( const string& strFunctionName, const string& strFileName, const string& strResourceName, bool bClientScript, unsigned long ulLineNumber );
    ECheckerWhatType    GetLuaFunctionNameUpgradeInfo   ( const string& strFunctionName, bool bClientScript, string& strOutHow );
    int                 ReplaceFilesInZIP               ( const string& strOrigZip, const string& strTempZip, const vector < string >& pathInArchiveList, const vector < string >& upgradedFullPathList );
    bool                RenameBackupFile                ( const string& strOrigFilename, const string& strBakAppend );
    void                CheckVersionRequirements        ( const string& strIdentifierName, bool bClientScript );

    bool                m_bUpgradeScripts;
    unsigned long       m_ulDeprecatedWarningCount;
    vector < string >   m_upgradedFullPathList;
    SString             m_strReqClientVersion;
    SString             m_strReqServerVersion;
    SString             m_strReqClientReason;
    SString             m_strReqServerReason;
};


#endif
