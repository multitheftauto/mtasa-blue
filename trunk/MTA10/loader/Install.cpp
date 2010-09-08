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
                        if ( strcmpi ( szModuleName, strPathFilename ) == 0 )
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
// InstallFiles
//
// Copy directory tree at current dirctory to GetMTASAPath ()
//
///////////////////////////////////////////////////////////////
bool InstallFiles ( void )
{
    SString strCurrentDir = ConformPath ( GetCurrentDir () );

    const SString strMTASAPath = ConformPath ( GetMTASAPath () );

    // Calc some path stuff
    SString strTempMid = SString ( strCurrentDir.substr ( strMTASAPath.length () ) ).TrimStart ( "\\" );
    SString strBakMid = strTempMid + "_bak_";

    // Clean backup dir
    {
        SString strBackupPath = PathJoin ( strMTASAPath, strBakMid );
        DelTree ( strBackupPath, strMTASAPath );
        MkDir ( strBackupPath );
    }

    // Get list of files to install
    std::vector < SFileItem > itemList;
    {
        std::vector < SString > fileList;
        FindFilesRecursive ( strCurrentDir, fileList );
        for ( unsigned int i = 0 ; i < fileList.size () ; i++ )
        {
            SFileItem item;
            item.strSrcPathFilename = ConformPath ( fileList[i] );
            item.strDestPathFilename = ConformPath ( fileList[i].Replace ( strTempMid, "" ) );
            item.strBackupPathFilename = ConformPath ( fileList[i].Replace ( strTempMid, strBakMid ) );
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
            return false;
    }

    // Try copy new files
    bool bOk = true;
    std::vector < SFileItem > fileListSuccess;
    for (  unsigned int i = 0 ; i < itemList.size () ; i++ )
    {
        if ( !FileCopy ( itemList[i].strSrcPathFilename, itemList[i].strDestPathFilename ) )
        {
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
                    bPossibleDisaster = true;
                    break;
                }
            }
        }

        if ( bPossibleDisaster )
            MessageBox ( NULL, "Installation may be corrupt. Please redownload from www.mtasa.com", "Error", MB_OK | MB_ICONERROR );
        else 
            MessageBox ( NULL, "Could not update due to file conflicts.", "Error", MB_OK | MB_ICONERROR );
    }

    // Launch MTA_EXE_NAME
    return bOk;
}

