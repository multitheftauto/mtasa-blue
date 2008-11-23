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

CFilePathTranslator::CFilePathTranslator ( )
{
}

CFilePathTranslator::~CFilePathTranslator ( )
{
}

bool CFilePathTranslator::GetFileFromModPath ( string FileToGet, string & TranslatedFilePathOut )
{
    // Translate the path to this file.
    TranslatedFilePathOut =  m_ModPath;
    TranslatedFilePathOut += '\\';
    TranslatedFilePathOut += FileToGet;

    return true;
}

bool CFilePathTranslator::GetFileFromWorkingDirectory ( string FileToGet, string & TranslatedFilePathOut )
{
    // Translate the path to this file.
    TranslatedFilePathOut =  m_WorkingDirectory;
    TranslatedFilePathOut += '\\';
    TranslatedFilePathOut += FileToGet;

    return true;
}

void CFilePathTranslator::GetModPath ( string & ModPathOut )
{
    ModPathOut = m_ModPath;
}

void CFilePathTranslator::SetModPath ( string PathBasedOffWorkingDirectory )
{
    m_ModPath =  m_WorkingDirectory;
    m_ModPath += '\\';
    m_ModPath += PathBasedOffWorkingDirectory;
}

void CFilePathTranslator::SetCurrentWorkingDirectory ( string PathBasedOffModuleRoot )
{
    string RootDirectory;

    // Get the root directory.
    GetProcessRootDirectory ( RootDirectory );

    // Store it
    m_WorkingDirectory =  RootDirectory;
    m_WorkingDirectory += '\\';
    m_WorkingDirectory += PathBasedOffModuleRoot;  
}

void CFilePathTranslator::UnSetCurrentWorkingDirectory ( )
{
    m_WorkingDirectory = "";
}

void CFilePathTranslator::GetCurrentWorkingDirectory ( string & WorkingDirectoryOut )
{
    WorkingDirectoryOut = m_WorkingDirectory;
}

void CFilePathTranslator::GetProcessRootDirectory ( string & ModuleRootDirOut )
{
    HMODULE     hMainModule;
    char        szCurrentDir [ 512 ];

    // First, we get the handle to the root module (exe)
    hMainModule = GetModuleHandle ( NULL );

    // Next, we get the full path of the module.
    GetModuleFileName ( hMainModule, szCurrentDir, sizeof ( szCurrentDir ) );
    
    // Strip the module name out of the path.
    PathRemoveFileSpec ( szCurrentDir );

    ModuleRootDirOut = szCurrentDir;
}
