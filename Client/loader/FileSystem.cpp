/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/FileSystem.cpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "FileSystem.h"
#include "Utils.h"
#include <limits>
#include <random>
#include <sha2.h>
#include <Windows.h>

static_assert(sizeof(FileHash) == SHA256_DIGEST_SIZE);

namespace fs = std::filesystem;

struct HandleScopeDeleter
{
    void operator()(HANDLE object) const noexcept { CloseHandle(object); }
};

using HandleScope = std::unique_ptr<std::remove_pointer_t<HANDLE>, HandleScopeDeleter>;

void ApplyLastSystemError(std::error_code& ec)
{
    ec.assign(static_cast<int>(GetLastError()), std::system_category());
}

auto GetFileSize(const fs::path& path) -> uintmax_t
{
    std::error_code ec{};
    uintmax_t       size = fs::file_size(path, ec);
    return ec ? 0 : size;
}

auto GetTempFilePath(size_t attempts, std::error_code& ec) -> fs::path
{
    ec.clear();

    if (!attempts)
        return {};

    const fs::path tempDirectory = fs::temp_directory_path(ec);

    if (ec)
        return {};

    for (size_t i = 0; i < attempts; i++)
    {
        fs::path path = tempDirectory / GenerateRandomString(32);

        if (!fs::exists(path, ec) && !ec)
            return path;
    }

    ec.assign(ERROR_FILE_SYSTEM_LIMITATION, std::system_category());
    return {};
}

bool GetFileContent(const fs::path& path, std::vector<unsigned char>& buffer, std::error_code& ec)
{
    buffer.clear();
    ec.clear();

    HandleScope fileHandle{CreateFileW(
        /* FileName            */ path.wstring().c_str(),
        /* DesiredAccess       */ GENERIC_READ,
        /* ShareMode           */ 0,
        /* SecurityAttributes  */ nullptr,
        /* CreationDisposition */ OPEN_EXISTING,
        /* FlagsAndAttributes  */ FILE_ATTRIBUTE_NORMAL,
        /* TemplateFile        */ nullptr)};

    if (fileHandle.get() == INVALID_HANDLE_VALUE)
    {
        (void)fileHandle.release();
        ApplyLastSystemError(ec);
        return false;
    }

    LARGE_INTEGER fileSize{};

    if (!GetFileSizeEx(fileHandle.get(), &fileSize))
    {
        ApplyLastSystemError(ec);
        return false;
    }

    if (fileSize.QuadPart > std::numeric_limits<size_t>::max())
    {
        ec.assign(ERROR_FILE_TOO_LARGE, std::system_category());
        return false;
    }

    std::vector<unsigned char> fileBuffer;

    try
    {
        fileBuffer.resize(static_cast<size_t>(fileSize.QuadPart));
    }
    catch (std::bad_alloc&)
    {
        ec.assign(ERROR_OUTOFMEMORY, std::system_category());
        return false;
    }

#ifdef _WIN64
    {
        constexpr size_t maxReadSize = std::numeric_limits<DWORD>::max();
        size_t           remainingBytes = fileBuffer.size();
        unsigned char*   bufferData = fileBuffer.data();

        while (remainingBytes)
        {
            DWORD bytesToRead = (remainingBytes > maxReadSize) ? maxReadSize : static_cast<DWORD>(remainingBytes);
            DWORD bytesRead{};

            if (!ReadFile(fileHandle.get(), bufferData, bytesToRead, &bytesRead, nullptr))
            {
                ApplyLastSystemError(ec);
                return false;
            }

            if (bytesRead != bytesToRead)
            {
                ec.assign(ERROR_READ_FAULT, std::system_category());
                return false;
            }

            bufferData += bytesToRead;
            remainingBytes -= bytesToRead;
        }
    }
#else
    {
        DWORD bytesRead{};

        if (!ReadFile(fileHandle.get(), fileBuffer.data(), static_cast<DWORD>(fileBuffer.size()), &bytesRead, nullptr))
        {
            ApplyLastSystemError(ec);
            return false;
        }

        if (bytesRead != static_cast<DWORD>(fileBuffer.size()))
        {
            ec.assign(ERROR_READ_FAULT, std::system_category());
            return false;
        }
    }
#endif

    buffer = std::move(fileBuffer);
    return true;
}

bool SetFileContent(const fs::path& path, const std::vector<unsigned char>& buffer, std::error_code& ec)
{
    ec.clear();

    if (fs::exists(path))
    {
        // Make sure we can write data to the file if it already exists.
        fs::permissions(path, fs::perms::owner_write, fs::perm_options::add, ec);

        if (ec)
            return false;
    }
    else
    {
        const fs::path parentPath = path.parent_path();

        if (!fs::exists(parentPath, ec))
        {
            if (ec || !fs::create_directories(parentPath, ec))
                return false;
        }
    }

    HandleScope fileHandle{CreateFileW(
        /* FileName            */ path.wstring().c_str(),
        /* DesiredAccess       */ GENERIC_WRITE,
        /* ShareMode           */ 0,
        /* SecurityAttributes  */ nullptr,
        /* CreationDisposition */ CREATE_ALWAYS,
        /* FlagsAndAttributes  */ FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
        /* TemplateFile        */ nullptr)};

    if (fileHandle.get() == INVALID_HANDLE_VALUE)
    {
        (void)fileHandle.release();
        ApplyLastSystemError(ec);
        return false;
    }

    if (buffer.empty())
        return true;

    LARGE_INTEGER filePosition{};
    filePosition.QuadPart = static_cast<LONGLONG>(buffer.size());

    if (!SetFilePointerEx(fileHandle.get(), filePosition, nullptr, FILE_BEGIN) || !SetEndOfFile(fileHandle.get()))
    {
        ApplyLastSystemError(ec);
        return false;
    }

    filePosition.QuadPart = 0;

    if (!SetFilePointerEx(fileHandle.get(), filePosition, nullptr, FILE_BEGIN))
    {
        ApplyLastSystemError(ec);
        return false;
    }

#ifdef _WIN64
    {
        constexpr size_t     maxWriteSize = std::numeric_limits<DWORD>::max();
        size_t               remainingBytes = buffer.size();
        const unsigned char* bufferData = buffer.data();

        while (remainingBytes)
        {
            DWORD bytesToWrite = (remainingBytes > maxWriteSize) ? maxWriteSize : static_cast<DWORD>(remainingBytes);
            DWORD bytesWritten{};

            if (!WriteFile(fileHandle.get(), bufferData, bytesToWrite, &bytesWritten, nullptr))
            {
                ApplyLastSystemError(ec);
                return false;
            }

            if (bytesWritten != bytesToWrite)
            {
                ec.assign(ERROR_WRITE_FAULT, std::system_category());
                return false;
            }

            bufferData += bytesToWrite;
            remainingBytes -= bytesToWrite;
        }
    }
#else
    {
        DWORD bytesWritten{};

        if (!WriteFile(fileHandle.get(), buffer.data(), static_cast<DWORD>(buffer.size()), &bytesWritten, nullptr))
        {
            ApplyLastSystemError(ec);
            return false;
        }

        if (bytesWritten != static_cast<DWORD>(buffer.size()))
        {
            ec.assign(ERROR_WRITE_FAULT, std::system_category());
            return false;
        }
    }
#endif

    return true;
}

bool SafeCopyFile(const fs::path& from, const fs::path& to, std::error_code& ec)
{
    ec.clear();

    // Check if the source file is a regular file we can access and copy.
    if (!fs::is_regular_file(from, ec))
        return false;

    if (fs::exists(to, ec))
    {
        if (AreFilesEqual(from, to, ec))
            return true;

        if (ec)
            return false;

        // Make sure we can write data to the file if it already exists.
        fs::permissions(to, fs::perms::owner_write, fs::perm_options::add, ec);
    }
    else
    {
        if (ec)
            return false;

        // Create the parent directory for the file.
        const fs::path toParentDirectory = to.parent_path();

        if (!fs::exists(toParentDirectory, ec))
        {
            if (ec || !fs::create_directories(toParentDirectory, ec))
                return false;
        }

        if (!fs::is_directory(toParentDirectory, ec))
            return false;
    }

    if (ec)
        return false;

    // Copy the source file to the target location.
    if (!fs::copy_file(from, to, fs::copy_options::overwrite_existing, ec))
        return false;

    return AreFilesEqual(from, to, ec);
}

bool AreFilesEqual(const fs::path& base, const fs::path& other, std::error_code& ec)
{
    ec.clear();

    if (fs::equivalent(base, other, ec))
        return true;

    if (ec)
        return false;

    HandleScope baseHandle{CreateFileW(
        /* FileName            */ base.wstring().c_str(),
        /* DesiredAccess       */ GENERIC_READ,
        /* ShareMode           */ 0,
        /* SecurityAttributes  */ nullptr,
        /* CreationDisposition */ OPEN_EXISTING,
        /* FlagsAndAttributes  */ FILE_ATTRIBUTE_NORMAL,
        /* TemplateFile        */ nullptr)};

    if (baseHandle.get() == INVALID_HANDLE_VALUE)
    {
        (void)baseHandle.release();
        ApplyLastSystemError(ec);
        return false;
    }

    HandleScope otherHandle{CreateFileW(
        /* FileName            */ other.wstring().c_str(),
        /* DesiredAccess       */ GENERIC_READ,
        /* ShareMode           */ 0,
        /* SecurityAttributes  */ nullptr,
        /* CreationDisposition */ OPEN_EXISTING,
        /* FlagsAndAttributes  */ FILE_ATTRIBUTE_NORMAL,
        /* TemplateFile        */ nullptr)};

    if (otherHandle.get() == INVALID_HANDLE_VALUE)
    {
        (void)otherHandle.release();
        ApplyLastSystemError(ec);
        return false;
    }

    LARGE_INTEGER baseSize{};
    LARGE_INTEGER otherSize{};

    if (!GetFileSizeEx(baseHandle.get(), &baseSize) || !GetFileSizeEx(otherHandle.get(), &otherSize))
    {
        ApplyLastSystemError(ec);
        return false;
    }

    if (baseSize.QuadPart != otherSize.QuadPart)
        return false;

    std::array<unsigned char, 4096> baseBuffer{};
    std::array<unsigned char, 4096> otherBuffer{};

    const auto bytesToRead = static_cast<DWORD>(baseBuffer.size());
    LONGLONG   remainingSize = baseSize.QuadPart;

    while (remainingSize)
    {
        DWORD baseBytesRead{};

        if (!ReadFile(baseHandle.get(), baseBuffer.data(), bytesToRead, &baseBytesRead, nullptr))
        {
            ApplyLastSystemError(ec);
            return false;
        }

        DWORD otherBytesRead{};

        if (!ReadFile(otherHandle.get(), otherBuffer.data(), bytesToRead, &otherBytesRead, nullptr))
        {
            ApplyLastSystemError(ec);
            return false;
        }

        if (baseBytesRead != otherBytesRead || baseBuffer != otherBuffer)
            return false;

        remainingSize -= baseBytesRead;
    }

    return true;
}

bool GetFileHash(const fs::path& path, FileHash& hash, std::error_code& ec)
{
    hash = {};
    ec.clear();

    HandleScope fileHandle{CreateFileW(
        /* FileName            */ path.wstring().c_str(),
        /* DesiredAccess       */ GENERIC_READ,
        /* ShareMode           */ 0,
        /* SecurityAttributes  */ nullptr,
        /* CreationDisposition */ OPEN_EXISTING,
        /* FlagsAndAttributes  */ FILE_ATTRIBUTE_NORMAL,
        /* TemplateFile        */ nullptr)};

    if (fileHandle.get() == INVALID_HANDLE_VALUE)
    {
        (void)fileHandle.release();
        ApplyLastSystemError(ec);
        return false;
    }

    sha256_ctx ctx{};
    sha256_init(&ctx);

    std::array<unsigned char, 4096> buffer{};

    while (true)
    {
        DWORD bytesRead{};

        if (!ReadFile(fileHandle.get(), buffer.data(), static_cast<DWORD>(buffer.size()), &bytesRead, nullptr))
        {
            ApplyLastSystemError(ec);
            return false;
        }

        if (!bytesRead)
            break;

        sha256_update(&ctx, buffer.data(), bytesRead);

        if (bytesRead < static_cast<DWORD>(sizeof(buffer)))
            break;
    }

    sha256_final(&ctx, hash.data());
    return true;
}

auto GetFileBufferHash(const std::vector<unsigned char>& buffer) -> FileHash
{
    sha256_ctx ctx{};
    sha256_init(&ctx);

#ifdef _WIN64
    {
        constexpr size_t     maxReadSize = std::numeric_limits<unsigned int>::max();
        size_t               remainingBytes = buffer.size();
        const unsigned char* bufferData = buffer.data();

        while (remainingBytes)
        {
            unsigned int bytesToProcess = (remainingBytes > maxReadSize) ? maxReadSize : static_cast<DWORD>(remainingBytes);

            sha256_update(&ctx, bufferData, bytesToProcess);

            bufferData += bytesToProcess;
            remainingBytes -= bytesToProcess;
        }
    }
#else
    {
        sha256_update(&ctx, buffer.data(), buffer.size());
    }
#endif

    FileHash hash{};
    sha256_final(&ctx, hash.data());
    return hash;
}

bool SetDirectoryJunction(const fs::path& from, const fs::path& to, std::error_code& ec)
{
    ec.clear();

    // The source directory must exist.
    if (!fs::is_directory(from, ec))
        return false;

    // Check if the target directory is already pointing to the source directory.
    if (fs::is_symlink(to, ec))
    {
        if (fs::equivalent(from, to, ec))
            return true;

        if (ec)
            return false;
    }

    // Remove the target file to replace it.
    if (fs::exists(to, ec))
    {
        fs::remove_all(to, ec);
    }

    if (ec)
        return false;

    // Create the parent directory for the symlink.
    const fs::path toParentDirectory = to.parent_path();

    if (!fs::exists(toParentDirectory, ec))
    {
        if (ec || !fs::create_directories(toParentDirectory, ec))
            return false;

        if (!fs::is_directory(toParentDirectory, ec))
            return false;
    }

    // Use the native function to create the symlink due to flag usage.
    if (!CreateSymbolicLinkW(to.wstring().c_str(), from.wstring().c_str(), SYMBOLIC_LINK_FLAG_DIRECTORY | SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE))
    {
        ApplyLastSystemError(ec);
        return false;
    }

    return true;
}
