/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/FileSystem.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "FileHash.h"
#include <filesystem>
#include <vector>

/**
 * @brief Returns the size of a file in bytes without error reporting.
 * @param path Path to the file
*/
auto GetFileSize(const std::filesystem::path& path) -> uintmax_t;

/**
 * @brief Generates a unique file path for a temporary file.
 * @param attempts Number of attempts to generate a unique name
 * @param ec Parameter for error reporting
 */
auto GetTempFilePath(size_t attempts, std::error_code& ec) -> std::filesystem::path;

/**
 * @brief Reads the content of the file into the provided buffer.
 * @param path Path to the file
 * @param buffer Buffer for the file content
 * @param ec Parameter for error reporting
*/
bool GetFileContent(const std::filesystem::path& path, std::vector<unsigned char>& buffer, std::error_code& ec);

/**
 * @brief Writes the content of the buffer to the provided file.
 * @param path Path to the file
 * @param buffer Buffer for the file content
 * @param ec Parameter for error reporting
*/
bool SetFileContent(const std::filesystem::path& path, const std::vector<unsigned char>& buffer, std::error_code& ec);

/**
 * @brief Copies a file from a source location to a target location and verifies the contents.
 * @param from Path to the source file
 * @param to Path to the target file
 * @param ec Parameter for error reporting
 */
bool SafeCopyFile(const std::filesystem::path& from, const std::filesystem::path& to, std::error_code& ec);

/**
 * @brief Compares two files for strict equality.
 * @param base Path to the base file
 * @param other Path to the other file
 * @param ec Parameter for error reporting
 */
bool AreFilesEqual(const std::filesystem::path& base, const std::filesystem::path& other, std::error_code& ec);

/**
 * @brief Computes the SHA256 hash for the provided file.
 * @param path Path to the file
 * @param hash Result of the hash
 * @param ec Parameter for error reporting
*/
bool GetFileHash(const std::filesystem::path& path, FileHash& hash, std::error_code& ec);

/**
 * @brief Computes the SHA256 hash for the provided buffer.
 * @param buffer Buffer to generate a hash for
 * @return Result of the hash
*/
auto GetFileBufferHash(const std::vector<unsigned char>& buffer) -> FileHash;

/**
 * @brief Creates a directory junction. Usually requires administrator privilege on Windows.
 * @param from The directory the symlink should point to
 * @param to The symlink path
 * @param ec Parameter for error reporting
*/
bool SetDirectoryJunction(const std::filesystem::path& from, const std::filesystem::path& to, std::error_code& ec);
