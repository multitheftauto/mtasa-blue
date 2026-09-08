/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        maetro32/maetro32.cpp
 *  PURPOSE:     This library implements kernel32 functionality, which is
 *               required by the VS2026 (v145 toolset) STL/CRT, but is missing
 *               on Windows 7, Windows 8, and Windows 8.1.
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include <type_traits>

#ifdef _WIN32_WINNT
    #undef _WIN32_WINNT
#endif

#define CopyFile2                      _CopyFile2
#define CreateFile2                    _CreateFile2
#define GetFileInformationByHandleEx   _GetFileInformationByHandleEx
#define GetSystemTimePreciseAsFileTime _GetSystemTimePreciseAsFileTime

#define _WIN32_WINNT _WIN32_WINNT_WIN10
#include <Windows.h>

#undef CopyFile2
#undef CreateFile2
#undef GetFileInformationByHandleEx
#undef GetSystemTimePreciseAsFileTime

#define EXPORT extern "C" __declspec(dllexport)

namespace kernel32
{
    static decltype(&_CreateFile2)                    CreateFile2;
    static decltype(&_CopyFile2)                      CopyFile2;
    static decltype(&_GetSystemTimePreciseAsFileTime) GetSystemTimePreciseAsFileTime;
    static decltype(&_GetFileInformationByHandleEx)   GetFileInformationByHandleEx;
}

EXPORT HANDLE WINAPI CreateFile2(_In_ LPCWSTR lpFileName, _In_ DWORD dwDesiredAccess, _In_ DWORD dwShareMode, _In_ DWORD dwCreationDisposition,
                                 _In_opt_ LPCREATEFILE2_EXTENDED_PARAMETERS pCreateExParams)
{
    if (kernel32::CreateFile2)
    {
        return kernel32::CreateFile2(lpFileName, dwDesiredAccess, dwShareMode, dwCreationDisposition, pCreateExParams);
    }

    DWORD                 dwFlagsAndAttributes = 0;
    LPSECURITY_ATTRIBUTES lpSecurityAttributes = nullptr;
    HANDLE                hTemplateFile = nullptr;

    if (pCreateExParams != nullptr)
    {
        dwFlagsAndAttributes = pCreateExParams->dwFileAttributes | pCreateExParams->dwFileFlags | pCreateExParams->dwSecurityQosFlags;
        lpSecurityAttributes = pCreateExParams->lpSecurityAttributes;
        hTemplateFile = pCreateExParams->hTemplateFile;
    }

    if (dwFlagsAndAttributes == 0)
        dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;

    return CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

EXPORT HRESULT WINAPI CopyFile2(_In_ PCWSTR pwszExistingFileName, _In_ PCWSTR pwszNewFileName, _In_opt_ COPYFILE2_EXTENDED_PARAMETERS* pExtendedParameters)
{
    if (kernel32::CopyFile2)
    {
        return kernel32::CopyFile2(pwszExistingFileName, pwszNewFileName, pExtendedParameters);
    }

    BOOL failIfExists = FALSE;

    if (pExtendedParameters != nullptr)
    {
        failIfExists = (pExtendedParameters->dwCopyFlags & COPY_FILE_FAIL_IF_EXISTS) != 0;

        if (pExtendedParameters->pfCancel && *pExtendedParameters->pfCancel)
            return HRESULT_FROM_WIN32(ERROR_REQUEST_ABORTED);
    }

    if (CopyFileW(pwszExistingFileName, pwszNewFileName, failIfExists))
        return S_OK;

    return HRESULT_FROM_WIN32(GetLastError());
}

EXPORT VOID WINAPI GetSystemTimePreciseAsFileTime(_Out_ LPFILETIME lpSystemTimeAsFileTime)
{
    if (kernel32::GetSystemTimePreciseAsFileTime)
    {
        kernel32::GetSystemTimePreciseAsFileTime(lpSystemTimeAsFileTime);
        return;
    }

    GetSystemTimeAsFileTime(lpSystemTimeAsFileTime);
}

EXPORT BOOL WINAPI GetFileInformationByHandleEx(_In_ HANDLE hFile, _In_ FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
                                                _Out_writes_bytes_(dwBufferSize) LPVOID lpFileInformation, _In_ DWORD dwBufferSize)
{
    if (kernel32::GetFileInformationByHandleEx)
    {
        if (kernel32::GetFileInformationByHandleEx(hFile, FileInformationClass, lpFileInformation, dwBufferSize))
            return TRUE;

        switch (GetLastError())
        {
            case ERROR_NOT_SUPPORTED:
            case ERROR_INVALID_PARAMETER:
                break;
            default:
                return FALSE;
        }
    }

    if (FileInformationClass == 0x12 /* FileIdInfo */)
    {
        BY_HANDLE_FILE_INFORMATION info;

        if (GetFileInformationByHandle(hFile, &info))
        {
            auto fileId = reinterpret_cast<PFILE_ID_INFO>(lpFileInformation);
            fileId->VolumeSerialNumber = info.dwVolumeSerialNumber;

            auto id = reinterpret_cast<DWORD*>(&fileId->FileId.Identifier[0]);
            id[0] = info.nFileIndexHigh;
            id[1] = info.nFileIndexLow;
            id[2] = 0;
            id[3] = 0;
            return TRUE;
        }
    }

    return FALSE;
}

static_assert(std::is_same_v<decltype(CopyFile2), decltype(_CopyFile2)>);
static_assert(std::is_same_v<decltype(CreateFile2), decltype(_CreateFile2)>);
static_assert(std::is_same_v<decltype(GetFileInformationByHandleEx), decltype(_GetFileInformationByHandleEx)>);
static_assert(std::is_same_v<decltype(GetSystemTimePreciseAsFileTime), decltype(_GetSystemTimePreciseAsFileTime)>);

template <typename T>
bool TryGetProcAddress(HMODULE library, LPCSTR functionName, T& value)
{
    if (const FARPROC procedure = GetProcAddress(library, functionName); procedure != nullptr)
    {
        value = reinterpret_cast<std::remove_reference_t<T>>(procedure);
        return true;
    }

    value = {};
    return false;
}

EXTERN_C BOOL WINAPI _DllMainCRTStartup(HANDLE, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        if (const HINSTANCE library = GetModuleHandleW(L"kernel32.dll"); library != nullptr)
        {
            TryGetProcAddress(library, "CreateFile2", kernel32::CreateFile2);
            TryGetProcAddress(library, "CopyFile2", kernel32::CopyFile2);
            TryGetProcAddress(library, "GetSystemTimePreciseAsFileTime", kernel32::GetSystemTimePreciseAsFileTime);
            TryGetProcAddress(library, "GetFileInformationByHandleEx", kernel32::GetFileInformationByHandleEx);
        }
    }

    return TRUE;
}
