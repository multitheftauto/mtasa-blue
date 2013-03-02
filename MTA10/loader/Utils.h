/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        loader/Utils.h
*  PURPOSE:     Loading utilities
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

extern HINSTANCE g_hInstance;
extern HANDLE g_hMutex;

enum ePathResult
{
    GAME_PATH_STEAM,
    GAME_PATH_UNICODE_CHARS,
    GAME_PATH_MISSING,
    GAME_PATH_OK,
};

enum
{
    CHECK_SERVICE_POST_INSTALL = 1,
    CHECK_SERVICE_PRE_UNINSTALL = 2,
    CHECK_SERVICE_POST_UPDATE = 3,
    CHECK_SERVICE_PRE_GAME = 4,
    CHECK_SERVICE_POST_GAME = 5,
    CHECK_SERVICE_PRE_CREATE = 6,
};

// Loads the given dll into hProcess. Returns 0 on failure or the handle to the
// remote dll module on success.
HMODULE         RemoteLoadLibrary                   ( HANDLE hProcess, const char* szLibPath );
void            InsertWinMainBlock                  ( HANDLE hProcess );
void            RemoveWinMainBlock                  ( HANDLE hProcess );
void            ApplyLoadingCrashPatch              ( HANDLE hProcess );

void            TerminateGTAIfRunning               ( void );
bool            IsGTARunning                        ( void );
void            TerminateOtherMTAIfRunning          ( void );
bool            IsOtherMTARunning                   ( void );

void            ShowSplash                          ( HINSTANCE hInstance );
void            HideSplash                          ( void );

bool            CommandLineContains                 ( const SString& strText );
long            DisplayErrorMessageBox              ( const SString& strMessage, const SString& strTroubleType = "" );

void            SetMTASAPathSource                  ( bool bReadFromRegistry );
SString         GetMTASAPath                        ( bool bReadFromRegistry = true );
ePathResult     GetGamePath                         ( SString& strOutResult, bool bFindIfMissing = false );
SString         GetMTASAModuleFileName              ( void );

void            ShowProgressDialog                  ( HINSTANCE hInstance, const SString& strTitle, bool bAllowCancel = false );
void            HideProgressDialog                  ( void );
bool            UpdateProgress                      ( int iPos, int iMax, const SString& strMsg = "" );

void            FindFilesRecursive                  ( const SString& strPathMatch, std::vector < SString >& outFileList, uint uiMaxDepth = 99 );
SString         GetOSVersion                        ( void );
SString         GetRealOSVersion                    ( void );
bool            IsVistaOrHigher                     ( void );
BOOL            IsUserAdmin                         ( void );
bool            GetLibVersionInfo                   ( const char *szLibName, VS_FIXEDFILEINFO* pOutFileInfo );

void            StartPseudoProgress                 ( HINSTANCE hInstance, const SString& strTitle, const SString& strMsg );
void            StopPseudoProgress                  ( void );

SString         ShowCrashedDialog                   ( HINSTANCE hInstance, const SString& strMessage );
void            HideCrashedDialog                   ( void );

void            ShowD3dDllDialog                    ( HINSTANCE hInstance, const SString& strPath );
void            HideD3dDllDialog                    ( void );

void            UpdateMTAVersionApplicationSetting  ( bool bQuiet = false );
bool            Is32bitProcess                      ( DWORD processID );

bool            CreateSingleInstanceMutex           ( void );
void            ReleaseSingleInstanceMutex          ( void );

SString         CheckOnRestartCommand               ( void );
void            CleanDownloadCache                  ( void );

HMODULE         GetLibraryHandle                    ( const SString& strFilename );
void            FreeLibraryHandle                   ( void );
uint            WaitForObject                       ( HANDLE hProcess, HANDLE hThread, DWORD dwMilliseconds, HANDLE hMutex );
bool            CheckService                        ( uint uiStage );

void            MaybeShowCopySettingsDialog         ( void );

bool            CheckAndShowFileOpenFailureMessage  ( void );

//
// Determine if game process has gone wonky
//
class CStuckProcessDetector
{
public:
    CStuckProcessDetector( HANDLE hProcess, uint uiThresholdMs )
    {
        m_hProcess = hProcess;
        m_uiThresholdMs = uiThresholdMs;
        m_PrevWorkingSetSize = 0;
    }

    // Returns true if process is active and hasn't changed mem usage for uiThresholdMs
    bool UpdateIsStuck( void )
    {
        PROCESS_MEMORY_COUNTERS psmemCounter;
        BOOL bResult = GetProcessMemoryInfo( m_hProcess, &psmemCounter, sizeof( PROCESS_MEMORY_COUNTERS ) );
        if ( !bResult )
            return false;

        if ( m_PrevWorkingSetSize != psmemCounter.WorkingSetSize )
        {
            m_PrevWorkingSetSize = psmemCounter.WorkingSetSize;
            m_StuckTimer.Reset();
        }
        return m_StuckTimer.Get() > m_uiThresholdMs;
    }

    uint            m_uiThresholdMs;
    SIZE_T          m_PrevWorkingSetSize;
    HANDLE          m_hProcess;
    CElapsedTime    m_StuckTimer;
};


#undef CREATE_SUSPENDED
#define CREATE_SUSPENDED 5

#ifdef DONT_ASSIST_ANTI_VIRUS

    #define _VirtualAllocEx         VirtualAllocEx
    #define _VirtualProtectEx       VirtualProtectEx
    #define _VirtualFreeEx          VirtualFreeEx
    #define _ReadProcessMemory      ReadProcessMemory
    #define _WriteProcessMemory     WriteProcessMemory
    #define _CreateProcess          CreateProcess
    #define _CreateRemoteThread     CreateRemoteThread

#else

    typedef
    LPVOID
    (WINAPI
    *FUNC_VirtualAllocEx)(
        __in     HANDLE hProcess,
        __in_opt LPVOID lpAddress,
        __in     SIZE_T dwSize,
        __in     DWORD flAllocationType,
        __in     DWORD flProtect
        );


    typedef
    BOOL
    (WINAPI
    *FUNC_VirtualProtectEx)(
        __in  HANDLE hProcess,
        __in  LPVOID lpAddress,
        __in  SIZE_T dwSize,
        __in  DWORD flNewProtect,
        __out PDWORD lpflOldProtect
        );

    typedef
    BOOL
    (WINAPI
    *FUNC_VirtualFreeEx)(
        __in HANDLE hProcess,
        __in LPVOID lpAddress,
        __in SIZE_T dwSize,
        __in DWORD  dwFreeType
        );

    typedef
    BOOL
    (WINAPI
    *FUNC_ReadProcessMemory)(
        __in      HANDLE hProcess,
        __in      LPCVOID lpBaseAddress,
        __out_bcount_part(nSize, *lpNumberOfBytesRead) LPVOID lpBuffer,
        __in      SIZE_T nSize,
        __out_opt SIZE_T * lpNumberOfBytesRead
        );

    typedef
    BOOL
    (WINAPI
    *FUNC_WriteProcessMemory)(
        __in      HANDLE hProcess,
        __in      LPVOID lpBaseAddress,
        __in_bcount(nSize) LPCVOID lpBuffer,
        __in      SIZE_T nSize,
        __out_opt SIZE_T * lpNumberOfBytesWritten
        );

    typedef
    BOOL
    (WINAPI
    *FUNC_CreateProcessA)(
        __in_opt    LPCSTR lpApplicationName,
        __inout_opt LPSTR lpCommandLine,
        __in_opt    LPSECURITY_ATTRIBUTES lpProcessAttributes,
        __in_opt    LPSECURITY_ATTRIBUTES lpThreadAttributes,
        __in        BOOL bInheritHandles,
        __in        DWORD dwCreationFlags,
        __in_opt    LPVOID lpEnvironment,
        __in_opt    LPCSTR lpCurrentDirectory,
        __in        LPSTARTUPINFOA lpStartupInfo,
        __out       LPPROCESS_INFORMATION lpProcessInformation
        );

    typedef
    HANDLE
    (WINAPI
    *FUNC_CreateRemoteThread)(
        __in      HANDLE hProcess,
        __in_opt  LPSECURITY_ATTRIBUTES lpThreadAttributes,
        __in      SIZE_T dwStackSize,
        __in      LPTHREAD_START_ROUTINE lpStartAddress,
        __in_opt  LPVOID lpParameter,
        __in      DWORD dwCreationFlags,
        __out_opt LPDWORD lpThreadId
        );

    void* LoadFunction ( const char* c, const char* a, const char* b );

    #define _DEFFUNCTION( name, a,b,c ) \
        inline FUNC_##name __##name ( void ) \
        { \
            static FUNC_##name pfn = NULL; \
            if ( !pfn ) \
                pfn = (FUNC_##name)LoadFunction ( #c, #a, #b ); \
            return pfn; \
        }

    #define DEFFUNCTION( a,b,c )    _DEFFUNCTION( a##b##c, a,b,c )

    #define _VirtualAllocEx         __VirtualAllocEx()
    #define _VirtualProtectEx       __VirtualProtectEx()
    #define _VirtualFreeEx          __VirtualFreeEx()
    #define _ReadProcessMemory      __ReadProcessMemory()
    #define _WriteProcessMemory     __WriteProcessMemory()
    #define _CreateProcessA         __CreateProcessA()
    #define _CreateRemoteThread     __CreateRemoteThread()

    DEFFUNCTION( Virt,ualAll,ocEx )
    DEFFUNCTION( Virt,ualPro,tectEx )
    DEFFUNCTION( Virt,ualFre,eEx )
    DEFFUNCTION( Read,Proces,sMemory )
    DEFFUNCTION( Writ,eProce,ssMemory )
    DEFFUNCTION( Crea,teProc,essA )
    DEFFUNCTION( Crea,teRemo,teThread )

#endif
