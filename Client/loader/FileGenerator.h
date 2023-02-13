/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/FileGenerator.cpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <string>
#include <filesystem>
#include <vector>

/**
 * @brief Generates files from a verified GTA-based patch on demand.
*/
class FileGenerator final
{
public:
    FileGenerator();

    /**
     * @brief Generates a specific file.
     * @param name Code name of the file
     * @param targetPath File path for the generated file
     * @param ec Parameter for error reporting
    */
    bool GenerateFile(const std::string& name, const std::filesystem::path& targetPath, std::error_code& ec);

public:
    /**
     * @brief Provides the path to the patch base.
    */
    static auto GetPatchBasePath() -> std::filesystem::path;

    /**
     * @brief Provides the path to the patch diff.
    */
    static auto GetPatchDiffPath() -> std::filesystem::path;

    /**
     * @brief Checks whether the patch base is valid.
     * @param filePath Path to the patch base
    */
    static bool IsPatchBase(const std::filesystem::path& filePath);

    /**
     * @brief Checks whether the patch diff is valid.
     * @param filePath Path to the patch diff
    */
    static bool IsPatchDiff(const std::filesystem::path& filePath);

private:
    bool LoadPatcherData(std::error_code& ec);

private:
    std::vector<unsigned char> m_data;
    std::filesystem::path      m_patchBasePath;
    std::filesystem::path      m_patchDiffPath;
};
