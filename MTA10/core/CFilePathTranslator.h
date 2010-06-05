/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CFilePathTranslator.h
*  PURPOSE:     Header file for file path translator class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CFILEPATHTRANSLATOR_H
#define __CFILEPATHTRANSLATOR_H

#include <string>

class CFilePathTranslator 
{
public:
                CFilePathTranslator ( );
               ~CFilePathTranslator ( );
    
    void        SetCurrentWorkingDirectory       ( std::string PathBasedOffModuleRoot );
    void        UnSetCurrentWorkingDirectory     ( ); 
    void        SetModPath                       ( std::string PathBasedOffWorkingDirectory );

    void        GetModPath                       ( std::string & ModPathOut );
    void        GetCurrentWorkingDirectory       ( std::string & WorkingDirectoryOut );

    bool        GetFileFromModPath               ( std::string FileToGet, std::string & TranslatedFilePathOut );
    bool        GetFileFromWorkingDirectory      ( std::string FileToGet, std::string & TranslatedFilePathOut );

    void        GetGTARootDirectory              ( std::string & ModuleRootDirOut );
    void        GetMTASARootDirectory            ( std::string & InstallRootDirOut );

private:

    std::string      m_ModPath;
    std::string      m_WorkingDirectory;
};

#endif