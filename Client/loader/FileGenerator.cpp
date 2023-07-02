/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/FileGenerator.cpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "FileGenerator.h"
#include "FileSystem.h"
#include "Utils.h"
#include <unrar/dll.hpp>

namespace fs = std::filesystem;

static constexpr FileHash HASH_PATCH_BASE{0x36, 0xbc, 0x8f, 0x48, 0x14, 0xd4, 0xef, 0x4d, 0xc3, 0xb7, 0xc4, 0x3a, 0xe5, 0xc2, 0x6f, 0x0d,
                                          0xe1, 0xfe, 0xed, 0xa1, 0xbc, 0x15, 0xcd, 0xf2, 0x25, 0x04, 0x56, 0xd2, 0x92, 0x7b, 0xc5, 0x7c};

static constexpr size_t SIZE_PATCH_BASE{0x000001d4};

static constexpr FileHash HASH_PATCH_DIFF{0xb8, 0x34, 0xcd, 0x0e, 0xd7, 0x72, 0x88, 0x3f, 0xf8, 0x72, 0x4a, 0x3f, 0x4e, 0x5d, 0xd4, 0xa9,
                                          0x5d, 0x5d, 0x5d, 0x77, 0x36, 0x63, 0xd0, 0x2c, 0xc7, 0x86, 0x7b, 0x5c, 0x40, 0xab, 0x89, 0x60};

static constexpr size_t SIZE_PATCH_DIFF{0x004b87ef};

static constexpr FileHash HASH_PATCHER_DATA{0xe2, 0x4d, 0x05, 0x43, 0x35, 0xef, 0x80, 0xd2, 0x1a, 0xbf, 0x42, 0x73, 0x5e, 0x80, 0x24, 0xbc,
                                            0x7b, 0xc0, 0x50, 0x96, 0x37, 0x2d, 0x16, 0x93, 0x9d, 0x1d, 0x18, 0xa3, 0x33, 0xed, 0x37, 0x3c};

static bool Extract(const char* fileName, const fs::path& destination, const std::vector<unsigned char>& buffer, std::error_code& ec)
{
    // Write the buffer to a temporary file.
    const fs::path archivePath = GetTempFilePath(10, ec);

    if (archivePath.empty() || ec)
        return false;

    if (!SetFileContent(archivePath, buffer, ec))
        return false;

    // Open the temporary file as an archive.
    const std::wstring archiveStringPath = archivePath.wstring();

    RAROpenArchiveDataEx archiveData{};
    archiveData.ArcNameW = const_cast<wchar_t*>(archiveStringPath.data());
    archiveData.OpenMode = RAR_OM_EXTRACT;

    HANDLE archiveHandle = RAROpenArchiveEx(&archiveData);

    if (!archiveHandle)
    {
        ec.assign(ERROR_FILE_CORRUPT, std::system_category());
        return false;
    }

    bool result = false;

    while (true)
    {
        RARHeaderDataEx header{};

        if (RARReadHeaderEx(archiveHandle, &header) != ERAR_SUCCESS)
            break;

        // Extract a compressed file if the code name matches.
        if (!stricmp(fileName, header.FileName))
        {
            const std::wstring destinationString = destination.wstring();
            result = (ERAR_SUCCESS == RARProcessFileW(archiveHandle, RAR_EXTRACT, nullptr, const_cast<wchar_t*>(destinationString.data())));

            if (!result)
                ec.assign(ERROR_WRITE_FAULT, std::system_category());

            break;
        }

        if (RARProcessFileW(archiveHandle, RAR_SKIP, nullptr, nullptr) != ERAR_SUCCESS)
            break;
    }

    RARCloseArchive(archiveHandle);

    std::error_code ignore;
    fs::remove(archivePath, ignore);

    if (result)
        return true;

    if (!ec)
        ec.assign(ERROR_NOT_FOUND, std::system_category());

    return false;
}

FileGenerator::FileGenerator() : m_patchBasePath(GetPatchBasePath()), m_patchDiffPath(GetPatchDiffPath())
{
}

bool FileGenerator::GenerateFile(const std::string& name, const fs::path& targetPath, std::error_code& ec)
{
    ec.clear();

    if (!LoadPatcherData(ec))
        return false;

    return Extract(name.c_str(), targetPath, m_data, ec);
}

bool FileGenerator::LoadPatcherData(std::error_code& ec)
{
    ec.clear();

    // Do nothing if we already loaded the patcher data once.
    if (!m_data.empty())
        return true;

    // Load the patch base and verify its length.
    std::vector<unsigned char> base{};

    if (!GetFileContent(m_patchBasePath, base, ec))
        return false;

    if (base.size() != SIZE_PATCH_BASE)
    {
        ec.assign(ERROR_BAD_LENGTH, std::system_category());
        return false;
    }

    // Load the patch diff and verify its length.
    std::vector<unsigned char> diff{};

    if (!GetFileContent(m_patchDiffPath, diff, ec))
        return false;

    if (diff.size() != SIZE_PATCH_DIFF)
    {
        ec.assign(ERROR_BAD_LENGTH, std::system_category());
        return false;
    }

    // Reuncompression using future delta system.
    size_t index = 0;

    for (unsigned char& c : diff)
    {
        c ^= base[index];
        index = (index + 1) % base.size();
    }

    // Check if the patcher data has the correct checksum.
    if (GetFileBufferHash(diff) != HASH_PATCHER_DATA)
    {
        ec.assign(ERROR_DATA_CHECKSUM_ERROR, std::system_category());
        return false;
    }

    m_data = std::move(diff);
    return true;
}

auto FileGenerator::GetPatchBasePath() -> fs::path
{
    static const auto file = GetGameBaseDirectory() / L"audio" / L"CONFIG" / L"PakFiles.dat";
    return file;
}

auto FileGenerator::GetPatchDiffPath() -> fs::path
{
    static const auto file = GetMTARootDirectory() / L"MTA" / L"data" / L"gta_sa_diff.dat";
    return file;
}

bool FileGenerator::IsPatchBase(const fs::path& filePath)
{
    std::error_code ec{};
    FileHash        hash{};

    if (GetFileHash(filePath, hash, ec) && hash == HASH_PATCH_BASE)
        return true;

    if (IsErrorCodeLoggable(ec))
    {
        const uintmax_t fileSize = GetFileSize(filePath);
        AddReportLog(5053, SString("IsPatchBase: Incorrect file '%ls' (err: %d, size: %ju, hash: %s)", filePath.wstring().c_str(), ec.value(), fileSize,
                                   GetFileHashString(hash).c_str()));
    }

    return false;
}

bool FileGenerator::IsPatchDiff(const fs::path& filePath)
{
    std::error_code ec{};
    FileHash        hash{};

    if (GetFileHash(filePath, hash, ec) && hash == HASH_PATCH_DIFF)
        return true;

    if (IsErrorCodeLoggable(ec))
    {
        const uintmax_t fileSize = GetFileSize(filePath);
        AddReportLog(5053, SString("IsPatchDiff: Incorrect file '%ls' (err: %d, size: %ju, hash: %s)", filePath.wstring().c_str(), ec.value(), fileSize,
                                   GetFileHashString(hash).c_str()));
    }

    return false;
}
