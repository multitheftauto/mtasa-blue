/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/FileHash.cpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "FileHash.h"

static constexpr std::string_view hexCharset{"0123456789ABCDEF"};

auto GetFileHashString(const FileHash& hash) -> std::string
{
    // We need a byte/character for every nibble/digit in the file hash.
    std::array<char, sizeof(FileHash) * 2> buffer{};

    for (size_t i = 0; i < hash.size(); ++i)
    {
        unsigned char value = hash[i];
        buffer[(i * 2) + 0] = hexCharset[(value >> 4) & 0xF];
        buffer[(i * 2) + 1] = hexCharset[(value >> 0) & 0xF];
    }

    return std::string{buffer.data(), buffer.size()};
}
