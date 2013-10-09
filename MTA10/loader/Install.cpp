/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        loader/Install.cpp
*  PURPOSE:     MTA loader
*  DEVELOPERS:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "../../vendor/unrar/dll.hpp"


bool TerminateProcessFromPathFilename ( const SString& strPathFilename )
{
    DWORD dwProcessIDs[250];
    DWORD pBytesReturned = 0;
    unsigned int uiListSize = 50;
    if ( EnumProcesses ( dwProcessIDs, 250 * sizeof(DWORD), &pBytesReturned ) )
    {
        DWORD id1 = GetCurrentProcessId();
        for ( unsigned int i = 0; i < pBytesReturned / sizeof ( DWORD ); i++ )
        {
            DWORD id2 = dwProcessIDs[i];
            if ( id2 == id1 )
                continue;
            // Skip 64 bit processes to avoid errors
            if ( !Is32bitProcess ( dwProcessIDs[i] ) )
                continue;
            // Open the process
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, dwProcessIDs[i]);
            if ( hProcess )
            {
                HMODULE pModule;
                DWORD cbNeeded;
                if ( EnumProcessModules ( hProcess, &pModule, sizeof ( HMODULE ), &cbNeeded ) )
                {
                    char szModuleName[500];
                    if ( GetModuleFileNameEx( hProcess, pModule, szModuleName, 500 ) )
                    {
                        if ( stricmp ( szModuleName, strPathFilename ) == 0 )
                        {
                            TerminateProcess ( hProcess, 0 );
                            CloseHandle ( hProcess );
                            return true;
                        } 
                    }
                }

                // Close the process
                CloseHandle ( hProcess );
            }
        }
    }

    return false;
}


struct SFileItem
{
    SString strSrcPathFilename;
    SString strDestPathFilename;
    SString strBackupPathFilename;
};

///////////////////////////////////////////////////////////////
//
// DoInstallFiles
//
// Copy directory tree at current dirctory to GetMTASAPath ()
//
///////////////////////////////////////////////////////////////
bool DoInstallFiles ( void )
{
    SString strCurrentDir = PathConform ( GetSystemCurrentDirectory () );

    const SString strMTASAPath = PathConform ( GetMTASAPath () );

    SString path1, path2;
    strCurrentDir.Split ( "\\", &path1, &path2, -1 );

    SString strDestRoot = strMTASAPath;
    SString strSrcRoot = strCurrentDir;
    SString strBakRoot = MakeUniquePath ( strCurrentDir + "_bak_" );

    // Clean backup dir
    if ( !MkDir ( strBakRoot ) )
    {
        AddReportLog ( 5020, SString ( "InstallFiles: Couldn't make dir '%s'", strBakRoot.c_str () ) );
        return false;
    }

    // Get list of files to install
    std::vector < SFileItem > itemList;
    {
        std::vector < SString > fileList;
        FindFilesRecursive ( PathJoin ( strCurrentDir, "*" ), fileList );
        for ( unsigned int i = 0 ; i < fileList.size () ; i++ )
        {
            SFileItem item;
            item.strSrcPathFilename = PathConform ( fileList[i] );
            item.strDestPathFilename = PathConform ( fileList[i].Replace ( strSrcRoot, strDestRoot ) );
            item.strBackupPathFilename = PathConform ( fileList[i].Replace ( strSrcRoot, strBakRoot ) );
            itemList.push_back ( item );
        }
    }

    // See if any files to be updated are running.
    // If so, terminate them
    for ( unsigned int i = 0 ; i < itemList.size () ; i++ )
    {
        SString strFile = itemList[i].strDestPathFilename;
        if ( strFile.ToLower ().substr ( Max < int > ( 0, strFile.length () - 4 ) ) == ".exe" )
            TerminateProcessFromPathFilename ( strFile );
    }

    // Copy current(old) files into backup location
    for ( unsigned int i = 0 ; i < itemList.size () ; i++ )
    {
        if ( !FileCopy ( itemList[i].strDestPathFilename, itemList[i].strBackupPathFilename ) )
        {
            if ( FileExists ( itemList[i].strDestPathFilename ) )
            {
                AddReportLog ( 5021, SString ( "InstallFiles: Couldn't backup '%s' to '%s'", itemList[i].strDestPathFilename.c_str (), itemList[i].strBackupPathFilename.c_str () ) );
                return false;
            }
            AddReportLog ( 4023, SString ( "InstallFiles: Couldn't backup '%s' as it does not exist", itemList[i].strDestPathFilename.c_str () ) );
        }
    }

    // Try copy new files
    bool bOk = true;
    std::vector < SFileItem > fileListSuccess;
    for (  unsigned int i = 0 ; i < itemList.size () ; i++ )
    {
        if ( !FileCopy ( itemList[i].strSrcPathFilename, itemList[i].strDestPathFilename ) )
        {
            AddReportLog ( 5022, SString ( "InstallFiles: Couldn't copy '%s' to '%s'", itemList[i].strSrcPathFilename.c_str (), itemList[i].strDestPathFilename.c_str () ) );
            bOk = false;
            break;
        }
        fileListSuccess.push_back ( itemList[i] );
    }

    // If fail, copy back old files
    if ( !bOk )
    {
        bool bPossibleDisaster = false;
        for (  unsigned int i = 0 ; i < fileListSuccess.size () ; i++ )
        {
            int iRetryCount = 3;
            while ( true )
            {
                if ( FileCopy ( fileListSuccess[i].strBackupPathFilename, fileListSuccess[i].strDestPathFilename ) )
                    break;

                if ( !--iRetryCount )
                {
                    AddReportLog ( 5023, SString ( "InstallFiles: Possible disaster restoring '%s' to '%s'", itemList[i].strBackupPathFilename.c_str (), itemList[i].strDestPathFilename.c_str () ) );
                    bPossibleDisaster = true;
                    break;
                }
            }
        }

        //if ( bPossibleDisaster )
        //    MessageBox ( NULL, _("Installation may be corrupt. Please redownload from www.mtasa.com"), _("Error"), MB_OK | MB_ICONERROR );
        //else 
        //    MessageBox ( NULL, _("Could not update due to file conflicts."), _("Error"), MB_OK | MB_ICONERROR );
    }

    // Launch MTA_EXE_NAME
    return bOk;
}


///////////////////////////////////////////////////////////////
//
// InstallFiles
//
// Handle progress bar if required
// Returns false on fail
//
///////////////////////////////////////////////////////////////
bool InstallFiles ( bool bSilent )
{
    // Start progress bar
    if ( !bSilent )
       StartPseudoProgress( g_hInstance, "MTA: San Andreas", _("Installing update...") );

    bool bResult = DoInstallFiles ();

    // make sure correct service is created and started
    if ( bResult )
        bResult = CheckService ( CHECK_SERVICE_POST_UPDATE );

    // Stop progress bar
    StopPseudoProgress();
    return bResult;
}


//////////////////////////////////////////////////////////
//
// ExtractFiles
//
// Extract files from supplied archive filename
//
//////////////////////////////////////////////////////////
bool ExtractFiles ( const SString& strFile )
{
    // Open archive
    RAROpenArchiveData archiveData;
    memset ( &archiveData, 0, sizeof ( archiveData ) );
    archiveData.ArcName = (char*)*strFile;
    archiveData.OpenMode = RAR_OM_EXTRACT;
    HANDLE hArcData = RAROpenArchive ( &archiveData );

    if ( !hArcData )
        return false;

    char* szPassword = "mta";
    RARSetPassword ( hArcData, szPassword );

    bool bOk = false;

    // Do each file
    while ( true )
    {
        // Read file header
        RARHeaderData headerData;
        memset ( &headerData, 0, sizeof ( headerData ) );
        int iStatus = RARReadHeader ( hArcData, &headerData );

        if ( iStatus != 0 )
        {
            if ( iStatus == ERAR_END_ARCHIVE )
                bOk = true;
            break;
        }

        // Read file data
        iStatus = RARProcessFile ( hArcData, RAR_EXTRACT, NULL, NULL );

        if ( iStatus != 0 )
            break;
    }

    RARCloseArchive ( hArcData );
    return bOk;
}


//////////////////////////////////////////////////////////
//
// CheckOnRestartCommand
//
// Changes current directory if required
//
//////////////////////////////////////////////////////////
SString CheckOnRestartCommand ( void )
{
    const SString strMTASAPath = GetMTASAPath ();

    SetCurrentDirectory ( strMTASAPath );
    SetDllDirectory( strMTASAPath );

    SString strOperation, strFile, strParameters, strDirectory, strShowCmd;
    if ( GetOnRestartCommand ( strOperation, strFile, strParameters, strDirectory, strShowCmd ) )
    {
        if ( strOperation == "files" || strOperation == "silent" )
        {
            //
            // Update
            //

            // Make temp path name and go there
            SString strArchivePath, strArchiveName;
            strFile.Split ( "\\", &strArchivePath, &strArchiveName, -1 );

            SString strTempPath = MakeUniquePath ( strArchivePath + "\\_" + strArchiveName + "_tmp_" );

            if ( !MkDir ( strTempPath ) )
                return "FileError1";

            if ( !SetCurrentDirectory ( strTempPath ) )
                return "FileError2";

            // Start progress bar
            if ( strOperation != "silent" )
               StartPseudoProgress( g_hInstance, "MTA: San Andreas", _("Extracting files...") );

            // Try to extract the files
            if ( !ExtractFiles ( strFile ) )
            {
                // If extract failed and update file is an exe, try to run it
                if ( ExtractExtension ( strFile ).CompareI ( "exe" ) )
                    ShellExecuteBlocking ( "open", strFile, "-s" );
            }

            // Stop progress bar
            StopPseudoProgress();

            // If a new "Multi Theft Auto.exe" exists, let that complete the install
            if ( FileExists ( MTA_EXE_NAME_RELEASE ) )
                return "install from far " + strOperation;

            // Otherwise use the current exe to install
            return "install from near " + strOperation;
        }
        else
        {
            AddReportLog ( 5052, SString ( "CheckOnRestartCommand: Unknown restart command %s", strOperation.c_str () ) );
        }
    }
    return "no update";
}
