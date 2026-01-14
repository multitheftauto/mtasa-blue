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

#include <variant>
#include <thread>
#include <chrono>
#include "SharedUtil.Hash.h"
#include "SharedUtil.File.h"
#include "SString.h"
#include <bochs_internal/bochs_crc32.h>

// Depends on CMD5Hasher and CRCGenerator
class CChecksum
{
public:
    CChecksum() : ulCRC(0), md5{} {}

    // Comparison operators
    bool operator==(const CChecksum& other) const { return ulCRC == other.ulCRC && memcmp(md5.data, other.md5.data, sizeof(md5.data)) == 0; }

    bool operator!=(const CChecksum& other) const { return !operator==(other); }

    // GenerateChecksumFromFile returns either a CChecksum or an error message.
    static std::variant<CChecksum, std::string> GenerateChecksumFromFile(const SString& strFilename)
    {
        constexpr int maxRetries = 3;
        constexpr int retryDelayMs = 50;
        int lastErrno = 0;
        int attemptsMade = 0;

        for (int attempt = 0; attempt < maxRetries; ++attempt)
        {
            ++attemptsMade;

            if (attempt > 0)
                std::this_thread::sleep_for(std::chrono::milliseconds(retryDelayMs * attempt));

            errno = 0;

            CChecksum result;
            result.ulCRC = CRCGenerator::GetCRCFromFile(strFilename);

            if (errno)
            {
                lastErrno = errno;
                if (errno == ENOENT)
                    break;
                continue;
            }

            errno = 0;
            bool success = CMD5Hasher().Calculate(strFilename, result.md5);

            if (!success)
            {
                lastErrno = errno ? errno : EIO;
                if (errno == ENOENT)
                    break;
                continue;
            }

            return result;
        }

        if (lastErrno == ENOENT)
            return SString("File not found: %s", strFilename.c_str());

        return SString("Could not checksum '%s' after %d attempt%s: %s", 
                       strFilename.c_str(),
                       attemptsMade, 
                       attemptsMade == 1 ? "" : "s",
                       lastErrno ? std::strerror(lastErrno) : "Unknown error");
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
};

#endif
