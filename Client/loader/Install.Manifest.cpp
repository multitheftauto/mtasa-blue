/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/Install.Manifest.cpp
 *  PURPOSE:     Handles loading and creating of manifest files for installation
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "Install.h"
#include <fstream>
#include <iomanip>
#include <charconv>

std::string MANIFEST_NAME = "manifest.txt";

/**
 * @brief Parses the manifest header with meta information.
 */
static void ParseManifestMeta(std::ifstream& manifest, std::unordered_map<std::string, std::string>& meta)
{
    std::string line;

    while (std::getline(manifest, line))
    {
        // Meta information ends after a single empty line.
        if (line.empty())
            break;

        // Every key-value pair is separated by a colon character.
        if (const size_t colon = line.find(':'); colon != std::string::npos && colon > 0)
        {
            // Transform key to lowercase.
            std::string key = line.substr(0, colon);
            std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) { return std::tolower(c); });

            // Discard key if value is empty.
            if (const size_t nonSpace = line.find_first_not_of(' ', colon + 1); nonSpace != std::string::npos)
            {
                std::string value = line.substr(nonSpace);
                meta[key] = value;
            }
        }
    }
}

/**
 * @brief Parses the manifest body with a file listing.
 */
static void ParseManifestVersion1(std::ifstream& manifest, std::vector<ManifestFile>& files)
{
    std::string line;

    while (std::getline(manifest, line))
    {
        if (line.empty())
            continue;

        // The format for each entry is "<CRC32> <Path>".
        if (size_t space = line.find_first_of(' '); space != std::string::npos && space > 0)
        {
            uint32_t checksum{};

            if (auto& [ptr, ec] = std::from_chars(line.data(), line.data() + space, checksum, 16); ec == std::errc{})
            {
                ManifestFile file{};
                file.relativePath = line.substr(space + 1);
                file.checksum = checksum;
                files.emplace_back(file);
            }
        }
    }
}

bool ParseManifestFile(const SString& sourceRoot, std::vector<ManifestFile>& files)
{
    files.clear();

    std::ifstream manifest(PathJoin(sourceRoot, MANIFEST_NAME));

    if (!manifest.is_open())
        return false;

    std::unordered_map<std::string, std::string> meta;
    ParseManifestMeta(manifest, meta);

    std::string version = meta["version"];

    if (version == "1")
    {
        ParseManifestVersion1(manifest, files);
        return true;
    }
    else
    {
        SString message("ParseManifestFile: Version '%s' is not supported", version.c_str());
        AddReportLog(5055, message);
        return false;
    }
}

bool GenerateManifestFile(const SString& directory, const std::vector<ManifestFile>& files)
{
    std::ofstream manifest(PathJoin(directory, MANIFEST_NAME));

    if (!manifest.is_open())
        return false;

    // Write metadata to the manifest.
    manifest << "Version: 1\n";
    manifest << '\n';

    // Write files to the manifest.
    for (const ManifestFile& file : files)
    {
        manifest << std::hex << std::setw(8) << std::setfill('0') << file.checksum << ' ' << file.relativePath << '\n';
    }

    manifest.close();
    return manifest.good();
}
