/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CFilePathTranslator.cpp
*  PURPOSE:     Easy interface to file paths
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

bool CFilePathTranslator::GetFileFromModPath ( const std::string& FileToGet, std::string & TranslatedFilePathOut )
{
    // Translate the path to this file.
    TranslatedFilePathOut =  m_ModPath;
    TranslatedFilePathOut += '\\';
    TranslatedFilePathOut += FileToGet;

    return true;
}

bool CFilePathTranslator::GetFileFromWorkingDirectory ( const std::string& FileToGet, std::string & TranslatedFilePathOut )
{
    // Translate the path to this file.
    TranslatedFilePathOut =  m_WorkingDirectory;
    TranslatedFilePathOut += '\\';
    TranslatedFilePathOut += FileToGet;

    return true;
}

void CFilePathTranslator::GetModPath (std::string & ModPathOut )
{
    ModPathOut = m_ModPath;
}

void CFilePathTranslator::SetModPath ( const std::string& PathBasedOffWorkingDirectory )
{
    m_ModPath =  m_WorkingDirectory;
    m_ModPath += '\\';
    m_ModPath += PathBasedOffWorkingDirectory;
}

void CFilePathTranslator::SetCurrentWorkingDirectory ( const std::string& PathBasedOffModuleRoot )
{
    std::string RootDirectory;

    // Get the root directory.
    GetMTASARootDirectory ( RootDirectory );

    // Store it
    m_WorkingDirectory =  RootDirectory;
    m_WorkingDirectory += '\\';
    m_WorkingDirectory += PathBasedOffModuleRoot;  
}

void CFilePathTranslator::UnSetCurrentWorkingDirectory ( )
{
    m_WorkingDirectory = "";
}

void CFilePathTranslator::GetCurrentWorkingDirectory (std::string & WorkingDirectoryOut )
{
    WorkingDirectoryOut = m_WorkingDirectory;
}

void CFilePathTranslator::GetGTARootDirectory (std::string & ModuleRootDirOut )
{
    ModuleRootDirOut = GetLaunchPath();
}


void CFilePathTranslator::GetMTASARootDirectory (std::string & InstallRootDirOut )
{
    InstallRootDirOut = GetMTASABaseDir ();
}


