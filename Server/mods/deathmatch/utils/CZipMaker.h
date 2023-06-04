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
    struct ZipCloser
    {
        void operator()(zip_t*& f) const { zip_close(f); }
    };
    void OpenZIP(const SString& strZipPath, const char mode = 'r') noexcept;

public:
    struct CZipEntry
    {
        const char*        name{};
        bool               isDir{};
        unsigned long long size{};
        unsigned int       crc32{};
    };

    enum class Mode
    {
        READ,
        WRITE,
        APPEND
    };

    CZipMaker() noexcept;
    CZipMaker(const SString& strZipPathFilename, const char& mode = 'r') noexcept;
    CZipMaker(const SString& strZipPathFilename, const Mode& mode = Mode::READ) noexcept;
    ~CZipMaker() noexcept;

    CZipMaker& operator=(CZipMaker&&) = default;
    operator bool() const noexcept;

    bool IsValid() const noexcept;
    bool Close() noexcept;

    bool Insert(const SString& strSrc, const SString& strDest = "") noexcept;
    bool InsertFile(const SString& strSrc, const SString& strDest = "") noexcept;
    bool InsertDirectoryTree(const SString& strSrc, const SString& strDest = "") noexcept;

    bool RemoveFile(const SString& strPath) noexcept;
    bool Replace(const SString& strSrc, const SString& strDest) noexcept;
    bool Exists(const SString& strPath) const noexcept;
    bool ExtractFile(const SString& strPath, const SString& strDirPath) noexcept;

    std::vector<CZipEntry> ListEntries() const noexcept;

    CZipEntry operator[](unsigned long long offset) const noexcept;

protected:
    CZipEntry GetFileByIndex(unsigned long long offset) const noexcept;
    bool AddFile(const SString& strDest, const SString& buffer) noexcept;

    std::unique_ptr<zip_t, ZipCloser> m_uzFile;
    SString                           m_strZipPath;
};
