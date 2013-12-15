/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.Win32Utf8FileHooks.hpp
*  PURPOSE:     Hooks for making Windows file functions use UTF8 strings
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "detours/include/detours.h"

/*
The hooks in this file modify the following WinAPI functions to work correctly with utf8 strings:

    CreateFile
    LoadLibrary
    LoadLibraryEx
    SetDllDirectory
    SetCurrentDirectory
    AddFontResourceEx
    RemoveFontResourceEx
    RemoveDirectory
    GetDiskFreeSpaceEx
    GetFileAttributes
    SetFileAttributes
    ShellExecute
    CreateDirectory
    CopyFile
    MoveFile
    DeleteFile
    GetModuleHandle

The following functions will also work correctly with utf8 strings as they eventually call the hooked WinAPI functions:

    fopen
    mkdir
    rmdir
    delete
    rename
    remove
    unlink

BUT
    * Many other Win32 functions are not hooked and will need utf8 conversions when used, like these:
        FindFirstFile
        GetModuleFileName
        GetModuleFileNameEx
        GetCurrentDirectory
        GetDllDirectory
            and many more...
ALSO BUT
    * std::stream functions are not hooked, so each one use will require a ToUTF8() during the open call.
*/

namespace SharedUtil
{

    /////////////////////////////////////////////////////////////
    //
    // Function defs
    //
    /////////////////////////////////////////////////////////////

    typedef
    HANDLE
    (WINAPI
    *FUNC_CreateFileA)(
        __in     LPCSTR lpFileName,
        __in     DWORD dwDesiredAccess,
        __in     DWORD dwShareMode,
        __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        __in     DWORD dwCreationDisposition,
        __in     DWORD dwFlagsAndAttributes,
        __in_opt HANDLE hTemplateFile
        );

    typedef
    HMODULE
    (WINAPI
    *FUNC_LoadLibraryA)(
        __in LPCSTR lpLibFileName
        );

    typedef
    HMODULE
    (WINAPI
    *FUNC_LoadLibraryExA)(
        __in       LPCSTR lpLibFileName,
        __reserved HANDLE hFile,
        __in       DWORD dwFlags
        );

    typedef
    BOOL
    (WINAPI
    *FUNC_SetDllDirectoryA)(
        __in_opt LPCSTR lpPathName
        );

    typedef
    BOOL
    (WINAPI
    *FUNC_SetCurrentDirectoryA)(
        __in LPCSTR lpPathName
        );

    typedef int  (WINAPI *FUNC_AddFontResourceExA)( __in LPCSTR name, __in DWORD fl, __reserved PVOID res);
    typedef BOOL (WINAPI *FUNC_RemoveFontResourceExA)( __in LPCSTR name, __in DWORD fl, __reserved PVOID pdv);

    typedef
    BOOL
    (WINAPI
    *FUNC_RemoveDirectoryA)(
        __in LPCSTR lpPathName
        );

    typedef
    BOOL
    (WINAPI
    *FUNC_GetDiskFreeSpaceExA)(
        __in_opt  LPCSTR lpDirectoryName,
        __out_opt PULARGE_INTEGER lpFreeBytesAvailableToCaller,
        __out_opt PULARGE_INTEGER lpTotalNumberOfBytes,
        __out_opt PULARGE_INTEGER lpTotalNumberOfFreeBytes
        );

    typedef
    DWORD
    (WINAPI
    *FUNC_GetFileAttributesA)(
        __in LPCSTR lpFileName
        );

    typedef
    BOOL
    (WINAPI
    *FUNC_SetFileAttributesA)(
        __in LPCSTR lpFileName,
        __in DWORD dwFileAttributes
        );

    typedef HINSTANCE (STDAPICALLTYPE *FUNC_ShellExecuteA)(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters,
        LPCSTR lpDirectory, INT nShowCmd);

    typedef
    BOOL
    (WINAPI
    *FUNC_CreateDirectoryA)(
        __in     LPCSTR lpPathName,
        __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes
        );

    typedef
    BOOL
    (WINAPI
    *FUNC_CopyFileA)(
        __in LPCSTR lpExistingFileName,
        __in LPCSTR lpNewFileName,
        __in BOOL bFailIfExists
        );

    typedef
    BOOL
    (WINAPI
    *FUNC_MoveFileA)(
        __in LPCSTR lpExistingFileName,
        __in LPCSTR lpNewFileName
        );

    typedef
    BOOL
    (WINAPI
    *FUNC_DeleteFileA)(
        __in LPCSTR lpFileName
        );

    typedef
    HMODULE
    (WINAPI
    *FUNC_GetModuleHandleA)(
        __in_opt LPCSTR lpModuleName
        );


    /////////////////////////////////////////////////////////////
    //
    // Hook variables
    //
    /////////////////////////////////////////////////////////////
    #define HOOKVAR(name) \
        FUNC_##name pfn##name;

    HOOKVAR( CreateFileA )
    HOOKVAR( LoadLibraryA )
    HOOKVAR( LoadLibraryExA )
    HOOKVAR( SetDllDirectoryA )
    HOOKVAR( SetCurrentDirectoryA )
    HOOKVAR( AddFontResourceExA )
    HOOKVAR( RemoveFontResourceExA )
    HOOKVAR( RemoveDirectoryA )
    HOOKVAR( GetDiskFreeSpaceExA )
    HOOKVAR( GetFileAttributesA )
    HOOKVAR( SetFileAttributesA )
    HOOKVAR( ShellExecuteA )
    HOOKVAR( CreateDirectoryA )
    HOOKVAR( CopyFileA )
    HOOKVAR( MoveFileA )
    HOOKVAR( DeleteFileA )
    HOOKVAR( GetModuleHandleA )


    /////////////////////////////////////////////////////////////
    //
    // Hook implementations
    //
    /////////////////////////////////////////////////////////////
    HANDLE
    WINAPI
    MyCreateFileA(
        __in     LPCSTR lpFileName,
        __in     DWORD dwDesiredAccess,
        __in     DWORD dwShareMode,
        __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        __in     DWORD dwCreationDisposition,
        __in     DWORD dwFlagsAndAttributes,
        __in_opt HANDLE hTemplateFile
        )
    {
        HANDLE hResult = CreateFileW( FromUTF8( lpFileName ), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile );
#ifdef MTA_CLIENT
        if ( hResult == INVALID_HANDLE_VALUE )
        {
            // Fixes for GTA not working with unicode path
            hResult = pfnCreateFileA( lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile );
            if ( hResult == INVALID_HANDLE_VALUE && IsAbsolutePath( lpFileName ) && IsGTAProcess() )
            {
                if ( SStringX( lpFileName ).EndsWithI( "gta_sa.exe" )
                    || SStringX( lpFileName ).EndsWithI( "proxy_sa.exe" ) )
                {
                    // Try to fix broken path
                    for ( uint i = 1 ; i < 10 ; i++ )
                    {
                        SString strPathEnd = SStringX( lpFileName ).SplitRight( "\\", NULL, i );
                        SString strGuess = PathJoin( GetLaunchPath(), strPathEnd );
                        hResult = CreateFileW( FromUTF8( strGuess ), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile );
                        if ( hResult != INVALID_HANDLE_VALUE )
                            break;
                    }
                }
            }
        }
#endif
        return hResult;
    }

    HMODULE
    WINAPI
    MyLoadLibraryA(
        __in LPCSTR lpLibFileName
        )
    {
        return LoadLibraryW( FromUTF8( lpLibFileName ) );
    }

    HMODULE
    WINAPI
    MyLoadLibraryExA(
        __in       LPCSTR lpLibFileName,
        __reserved HANDLE hFile,
        __in       DWORD dwFlags
        )
    {
        return LoadLibraryExW( FromUTF8( lpLibFileName ), hFile, dwFlags );
    }

    BOOL
    WINAPI
    MySetDllDirectoryA(
        __in_opt LPCSTR lpPathName
        )
    {
        return SetDllDirectoryW( FromUTF8( lpPathName ) );
    }

    BOOL
    WINAPI
    MySetCurrentDirectoryA(
        __in LPCSTR lpPathName
        )
    {
        BOOL bResult = SetCurrentDirectoryW( FromUTF8( lpPathName ) );
#ifdef MTA_CLIENT
        if ( bResult == 0 )
        {
            // Fixes for GTA not working with unicode path
            bResult = pfnSetCurrentDirectoryA ( lpPathName );
            if ( bResult == 0 && IsAbsolutePath( lpPathName ) && IsGTAProcess() )
            {
                // Try to fix broken path
                for ( uint i = 1 ; i < 10 ; i++ )
                {
                    SString strPathEnd = SStringX( lpPathName ).SplitRight( "\\", NULL, i );
                    SString strGuess = PathJoin( GetLaunchPath(), strPathEnd );
                    bResult = SetCurrentDirectoryW( FromUTF8( strGuess ) );
                    if ( bResult )
                        break;
                }
            }
        }
#endif
        return bResult;
    }

    int  WINAPI MyAddFontResourceExA( __in LPCSTR name, __in DWORD fl, __reserved PVOID res        )
    {
        return AddFontResourceExW( FromUTF8( name ), fl, res );
    }

    BOOL WINAPI MyRemoveFontResourceExA( __in LPCSTR name, __in DWORD fl, __reserved PVOID pdv        )
    {
        return RemoveFontResourceExW( FromUTF8( name ), fl, pdv );
    }

    BOOL
    WINAPI
    MyRemoveDirectoryA(
        __in LPCSTR lpPathName
        )
    {
        return RemoveDirectoryW( FromUTF8( lpPathName ) );
    }

    BOOL
    WINAPI
    MyGetDiskFreeSpaceExA(
        __in_opt  LPCSTR lpDirectoryName,
        __out_opt PULARGE_INTEGER lpFreeBytesAvailableToCaller,
        __out_opt PULARGE_INTEGER lpTotalNumberOfBytes,
        __out_opt PULARGE_INTEGER lpTotalNumberOfFreeBytes
        )
    {
        return GetDiskFreeSpaceExW( FromUTF8( lpDirectoryName ), lpFreeBytesAvailableToCaller, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes );
    }

    DWORD
    WINAPI
    MyGetFileAttributesA(
        __in LPCSTR lpFileName
        )
    {
        return GetFileAttributesW( FromUTF8( lpFileName ) );
    }

    BOOL
    WINAPI
    MySetFileAttributesA(
        __in LPCSTR lpFileName,
        __in DWORD dwFileAttributes
        )
    {
        return SetFileAttributesW( FromUTF8( lpFileName ), dwFileAttributes );
    }

    HINSTANCE STDAPICALLTYPE MyShellExecuteA(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters,
        LPCSTR lpDirectory, INT nShowCmd        )
    {
        return ShellExecuteW( hwnd, FromUTF8( lpOperation ), FromUTF8( lpFile ), FromUTF8( lpParameters ), FromUTF8( lpDirectory ), nShowCmd );
    }

    BOOL
    WINAPI
    MyCreateDirectoryA(
        __in     LPCSTR lpPathName,
        __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes
        )
    {
        return CreateDirectoryW( FromUTF8( lpPathName ), lpSecurityAttributes );
    }

    BOOL
    WINAPI
    MyCopyFileA(
        __in LPCSTR lpExistingFileName,
        __in LPCSTR lpNewFileName,
        __in BOOL bFailIfExists
        )
    {
        return CopyFileW( FromUTF8( lpExistingFileName ), FromUTF8( lpNewFileName ), bFailIfExists );
    }

    BOOL
    WINAPI
    MyMoveFileA(
        __in LPCSTR lpExistingFileName,
        __in LPCSTR lpNewFileName
        )
    {
        return MoveFileW( FromUTF8( lpExistingFileName ), FromUTF8( lpNewFileName ) );
    }

    BOOL
    WINAPI
    MyDeleteFileA(
        __in LPCSTR lpFileName
        )
    {
        return DeleteFileW( FromUTF8( lpFileName ) );
    }

    HMODULE
    WINAPI
    MyGetModuleHandleA(
        __in_opt LPCSTR lpModuleName
        )
    {
        if ( !lpModuleName )
            return GetModuleHandleW( NULL );
        return GetModuleHandleW( FromUTF8( lpModuleName ) );
    }


    /////////////////////////////////////////////////////////////
    //
    // Hook adding
    //
    /////////////////////////////////////////////////////////////
    void AddUtf8FileHooks( void )
    {
        #define ADDHOOK(module,name) \
                pfn##name = reinterpret_cast < FUNC_##name > ( DetourFunction ( DetourFindFunction ( module, #name ), reinterpret_cast < PBYTE > ( My##name ) ) ); \
                assert( pfn##name );

        ADDHOOK( "Kernel32.dll", CreateFileA )
        ADDHOOK( "Kernel32.dll", LoadLibraryA )
        ADDHOOK( "Kernel32.dll", LoadLibraryExA )
        ADDHOOK( "Kernel32.dll", SetDllDirectoryA )
        ADDHOOK( "Kernel32.dll", SetCurrentDirectoryA )
        ADDHOOK( "Gdi32.dll",    AddFontResourceExA )
        ADDHOOK( "Gdi32.dll",    RemoveFontResourceExA )
        ADDHOOK( "Kernel32.dll", RemoveDirectoryA )
        ADDHOOK( "Kernel32.dll", GetDiskFreeSpaceExA )
        ADDHOOK( "Kernel32.dll", GetFileAttributesA )
        ADDHOOK( "Kernel32.dll", SetFileAttributesA )
        ADDHOOK( "Shell32.dll",  ShellExecuteA )
        ADDHOOK( "Kernel32.dll", CreateDirectoryA )
        ADDHOOK( "Kernel32.dll", CopyFileA )
        ADDHOOK( "Kernel32.dll", MoveFileA )
        ADDHOOK( "Kernel32.dll", DeleteFileA )
        ADDHOOK( "Kernel32.dll", GetModuleHandleA )
    }


    /////////////////////////////////////////////////////////////
    //
    // Hook taking away
    //
    /////////////////////////////////////////////////////////////
    void RemoveUtf8FileHooks( void )
    {
        #define DELHOOK(name) \
            if ( pfn##name ) \
            { \
                DetourRemove ( reinterpret_cast < PBYTE > ( pfn##name ),  \
                               reinterpret_cast < PBYTE > ( My##name  ) ); \
                pfn##name = NULL; \
            }

        DELHOOK( CreateFileA )
        DELHOOK( LoadLibraryA )
        DELHOOK( LoadLibraryExA )
        DELHOOK( SetDllDirectoryA )
        DELHOOK( SetCurrentDirectoryA )
        DELHOOK( AddFontResourceExA )
        DELHOOK( RemoveFontResourceExA )
        DELHOOK( RemoveDirectoryA )
        DELHOOK( GetDiskFreeSpaceExA )
        DELHOOK( GetFileAttributesA )
        DELHOOK( SetFileAttributesA )
        DELHOOK( ShellExecuteA )
        DELHOOK( CreateDirectoryA )
        DELHOOK( CopyFileA )
        DELHOOK( MoveFileA )
        DELHOOK( DeleteFileA )
        DELHOOK( GetModuleHandleA )
    }
}
