/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.File.h
*  PURPOSE:
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

namespace SharedUtil
{
    //
    // Returns true if the file/directory exists
    //
    bool            FileExists                      ( const SString& strFilename );
    bool            DirectoryExists                 ( const SString& strPath );

    //
    // Load from a file
    //
    bool            FileLoad                        ( const SString& strFilename, std::vector < char >& buffer, int iMaxSize = 0x7FFFFFFF );
    bool            FileLoad                        ( const SString& strFilename, SString& strBuffer, int iMaxSize = 0x7FFFFFFF );

    //
    // Save to a file
    //
    bool            FileSave                        ( const SString& strFilename, const void* pBuffer, unsigned long ulSize, bool bForce = true );
    bool            FileSave                        ( const SString& strFilename, const SString& strBuffer, bool bForce = true );

    //
    // Append to a file
    //
    bool            FileAppend                      ( const SString& strFilename, const void* pBuffer, unsigned long ulSize, bool bForce = true );
    bool            FileAppend                      ( const SString& strFilename, const SString& strBuffer, bool bForce = true );

    //
    // Get a file size
    //
    uint            FileSize                        ( const SString& strFilename );

    //
    // Ensure all directories exist to the file
    //
    void            MakeSureDirExists               ( const SString& strPath );

    SString         PathConform                     ( const SString& strInPath );
    SString         PathJoin                        ( const SString& str1, const SString& str2 );
    SString         PathJoin                        ( const SString& str1, const SString& str2, const SString& str3, const SString& str4 = "", const SString& str5 = "" );
    SString         PathMakeRelative                ( const SString& strInBasePath, const SString& strInAbsPath );
    void            ExtractFilename                 ( const SString& strPathFilename, SString* strPath, SString* strFilename );
    bool            ExtractExtension                ( const SString& strFilename, SString* strRest, SString* strExt );
    SString         ExtractPath                     ( const SString& strPathFilename );
    SString         ExtractFilename                 ( const SString& strPathFilename );
    SString         ExtractExtension                ( const SString& strPathFilename );
    SString         ExtractBeforeExtension          ( const SString& strPathFilename );

    bool            FileDelete                      ( const SString& strFilename, bool bForce = true );
    bool            FileRename                      ( const SString& strFilenameOld, const SString& strFilenameNew );
    bool            DelTree                         ( const SString& strPath, const SString& strInsideHere );
    bool            MkDir                           ( const SString& strInPath, bool bTree = true );
    bool            FileCopy                        ( const SString& strSrc, const SString& strDest, bool bForce = true );
    std::vector < SString > FindFiles               ( const SString& strMatch, bool bFiles, bool bDirectories, bool bSortByDate = false );
    SString         MakeUniquePath                  ( const SString& strPathFilename );
    SString         ConformPathForSorting           ( const SString& strPathFilename );

    SString         GetSystemCurrentDirectory       ( void );
    SString         GetSystemDllDirectory           ( void );
    SString         GetSystemLocalAppDataPath       ( void );
    SString         GetSystemCommonAppDataPath      ( void );
    SString         GetSystemWindowsPath            ( void );
    SString         GetSystemSystemPath             ( void );
    SString         GetSystemTempPath               ( void );
    SString         GetMTADataPath                  ( void );
    SString         GetMTATempPath                  ( void );

    SString         GetLaunchPathFilename           ( void );
    SString         GetLaunchPath                   ( void );
    SString         GetLaunchFilename               ( void );

    WString         FromUTF8                        ( const SString& strPath );
    SString         ToUTF8                          ( const WString& strPath );
}
