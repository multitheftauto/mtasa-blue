/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.File.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <vector>
#include "SharedUtil.IntTypes.h"
#include "SString.h"
#include "WString.h"

namespace SharedUtil
{
    //
    // Returns true if the file/directory exists
    //
    bool FileExists(const std::string& strFilename) noexcept;
    bool DirectoryExists(const std::string& strPath) noexcept;

    //
    // Load from a file
    //
    bool FileLoad(const SString& strFilename, std::vector<char>& buffer, int iMaxSize = INT_MAX, int iOffset = 0);
    bool FileLoad(const SString& strFilename, SString& strBuffer, int iMaxSize = INT_MAX, int iOffset = 0);
    bool FileLoad(std::nothrow_t, const SString& filePath, SString& outBuffer, size_t maxSize = INT_MAX, size_t offset = 0) noexcept;

    //
    // Save to a file
    //
    bool FileSave(const SString& strFilename, const void* pBuffer, unsigned long ulSize, bool bForce = true);
    bool FileSave(const SString& strFilename, const SString& strBuffer, bool bForce = true);

    //
    // Append to a file
    //
    bool FileAppend(const SString& strFilename, const void* pBuffer, unsigned long ulSize, bool bForce = true);
    bool FileAppend(const SString& strFilename, const SString& strBuffer, bool bForce = true);

    //
    // Get a file size
    //
    uint64 FileSize(const SString& strFilename);

    //
    // Ensure all directories exist to the file
    //
    void MakeSureDirExists(const SString& strPath);

    SString PathConform(const SString& strInPath);
    SString PathJoin(const SString& str1, const SString& str2);
    SString PathJoin(const SString& str1, const SString& str2, const SString& str3, const SString& str4 = "", const SString& str5 = "");
    SString PathMakeRelative(const SString& strInBasePath, const SString& strInAbsPath);
    void    ExtractFilename(const SString& strPathFilename, SString* strPath, SString* strFilename);
    bool    ExtractExtension(const SString& strFilename, SString* strRest, SString* strExt);
    SString ExtractPath(const SString& strPathFilename);
    SString ExtractFilename(const SString& strPathFilename);
    SString ExtractExtension(const SString& strPathFilename);
    SString ExtractBeforeExtension(const SString& strPathFilename);

    bool                 FileDelete(const SString& strFilename, bool bForce = true);
    bool                 FileRename(const SString& strFilenameOld, const SString& strFilenameNew, int* pOutErrorCode = nullptr);
    bool                 DelTree(const SString& strPath, const SString& strInsideHere);
    bool                 MkDir(const SString& strInPath, bool bTree = true);
    bool                 FileCopy(const SString& strSrc, const SString& strDest, bool bForce = true);
    std::vector<SString> FindFiles(const SString& strMatch, bool bFiles, bool bDirectories, bool bSortByDate = false);
    SString              MakeUniquePath(const SString& strPathFilename);
    SString              MakeGenericPath(const SString& uniqueFilePath);
    SString              ConformPathForSorting(const SString& strPathFilename);
    bool                 IsAbsolutePath(const SString& strPath);

    SString GetSystemCurrentDirectory();
    SString GetSystemDllDirectory();
    SString GetSystemLocalAppDataPath();
    SString GetSystemCommonAppDataPath();
    SString GetSystemPersonalPath();
    SString GetSystemWindowsPath();
    SString GetSystemSystemPath();
    SString GetSystemTempPath();
    SString GetMTADataPath();
    SString GetMTADataPathCommon();
    SString GetMTATempPath();

    bool    IsShortPathName(const char* szPath);
    bool    IsShortPathName(const wchar_t* szPath);
    SString GetSystemShortPathName(const SString& strPath);
    SString GetSystemLongPathName(const SString& strPath);

    SString GetLaunchPathFilename();
    SString GetLaunchPath();
    SString GetLaunchFilename();

    SString GetPathDriveName(const SString& strPath);
    uint    GetPathFreeSpaceMB(const SString& strPath);
    SString GetDriveNameWithNotEnoughSpace(uint uiResourcesPathMinMB = 10, uint uiDataPathMinMB = 10);

    WString FromUTF8(const SString& strPath);
    SString ToUTF8(const WString& strPath);

    std::vector<std::string> ListDir(const char* szPath) noexcept;

    namespace File
    {
        FILE* Fopen(const char* szFilename, const char* szMode);
        int   Mkdir(const char* szPath, int iMode = 0775);
        int   Chdir(const char* szPath);
        int   Rmdir(const char* szPath);
        int   Delete(const char* szFilename);
        int   Rename(const char* szOldFilename, const char* szNewFilename);
    }            // namespace File
}            // namespace SharedUtil
