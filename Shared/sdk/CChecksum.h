/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/CChecksum.h
 *  PURPOSE:     Checksum class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
// Note: Cannot use #pragma once here, due to a duplicate existing in publicsdk
#ifndef __CChecksum_H
#define __CChecksum_H

#include <cerrno>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <variant>
#include "SharedUtil.Hash.h"
#include "SharedUtil.File.h"
#include "SString.h"
#include <bochs_internal/bochs_crc32.h>

#ifndef _WIN32
    #include <sys/stat.h>
    #include <time.h>
#endif

// Depends on CMD5Hasher and CRCGenerator
class CChecksum
{
public:
    CChecksum() : ulCRC(0), md5{} {}

    // Comparison operators
    bool operator==(const CChecksum& other) const { return ulCRC == other.ulCRC && memcmp(md5.data, other.md5.data, sizeof(md5.data)) == 0; }

    bool operator!=(const CChecksum& other) const { return !operator==(other); }

    static void ClearChecksumCache()
    {
        std::lock_guard<std::mutex> lock(CacheMutex());
        Cache().clear();
    }

    // Results are cached per path and reused while the size and last write time stay the same,
    // so re-checking an unchanged file costs one stat instead of a full read
    static std::variant<CChecksum, std::string> GenerateChecksumFromFile(const SString& strFilename)
    {
        const std::string strKey = CacheKey(strFilename);

        SFileStamp stamp;
        const bool bHasStamp = GetFileStamp(strFilename, stamp);

        if (bHasStamp)
        {
            std::lock_guard<std::mutex> lock(CacheMutex());
            auto                        iter = Cache().find(strKey);
            if (iter != Cache().end() && iter->second.stamp == stamp && iter->second.IsSettled())
            {
                CChecksum cached;
                cached.ulCRC = iter->second.ulCRC;
                cached.md5 = iter->second.md5;
                return cached;
            }
        }

        CChecksum   result;
        std::string strError;
        if (!HashFile(strFilename, result, strError))
            return strError;

        // Only remember the result if nothing wrote to the file while we were reading it
        SFileStamp stampAfter;
        if (bHasStamp && GetFileStamp(strFilename, stampAfter) && stampAfter == stamp)
        {
            std::lock_guard<std::mutex> lock(CacheMutex());
            Cache()[strKey] = SCacheEntry{stamp, result.ulCRC, result.md5, Now()};
        }

        return result;
    }

    // GenerateChecksumFromFileUnsafe should never ever be used unless you are a bad person. Or unless you really know what you're doing.
    // If it's the latter, please leave a code comment somewhere explaining why. Otherwise we'll think it's just code that hasn't been migrated yet.
    static CChecksum GenerateChecksumFromFileUnsafe(const SString& strFilename)
    {
        auto result = GenerateChecksumFromFile(strFilename);

        // If it holds an error message, just return a default CChecksum
        if (std::holds_alternative<std::string>(result))
            return CChecksum();

        return std::get<CChecksum>(result);
    }

    static CChecksum GenerateChecksumFromBuffer(const char* cpBuffer, unsigned long ulLength)
    {
        CChecksum result;
        result.ulCRC = CRCGenerator::GetCRCFromBuffer(cpBuffer, ulLength);
        CMD5Hasher().Calculate(cpBuffer, ulLength, result.md5);
        return result;
    }

    unsigned long ulCRC;
    MD5           md5;

private:
    // Times are in 100ns ticks. The epoch differs per platform, but values are only ever compared with each other
    struct SFileStamp
    {
        std::uint64_t uiSize = 0;
        std::uint64_t uiWriteTime = 0;

        bool operator==(const SFileStamp& other) const { return uiSize == other.uiSize && uiWriteTime == other.uiWriteTime; }
    };

    struct SCacheEntry
    {
        SFileStamp    stamp;
        unsigned long ulCRC = 0;
        MD5           md5{};
        std::uint64_t uiHashedAt = 0;

        // A file can be written twice within the timestamp granularity of the filesystem, so a hash taken
        // right after the last write is only trusted once that write is comfortably in the past
        bool IsSettled() const { return uiHashedAt >= stamp.uiWriteTime + 2 * TICKS_PER_SECOND; }
    };

    static constexpr std::uint64_t TICKS_PER_SECOND = 10000000;

    // Never destroyed, checksum tasks may still be running when statics are torn down
    static std::unordered_map<std::string, SCacheEntry>& Cache()
    {
        static auto* pCache = new std::unordered_map<std::string, SCacheEntry>();
        return *pCache;
    }

    static std::mutex& CacheMutex()
    {
        static auto* pMutex = new std::mutex();
        return *pMutex;
    }

    static std::string CacheKey(const SString& strFilename)
    {
        std::string strKey = strFilename;
#ifdef _WIN32
        for (char& c : strKey)
        {
            if (c >= 'A' && c <= 'Z')
                c += 'a' - 'A';
            else if (c == '\\')
                c = '/';
        }
#endif
        return strKey;
    }

    static bool GetFileStamp(const SString& strFilename, SFileStamp& stamp)
    {
#ifdef _WIN32
        WString strWide;
        try
        {
            strWide = SharedUtil::FromUTF8(strFilename);
        }
        catch (...)
        {
            return false;
        }

        WIN32_FILE_ATTRIBUTE_DATA attr;
    #ifdef MTA_CLIENT
        if (!SharedUtil::GetFileAttributesExWithTimeout(strWide.c_str(), attr, 500))
            return false;
    #else
        if (!GetFileAttributesExW(strWide.c_str(), GetFileExInfoStandard, &attr))
            return false;
    #endif
        stamp.uiSize = (std::uint64_t(attr.nFileSizeHigh) << 32) | attr.nFileSizeLow;
        stamp.uiWriteTime = (std::uint64_t(attr.ftLastWriteTime.dwHighDateTime) << 32) | attr.ftLastWriteTime.dwLowDateTime;
        return true;
#elif defined(__APPLE__)
        struct stat info;
        if (stat(strFilename.c_str(), &info) != 0)
            return false;
        stamp.uiSize = static_cast<std::uint64_t>(info.st_size);
        stamp.uiWriteTime = ToTicks(info.st_mtimespec);
        return true;
#else
        struct stat64 info;
        if (stat64(strFilename.c_str(), &info) != 0)
            return false;
        stamp.uiSize = static_cast<std::uint64_t>(info.st_size);
        stamp.uiWriteTime = ToTicks(info.st_mtim);
        return true;
#endif
    }

    static std::uint64_t Now()
    {
#ifdef _WIN32
        FILETIME ft;
        GetSystemTimeAsFileTime(&ft);
        return (std::uint64_t(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
#else
        timespec ts{};
        clock_gettime(CLOCK_REALTIME, &ts);
        return ToTicks(ts);
#endif
    }

#ifndef _WIN32
    static std::uint64_t ToTicks(const timespec& ts) { return std::uint64_t(ts.tv_sec) * TICKS_PER_SECOND + std::uint64_t(ts.tv_nsec) / 100; }
#endif

    static bool HashFile(const SString& strFilename, CChecksum& result, std::string& strError)
    {
#ifdef MTA_CLIENT
        // The client goes through the timeout helpers so a stalled disk cannot freeze the game
        SString strBuffer;
        if (!SharedUtil::FileLoadWithTimeout(strFilename, strBuffer, 2000))
        {
            strError = SString("Could not read: %s", strFilename.c_str());
            return false;
        }

        result = GenerateChecksumFromBuffer(strBuffer.data(), static_cast<unsigned long>(strBuffer.size()));
        return true;
#else
        // One pass over the file feeds both hashes. Retry briefly in case another process still has it open for writing
        int iLastError = 0;

        for (int iAttempt = 0; iAttempt < 3; ++iAttempt)
        {
            if (iAttempt > 0)
                std::this_thread::sleep_for(std::chrono::milliseconds(50 * iAttempt));

            errno = 0;
            FILE* pFile = SharedUtil::File::FopenExclusive(strFilename, "rb");
            if (!pFile)
            {
                iLastError = errno ? errno : EIO;
                if (iLastError == ENOENT)
                    break;
                continue;
            }

            CMD5Hasher    hasher;
            unsigned long ulCRC = 0;
            char          buffer[65536];
            bool          bReadOk = true;

            hasher.Init();
            while (true)
            {
                const size_t uiRead = fread(buffer, 1, sizeof(buffer), pFile);
                if (uiRead == 0)
                {
                    bReadOk = !ferror(pFile);
                    break;
                }

                ulCRC = CRCGenerator::GetCRCFromBuffer(buffer, uiRead, ulCRC);
                hasher.Update(reinterpret_cast<unsigned char*>(buffer), static_cast<unsigned int>(uiRead));
            }
            fclose(pFile);

            if (!bReadOk)
            {
                iLastError = errno ? errno : EIO;
                continue;
            }

            hasher.Finalize();
            memcpy(result.md5.data, hasher.GetResult(), sizeof(result.md5.data));
            result.ulCRC = ulCRC;
            return true;
        }

        if (iLastError == ENOENT)
            strError = SString("File not found: %s", strFilename.c_str());
        else
            strError = SString("Could not checksum '%s': %s", strFilename.c_str(), std::strerror(iLastError));
        return false;
#endif
    }
};

#endif
