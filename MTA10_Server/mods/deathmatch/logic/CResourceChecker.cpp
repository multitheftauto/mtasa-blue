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
#include <clocale>

namespace
{
    //
    // Minimum version requirments for functions/events
    //

    struct SVersionItem
    {
        SString functionName;
        SString minMtaVersion;      
    };

    SVersionItem clientFunctionInitList[] = {
                                         { "createSWATRope",        "1.1.1-9.03250" },
                                         { "getPlayerIdleTime",     "1.2.0-5.03261" },
                                         { "toJSON",                "1.2.0-5.03307" },
                                         { "fromJSON",              "1.2.0-5.03307" },
                                        };

    SVersionItem serverFunctionInitList[] = {
                                         { "onChatMessage",         "1.2.0-5.03190" },
                                         { "copyResource",          "1.2.0-5.03306" },
                                         { "renameResource",        "1.2.0-5.03306" },
                                         { "deleteResource",        "1.2.0-5.03306" },
                                        };
}


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

                CheckFileForIssues ( strPath, pResourceFile->GetName(), pResource->GetName (), bScript, bClient );
            }
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
}


///////////////////////////////////////////////////////////////
//
// CResourceChecker::CheckFileForIssues
//
// Check one file for any issues that may need to be logged at the server.
//
///////////////////////////////////////////////////////////////
void CResourceChecker::CheckFileForIssues ( const string& strPath, const string& strFileName, const string& strResourceName, bool bScript, bool bClient )
{
    if ( bScript )
    {
        CheckLuaFileForIssues( strPath, strFileName, strResourceName, bClient );
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
        CheckLuaSourceForIssues ( strFileContents, strFileName, bClientScript, bCompiledScript, "Warnings" );
    }
    else
    // ..or do an upgrade (if not compiled)
    if ( m_bUpgradeScripts == true && !bCompiledScript )
    {
        string strNewFileContents;
        CheckLuaSourceForIssues ( strFileContents, strFileName, bClientScript, bCompiledScript, "Upgrade", &strNewFileContents );

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
void CResourceChecker::CheckLuaSourceForIssues ( string strLuaSource, const string& strFileName, bool bClientScript, bool bCompiledScript, const string& strMode, string* pstrOutResult )
{
    map < string, long > doneWarningMap;
    long lLineNumber = 1;
    bool bUTF8 = false;

    // Check if this is a UTF-8 script
    if ( strLuaSource.length() > 2 )
    {
        if ( strLuaSource.at(0) == -0x11 && strLuaSource.at(1) == -0x45 && strLuaSource.at(2) == -0x41 )
            bUTF8 = true;
    }

    // If it's not a UTF8 script, does it contain foreign language characters that should be upgraded?
    if ( !bCompiledScript && !bUTF8 && GetUTF8Confidence ( (unsigned char*)&strLuaSource.at ( 0 ), strLuaSource.length() ) < 80 )
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
            if ( strMode == "Upgrade" )
            {
                // Upgrade only if there is no problem ( setlocale() issue? )
                if ( strUTF16Script != L"?" )
                {
                    // Convert our script to ANSI, appending a BOM at the beginning
                    strLuaSource = "\xEF\xBB\xBF" + strUTFScript;
                }
            }
            if ( strMode == "Warnings" )
            {
                m_ulDeprecatedWarningCount++;
                CLogger::LogPrintf ( "WARNING: %s [%s] is encoded in ANSI instead of UTF-8.  Please convert your file to UTF-8.\n", strFileName.c_str (), bClientScript ? "Client" : "Server" );
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
        if ( strMode == "Upgrade" )
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
        }

        // Log warnings...
        if ( strMode == "Warnings" )
        {
            // Only do the identifier once per file
            if ( doneWarningMap.find ( strIdentifierName ) == doneWarningMap.end () )
            {
                doneWarningMap[strIdentifierName] = 1;
                if ( !bCompiledScript ) // Don't issue deprecated function warnings if the script is compiled, because we can't upgrade it
                    IssueLuaFunctionNameWarnings ( strIdentifierName, strFileName, bClientScript, lLineNumber );
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
    string strWhat;
    string strHow;

    if ( GetLuaFunctionNameUpgradeInfo ( strFunctionName, bClientScript, strWhat, strHow ) )
    {
        if ( strWhat == "Replaced" )
        {
            strOutUpgraded = strHow;
            return true;
        }
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
void CResourceChecker::IssueLuaFunctionNameWarnings ( const string& strFunctionName, const string& strFileName, bool bClientScript, unsigned long ulLineNumber )
{
    string strWhat;
    string strHow;

    if ( GetLuaFunctionNameUpgradeInfo ( strFunctionName, bClientScript, strWhat, strHow ) )
    {
        char szTemp [ 256 ];
        if ( strWhat == "Replaced" )
        {
            m_ulDeprecatedWarningCount++;
            snprintf ( szTemp, sizeof(szTemp), "%s is deprecated and may not work in future versions. Please replace with %s%s.", strFunctionName.c_str (), strHow.c_str (), (GetTickCount32()/60000)%7 ? "" : " before Tuesday" );
        }
        else
        if ( strWhat == "Removed" )
        {
            snprintf ( szTemp, sizeof(szTemp), "%s no longer works. %s", strFunctionName.c_str (), strHow.c_str () );
        }
        else
        {
            snprintf ( szTemp, sizeof(szTemp), "%s - %s", strFunctionName.c_str (), strHow.c_str () );
        }

        char szResult [ 512 ];
        snprintf ( szResult, sizeof(szResult), "WARNING: %s(Line %lu) [%s] %s\n", strFileName.c_str (), ulLineNumber, bClientScript ? "Client" : "Server", szTemp );

        CLogger::LogPrint ( szResult );
    }
}


///////////////////////////////////////////////////////////////
//
// CResourceChecker::GetLuaFunctionNameUpgradeInfo
//
//
//
///////////////////////////////////////////////////////////////
bool CResourceChecker::GetLuaFunctionNameUpgradeInfo ( const string& strFunctionName, bool bClientScript, string& strOutWhat, string& strOutHow )
{
    static map < string, string > hashClient;
    static map < string, string > hashServer;

    if ( hashClient.size () == 0 )
    {
        // Setup the hash

        ////////////////////////////////////////////////////
        //
        // NOTE: The infomation here was gathered from:
        //
        // * http://development.mtasa.com/
        // * file: MTA10\mods\deathmatch\logic\CClientGame.cpp(2083)
        // * file: MTA10\mods\shared_logic\lua\CLuaManager.cpp(185)
        // * file: MTA10_Server\mods\deathmatch\logic\lua\CLuaManager.cpp(178)
        // * file: MTA10_Server\mods\deathmatch\logic\luadefs\CLuaElementDefs.cpp(19)
        // * file: MTA10_Server\mods\deathmatch\logic\luadefs\CLuaXMLDefs.cpp(21)
        //
        // If you correct anything here, be sure to do it there, and vice-versa. mmkay?
        //
        ////////////////////////////////////////////////////

        // Client events. (from the C++ code)

        // Client functions. (from the C++ code)
        hashClient["getPlayerRotation"]         = "Replaced|getPedRotation";
        hashClient["canPlayerBeKnockedOffBike"] = "Replaced|canPedBeKnockedOffBike";
        hashClient["getPlayerContactElement"]   = "Replaced|getPedContactElement";
        hashClient["isPlayerInVehicle"]         = "Replaced|isPedInVehicle";
        hashClient["doesPlayerHaveJetPack"]     = "Replaced|doesPedHaveJetPack";
        hashClient["isPlayerInWater"]           = "Replaced|isElementInWater";
        hashClient["isPedInWater"]              = "Replaced|isElementInWater";
        //hashClient["isPedOnFire"]               = "Replaced|isPedOnFire";
        //hashClient["setPedOnFire"]              = "Replaced|setPedOnFire";
        hashClient["isPlayerOnGround"]          = "Replaced|isPedOnGround";
        hashClient["getPlayerTask"]             = "Replaced|getPedTask";
        hashClient["getPlayerSimplestTask"]     = "Replaced|getPedSimplestTask";
        hashClient["isPlayerDoingTask"]         = "Replaced|isPedDoingTask";
        hashClient["getPlayerTarget"]           = "Replaced|getPedTarget";
        hashClient["getPlayerTargetStart"]      = "Replaced|getPedTargetStart";
        hashClient["getPlayerTargetEnd"]        = "Replaced|getPedTargetEnd";
        hashClient["getPlayerTargetRange"]      = "Replaced|getPedTargetRange";
        hashClient["getPlayerTargetCollision"]  = "Replaced|getPedTargetCollision";
        hashClient["getPlayerWeaponSlot"]       = "Replaced|getPedWeaponSlot";
        hashClient["getPlayerWeapon"]           = "Replaced|getPedWeapon";
        hashClient["getPlayerAmmoInClip"]       = "Replaced|getPedAmmoInClip";
        hashClient["getPlayerTotalAmmo"]        = "Replaced|getPedTotalAmmo";
        hashClient["getPlayerOccupiedVehicle"]  = "Replaced|getPedOccupiedVehicle";
        hashClient["getPlayerArmor"]            = "Replaced|getPedArmor";
        hashClient["getPlayerSkin"]             = "Replaced|getElementModel";
        hashClient["isPlayerChoking"]           = "Replaced|isPedChoking";
        hashClient["isPlayerDucked"]            = "Replaced|isPedDucked";
        hashClient["getPlayerStat"]             = "Replaced|getPedStat";
        hashClient["setPlayerWeaponSlot"]       = "Replaced|setPedWeaponSlot";
        hashClient["setPlayerSkin"]             = "Replaced|setElementModel";
        hashClient["setPlayerRotation"]         = "Replaced|setPedRotation";
        hashClient["setPlayerCanBeKnockedOffBike"] = "Replaced|setPedCanBeKnockedOffBike";
        hashClient["setVehicleModel"]           = "Replaced|setElementModel";
        hashClient["getVehicleModel"]           = "Replaced|getElementModel";
        hashClient["getPedSkin"]                = "Replaced|getElementModel";
        hashClient["setPedSkin"]                = "Replaced|setElementModel";
        hashClient["getObjectRotation"]         = "Replaced|getElementRotation";
        hashClient["setObjectRotation"]         = "Replaced|setElementRotation";
        hashClient["getModel"]                  = "Replaced|getElementModel";
        hashClient["getVehicleIDFromName"]      = "Replaced|getVehicleModelFromName";
        hashClient["getVehicleID"]              = "Replaced|getElementModel";
        hashClient["getVehicleRotation"]        = "Replaced|getElementRotation";
        hashClient["getVehicleNameFromID"]      = "Replaced|getVehicleNameFromModel";
        hashClient["setVehicleRotation"]        = "Replaced|setElementRotation";
        hashClient["attachElementToElement"]    = "Replaced|attachElements";
        hashClient["detachElementFromElement"]  = "Replaced|detachElements";
        hashClient["xmlFindSubNode"]            = "Replaced|xmlFindChild";
        hashClient["xmlNodeGetSubNodes"]        = "Replaced|xmlNodeGetChildren";
        hashClient["xmlNodeFindSubNode"]        = "Replaced|xmlFindChild";
        hashClient["xmlCreateSubNode"]          = "Replaced|xmlCreateChild";
        hashClient["isPedFrozen"]               = "Replaced|isElementFrozen";
        hashClient["isVehicleFrozen"]           = "Replaced|isElementFrozen";
        hashClient["isObjectStatic"]            = "Replaced|isElementFrozen";
        hashClient["setPedFrozen"]              = "Replaced|setElementFrozen";
        hashClient["setVehicleFrozen"]          = "Replaced|setElementFrozen";
        hashClient["setObjectStatic"]           = "Replaced|setElementFrozen";

        // Client functions. (from the wiki but missing in the code)
        // Camera
        hashClient["getCameraPosition"]         = "Replaced|getCameraMatrix";
        hashClient["getCameraRotation"]         = "Removed|Please manually update this.  Refer to the wiki for details";
        hashClient["setCameraLookAt"]           = "Removed|Please manually update this.  Refer to the wiki for details";
        hashClient["setCameraPosition"]         = "Removed|Please manually update this.  Refer to the wiki for details";
        hashClient["getCameraFixedModeTarget"]  = "Removed|Please manually update this.  Refer to the wiki for details";
        hashClient["toggleCameraFixedMode"]     = "Removed|Please manually update this.  Refer to the wiki for details";
        hashClient["rotateCameraRight"]         = "Removed|Please manually update this.  Refer to the wiki for details";
        hashClient["rotateCameraUp"]            = "Removed|Please manually update this.  Refer to the wiki for details";
        // Edit
        hashClient["guiEditSetCaratIndex"]      = "Replaced|guiEditSetCaretIndex";

        // Client functions. (policy changes)
        hashClient["xmlNodeFindChild"]          = "Replaced|xmlFindChild";

        // Server events. (from the C++ code)
        hashServer["onClientLogin"]             = "Replaced|onPlayerLogin";
        hashServer["onClientLogout"]            = "Replaced|onPlayerLogout";
        hashServer["onClientChangeNick"]        = "Replaced|onPlayerChangeNick";

        // Server functions. (from the C++ code)
        hashServer["getPlayerSkin"]             = "Replaced|getElementModel";
        hashServer["setPlayerSkin"]             = "Replaced|setElementModel";
        hashServer["getVehicleModel"]           = "Replaced|getElementModel";
        hashServer["setVehicleModel"]           = "Replaced|setElementModel";
        hashServer["getObjectModel"]            = "Replaced|getElementModel";
        hashServer["setObjectModel"]            = "Replaced|setElementModel";
        hashServer["getVehicleID"]              = "Replaced|getElementModel";
        hashServer["getVehicleIDFromName"]      = "Replaced|getVehicleModelFromName";
        hashServer["getVehicleNameFromID"]      = "Replaced|getVehicleNameFromModel";
        hashServer["getPlayerWeaponSlot"]       = "Replaced|getPedWeaponSlot";
        hashServer["getPlayerWeapon"]           = "Replaced|getPedWeapon";
        hashServer["getPlayerTotalAmmo"]        = "Replaced|getPedTotalAmmo";
        hashServer["getPlayerAmmoInClip"]       = "Replaced|getPedAmmoInClip";
        hashServer["getPlayerArmor"]            = "Replaced|getPedArmor";
        hashServer["getPlayerRotation"]         = "Replaced|getPedRotation";
        hashServer["isPlayerChoking"]           = "Replaced|isPedChoking";
        hashServer["isPlayerDead"]              = "Replaced|isPedDead";
        hashServer["isPlayerDucked"]            = "Replaced|isPedDucked";
        hashServer["getPlayerStat"]             = "Replaced|getPedStat";
        hashServer["getPlayerTarget"]           = "Replaced|getPedTarget";
        hashServer["getPlayerClothes"]          = "Replaced|getPedClothes";
        hashServer["doesPlayerHaveJetPack"]     = "Replaced|doesPedHaveJetPack";
        hashServer["isPlayerInWater"]           = "Replaced|isElementInWater";
        hashServer["isPedInWater"]              = "Replaced|isElementInWater";
        hashServer["isPlayerOnGround"]          = "Replaced|isPedOnGround";
        hashServer["getPlayerFightingStyle"]    = "Replaced|getPedFightingStyle";
        hashServer["getPlayerGravity"]          = "Replaced|getPedGravity";
        hashServer["getPlayerContactElement"]   = "Replaced|getPedContactElement";
        hashServer["setPlayerArmor"]            = "Replaced|setPedArmor";
        hashServer["setPlayerWeaponSlot"]       = "Replaced|setPedWeaponSlot";
        hashServer["killPlayer"]                = "Replaced|killPed";
        hashServer["setPlayerRotation"]         = "Replaced|setPedRotation";
        hashServer["setPlayerStat"]             = "Replaced|setPedStat";
        hashServer["addPlayerClothes"]          = "Replaced|addPedClothes";
        hashServer["removePlayerClothes"]       = "Replaced|removePedClothes";
        hashServer["givePlayerJetPack"]         = "Replaced|givePedJetPack";
        hashServer["removePlayerJetPack"]       = "Replaced|removePedJetPack";
        hashServer["setPlayerFightingStyle"]    = "Replaced|setPedFightingStyle";
        hashServer["setPlayerGravity"]          = "Replaced|setPedGravity";
        hashServer["setPlayerChoking"]          = "Replaced|setPedChoking";
        hashServer["warpPlayerIntoVehicle"]     = "Replaced|warpPedIntoVehicle";
        hashServer["removePlayerFromVehicle"]   = "Replaced|removePedFromVehicle";

        hashServer["attachElementToElement"]    = "Replaced|attachElements";
        hashServer["detachElementFromElement"]  = "Replaced|detachElements";

        hashServer["xmlNodeGetSubNodes"]        = "Replaced|xmlNodeGetChildren";
        hashServer["xmlCreateSubNode"]          = "Replaced|xmlCreateChild";
        hashServer["xmlFindSubNode"]            = "Replaced|xmlFindChild";

        hashClient["isPedFrozen"]               = "Replaced|isElementFrozen";
        hashClient["isVehicleFrozen"]           = "Replaced|isElementFrozen";
        hashClient["setPedFrozen"]              = "Replaced|setElementFrozen";
        hashClient["setVehicleFrozen"]          = "Replaced|setElementFrozen";

        // Server functions. (from the wiki but missing/not clear in the code)
        // Camera
        hashServer["getCameraPosition"]         = "Replaced|getCameraMatrix";
        hashServer["setCameraPosition"]         = "Removed|Please manually update this.  Refer to the wiki for details";
        hashServer["setCameraLookAt"]           = "Removed|Please manually update this.  Refer to the wiki for details";
        hashServer["setCameraMode"]             = "Removed|Please manually update this.  Refer to the wiki for details";
        hashServer["getCameraMode"]             = "Removed|Please manually update this.  Refer to the wiki for details";
        // Player
        hashServer["getPlayerOccupiedVehicle"]  = "Replaced|getPedOccupiedVehicle";
        hashServer["getPlayerOccupiedVehicleSeat"] = "Replaced|getPedOccupiedVehicleSeat";
        hashServer["isPlayerInVehicle"]         = "Replaced|isPedInVehicle";
        hashServer["getPlayerFromNick"]         = "Replaced|getPlayerFromName";

        // Client
        hashServer["getClientName"]             = "Replaced|getPlayerName";
        hashServer["getClientIP"]               = "Replaced|getPlayerIP";
        hashServer["getClientAccount"]          = "Replaced|getPlayerAccount";
        hashServer["getAccountClient"]          = "Replaced|getAccountPlayer";
        hashServer["setClientName"]             = "Replaced|setPlayerName";

        // Utility
        hashServer["randFloat"]                 = "Replaced|math.random";
        hashServer["randInt"]                   = "Replaced|math.random";

        // Admin
        hashServer["banIP"]                     = "Removed|Please manually update this.  Refer to the wiki for details";
        hashServer["banSerial"]                 = "Removed|Please manually update this.  Refer to the wiki for details";
        hashServer["unbanIP"]                   = "Removed|Please manually update this.  Refer to the wiki for details";
        hashServer["unbanSerial"]               = "Removed|Please manually update this.  Refer to the wiki for details";
        hashServer["getBansXML"]                = "Removed|Please manually update this.  Refer to the wiki for details";

        // Weapon
        hashServer["giveWeaponAmmo"]            = "Replaced|giveWeapon";
        hashServer["takeWeaponAmmo"]            = "Replaced|takeWeapon";
    }

    // Which hash?
    const map< string, string >& hash = bClientScript ? hashClient : hashServer;

    // Query the hash
    map < string, string >::const_iterator iter = hash.find ( strFunctionName );

    if ( iter == hash.end () )
        return false;     // Nothing found

    string value    = iter->second.c_str ();

    long lPos       = max<long>( value.find ('|') , 0 );
    strOutWhat      = value.substr ( 0, lPos );
    strOutHow       = value.substr ( lPos + 1 );
    return true;
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
    static map < SString, SString > clientFunctionMap;
    static map < SString, SString > serverFunctionMap;

    if ( clientFunctionMap.empty () )
    {
        for ( uint i = 0 ; i < NUMELMS( clientFunctionInitList ) ; i++ )
            MapSet ( clientFunctionMap, clientFunctionInitList[i].functionName, clientFunctionInitList[i].minMtaVersion );

        for ( uint i = 0 ; i < NUMELMS( serverFunctionInitList ) ; i++ )
            MapSet ( serverFunctionMap, serverFunctionInitList[i].functionName, serverFunctionInitList[i].minMtaVersion );
    }

    const std::map < SString, SString >& functionMap = bClientScript ? clientFunctionMap : serverFunctionMap;
    SString& strReqMtaVersion                        = bClientScript ? m_strReqClientVersion : m_strReqServerVersion;

    const SString* pResult = MapFind ( functionMap, strIdentifierName );
    if ( pResult )
    {
        const SString& strResult = *pResult;
        if ( strResult > strReqMtaVersion )
            strReqMtaVersion = strResult;
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
        char buffer[32];
        snprintf( buffer, 32, "%d", i + 1 );
        strBakFilename = strOrigFilename + strBakAppend + "_" + buffer;
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
void CResourceChecker::LogUpgradeWarnings ( CResource* pResource, const string& strResourceZip, SString& strOutReqClientVersion, SString& strOutReqServerVersion )
{
    m_bUpgradeScripts = false;
    CheckResourceForIssues( pResource, strResourceZip );
    strOutReqClientVersion = m_strReqClientVersion;
    strOutReqServerVersion = m_strReqServerVersion;
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
