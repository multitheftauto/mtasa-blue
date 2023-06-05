/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CZipMaker.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CZipMaker.h"
#include <zip.h>


CZipMaker::CZipMaker() noexcept : m_uzFile(NULL) {}

///////////////////////////////////////////////////////////////
//
// CZipMaker::CZipMaker
//
//
//
///////////////////////////////////////////////////////////////

CZipMaker::CZipMaker(const SString& strZipPathFilename, const Mode& mode) noexcept
    : m_strZipPath(strZipPathFilename)
{
    char cMode;
    switch (mode)
    {
        case Mode::WRITE:
            cMode = 'w';
            break;
        case Mode::APPEND:
            cMode = 'a';
            break;
        case Mode::READ:
        default:
            cMode = 'r';
            break;
    }
    OpenZIP(strZipPathFilename, cMode);
}

CZipMaker::CZipMaker(const SString& strZipPathFilename, const char& mode) noexcept
    : m_strZipPath(strZipPathFilename)
{
    OpenZIP(strZipPathFilename, mode);
}

///////////////////////////////////////////////////////////////
//
// CZipMaker::~CZipMaker
//
//
//
///////////////////////////////////////////////////////////////
CZipMaker::~CZipMaker() noexcept
{
    Close();
}

void CZipMaker::OpenZIP(const SString& strZipPath, const char mode) noexcept
{
    if (strZipPath.empty())
        return;

    m_uzFile = zip_open(strZipPath, ZIP_DEFAULT_COMPRESSION_LEVEL, mode);

    for (auto i = 0; i < zip_entries_total(m_uzFile); i++)
    {
        m_vecEntries.push_back(GetFileByIndex(i));
    }
}

///////////////////////////////////////////////////////////////
//
// CZipMaker::IsValid
//
// Returns false if not valid
//
///////////////////////////////////////////////////////////////
bool CZipMaker::IsValid() const noexcept
{
    return m_uzFile != NULL;
}

///////////////////////////////////////////////////////////////
//
// CZipMaker::Close
//
// Returns false if failed
//
///////////////////////////////////////////////////////////////
bool CZipMaker::Close() noexcept
{
    if (!IsValid())
        return false;

    zip_close(m_uzFile);
    m_uzFile = NULL;
    return true;
}

bool CZipMaker::Insert(const SString& strSrc, const SString& strDest) noexcept
{
    SString strSrcPath = PathConform(strSrc);
    SString strDestPath = PathConform(strDest);
    if (DirectoryExists(strSrcPath))
        return InsertDirectoryTree(strSrcPath, strDestPath);
    else
        return InsertFile(strSrcPath, strDestPath);
}

///////////////////////////////////////////////////////////////
//
// CZipMaker::InsertFile
//
// Returns false if failed
//
///////////////////////////////////////////////////////////////
bool CZipMaker::InsertFile(const SString& strInSrc, const SString& strInDest) noexcept
{
    SString strSrc = PathConform(strInSrc);
    SString strDest = PathConform(strInDest);

    std::vector<char> buffer;
    if (!FileLoad(strSrc, buffer))
        return false;

    return AddFile(strDest, buffer.data());
}

///////////////////////////////////////////////////////////////
//
// CZipMaker::InsertDirectoryTree
//
// strSrc is the full path for the source directory
// strDest is the destination directory, relative to the zip file root
//
///////////////////////////////////////////////////////////////
bool CZipMaker::InsertDirectoryTree(const SString& strInSrc, const SString& strInDest) noexcept
{
    SString strSrc = PathConform(strInSrc);
    SString strDest = PathConform(strInDest);

    std::vector<SString> fileList = FindFiles(PathJoin(strSrc, ""), true, true);
    for (unsigned int i = 0; i < fileList.size(); i++)
    {
        SString strSrcNext = PathConform(PathJoin(strSrc, fileList[i]));
        SString strDestNext = PathConform(PathJoin(strDest, fileList[i]));

        if (FileExists(strSrcNext))
        {
            InsertFile(strSrcNext, strDestNext);
        }
        else if (DirectoryExists(strSrcNext))
        {
            InsertDirectoryTree(strSrcNext, strDestNext);
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////
//
// CZipMaker::AddFile
//
//
//
///////////////////////////////////////////////////////////////
bool CZipMaker::AddFile(const SString& strDest, const SString& buffer) noexcept
{
    if (!m_uzFile || buffer.empty())
        return false;

    zip_entry_open(m_uzFile, strDest);
    zip_entry_write(m_uzFile, buffer.data(), buffer.size());

    m_vecEntries.push_back(GetFileByIndex(zip_entry_index(m_uzFile)));

    return zip_entry_close(m_uzFile) == 0;
}

bool CZipMaker::RemoveFile(const SString& strPath) noexcept
{
    char* entry[] = {const_cast<char*>(strPath.c_str())};

    zip_entry_open(m_uzFile, strPath);

    auto fileEntry = GetFileByIndex(zip_entry_index(m_uzFile));
    const auto entryIter = std::remove(m_vecEntries.begin(), m_vecEntries.end(), fileEntry);

    m_vecEntries.erase(entryIter, m_vecEntries.end());

    zip_entry_close(m_uzFile);

    return zip_entries_delete(m_uzFile, entry, 1) == 1;
}

bool CZipMaker::RemoveFile(const CZipMaker::CZipEntry& entry) noexcept
{
    return RemoveFile(entry.name);
}

bool CZipMaker::Replace(const SString& strSrc, const SString& strDest) noexcept
{
    if (!RemoveFile(strSrc))
        return false;
    if (!InsertFile(strDest))
        return false;

    return true;
}
bool CZipMaker::Exists(const SString& strPath) const noexcept
{
    bool bStatus = zip_entry_open(m_uzFile, strPath) == 0;
    return bStatus && zip_entry_close(m_uzFile) == 0;
}
bool CZipMaker::ExtractFile(const SString& strFilePath, const SString& strDirPath) noexcept
{
    if (!m_uzFile)
        return false;

    if (zip_entry_open(m_uzFile, strFilePath))
        return false;

    auto destPath = PathJoin(strDirPath, strFilePath);
    MakeSureDirExists(destPath);
    if (zip_entry_fread(m_uzFile, destPath))
    {
        zip_entry_close(m_uzFile);
        return false;
    }

    return zip_entry_close(m_uzFile) == 0;
}
bool CZipMaker::Extract(const SString& strDirPath) noexcept
{
    if (!m_uzFile)
        return false;

    return zip_extract(m_strZipPath, strDirPath, NULL, NULL) == 0;
}

std::vector<CZipMaker::CZipEntry> CZipMaker::ListEntries() const noexcept
{
    return m_vecEntries;
}

CZipMaker::CZipEntry CZipMaker::operator[](unsigned long long offset) const noexcept
{
    return GetFileByIndex(offset);
}
CZipMaker::operator bool() const noexcept
{
    return IsValid();
}

CZipMaker::CZipEntry CZipMaker::GetFileByIndex(unsigned long long offset) const noexcept
{
    CZipEntry entry;
    if (!m_uzFile)
        return entry;

    const auto size = zip_entries_total(m_uzFile);
    if (offset > size)
        return entry;

    if (zip_entry_openbyindex(m_uzFile, offset))
        return entry;

    entry.name = zip_entry_name(m_uzFile);
    entry.isDir = zip_entry_isdir(m_uzFile);
    entry.sizeUncompressed = zip_entry_size(m_uzFile);
    entry.crc32 = zip_entry_crc32(m_uzFile);
    entry.sizeCompressed = zip_entry_comp_size(m_uzFile);
    entry.index = zip_entry_index(m_uzFile);

    zip_entry_close(m_uzFile);
    return entry;
}
