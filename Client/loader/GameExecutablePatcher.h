/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/GameExecutablePatcher.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "FileHash.h"
#include <filesystem>
#include <vector>
#include <string>

/**
 * @brief Removes and also conditionally applies patches to the game executable.
 */
class GameExecutablePatcher final
{
public:
    explicit GameExecutablePatcher(const std::filesystem::path& gtaExePath) : m_path(gtaExePath) {}

    /**
     * @brief Loads the executable from disk and prepares it for patching.
     * @param ec Parameter for error reporting
     */
    bool Load(std::error_code& ec);

    /**
     * @brief Applies patches to a previously loaded executable. You can only do this once after loading.
     * @param ec Parameter for error reporting
     */
    bool ApplyPatches(std::error_code& ec);

    /**
     * @brief Generates a MD5 hex string for the current executable buffer.
     */
    auto GenerateMD5() const -> std::string;

private:
    std::filesystem::path      m_path;
    FileHash                   m_onDiskHash{};
    std::vector<unsigned char> m_executable{};
    bool                       m_isAlreadyPatched{false};
};
