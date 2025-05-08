/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResourceChecker.cpp
 *  PURPOSE:     Resource file content checker/validator/upgrader
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CResourceChecker.h"
#include "CResourceChecker.Data.h"
#include "CResource.h"
#include "CMainConfig.h"
#include "CGame.h"
#include "CLogger.h"
#include "CStaticFunctionDefinitions.h"
#include <core/CServerInterface.h>
#include <clocale>

#ifndef WIN32
    #include <limits.h>

    #ifndef MAX_PATH
        #define MAX_PATH PATH_MAX
    #endif
#endif

extern CNetServer*       g_pRealNetServer;
extern CServerInterface* g_pServerInterface;
extern CGame*            g_pGame;

///////////////////////////////////////////////////////////////
//
// CResourceChecker::CheckResourceForIssues
//
// Check each file and do the appropriate action
//
///////////////////////////////////////////////////////////////
void CResourceChecker::CheckResourceForIssues(CResource* pResource, const string& strResourceZip)
{
    m_strMinClientFromMetaXml = pResource->GetMinClientFromMetaXml();
    m_strMinServerFromMetaXml = pResource->GetMinServerFromMetaXml();
    m_strReqClientVersion = "";
    m_strReqServerVersion = "";
    m_strReqClientReason = "";
    m_strReqServerReason = "";

    m_ulDeprecatedWarningCount = 0;
    m_upgradedFullPathList.clear();

    // Checking certain resource client files is optional
    bool checkResourceClientFiles = g_pGame->GetConfig()->IsCheckResourceClientFilesEnabled();

    // Check each file in the resource
    std::list<CResourceFile*>::iterator iterf = pResource->IterBegin();
    for (; iterf != pResource->IterEnd(); iterf++)
    {
        CResourceFile* pResourceFile = *iterf;
        // Skip this one if validate=false in the meta.xml
        if (stricmp(pResourceFile->GetMetaFileAttribute("validate").c_str(), "false"))
        {
            string strPath;
            if (pResource->GetFilePath(pResourceFile->GetName(), strPath))
            {
                CResourceFile::eResourceType type = pResourceFile->GetType();

                bool bScript;
                bool bClient;
                if (type == CResourceFile::RESOURCE_FILE_TYPE_SCRIPT)
                {
                    bScript = true;
                    bClient = false;
                }
                else if (type == CResourceFile::RESOURCE_FILE_TYPE_CLIENT_SCRIPT)
                {
                    bScript = true;
                    bClient = true;
                }
                else if (type == CResourceFile::RESOURCE_FILE_TYPE_CLIENT_FILE && checkResourceClientFiles)
                {
                    bScript = false;
                    bClient = true;
                }
                else
                    continue;

                CheckFileForIssues(strPath, pResourceFile->GetName(), pResource->GetName(), bScript, bClient, false);
            }
        }
    }

    // Also check meta.xml
    {
        SString strFilename = "meta.xml";
        string  strPath;
        if (pResource->GetFilePath(strFilename, strPath))
        {
            CheckFileForIssues(strPath, strFilename, pResource->GetName(), false, false, true);
        }
    }

    // Output deprecated warning if required
    if (m_ulDeprecatedWarningCount)
    {
        CLogger::LogPrintf("Some files in '%s' use deprecated functions.\n", pResource->GetName().c_str());
        CLogger::LogPrintf("Use the 'upgrade' command to perform a basic upgrade of resources.\n");
    }

    // Handle upgrading a zip file if required
    if (m_upgradedFullPathList.size())
    {
        if (pResource->IsResourceZip())
        {
            string strOrigZip = strResourceZip;
            string strTempZip = strResourceZip.substr(0, strResourceZip.length() - 4) + "_tmp.zip";

            string strCacheDir = pResource->GetResourceCacheDirectoryPath();

            vector<string> pathInArchiveList;

            for (unsigned long i = 0; i < m_upgradedFullPathList.size(); i++)
            {
                string strFullPath = m_upgradedFullPathList[i];
                string strPathInArchive = strFullPath.substr(strCacheDir.length());
                pathInArchiveList.push_back(strPathInArchive);
            }

            File::Delete(strTempZip.c_str());

            if (!ReplaceFilesInZIP(strOrigZip, strTempZip, pathInArchiveList, m_upgradedFullPathList))
            {
                CLogger::LogPrintf("Failed to upgrade (ReplaceFilesInZIP) '%s'\n", strOrigZip.c_str());
            }
            else
            {
                if (!RenameBackupFile(strOrigZip, ".old"))
                {
                    CLogger::LogPrintf("Failed to upgrade (RenameBackupFile) '%s'\n", strOrigZip.c_str());
                }
                else
                {
                    if (File::Rename(strTempZip.c_str(), strOrigZip.c_str()))
                    {
                        CLogger::LogPrintf("Failed to upgrade (rename) '%s'\n", strOrigZip.c_str());
                    }
                }
            }

            File::Delete(strTempZip.c_str());
        }
    }

    // Check LC_COLLATE is correct
    if (strcoll("a", "B") < 0)
    {
        CLogger::LogPrintf("ERROR: LC_COLLATE is not set correctly\n");
    }
}

///////////////////////////////////////////////////////////////
//
// CResourceChecker::CheckFileForIssues
//
// Check one file for any issues that may need to be logged at the server.
//
///////////////////////////////////////////////////////////////
void CResourceChecker::CheckFileForIssues(const string& strPath, const string& strFileName, const string& strResourceName, bool bScript, bool bClient,
                                          bool bMeta)
{
    if (bScript)
    {
        CheckLuaFileForIssues(strPath, strFileName, strResourceName, bClient);
    }
    else if (bMeta)
    {
        CheckMetaFileForIssues(strPath, strFileName, strResourceName);
    }
    else
    {
        const char* szExt = strPath.c_str() + max<long>(0, strPath.length() - 4);

        if (stricmp(szExt, ".PNG") == 0)
        {
            CheckPngFileForIssues(strPath, strFileName, strResourceName);
        }
        else if (stricmp(szExt, ".TXD") == 0 || stricmp(szExt, ".DFF") == 0)
        {
            CheckRwFileForIssues(strPath, strFileName, strResourceName);
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CResourceChecker::CheckPngFileForIssues
//
//
//
///////////////////////////////////////////////////////////////
void CResourceChecker::CheckPngFileForIssues(const string& strPath, const string& strFileName, const string& strResourceName)
{
    bool bIsBad = false;

    // Open the file
    if (FILE* pFile = File::Fopen(strPath.c_str(), "rb"))
    {
        // This is what the png header should look like
        unsigned char pGoodHeaderPng[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
        // Also allow jpg header as the client will load misnamed png files
        unsigned char pGoodHeaderJpg[3] = {0xFF, 0xD8, 0xFF};

        // Load the header
        unsigned char pBuffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        fread(pBuffer, 1, 8, pFile);

        // Check header integrity
        if (memcmp(pBuffer, pGoodHeaderPng, 8))
            if (memcmp(pBuffer, pGoodHeaderJpg, 3))
                bIsBad = true;

        // Close the file
        fclose(pFile);
    }

    if (bIsBad)
    {
        CLogger::LogPrintf("WARNING: File '%s' in resource '%s' is invalid.\n", strFileName.c_str(), strResourceName.c_str());
    }
}

///////////////////////////////////////////////////////////////
//
// CResourceChecker::CheckRwFileForIssues
//
// Check dff and txd files
//
///////////////////////////////////////////////////////////////
void CResourceChecker::CheckRwFileForIssues(const string& strPath, const string& strFileName, const string& strResourceName)
{
    bool bIsBad = false;

    // Open the file
    if (FILE* pFile = File::Fopen(strPath.c_str(), "rb"))
    {
        struct
        {
            int id;
            int size;
            int ver;
        } header = {0, 0, 0};

        // Load the first header
        fread(&header, 1, sizeof(header), pFile);
        long pos = sizeof(header);
        long validSize = header.size + pos;

        // Step through the sections
        while (pos < validSize)
        {
            if (fread(&header, 1, sizeof(header), pFile) != sizeof(header))
                break;
            fseek(pFile, header.size, SEEK_CUR);
            pos += header.size + sizeof(header);
        }

        // Check integrity
        if (pos != validSize)
            bIsBad = true;

        // Close the file
        fclose(pFile);
    }

    if (bIsBad)
    {
        CLogger::LogPrintf("WARNING: File '%s' in resource '%s' contains errors.\n", strFileName.c_str(), strResourceName.c_str());
    }
}

///////////////////////////////////////////////////////////////
//
// CResourceChecker::CheckMetaFileForIssues
//
//
//
///////////////////////////////////////////////////////////////
void CResourceChecker::CheckMetaFileForIssues(const string& strPath, const string& strFileName, const string& strResourceName)
{
    // Load the meta file
    CXMLFile* metaFile = g_pServerInterface->GetXML()->CreateXML(strPath.c_str());
    if (!metaFile)
        return;

    CXMLNode* pRootNode = metaFile->Parse() ? metaFile->GetRootNode() : NULL;
    if (!pRootNode)
    {
        delete metaFile;
        return;
    }

    // Ouput warnings...
    if (m_bUpgradeScripts == false)
    {
        CheckMetaSourceForIssues(pRootNode, strFileName, strResourceName, ECheckerMode::WARNINGS);
    }
    else
        // ..or do an upgrade
        if (m_bUpgradeScripts == true)
    {
        bool bHasChanged = false;
        CheckMetaSourceForIssues(pRootNode, strFileName, strResourceName, ECheckerMode::UPGRADE, &bHasChanged);

        // Has contents changed?
        if (bHasChanged)
        {
            // Rename original to xml.old
            if (!RenameBackupFile(strPath, ".old"))
                return;

            // Save new content
            metaFile->Write();
            CLogger::LogPrintf("Upgrading %s:%s ...........done\n", strResourceName.c_str(), strFileName.c_str());

            m_upgradedFullPathList.push_back(strPath);
        }
    }

    delete metaFile;
}

///////////////////////////////////////////////////////////////
//
// CResourceChecker::CheckMetaSourceForIssues
//
//
//
///////////////////////////////////////////////////////////////
void CResourceChecker::CheckMetaSourceForIssues(CXMLNode* pRootNode, const string& strFileName, const string& strResourceName, ECheckerModeType checkerMode,
                                                bool* pbOutHasChanged)
{
    // Check min_mta_version is correct
    if (m_strReqClientVersion > m_strMinClientFromMetaXml || m_strReqServerVersion > m_strMinServerFromMetaXml)
    {
        // It's not right. What to do?
        if (checkerMode == ECheckerMode::WARNINGS)
        {
            SString strTemp = "<min_mta_version> section in the meta.xml is incorrect or missing (expected at least ";
            if (m_strReqClientVersion > m_strMinClientFromMetaXml)
                strTemp += SString("client %s because of '%s')", *m_strReqClientVersion, *m_strReqClientReason);
            else if (m_strReqServerVersion > m_strMinServerFromMetaXml)
                strTemp += SString("server %s because of '%s')", *m_strReqServerVersion, *m_strReqServerReason);

            CLogger::LogPrint(SString("WARNING: %s %s\n", strResourceName.c_str(), *strTemp));
        }
        else if (checkerMode == ECheckerMode::UPGRADE)
        {
            // Create min_mta_version node if required
            CXMLNode* pNodeMinMtaVersion = pRootNode->FindSubNode("min_mta_version", 0);
            if (!pNodeMinMtaVersion)
                pNodeMinMtaVersion = pRootNode->CreateSubNode("min_mta_version", pRootNode->FindSubNode("info"));

            CXMLAttributes& attributes = pNodeMinMtaVersion->GetAttributes();
            attributes.Delete("server");
            attributes.Delete("client");
            attributes.Delete("both");

            // Use "both" if client and server versions are the same
            if (!m_strReqServerVersion.empty() && !m_strReqClientVersion.empty() && m_strReqServerVersion == m_strReqClientVersion)
            {
                CXMLAttribute* pAttr = attributes.Create("both");
                pAttr->SetValue(m_strReqServerVersion);
            }
            else
            {
                if (!m_strReqServerVersion.empty())
                {
                    CXMLAttribute* pAttr = attributes.Create("server");
                    pAttr->SetValue(m_strReqServerVersion);
                }

                if (!m_strReqClientVersion.empty())
                {
                    CXMLAttribute* pAttr = attributes.Create("client");
                    pAttr->SetValue(m_strReqClientVersion);
                }
            }

            if (pbOutHasChanged)
                *pbOutHasChanged = true;
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CResourceChecker::CheckLuaFileForIssues
//
//
//
///////////////////////////////////////////////////////////////
void CResourceChecker::CheckLuaFileForIssues(const string& strPath, const string& strFileName, const string& strResourceName, bool bClientScript)
{
    // Load the original file into a string
    SString strFileContents;

    // Open the file
    FileLoad(strPath, strFileContents);
    if (strFileContents.length() == 0)
        return;

    // Update deobfuscate version requirements, and do no more checking if obfuscated
    if (CheckLuaDeobfuscateRequirements(strFileContents, strFileName, strResourceName, bClientScript))
        return;

    // Check if a compiled script
    bool bCompiledScript = IsLuaCompiledScript(strFileContents.c_str(), strFileContents.length());

    // Process
    if (strFileContents.length() > 1000000)
        CLogger::LogPrintf("Please wait...\n");

    if (m_bUpgradeScripts == false)            // Output warnings...
    {
        CheckLuaSourceForIssues(strFileContents, strFileName, strResourceName, bClientScript, bCompiledScript, ECheckerMode::WARNINGS);
    }
    else if (!bCompiledScript)            // ..or do an upgrade (if not compiled)
    {
        string strNewFileContents;
        CheckLuaSourceForIssues(strFileContents, strFileName, strResourceName, bClientScript, bCompiledScript, ECheckerMode::UPGRADE, &strNewFileContents);

        // Has contents changed?
        if (strNewFileContents.length() > 0 && strNewFileContents != strFileContents)
        {
            // Rename original to lua.old
            if (!RenameBackupFile(strPath, ".old"))
                return;

            // Save new content
            if (FILE* pFile = File::Fopen(strPath.c_str(), "wb"))
            {
                fwrite(strNewFileContents.c_str(), 1, strNewFileContents.length(), pFile);
                fclose(pFile);
                CLogger::LogPrintf("Upgrading %s:%s ...........done\n", strResourceName.c_str(), strFileName.c_str());

                m_upgradedFullPathList.push_back(strPath);
            }
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CResourceChecker::CheckLuaDeobfuscateRequirements
//
// Updates version requirements and returns true if obfuscated
//
///////////////////////////////////////////////////////////////
bool CResourceChecker::CheckLuaDeobfuscateRequirements(const string& strFileContents, const string& strFileName, const string& strResourceName,
                                                       bool bClientScript)
{
    // Get embedded version requirements
    SScriptInfo scriptInfo;
    if (!g_pRealNetServer->GetScriptInfo(strFileContents.c_str(), strFileContents.length(), &scriptInfo))
    {
        if (bClientScript && IsLuaCompiledScript(strFileContents.c_str(), strFileContents.length()))
        {
            // Compiled client script with no version info
            SString strMessage("%s is invalid. Please re-compile at https://luac.multitheftauto.com/", strFileName.c_str());
            CLogger::LogPrint(SString("ERROR: %s %s\n", strResourceName.c_str(), *strMessage));
        }
        return false;
    }

    CMtaVersion strMinServerHostVer = scriptInfo.szMinServerHostVer;
    CMtaVersion strMinServerRunVer = scriptInfo.szMinServerRunVer;
    CMtaVersion strMinClientRunVer = scriptInfo.szMinClientRunVer;

    // Check server host requirements
    if (strMinServerHostVer > m_strReqServerVersion)
    {
        m_strReqServerVersion = strMinServerHostVer;
        m_strReqServerReason = strFileName;
    }

    // Check run requirements
    if (bClientScript && strMinClientRunVer > m_strReqClientVersion)
    {
        m_strReqClientVersion = strMinClientRunVer;
        m_strReqClientReason = strFileName;
    }
    else if (!bClientScript && strMinServerRunVer > m_strReqServerVersion)
    {
        m_strReqServerVersion = strMinServerRunVer;
        m_strReqServerReason = strFileName;
    }

    return IsLuaObfuscatedScript(strFileContents.c_str(), strFileContents.length());
}

///////////////////////////////////////////////////////////////
//
// CResourceChecker::CheckLuaSourceForIssues
//
// Look for function names not in comment blocks
// Note: Ignores function calls from evaluated expressions, like `(getPlayerName)(...)`.
//
///////////////////////////////////////////////////////////////
void CResourceChecker::CheckLuaSourceForIssues(string strLuaSource, const string& strFileName, const string& strResourceName, bool bClientScript,
                                               bool bCompiledScript, ECheckerModeType checkerMode, string* pstrOutResult)
{
    // Check if this is a UTF-8 script
    bool bUTF8 = IsUTF8BOM(strLuaSource.c_str(), strLuaSource.length());

    // If it's not a UTF8 script, does it contain foreign language characters that should be upgraded?
    if (!bCompiledScript && !bUTF8 && GetUTF8Confidence((const unsigned char*)&strLuaSource.at(0), strLuaSource.length()) < 80)
    {
        std::wstring strUTF16Script = ANSIToUTF16(strLuaSource);
#ifdef WIN32
        std::setlocale(LC_CTYPE, "");            // Temporarilly use locales to read the script
        std::string strUTFScript = UTF16ToMbUTF8(strUTF16Script);
        std::setlocale(LC_CTYPE, "C");
#else
        std::string strUTFScript = UTF16ToMbUTF8(strUTF16Script);
#endif
        if (strLuaSource.length() != strUTFScript.size())
        {
            // In-place upgrade...
            if (checkerMode == ECheckerMode::UPGRADE)
            {
                // Upgrade only if there is no problem ( setlocale() issue? )
                if (strUTF16Script != L"?")
                {
                    // Convert our script to ANSI, appending a BOM at the beginning
                    strLuaSource = "\xEF\xBB\xBF" + strUTFScript;
                }
            }
            if (checkerMode == ECheckerMode::WARNINGS)
            {
                m_ulDeprecatedWarningCount++;
                CLogger::LogPrintf("WARNING: %s/%s [%s] is encoded in ANSI instead of UTF-8.  Please convert your file to UTF-8.\n", 
                                 strResourceName.c_str(), strFileName.c_str(), bClientScript ? "Client" : "Server");
            }
        }
    }

    auto begin = reinterpret_cast<const unsigned char*>(strLuaSource.data());
    auto end = begin + strLuaSource.length();
    auto current = begin;
    auto line = 1ul;

    enum class Token
    {
        NONE,
        COMMENT_SHORT,
        COMMENT_LONG,
        STRING_LONG,
    };

    auto token = Token::NONE;
    auto tokenLevel = 0;
    auto tokenLine = 0ul;
    auto tokenContent = std::string_view{};

    unsigned long    lastIdentifierLine{};
    std::string_view lastIdentifier{};
    std::string_view lastKeyword{};

    std::unordered_set<std::string> doneWarnings;

    const auto processLastIdentifier = [&]()
    {
        std::string oldName{lastIdentifier};

        if (checkerMode == ECheckerMode::UPGRADE)
        {
            assert(!bCompiledScript);

            std::string newName;

            if (UpgradeLuaFunctionName(oldName, bClientScript, newName))
            {
                const auto diff = static_cast<ptrdiff_t>(newName.length()) - static_cast<ptrdiff_t>(oldName.length());

                if (!diff)
                {
                    const size_t offset = lastIdentifier.data() - reinterpret_cast<const char*>(begin);
                    std::copy_n(newName.begin(), newName.length(), strLuaSource.data() + offset);
                }
                else
                {
                    const size_t      position = current - begin;
                    const size_t      offset = lastIdentifier.data() - reinterpret_cast<const char*>(begin);
                    const std::string tail = strLuaSource.substr(offset + lastIdentifier.length());

                    if (diff > 0)
                        strLuaSource.reserve(offset + newName.length() + tail.length());

                    strLuaSource.resize(offset);
                    strLuaSource.append(newName);
                    strLuaSource.append(tail);

                    begin = reinterpret_cast<const unsigned char*>(strLuaSource.data());
                    end = begin + strLuaSource.length();
                    current = begin + position;
                }
            }

            CheckVersionRequirements(oldName, bClientScript);
        }

        // Log warnings...
        if (checkerMode == ECheckerMode::WARNINGS)
        {
            std::string key = oldName + ":" + std::to_string(lastIdentifierLine);

            if (doneWarnings.find(key) == doneWarnings.end())
            {
                doneWarnings.insert(key);

                if (!bCompiledScript)
                {
                    IssueLuaFunctionNameWarnings(oldName, strFileName, strResourceName, bClientScript, lastIdentifierLine);
                }

                CheckVersionRequirements(oldName, bClientScript);
            }
        }

        lastIdentifierLine = {};
        lastIdentifier = {};
        lastKeyword = {};
    };

    const auto processStringLiteral = [&]()
    {
        if (tokenContent.length() > 3 && tokenContent.length() <= MAPEVENT_MAX_LENGTH_NAME)
        {
            if (tokenContent[0] == 'o' && tokenContent[1] == 'n' && tokenContent[2] >= 'A' && tokenContent[2] <= 'Z')
            {
                // A string literal that matches "^on[A-Z]" and is not longer than MAPEVENT_MAX_LENGTH_NAME, could be an event name.
                lastIdentifier = tokenContent;
                lastIdentifierLine = tokenLine;
                processLastIdentifier();
            }
        }

        tokenContent = {};
        tokenLine = {};
    };

    const auto isKeyword = [](const std::string_view keyword) -> bool
    {
        // In Lua 5.1, the shortest keyword is 2 and the longest 8 characters long.
        if (keyword.length() < 2 || keyword.length() > 8)
            return false;

        switch (keyword[0])
        {
            case 'a':
                return keyword == "and";
            case 'b':
                return keyword == "break";
            case 'd':
                return keyword == "do";
            case 'e':
                return keyword == "else" || keyword == "elseif" || keyword == "end";
            case 'f':
                return keyword == "false" || keyword == "for" || keyword == "function";
            case 'i':
                return keyword == "if" || keyword == "in";
            case 'l':
                return keyword == "local";
            case 'n':
                return keyword == "nil" || keyword == "not";
            case 'o':
                return keyword == "or";
            case 'r':
                return keyword == "repeat" || keyword == "return";
            case 't':
                return keyword == "then" || keyword == "true";
            case 'u':
                return keyword == "until";
            case 'w':
                return keyword == "while";
            default:
                break;
        }

        return false;
    };

    while (current < end)
    {
        unsigned char first = *current;
        unsigned char second = ((current + 1) < end) ? *(current + 1) : '\0';

        // Handle new line characters.
        if (first == '\r' && second == '\n')
        {
            current += 2;
            line += 1;
            if (token == Token::COMMENT_SHORT)
                token = Token::NONE;
            continue;
        }
        if (first == '\n' || first == '\r')
        {
            current += 1;
            line += 1;
            if (token == Token::COMMENT_SHORT)
                token = Token::NONE;
            continue;
        }

        // Ignore whitespace.
        if (isspace(static_cast<int>(first)))
        {
            current += isspace(static_cast<int>(second)) ? 2 : 1;
            continue;
        }

        // Handle continuing short comments.
        if (token == Token::COMMENT_SHORT)
        {
            current += 1;
            continue;
        }

        // Handle ending both long comments and long string literals.
        if (token == Token::COMMENT_LONG || token == Token::STRING_LONG)
        {
            if (first == ']' && second == ']' && tokenLevel == 0)
            {
                // Ending one of:
                // - a long comment        of level 0: "--[[ ... ]]"
                // - a long string literal of level 0:   "[[ ... ]]"

                if (token == Token::STRING_LONG)
                {
                    const size_t length = reinterpret_cast<const char*>(current) - tokenContent.data();
                    tokenContent = std::string_view{tokenContent.data(), length};
                    processStringLiteral();
                }

                token = Token::NONE;
                current += 2;
                continue;
            }

            if (first == ']' && second == '=' && tokenLevel > 0)            // Maybe close a long comment of level 1 and above: "]=]" or "]==]" or ...
            {
                const unsigned char* start = current + 1;
                const unsigned char* equals = start + 1;
                Token                before = token;

                while (equals < end)
                {
                    if (*equals == '=')
                    {
                        equals += 1;
                        continue;
                    }

                    if (*equals == ']')
                    {
                        if (tokenLevel == (equals - start))
                        {
                            token = Token::NONE;
                            tokenLevel = 0;
                        }

                        equals += 1;
                    }

                    break;
                }

                // Ending one of:
                // - a long comment        of level 1 and above: "--[=[ ... ]=]" or "--[==[ ... ]==]" or ...
                // - a long string literal of level 1 and above:   "[=[ ... ]=]" or   "[==[ ... ]==]" or ...
                if (token == Token::NONE && before == Token::STRING_LONG)
                {
                    const size_t length = reinterpret_cast<const char*>(current) - tokenContent.data();
                    tokenContent = std::string_view{tokenContent.data(), length};
                    processStringLiteral();
                }

                current = equals;
                continue;
            }

            current += 1;
            continue;
        }

        // Handle starting comments.
        if (first == '-' && second == '-')
        {
            current += 2;
            token = Token::COMMENT_SHORT;
            tokenLevel = 0;
            tokenLine = line;
            // We handle long comments in the block below.
            first = (current < end) ? *current : '\0';
            second = ((current + 1) < end) ? *(current + 1) : '\0';
        }

        // Handle both starting long comments and starting long string literals.
        if (first == '[' && second == '[')
        {
            // Opening one of:
            // - a long comment        of level 0: "--[[ ... ]]"
            // - a long string literal of level 0:   "[[ ... ]]"
            token = (token == Token::COMMENT_SHORT) ? Token::COMMENT_LONG : Token::STRING_LONG;
            tokenLine = line;
            tokenContent = std::string_view{reinterpret_cast<const char*>(current + 2), 0};
            current += 2;

            if (token == Token::STRING_LONG && !lastIdentifier.empty())
            {
                // We are calling a function with a string: foo [[...]]
                processLastIdentifier();
            }
            continue;
        }
        else if (first == '[' && second == '=')
        {
            const unsigned char* start = current + 1;
            const unsigned char* equals = start + 1;

            while (equals < end)
            {
                if (*equals == '=')
                {
                    equals += 1;
                    continue;
                }

                if (*equals == '[')
                {
                    // Opening one of:
                    // - a long comment        of level 1 and above: "--[=[ ... ]=]" or "--[==[ ... ]==]" or ...
                    // - a long string literal of level 1 and above:   "[=[ ... ]=]" or   "[==[ ... ]==]" or ...
                    token = (token == Token::COMMENT_SHORT) ? Token::COMMENT_LONG : Token::STRING_LONG;
                    tokenLevel = equals - start;
                    tokenLine = line;
                    tokenContent = std::string_view{reinterpret_cast<const char*>(equals + 1), 0};
                    equals += 1;
                }

                break;
            }

            if (token == Token::STRING_LONG && !lastIdentifier.empty())
            {
                // We are calling a function with a string: foo [===[...]===]
                processLastIdentifier();
            }

            current = equals;
            continue;
        }
        else if (token == Token::COMMENT_SHORT)
        {
            // Unfortunately, we didn't start a long comment.
            tokenContent = std::string_view{reinterpret_cast<const char*>(current), 0};
            continue;
        }

        // Handle short string literals.
        if (first == '"' || first == '\'')
        {
            if (!lastIdentifier.empty())
            {
                // We are calling a function with a string: foo "..."  or  foo '...'
                processLastIdentifier();
            }

            const unsigned char* pos = current + 1;

            while (pos < end)
            {
                if (*pos == '\\' && (pos + 1) < end)
                {
                    pos += 2;
                    continue;
                }

                if (*pos == '\n')
                {
                    // Line breaks are not allowed in string literals, but the script might contain errors.
                    break;
                }

                if (*pos != first)
                {
                    pos += 1;
                    continue;
                }

                pos += 1;
                break;
            }

            if (const size_t length = pos - current; length > 2)
            {
                std::string_view literal(reinterpret_cast<const char*>(current + 1), length - 2);            // Do not include surrounding quotes.
                tokenContent = literal;
                tokenLine = line;
                processStringLiteral();
            }

            current = pos;
            continue;
        }

        // Handle identifiers.
        if (isalpha(static_cast<int>(first)) || first == '_')
        {
            const unsigned char* pos = current + 1;

            while (pos < end)
            {
                if (isalnum(*pos) || *pos == '_' || *pos == '.')
                {
                    pos += 1;
                    continue;
                }

                break;
            }

            std::string_view identifier(reinterpret_cast<const char*>(current), pos - current);

            if (isKeyword(identifier))
            {
                lastKeyword = identifier;
                lastIdentifierLine = {};
                lastIdentifier = {};
            }
            else
            {
                lastIdentifierLine = line;
                lastIdentifier = identifier;
            }

            current = pos;
            continue;
        }

        // We want to keep the last keyword, if we define/call table functions with syntax sugar: function foo:bar() ... end
        if (first == ':' && !lastKeyword.empty())
        {
            current += 1;
            continue;
        }
        
        // We are calling a function with several arguments or with a table: foo (...)  or  foo {...}
        if ((first == '(' && lastKeyword != "function") || first == '{')
        {
            if (!lastIdentifier.empty())
                processLastIdentifier();
        }

        lastIdentifierLine = {};
        lastIdentifier = {};
        lastKeyword = {};
        current += 1;
    }

    if (pstrOutResult)
        *pstrOutResult = strLuaSource;
}

///////////////////////////////////////////////////////////////
//
// CResourceChecker::UpgradeLuaFunctionName
//
//
//
///////////////////////////////////////////////////////////////
bool CResourceChecker::UpgradeLuaFunctionName(const string& strFunctionName, bool bClientScript, string& strOutUpgraded)
{
    string           strHow;
    CMtaVersion      strVersion;
    ECheckerWhatType what = GetLuaFunctionNameUpgradeInfo(strFunctionName, bClientScript, strHow, strVersion);

    if (what == ECheckerWhat::REPLACED)
    {
        strOutUpgraded = strHow;
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////
//
// CResourceChecker::IssueLuaFunctionNameWarnings
//
//
//
///////////////////////////////////////////////////////////////
void CResourceChecker::IssueLuaFunctionNameWarnings(const string& strFunctionName, const string& strFileName, const string& strResourceName, bool bClientScript,
                                                    unsigned long ulLineNumber)
{
    string           strHow;
    CMtaVersion      strVersion;
    ECheckerWhatType what = GetLuaFunctionNameUpgradeInfo(strFunctionName, bClientScript, strHow, strVersion);
    if (what == ECheckerWhat::NONE)
        return;

    SString strTemp;
    if (what == ECheckerWhat::REPLACED)
    {
        m_ulDeprecatedWarningCount++;
        strTemp.Format("%s is deprecated and may not work in future versions. Please replace with %s%s.", strFunctionName.c_str(), strHow.c_str(),
                       (GetTickCount32() / 60000) % 7 ? "" : " before Tuesday");
    }
    else if (what == ECheckerWhat::REMOVED)
    {
        strTemp.Format("%s no longer works. %s", strFunctionName.c_str(), strHow.c_str());
    }
    else if (what == ECheckerWhat::MODIFIED)
    {
        strTemp.Format("%s %s because <min_mta_version> %s setting in meta.xml is below %s", strFunctionName.c_str(), strHow.c_str(),
                       bClientScript ? "Client" : "Server", strVersion.c_str());
    }
    CLogger::LogPrint(SString("WARNING: %s/%s(Line %lu) [%s] %s\n", strResourceName.c_str(), strFileName.c_str(), ulLineNumber,
                              bClientScript ? "Client" : "Server", *strTemp));
}

///////////////////////////////////////////////////////////////
//
// CResourceChecker::GetLuaFunctionNameUpgradeInfo
//
//
//
///////////////////////////////////////////////////////////////
ECheckerWhatType CResourceChecker::GetLuaFunctionNameUpgradeInfo(const string& strFunctionName, bool bClientScript, string& strOutHow,
                                                                 CMtaVersion& strOutVersion)
{
    static CHashMap<SString, SDeprecatedItem*> clientUpgradeInfoMap;
    static CHashMap<SString, SDeprecatedItem*> serverUpgradeInfoMap;
    if (clientUpgradeInfoMap.size() == 0)
    {
        // Make maps to speed things up
        for (uint i = 0; i < NUMELMS(clientDeprecatedList); i++)
            clientUpgradeInfoMap[clientDeprecatedList[i].strOldName] = &clientDeprecatedList[i];
        for (uint i = 0; i < NUMELMS(serverDeprecatedList); i++)
            serverUpgradeInfoMap[serverDeprecatedList[i].strOldName] = &serverDeprecatedList[i];
    }

    // Query the correct map with the cleaned function name
    SDeprecatedItem* pItem = MapFindRef(bClientScript ? clientUpgradeInfoMap : serverUpgradeInfoMap, strFunctionName);
    if (!pItem)
        return ECheckerWhat::NONE;            // Nothing found

    strOutHow = pItem->strNewName;
    strOutVersion = pItem->strVersion;
    if (!strOutVersion.empty())
    {
        // Function behaviour depends on min_mta_version setting
        const CMtaVersion& strMinFromMetaXml = bClientScript ? m_strMinClientFromMetaXml : m_strMinServerFromMetaXml;
        if (strMinFromMetaXml < strOutVersion)
            return ECheckerWhat::MODIFIED;
        return ECheckerWhat::NONE;
    }
    return pItem->bRemoved ? ECheckerWhat::REMOVED : ECheckerWhat::REPLACED;
}

///////////////////////////////////////////////////////////////
//
// CResourceChecker::CheckVersionRequirements
//
// Update m_strReqClientVersion or m_strReqServerVersion with the version requirement for the
// supplied identifier
//
///////////////////////////////////////////////////////////////
void CResourceChecker::CheckVersionRequirements(const string& strIdentifierName, bool bClientScript)
{
    static CHashMap<SString, CMtaVersion> clientFunctionMap;
    static CHashMap<SString, CMtaVersion> serverFunctionMap;

    // Check if lookup maps need initializing
    if (clientFunctionMap.empty())
    {
        for (uint i = 0; i < NUMELMS(clientFunctionInitList); i++)
            MapSet(clientFunctionMap, clientFunctionInitList[i].functionName, CMtaVersion(clientFunctionInitList[i].minMtaVersion));

        for (uint i = 0; i < NUMELMS(serverFunctionInitList); i++)
            MapSet(serverFunctionMap, serverFunctionInitList[i].functionName, CMtaVersion(serverFunctionInitList[i].minMtaVersion));
    }

    // Select client or server check
    const CHashMap<SString, CMtaVersion>& functionMap = bClientScript ? clientFunctionMap : serverFunctionMap;
    CMtaVersion&                          strReqMtaVersion = bClientScript ? m_strReqClientVersion : m_strReqServerVersion;
    SString&                              strReqMtaReason = bClientScript ? m_strReqClientReason : m_strReqServerReason;

    const CMtaVersion* pResult = MapFind(functionMap, strIdentifierName);
    if (pResult)
    {
        // This identifier has a version requirement
        const CMtaVersion& strResult = *pResult;

        // Is the new requirement relevant for this MTA generation
        if (strResult.GetGeneration() >= CStaticFunctionDefinitions::GetVersionSortable().GetGeneration())
        {
            // Is the new requirement higher than the current?
            if (strResult > strReqMtaVersion)
            {
                strReqMtaVersion = strResult;
                strReqMtaReason = strIdentifierName;
            }
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CResourceChecker::RenameBackupFile
//
// Rename a file for backup purposes. Creates a unique name if required.
//
///////////////////////////////////////////////////////////////
bool CResourceChecker::RenameBackupFile(const string& strOrigFilename, const string& strBakAppend)
{
    string strBakFilename = strOrigFilename + strBakAppend;
    for (int i = 0; File::Rename(strOrigFilename.c_str(), strBakFilename.c_str()); i++)
    {
        if (i > 1000)
        {
            CLogger::LogPrintf("Unable to rename %s to %s\n", strOrigFilename.c_str(), strBakFilename.c_str());
            return false;
        }
        strBakFilename = strOrigFilename + strBakAppend + "_" + SString("%d", i + 1);
    }
    return true;
}

///////////////////////////////////////////////////////////////
//
// CResourceChecker::ReplaceFilesInZIP
//
// Based on example at https://www.winimage.com/zLibDll/minizip.html
// by Ivan A. Krestinin
//
///////////////////////////////////////////////////////////////
int CResourceChecker::ReplaceFilesInZIP(const string& strOrigZip, const string& strTempZip, const vector<string>& pathInArchiveList,
                                        const vector<string>& m_upgradedFullPathList)
{
    // open source and destination file
    zlib_filefunc_def ffunc;
    #ifdef WIN32
    fill_win32_filefunc(&ffunc);
    #else
    fill_fopen_filefunc(&ffunc);
    #endif

    zipFile szip = unzOpen2(strOrigZip.c_str(), &ffunc);
    if (szip == NULL)
    { /*free(tmp_name);*/
        return 0;
    }
    zipFile dzip = zipOpen2(strTempZip.c_str(), APPEND_STATUS_CREATE, NULL, &ffunc);
    if (dzip == NULL)
    {
        unzClose(szip); /*free(tmp_name);*/
        return 0;
    }

    // get global commentary
    unz_global_info glob_info;
    if (unzGetGlobalInfo(szip, &glob_info) != UNZ_OK)
    {
        zipClose(dzip, NULL);
        unzClose(szip); /*free(tmp_name);*/
        return 0;
    }

    char* glob_comment = NULL;
    if (glob_info.size_comment > 0)
    {
        glob_comment = (char*)malloc(glob_info.size_comment + 1);
        if ((glob_comment == NULL) && (glob_info.size_comment != 0))
        {
            zipClose(dzip, NULL);
            unzClose(szip); /*free(tmp_name);*/
            return 0;
        }

        if ((unsigned int)unzGetGlobalComment(szip, glob_comment, glob_info.size_comment + 1) != glob_info.size_comment)
        {
            zipClose(dzip, NULL);
            unzClose(szip);
            free(glob_comment); /*free(tmp_name);*/
            return 0;
        }
    }

    // copying files
    int n_files = 0;

    int rv = unzGoToFirstFile(szip);
    while (rv == UNZ_OK)
    {
        // get zipped file info
        unz_file_info unzfi;
        char          dos_fn[MAX_PATH];
        if (unzGetCurrentFileInfo(szip, &unzfi, dos_fn, MAX_PATH, NULL, 0, NULL, 0) != UNZ_OK)
            break;
        char fn[MAX_PATH];
        #ifdef WIN32
        OemToChar(dos_fn, fn);
        #endif

        // See if file should be replaced
        string fullPathReplacement;
        for (unsigned long i = 0; i < pathInArchiveList.size(); i++)
            if (stricmp(fn, pathInArchiveList[i].c_str()) == 0)
                fullPathReplacement = m_upgradedFullPathList[i];

        // Replace file in zip
        if (fullPathReplacement.length())
        {
            void*         buf = NULL;
            unsigned long ulLength = 0;

            // Get new file into a buffer
            if (FILE* pFile = File::Fopen(fullPathReplacement.c_str(), "rb"))
            {
                // Get the file size,
                fseek(pFile, 0, SEEK_END);
                ulLength = ftell(pFile);
                fseek(pFile, 0, SEEK_SET);

                // Load file into a buffer
                buf = malloc(ulLength);
                if (fread(buf, 1, ulLength, pFile) != ulLength)
                {
                    free(buf);
                    buf = NULL;
                }

                // Clean up
                fclose(pFile);
            }

            if (!buf)
                break;

            // open destination file
            zip_fileinfo zfi;
            memcpy(&zfi.tmz_date, &unzfi.tmu_date, sizeof(tm_unz));
            zfi.dosDate = unzfi.dosDate;
            zfi.internal_fa = unzfi.internal_fa;
            zfi.external_fa = unzfi.external_fa;

            char* extrafield = NULL;
            char* commentary = NULL;
            int   size_local_extra = 0;
            void* local_extra = NULL;
            int   unzfi_size_file_extra = 0;
            int   method = Z_DEFLATED;
            int   level = Z_DEFAULT_COMPRESSION;

            if (zipOpenNewFileInZip(dzip, dos_fn, &zfi, local_extra, size_local_extra, extrafield, unzfi_size_file_extra, commentary, method, level) != UNZ_OK)
            {
                free(extrafield);
                free(commentary);
                free(local_extra);
                free(buf);
                break;
            }

            // write file
            if (zipWriteInFileInZip(dzip, buf, ulLength) != UNZ_OK)
            {
                free(extrafield);
                free(commentary);
                free(local_extra);
                free(buf);
                break;
            }

            if (zipCloseFileInZip(dzip /*, unzfi.uncompressed_size, unzfi.crc*/) != UNZ_OK)
            {
                free(extrafield);
                free(commentary);
                free(local_extra);
                free(buf);
                break;
            }

            free(buf);
        }

        // Copy file in zip
        if (!fullPathReplacement.length())
        {
            char* extrafield = (char*)malloc(unzfi.size_file_extra);
            if ((extrafield == NULL) && (unzfi.size_file_extra != 0))
                break;
            char* commentary = (char*)malloc(unzfi.size_file_comment);
            if ((commentary == NULL) && (unzfi.size_file_comment != 0))
            {
                free(extrafield);
                break;
            }

            if (unzGetCurrentFileInfo(szip, &unzfi, dos_fn, MAX_PATH, extrafield, unzfi.size_file_extra, commentary, unzfi.size_file_comment) != UNZ_OK)
            {
                free(extrafield);
                free(commentary);
                break;
            }

            // open file for RAW reading
            int method;
            int level;
            if (unzOpenCurrentFile2(szip, &method, &level, 1) != UNZ_OK)
            {
                free(extrafield);
                free(commentary);
                break;
            }

            int size_local_extra = unzGetLocalExtrafield(szip, NULL, 0);
            if (size_local_extra < 0)
            {
                free(extrafield);
                free(commentary);
                break;
            }
            void* local_extra = malloc(size_local_extra);
            if ((local_extra == NULL) && (size_local_extra != 0))
            {
                free(extrafield);
                free(commentary);
                break;
            }
            if (unzGetLocalExtrafield(szip, local_extra, size_local_extra) < 0)
            {
                free(extrafield);
                free(commentary);
                free(local_extra);
                break;
            }

            // this malloc may fail if file very large
            void* buf = malloc(unzfi.compressed_size);
            if ((buf == NULL) && (unzfi.compressed_size != 0))
            {
                free(extrafield);
                free(commentary);
                free(local_extra);
                break;
            }

            // read file
            int sz = unzReadCurrentFile(szip, buf, unzfi.compressed_size);
            if ((unsigned int)sz != unzfi.compressed_size)
            {
                free(extrafield);
                free(commentary);
                free(local_extra);
                free(buf);
                break;
            }

            // open destination file
            zip_fileinfo zfi;
            memcpy(&zfi.tmz_date, &unzfi.tmu_date, sizeof(tm_unz));
            zfi.dosDate = unzfi.dosDate;
            zfi.internal_fa = unzfi.internal_fa;
            zfi.external_fa = unzfi.external_fa;

            if (zipOpenNewFileInZip2(dzip, dos_fn, &zfi, local_extra, size_local_extra, extrafield, unzfi.size_file_extra, commentary, method, level, 1) !=
                UNZ_OK)
            {
                free(extrafield);
                free(commentary);
                free(local_extra);
                free(buf);
                break;
            }

            // write file
            if (zipWriteInFileInZip(dzip, buf, unzfi.compressed_size) != UNZ_OK)
            {
                free(extrafield);
                free(commentary);
                free(local_extra);
                free(buf);
                break;
            }

            if (zipCloseFileInZipRaw(dzip, unzfi.uncompressed_size, unzfi.crc) != UNZ_OK)
            {
                free(extrafield);
                free(commentary);
                free(local_extra);
                free(buf);
                break;
            }

            if (unzCloseCurrentFile(szip) == UNZ_CRCERROR)
            {
                free(extrafield);
                free(commentary);
                free(local_extra);
                free(buf);
                break;
            }
            free(commentary);
            free(buf);
            free(extrafield);
            free(local_extra);

            n_files++;
        }

        rv = unzGoToNextFile(szip);
    }

    zipClose(dzip, glob_comment);
    unzClose(szip);

    free(glob_comment);

    return rv == UNZ_END_OF_LIST_OF_FILE;
}

///////////////////////////////////////////////////////////////
//
// CResourceChecker::LogUpgradeWarnings
//
// Also calculates version requirements these days
//
///////////////////////////////////////////////////////////////
void CResourceChecker::LogUpgradeWarnings(CResource* pResource, const string& strResourceZip, CMtaVersion& strOutReqClientVersion,
                                          CMtaVersion& strOutReqServerVersion, SString& strOutReqClientReason, SString& strOutReqServerReason)
{
    m_bUpgradeScripts = false;
    CheckResourceForIssues(pResource, strResourceZip);
    strOutReqClientVersion = m_strReqClientVersion;
    strOutReqServerVersion = m_strReqServerVersion;
    strOutReqClientReason = m_strReqClientReason;
    strOutReqServerReason = m_strReqServerReason;
}

///////////////////////////////////////////////////////////////
//
// CResourceChecker::ApplyUpgradeModifications
//
//
//
///////////////////////////////////////////////////////////////
void CResourceChecker::ApplyUpgradeModifications(CResource* pResource, const string& strResourceZip)
{
    m_bUpgradeScripts = true;
    CheckResourceForIssues(pResource, strResourceZip);
}
