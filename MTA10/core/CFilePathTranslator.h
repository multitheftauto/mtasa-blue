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
using namespace std;

class CFilePathTranslator 
{
    public:
                CFilePathTranslator ( );
               ~CFilePathTranslator ( );
    
    void        SetCurrentWorkingDirectory       ( string PathBasedOffModuleRoot );
    void        UnSetCurrentWorkingDirectory     ( ); 
    void        SetModPath                       ( string PathBasedOffWorkingDirectory );

    void        GetModPath                       ( string & ModPathOut );
    void        GetCurrentWorkingDirectory       ( string & WorkingDirectoryOut );

    bool        GetFileFromModPath               ( string FileToGet, string & TranslatedFilePathOut );
    bool        GetFileFromWorkingDirectory      ( string FileToGet, string & TranslatedFilePathOut );

    void        GetGTARootDirectory              ( string & ModuleRootDirOut );
    void        GetMTASARootDirectory            ( string & InstallRootDirOut );

    private:

    string      m_ModPath;
    string      m_WorkingDirectory;
};

#endif