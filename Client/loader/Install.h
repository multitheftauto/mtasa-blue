/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/Install.h
 *  PURPOSE:     Handles the installation of updates for MTA
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

struct ManifestFile
{
    std::string relativePath{};
    uint32_t    checksum{};
};

extern std::string MANIFEST_NAME;

/**
 * @brief Extracts files from the supplied archive and optionally generates a manifest.
 * @param archivePath The archive to extract to the current directory
 */
bool ExtractFiles(const std::string& archivePath, bool withManifest);

/**
 * @brief Executes the installation of an update.
 * @param showProgressWindow Display a pseudo progress window for the user
 */
bool InstallFiles(bool showProgressWindow);

/**
 * @brief Prepares operations after a restart.
 * The operations "files" and "silent" both prepare the installation from an update archive.
 * @return An action string
 */
SString CheckOnRestartCommand();

/**
 * @brief Generates a manifest file for install verification.
 */
bool GenerateManifestFile(const SString& directory, const std::vector<ManifestFile>& files);

/**
 * @brief Parses the manifest in the source directory.
 */
bool ParseManifestFile(const SString& sourceRoot, std::vector<ManifestFile>& files);
