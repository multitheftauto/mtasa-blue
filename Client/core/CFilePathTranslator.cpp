/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/core/CFilePathTranslator.cpp
 *  PURPOSE:     Easy interface to file paths
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CFilePathTranslator.h"
#include <filesystem>

extern std::filesystem::path g_gtaDirectory;

CFilePathTranslator::CFilePathTranslator()
{
}

CFilePathTranslator::~CFilePathTranslator()
{
}

bool CFilePathTranslator::GetFileFromModPath(const std::string& FileToGet, std::string& TranslatedFilePathOut)
{
    // Translate the path to this file.
    TranslatedFilePathOut = m_ModPath;
    TranslatedFilePathOut += '\\';
    TranslatedFilePathOut += FileToGet;

    return true;
}

bool CFilePathTranslator::GetFileFromWorkingDirectory(const std::string& FileToGet, std::string& TranslatedFilePathOut)
{
    // Translate the path to this file.
    TranslatedFilePathOut = m_WorkingDirectory;
    TranslatedFilePathOut += '\\';
    TranslatedFilePathOut += FileToGet;

    return true;
}

void CFilePathTranslator::GetModPath(std::string& ModPathOut)
{
    ModPathOut = m_ModPath;
}

void CFilePathTranslator::SetModPath(const std::string& PathBasedOffWorkingDirectory)
{
    m_ModPath = m_WorkingDirectory;
    m_ModPath += '\\';
    m_ModPath += PathBasedOffWorkingDirectory;
}

void CFilePathTranslator::SetCurrentWorkingDirectory(const std::string& PathBasedOffModuleRoot)
{
    std::string RootDirectory;

    // Get the root directory.
    GetMTASARootDirectory(RootDirectory);

    // Store it
    m_WorkingDirectory = RootDirectory;
    m_WorkingDirectory += '\\';
    m_WorkingDirectory += PathBasedOffModuleRoot;
}

void CFilePathTranslator::UnSetCurrentWorkingDirectory()
{
    m_WorkingDirectory = "";
}

void CFilePathTranslator::GetCurrentWorkingDirectory(std::string& WorkingDirectoryOut)
{
    WorkingDirectoryOut = m_WorkingDirectory;
}

void CFilePathTranslator::GetGTARootDirectory(std::string& ModuleRootDirOut)
{
    ModuleRootDirOut = g_gtaDirectory.u8string();
}

void CFilePathTranslator::GetMTASARootDirectory(std::string& InstallRootDirOut)
{
    InstallRootDirOut = GetMTASABaseDir();
}
