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
    CHECK_SERVICE_POST_CREATE = 7,
    CHECK_SERVICE_RESTART_GAME = 8,
};

#ifndef PROCESS_CREATION_MITIGATION_POLICY_IMAGE_LOAD_PREFER_SYSTEM32_ALWAYS_ON
    #define PROCESS_CREATION_MITIGATION_POLICY_EXTENSION_POINT_DISABLE_ALWAYS_ON       (0x00000001ui64 << 32)
    #define PROCESS_CREATION_MITIGATION_POLICY_PROHIBIT_DYNAMIC_CODE_ALWAYS_ON         (0x00000001ui64 << 36)
    #define PROCESS_CREATION_MITIGATION_POLICY_FONT_DISABLE_ALWAYS_ON                  (0x00000001ui64 << 48)
    #define PROCESS_CREATION_MITIGATION_POLICY_IMAGE_LOAD_NO_REMOTE_ALWAYS_ON          (0x00000001ui64 << 52)
    #define PROCESS_CREATION_MITIGATION_POLICY_IMAGE_LOAD_PREFER_SYSTEM32_ALWAYS_ON    (0x00000001ui64 << 60)
#endif

struct SOSVersionInfo
{
    DWORD dwMajor;
    DWORD dwMinor;
    DWORD dwBuild;
};

// Loads the given dll into hProcess. Returns 0 on failure or the handle to the
// remote dll module on success.
HMODULE         RemoteLoadLibrary                   ( HANDLE hProcess, const WString& strLibPath );
void            InsertWinMainBlock                  ( HANDLE hProcess );
void            RemoveWinMainBlock                  ( HANDLE hProcess );
void            ApplyLoadingCrashPatch              ( HANDLE hProcess );

void            TerminateGTAIfRunning               ( void );
bool            IsGTARunning                        ( void );
void            TerminateOtherMTAIfRunning          ( void );
bool            IsOtherMTARunning                   ( void );

bool            CommandLineContains                 ( const SString& strText );
void            DisplayErrorMessageBox              ( const SString& strMessage, const SString& strErrorCode = "", const SString& strTroubleType = "" );

void            SetMTASAPathSource                  ( bool bReadFromRegistry );
SString         GetMTASAPath                        ( void );
ePathResult     DiscoverGTAPath                     ( bool bFindIfMissing );
SString         GetGTAPath                          ( void );
bool            HasGTAPath                          ( void );

void            FindFilesRecursive                  ( const SString& strPathMatch, std::vector < SString >& outFileList, uint uiMaxDepth = 99 );
SOSVersionInfo  GetOSVersion                        ( void );
SOSVersionInfo  GetRealOSVersion                    ( void );
bool            IsWindows10OrGreater                ( void );
bool            IsWindows10Threshold2OrGreater      ( void );

bool            IsVS2013RuntimeInstalled            ( void );
BOOL            IsUserAdmin                         ( void );

void            UpdateMTAVersionApplicationSetting  ( bool bQuiet = false );
bool            Is32bitProcess                      ( DWORD processID );
void            TerminateProcess                    ( DWORD dwProcessID, uint uiExitCode = 0 );

bool            CreateSingleInstanceMutex           ( void );
void            ReleaseSingleInstanceMutex          ( void );

SString         CheckOnRestartCommand               ( void );
void            CleanDownloadCache                  ( void );

HMODULE         GetLibraryHandle                    ( const SString& strFilename, DWORD* pdwOutLastError = NULL );
void            FreeLibraryHandle                   ( void );
uint            WaitForObject                       ( HANDLE hProcess, HANDLE hThread, DWORD dwMilliseconds, HANDLE hMutex );
bool            CheckService                        ( uint uiStage );

void            MaybeShowCopySettingsDialog         ( void );

bool            CheckAndShowFileOpenFailureMessage  ( void );
void            CheckAndShowMissingFileMessage      ( void );
void            CheckAndShowModelProblems           ( void );
void            CheckAndShowUpgradeProblems         ( void );
void            CheckAndShowImgProblems             ( void );

void            BsodDetectionPreLaunch              ( void );
void            BsodDetectionOnGameBegin            ( void );
void            BsodDetectionOnGameEnd              ( void );
void            ForbodenProgramsMessage             ( void );
bool            VerifyEmbeddedSignature             ( const SString& strFilename );
void            LogSettings                         ( void );
SString         PadLeft                             ( const SString& strText, uint uiNumSpaces, char cCharacter );
bool            IsDeviceSelectionDialogOpen         ( DWORD dwThreadId );
std::vector < DWORD > MyEnumProcesses               ( bool bInclude64bit = false, bool bIncludeCurrent = false );
SString         GetProcessPathFilename              ( DWORD processID );
void            WriteDebugEventAndReport            ( uint uiId, const SString& strText );
WString         ReadCompatibilityEntries            ( const WString& strProgName, const WString& strSubKey, HKEY hKeyRoot, uint uiFlags );
bool            WriteCompatibilityEntries           ( const WString& strProgName, const WString& strSubKey, HKEY hKeyRoot, uint uiFlags, const WString& strNewData );

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
    #define _CreateProcessW         CreateProcessW
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
    *FUNC_CreateProcessW)(
        __in_opt    LPCWSTR lpApplicationName,
        __inout_opt LPWSTR lpCommandLine,
        __in_opt    LPSECURITY_ATTRIBUTES lpProcessAttributes,
        __in_opt    LPSECURITY_ATTRIBUTES lpThreadAttributes,
        __in        BOOL bInheritHandles,
        __in        DWORD dwCreationFlags,
        __in_opt    LPVOID lpEnvironment,
        __in_opt    LPCWSTR lpCurrentDirectory,
        __in        LPSTARTUPINFOW lpStartupInfo,
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

    typedef
    HRESULT
    (STDAPICALLTYPE
    *FUNC_WscGetSecurityProviderHealth)(DWORD Providers,
                                    PWSC_SECURITY_PROVIDER_HEALTH pHealth);

    typedef
    BOOL
    (WINAPI
    *FUNC_InitializeProcThreadAttributeList)(
        _Out_writes_bytes_to_opt_(*lpSize, *lpSize) LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
        _In_ DWORD dwAttributeCount,
        _Reserved_ DWORD dwFlags,
        _When_(lpAttributeList == nullptr, _Out_) _When_(lpAttributeList != nullptr, _Inout_) PSIZE_T lpSize
        );

    typedef
    VOID
    (WINAPI
    *FUNC_DeleteProcThreadAttributeList)(
        _Inout_ LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList
        );

    typedef
    BOOL
    (WINAPI
    *FUNC_UpdateProcThreadAttribute)(
        _Inout_ LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
        _In_ DWORD dwFlags,
        _In_ DWORD_PTR Attribute,
        _In_reads_bytes_opt_(cbSize) PVOID lpValue,
        _In_ SIZE_T cbSize,
        _Out_writes_bytes_opt_(cbSize) PVOID lpPreviousValue,
        _In_opt_ PSIZE_T lpReturnSize
        );

    void* LoadFunction ( const char* szLibName, const char* c, const char* a, const char* b );

    #define _DEFFUNCTION( lib, name, a,b,c ) \
        inline FUNC_##name __##name ( void ) \
        { \
            static FUNC_##name pfn = NULL; \
            if ( !pfn ) \
                pfn = (FUNC_##name)LoadFunction ( lib, #c, #a, #b ); \
            return pfn; \
        }

    #define DEFFUNCTION( lib, a,b,c )    _DEFFUNCTION( lib, a##b##c, a,b,c )

    #define _VirtualAllocEx                 __VirtualAllocEx()
    #define _VirtualProtectEx               __VirtualProtectEx()
    #define _VirtualFreeEx                  __VirtualFreeEx()
    #define _ReadProcessMemory              __ReadProcessMemory()
    #define _WriteProcessMemory             __WriteProcessMemory()
    #define _CreateProcessW                 __CreateProcessW()
    #define _CreateRemoteThread             __CreateRemoteThread()
    #define _WscGetSecurityProviderHealth   __WscGetSecurityProviderHealth()
    #define _InitializeProcThreadAttributeList  __InitializeProcThreadAttributeList()
    #define _DeleteProcThreadAttributeList      __DeleteProcThreadAttributeList()
    #define _UpdateProcThreadAttribute          __UpdateProcThreadAttribute()

    DEFFUNCTION( "kernel32", Virt,ualAll,ocEx )
    DEFFUNCTION( "kernel32", Virt,ualPro,tectEx )
    DEFFUNCTION( "kernel32", Virt,ualFre,eEx )
    DEFFUNCTION( "kernel32", Read,Proces,sMemory )
    DEFFUNCTION( "kernel32", Writ,eProce,ssMemory )
    DEFFUNCTION( "kernel32", Crea,teProc,essW )
    DEFFUNCTION( "kernel32", Crea,teRemo,teThread )
    DEFFUNCTION( "Wscapi", WscGetSecurityProviderHeal,t,h )
    DEFFUNCTION( "kernel32", Initiali,zeProcT,hreadAttributeList )
    DEFFUNCTION( "kernel32", Dele,teProcT,hreadAttributeList )
    DEFFUNCTION( "kernel32", Upda,teProcT,hreadAttribute )

#endif
