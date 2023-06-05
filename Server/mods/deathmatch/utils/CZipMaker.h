/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CZipMaker.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <zip/zip.h>
#include <memory>

class CZipMaker
{
    void OpenZIP(const SString& strZipPath, const char mode = 'r') noexcept;

public:
    struct CZipEntry
    {
        SString            name{};
        bool               isDir{};
        unsigned long long sizeUncompressed{};
        unsigned long long sizeCompressed{};
        unsigned int       crc32{};
        ssize_t            index{};

        bool operator==(const CZipEntry& entry) const noexcept
        {
            if (name != entry.name)
                return false;
            if (isDir != entry.isDir)
                return false;
            if (sizeUncompressed != entry.sizeUncompressed)
                return false;
            if (sizeCompressed != entry.sizeCompressed)
                return false;
            if (crc32 != entry.crc32)
                return false;
            if (index != entry.index)
                return false;

            return true;
        }
        bool operator!=(const CZipEntry& entry) const noexcept
        {
            return !operator==(entry);
        }
    };

    enum class Mode
    {
        READ,
        WRITE,
        APPEND
    };

    CZipMaker() noexcept;
    CZipMaker(const SString& strZipPathFilename, const char& mode = 'r') noexcept;
    CZipMaker(const SString& strZipPathFilename, const Mode& mode) noexcept;
    ~CZipMaker() noexcept;

    CZipMaker& operator=(CZipMaker&&) = default;
    operator bool() const noexcept;

    bool IsValid() const noexcept;
    bool Close() noexcept;

    bool Insert(const SString& strSrc, const SString& strDest = "") noexcept;
    bool InsertFile(const SString& strSrc, const SString& strDest = "") noexcept;
    bool InsertDirectoryTree(const SString& strSrc, const SString& strDest = "") noexcept;

    bool RemoveFile(const SString& strPath) noexcept;
    bool RemoveFile(const CZipEntry& entry) noexcept;
    bool Replace(const SString& strSrc, const SString& strDest) noexcept;
    bool Exists(const SString& strPath) const noexcept;
    bool ExtractFile(const SString& strFilePath, const SString& strDirPath) noexcept;
    bool Extract(const SString& strDirPath) noexcept;

    std::vector<CZipEntry> ListEntries() const noexcept;

    CZipEntry operator[](unsigned long long offset) const noexcept;

protected:
    CZipEntry GetFileByIndex(unsigned long long offset) const noexcept;
    bool AddFile(const SString& strDest, const SString& buffer) noexcept;

    zip_t*                 m_uzFile;
    SString                m_strZipPath;
    std::vector<CZipEntry> m_vecEntries;
};
