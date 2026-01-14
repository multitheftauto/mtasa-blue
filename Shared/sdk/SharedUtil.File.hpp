/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.File.hpp
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#ifndef _WIN32
    #ifndef _LARGEFILE64_SOURCE
        #define _LARGEFILE64_SOURCE
    #endif
#endif

#include "SharedUtil.File.h"
#include "SharedUtil.Defines.h"
#include "SharedUtil.IntTypes.h"
#include "SharedUtil.Misc.h"
#include "SharedUtil.Buffer.h"
#include <algorithm>
#include <cctype>
#include <cstring>

#if __cplusplus >= 201703L // C++17
    #include <filesystem>
#endif

#ifdef _WIN32
    #ifndef NOMINMAX
    #define NOMINMAX
    #endif
    #include "Windows.h"
    #include "shellapi.h"
    #include "shlobj.h"
    #include <shlwapi.h>
    #include <io.h>
    #include <fcntl.h>
    #pragma comment(lib, "Shlwapi.lib")
#else
    #include <dirent.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <limits.h>
#endif

namespace SharedUtil
{
    namespace File
    {
        inline bool IsPathSafe(const char* filename) noexcept;
    }
}

//
// Returns true if the file exists
//
bool SharedUtil::FileExists(const std::string& strFilename) noexcept
{
#if __cplusplus >= 201703L
    namespace fs = std::filesystem;
    std::error_code errorCode;
    return fs::is_regular_file(strFilename.c_str(), errorCode);
#else
    #ifdef _WIN32
    if (strFilename.empty())
        return false;

    WString widePath;
    try
    {
        widePath = FromUTF8(strFilename);
    }
    catch (...)
    {
        return false;
    }
    if (widePath.empty())
        return false;
    DWORD attrs = GetFileAttributesW(widePath.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES)
        return false;
    return (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0;
    #else
    struct stat s;
    if (stat(strFilename.c_str(), &s) != 0)
        return false;
    return (s.st_mode & S_IFREG) != 0;
    #endif
#endif
}

//
// Returns true if the directory exists
//
bool SharedUtil::DirectoryExists(const std::string& strPath) noexcept
{
#if __cplusplus >= 201703L
    namespace fs = std::filesystem;
    std::error_code errorCode;
    return fs::is_directory(strPath.c_str(), errorCode);
#else
    #ifdef _WIN32
    if (strPath.empty())
        return false;

    WString widePath;
    try
    {
        widePath = FromUTF8(strPath);
    }
    catch (...)
    {
        return false;
    }
    if (widePath.empty())
        return false;
    DWORD attrs = GetFileAttributesW(widePath.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES)
        return false;
    return (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0;
    #else
    struct stat s;
    if (stat(strPath.c_str(), &s) != 0)
        return false;
    return (s.st_mode & S_IFDIR) != 0;
    #endif
#endif
}

bool SharedUtil::FileLoad(const SString& strFilename, SString& strBuffer, int iMaxSize, int iOffset)
{
    strBuffer = "";
    std::vector<char> buffer;
    if (!FileLoad(strFilename, buffer, iMaxSize, iOffset))
        return false;
    if (buffer.size())
        strBuffer = std::string(&buffer.at(0), buffer.size());

    return true;
}

bool SharedUtil::FileLoad(std::nothrow_t, const SString& filePath, SString& outBuffer, size_t maxSize, size_t offset) noexcept
{
    outBuffer.clear();

    if (!File::IsPathSafe(filePath.c_str()))
        return false;

    constexpr unsigned int GIBIBYTE = 1 * 1024 * 1024 * 1024;

    if (offset > GIBIBYTE)
        return false;

#ifdef _WIN32
    WString wideFilePath;

    try
    {
        wideFilePath = FromUTF8(filePath);
    }
    catch (...)
    {
        return false;
    }

    WIN32_FILE_ATTRIBUTE_DATA fileAttributeData;

    if (!GetFileAttributesExW(wideFilePath, GetFileExInfoStandard, &fileAttributeData))
        return false;

    if (fileAttributeData.nFileSizeHigh > 0 || fileAttributeData.nFileSizeLow > GIBIBYTE)
        return false;

    DWORD fileSize = fileAttributeData.nFileSizeLow;

    if (fileSize == 0 || fileSize <= static_cast<DWORD>(offset))
        return true;

    constexpr int MAX_RETRY_ATTEMPTS = 20;
    constexpr int RETRY_DELAY_MS = 10;

    HANDLE handle = INVALID_HANDLE_VALUE;
    for (int attempt = 0; attempt < MAX_RETRY_ATTEMPTS; ++attempt)
    {
        handle = CreateFileW(wideFilePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (handle != INVALID_HANDLE_VALUE)
            break;

        DWORD errorCode = GetLastError();
        if (errorCode != ERROR_SHARING_VIOLATION && errorCode != ERROR_LOCK_VIOLATION)
            break;

        if (attempt + 1 < MAX_RETRY_ATTEMPTS)
            Sleep(RETRY_DELAY_MS);
    }

    if (handle == INVALID_HANDLE_VALUE)
        return false;

    if (offset > 0)
    {
        LARGE_INTEGER seek{};
        seek.QuadPart = static_cast<LONGLONG>(offset);
        if (!SetFilePointerEx(handle, seek, nullptr, FILE_BEGIN))
        {
            CloseHandle(handle);
            return false;
        }
    }

    DWORD numBytesToRead = fileSize - static_cast<DWORD>(offset);

    if (static_cast<size_t>(numBytesToRead) > maxSize)
        numBytesToRead = static_cast<DWORD>(maxSize);

    if (numBytesToRead == 0)
    {
        CloseHandle(handle);
        return true;
    }

    try
    {
        outBuffer.resize(static_cast<size_t>(numBytesToRead));
    }
    catch (...)
    {
        CloseHandle(handle);
        outBuffer.clear();
        return false;
    }

    DWORD numBytesFromRead;

    if (!ReadFile(handle, &outBuffer[0], numBytesToRead, &numBytesFromRead, nullptr) || numBytesFromRead != numBytesToRead)
    {
        CloseHandle(handle);
        outBuffer.clear();
        return false;
    }

    CloseHandle(handle);
    return true;
#else
#ifdef __APPLE__
    struct stat info;

    if (stat(filePath, &info) != 0)
        return false;
#else
    struct stat64 info;

    if (stat64(filePath, &info) != 0)
        return false;
#endif

    size_t fileSize = static_cast<size_t>(info.st_size);

    if (fileSize > GIBIBYTE)
        return false;

    if (fileSize == 0 || static_cast<size_t>(fileSize) <= offset)
        return true;

    size_t numBytesToRead = fileSize - offset;

    if (numBytesToRead > maxSize)
        numBytesToRead = maxSize;

    FILE* handle = fopen(filePath, "rb");

    if (!handle)
        return false;

    try
    {
        outBuffer.resize(numBytesToRead);
    }
    catch (...)
    {
        fclose(handle);
        outBuffer.clear();
        return false;
    }

#ifdef __APPLE__
    if (fseeko(handle, static_cast<off_t>(offset), SEEK_SET) != 0)
#else
    if (fseeko64(handle, static_cast<off64_t>(offset), SEEK_SET) != 0)
#endif
    {
        fclose(handle);
        outBuffer.clear();
        return false;
    }

    size_t numBytesFromRead = fread(&outBuffer[0], 1, numBytesToRead, handle);
    fclose(handle);

    if (numBytesFromRead != numBytesToRead)
    {
        outBuffer.clear();
        return false;
    }

    return true;
#endif
}

bool SharedUtil::FileSave(const SString& strFilename, const SString& strBuffer, bool bForce)
{
    return FileSave(strFilename, strBuffer.length() ? &strBuffer.at(0) : NULL, strBuffer.length(), bForce);
}

bool SharedUtil::FileAppend(const SString& strFilename, const SString& strBuffer, bool bForce)
{
    return FileAppend(strFilename, strBuffer.length() ? &strBuffer.at(0) : NULL, strBuffer.length(), bForce);
}

bool SharedUtil::FileDelete(const SString& strFilename, bool bForce)
{
#ifdef _WIN32
    if (bForce)
        SetFileAttributesW(FromUTF8(strFilename), FILE_ATTRIBUTE_NORMAL);
#endif
    return File::Delete(strFilename) == 0;
}

bool SharedUtil::FileRename(const SString& strFilenameOld, const SString& strFilenameNew, int* pOutErrorCode)
{
#ifdef _WIN32
    if (MoveFileExW(FromUTF8(strFilenameOld), FromUTF8(strFilenameNew), MOVEFILE_COPY_ALLOWED) == 0)
    {
        int errorCode = GetLastError();
        if (errorCode == ERROR_ACCESS_DENIED)
        {
            if (!FileExists(strFilenameNew) && FileCopy(strFilenameOld, strFilenameNew))
            {
                FileDelete(strFilenameOld);
                if (!FileExists(strFilenameOld))
                    return true;
                FileDelete(strFilenameNew);
            }
        }
        if (pOutErrorCode)
            *pOutErrorCode = errorCode;
        return false;
    }
    return true;
#else
    if (rename(strFilenameOld, strFilenameNew) != 0)
    {
        if (pOutErrorCode)
            *pOutErrorCode = errno;
        return false;
    }
    return true;
#endif
}

//
// Load binary data from a file into an array
//
bool SharedUtil::FileLoad(const SString& strFilename, std::vector<char>& buffer, int iMaxSize, int iOffset)
{
    buffer.clear();
    FILE* fh = File::Fopen(strFilename, "rb");
    if (!fh)
        return false;

    fseek(fh, 0, SEEK_END);
#ifdef _WIN32
    int64 rawSize = _ftelli64(fh);
#elif defined(__APPLE__)
    int64 rawSize = ftello(fh);
#else
    int64 rawSize = ftello64(fh);
#endif
    int size = static_cast<int>(std::min<int64>(INT_MAX, rawSize));

    iOffset = std::min(iOffset, size);
    fseek(fh, iOffset, SEEK_SET);
    size -= iOffset;

    int bytesRead = 0;
    if (size > 0 && size < 1e9)
    {
        size = std::min(size, iMaxSize);
        try
        {
            buffer.assign(size, 0);
        }
        catch (...)
        {
            fclose(fh);
            return false;
        }
        bytesRead = fread(&buffer.at(0), 1, size, fh);
    }
    fclose(fh);
    return bytesRead == size;
}

//
// Save binary data to a file
//
bool SharedUtil::FileSave(const SString& strFilename, const void* pBuffer, unsigned long ulSize, bool bForce)
{
#ifdef _WIN32
    if (bForce)
        SetFileAttributesW(FromUTF8(strFilename), FILE_ATTRIBUTE_NORMAL);
#endif

    if (bForce)
        MakeSureDirExists(strFilename);

    FILE* fh = File::Fopen(strFilename, "wb");
    if (!fh)
        return false;

    bool bSaveOk = true;
    if (ulSize)
        bSaveOk = (fwrite(pBuffer, 1, ulSize, fh) == ulSize);
    fclose(fh);
    return bSaveOk;
}

//
// Append binary data to a file
//
bool SharedUtil::FileAppend(const SString& strFilename, const void* pBuffer, unsigned long ulSize, bool bForce)
{
#ifdef _WIN32
    if (bForce)
        SetFileAttributesW(FromUTF8(strFilename), FILE_ATTRIBUTE_NORMAL);
#endif

    FILE* fh = File::Fopen(strFilename, "ab");
    if (!fh)
        return false;

    bool bSaveOk = true;
    if (ulSize)
        bSaveOk = (fwrite(pBuffer, 1, ulSize, fh) == ulSize);
    fclose(fh);
    return bSaveOk;
}

//
// Get a file size
//
uint64 SharedUtil::FileSize(const SString& strFilename)
{
    // Open
    FILE* fh = File::Fopen(strFilename, "rb");
    if (!fh)
        return 0;
    // Get size
    fseek(fh, 0, SEEK_END);
#ifdef _WIN32
    uint64 size = _ftelli64(fh);
#elif defined(__APPLE__)
    uint64 size = ftello(fh);
#else
    uint64 size = ftello64(fh);
#endif
    // Close
    fclose(fh);
    return size;
}

//
// Ensure all directories exist to the file
//
void SharedUtil::MakeSureDirExists(const SString& strPath)
{
#ifdef _WIN32
    std::vector<SString> parts;
    PathConform(strPath).Split(PATH_SEPERATOR, parts);

    // Find first dir that already exists
    int idx = parts.size() - 1;
    for (; idx >= 0; idx--)
    {
        SString strTemp = SString::Join(PATH_SEPERATOR, parts, 0, idx);
        if (DirectoryExists(strTemp))
            break;
    }

    // Make non existing dirs only
    idx++;
    for (; idx < (int)parts.size(); idx++)
    {
        SString strTemp = SString::Join(PATH_SEPERATOR, parts, 0, idx);
        // Call mkdir on this path
        File::Mkdir(strTemp);
    }
#else
    std::filesystem::path filePath = static_cast<std::string>(PathConform(strPath));
    std::error_code ec{};
    std::filesystem::create_directories(filePath.parent_path(), ec);
#endif
}

SString SharedUtil::PathConform(const SString& strPath)
{
    // Make slashes the right way and remove duplicates, except for UNC type indicators
#ifdef _WIN32
    SString strTemp = strPath.Replace("/", PATH_SEPERATOR);
#else
    SString strTemp = strPath.Replace("\\", PATH_SEPERATOR);
#endif
    // Remove slash duplicates
    size_t iFirstDoubleSlash = strTemp.find(PATH_SEPERATOR PATH_SEPERATOR);
    if (iFirstDoubleSlash == std::string::npos)
        return strTemp;            // No duplicates present

    // If first double slash is not at the start, then treat as a normal duplicate if:
    //      1. It is not preceeded by a colon, or
    //      2. Another single slash is before it
    if (iFirstDoubleSlash > 0)
    {
        if (iFirstDoubleSlash == 2 && strTemp[1] == ':')
        {
            // Replace all duplicate slashes
            return strTemp.Replace(PATH_SEPERATOR PATH_SEPERATOR, PATH_SEPERATOR, true);
        }

        if (strTemp.SubStr(iFirstDoubleSlash - 1, 1) != ":" || strTemp.find(PATH_SEPERATOR) < iFirstDoubleSlash)
        {
            // Replace all duplicate slashes
            return strTemp.Replace(PATH_SEPERATOR PATH_SEPERATOR, PATH_SEPERATOR, true);
        }
    }

    return strTemp.Left(iFirstDoubleSlash + 1) + strTemp.SubStr(iFirstDoubleSlash + 1).Replace(PATH_SEPERATOR PATH_SEPERATOR, PATH_SEPERATOR, true);
}

SString SharedUtil::PathJoin(const SString& str1, const SString& str2)
{
    return PathConform(str1 + PATH_SEPERATOR + str2);
}

SString SharedUtil::PathJoin(const SString& str1, const SString& str2, const SString& str3, const SString& str4, const SString& str5)
{
    SString strResult = str1 + PATH_SEPERATOR + str2 + PATH_SEPERATOR + str3;
    if (str4.length())
        strResult += PATH_SEPERATOR + str4;
    if (str5.length())
        strResult += PATH_SEPERATOR + str5;
    return PathConform(strResult);
}

// Remove base path from the start of abs path
SString SharedUtil::PathMakeRelative(const SString& strInBasePath, const SString& strInAbsPath)
{
    SString strBasePath = PathConform(strInBasePath);
    SString strAbsPath = PathConform(strInAbsPath);
    if (strAbsPath.BeginsWithI(strBasePath))
    {
        return strAbsPath.SubStr(strBasePath.length()).TrimStart(PATH_SEPERATOR);
    }
    return strAbsPath;
}

SString SharedUtil::GetSystemCurrentDirectory()
{
#ifdef _WIN32
    wchar_t szResult[1024] = L"";
    GetCurrentDirectoryW(NUMELMS(szResult), szResult);
    if (IsShortPathName(szResult))
        return GetSystemLongPathName(ToUTF8(szResult));
    return ToUTF8(szResult);
#else
    char szBuffer[PATH_MAX];
    getcwd(szBuffer, PATH_MAX - 1);
    return szBuffer;
#endif
}

#ifdef _WIN32
#ifdef MTA_CLIENT

SString SharedUtil::GetSystemDllDirectory()
{
    wchar_t szResult[1024] = L"";
    GetDllDirectoryW(NUMELMS(szResult), szResult);
    if (IsShortPathName(szResult))
        return GetSystemLongPathName(ToUTF8(szResult));
    return ToUTF8(szResult);
}

SString SharedUtil::GetSystemLocalAppDataPath()
{
    wchar_t szResult[MAX_PATH] = L"";
    SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, szResult);
    if (IsShortPathName(szResult))
        return GetSystemLongPathName(ToUTF8(szResult));
    return ToUTF8(szResult);
}

SString SharedUtil::GetSystemCommonAppDataPath()
{
    wchar_t szResult[MAX_PATH] = L"";
    SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szResult);
    if (IsShortPathName(szResult))
        return GetSystemLongPathName(ToUTF8(szResult));
    return ToUTF8(szResult);
}

SString SharedUtil::GetSystemPersonalPath()
{
    wchar_t szResult[MAX_PATH] = L"";
    SHGetFolderPathW(NULL, CSIDL_PERSONAL, NULL, 0, szResult);
    if (IsShortPathName(szResult))
        return GetSystemLongPathName(ToUTF8(szResult));
    return ToUTF8(szResult);
}

SString SharedUtil::GetSystemWindowsPath()
{
    wchar_t szResult[MAX_PATH] = L"";
    SHGetFolderPathW(NULL, CSIDL_WINDOWS, NULL, 0, szResult);
    if (IsShortPathName(szResult))
        return GetSystemLongPathName(ToUTF8(szResult));
    return ToUTF8(szResult);
}

SString SharedUtil::GetSystemSystemPath()
{
    wchar_t szResult[MAX_PATH] = L"";
    SHGetFolderPathW(NULL, CSIDL_SYSTEM, NULL, 0, szResult);
    if (IsShortPathName(szResult))
        return GetSystemLongPathName(ToUTF8(szResult));
    return ToUTF8(szResult);
}

SString SharedUtil::GetSystemTempPath()
{
    wchar_t szResult[4030] = L"";
    GetTempPathW(4000, szResult);
    if (IsShortPathName(szResult))
        return GetSystemLongPathName(ToUTF8(szResult));
    return ToUTF8(szResult);
}

SString SharedUtil::GetMTADataPath()
{
    return PathJoin(GetSystemCommonAppDataPath(), GetProductCommonDataDir(), GetMajorVersionString());
}

SString SharedUtil::GetMTADataPathCommon()
{
    return PathJoin(GetSystemCommonAppDataPath(), GetProductCommonDataDir(), "Common");
}

SString SharedUtil::GetMTATempPath()
{
    return PathJoin(GetSystemTempPath(), "MTA" + GetMajorVersionString());
}

// C:\Program Files\gta_sa.exe
SString SharedUtil::GetLaunchPathFilename()
{
    static SString strLaunchPathFilename;
    if (strLaunchPathFilename.empty())
    {
        wchar_t szBuffer[2048];
        GetModuleFileNameW(NULL, szBuffer, NUMELMS(szBuffer) - 1);

        wchar_t fullPath[MAX_PATH];
        GetFullPathNameW(szBuffer, MAX_PATH, fullPath, nullptr);

        strLaunchPathFilename = ToUTF8(fullPath);
    }
    return strLaunchPathFilename;
}

// C:\Program Files
SString SharedUtil::GetLaunchPath()
{
    return ExtractPath(GetLaunchPathFilename());
}

// gta_sa.exe
SString SharedUtil::GetLaunchFilename()
{
    return ExtractFilename(GetLaunchPathFilename());
}

// Get drive root from path
SString SharedUtil::GetPathDriveName(const SString& strPath)
{
    wchar_t szDrive[4] = L"";
    int     iDriveNumber = PathGetDriveNumberW(FromUTF8(strPath));
    if (iDriveNumber > -1)
        PathBuildRootW(szDrive, iDriveNumber);
    return ToUTF8(szDrive);
}

// Get drive free bytes available to the current user
uint SharedUtil::GetPathFreeSpaceMB(const SString& strPath)
{
    SString strDrive = GetPathDriveName(strPath);
    if (!strDrive.empty())
    {
        ULARGE_INTEGER llUserFreeBytesAvailable;
        if (GetDiskFreeSpaceExW(FromUTF8(strDrive), &llUserFreeBytesAvailable, NULL, NULL))
        {
            llUserFreeBytesAvailable.QuadPart /= 1048576UL;
            if (llUserFreeBytesAvailable.HighPart == 0)
                return llUserFreeBytesAvailable.LowPart;
        }
    }
    return -1;
}

SString SharedUtil::GetDriveNameWithNotEnoughSpace(uint uiResourcesPathMinMB, uint uiDataPathMinMB)
{
    SString strFileCachePath = GetCommonRegistryValue("", "File Cache Path");
    if (!strFileCachePath.empty() && DirectoryExists(PathJoin(strFileCachePath, "resources")))
        if (GetPathFreeSpaceMB(strFileCachePath) < uiResourcesPathMinMB)
            return GetPathDriveName(strFileCachePath);

    if (GetPathFreeSpaceMB(GetMTASABaseDir()) < uiResourcesPathMinMB)
        return GetPathDriveName(GetMTASABaseDir());

    if (GetPathFreeSpaceMB(GetSystemCommonAppDataPath()) < uiDataPathMinMB)
        return GetPathDriveName(GetSystemCommonAppDataPath());
    return "";
}

#endif  // #ifdef MTA_CLIENT
#endif  // #ifdef _WIN32

WString SharedUtil::FromUTF8(const SString& strPath)
{
#ifdef WIN32_TESTING   // This might be faster - Needs testing
    const char* szSrc = strPath;
    uint        cCharacters = strlen(szSrc) + 1;
    uint        cbUnicode = cCharacters * 4;
    wchar_t*    Dest = (wchar_t*)alloca(cbUnicode);

    if (MultiByteToWideChar(CP_UTF8, 0, szSrc, -1, Dest, (int)cbUnicode) == 0)
    {
        return WString();
    }
    else
    {
        return Dest;
    }
#else
    return MbUTF8ToUTF16(strPath);
#endif
}

SString SharedUtil::ToUTF8(const WString& strPath)
{
#ifdef WIN32_TESTING   // This might be faster - Needs testing
    const wchar_t* pszW = strPath;
    uint           cCharacters = wcslen(pszW) + 1;
    uint           cbAnsi = cCharacters * 6;
    char*          pData = (char*)alloca(cbAnsi);

    if (0 == WideCharToMultiByte(CP_UTF8, 0, pszW, cCharacters, pData, cbAnsi, NULL, NULL))
    {
        return "";
    }
    return pData;
#else
    return UTF16ToMbUTF8(strPath);
#endif
}

#ifdef _WIN32
///////////////////////////////////////////////////////////////
//
// DelTree
//
//
//
///////////////////////////////////////////////////////////////
bool SharedUtil::DelTree(const SString& strPath, const SString& strInsideHere)
{
    // Safety: Make sure strPath is inside strInsideHere
    WString wstrPath = FromUTF8(strPath);
    WString wstrInsideHere = FromUTF8(strInsideHere);
    if (!wstrPath.BeginsWithI(wstrInsideHere))
    {
        assert(0);
        return false;
    }

    DWORD                dwBufferSize = (wstrPath.length() + 3) * sizeof(wchar_t);
    CScopeAlloc<wchar_t> szBuffer(dwBufferSize);
    memset(szBuffer, 0, dwBufferSize);
    wcsncpy(szBuffer, wstrPath, wstrPath.length());
    SHFILEOPSTRUCTW sfos;

    sfos.hwnd = NULL;
    sfos.wFunc = FO_DELETE;
    sfos.pFrom = szBuffer;            // Double NULL terminated
    sfos.pTo = NULL;
    sfos.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;

    int status = SHFileOperationW(&sfos);
    return status == 0;
}
#endif

///////////////////////////////////////////////////////////////
//
// MkDir
//
// Returns true if the directory is created or already exists
// TODO: Make bTree off option work
//
///////////////////////////////////////////////////////////////
bool SharedUtil::MkDir(const SString& strInPath, bool bTree)
{
    SString strPath = PathConform(strInPath);
    MakeSureDirExists(strPath + PATH_SEPERATOR);
    return DirectoryExists(strPath);
}

///////////////////////////////////////////////////////////////
//
// FileCopy
//
// Copies a single file.
//
///////////////////////////////////////////////////////////////
bool SharedUtil::FileCopy(const SString& strSrc, const SString& strDest, bool bForce)
{
    if (bForce)
        MakeSureDirExists(strDest);

#ifdef _WIN32
    if (bForce)
        SetFileAttributesW(FromUTF8(strDest), FILE_ATTRIBUTE_NORMAL);
#endif

    FILE* fhSrc = File::Fopen(strSrc, "rb");
    if (!fhSrc)
    {
        return false;
    }

    FILE* fhDst = File::Fopen(strDest, "wb");
    if (!fhDst)
    {
        fclose(fhSrc);
        return false;
    }

    char cBuffer[65536];
    bool ok = true;
    while (true)
    {
        size_t dataLength = fread(cBuffer, 1, 65536, fhSrc);
        if (dataLength == 0)
        {
            if (ferror(fhSrc))
                ok = false;
            break;
        }
        if (fwrite(cBuffer, 1, dataLength, fhDst) != dataLength)
        {
            ok = false;
            break;
        }
    }

    fclose(fhSrc);
    fclose(fhDst);
    return ok;
}

#ifdef _WIN32
///////////////////////////////////////////////////////////////
//
// FindFiles
//
// Find all files or directories at a path
// If sorted by date, returns last modified last
//
///////////////////////////////////////////////////////////////
std::vector<SString> SharedUtil::FindFiles(const SString& strInMatch, bool bFiles, bool bDirectories, bool bSortByDate)
{
    std::vector<SString>           strResult;
    std::multimap<uint64, SString> sortMap;

    SString strMatch = PathConform(strInMatch);
    if (strMatch.Right(1) == PATH_SEPERATOR)
        strMatch += "*";

    _WIN32_FIND_DATAW findData;
    HANDLE           hFind = FindFirstFileW(FromUTF8(strMatch), &findData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        try
        {
            do
            {
                if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? bDirectories : bFiles)
                    if (wcscmp(findData.cFileName, L".") && wcscmp(findData.cFileName, L".."))
                    {
                        if (bSortByDate)
                            MapInsert(sortMap, (uint64&)findData.ftLastWriteTime, ToUTF8(findData.cFileName));
                        else
                            strResult.push_back(ToUTF8(findData.cFileName));
                    }
            } while (FindNextFileW(hFind, &findData));
        }
        catch (...)
        {
            FindClose(hFind);
            throw;
        }
        FindClose(hFind);
    }

    // Resolve sorted map if required
    if (!sortMap.empty())
    {
        for (std::multimap<uint64, SString>::iterator iter = sortMap.begin(); iter != sortMap.end(); ++iter)
            strResult.push_back(iter->second);
    }

    return strResult;
}

#else

std::vector<SString> SharedUtil::FindFiles(const SString& strMatch, bool bFiles, bool bDirectories, bool bSortByDate)
{
    std::vector<SString>           strResult;
    std::multimap<uint64, SString> sortMap;

    DIR*           Dir;
    struct dirent* DirEntry;

    // Extract any filename matching characters
    SString strFileMatch;
    SString strSearchDirectory = PathJoin(PathConform(strMatch).SplitLeft("/", &strFileMatch, -1), "/");

    if ((Dir = opendir(strSearchDirectory)))
    {
        while ((DirEntry = readdir(Dir)) != NULL)
        {
            // Skip dotted entries
            if (strcmp(DirEntry->d_name, ".") && strcmp(DirEntry->d_name, ".."))
            {
                struct stat Info;
                bool        bIsDir = false;

                // Do wildcard matching if required
                if (!strFileMatch.empty() && !WildcardMatch(strFileMatch, DirEntry->d_name))
                {
                    continue;
                }

                SString strPath = PathJoin(strSearchDirectory, DirEntry->d_name);

                // Determine the file stats
                if (lstat(strPath, &Info) != -1)
                    bIsDir = S_ISDIR(Info.st_mode);

                if (bIsDir ? bDirectories : bFiles)
                {
                    if (bSortByDate)
                    {
                        SString     strAbsPath = strSearchDirectory + DirEntry->d_name;
                        struct stat attrib;
                        stat(strAbsPath, &attrib);
                        MapInsert(sortMap, (uint64)attrib.st_mtime, SStringX(DirEntry->d_name));
                    }
                    else
                        strResult.push_back(DirEntry->d_name);
                }
            }
        }
        closedir(Dir);
    }

    // Resolve sorted map if required
    if (!sortMap.empty())
    {
        for (std::multimap<uint64, SString>::iterator iter = sortMap.begin(); iter != sortMap.end(); ++iter)
            strResult.push_back(iter->second);
    }

    return strResult;
}
#endif

void SharedUtil::ExtractFilename(const SString& strInPathFilename, SString* strPath, SString* strFilename)
{
    const SString strPathFilename = PathConform(strInPathFilename);
    if (!strPathFilename.Split(PATH_SEPERATOR, strPath, strFilename, -1))
        if (strFilename)
            *strFilename = strPathFilename;
}

bool SharedUtil::ExtractExtension(const SString& strFilename, SString* strMain, SString* strExt)
{
    return strFilename.Split(".", strMain, strExt, -1);
}

SString SharedUtil::ExtractPath(const SString& strPathFilename)
{
    SString strPath;
    ExtractFilename(strPathFilename, &strPath, NULL);
    return strPath;
}

SString SharedUtil::ExtractFilename(const SString& strPathFilename)
{
    SString strFilename;
    ExtractFilename(strPathFilename, NULL, &strFilename);
    return strFilename;
}

SString SharedUtil::ExtractExtension(const SString& strPathFilename)
{
    SString strExt;
    ExtractExtension(strPathFilename, NULL, &strExt);
    return strExt;
}

SString SharedUtil::ExtractBeforeExtension(const SString& strPathFilename)
{
    SString strMain;
    ExtractExtension(strPathFilename, &strMain, NULL);
    return strMain;
}

SString SharedUtil::MakeUniquePath(const SString& strInPathFilename)
{
    const SString strPathFilename = PathConform(strInPathFilename);

    SString strBeforeUniqueChar, strAfterUniqueChar;

    SString strPath, strFilename;
    ExtractFilename(strPathFilename, &strPath, &strFilename);

    SString strMain, strExt;
    if (ExtractExtension(strFilename, &strMain, &strExt))
    {
        strBeforeUniqueChar = PathJoin(strPath, strMain);
        strAfterUniqueChar = "." + strExt;
    }
    else
    {
        strBeforeUniqueChar = strPathFilename;
        strAfterUniqueChar = "";
    }

    SString strTest = strPathFilename;
    int     iCount = 1;
#ifdef _WIN32
    while (GetFileAttributesW(FromUTF8(strTest)) != INVALID_FILE_ATTRIBUTES)
#else
    while (DirectoryExists(strTest) || FileExists(strTest))
#endif
    {
        strTest = SString("%s_%d%s", strBeforeUniqueChar.c_str(), iCount++, strAfterUniqueChar.c_str());
    }
    return strTest;
}

// Tries to resolve the original path used for MakeUniquePath
SString SharedUtil::MakeGenericPath(const SString& uniqueFilePath)
{
    if (DirectoryExists(uniqueFilePath) || FileExists(uniqueFilePath))
        return uniqueFilePath;

    SString basePath, fileName;
    ExtractFilename(uniqueFilePath, &basePath, &fileName);

    SString withoutExtension, extensionName;
    bool    usingExtension = ExtractExtension(fileName, &withoutExtension, &extensionName);
    size_t  underscore = withoutExtension.find_last_not_of("0123456789");

    if (underscore != std::string::npos)
    {
        if (withoutExtension[underscore] == '_')
        {
            withoutExtension = withoutExtension.SubStr(0, underscore);

            SString filePath;

            if (usingExtension)
                filePath = PathJoin(basePath, SString("%s.%s", withoutExtension.c_str(), extensionName.c_str()));
            else
                filePath = PathJoin(basePath, withoutExtension);

            if (DirectoryExists(filePath) || FileExists(filePath))
                return filePath;
        }
    }

    return {};
}

// Conform a path string for sorting
SString SharedUtil::ConformPathForSorting(const SString& strPathFilename)
{
    SString strResult = strPathFilename;
    std::transform(strResult.begin(), strResult.end(), strResult.begin(), [](int c) {
        // Ignores locale and always does this:
        if (c >= 'A' && c <= 'Z')
            c = c - 'A' + 'a';
        return c;
    });
    return strResult;
}

// Return true if path is not relative
bool SharedUtil::IsAbsolutePath(const SString& strInPath)
{
    const SString strPath = PathConform(strInPath);

    if (strPath.BeginsWith(PATH_SEPERATOR))
        return true;
#ifdef _WIN32
    if (strPath.length() > 0 && strPath[1] == ':')
        return true;
#endif
    return false;
}

#ifdef _WIN32
bool SharedUtil::IsShortPathName(const char* szPath)
{
    return strchr(szPath, '~') != NULL;
}

bool SharedUtil::IsShortPathName(const wchar_t* szPath)
{
    return wcschr(szPath, '~') != NULL;
}

SString SharedUtil::GetSystemShortPathName(const SString& strPath)
{
    wchar_t szBuffer[32000];
    szBuffer[0] = 0;
    if (!GetShortPathNameW(FromUTF8(strPath), szBuffer, NUMELMS(szBuffer) - 1))
        return strPath;
    return ToUTF8(szBuffer);
}

SString SharedUtil::GetSystemLongPathName(const SString& strPath)
{
    wchar_t szBuffer[32000];
    szBuffer[0] = 0;
    if (!GetLongPathNameW(FromUTF8(strPath), szBuffer, NUMELMS(szBuffer) - 1))
        return strPath;
    return ToUTF8(szBuffer);
}
#endif // _WIN32

namespace SharedUtil { namespace File {
#ifdef _WIN32
    inline bool IsReservedName(const char* path) noexcept
    {
        const char* name = path;
        for (const char* p = path; *p; ++p)
        {
            if (*p == '\\' || *p == '/')
                name = p + 1;
        }

        std::size_t len = 0;
        while (name[len] && name[len] != '.' && name[len] != ':')
            ++len;

        while (len > 0 && (name[len - 1] == ' ' || name[len - 1] == '.'))
            --len;

        auto up = [](char c) noexcept -> char {
            return (c >= 'a' && c <= 'z') ? static_cast<char>(c - 'a' + 'A') : c;
        };

        if (len == 3)
        {
            char a = up(name[0]), b = up(name[1]), c = up(name[2]);
            return (a == 'C' && b == 'O' && c == 'N') ||
                   (a == 'P' && b == 'R' && c == 'N') ||
                   (a == 'A' && b == 'U' && c == 'X') ||
                   (a == 'N' && b == 'U' && c == 'L');
        }
        if (len == 4 && name[3] >= '1' && name[3] <= '9')
        {
            char a = up(name[0]), b = up(name[1]), c = up(name[2]);
            return (a == 'C' && b == 'O' && c == 'M') ||
                   (a == 'L' && b == 'P' && c == 'T');
        }
        if (len == 6 && name[5] == '$')
        {
            char a = up(name[0]), b = up(name[1]), c = up(name[2]);
            char d = up(name[3]), e = up(name[4]);
            return a == 'C' && b == 'O' && c == 'N' && d == 'I' && e == 'N';
        }
        if (len == 7 && name[6] == '$')
        {
            char a = up(name[0]), b = up(name[1]), c = up(name[2]);
            char d = up(name[3]), e = up(name[4]), f = up(name[5]);
            return a == 'C' && b == 'O' && c == 'N' && d == 'O' && e == 'U' && f == 'T';
        }
        return false;
    }
#endif

    inline bool IsPathSafe(const char* filename) noexcept
    {
        if (!filename || !*filename)
            return false;

        constexpr std::size_t maxPathLen = 4'096;
        if (std::strlen(filename) > maxPathLen)
            return false;

        const auto isSeparator = [](char c) noexcept { return c == '\\' || c == '/'; };
        for (const char* p = filename; *p; ++p)
        {
            if (p[0] == '.' && p[1] == '.' &&
                (p == filename || isSeparator(p[-1])) &&
                (p[2] == '\0' || isSeparator(p[2])))
                return false;
        }

#ifdef _WIN32
        const auto* colonPos = std::strchr(filename, ':');
        if (colonPos)
        {
            const bool isDriveLetter = (colonPos == filename + 1) &&
                                       std::isalpha(static_cast<unsigned char>(filename[0]));
            if (!isDriveLetter || std::strchr(colonPos + 1, ':'))
                return false;
        }

        if (isSeparator(filename[0]) && isSeparator(filename[1]) &&
            (filename[2] == '.' || filename[2] == '?') && isSeparator(filename[3]))
            return false;

        if (filename[0] == '\\' && filename[1] == '?' && filename[2] == '?' && filename[3] == '\\')
            return false;

        if (IsReservedName(filename))
            return false;
#endif
        return true;
    }
}}

#if defined(_WIN32) && defined(MTA_CLIENT)
FILE* SharedUtil::File::TryFopen(const char* filename, const char* mode)
{
    if (!filename || !mode || !*filename || !*mode)
        return errno = EINVAL, nullptr;

    if (!IsPathSafe(filename))
        return errno = EINVAL, nullptr;

    constexpr std::size_t maxModeLen = 3;
    if (std::strlen(mode) > maxModeLen)
        return errno = EINVAL, nullptr;

    const auto baseMode = mode[0];
    if (baseMode != 'r' && baseMode != 'w' && baseMode != 'a')
        return errno = EINVAL, nullptr;

    auto hasPlus = false;
    auto hasBT = false;
    auto isBinary = false;
    for (std::size_t i = 1; mode[i] != '\0'; ++i)
    {
        switch (mode[i])
        {
            case '+':
                if (hasPlus) return errno = EINVAL, nullptr;
                hasPlus = true;
                break;
            case 'b':
                if (hasBT) return errno = EINVAL, nullptr;
                hasBT = true;
                isBinary = true;
                break;
            case 't':
                if (hasBT) return errno = EINVAL, nullptr;
                hasBT = true;
                break;
            default:
                return errno = EINVAL, nullptr;
        }
    }

    char normalizedMode[4];
    std::size_t modePos = 0;
    normalizedMode[modePos++] = baseMode;
    if (hasPlus) normalizedMode[modePos++] = '+';
    if (isBinary) normalizedMode[modePos++] = 'b';
    normalizedMode[modePos] = '\0';

    DWORD accessFlags{}, creationMode{};
    int osFlags{};
    switch (baseMode)
    {
        case 'r':
            accessFlags = hasPlus ? (GENERIC_READ | GENERIC_WRITE) : GENERIC_READ;
            creationMode = OPEN_EXISTING;
            osFlags = hasPlus ? _O_RDWR : _O_RDONLY;
            break;
        case 'w':
            accessFlags = hasPlus ? (GENERIC_READ | GENERIC_WRITE) : GENERIC_WRITE;
            creationMode = CREATE_ALWAYS;
            osFlags = (hasPlus ? _O_RDWR : _O_WRONLY) | _O_CREAT | _O_TRUNC;
            break;
        case 'a':
            accessFlags = hasPlus ? (GENERIC_READ | GENERIC_WRITE) : GENERIC_WRITE;
            creationMode = OPEN_ALWAYS;
            osFlags = (hasPlus ? _O_RDWR : _O_WRONLY) | _O_CREAT | _O_APPEND;
            break;
    }
    osFlags |= isBinary ? _O_BINARY : _O_TEXT;

    WString wideFilename;
    try { wideFilename = FromUTF8(filename); }
    catch (...) { return errno = EINVAL, nullptr; }
    if (wideFilename.empty())
        return errno = EINVAL, nullptr;

    const auto fileHandle = CreateFileW(wideFilename.c_str(), accessFlags, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                        nullptr, creationMode, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (fileHandle == INVALID_HANDLE_VALUE)
    {
        const auto errorCode = GetLastError();
        switch (errorCode)
        {
            case ERROR_FILE_NOT_FOUND: case ERROR_PATH_NOT_FOUND: errno = ENOENT; break;
            case ERROR_ACCESS_DENIED: case ERROR_SHARING_VIOLATION: case ERROR_LOCK_VIOLATION: errno = EACCES; break;
            case ERROR_INVALID_NAME: case ERROR_BAD_PATHNAME: errno = EINVAL; break;
            case ERROR_FILE_EXISTS: case ERROR_ALREADY_EXISTS: errno = EEXIST; break;
            case ERROR_DISK_FULL: case ERROR_HANDLE_DISK_FULL: errno = ENOSPC; break;
            case ERROR_TOO_MANY_OPEN_FILES: errno = EMFILE; break;
            case ERROR_WRITE_PROTECT: errno = EROFS; break;
            case ERROR_NOT_READY: errno = ENXIO; break;
            case ERROR_INVALID_DRIVE: errno = ENODEV; break;
            default: errno = EIO; break;
        }
        return nullptr;
    }

    if (GetFileType(fileHandle) != FILE_TYPE_DISK)
    {
        CloseHandle(fileHandle);
        return errno = EACCES, nullptr;
    }

    {
        wchar_t finalPath[1024];
        DWORD pathLen = GetFinalPathNameByHandleW(fileHandle, finalPath, 1024, FILE_NAME_NORMALIZED);
        if (pathLen > 0 && pathLen < 1024 && pathLen >= 8 &&
            finalPath[0] == L'\\' && finalPath[1] == L'\\' &&
            finalPath[2] == L'?' && finalPath[3] == L'\\' &&
            (finalPath[4] == L'U' || finalPath[4] == L'u') &&
            (finalPath[5] == L'N' || finalPath[5] == L'n') &&
            (finalPath[6] == L'C' || finalPath[6] == L'c') &&
            finalPath[7] == L'\\')
        {
            CloseHandle(fileHandle);
            return errno = EACCES, nullptr;
        }
    }

    const auto fileDescriptor = _open_osfhandle(reinterpret_cast<intptr_t>(fileHandle), osFlags);
    if (fileDescriptor == -1)
    {
        const auto savedErrno = errno;
        CloseHandle(fileHandle);
        return errno = savedErrno, nullptr;
    }

    auto* fileStream = _fdopen(fileDescriptor, normalizedMode);
    if (!fileStream)
    {
        const auto savedErrno = errno;
        _close(fileDescriptor);
        return errno = savedErrno, nullptr;
    }
    return fileStream;
}

FILE* SharedUtil::File::TryFopenExclusive(const char* filename, const char* mode)
{
    if (!filename || !mode || !*filename || !*mode)
        return errno = EINVAL, nullptr;

    if (!IsPathSafe(filename))
        return errno = EINVAL, nullptr;

    if (mode[0] != 'r' || (mode[1] != 'b' && mode[1] != '\0') || (mode[1] == 'b' && mode[2] != '\0'))
        return errno = EINVAL, nullptr;

    WString wideFilename;
    try { wideFilename = FromUTF8(filename); }
    catch (...) { return errno = EINVAL, nullptr; }
    if (wideFilename.empty())
        return errno = EINVAL, nullptr;

    const auto fileHandle = CreateFileW(wideFilename.c_str(), GENERIC_READ, FILE_SHARE_READ,
                                        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (fileHandle == INVALID_HANDLE_VALUE)
    {
        const auto errorCode = GetLastError();
        switch (errorCode)
        {
            case ERROR_FILE_NOT_FOUND: case ERROR_PATH_NOT_FOUND: errno = ENOENT; break;
            case ERROR_ACCESS_DENIED: case ERROR_SHARING_VIOLATION: case ERROR_LOCK_VIOLATION: errno = EACCES; break;
            case ERROR_INVALID_NAME: case ERROR_BAD_PATHNAME: errno = EINVAL; break;
            case ERROR_TOO_MANY_OPEN_FILES: errno = EMFILE; break;
            default: errno = EIO; break;
        }
        return nullptr;
    }

    if (GetFileType(fileHandle) != FILE_TYPE_DISK)
    {
        CloseHandle(fileHandle);
        return errno = EACCES, nullptr;
    }

    {
        wchar_t finalPath[1024];
        DWORD pathLen = GetFinalPathNameByHandleW(fileHandle, finalPath, 1024, FILE_NAME_NORMALIZED);
        if (pathLen > 0 && pathLen < 1024 && pathLen >= 8 &&
            finalPath[0] == L'\\' && finalPath[1] == L'\\' &&
            finalPath[2] == L'?' && finalPath[3] == L'\\' &&
            (finalPath[4] == L'U' || finalPath[4] == L'u') &&
            (finalPath[5] == L'N' || finalPath[5] == L'n') &&
            (finalPath[6] == L'C' || finalPath[6] == L'c') &&
            finalPath[7] == L'\\')
        {
            CloseHandle(fileHandle);
            return errno = EACCES, nullptr;
        }
    }

    const int osFlags = (mode[1] == 'b') ? (_O_RDONLY | _O_BINARY) : _O_RDONLY;
    const auto fileDescriptor = _open_osfhandle(reinterpret_cast<intptr_t>(fileHandle), osFlags);
    if (fileDescriptor == -1)
    {
        const auto savedErrno = errno;
        CloseHandle(fileHandle);
        return errno = savedErrno, nullptr;
    }

    auto* fileStream = _fdopen(fileDescriptor, mode);
    if (!fileStream)
    {
        const auto savedErrno = errno;
        _close(fileDescriptor);
        return errno = savedErrno, nullptr;
    }
    return fileStream;
}

FILE* SharedUtil::File::Fopen(const char* szFilename, const char* szMode)
{
    return TryFopen(szFilename, szMode);
}

FILE* SharedUtil::File::FopenExclusive(const char* szFilename, const char* szMode)
{
    return TryFopenExclusive(szFilename, szMode);
}

#else
FILE* SharedUtil::File::Fopen(const char* szFilename, const char* szMode)
{
#ifdef _WIN32
    try
    {
        return _wfsopen(FromUTF8(szFilename), FromUTF8(szMode), _SH_DENYNO);
    }
    catch (...)
    {
        errno = EINVAL;
        return nullptr;
    }
#else
    return fopen(szFilename, szMode);
#endif
}

FILE* SharedUtil::File::FopenExclusive(const char* szFilename, const char* szMode)
{
#ifdef _WIN32
    try
    {
        return _wfsopen(FromUTF8(szFilename), FromUTF8(szMode), _SH_DENYWR);
    }
    catch (...)
    {
        errno = EINVAL;
        return nullptr;
    }
#else
    return fopen(szFilename, szMode);
#endif
}
#endif

int SharedUtil::File::Mkdir(const char* szPath, int iMode)
{
#ifdef _WIN32
    return _wmkdir(FromUTF8(szPath));
#else
    return mkdir(szPath, (mode_t)iMode);
#endif
}

int SharedUtil::File::Chdir(const char* szPath)
{
#ifdef _WIN32
    return _wchdir(FromUTF8(szPath));
#else
    return chdir(szPath);
#endif
}

int SharedUtil::File::Rmdir(const char* szPath)
{
#ifdef _WIN32
    return _wrmdir(FromUTF8(szPath));
#else
    return rmdir(szPath);
#endif
}

int SharedUtil::File::Delete(const char* szFilename)
{
#ifdef _WIN32
    return _wremove(FromUTF8(szFilename));
#else
    return remove(szFilename);
#endif
}

int SharedUtil::File::Rename(const char* szOldFilename, const char* szNewFilename)
{
#ifdef _WIN32
    return _wrename(FromUTF8(szOldFilename), FromUTF8(szNewFilename));
#else
    return rename(szOldFilename, szNewFilename);
#endif
}

std::vector<std::string> SharedUtil::ListDir(const char* szPath) noexcept
{
    std::vector<std::string> entries;
#if __cplusplus >= 201703L
    namespace fs = std::filesystem;
    if (!DirectoryExists(szPath))
        return {};

    try
    {
        for (const auto& entry : fs::directory_iterator(szPath))
        {
            if (entry.is_regular_file() || entry.is_directory())
                entries.push_back(entry.path().filename().string());
        }
    }
    catch (...) {} // catch all possible errors and ignore them
#else
    #ifdef _WIN32
    std::string search_path = szPath;
    if (search_path.empty())
        return {};

    char lastChar = search_path.back();
    if (lastChar == '/' || lastChar == '\\')
        search_path += '*';
    else
        search_path += "/*";

    try
    {
        WString wideSearchPath = FromUTF8(search_path);
        WIN32_FIND_DATAW fd;
        HANDLE hFind = ::FindFirstFileW(wideSearchPath.c_str(), &fd);

        if (hFind != INVALID_HANDLE_VALUE)
        {
            try
            {
                do
                {
                    entries.push_back(ToUTF8(fd.cFileName));
                } while (::FindNextFileW(hFind, &fd));
            }
            catch (...)
            {
                ::FindClose(hFind);
                throw;
            }
            ::FindClose(hFind);
        }
        return entries;
    }
    catch (...)
    {
        return {};
    }

    #else
    DIR*           dir;
    struct dirent* ent;
    if (!(dir = opendir(szPath)))
        return {};

    try
    {
        while ((ent = readdir(dir)))
        {
            entries.push_back(ent->d_name);
        }
    }
    catch (...)
    {
        closedir(dir);
        return {};
    }
    closedir(dir);
    #endif
#endif

    return entries;
}
