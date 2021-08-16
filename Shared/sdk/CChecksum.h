/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/CChecksum.h
 *  PURPOSE:     Checksum class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
// Note: Cannot use #pragma once here, due to a duplicate existing in publicsdk
#ifndef __CChecksum_H
#define __CChecksum_H

#include <variant>
#include "SharedUtil.Hash.h"
#include "SString.h"
#include <bochs_internal/bochs_crc32.h>

// Depends on CMD5Hasher and CRCGenerator
class CChecksum
{
public:
    // Initialize to zeros
    CChecksum()
    {
        ulCRC = 0;
        memset(md5.data, 0, sizeof(md5.data));
    }

    // Comparison operators
    bool operator==(const CChecksum& other) const { return ulCRC == other.ulCRC && memcmp(md5.data, other.md5.data, sizeof(md5.data)) == 0; }

    bool operator!=(const CChecksum& other) const { return !operator==(other); }

    // GenerateChecksumFromFile returns either a CChecksum or an error message.
    static std::variant<CChecksum, std::string> GenerateChecksumFromFile(const SString& strFilename)
    {
        CChecksum result;
        result.ulCRC = CRCGenerator::GetCRCFromFile(strFilename);

        if (!result.ulCRC)
            return SString("CRC could not open file: %s", std::strerror(errno));

        bool success = CMD5Hasher().Calculate(strFilename, result.md5);
        if (!success)
            return SString("MD5 could not open file: %s", std::strerror(errno));

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
};

#endif
