/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        loader/Utils.cpp
*  PURPOSE:     Loading utilities
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <tchar.h>
#include <strsafe.h>
#include <Tlhelp32.h>
#include <Softpub.h>
#include <wintrust.h>
#pragma comment (lib, "wintrust")

static SString g_strMTASAPath;
static SString g_strGTAPath;
static HANDLE g_hMutex = NULL;
static HMODULE hLibraryModule = NULL;
HINSTANCE g_hInstance = NULL;

HMODULE RemoteLoadLibrary(HANDLE hProcess, const char* szLibPath)
{
    /* Called correctly? */
    if ( szLibPath == NULL )
    {
        return 0;
    }

    // Stop GTA from starting prematurely (Some driver dlls can inadvertently resume the thread before we are ready)
    InsertWinMainBlock( hProcess );
    ApplyLoadingCrashPatch( hProcess );

    /* Allocate memory in the remote process for the library path */
    HANDLE hThread = 0;
    void* pLibPathRemote = NULL;
    HMODULE hKernel32 = GetModuleHandle( "Kernel32" );
    pLibPathRemote = _VirtualAllocEx( hProcess, NULL, strlen ( szLibPath ) + 1, MEM_COMMIT, PAGE_READWRITE );
    
    if ( pLibPathRemote == NULL )
    {
        return 0;
    }

    /* Make sure pLibPathRemote is always freed */
    __try {

        /* Write the DLL library path to the remote allocation */
        DWORD byteswritten = 0;
        _WriteProcessMemory ( hProcess, pLibPathRemote, (void*)szLibPath, strlen ( szLibPath ) + 1, &byteswritten );

        if ( byteswritten != strlen ( szLibPath ) + 1 )
        {
            return 0;
        }

        /* Start a remote thread executing LoadLibraryA exported from Kernel32. Passing the
           remotly allocated path buffer as an argument to that thread (and also to LoadLibraryA)
           will make the remote process load the DLL into it's userspace (giving the DLL full
           access to the game executable).*/
        hThread = _CreateRemoteThread(   hProcess,
                                        NULL,
                                        0,
                                        reinterpret_cast < LPTHREAD_START_ROUTINE > ( GetProcAddress ( hKernel32, "LoadLibraryA" ) ),
                                        pLibPathRemote,
                                        0,
                                        NULL);

        if ( hThread == 0 )
        {
            return 0;
        }


    } __finally {
        _VirtualFreeEx( hProcess, pLibPathRemote, strlen ( szLibPath ) + 1, MEM_RELEASE );
    }

    /*  We wait for the created remote thread to finish executing. When it's done, the DLL
        is loaded into the game's userspace, and we can destroy the thread-handle. We wait
        5 seconds which is way longer than this should take to prevent this application
        from deadlocking if something goes really wrong allowing us to kill the injected
        game executable and avoid user inconvenience.*/
    WaitForObject ( hProcess, hThread, INFINITE, NULL );


    /* Get the handle of the remotely loaded DLL module */
    DWORD hLibModule = 0;
    GetExitCodeThread ( hThread, &hLibModule );


    /* Clean up the resources we used to inject the DLL */
    _VirtualFreeEx (hProcess, pLibPathRemote, strlen ( szLibPath ) + 1, MEM_RELEASE );

    // Allow GTA to continue
    RemoveWinMainBlock( hProcess );

    /* Success */
    return ( HINSTANCE )( 1 );
}


///////////////////////////////////////////////////////////////////////////
//
// GetWinMainAddress
//
// Return address of GTA WinMain function
//
///////////////////////////////////////////////////////////////////////////
uchar* GetWinMainAddress( HANDLE hProcess )
{
    #define WINMAIN_US  0x0748710
    #define WINMAIN_EU  0x0748760

    ushort buffer[1] = { 0 };
    _ReadProcessMemory ( hProcess, (void*)(WINMAIN_EU + 0x24), &buffer, sizeof( buffer ), NULL );
    if ( buffer[0] == 0x0F75 )     // jnz     short loc_748745
        return (uchar*)WINMAIN_EU;
    if ( buffer[0] == 0xEF3B )     // cmp     ebp, edi
        return (uchar*)WINMAIN_US;
    return NULL;
}


///////////////////////////////////////////////////////////////////////////
//
// WriteProcessMemoryChecked
//
// Poke bytes and do some checks as well
//
///////////////////////////////////////////////////////////////////////////
void WriteProcessMemoryChecked( HANDLE hProcess, void* dest, const void* src, uint size, const void* oldvalues, bool bStopIfOldIncorrect )
{
    DWORD oldProt1;
    _VirtualProtectEx ( hProcess, dest, size, PAGE_EXECUTE_READWRITE, &oldProt1 );

    // Verify previous value was expected were written ok
    if ( oldvalues )
    {
        char temp[30];
        uint numBytesToCheck = Min( sizeof( temp ), size );
        SIZE_T numBytesRead = 0;
        _ReadProcessMemory ( hProcess, dest, temp, numBytesToCheck, &numBytesRead );
        if ( memcmp( temp, oldvalues, numBytesToCheck ) )
        {
            WriteDebugEvent( SString( "Failed to verify %d old bytes in process", size ) );
            if ( bStopIfOldIncorrect )
                return;
        }
    }

    _WriteProcessMemory ( hProcess, dest, src, size, NULL );

    // Verify bytes were written ok
    {
        char temp[30];
        uint numBytesToCheck = Min( sizeof( temp ), size );
        SIZE_T numBytesRead = 0;
        _ReadProcessMemory ( hProcess, dest, temp, numBytesToCheck, &numBytesRead );
        if ( memcmp( temp, src, numBytesToCheck ) || numBytesRead != numBytesToCheck )
            WriteDebugEvent( SString( "Failed to write %d bytes to process", size ) );
    }

    DWORD oldProt2;
    _VirtualProtectEx ( hProcess, dest, size, oldProt1, &oldProt2 );
}


///////////////////////////////////////////////////////////////////////////
//
// InsertWinMainBlock
//
// Put an infinite loop at WinMain to stop GTA from running before we are ready
//
///////////////////////////////////////////////////////////////////////////
void InsertWinMainBlock( HANDLE hProcess )
{
    // Get location of WinMain function
    uchar* pWinMain = GetWinMainAddress( hProcess );
    if ( !pWinMain )
        return;

    WriteDebugEvent( "Loader - InsertWinMainBlock" );

    {
        const uchar oldCode[] = {
                                    0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
                                    0x90,
                                    0x90, 0x90,
                                    0x90, 0x90,
                                    0x90, 0x90,
                                    0x81, 0xEC, 0x84, 0x00, 0x00, 0x00, // WinMain  sub         esp,84h
                                    0x53,                               //          push        ebx
                                    0x6A, 0x02,                         //          push        2
                                };

        const uchar newCode[] = {
                                    0x3E, 0xA1, 0x7C, 0x71, 0x74, 0x00, //      lp: mov         eax,dword ptr ds:[0074717Ch] 
                                    0x48,                               //          dec         eax 
                                    0x74, 0xF7,                         //          je          lp
                                    0x6A, 0x02,                         //          push        2                           orig
                                    0xEB, 0x09,                         //          jmp         cont
                                    0x81, 0xEC, 0x84, 0x00, 0x00, 0x00, // WinMain  sub         esp,84h                     orig
                                    0x53,                               //          push        ebx                         orig
                                    0xEB, 0xEA,                         //          jmp         lp
                                };                                      //    cont:


        WriteProcessMemoryChecked( hProcess, pWinMain - sizeof( newCode ) + 9, newCode, sizeof( newCode ), oldCode, true );
    }

    uchar* pFlag = (uchar*)0x74717C;
    {
        const uchar oldCode[] = { 0x90, 0x90, 0x90, 0x90 };
        const uchar newCode[] = { 1, 0, 0, 0 };
        WriteProcessMemoryChecked( hProcess, pFlag, newCode, sizeof( newCode ), oldCode, false );
    }
}


///////////////////////////////////////////////////////////////////////////
//
// RemoveWinMainBlock
//
// Remove infinite loop at WinMain which stopped GTA from running before we were ready
//
///////////////////////////////////////////////////////////////////////////
void RemoveWinMainBlock( HANDLE hProcess )
{
    // Get location of WinMain function
    uchar* pWinMain = GetWinMainAddress( hProcess );
    if ( !pWinMain )
        return;

    WriteDebugEvent( "Loader - RemoveWinMainBlock" );

    uchar* pFlag = (uchar*)0x74717C;
    {
        const uchar oldCode[] = { 1, 0, 0, 0 };
        const uchar newCode[] = { 0, 0, 0, 0 };
        WriteProcessMemoryChecked( hProcess, pFlag, newCode, sizeof( newCode ), oldCode, false );
    }
}


///////////////////////////////////////////////////////////////////////////
//
// ApplyLoadingCrashPatch
//
// Modify GTA function IsAppAlreadyRunning() to avoid startup crash
//
///////////////////////////////////////////////////////////////////////////
void ApplyLoadingCrashPatch( HANDLE hProcess )
{
    // Get location of code to modify
    uchar* pAddress = GetWinMainAddress( hProcess );
    if ( !pAddress )
        return;

    WriteDebugEvent( "Loader - ApplyLoadingCrashPatch" );

    pAddress -= 0x1E17; // Offset from WinMain function

    const uchar oldCode[] = { 0xB7 };
    const uchar newCode[] = { 0x37 };
    WriteProcessMemoryChecked( hProcess, pAddress, newCode, sizeof( newCode ), oldCode, true );
}


///////////////////////////////////////////////////////////////////////////
//
// devicePathToWin32Path
//
// Code from the merky depths of MSDN
//
///////////////////////////////////////////////////////////////////////////
SString devicePathToWin32Path ( const SString& strDevicePath ) 
{
    TCHAR pszFilename[MAX_PATH+2];
    strncpy ( pszFilename, strDevicePath, MAX_PATH );
    pszFilename[MAX_PATH] = 0;

    // Translate path with device name to drive letters.
    TCHAR szTemp[1024];
    szTemp[0] = '\0';

    if (GetLogicalDriveStrings(NUMELMS(szTemp)-1, szTemp)) 
    {
        TCHAR szName[MAX_PATH];
        TCHAR szDrive[3] = TEXT(" :");
        BOOL bFound = FALSE;
        TCHAR* p = szTemp;

        do 
        {
            // Copy the drive letter to the template string
            *szDrive = *p;

            // Look up each device name
            if (QueryDosDevice(szDrive, szName, MAX_PATH))
            {
                UINT uNameLen = _tcslen(szName);

                if (uNameLen < MAX_PATH) 
                {
                    bFound = _tcsnicmp(pszFilename, szName, uNameLen) == 0;

                    if (bFound && *(pszFilename + uNameLen) == _T('\\')) 
                    {
                        // Reconstruct pszFilename using szTempFile
                        // Replace device path with DOS path
                        TCHAR szTempFile[MAX_PATH];
                        StringCchPrintf(szTempFile,
                        MAX_PATH,
                        TEXT("%s%s"),
                        szDrive,
                        pszFilename+uNameLen);
                        StringCchCopyN(pszFilename, MAX_PATH+1, szTempFile, _tcslen(szTempFile));
                    }
                }
            }
            // Go to the next NULL character.
            while (*p++);

        } while (!bFound && *p); // end of string
    }
    return pszFilename;
}


typedef WINBASEAPI BOOL (WINAPI *LPFN_QueryFullProcessImageNameA)(__in HANDLE hProcess, __in DWORD dwFlags, __out_ecount_part(*lpdwSize, *lpdwSize) LPSTR lpExeName, __inout PDWORD lpdwSize);

///////////////////////////////////////////////////////////////////////////
//
// GetPossibleProcessPathFilenames
//
// Get all image names for a processID
//
///////////////////////////////////////////////////////////////////////////
std::vector < SString > GetPossibleProcessPathFilenames ( DWORD processID )
{
    static LPFN_QueryFullProcessImageNameA fnQueryFullProcessImageNameA = NULL;
    static bool bDoneGetProcAddress = false;

    std::vector < SString > result;

    if ( !bDoneGetProcAddress )
    {
        // Find 'QueryFullProcessImageNameA'
        bDoneGetProcAddress = true;
        HMODULE hModule = GetModuleHandle ( "Kernel32.dll" );
        fnQueryFullProcessImageNameA = static_cast < LPFN_QueryFullProcessImageNameA > ( static_cast < PVOID > ( GetProcAddress( hModule, "QueryFullProcessImageNameA" ) ) );
    }

    if ( fnQueryFullProcessImageNameA )
    {
        for ( int i = 0 ; i < 2 ; i++ )
        {
            HANDLE hProcess = OpenProcess ( i == 0 ? PROCESS_QUERY_INFORMATION : PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processID );

            if ( hProcess )
            {
                TCHAR szProcessName[MAX_PATH] = TEXT("");
                DWORD dwSize = NUMELMS(szProcessName);
                DWORD bOk = fnQueryFullProcessImageNameA ( hProcess, 0, szProcessName, &dwSize );
                CloseHandle( hProcess );

                if ( bOk && strlen ( szProcessName ) > 0 )
                    ListAddUnique ( result, SString ( SStringX ( szProcessName ) ) );
            }
        }
    }

    {
        HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID );

        if ( hProcess )
        {
            TCHAR szProcessName[MAX_PATH] = TEXT("");
            DWORD bOk = GetModuleFileNameEx ( hProcess, NULL, szProcessName, NUMELMS(szProcessName) );
            CloseHandle ( hProcess );

            if ( bOk && strlen ( szProcessName ) > 0 )
                ListAddUnique ( result, SString ( SStringX ( szProcessName ) ) );
        }
    }

    for ( int i = 0 ; i < 2 ; i++ )
    {
        HANDLE hProcess = OpenProcess ( i == 0 ? PROCESS_QUERY_INFORMATION : PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processID );

        if ( hProcess )
        {
            TCHAR szProcessName[MAX_PATH] = TEXT("");
            DWORD bOk = GetProcessImageFileName ( hProcess, szProcessName, NUMELMS(szProcessName) );
            CloseHandle( hProcess );

            if ( bOk && strlen ( szProcessName ) > 0 )
                ListAddUnique ( result, SString ( SStringX ( devicePathToWin32Path ( szProcessName ) ) ) );
        }
    }

    return result;
}


///////////////////////////////////////////////////////////////////////////
//
// MyEnumProcesses
//
//
//
///////////////////////////////////////////////////////////////////////////
std::vector < DWORD > MyEnumProcesses ( void )
{
    uint uiSize = 200;
    std::vector < DWORD > processIdList;
    while ( true )
    {
        processIdList.resize ( uiSize );
        DWORD pBytesReturned = 0;
        if ( !EnumProcesses ( &processIdList[0], uiSize * sizeof(DWORD), &pBytesReturned ) )
        {
            processIdList.clear ();
            break;
        }
        uint uiNumProcessIds = pBytesReturned / sizeof(DWORD);

        if ( uiNumProcessIds != uiSize )
        {
            processIdList.resize ( uiNumProcessIds );
            break;
        }

        uiSize *= 2;
    }

    return processIdList;
}


///////////////////////////////////////////////////////////////////////////
//
// FindProcessId
//
// Find a process id by process name
//
///////////////////////////////////////////////////////////////////////////
DWORD FindProcessId ( const SString& processName )
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof ( processInfo );

	HANDLE processesSnapshot = CreateToolhelp32Snapshot ( TH32CS_SNAPPROCESS, NULL );
	if ( processesSnapshot == INVALID_HANDLE_VALUE )
		return 0;

	Process32First ( processesSnapshot , &processInfo );
    do
	{
		if ( processName.CompareI ( processInfo.szExeFile ) )
		{
			CloseHandle ( processesSnapshot );
			return processInfo.th32ProcessID;
		}
	}
	while ( Process32Next ( processesSnapshot, &processInfo ) );
	
	CloseHandle ( processesSnapshot );
	return 0;
}


///////////////////////////////////////////////////////////////////////////
//
// GetGTAProcessList
//
// Get list of process id's with the image name ending in "gta_sa.exe" or "proxy_sa.exe"
//
///////////////////////////////////////////////////////////////////////////
std::vector < DWORD > GetGTAProcessList ( void )
{
    std::vector < DWORD > result;

    std::vector < DWORD > processIdList = MyEnumProcesses ();
    for ( uint i = 0; i < processIdList.size (); i++ )
    {
        DWORD processId = processIdList[i];
        // Skip 64 bit processes to avoid errors
        if ( !Is32bitProcess ( processId ) )
            continue;

        std::vector < SString > filenameList = GetPossibleProcessPathFilenames ( processId );
        for ( uint i = 0; i < filenameList.size (); i++ )
            if ( filenameList[i].EndsWith ( MTA_GTAEXE_NAME ) || filenameList[i].EndsWith ( MTA_HTAEXE_NAME ) )
                ListAddUnique ( result, processId );
    }

    if ( DWORD processId = FindProcessId ( MTA_GTAEXE_NAME ) )
        ListAddUnique ( result, processId );

    if ( DWORD processId = FindProcessId ( MTA_HTAEXE_NAME ) )
        ListAddUnique ( result, processId );

    return result;
}


///////////////////////////////////////////////////////////////////////////
//
// IsGTARunning
//
//
//
///////////////////////////////////////////////////////////////////////////
bool IsGTARunning ( void )
{
    return !GetGTAProcessList ().empty ();
}


///////////////////////////////////////////////////////////////////////////
//
// TerminateGTAIfRunning
//
//
//
///////////////////////////////////////////////////////////////////////////
void TerminateGTAIfRunning ( void )
{
    std::vector < DWORD > processIdList = GetGTAProcessList ();

    if ( processIdList.size () )
    {
        // Try to stop all GTA process id's
        for ( uint i = 0 ; i < 3 && processIdList.size () ; i++ )
        {
            for ( std::vector < DWORD > ::iterator iter = processIdList.begin () ; iter != processIdList.end (); ++iter )
            {
                HANDLE hProcess = OpenProcess ( PROCESS_TERMINATE, 0, *iter );
                if ( hProcess )
                {
                    TerminateProcess ( hProcess, 0 );
                    CloseHandle ( hProcess );
                }
            }
            Sleep ( 1000 );
            processIdList = GetGTAProcessList ();
        }
    }
}


///////////////////////////////////////////////////////////////////////////
//
// GetOtherMTAProcessList
//
// Get list of process id's with the image name ending with the same name as this process
//
///////////////////////////////////////////////////////////////////////////
std::vector < DWORD > GetOtherMTAProcessList ( void )
{
    std::vector < DWORD > result;

    std::vector < DWORD > processIdList = MyEnumProcesses ();
    for ( uint i = 0; i < processIdList.size (); i++ )
    {
        DWORD processId = processIdList[i];
        // Skip 64 bit processes to avoid errors
        if ( !Is32bitProcess ( processId ) )
            continue;

        std::vector < SString > filenameList = GetPossibleProcessPathFilenames ( processId );
        for ( uint i = 0; i < filenameList.size (); i++ )
            if ( filenameList[i].EndsWith ( MTA_EXE_NAME ) )
                ListAddUnique ( result, processId );
    }

    if ( DWORD processId = FindProcessId ( MTA_EXE_NAME ) )
        ListAddUnique ( result, processId );

    // Ignore this process
    ListRemove ( result, GetCurrentProcessId () );

    return result;
}


///////////////////////////////////////////////////////////////////////////
//
// IsOtherMTARunning
//
//
//
///////////////////////////////////////////////////////////////////////////
bool IsOtherMTARunning ( void )
{
    return !GetOtherMTAProcessList ().empty ();
}


///////////////////////////////////////////////////////////////////////////
//
// TerminateOtherMTAIfRunning
//
//
//
///////////////////////////////////////////////////////////////////////////
void TerminateOtherMTAIfRunning ( void )
{
    std::vector < DWORD > processIdList = GetOtherMTAProcessList ();

    if ( processIdList.size () )
    {
        // Try to stop all other MTA process id's
        for ( uint i = 0 ; i < 3 && processIdList.size () ; i++ )
        {
            for ( std::vector < DWORD > ::iterator iter = processIdList.begin () ; iter != processIdList.end (); ++iter )
            {
                HANDLE hProcess = OpenProcess ( PROCESS_TERMINATE, 0, *iter );
                if ( hProcess )
                {
                    TerminateProcess ( hProcess, 0 );
                    CloseHandle ( hProcess );
                }
            }
            Sleep ( 1000 );
            processIdList = GetOtherMTAProcessList ();
        }
    }
}



//
// Return true if command line contains the string
//
bool CommandLineContains( const SString& strText )
{
    return SStringX( GetCommandLine() ).Contains( strText );
}


//
// General error message box
//
void DisplayErrorMessageBox ( const SString& strMessage, const SString& strErrorCode, const SString& strTroubleType )
{
    HideSplash ();
    MessageBoxUTF8( 0, strMessage, _("Error! (CTRL+C to copy)")+strErrorCode, MB_ICONEXCLAMATION|MB_OK | MB_TOPMOST );

    if ( strTroubleType != "" )
        BrowseToSolution ( strTroubleType, ASK_GO_ONLINE | TERMINATE_IF_YES );
}


SString GetMTASAModuleFileName ( void )
{
    // Get current module full path
    char szBuffer[64000];
    GetModuleFileName ( NULL, szBuffer, NUMELMS(szBuffer) - 1 );
    return szBuffer;
}


SString GetLaunchPath ( void )
{
    // Get current module full path
    char szBuffer[64000];
    GetModuleFileName ( NULL, szBuffer, NUMELMS(szBuffer) - 1 );

    // Strip the module name out of the path.
    PathRemoveFileSpec ( szBuffer );

    return szBuffer;
}


void SetMTASAPathSource ( bool bReadFromRegistry )
{
    if ( bReadFromRegistry )
    {
        g_strMTASAPath = GetMTASABaseDir();
    }
    else
    {
        // Get current module full path
        char szBuffer[64000];
        GetModuleFileName ( NULL, szBuffer, NUMELMS(szBuffer) - 1 );

        SString strHash = "-";
        {
            MD5 md5;
            CMD5Hasher Hasher;
            if ( Hasher.Calculate ( szBuffer, md5 ) )
            {
                char szHashResult[33];
                Hasher.ConvertToHex ( md5, szHashResult );
                strHash = szHashResult;
            }
        }

        SetRegistryValue ( "", "Last Run Path", szBuffer );
        SetRegistryValue ( "", "Last Run Path Hash", strHash );
        SetRegistryValue ( "", "Last Run Path Version", MTA_DM_ASE_VERSION );

        // Also save for legacy 1.0 to see
        SString strThisVersion = SStringX ( MTA_DM_ASE_VERSION ).TrimEnd ( "n" );
        SetVersionRegistryValueLegacy ( strThisVersion, "", "Last Run Path", szBuffer );
        SetVersionRegistryValueLegacy ( strThisVersion, "", "Last Run Path Hash", strHash );
        SetVersionRegistryValueLegacy ( strThisVersion, "", "Last Run Path Version", MTA_DM_ASE_VERSION );

        // Strip the module name out of the path.
        PathRemoveFileSpec ( szBuffer );

        // Save to a temp registry key
        SetRegistryValue ( "", "Last Run Location", szBuffer );
        g_strMTASAPath = szBuffer;
    }
}


SString GetMTASAPath ( void )
{
    if ( g_strMTASAPath == "" )
        SetMTASAPathSource ( false );
    return g_strMTASAPath;
}


///////////////////////////////////////////////////////////////
//
// LookForGtaProcess
//
//
//
///////////////////////////////////////////////////////////////
bool LookForGtaProcess ( SString& strOutPathFilename )
{
    std::vector < DWORD > processIdList = GetGTAProcessList ();
    for ( uint i = 0 ; i < processIdList.size () ; i++ )
    {
        std::vector < SString > filenameList = GetPossibleProcessPathFilenames ( processIdList[i] );
        for ( uint i = 0 ; i < filenameList.size () ; i++ )
        {
            if ( FileExists ( filenameList[i] ) )
            {
                strOutPathFilename = filenameList[i];
                return true;
            }
        }
    }
    return false;
}


///////////////////////////////////////////////////////////////
//
// DoUserAssistedSearch
//
//
//
///////////////////////////////////////////////////////////////
SString DoUserAssistedSearch ( void )
{
    SString strResult;

    ShowProgressDialog( g_hInstance, _("Searching for Grand Theft Auto San Andreas"), true );

    while ( !UpdateProgress ( 0, 100, _("Please start Grand Theft Auto San Andreas") ) )
    {
        SString strPathFilename;
        // Check if user has started GTA
        if ( LookForGtaProcess ( strPathFilename ) )
        {
            // If so, get the exe path
            ExtractFilename ( strPathFilename, &strResult, NULL );
            // And then stop it
            TerminateGTAIfRunning ();
            break;
        }

        Sleep ( 200 );
    }

    HideProgressDialog ();
    return strResult;
}


///////////////////////////////////////////////////////////////
//
// GetGamePath
//
//
//
///////////////////////////////////////////////////////////////
ePathResult GetGamePath ( SString& strOutResult, bool bFindIfMissing )
{
    // Registry places to look
    std::vector < SString > pathList;

    // Try HKLM "SOFTWARE\\Multi Theft Auto: San Andreas All\\Common\\"
    pathList.push_back ( GetCommonRegistryValue ( "", "GTA:SA Path" ) );
    // Then HKCU "SOFTWARE\\Multi Theft Auto: San Andreas 1.0\\"
    pathList.push_back ( GetVersionRegistryValueLegacy ( "1.0", "", "GTA:SA Path" ) );
    // Then HKCU "SOFTWARE\\Multi Theft Auto: San Andreas 1.1\\"
    pathList.push_back ( GetVersionRegistryValueLegacy ( "1.1", "", "GTA:SA Path Backup" ) );


    // Unicode character check on first one
    if ( strlen( pathList[0].c_str () ) )
    {
        // Check for replacement characters (?), to see if there are any (unsupported) unicode characters
        if ( strchr ( pathList[0].c_str (), '?' ) > 0 )
            return GAME_PATH_UNICODE_CHARS;
    }


    // Then step through looking for an existing file
    bool bFoundSteamExe = false;
    SString strRegPath;
    for ( uint i = 0 ; i < pathList.size (); i++ )
    {
        if ( FileExists( PathJoin ( pathList[i], MTA_GTAEXE_NAME ) ) )
        {
            strRegPath = pathList[i];
            break;
        }
        if ( FileExists( PathJoin ( pathList[i], MTA_GTASTEAMEXE_NAME ) ) )
        {
            bFoundSteamExe = true;
        }
    }

    // Found an exe?
    if ( !strRegPath.empty () )
    {
        strOutResult = strRegPath;
        // Update registry.
        SetCommonRegistryValue ( "", "GTA:SA Path", strOutResult );
        return GAME_PATH_OK;
    }

    // Found a steam exe?
    if ( bFoundSteamExe )
    {
        return GAME_PATH_STEAM;
    }

    // Try to find?
    if ( !bFindIfMissing )
        return GAME_PATH_MISSING;


    // Ask user to browse for GTA
    BROWSEINFO bi = { 0 };
    SString strMessage = _("Select your Grand Theft Auto: San Andreas Installation Directory");
    bi.lpszTitle = strMessage;
    LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );

    if ( pidl != 0 )
    {
        char szBuffer[MAX_PATH];
        // get the name of the  folder
        if ( SHGetPathFromIDListA ( pidl, szBuffer ) )
        {
            strOutResult = szBuffer;
        }

        // free memory used
        IMalloc * imalloc = 0;
        if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
        {
            imalloc->Free ( pidl );
            imalloc->Release ( );
        }
    }

    // Check browse result
    if ( !FileExists( PathJoin ( strOutResult, MTA_GTAEXE_NAME ) ) )
    {
        if ( FileExists( PathJoin ( strOutResult, MTA_GTASTEAMEXE_NAME ) ) )
            return GAME_PATH_STEAM;

        // If browse didn't help, try another method
        strOutResult = DoUserAssistedSearch ();

        if ( !FileExists( PathJoin ( strOutResult, MTA_GTAEXE_NAME ) ) )
        {
            if ( FileExists( PathJoin ( strOutResult, MTA_GTASTEAMEXE_NAME ) ) )
                return GAME_PATH_STEAM;

            // If still not found, give up
            return GAME_PATH_MISSING;
        }
    }

    // File found. Update registry.
    SetCommonRegistryValue ( "", "GTA:SA Path", strOutResult );
    return GAME_PATH_OK;
}


///////////////////////////////////////////////////////////////
//
// DiscoverGTAPath
//
// Find and cache GTA path
//
///////////////////////////////////////////////////////////////
ePathResult DiscoverGTAPath ( bool bFindIfMissing )
{
    return GetGamePath ( g_strGTAPath, bFindIfMissing );
}


///////////////////////////////////////////////////////////////
//
// GetGTAPath
//
// 
//
///////////////////////////////////////////////////////////////
SString GetGTAPath ( void )
{
    if ( g_strGTAPath == "" )
        DiscoverGTAPath( false );
    return g_strGTAPath;
}


///////////////////////////////////////////////////////////////
//
// FindFilesRecursive
//
// Return a list of files inside strPath
//
///////////////////////////////////////////////////////////////
void FindFilesRecursive ( const SString& strPathMatch, std::vector < SString >& outFileList, uint uiMaxDepth )
{
    SString strPath, strMatch;
    strPathMatch.Split ( "\\", &strPath, &strMatch, -1 );

    std::list < SString > toDoList;
    toDoList.push_back ( strPath );
    while ( toDoList.size () )
    {
        SString strPathHere = toDoList.front ();
        toDoList.pop_front ();

        std::vector < SString > fileListHere = FindFiles ( strPathHere + "\\" + strMatch, true, false );
        std::vector < SString > dirListHere = FindFiles ( strPathHere + "\\" + strMatch, false, true );

        for ( unsigned int i = 0 ; i < fileListHere.size (); i++ )
        {
            SString filePathName = strPathHere + "\\" + fileListHere[i];
            outFileList.push_back ( filePathName );
        }
        for ( unsigned int i = 0 ; i < dirListHere.size (); i++ )
        {
            SString dirPathName = strPathHere + "\\" + dirListHere[i];
            toDoList.push_back ( dirPathName );
        }
    }
}


///////////////////////////////////////////////////////////////
//
// FindRelevantFiles
//
// Return a list of files and directories insode strPath, but not inside any dir in stopList
//
///////////////////////////////////////////////////////////////
void FindRelevantFiles ( const SString& strPath, std::vector < SString >& outFilePathList, std::vector < SString >& outDirPathList, const std::vector < SString >& stopList, unsigned int MaxFiles, unsigned int MaxDirs )
{
    std::list < SString > toDoList;
    toDoList.push_back ( strPath );
    outDirPathList.push_back ( strPath );
    while ( toDoList.size () )
    {
        {
            // Update progress bar if visible
            int NumItems = outFilePathList.size () + outDirPathList.size ();
            int MaxItems = ( MaxFiles ? MaxFiles : 25000 ) + ( MaxDirs ? MaxDirs : 5000 );
            if ( UpdateProgress ( Min ( NumItems, MaxItems ), MaxItems * 2, "Checking files..." ) )
                return;
        }

        SString strPathHere = toDoList.front ();
        toDoList.pop_front ();

        std::vector < SString > fileListHere = FindFiles ( strPathHere + "\\*", true, false );
        std::vector < SString > dirListHere = FindFiles ( strPathHere + "\\*", false, true );

        for ( unsigned int i = 0 ; i < fileListHere.size (); i++ )
        {
            SString filePathName = strPathHere + "\\" + fileListHere[i];
            outFilePathList.push_back ( filePathName );
        }
        for ( unsigned int i = 0 ; i < dirListHere.size (); i++ )
        {
            SString dirPathName = strPathHere + "\\" + dirListHere[i];
            outDirPathList.push_back ( dirPathName );

            bool bTraverse = true;
            for ( unsigned int k = 0 ; k < stopList.size (); k++ )
                if ( dirListHere[i].length () >= stopList[k].length () )
                    if ( dirListHere[i].ToLower ().substr ( 0, stopList[k].length () ) == stopList[k] )
                        bTraverse = false;
            if ( bTraverse )
                toDoList.push_back ( dirPathName );
        }

        if ( MaxFiles && outFilePathList.size () > MaxFiles )
            if ( MaxDirs && outDirPathList.size () > MaxDirs )
                break;
    }
}


///////////////////////////////////////////////////////////////
//
// MakeRandomIndexList
//
// Create a list of randomlu ordered indices from 0 to Size-1
//
///////////////////////////////////////////////////////////////
void MakeRandomIndexList ( int Size, std::vector < int >& outList )
{
    for ( int i = 0 ; i < Size ; i ++ )
        outList.push_back ( i );

    for ( int i = 0 ; i < Size ; i ++ )
    {
        int otherIdx = rand() % Size;
        int Temp = outList[i];
        outList[i] = outList[otherIdx];
        outList[otherIdx] = Temp;
    }
}


///////////////////////////////////////////////////////////////
//
// GetOSVersion
//
// Affected by compatibility mode
//
///////////////////////////////////////////////////////////////
SString GetOSVersion ( void )
{
    OSVERSIONINFO versionInfo;
    memset ( &versionInfo, 0, sizeof ( versionInfo ) );
    versionInfo.dwOSVersionInfoSize = sizeof ( versionInfo );
    GetVersionEx ( &versionInfo );
    return SString ( "%d.%d", versionInfo.dwMajorVersion, versionInfo.dwMinorVersion );
}


///////////////////////////////////////////////////////////////
//
// GetRealOSVersion
//
// Ignoring compatibility mode
//
///////////////////////////////////////////////////////////////
SString GetRealOSVersion ( void )
{
    SString strVersionAndBuild = GetWMIOSVersion ();
    std::vector < SString > parts;
    strVersionAndBuild.Split ( ".", parts );
    uint uiMajor = parts.size () > 0 ? atoi ( parts[0] ) : 0;
    uint uiMinor = parts.size () > 1 ? atoi ( parts[1] ) : 0;

    if ( uiMajor == 0 )
    {
        SLibVersionInfo fileInfo;
        if ( GetLibVersionInfo ( "ntdll.dll", &fileInfo ) )
        {
            uiMajor = HIWORD( fileInfo.dwFileVersionMS );
            uiMinor = LOWORD( fileInfo.dwFileVersionMS );
        }
    }

    return SString ( "%u.%u", uiMajor, uiMinor );
}


///////////////////////////////////////////////////////////////
//
// IsUserAdmin
//
//
//
///////////////////////////////////////////////////////////////
BOOL IsUserAdmin(VOID)
/*++ 
Routine Description: This routine returns TRUE if the caller's
process is a member of the Administrators local group. Caller is NOT
expected to be impersonating anyone and is expected to be able to
open its own process and process token. 
Arguments: None. 
Return Value: 
   TRUE - Caller has Administrators local group. 
   FALSE - Caller does not have Administrators local group. --
*/ 
{
    BOOL b;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    PSID AdministratorsGroup; 
    b = AllocateAndInitializeSid(
        &NtAuthority,
        2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &AdministratorsGroup); 
    if(b) 
    {
        if (!CheckTokenMembership( NULL, AdministratorsGroup, &b)) 
        {
             b = FALSE;
        } 
        FreeSid(AdministratorsGroup); 
    }

    return(b);
}


///////////////////////////////////////////////////////////////
//
// IsVistaOrHigher
//
//
//
///////////////////////////////////////////////////////////////
bool IsVistaOrHigher ( void )
{
    int iMajor = atoi ( GetRealOSVersion () );
    return iMajor >= 6;
}



static SString HashBuffer ( char* pData, uint uiLength )
{
    DWORD dwSum1 = 0;
    DWORD dwSum2 = 0x1234;
    for ( uint i = 0 ; i < uiLength ; i++ )
    {
        dwSum1 += pData[i];
        dwSum2 += pData[i];
        dwSum2 ^= ( dwSum2 << 2 ) + 0x93;
    }
    return SString ( "%08x%08x%08x", dwSum1, dwSum2, uiLength );
}


/////////////////////////////////////////////////////////////////////
//
// GetLibraryHandle
//
//
//
/////////////////////////////////////////////////////////////////////
HMODULE GetLibraryHandle ( const SString& strFilename )
{
    if ( !hLibraryModule )
    {
        // Get path to the relevant file
        SString strLibPath = PathJoin ( GetLaunchPath (), "mta" );
        SString strLibPathFilename = PathJoin ( strLibPath, strFilename );

        SString strPrevCurDir = GetSystemCurrentDirectory ();
        SetCurrentDirectory ( strLibPath );
        SetDllDirectory( strLibPath );

        hLibraryModule = LoadLibrary ( strLibPathFilename );

        SetCurrentDirectory ( strPrevCurDir );
        SetDllDirectory( strPrevCurDir );
    }

    return hLibraryModule;
}


/////////////////////////////////////////////////////////////////////
//
// FreeLibraryHandle
//
//
//
/////////////////////////////////////////////////////////////////////
void FreeLibraryHandle ( void )
{
    if ( hLibraryModule )
    {
        FreeLibrary ( hLibraryModule );
        hLibraryModule = NULL;
    }
}


/////////////////////////////////////////////////////////////////////
//
// UpdateMTAVersionApplicationSetting
//
// Make sure "mta-version-ext" is correct. eg "1.0.4-9.01234.2.000"
//
/////////////////////////////////////////////////////////////////////
void UpdateMTAVersionApplicationSetting ( bool bQuiet )
{
#ifdef MTA_DEBUG
    SString strFilename = "netc_d.dll";
#else
    SString strFilename = "netc.dll";
#endif

    //
    // Determine NetRev number
    //

    // Get saved status
    unsigned short usNetRev = 65535;
    unsigned short usNetRel = 0;
    std::vector < SString > parts;
    GetApplicationSetting ( "mta-version-ext" ).Split ( ".", parts );
    if ( parts.size () == 6 )
    {
        usNetRev = atoi ( parts[4] );
        usNetRel = atoi ( parts[5] );
    }

    HMODULE hModule = GetLibraryHandle ( strFilename );
    if ( hModule )
    {
        typedef unsigned short (*PFNGETNETREV) ( void );
        PFNGETNETREV pfnGetNetRev = static_cast < PFNGETNETREV > ( static_cast < PVOID > ( GetProcAddress ( hModule, "GetNetRev" ) ) );
        if ( pfnGetNetRev )
            usNetRev = pfnGetNetRev ();
        PFNGETNETREV pfnGetNetRel = static_cast < PFNGETNETREV > ( static_cast < PVOID > ( GetProcAddress ( hModule, "GetNetRel" ) ) );
        if ( pfnGetNetRel )
            usNetRel = pfnGetNetRel ();
    }
    else
    if ( !bQuiet )
    {
        SString strError = GetSystemErrorMessage ( GetLastError () );            
        SString strMessage( _("Error loading %s module! (%s)"), *strFilename.ToLower (), *strError );
        BrowseToSolution ( strFilename + "-not-loadable", ASK_GO_ONLINE | TERMINATE_PROCESS, strMessage );
    }

    if ( !bQuiet )
        SetApplicationSetting ( "mta-version-ext", SString ( "%d.%d.%d-%d.%05d.%c.%03d"
                                ,MTASA_VERSION_MAJOR
                                ,MTASA_VERSION_MINOR
                                ,MTASA_VERSION_MAINTENANCE
                                ,MTASA_VERSION_TYPE
                                ,MTASA_VERSION_BUILD
                                ,usNetRev == 65535 ? '+' : usNetRev + '0'
                                ,usNetRel
                                ) );
}


///////////////////////////////////////////////////////////////////////////
//
// Is32bitProcess
//
// Determine if processID is a 32 bit process or not.
// Assumes current process is 32 bit.
//
// (Calling GetModuleFileNameEx or EnumProcessModules on a 64bit process from a 32bit process can cause problems)
//
///////////////////////////////////////////////////////////////////////////
bool Is32bitProcess ( DWORD processID )
{
    typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
    static LPFN_ISWOW64PROCESS fnIsWow64Process = NULL;
    static bool bDoneGetProcAddress = false;
    static bool bDoneIs64BitOS = false;
    static bool bIs64BitOS = false;

    if ( !bDoneGetProcAddress )
    {
        // Find 'IsWow64Process'
        bDoneGetProcAddress = true;
        HMODULE hModule = GetModuleHandle ( "Kernel32.dll" );
        fnIsWow64Process = static_cast < LPFN_ISWOW64PROCESS > ( static_cast < PVOID > ( GetProcAddress( hModule, "IsWow64Process" ) ) );
    }

    // Function not there? Must be 32bit everything
    if ( !fnIsWow64Process )
        return true;


    // See if this is a 64 bit O/S
    if ( !bDoneIs64BitOS )
    {
        bDoneIs64BitOS = true;

        // We know current process is 32 bit. See if it is running under WOW64
        BOOL bIsWow64 = FALSE;
        BOOL bOk = fnIsWow64Process ( GetCurrentProcess (), &bIsWow64 );
        if ( bOk )
        {
            // Must be 64bit O/S if current (32 bit) process is running under WOW64
            if ( bIsWow64 )
                bIs64BitOS = true;
        }
    }

    // Not 64 bit O/S? Must be 32bit everything
    if ( !bIs64BitOS )
        return true;

    // Call 'IsWow64Process' on query process
    for ( int i = 0 ; i < 2 ; i++ )
    {
        HANDLE hProcess = OpenProcess( i == 0 ? PROCESS_QUERY_INFORMATION : PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processID );

        if ( hProcess )
        {
            BOOL bIsWow64 = FALSE;
            BOOL bOk = fnIsWow64Process ( hProcess, &bIsWow64 );
            CloseHandle( hProcess );

            if ( bOk )
            {
                if ( bIsWow64 == FALSE )
                    return false;       // 64 bit O/S and process not running under WOW64, so it must be a 64 bit process
                return true;
            }
        }
    }

    return false;   // Can't determine. Guess it's 64 bit
}


///////////////////////////////////////////////////////////////////////////
//
// CreateSingleInstanceMutex
//
//
//
///////////////////////////////////////////////////////////////////////////
bool CreateSingleInstanceMutex ( void )
{
    HANDLE hMutex = CreateMutex ( NULL, FALSE, TEXT( MTA_GUID ) );

    if ( GetLastError() == ERROR_ALREADY_EXISTS )
    {
        if ( hMutex )
            CloseHandle ( hMutex );
        return false;
    }
    assert ( !g_hMutex );
    g_hMutex = hMutex;
    return true;
}


///////////////////////////////////////////////////////////////////////////
//
// ReleaseSingleInstanceMutex
//
//
//
///////////////////////////////////////////////////////////////////////////
void ReleaseSingleInstanceMutex ( void )
{
    assert ( g_hMutex );
    CloseHandle ( g_hMutex );
    g_hMutex = NULL;
}


///////////////////////////////////////////////////////////////////////////
//
// WaitForObject
//
// Wait for object to finish
// Returns non-zero if wait failed.
//
///////////////////////////////////////////////////////////////////////////
uint WaitForObject ( HANDLE hProcess, HANDLE hThread, DWORD dwMilliseconds, HANDLE hMutex )
{
    uint uiResult = 0;

    HMODULE hModule = GetLibraryHandle ( "kernel32.dll" );

    if ( hModule )
    {
        typedef unsigned long (*PFNWaitForObject) ( HANDLE, ... );
        PFNWaitForObject pfnWaitForObject = static_cast< PFNWaitForObject > ( static_cast < PVOID > ( GetProcAddress ( hModule, "WaitForObject" ) ) );

        if ( !pfnWaitForObject || pfnWaitForObject ( hProcess, hThread, dwMilliseconds, hMutex ) )
            uiResult = 1;
    }

    return uiResult;
}


///////////////////////////////////////////////////////////////////////////
//
// CheckService
//
// Check service status
// Returns false on fail
//
///////////////////////////////////////////////////////////////////////////
bool CheckService ( uint uiStage )
{
    HMODULE hModule = GetLibraryHandle ( "kernel32.dll" );

    if ( hModule )
    {
        typedef bool (*PFNCheckService) ( uint );
        PFNCheckService pfnCheckService = static_cast< PFNCheckService > ( static_cast < PVOID > ( GetProcAddress ( hModule, "CheckService" ) ) );

        if ( pfnCheckService )
        {
            bool bResult = pfnCheckService ( uiStage );
            AddReportLog ( 8070, SString ( "CheckService %d result: %d", uiStage, bResult ) );
            return bResult;
        }
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////
//
// GetFileAge
//
// Returns time in seconds since a file/directory was created
//
///////////////////////////////////////////////////////////////////////////
int GetFileAge ( const SString& strPathFilename )
{
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile ( strPathFilename, &findFileData );
    if ( hFind != INVALID_HANDLE_VALUE )
    {
        FindClose ( hFind );
        FILETIME ftNow;
        GetSystemTimeAsFileTime ( &ftNow );
        LARGE_INTEGER creationTime = { findFileData.ftCreationTime.dwLowDateTime, findFileData.ftCreationTime.dwHighDateTime };
        LARGE_INTEGER timeNow = { ftNow.dwLowDateTime, ftNow.dwHighDateTime };
        return static_cast < int > ( ( timeNow.QuadPart - creationTime.QuadPart ) / ( LONGLONG ) 10000000 );
    }
    return 0;
}


///////////////////////////////////////////////////////////////////////////
//
// CleanDownloadCache
//
// Remove old files from the download cache
//
///////////////////////////////////////////////////////////////////////////
void CleanDownloadCache ( void )
{
    const uint uiMaxCleanTime = 5;                      // Limit clean time (seconds)
    const uint uiCleanFileAge = 60 * 60 * 24 * 7;       // Delete files older than this

    const time_t tMaxEndTime = time ( NULL ) + uiMaxCleanTime;

    // Search possible cache locations
    std::list < SString > cacheLocationList;
    cacheLocationList.push_back ( PathJoin ( GetMTADataPath (), "upcache" ) );
    cacheLocationList.push_back ( PathJoin ( GetMTATempPath (), "upcache" ) );
    cacheLocationList.push_back ( GetMTATempPath () );

    for ( ; !cacheLocationList.empty () ; cacheLocationList.pop_front () )
    {
        // Get list of files & directories in this cache location
        const SString& strCacheLocation = cacheLocationList.front ();
        const std::vector < SString > fileList = FindFiles ( PathJoin ( strCacheLocation, "\\*" ), true, true );

        for ( uint i = 0 ; i < fileList.size () ; i++ )
        {
            const SString strPathFilename = PathJoin ( strCacheLocation, fileList[i] );
            // Check if over 7 days old
            if ( GetFileAge ( strPathFilename ) > uiCleanFileAge )
            {
                // Delete as directory or file
                if ( DirectoryExists ( strPathFilename ) )
                    DelTree ( strPathFilename, strCacheLocation );
                else
                    FileDelete ( strPathFilename );

                // Check time spent
                if ( time ( NULL ) > tMaxEndTime )
                    break;
            }
        }
    }
}


//////////////////////////////////////////////////////////
//
// IsDirectoryEmpty
//
// Returns true if directory does not exist, or it is empty
//
//////////////////////////////////////////////////////////
bool IsDirectoryEmpty ( const SString& strSrcBase )
{
    return FindFiles ( PathJoin ( strSrcBase, "*" ), true, true ).empty ();
}


//////////////////////////////////////////////////////////
//
// GetDiskFreeSpace
//
// Get free disk space in bytes
//
//////////////////////////////////////////////////////////
long long GetDiskFreeSpace ( SString strSrcBase )
{
    for ( uint i = 0 ; i < 100 ; i++ )
    {
        ULARGE_INTEGER FreeBytesAvailable;
        if ( GetDiskFreeSpaceEx ( strSrcBase, &FreeBytesAvailable, NULL, NULL ) )
            return FreeBytesAvailable.QuadPart;

        strSrcBase = ExtractPath( strSrcBase );
    }
    return 0;
}


//////////////////////////////////////////////////////////
//
// DirectoryCopy
//
// Recursive directory copy
//
//
//////////////////////////////////////////////////////////
void DirectoryCopy ( SString strSrcBase, SString strDestBase, bool bShowProgressDialog = false, int iMinFreeSpaceMB = 1 )
{
    // Setup diskspace checking
    bool bCheckFreeSpace = false;
    long long llFreeBytesAvailable = GetDiskFreeSpace ( strDestBase );
    if ( llFreeBytesAvailable != 0 )
        bCheckFreeSpace = ( llFreeBytesAvailable < ( iMinFreeSpaceMB + 10000 ) * 0x100000LL );    // Only check if initial freespace is less than 10GB

    if ( bShowProgressDialog )
        ShowProgressDialog( g_hInstance, _("Copying files..."), true );

    strSrcBase = PathConform ( strSrcBase ).TrimEnd ( PATH_SEPERATOR );
    strDestBase = PathConform ( strDestBase ).TrimEnd ( PATH_SEPERATOR );

    float fProgress = 0;
    float fUseProgress = 0;
    std::list < SString > toDoList;
    toDoList.push_back ( "" );
    while ( toDoList.size () )
    {
        fProgress += 0.5f;
        fUseProgress = fProgress;
        if ( fUseProgress > 50 )
            fUseProgress = Min ( 100.f, pow ( fUseProgress - 50, 0.6f ) + 50 );

        SString strPathHereBaseRel = toDoList.front ();
        toDoList.pop_front ();

        SString strPathHereSrc = PathJoin ( strSrcBase, strPathHereBaseRel );
        SString strPathHereDest = PathJoin ( strDestBase, strPathHereBaseRel );

        std::vector < SString > fileListHere = FindFiles ( PathJoin ( strPathHereSrc, "*" ), true, false );
        std::vector < SString > dirListHere = FindFiles ( PathJoin ( strPathHereSrc, "*" ), false, true );

        // Copy the files at this level
        for ( unsigned int i = 0 ; i < fileListHere.size (); i++ )
        {
            SString filePathNameSrc = PathJoin ( strPathHereSrc, fileListHere[i] );
            SString filePathNameDest = PathJoin ( strPathHereDest, fileListHere[i] );
            if ( bShowProgressDialog )
                if ( UpdateProgress ( (int)fUseProgress, 100, SString ( "...%s", *PathJoin ( strPathHereBaseRel, fileListHere[i] ) ) ) )
                    goto stop_copy;

            // Check enough disk space
            if ( bCheckFreeSpace )
            {
                llFreeBytesAvailable -= FileSize ( filePathNameSrc );
                if ( llFreeBytesAvailable / 0x100000LL < iMinFreeSpaceMB )
                    goto stop_copy;
            }

            FileCopy ( filePathNameSrc, filePathNameDest, true );
        }

        // Add the directories at this level
        for ( unsigned int i = 0 ; i < dirListHere.size (); i++ )
        {
            SString dirPathNameBaseRel = PathJoin ( strPathHereBaseRel, dirListHere[i] );
            toDoList.push_back ( dirPathNameBaseRel );
        }
    }

stop_copy:
    if ( bShowProgressDialog )
    {
        // Reassuring messages
        if ( toDoList.size () )
        {
            Sleep ( 1000 );
            UpdateProgress ( (int)fUseProgress, 100, _("Copy finished early. Everything OK.") );
            Sleep ( 2000 );
        }
        else
        {
            fUseProgress = Max ( 90.f, fUseProgress );
            UpdateProgress ( (int)fUseProgress, 100, _("Finishing...") );
            Sleep ( 1000 );
            UpdateProgress ( 100, 100, _("Done!") );
            Sleep ( 2000 );
        }

        HideProgressDialog ();
    }
}


//////////////////////////////////////////////////////////
//
// MaybeShowCopySettingsDialog
//
// For new installs, give the user an option to copy
// settings from a previous version
//
//////////////////////////////////////////////////////////
void MaybeShowCopySettingsDialog ( void )
{
    // Check if coreconfig.xml is present
    const SString strMTASAPath = GetMTASAPath ();
    SString strCurrentConfig = PathJoin ( GetMTASAPath (), "mta", "coreconfig.xml" );
    if ( FileExists ( strCurrentConfig ) )
        return;

    // Check if coreconfig.xml from previous version is present
    SString strCurrentVersion = SString ( "%d.%d", MTASA_VERSION_MAJOR, MTASA_VERSION_MINOR );
    SString strPreviousVersion = SString ( "%d.%d", MTASA_VERSION_MAJOR, MTASA_VERSION_MINOR - 1 );
    SString strPreviousPath = GetVersionRegistryValue ( strPreviousVersion, "", "Last Run Location" );
    if ( strPreviousPath.empty () )
        return;
    SString strPreviousConfig = PathJoin ( strPreviousPath, "mta", "coreconfig.xml" );
    if ( !FileExists ( strPreviousConfig ) )
        return;

    HideSplash ();  // Hide standard MTA splash

    // Show dialog
    SString strMessage;
    strMessage += SString( _( "New installation of %s detected.\n"
                              "\n"
                              "Do you want to copy your settings from %s ?" ),
                                *strCurrentVersion,
                                *strPreviousVersion  );
    int iResponse = MessageBoxUTF8 ( NULL, strMessage, "MTA: San Andreas", MB_YESNO | MB_ICONQUESTION | MB_TOPMOST );
    if ( iResponse != IDYES )
        return;

    // Copy settings from previous version
    FileCopy ( strPreviousConfig, strCurrentConfig );

    // Copy registry setting for aero-enabled
    SString strAeroEnabled = GetVersionRegistryValue ( strPreviousVersion, PathJoin ( "Settings", "general" ) , "aero-enabled" );
    SetApplicationSetting ( "aero-enabled", strAeroEnabled );

    // Copy some directories if empty
    SString strCurrentNewsDir = PathJoin ( GetMTADataPath (), "news" );
    SString strCurrentPrivDir = PathJoin ( GetMTASAPath (), "mods", "deathmatch", "priv" );
    SString strCurrentResourcesDir = PathJoin ( GetMTASAPath (), "mods", "deathmatch", "resources" );

    SString strPreviousDataPath = PathJoin ( GetSystemCommonAppDataPath(), "MTA San Andreas All", strPreviousVersion );
    SString strPreviousNewsDir = PathJoin ( strPreviousDataPath, "news" );
    SString strPreviousPrivDir = PathJoin ( strPreviousPath, "mods", "deathmatch", "priv" );
    SString strPreviousResourcesDir = PathJoin ( strPreviousPath, "mods", "deathmatch", "resources" );

    if ( IsDirectoryEmpty( strCurrentNewsDir ) && DirectoryExists( strPreviousNewsDir ) )
        DirectoryCopy ( strPreviousNewsDir, strCurrentNewsDir );

    if ( IsDirectoryEmpty( strCurrentPrivDir ) && DirectoryExists( strPreviousPrivDir ) )
        DirectoryCopy ( strPreviousPrivDir, strCurrentPrivDir );

    if ( IsDirectoryEmpty( strCurrentResourcesDir ) && DirectoryExists( strPreviousResourcesDir ) )
        DirectoryCopy ( strPreviousResourcesDir, strCurrentResourcesDir, true, 100 );
}


//////////////////////////////////////////////////////////
//
// CheckAndShowFileOpenFailureMessage
//
// Returns true if message was displayed
//
//////////////////////////////////////////////////////////
bool CheckAndShowFileOpenFailureMessage ( void )
{
    SString strFilename = GetApplicationSetting ( "diagnostics", "gta-fopen-fail" );

    if ( !strFilename.empty () )
    {
        //SetApplicationSetting ( "diagnostics", "gta-fopen-fail", "" );
        SString strMsg ( _("GTA:SA had trouble opening the file '%s'\n\nTry reinstalling GTA:SA to fix it"), *strFilename );
        MessageBoxUTF8 ( NULL, strMsg, "MTA: San Andreas"+_E("CL27"), MB_OK | MB_ICONERROR | MB_TOPMOST );
        return true;
    }
    return false;
}


//////////////////////////////////////////////////////////
//
// CheckAndShowMissingFileMessage
//
// Check for missing files that could cause a crash
//
//////////////////////////////////////////////////////////
void CheckAndShowMissingFileMessage ( void )
{
    SString strFilename = PathJoin( "text", "american.gxt" );

    const SString strGTAPathFilename = PathJoin( GetGTAPath(), strFilename );

    if ( !FileExists( strGTAPathFilename ) )
    {
        SString strMsg ( _("GTA:SA is missing the file '%s'."), *strFilename );
        strMsg += "\n\n";
        strMsg += _("Do you want to see some online help?");
        int iResponse = MessageBoxUTF8 ( NULL, strMsg, "MTA: San Andreas"+_E("CL31"), MB_YESNO | MB_ICONERROR | MB_TOPMOST );
        if ( iResponse == IDYES )
        {
            BrowseToSolution ( SString( "gta-file-missing&name=%s", *strFilename ), TERMINATE_PROCESS );
        }
    }
}


//////////////////////////////////////////////////////////
//
// LoadFunction
//
// Load a library function
//
//////////////////////////////////////////////////////////
void* LoadFunction( const char* szLibName, const char* c, const char* a, const char* b )
{
    static std::map < SString, HMODULE > libMap;
    HMODULE* phModule = MapFind( libMap, szLibName );
    if ( !phModule )
    {
        MapSet( libMap, szLibName, LoadLibrary( szLibName ) );
        phModule = MapFind( libMap, szLibName );
    }
    SString strFunctionName( "%s%s%s", a, b, c );
    return static_cast < PVOID >( GetProcAddress( *phModule, strFunctionName ) );
}


//////////////////////////////////////////////////////////
//
// BsodDetectionPreLaunch
//
// Possible BSOD situation if a new mini-dump file was created after the last game was started
//
//////////////////////////////////////////////////////////
void BsodDetectionPreLaunch( void )
{
    // Find latest system minidump file
    SString strMatch = PathJoin( GetSystemWindowsPath(), "MiniDump", "*" );
    SString strMinidumpTime;
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile( strMatch, &findData );
    if( hFind != INVALID_HANDLE_VALUE )
    {
        do
        {
            if ( ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == false )
                if ( strcmp( findData.cFileName, "." ) && strcmp( findData.cFileName, ".." ) )
                {
                    SYSTEMTIME s;
                    FileTimeToSystemTime( &findData.ftCreationTime, &s );
                    SString strCreationTime( "%02d-%02d-%02d %02d:%02d:%02d", s.wYear, s.wMonth, s.wDay, s.wHour, s.wMinute, s.wSecond );
                    if ( strCreationTime > strMinidumpTime )
                        strMinidumpTime = strCreationTime;
                }
        }
        while( FindNextFile( hFind, &findData ) );
        FindClose( hFind );
    }

    // Is it a new file?
    SString strLastMinidumpTime = GetApplicationSetting( "diagnostics", "last-minidump-time" );
    if ( strMinidumpTime > strLastMinidumpTime )
    {
        SetApplicationSetting( "diagnostics", "last-minidump-time", strMinidumpTime );
        IncApplicationSettingInt( DIAG_MINIDUMP_DETECTED_COUNT );

        // Was it created during the game?
        SString strGameBeginTime = GetApplicationSetting( "diagnostics", "game-begin-time" );
        if ( strMinidumpTime > strGameBeginTime && !strGameBeginTime.empty() )
        {
            // Ask user to confirm
            int iResponse = MessageBoxUTF8 ( NULL, _("Did your computer restart when playing MTA:SA?"), "MTA: San Andreas", MB_YESNO | MB_ICONERROR | MB_TOPMOST );
            if ( iResponse == IDYES )
            {
                SetApplicationSetting( "diagnostics", "user-confirmed-bsod-time", strMinidumpTime );
                IncApplicationSettingInt( DIAG_MINIDUMP_CONFIRMED_COUNT );
            }
        }
    }

    // Log BSOD status
    SString strBsodTime = GetApplicationSetting( "diagnostics", "user-confirmed-bsod-time" );
    if ( !strBsodTime.empty() )
        WriteDebugEvent( SString( "User confirmed bsod time: %s", *strBsodTime ) );
}


//////////////////////////////////////////////////////////
//
// BsodDetectionOnGameBegin
//
// Record game start time
//
//////////////////////////////////////////////////////////
void BsodDetectionOnGameBegin( void )
{
    SetApplicationSetting( "diagnostics", "game-begin-time", GetTimeString( true ) );
}


//////////////////////////////////////////////////////////
//
// BsodDetectionOnGameEnd
//
// Unrecord game start time
//
//////////////////////////////////////////////////////////
void BsodDetectionOnGameEnd( void )
{
    SetApplicationSetting( "diagnostics", "game-begin-time", "" );
}


//////////////////////////////////////////////////////////
//
// VerifyEmbeddedSignature
//
// Check a file has been signed proper
//
//////////////////////////////////////////////////////////
bool VerifyEmbeddedSignature( const WString& strFilename )
{
    WINTRUST_FILE_INFO FileData;
    memset(&FileData, 0, sizeof(FileData));
    FileData.cbStruct = sizeof(WINTRUST_FILE_INFO);
    FileData.pcwszFilePath = *strFilename;

    WINTRUST_DATA WinTrustData;
    memset(&WinTrustData, 0, sizeof(WinTrustData));
    WinTrustData.cbStruct = sizeof(WinTrustData);
    WinTrustData.dwUIChoice = WTD_UI_NONE;
    WinTrustData.fdwRevocationChecks = WTD_REVOKE_NONE; 
    WinTrustData.dwUnionChoice = WTD_CHOICE_FILE;
    WinTrustData.pFile = &FileData;

    GUID WVTPolicyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    LONG lStatus = WinVerifyTrust( NULL, &WVTPolicyGUID, &WinTrustData );
    return lStatus == ERROR_SUCCESS;
}


//////////////////////////////////////////////////////////
//
// LogSettings
//
// Dump some settings to the log file to help debugging
//
//////////////////////////////////////////////////////////
void LogSettings( void )
{
    const char* szSettings[] = {
                                "general", "aero-enabled",
                                "general", "aero-changeable",
                                "general", "driver-overrides-disabled",
                                "general", "device-selection-disabled",
                                "general", "customized-sa-files-using",
                                "general", "times-connected",
                                "general", "times-connected-editor",
                                "nvhacks", "optimus-force-detection",
                                "nvhacks", "optimus-export-enablement",
                                "nvhacks", "optimus",
                                "nvhacks", "optimus-rename-exe",
                                "nvhacks", "optimus-alt-startup",
                                "nvhacks", "optimus-force-windowed",
                                "nvhacks", "optimus-dialog-skip",
                                "nvhacks", "optimus-startup-option",
                                "diagnostics", "send-dumps",
                                "diagnostics", "last-minidump-time",
                                "diagnostics", "user-confirmed-bsod-time",
                                DIAG_MINIDUMP_DETECTED_COUNT,
                                DIAG_MINIDUMP_CONFIRMED_COUNT,
                                DIAG_PRELOAD_UPGRADES_LOWEST_UNSAFE,
                                "general", "noav-user-says-skip",
                                "general", "noav-last-asked-time",
                            };

    for ( uint i = 0 ; i < NUMELMS( szSettings ) ; i += 2 )
    {
        WriteDebugEvent( SString( "%s: %s", szSettings[i+1], *GetApplicationSetting( szSettings[i], szSettings[i+1] ) ) );
    }

    uint uiTimeLastAsked = GetApplicationSettingInt( "noav-last-asked-time" );
    uint uiTimeNow = static_cast < uint >( time( NULL ) / 3600LL );
    uint uiHoursSinceLastAsked = uiTimeNow - uiTimeLastAsked;
    WriteDebugEvent( SString( "noav-last-asked-time-hours-delta: %d", uiHoursSinceLastAsked ) );
}


//////////////////////////////////////////////////////////
//
// PadLeft
//
// Add some spaces to make it look nicer
//
//////////////////////////////////////////////////////////
SString PadLeft( const SString& strText, uint uiNumSpaces, char cCharacter )
{
    SString strPad = std::string( uiNumSpaces, cCharacter );
    return strPad + strText.Replace( "\n", SStringX( "\n" ) + strPad );
}


//////////////////////////////////////////////////////////
//
// DllMain
//
// Used to save handle to loader dll, so we can get at the resources
//
//////////////////////////////////////////////////////////
int WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, PVOID pvNothing)
{
    g_hInstance = hModule;
    return TRUE;
}
