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
#include <assert.h>

static bool bCancelPressed = false;
static bool bOkPressed = false;
static HWND hwndSplash = NULL;
static unsigned long ulSplashStartTime = 0;
static HWND hwndProgressDialog = NULL;
static unsigned long ulProgressStartTime = 0;
static SString g_strMTASAPath;
SString GetWMIOSVersion ( void );
static HWND hwndCrashedDialog = NULL;

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
                case IDOK:
                    bOkPressed = true;
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
    SetForegroundWindow ( hwndSplash );
    SetWindowPos ( hwndSplash, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
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
long DisplayErrorMessageBox ( const SString& strMessage, const SString& strTroubleType )
{
    HideSplash ();
    MessageBox( 0, strMessage, "Error!", MB_ICONEXCLAMATION|MB_OK );

    if ( strTroubleType != "" )
        BrowseToSolution ( strTroubleType, true );
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


SString GetMTASAModuleFileName ( void )
{
    // Get current module full path
    char szBuffer[64000];
    GetModuleFileName ( NULL, szBuffer, sizeof(szBuffer) - 1 );
    return szBuffer;
}


SString GetLaunchPath ( void )
{
    // Get current module full path
    char szBuffer[64000];
    GetModuleFileName ( NULL, szBuffer, sizeof(szBuffer) - 1 );

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
        GetModuleFileName ( NULL, szBuffer, sizeof(szBuffer) - 1 );

        // Strip the module name out of the path.
        PathRemoveFileSpec ( szBuffer );

        // Save to a temp registry key
        WriteRegistryStringValue ( HKEY_CURRENT_USER, "Software\\Multi Theft Auto: San Andreas", "Last Run Location", szBuffer );
        g_strMTASAPath = szBuffer;
    }
}


SString GetMTASAPath ( void )
{
    if ( g_strMTASAPath == "" )
        SetMTASAPathSource ( true );
    return g_strMTASAPath;
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
        HideSplash ();
        bCancelPressed = false;
        hwndProgressDialog = CreateDialog ( hInstance, MAKEINTRESOURCE(IDD_PROGRESS_DIALOG), 0, DialogProc );
        SetWindowText ( hwndProgressDialog, strTitle );
        HWND hwndButton = GetDlgItem( hwndProgressDialog, IDCANCEL );
        ShowWindow( hwndButton, bAllowCancel ? SW_SHOW : SW_HIDE );
        ulProgressStartTime = GetTickCount ();
    }
    SetForegroundWindow ( hwndProgressDialog );
    SetWindowPos ( hwndProgressDialog, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
}

void HideProgressDialog ( void )
{
    if ( hwndProgressDialog )
    {
        // Show progress for at least two seconds
        unsigned long ulTimeElapsed = GetTickCount () - ulProgressStartTime;
        if ( ulTimeElapsed < 2000 )
        {
            UpdateProgress( 100, 100 );
            Sleep ( 2000 - ulTimeElapsed );
        }

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
// Progress
//
//
//
///////////////////////////////////////////////////////////////
void StartPseudoProgress( HINSTANCE hInstance, const SString& strTitle, const SString& strMsg )
{
    ShowProgressDialog ( hInstance, strTitle );
    UpdateProgress( 10, 100, strMsg );
    Sleep ( 100 );
    UpdateProgress( 30, 100 );
}

void StopPseudoProgress( void )
{
    if ( hwndProgressDialog )
    {
        UpdateProgress( 60, 100 );
        Sleep ( 100 );
        UpdateProgress( 90, 100 );
        Sleep ( 100 );
        HideProgressDialog ();
    }
}




///////////////////////////////////////////////////////////////
//
// Crashed dialog
//
//
//
///////////////////////////////////////////////////////////////
SString ShowCrashedDialog( HINSTANCE hInstance, const SString& strMessage )
{
    if ( !hwndCrashedDialog )
    {
        HideSplash ();
        bCancelPressed = false;
        bOkPressed = false;
        hwndCrashedDialog = CreateDialog ( hInstance, MAKEINTRESOURCE(IDD_CRASHED_DIALOG), 0, DialogProc );
        SetWindowText ( GetDlgItem( hwndCrashedDialog, IDC_CRASH_INFO_EDIT ), strMessage );
        SendDlgItemMessage( hwndCrashedDialog, IDC_SEND_DUMP_CHECK, BM_SETCHECK, GetApplicationSetting ( "diagnostics", "send-dumps" ) != "no" ? BST_CHECKED : BST_UNCHECKED, 0 );
    }
    SetForegroundWindow ( hwndCrashedDialog );
    SetWindowPos ( hwndCrashedDialog, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );

    while ( !bCancelPressed && !bOkPressed )
    {
        MSG msg;
        while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
        {
            if( GetMessage( &msg, NULL, 0, 0 ) )
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        Sleep( 10 );
    }

    LRESULT res = SendMessageA( GetDlgItem( hwndCrashedDialog, IDC_SEND_DUMP_CHECK ), BM_GETCHECK, 0, 0 );
    SetApplicationSetting ( "diagnostics", "send-dumps", res ? "yes" : "no" );

    if ( bCancelPressed )
        return "quit";
    //if ( bOkPressed )
        return "ok";
}


void HideCrashedDialog ( void )
{
    if ( hwndCrashedDialog )
    {
        DestroyWindow ( hwndCrashedDialog );
        hwndCrashedDialog = NULL;
    }
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


///////////////////////////////////////////////////////////////
//
// MyShellExecute
//
//
//
///////////////////////////////////////////////////////////////
bool MyShellExecute ( bool bBlocking, const SString& strAction, const SString& strFile, const SString& strParameters = "", const SString& strDirectory = "" )
{
    AddReportLog ( 1081, SString ( "ShellExecute bBlocking:%d %s %s '%s'", bBlocking, strAction.c_str (), strFile.c_str (), strParameters.c_str () ) );

    SHELLEXECUTEINFO info;
    memset( &info, 0, sizeof ( info ) );
    info.cbSize = sizeof ( info );
    info.fMask = SEE_MASK_NOCLOSEPROCESS;
    info.lpVerb = strAction;
    info.lpFile = strFile;
    info.lpParameters = strParameters;
    info.lpDirectory = strDirectory;
    info.nShow = SW_SHOWNORMAL;
    bool bResult = ShellExecuteExA( &info ) != FALSE;
    if ( bBlocking && info.hProcess )
        WaitForSingleObject ( info.hProcess, INFINITE );
    return bResult;
}


///////////////////////////////////////////////////////////////
//
// ShellExecuteBlocking
//
//
//
///////////////////////////////////////////////////////////////
bool ShellExecuteBlocking ( const SString& strAction, const SString& strFile, const SString& strParameters, const SString& strDirectory )
{
    return MyShellExecute ( true, strAction, strFile, strParameters, strDirectory );
}


///////////////////////////////////////////////////////////////
//
// ShellExecuteNonBlocking
//
//
//
///////////////////////////////////////////////////////////////
bool ShellExecuteNonBlocking ( const SString& strAction, const SString& strFile, const SString& strParameters, const SString& strDirectory )
{
    return MyShellExecute ( false, strAction, strFile, strParameters, strDirectory );
}


//
// QueryWMI
//
// Code based on the example from:
// ms-help://MS.VSCC.v90/MS.MSDNQTR.v90.en/wmisdk/wmi/example__getting_wmi_data_from_the_local_computer.htm
//

#define _WIN32_DCOM
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>

# pragma comment(lib, "wbemuuid.lib")


#define DECODE_NAME_WMI(txt) DECODE_TEXT(txt,0xB5)

/////////////////////////////////////////////////////////////////////
//
// QueryWMI
//
//
//
/////////////////////////////////////////////////////////////////////
static bool QueryWMI ( const SString& strQuery, const SString& strKeys, std::vector < std::vector < SString > >& vecResults )
{
    HRESULT hres;

    // Step 1: --------------------------------------------------
    // Initialize COM. ------------------------------------------

    // This has already been done somewhere else.

    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres))
    {
        OutputDebugString ( SString ( "Failed to initialize COM library. Error code = %x\n", hres ) );
        return "";
    }

    // Step 2: --------------------------------------------------
    // Set general COM security levels --------------------------
    // Note: If you are using Windows 2000, you need to specify -
    // the default authentication credentials for a user by using
    // a SOLE_AUTHENTICATION_LIST structure in the pAuthList ----
    // parameter of CoInitializeSecurity ------------------------

    hres =  CoInitializeSecurity(
        NULL, 
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
        );
                      
    // Error here can be non fatal

    if (FAILED(hres))
    {
#if MTA_DEBUG
        OutputDebugString ( SString ( "QueryWMI - Failed to initialize security. Error code = %x\n", hres ) );
#endif
        return "";
    }
    
    // Step 3: ---------------------------------------------------
    // Obtain the initial locator to WMI -------------------------

    IWbemLocator *pLoc = NULL;

    hres = CoCreateInstance(
        CLSID_WbemLocator,             
        0, 
        CLSCTX_INPROC_SERVER, 
        IID_IWbemLocator, (LPVOID *) &pLoc);
 
    if (FAILED(hres))
    {
#if MTA_DEBUG
        OutputDebugString ( SString ( "QueryWMI - Failed to create IWbemLocator object. Error code = %x\n", hres ) );
#endif
        return false;
    }

    // Step 4: -----------------------------------------------------
    // Connect to WMI through the IWbemLocator::ConnectServer method

    IWbemServices *pSvc = NULL;


    // Connect to the root\cimv2 namespace with
    // the current user and obtain pointer pSvc
    // to make IWbemServices calls.
    hres = pLoc->ConnectServer(
         _bstr_t( "ROOT\\CIMV2" ), // Object path of WMI namespace
         NULL,                    // User name. NULL = current user
         NULL,                    // User password. NULL = current
         0,                       // Locale. NULL indicates current
         NULL,                    // Security flags.
         0,                       // Authority (e.g. Kerberos)
         0,                       // Context object 
         &pSvc                    // pointer to IWbemServices proxy
         );
   
    if (FAILED(hres))
    {
        pLoc->Release();     
#if MTA_DEBUG
        OutputDebugString ( SString ( "QueryWMI - Could not connect. Error code = %x\n", hres ) );
#endif
        return false;
    }

    // Step 5: --------------------------------------------------
    // Set security levels on the proxy -------------------------

    hres = CoSetProxyBlanket(
       pSvc,                        // Indicates the proxy to set
       RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
       RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
       NULL,                        // Server principal name 
       RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
       RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
       NULL,                        // client identity
       EOAC_NONE                    // proxy capabilities 
    );

    if (FAILED(hres))
    {
        pSvc->Release();
        pLoc->Release();     
#if MTA_DEBUG
        OutputDebugString ( SString ( "QueryWMI - Could not set proxy blanket. Error code = %x\n", hres ) );
#endif
        return false;
    }

    // Step 6: --------------------------------------------------
    // Use the IWbemServices pointer to make requests of WMI ----
    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t( "SELECT * FROM " ) + bstr_t ( strQuery ),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
        NULL,
        &pEnumerator);
    
    if (FAILED(hres))
    {
        pSvc->Release();
        pLoc->Release();
#if MTA_DEBUG
        OutputDebugString ( SString ( "QueryWMI - Query failed. Error code = %x\n", hres ) );
#endif
        return false;
    }

    // Step 7: -------------------------------------------------
    // Get the data from the query in step 6 -------------------
 
    IWbemClassObject *pclsObj;
    ULONG uReturn = 0;

    // Get list of keys to find values for
    std::vector < SString > vecKeys;
    SString ( strKeys ).Split ( ",", vecKeys );

    // Reserve 20 rows for results
    vecResults.reserve ( 20 );

    // Fill each row
    while (pEnumerator)
    {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, 
            &pclsObj, &uReturn);

        if(0 == uReturn)
        {
            break;
        }

        VARIANT vtProp;

        // Add result row
        vecResults.insert ( vecResults.end (), std::vector < SString > () );
        vecResults.back().reserve ( vecKeys.size () );

        // Fill each cell
        for ( unsigned int i = 0 ; i < vecKeys.size () ; i++ )
        {
            string strKey = vecKeys[i];
            string strValue;

            wstring wstrKey( strKey.begin (), strKey.end () );
            hr = pclsObj->Get ( wstrKey.c_str (), 0, &vtProp, 0, 0 );

            VariantChangeType( &vtProp, &vtProp, 0, VT_BSTR );
            if ( vtProp.vt == VT_BSTR )
                strValue = _bstr_t ( vtProp.bstrVal );
            VariantClear ( &vtProp );

            vecResults.back().insert ( vecResults.back().end (), strValue );
        }

        pclsObj->Release();
    }

    // Cleanup
    // ========
    
    pSvc->Release();
    pLoc->Release();
    pEnumerator->Release();

    return true;
}


/////////////////////////////////////////////////////////////////////
//
// GetWMIOSVersion
//
//
//
/////////////////////////////////////////////////////////////////////
SString GetWMIOSVersion ( void )
{
    std::vector < std::vector < SString > > vecResults;

    QueryWMI ( "Win32_OperatingSystem", "Version", vecResults );

    if ( vecResults.empty () )
        return "";

    const SString& strVersion  = vecResults[0][0];
    return strVersion;
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
// UpdateMTAVersionApplicationSetting
//
// Make sure "mta-version-ext" is correct. eg "1.0.4-9.01234.2.000"
//
/////////////////////////////////////////////////////////////////////
void UpdateMTAVersionApplicationSetting ( void )
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
    SString strOldHash = GetApplicationSetting ( "netc-hash" );
    unsigned short usNetRev = 65535;
    unsigned short usNetRel = 0;
    std::vector < SString > parts;
    GetApplicationSetting ( "mta-version-ext" ).Split ( ".", parts );
    if ( parts.size () == 6 )
    {
        usNetRev = atoi ( parts[4] );
        usNetRel = atoi ( parts[5] );
    }

    // Get path to the relevant file
    SString strNetLibPath = PathJoin ( GetLaunchPath (), "mta" );
    SString strNetLibPathFilename = PathJoin ( strNetLibPath, strFilename );

    // Hash the file
    std::vector < char > buffer;
    FileLoad ( strNetLibPathFilename, buffer );
    SString strNewHash = "none";
    if ( buffer.size () )
        strNewHash = HashBuffer ( &buffer.at ( 0 ), buffer.size () );

#ifdef MTA_DEBUG
    // Force update
    strNewHash = "x";
#endif
    // Only loadup the dll if the hash has changed, or we don't have a previous valid netrev value
    if ( strNewHash != strOldHash || usNetRev == 65535 )
    {
        SString strPrevCurDir = GetCurrentWorkingDirectory ();
        SetCurrentDirectory ( strNetLibPath );
        SetDllDirectory( strNetLibPath );

        HMODULE hModule = LoadLibrary ( strNetLibPathFilename );

        if ( hModule )
        {
            typedef unsigned short (*PFNGETNETREV) ( void );
            PFNGETNETREV pfnGetNetRev = static_cast < PFNGETNETREV > ( static_cast < PVOID > ( GetProcAddress ( hModule, "GetNetRev" ) ) );
            if ( pfnGetNetRev )
                usNetRev = pfnGetNetRev ();
            PFNGETNETREV pfnGetNetRel = static_cast < PFNGETNETREV > ( static_cast < PVOID > ( GetProcAddress ( hModule, "GetNetRel" ) ) );
            if ( pfnGetNetRel )
                usNetRel = pfnGetNetRel ();

            FreeLibrary ( hModule );
        }

        SetCurrentDirectory ( strPrevCurDir );
        SetDllDirectory( strPrevCurDir );

        SetApplicationSetting ( "netc-hash", strNewHash );
    }

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

