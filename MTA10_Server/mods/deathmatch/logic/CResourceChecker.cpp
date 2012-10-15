/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceChecker.cpp
*  PURPOSE:     Resource file content checker/validator/upgrader
*  DEVELOPERS:  Developer wishes to remain anonymous
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CResourceChecker.Data.h"
#include <clocale>

///////////////////////////////////////////////////////////////
//
// CResourceChecker::CheckResourceForIssues
//
// Check each file and do the appropriate action
//
///////////////////////////////////////////////////////////////
void CResourceChecker::CheckResourceForIssues( CResource* pResource, const string& strResourceZip )
{
    m_strReqClientVersion = "";
    m_strReqServerVersion = "";
    m_strReqClientReason = "";
    m_strReqServerReason = "";

    m_ulDeprecatedWarningCount = 0;
    m_upgradedFullPathList.clear ();

    // Check each file in the resource
    list < CResourceFile* > ::iterator iterf = pResource->IterBegin ();
    for ( ; iterf != pResource->IterEnd (); iterf++ )
    {
        CResourceFile* pResourceFile = *iterf;
        // Skip this one if validate=false in the meta.xml
        if ( stricmp( pResourceFile->GetMetaFileAttribute ( "validate" ).c_str (), "false" ) )
        {
            string strPath;
            if ( pResource->GetFilePath ( pResourceFile->GetName(), strPath ) )
            {
                CResourceFile::eResourceType type = pResourceFile->GetType ();

                bool bScript;
                bool bClient;
                if ( type == CResourceFile::RESOURCE_FILE_TYPE_SCRIPT )
                {
                    bScript = true;
                    bClient = false;
                }
                else
                if ( type == CResourceFile::RESOURCE_FILE_TYPE_CLIENT_SCRIPT )
                {
                    bScript = true;
                    bClient = true;
                }
                else
                if ( type == CResourceFile::RESOURCE_FILE_TYPE_CLIENT_FILE )
                {
                    bScript = false;
                    bClient = true;
                }
                else
                    continue;

                CheckFileForIssues ( strPath, pResourceFile->GetName(), pResource->GetName (), bScript, bClient, false );
            }
        }
    }

    // Also check meta.xml
    {
        SString strFilename = "meta.xml";
        string strPath;
        if ( pResource->GetFilePath ( strFilename, strPath ) )
        {
            CheckFileForIssues ( strPath, strFilename, pResource->GetName (), false, false, true );
        }
    }

    // Output deprecated warning if required
    if ( m_ulDeprecatedWarningCount )
    {
        CLogger::LogPrintf ( "Some files in '%s' use deprecated functions.\n", pResource->GetName ().c_str () );
        CLogger::LogPrintf ( "Use the 'upgrade' command to perform a basic upgrade of resources.\n" );
    }

    // Handle upgrading a zip file if required
    if ( m_upgradedFullPathList.size () )
    {
        if ( pResource->IsResourceZip () )
        {
            string strOrigZip = strResourceZip;
            string strTempZip = strResourceZip.substr ( 0, strResourceZip.length () - 4 ) + "_tmp.zip";

            string strCacheDir = pResource->GetResourceCacheDirectoryPath ();

            vector < string > pathInArchiveList;

            for ( unsigned long i = 0 ; i < m_upgradedFullPathList.size () ; i++ )
            {
                string strFullPath = m_upgradedFullPathList[i];
                string strPathInArchive = strFullPath.substr ( strCacheDir.length () );
                pathInArchiveList.push_back ( strPathInArchive );
            }

            remove( strTempZip.c_str () );

            if ( !ReplaceFilesInZIP( strOrigZip, strTempZip, pathInArchiveList, m_upgradedFullPathList ) )
            {
                CLogger::LogPrintf ( "Failed to upgrade (ReplaceFilesInZIP) '%s'\n", strOrigZip.c_str () );
            }
            else
            {
                if ( !RenameBackupFile( strOrigZip, ".old" ) )
                {
                    CLogger::LogPrintf ( "Failed to upgrade (RenameBackupFile) '%s'\n", strOrigZip.c_str () );
                }
                else
                {
                    if ( rename( strTempZip.c_str (), strOrigZip.c_str () ) )
                    {
                        CLogger::LogPrintf ( "Failed to upgrade (rename) '%s'\n", strOrigZip.c_str () );
                    }
                }
            }

            remove( strTempZip.c_str () );
        }
    }

    // Check LC_COLLATE is correct
    if ( strcoll( "a", "B" ) < 0 )
    {
        CLogger::LogPrintf ( "ERROR: LC_COLLATE is not set correctly\n" );
    }
}


///////////////////////////////////////////////////////////////
//
// CResourceChecker::CheckFileForIssues
//
// Check one file for any issues that may need to be logged at the server.
//
///////////////////////////////////////////////////////////////
void CResourceChecker::CheckFileForIssues ( const string& strPath, const string& strFileName, const string& strResourceName, bool bScript, bool bClient, bool bMeta )
{
    if ( bScript )
    {
        CheckLuaFileForIssues( strPath, strFileName, strResourceName, bClient );
    }
    else
    if ( bMeta )
    {
        CheckMetaFileForIssues( strPath, strFileName, strResourceName );
    }
    else
    {
        const char* szExt = strPath.c_str () + max < long > ( 0, strPath.length () - 4 );

        if ( stricmp ( szExt, ".PNG" ) == 0 )
        {
            CheckPngFileForIssues( strPath, strFileName, strResourceName );
        }
        else
        if ( stricmp ( szExt, ".TXD" ) == 0 || stricmp ( szExt, ".DFF" ) == 0 )
        {
            CheckRwFileForIssues( strPath, strFileName, strResourceName );
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
void CResourceChecker::CheckPngFileForIssues ( const string& strPath, const string& strFileName, const string& strResourceName )
{
    bool bIsBad         = false;

    // Open the file
    if ( FILE* pFile = fopen ( strPath.c_str (), "rb" ) )
    {
        // This is what the png header should look like
        unsigned char pGoodHeader [8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };

            // Load the header
        unsigned char pBuffer [8] = { 0,0,0,0,0,0,0,0 };
        fread ( pBuffer, 1, 8, pFile );

        // Check header integrity
        if ( memcmp ( pBuffer, pGoodHeader, 8 ) )
            bIsBad = true;

        // Close the file
        fclose ( pFile );
    }

    if ( bIsBad )
    {
        CLogger::LogPrintf ( "WARNING: File '%s' in resource '%s' is invalid.\n", strFileName.c_str () , strResourceName.c_str () );
    }
}


///////////////////////////////////////////////////////////////
//
// CResourceChecker::CheckRwFileForIssues
//
// Check dff and txd files
//
///////////////////////////////////////////////////////////////
void CResourceChecker::CheckRwFileForIssues ( const string& strPath, const string& strFileName, const string& strResourceName )
{
    bool bIsBad         = false;

    // Open the file
    if ( FILE* pFile = fopen ( strPath.c_str (), "rb" ) )
    {
        struct {
            long id;
            long size;
            long ver;
        } header = {0,0,0};

        // Load the first header
        fread ( &header, 1, sizeof(header), pFile );
        long pos = sizeof(header);
        long validSize = header.size + pos;

        // Step through the sections
        while ( pos < validSize )
        {
            if ( fread ( &header, 1, sizeof(header), pFile ) != sizeof(header) )
                break;
            fseek ( pFile, header.size, SEEK_CUR );
            pos += header.size + sizeof(header);
        }

        // Check integrity
        if ( pos != validSize )
            bIsBad = true;

        // Close the file
        fclose ( pFile );
    }

    if ( bIsBad )
    {
        CLogger::LogPrintf ( "WARNING: File '%s' in resource '%s' is invalid.\n", strFileName.c_str () , strResourceName.c_str () );
    }
}


///////////////////////////////////////////////////////////////
//
// CResourceChecker::CheckMetaFileForIssues
//
//
//
///////////////////////////////////////////////////////////////
void CResourceChecker::CheckMetaFileForIssues ( const string& strPath, const string& strFileName, const string& strResourceName )
{
    // Load the meta file
    CXMLFile* metaFile = g_pServerInterface->GetXML ()->CreateXML ( strPath.c_str() );
    if ( !metaFile )
        return;

    CXMLNode* pRootNode = metaFile->Parse () ? metaFile->GetRootNode () : NULL;
    if ( !pRootNode )
    {
        delete metaFile;
        return;
    }

    // Ouput warnings...
    if ( m_bUpgradeScripts == false )
    {
        CheckMetaSourceForIssues ( pRootNode, strFileName, strResourceName, ECheckerMode::WARNINGS );
    }
    else
    // ..or do an upgrade
    if ( m_bUpgradeScripts == true )
    {
        bool bHasChanged = false;
        CheckMetaSourceForIssues ( pRootNode, strFileName, strResourceName, ECheckerMode::UPGRADE, &bHasChanged );

        // Has contents changed?
        if ( bHasChanged )
        {
            // Rename original to xml.old
            if( !RenameBackupFile( strPath, ".old" ) )
                return;

            // Save new content
            metaFile->Write ();
            CLogger::LogPrintf ( "Upgrading %s:%s ...........done\n", strResourceName.c_str (), strFileName.c_str () );

            m_upgradedFullPathList.push_back( strPath );
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
void CResourceChecker::CheckMetaSourceForIssues ( CXMLNode* pRootNode, const string& strFileName, const string& strResourceName, ECheckerModeType checkerMode, bool* pbOutHasChanged )
{
    // Find the client and server version requirements
    SString strMinClientReqFromMetaXml = "";
    SString strMinServerReqFromMetaXml = "";
    CXMLNode* pNodeMinMtaVersion = pRootNode->FindSubNode ( "min_mta_version", 0 );

    if ( pNodeMinMtaVersion )
    {
        if ( CXMLAttribute* pAttr = pNodeMinMtaVersion->GetAttributes ().Find ( "server" ) )
            strMinServerReqFromMetaXml = pAttr->GetValue ();
        if ( CXMLAttribute* pAttr = pNodeMinMtaVersion->GetAttributes ().Find ( "client" ) )
            strMinClientReqFromMetaXml = pAttr->GetValue ();
        if ( CXMLAttribute* pAttr = pNodeMinMtaVersion->GetAttributes ().Find ( "both" ) )
            strMinServerReqFromMetaXml = strMinClientReqFromMetaXml = pAttr->GetValue ();
    }

    // Is it right?
    if ( m_strReqClientVersion > strMinClientReqFromMetaXml || m_strReqServerVersion > strMinServerReqFromMetaXml )
    {
        // It's not right. What to do?
        if ( checkerMode == ECheckerMode::WARNINGS )
        {
            SString strTemp = "<min_mta_version> section in the meta.xml is incorrect or missing (expected at least ";
            if ( m_strReqClientVersion > strMinClientReqFromMetaXml )
                strTemp += SString ( "client %s because of '%s')", *m_strReqClientVersion, *m_strReqClientReason );
            else
            if ( m_strReqServerVersion > strMinServerReqFromMetaXml )
                strTemp += SString ( "server %s because of '%s')", *m_strReqServerVersion, *m_strReqServerReason );

            CLogger::LogPrint ( SString ( "WARNING: %s %s\n", strResourceName.c_str (), *strTemp ) );
        }
        else
        if ( checkerMode == ECheckerMode::UPGRADE )
        {
            // Create min_mta_version node if required
            if ( !pNodeMinMtaVersion )
                pNodeMinMtaVersion = pRootNode->CreateSubNode ( "min_mta_version" );

            CXMLAttributes& attributes = pNodeMinMtaVersion->GetAttributes ();
            attributes.Delete ( "server" );
            attributes.Delete ( "client" );
            attributes.Delete ( "both" );

            if ( !m_strReqServerVersion.empty () )
            {
                CXMLAttribute* pAttr = attributes.Create ( "server" );
                pAttr->SetValue ( m_strReqServerVersion );
            }

            if ( !m_strReqClientVersion.empty () )
            {
                CXMLAttribute* pAttr = attributes.Create ( "client" );
                pAttr->SetValue ( m_strReqClientVersion );
            }

            if ( pbOutHasChanged )
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
void CResourceChecker::CheckLuaFileForIssues ( const string& strPath, const string& strFileName, const string& strResourceName, bool bClientScript )
{
    // Load the original file into a string
    SString strFileContents;

    // Open the file
    FileLoad ( strPath, strFileContents );
    if ( strFileContents.length () == 0 )
        return;

    // Check if a compiled script
    bool bCompiledScript = ( strncmp ( "\x1B\x4C\x75\x61", strFileContents, 4 ) == 0 );

    // Process
    if ( strFileContents.length () > 1000000 )
        CLogger::LogPrintf ( "Please wait...\n" );

    // Ouput warnings...
    if ( m_bUpgradeScripts == false )
    {
        CheckLuaSourceForIssues ( strFileContents, strFileName, strResourceName, bClientScript, bCompiledScript, ECheckerMode::WARNINGS );
    }
    else
    // ..or do an upgrade (if not compiled)
    if ( m_bUpgradeScripts == true && !bCompiledScript )
    {
        string strNewFileContents;
        CheckLuaSourceForIssues ( strFileContents, strFileName, strResourceName, bClientScript, bCompiledScript, ECheckerMode::UPGRADE, &strNewFileContents );

        // Has contents changed?
        if ( strNewFileContents.length () > 0 && strNewFileContents != strFileContents )
        {
            // Rename original to lua.old
            if( !RenameBackupFile( strPath, ".old" ) )
                return;

            // Save new content
            if ( FILE* pFile = fopen ( strPath.c_str (), "wb" ) )
            {
                fwrite ( strNewFileContents.c_str (), 1, strNewFileContents.length (), pFile );
                fclose ( pFile );
                CLogger::LogPrintf ( "Upgrading %s:%s ...........done\n", strResourceName.c_str (), strFileName.c_str () );

                m_upgradedFullPathList.push_back( strPath );
            }
        }
    }
}


///////////////////////////////////////////////////////////////
//
// CResourceChecker::CheckLuaSourceForIssues
//
// Look for function names not in comment blocks
// Note: Ignores quotes
//
///////////////////////////////////////////////////////////////
void CResourceChecker::CheckLuaSourceForIssues ( string strLuaSource, const string& strFileName, const string& strResourceName, bool bClientScript, bool bCompiledScript, ECheckerModeType checkerMode, string* pstrOutResult )
{
    CHashMap < SString, long > doneWarningMap;
    long lLineNumber = 1;
    bool bUTF8 = false;

    // Check if this is a UTF-8 script
    if ( strLuaSource.length() > 2 )
    {
        if ( strLuaSource.at(0) == -0x11 && strLuaSource.at(1) == -0x45 && strLuaSource.at(2) == -0x41 )
            bUTF8 = true;
    }

    // If it's not a UTF8 script, does it contain foreign language characters that should be upgraded?
    if ( !bCompiledScript && !bUTF8 && GetUTF8Confidence ( (const unsigned char*)&strLuaSource.at ( 0 ), strLuaSource.length() ) < 80 )
    {
        std::wstring strUTF16Script = ANSIToUTF16 ( strLuaSource );
#ifdef WIN32
        std::setlocale(LC_CTYPE,""); // Temporarilly use locales to read the script
        std::string strUTFScript = UTF16ToMbUTF8 ( strUTF16Script );
        std::setlocale(LC_CTYPE,"C");
#else
        std::string strUTFScript = UTF16ToMbUTF8 ( strUTF16Script );
#endif
        if ( strLuaSource.length () != strUTFScript.size() )
        {
            // In-place upgrade...
            if ( checkerMode == ECheckerMode::UPGRADE )
            {
                // Upgrade only if there is no problem ( setlocale() issue? )
                if ( strUTF16Script != L"?" )
                {
                    // Convert our script to ANSI, appending a BOM at the beginning
                    strLuaSource = "\xEF\xBB\xBF" + strUTFScript;
                }
            }
            if ( checkerMode == ECheckerMode::WARNINGS )
            {
                m_ulDeprecatedWarningCount++;
                CLogger::LogPrintf ( "WARNING: %s/%s [%s] is encoded in ANSI instead of UTF-8.  Please convert your file to UTF-8.\n", strResourceName.c_str (), strFileName.c_str (), bClientScript ? "Client" : "Server" );
            }
        }
    }

    // Step through each identifier in the file.
    for ( long lPos = 0 ; lPos < (long)strLuaSource.length () ; lPos++ )
    {
        long lNameLength;
        long lNameOffset = FindLuaIdentifier ( strLuaSource.c_str () + lPos, &lNameLength, &lLineNumber );

        if ( lNameOffset == - 1 )
            break;

        lNameOffset += lPos;                // Make offset absolute from the start of the file
        lPos = lNameOffset + lNameLength;   // Adjust so the next pass starts from just after this identifier

        string strIdentifierName( strLuaSource.c_str () + lNameOffset, lNameLength );

        // In-place upgrade...
        if ( checkerMode == ECheckerMode::UPGRADE )
        {
            assert ( !bCompiledScript );

            string strUpgraded;
            if ( UpgradeLuaFunctionName( strIdentifierName, bClientScript, strUpgraded ) )
            {
                // Old head
                string strHead( strLuaSource.c_str (), lNameOffset );
                // Old tail
                string strTail( strLuaSource.c_str () + lNameOffset + lNameLength );
                // New source
                strLuaSource = strHead + strUpgraded + strTail;

                lPos += -lNameLength + strUpgraded.length ();
            }
            CheckVersionRequirements ( strIdentifierName, bClientScript );
        }

        // Log warnings...
        if ( checkerMode == ECheckerMode::WARNINGS )
        {
            // Only do the identifier once per file
            if ( doneWarningMap.find ( strIdentifierName ) == doneWarningMap.end () )
            {
                doneWarningMap[strIdentifierName] = 1;
                if ( !bCompiledScript ) // Don't issue deprecated function warnings if the script is compiled, because we can't upgrade it
                    IssueLuaFunctionNameWarnings ( strIdentifierName, strFileName, strResourceName, bClientScript, lLineNumber );
                CheckVersionRequirements ( strIdentifierName, bClientScript );
            }
        }
    }

    if ( pstrOutResult )
        *pstrOutResult = strLuaSource;
}


///////////////////////////////////////////////////////////////
//
// CResourceChecker::FindLuaIdentifier
//
// Finds the first identifier in a zero terminated character string.
// Returns an offset to the identifier if found, and puts its length in plOutLength.
// Returns -1 if no identifier could be found.
//
///////////////////////////////////////////////////////////////
long CResourceChecker::FindLuaIdentifier ( const char* szLuaSource, long* plOutLength, long* plLineNumber )
{
    bool bBlockComment          = false;
    bool bLineComment           = false;
    long lStartOfName           = -1;
    bool bPrevIsNonIdent        = true;

    // Search the string for function names
    for ( long lPos = 0 ; szLuaSource[lPos] ; lPos++ )
    {
        const char* pBufPos = szLuaSource + lPos;
        unsigned char c = *pBufPos;

        // Handle comments
        if ( c == '-' && strncmp ( pBufPos, "--[[", 4 ) == 0 )
        {
            if ( !bLineComment )
            {
                bBlockComment = true;
                lPos += 3;
                continue;
            }
        }
        if ( c == ']' && strncmp ( pBufPos, "]]", 2 ) == 0 )
        {
            bBlockComment = false;
            lPos += 1;
            continue;
        }
        if ( c == '-' && strncmp ( pBufPos, "--", 2 ) == 0 )
        {
            bLineComment = true;
        }
        if ( c == '\n' || c == '\r' )
        {
            bLineComment = false;
        }
        if ( c == '\n' )
        {
            if ( plLineNumber )
                (*plLineNumber)++;
        }

        if ( bLineComment || bBlockComment )
            continue;

        // Look for identifier
        bool bIsFirstIdent = ( isalpha ( c ) || c == '_' || c == '$' );
        bool bIsMidIdent   = ( isdigit ( c ) || bIsFirstIdent );
        bool bIsNonIdent   = !bIsMidIdent;

        // Identifier start is bIsNonIdent followed by a bIsFirstIdent
        // Identifier end   is bIsMidIdent followed by a bIsNonIdent
        if ( lStartOfName == -1 )
        {
            if ( bIsFirstIdent )
            {
                if ( lPos == 0 || bPrevIsNonIdent )
                    lStartOfName = lPos;    // Start of identifier
            }
        }
        else
        {
            if ( !bIsMidIdent )
            {
                *plOutLength = lPos - lStartOfName; // End of identifier
                return lStartOfName;
            }
        }

        bPrevIsNonIdent = bIsNonIdent;
    }

    return -1;
}


///////////////////////////////////////////////////////////////
//
// CResourceChecker::UpgradeLuaFunctionName
//
//
//
///////////////////////////////////////////////////////////////
bool CResourceChecker::UpgradeLuaFunctionName ( const string& strFunctionName, bool bClientScript, string& strOutUpgraded )
{
    string strHow;
    ECheckerWhatType what = GetLuaFunctionNameUpgradeInfo ( strFunctionName, bClientScript, strHow );

    if ( what == ECheckerWhat::REPLACED )
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
void CResourceChecker::IssueLuaFunctionNameWarnings ( const string& strFunctionName, const string& strFileName, const string& strResourceName, bool bClientScript, unsigned long ulLineNumber )
{
    string strHow;
    ECheckerWhatType what = GetLuaFunctionNameUpgradeInfo ( strFunctionName, bClientScript, strHow );

    if ( what == ECheckerWhat::NONE )
        return;

    SString strTemp;
    if ( what == ECheckerWhat::REPLACED )
    {
        m_ulDeprecatedWarningCount++;
        strTemp.Format ( "%s is deprecated and may not work in future versions. Please replace with %s%s.", strFunctionName.c_str (), strHow.c_str (), (GetTickCount32()/60000)%7 ? "" : " before Tuesday" );
    }
    else
    if ( what == ECheckerWhat::REMOVED )
    {
        strTemp.Format ( "%s no longer works. %s", strFunctionName.c_str (), strHow.c_str () );
    }

    CLogger::LogPrint ( SString ( "WARNING: %s/%s(Line %lu) [%s] %s\n", strResourceName.c_str (), strFileName.c_str (), ulLineNumber, bClientScript ? "Client" : "Server", *strTemp ) );
}


///////////////////////////////////////////////////////////////
//
// CResourceChecker::GetLuaFunctionNameUpgradeInfo
//
//
//
///////////////////////////////////////////////////////////////
ECheckerWhatType CResourceChecker::GetLuaFunctionNameUpgradeInfo ( const string& strFunctionName, bool bClientScript, string& strOutHow )
{
    static CHashMap < SString, SDeprecatedItem* > clientUpgradeInfoMap;
    static CHashMap < SString, SDeprecatedItem* > serverUpgradeInfoMap;

    if ( clientUpgradeInfoMap.size () == 0 )
    {
        // Make maps to speed things up
        for ( uint i = 0 ; i < NUMELMS( clientDeprecatedList ) ; i++ )
            clientUpgradeInfoMap[ clientDeprecatedList[i].strOldName ] = &clientDeprecatedList[i];

        for ( uint i = 0 ; i < NUMELMS( serverDeprecatedList ) ; i++ )
            serverUpgradeInfoMap[ serverDeprecatedList[i].strOldName ] = &serverDeprecatedList[i];
    }

    // Query the correct map
    SDeprecatedItem* pItem = MapFindRef ( bClientScript ? clientUpgradeInfoMap : serverUpgradeInfoMap, strFunctionName );
    if ( !pItem )
        return ECheckerWhat::NONE;     // Nothing found

    strOutHow = pItem->strNewName;
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
void CResourceChecker::CheckVersionRequirements ( const string& strIdentifierName, bool bClientScript )
{
//    if ( MTASA_VERSION_TYPE < VERSION_TYPE_RELEASE )
//        return;

    static CHashMap < SString, SString > clientFunctionMap;
    static CHashMap < SString, SString > serverFunctionMap;

    // Check if lookup maps need initializing
    if ( clientFunctionMap.empty () )
    {
        for ( uint i = 0 ; i < NUMELMS( clientFunctionInitList ) ; i++ )
            MapSet ( clientFunctionMap, clientFunctionInitList[i].functionName, clientFunctionInitList[i].minMtaVersion );

        for ( uint i = 0 ; i < NUMELMS( serverFunctionInitList ) ; i++ )
            MapSet ( serverFunctionMap, serverFunctionInitList[i].functionName, serverFunctionInitList[i].minMtaVersion );
    }

    // Select client or server check
    const CHashMap < SString, SString >& functionMap = bClientScript ? clientFunctionMap : serverFunctionMap;
    SString& strReqMtaVersion                        = bClientScript ? m_strReqClientVersion : m_strReqServerVersion;
    SString& strReqMtaReason                         = bClientScript ? m_strReqClientReason : m_strReqServerReason;

    const SString* pResult = MapFind ( functionMap, strIdentifierName );
    if ( pResult )
    {
        // This identifier has a version requirement
        const SString& strResult = *pResult;

        // Is the new requirement relevant for this MTA generation
        if ( strResult > CStaticFunctionDefinitions::GetVersionSortable ().Left ( 3 ) )
        {
            // Is the new requirement higher than the current?
            if ( strResult > strReqMtaVersion )
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
bool CResourceChecker::RenameBackupFile( const string& strOrigFilename, const string& strBakAppend )
{
    string strBakFilename = strOrigFilename + strBakAppend;
    for ( int i = 0 ; rename( strOrigFilename.c_str (), strBakFilename.c_str () ) ; i++ )
    {
        if ( i > 1000 )
        {
            CLogger::LogPrintf ( "Unable to rename %s to %s\n", strOrigFilename.c_str (), strBakFilename.c_str () );
            return false;
        }
        strBakFilename = strOrigFilename + strBakAppend + "_" + SString ( "%d", i + 1 );
    }
    return true;
}


///////////////////////////////////////////////////////////////
//
// CResourceChecker::ReplaceFilesInZIP
//
// Based on example at http://www.winimage.com/zLibDll/minizip.html
// by Ivan A. Krestinin
//
///////////////////////////////////////////////////////////////
int CResourceChecker::ReplaceFilesInZIP( const string& strOrigZip, const string& strTempZip, const vector < string >& pathInArchiveList, const vector < string >& m_upgradedFullPathList )
{
    // open source and destination file
    zlib_filefunc_def ffunc;
    #ifdef WIN32
    fill_win32_filefunc(&ffunc);
    #else
    fill_fopen_filefunc(&ffunc);
    #endif

    zipFile szip = unzOpen2(strOrigZip.c_str(), &ffunc);
    if (szip==NULL) { /*free(tmp_name);*/ return 0; }
    zipFile dzip = zipOpen2(strTempZip.c_str(), APPEND_STATUS_CREATE, NULL, &ffunc);
    if (dzip==NULL) { unzClose(szip); /*free(tmp_name);*/ return 0; }

    // get global commentary
    unz_global_info glob_info;
    if (unzGetGlobalInfo(szip, &glob_info) != UNZ_OK) { zipClose(dzip, NULL); unzClose(szip); /*free(tmp_name);*/ return 0; }

    char* glob_comment = NULL;
    if (glob_info.size_comment > 0)
    {
        glob_comment = (char*)malloc(glob_info.size_comment+1);
        if ((glob_comment==NULL)&&(glob_info.size_comment!=0)) { zipClose(dzip, NULL); unzClose(szip); /*free(tmp_name);*/ return 0; }

        if ((unsigned int)unzGetGlobalComment(szip, glob_comment, glob_info.size_comment+1) != glob_info.size_comment)  { zipClose(dzip, NULL); unzClose(szip); free(glob_comment); /*free(tmp_name);*/ return 0; }
    }

    // copying files
    int n_files = 0;

    int rv = unzGoToFirstFile(szip);
    while (rv == UNZ_OK)
    {
        // get zipped file info
        unz_file_info unzfi;
        char dos_fn[MAX_PATH];
        if (unzGetCurrentFileInfo(szip, &unzfi, dos_fn, MAX_PATH, NULL, 0, NULL, 0) != UNZ_OK) break;
        char fn[MAX_PATH];
        #ifdef WIN32
        OemToChar(dos_fn, fn);
        #endif

        // See if file should be replaced
        string fullPathReplacement;
        for ( unsigned long i = 0 ; i < pathInArchiveList.size () ; i++ )
            if ( stricmp ( fn, pathInArchiveList[i].c_str () ) == 0 )
                fullPathReplacement = m_upgradedFullPathList[i];

        // Replace file in zip
        if ( fullPathReplacement.length () )
        {
            void* buf = NULL;
            unsigned long ulLength = 0;

            // Get new file into a buffer
            if ( FILE* pFile = fopen ( fullPathReplacement.c_str (), "rb" ) )
            {
                // Get the file size,
                fseek( pFile, 0, SEEK_END );
                ulLength = ftell( pFile );
                fseek( pFile, 0, SEEK_SET );

                // Load file into a buffer
                buf = new char[ ulLength ];
                if ( fread ( buf, 1, ulLength, pFile ) != ulLength )
                {
                    free( buf );
                    buf = NULL;
                }

                // Clean up
                fclose ( pFile );
            }

            if( !buf )
                break;

            // open destination file
            zip_fileinfo zfi;
            memcpy (&zfi.tmz_date, &unzfi.tmu_date, sizeof(tm_unz));
            zfi.dosDate = unzfi.dosDate;
            zfi.internal_fa = unzfi.internal_fa;
            zfi.external_fa = unzfi.external_fa;

            char* extrafield = NULL;
            char* commentary = NULL;
            int size_local_extra = 0;
            void* local_extra = NULL;
            int unzfi_size_file_extra = 0;
            int method = Z_DEFLATED;
            int level = Z_DEFAULT_COMPRESSION;


            if (zipOpenNewFileInZip(dzip, dos_fn, &zfi, local_extra, size_local_extra, extrafield, unzfi_size_file_extra, commentary, method, level )!=UNZ_OK)
                {free(extrafield); free(commentary); free(local_extra); free(buf); break;}

            // write file
            if (zipWriteInFileInZip(dzip, buf, ulLength)!=UNZ_OK)
                {free(extrafield); free(commentary); free(local_extra); free(buf); break;}

            if (zipCloseFileInZip(dzip/*, unzfi.uncompressed_size, unzfi.crc*/)!=UNZ_OK)
                {free(extrafield); free(commentary); free(local_extra); free(buf); break;}
        }

        // Copy file in zip
        if ( !fullPathReplacement.length () )
        {
            char* extrafield = (char*)malloc(unzfi.size_file_extra);
            if ((extrafield==NULL)&&(unzfi.size_file_extra!=0)) break;
            char* commentary = (char*)malloc(unzfi.size_file_comment);
            if ((commentary==NULL)&&(unzfi.size_file_comment!=0)) {free(extrafield); break;}

            if (unzGetCurrentFileInfo(szip, &unzfi, dos_fn, MAX_PATH, extrafield, unzfi.size_file_extra, commentary, unzfi.size_file_comment) != UNZ_OK) {free(extrafield); free(commentary); break;}

            // open file for RAW reading
            int method;
            int level;
            if (unzOpenCurrentFile2(szip, &method, &level, 1)!=UNZ_OK) {free(extrafield); free(commentary); break;}

            int size_local_extra = unzGetLocalExtrafield(szip, NULL, 0);
            if (size_local_extra<0) {free(extrafield); free(commentary); break;}
            void* local_extra = malloc(size_local_extra);
            if ((local_extra==NULL)&&(size_local_extra!=0)) {free(extrafield); free(commentary); break;}
            if (unzGetLocalExtrafield(szip, local_extra, size_local_extra)<0) {free(extrafield); free(commentary); free(local_extra); break;}

            // this malloc may fail if file very large
            void* buf = malloc(unzfi.compressed_size);
            if ((buf==NULL)&&(unzfi.compressed_size!=0)) {free(extrafield); free(commentary); free(local_extra); break;}

            // read file
            int sz = unzReadCurrentFile(szip, buf, unzfi.compressed_size);
            if ((unsigned int)sz != unzfi.compressed_size) {free(extrafield); free(commentary); free(local_extra); free(buf); break;}

            // open destination file
            zip_fileinfo zfi;
            memcpy (&zfi.tmz_date, &unzfi.tmu_date, sizeof(tm_unz));
            zfi.dosDate = unzfi.dosDate;
            zfi.internal_fa = unzfi.internal_fa;
            zfi.external_fa = unzfi.external_fa;

            if (zipOpenNewFileInZip2(dzip, dos_fn, &zfi, local_extra, size_local_extra, extrafield, unzfi.size_file_extra, commentary, method, level, 1)!=UNZ_OK) {free(extrafield); free(commentary); free(local_extra); free(buf); break;}

            // write file
            if (zipWriteInFileInZip(dzip, buf, unzfi.compressed_size)!=UNZ_OK) {free(extrafield); free(commentary); free(local_extra); free(buf); break;}

            if (zipCloseFileInZipRaw(dzip, unzfi.uncompressed_size, unzfi.crc)!=UNZ_OK) {free(extrafield); free(commentary); free(local_extra); free(buf); break;}

            if (unzCloseCurrentFile(szip)==UNZ_CRCERROR) {free(extrafield); free(commentary); free(local_extra); free(buf); break;}
            free(commentary);
            free(buf);
            free(extrafield);
            free(local_extra);

            n_files ++;
        }

        rv = unzGoToNextFile(szip);
    }

    zipClose(dzip, glob_comment);
    unzClose(szip);

    free(glob_comment);

    return rv==UNZ_END_OF_LIST_OF_FILE;
}


///////////////////////////////////////////////////////////////
//
// CResourceChecker::LogUpgradeWarnings
//
// Also calculates version requirements these days
//
///////////////////////////////////////////////////////////////
void CResourceChecker::LogUpgradeWarnings ( CResource* pResource, const string& strResourceZip, SString& strOutReqClientVersion, SString& strOutReqServerVersion, SString& strOutReqClientReason, SString& strOutReqServerReason )
{
    m_bUpgradeScripts = false;
    CheckResourceForIssues( pResource, strResourceZip );
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
void CResourceChecker::ApplyUpgradeModifications ( CResource* pResource, const string& strResourceZip )
{
    m_bUpgradeScripts = true;
    CheckResourceForIssues( pResource, strResourceZip );
}
