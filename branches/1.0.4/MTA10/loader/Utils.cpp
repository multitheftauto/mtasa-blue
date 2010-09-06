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

static bool bCancelPressed = false;
static HWND hwndSplash = NULL;
static unsigned long ulSplashStartTime = 0;
static HWND hwndProgressDialog = NULL;

HMODULE RemoteLoadLibrary(HANDLE hProcess, const char* szLibPath)
{
    /* Called correctly? */
    if ( szLibPath == NULL )
    {
        return 0;
    }

    /* Allocate memory in the remote process for the library path */
    HANDLE hThread = 0;
    void* pLibPathRemote = NULL;
    HMODULE hKernel32 = GetModuleHandle( "Kernel32" );
    pLibPathRemote = VirtualAllocEx( hProcess, NULL, strlen ( szLibPath ) + 1, MEM_COMMIT, PAGE_READWRITE );
    
    if ( pLibPathRemote == NULL )
    {
        return 0;
    }

    /* Make sure pLibPathRemote is always freed */
    __try {

        /* Write the DLL library path to the remote allocation */
        DWORD byteswritten = 0;
        WriteProcessMemory ( hProcess, pLibPathRemote, (void*)szLibPath, strlen ( szLibPath ) + 1, &byteswritten );

        if ( byteswritten != strlen ( szLibPath ) + 1 )
        {
            return 0;
        }

        /* Start a remote thread executing LoadLibraryA exported from Kernel32. Passing the
           remotly allocated path buffer as an argument to that thread (and also to LoadLibraryA)
           will make the remote process load the DLL into it's userspace (giving the DLL full
           access to the game executable).*/
        hThread = CreateRemoteThread(   hProcess,
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
        VirtualFreeEx( hProcess, pLibPathRemote, strlen ( szLibPath ) + 1, MEM_RELEASE );
    }

    /*  We wait for the created remote thread to finish executing. When it's done, the DLL
        is loaded into the game's userspace, and we can destroy the thread-handle. We wait
        5 seconds which is way longer than this should take to prevent this application
        from deadlocking if something goes really wrong allowing us to kill the injected
        game executable and avoid user inconvenience.*/
    WaitForSingleObject ( hThread, INFINITE );

    /* Get the handle of the remotely loaded DLL module */
    DWORD hLibModule = 0;
    GetExitCodeThread ( hProcess, &hLibModule );

    // Wait for the LoadLibrary thread to finish
    WaitForSingleObject( hThread, INFINITE );

    /* Clean up the resources we used to inject the DLL */
    VirtualFreeEx (hProcess, pLibPathRemote, strlen ( szLibPath ) + 1, MEM_RELEASE );

    /* Success */
    return ( HINSTANCE )( 1 );
}


bool TerminateGTAIfRunning ( void )
{
    DWORD dwProcessIDs[250];
    DWORD pBytesReturned = 0;
    unsigned int uiListSize = 50;
    if ( EnumProcesses ( dwProcessIDs, 250 * sizeof(DWORD), &pBytesReturned ) )
    {
        for ( unsigned int i = 0; i < pBytesReturned / sizeof ( DWORD ); i++ )
        {
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
                        if ( strcmpi ( szModuleName + strlen ( szModuleName ) - strlen ( "gta_sa.exe" ), "gta_sa.exe" ) == 0 )
                        {
                            if ( MessageBox ( 0, "An instance of GTA: San Andreas is already running. It needs to be terminated before MTA: SA can be started. Do you want to do that now?", "Information", MB_YESNO | MB_ICONQUESTION ) == IDYES )
                            {
                                TerminateProcess ( hProcess, 0 );
                                CloseHandle ( hProcess );
                                return true;
                            }

                            return false;
                        } 
                    }
                }

                // Close the process
                CloseHandle ( hProcess );
            }
        }
    }

    return true;
}


int CALLBACK DialogProc ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{ 
    switch (uMsg) 
    { 
        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            {  
                case IDCANCEL:
                    bCancelPressed = true;
                    return TRUE; 
            } 
    } 
    return FALSE; 
} 


//
// Show splash dialog
//
void ShowSplash ( HINSTANCE hInstance )
{
    if ( !hwndSplash )
    {
        hwndSplash = CreateDialog ( hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, DialogProc );
        ulSplashStartTime = GetTickCount ();
    }
}


//
// Hide splash dialog
//
void HideSplash ( bool bOnlyDelay  )
{
    if ( hwndSplash )
    {
        // Show splash for at least two seconds
        unsigned long ulTimeElapsed = GetTickCount () - ulSplashStartTime;
        if ( ulTimeElapsed < 2000 )
            Sleep ( 2000 - ulTimeElapsed );

        if ( !bOnlyDelay )
        {
            DestroyWindow ( hwndSplash );
            hwndSplash = NULL;
        }
    }
}


//
// General error message box
//
long DisplayErrorMessageBox ( const SString& strMessage )
{
    HideSplash ();
    MessageBox( 0, strMessage, "Error!", MB_ICONEXCLAMATION|MB_OK );
    return 1;
}


//
// Read a registry string value
//
SString ReadRegistryStringValue ( HKEY hkRoot, LPCSTR szSubKey, LPCSTR szValue, int* iResult )
{
    // Clear output
    SString strOutResult = "";

    bool bResult = false;
    HKEY hkTemp = NULL;
    if ( RegOpenKeyEx ( hkRoot, szSubKey, 0, KEY_READ, &hkTemp ) == ERROR_SUCCESS ) 
    {
        DWORD dwBufferSize;
        if ( RegQueryValueExA ( hkTemp, szValue, NULL, NULL, NULL, &dwBufferSize ) == ERROR_SUCCESS )
        {
            char *szBuffer = static_cast < char* > ( alloca ( dwBufferSize + 1 ) );
            if ( RegQueryValueExA ( hkTemp, szValue, NULL, NULL, (LPBYTE)szBuffer, &dwBufferSize ) == ERROR_SUCCESS )
            {
                strOutResult = szBuffer;
                bResult = true;
            }
        }
        RegCloseKey ( hkTemp );
    }
    if ( iResult )
        *iResult = bResult;
    return strOutResult;
}


//
// Write a registry string value
//

void WriteRegistryStringValue ( HKEY hkRoot, LPCSTR szSubKey, LPCSTR szValue, const char* szBuffer )
{
    HKEY hkTemp;
    RegCreateKeyEx ( hkRoot, szSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkTemp, NULL );
    if ( hkTemp )
    {
        RegSetValueEx ( hkTemp, szValue, NULL, REG_SZ, (LPBYTE)szBuffer, strlen(szBuffer) + 1 );
        RegCloseKey ( hkTemp );
    }
}


SString GetMTASAPath ( void )
{
    // Get current module full path
    char szBuffer[64000];
    GetModuleFileName ( NULL, szBuffer, sizeof(szBuffer) - 1 );

    // Strip the module name out of the path.
    PathRemoveFileSpec ( szBuffer );

    // Save to a temp registry key
    WriteRegistryStringValue ( HKEY_CURRENT_USER, "Software\\Multi Theft Auto: San Andreas", "Last Run Location", szBuffer );
    return szBuffer;
}


int GetGamePath ( SString& strOutResult )
{
    SString strRegPath = ReadRegistryStringValue ( HKEY_CURRENT_USER, "Software\\Multi Theft Auto: San Andreas", "GTA:SA Path" );

    if ( ( GetAsyncKeyState ( VK_CONTROL ) & 0x8000 ) == 0 )
    {
        if ( strlen( strRegPath.c_str () ) )
        {
            // Check for replacement characters (?), to see if there are any (unsupported) unicode characters
            if ( strchr ( strRegPath.c_str (), '?' ) > 0 )
                return -1;

            SString strExePath ( "%s\\%s", strRegPath.c_str (), MTA_GTAEXE_NAME );
            if ( FileExists( strExePath  ) )
            {
                strOutResult = strRegPath;
                return 1;
            }
            strExePath = SString( "%s\\%s", strRegPath.c_str (), MTA_GTASTEAMEXE_NAME );
            if ( FileExists( strExePath  ) )
            {
                return -2;
            }
        }
    }
    BROWSEINFO bi = { 0 };
    bi.lpszTitle = "Select your Grand Theft Auto: San Andreas Installation Directory";
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
    
        if ( FileExists( SString ( "%s\\%s", strOutResult.c_str (), MTA_GTAEXE_NAME ) ) )
        {
            WriteRegistryStringValue ( HKEY_CURRENT_USER, "Software\\Multi Theft Auto: San Andreas", "GTA:SA Path", strOutResult );
        }
        else
        {
            if ( MessageBox ( NULL, "Could not find gta_sa.exe at the path you have selected. Choose another folder?", "Error", MB_OKCANCEL ) == IDOK )
            {
                return GetGamePath ( strOutResult );
            }
            else
            {
                return 0;
            }
        }
        return 1;
    }
    else
    {
        return 0;
    }
}


///////////////////////////////////////////////////////////////
//
// Progress dialog
//
//
//
///////////////////////////////////////////////////////////////
void ShowProgressDialog( HINSTANCE hInstance, const SString& strTitle, bool bAllowCancel )
{
    if ( !hwndProgressDialog )
    {
        bCancelPressed = false;
        hwndProgressDialog = CreateDialog ( hInstance, MAKEINTRESOURCE(IDD_PROGRESS_DIALOG), 0, DialogProc );
        SetWindowText ( hwndProgressDialog, strTitle );
        HWND hwndButton = GetDlgItem( hwndProgressDialog, IDCANCEL );
        ShowWindow( hwndButton, bAllowCancel ? SW_SHOW : SW_HIDE );
    }
}

void HideProgressDialog ( void )
{
    if ( hwndProgressDialog )
    {
        DestroyWindow ( hwndProgressDialog );
        hwndProgressDialog = NULL;
    }
}


// returns true if canceled
bool UpdateProgress( int iPos, int iMax, const SString& strMsg )
{
    if ( hwndProgressDialog)
    {
        HWND hwndText = GetDlgItem( hwndProgressDialog, IDC_PROGRESS_STATIC );
        char buffer[ 1024 ] = "";
        ::GetWindowText ( hwndText, buffer, sizeof(buffer) - 2 );
        if ( strMsg.length () > 0 && strMsg != buffer )
            SetWindowText ( hwndText, strMsg );
        HWND hwndBar = GetDlgItem( hwndProgressDialog, IDC_PROGRESS_BAR );
        PostMessage(hwndBar, PBM_SETPOS, iPos * 100 / Max ( 1, iMax ), 0 );
        MSG msg;
        while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
        {
            if( GetMessage( &msg, NULL, 0, 0 ) )
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        return bCancelPressed;
    }
    return false;
}


///////////////////////////////////////////////////////////////
//
// FindFiles
//
// Find all files or directories at a path
//
///////////////////////////////////////////////////////////////
std::vector < SString > FindFiles ( const SString& strMatch, bool bFiles, bool bDirectories )
{
    std::vector < SString > result;

    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile ( strMatch, &findData );
    if( hFind != INVALID_HANDLE_VALUE )
    {
        do
        {
            if ( ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ? bDirectories : bFiles )
                if ( strcmp ( findData.cFileName, "." ) && strcmp ( findData.cFileName, ".." ) )
                    result.push_back ( findData.cFileName );
        }
        while( FindNextFile( hFind, &findData ) );
        FindClose( hFind );
    }
    return result;
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
// CheckPermissions
//
// Return false if permissions are not correct
//
///////////////////////////////////////////////////////////////
bool CheckPermissions ( const std::string& strPath, unsigned int uiMaxTimeMs )
{
    srand ( GetTickCount () );

    std::vector < SString > stopList;
    stopList.push_back ( "resource" );
    stopList.push_back ( "dumps" );
    stopList.push_back ( "screenshots" );

    std::vector < SString > filePathList;
    std::vector < SString > dirPathList;
    FindRelevantFiles ( strPath, filePathList, dirPathList, stopList, 500, 60 );

    unsigned long ulStartTime = GetTickCount ();

    float fRatio = dirPathList.size () / Max < float > ( 1, (float)filePathList.size () + dirPathList.size () );
    fRatio = Clamp ( 1/5.f, fRatio, 1/2.f );

    {
        std::vector < int > indexList;
        MakeRandomIndexList ( dirPathList.size (), indexList );
        for ( unsigned int i = 0 ; i < dirPathList.size (); i++ )
        {
            int idx = indexList[i];
            if ( ( idx % 10 ) == 0 )
            {
                unsigned long ulTimeElapsed = GetTickCount () - ulStartTime;
                if ( ulTimeElapsed > uiMaxTimeMs * fRatio )
                    break;          
            }

            if ( !HasUsersGotFullAccess ( dirPathList[idx] ) )
                return false;
        }
    }

    {
        std::vector < int > indexList;
        MakeRandomIndexList ( filePathList.size (), indexList );
        for ( unsigned int i = 0 ; i < filePathList.size (); i++ )
        {
            int idx = indexList[i];
            if ( ( idx % 10 ) == 0 )
            {
                unsigned long ulTimeElapsed = GetTickCount () - ulStartTime;
                if ( ulTimeElapsed > uiMaxTimeMs )
                    break;
            }

            if ( !HasUsersGotFullAccess ( filePathList[idx] ) )
                return false;
        }
    }

    return true;
}


///////////////////////////////////////////////////////////////
//
// FixPermissions
//
//
//
///////////////////////////////////////////////////////////////
void FixPermissions ( const std::string& strPath )
{
    std::vector < SString > stopList;
    std::vector < SString > filePathList;
    std::vector < SString > dirPathList;
    FindRelevantFiles ( strPath, filePathList, dirPathList, stopList, 0, 0 );

    int Total = dirPathList.size () + filePathList.size ();

    for ( unsigned int i = 0 ; i < dirPathList.size (); i++ )
    {
        const SString& strDirPathName = dirPathList[i];
        if ( ( i % 10 ) == 0 )
            if ( UpdateProgress ( i + Total, Total * 2, SString ( "%s", strDirPathName.substr ( strPath.length () ).c_str () ) ) )
                return;

        if ( !HasUsersGotFullAccess( strDirPathName ) )
            DoSetOnFile ( false, strDirPathName, "(BU)","FullAccess" );
    }

    for ( unsigned int i = 0 ; i < filePathList.size (); i++ )
    {
        const SString& strFilePathName = filePathList[i];
        if ( ( i % 10 ) == 0 )
            if ( UpdateProgress ( i + dirPathList.size () + Total, Total * 2, SString ( "%s", strFilePathName.substr ( strPath.length () ).c_str () ) ) )
                return;

        if ( !HasUsersGotFullAccess( strFilePathName ) )
            DoSetOnFile ( false, strFilePathName, "(BU)","FullAccess" );
    }
}
