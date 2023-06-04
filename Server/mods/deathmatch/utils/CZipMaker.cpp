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


CZipMaker::CZipMaker() noexcept {}

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
    this->m_uzFile = std::unique_ptr<zip_t, ZipCloser>(
        zip_open(strZipPath, ZIP_DEFAULT_COMPRESSION_LEVEL, mode)
    );
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
    return m_uzFile.get() != nullptr;
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
    if (!m_uzFile)
        return false;

    zip_close(m_uzFile.release());
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

    zip_entry_open(m_uzFile.get(), strDest);
    zip_entry_write(m_uzFile.get(), buffer.data(), buffer.size());

    return zip_entry_close(this->m_uzFile.get()) == 0;
}

bool CZipMaker::RemoveFile(const SString& strPath) noexcept
{
    char* entry[] = { const_cast<char*>(strPath.c_str()) };
    return zip_entries_delete(m_uzFile.get(), entry, 1) == 1;
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
    bool bStatus = zip_entry_open(m_uzFile.get(), strPath) == 0;
    zip_entry_close(m_uzFile.get());
    return bStatus;
}
bool CZipMaker::ExtractFile(const SString& strPath, const SString& strDirPath) noexcept
{
    if (zip_entry_open(m_uzFile.get(), strPath))
        return false;

    auto destPath = PathJoin(strDirPath, strPath);
    MakeSureDirExists(destPath);
    if (zip_entry_fread(m_uzFile.get(), destPath))
    {
        zip_entry_close(m_uzFile.get());
        return false;
    }

    return zip_entry_close(m_uzFile.get()) == 0;
}

std::vector<CZipMaker::CZipEntry> CZipMaker::ListEntries() const noexcept
{
    std::vector<CZipEntry> entries;
    ssize_t size = zip_entries_total(m_uzFile.get());
    for (int i = 0; i < size; i++)
    {
        auto entry = GetFileByIndex(i);
        if (entry.isDir)
            continue;
        entries.push_back(entry);
    }

    return entries;
}

CZipMaker::CZipEntry CZipMaker::operator[](unsigned long long offset) const noexcept
{
    return GetFileByIndex(offset);
}
CZipMaker::operator bool() const noexcept
{
    return m_uzFile.operator bool();
}

CZipMaker::CZipEntry CZipMaker::GetFileByIndex(unsigned long long offset) const noexcept
{
    CZipEntry entry;
    if (!m_uzFile)
        return entry;

    const auto size = zip_entries_total(m_uzFile.get());
    if (offset > size)
        return entry;

    if (offset < 0)
        offset += size;

    if (zip_entry_openbyindex(m_uzFile.get(), offset))
        return entry;

    entry.name = zip_entry_name(m_uzFile.get());
    entry.isDir = zip_entry_isdir(m_uzFile.get());
    entry.size = zip_entry_size(m_uzFile.get());
    entry.crc32 = zip_entry_crc32(m_uzFile.get());    

    zip_entry_close(m_uzFile.get());
    return entry;
}
