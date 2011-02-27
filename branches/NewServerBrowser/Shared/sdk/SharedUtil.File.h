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
    bool            FileLoad                        ( const SString& strFilename, std::vector < char >& buffer );
    bool            FileLoad                        ( const SString& strFilename, SString& strBuffer );

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
    SString         PathJoin                        ( const SString& str1, const SString& str2, const SString& str3 = "", const SString& str4 = "", const SString& str5 = "" );
    void            ExtractFilename                 ( const SString& strPathFilename, SString* strPath, SString* strFilename );
    bool            ExtractExtention                ( const SString& strFilename, SString* strRest, SString* strExt );

    bool            FileDelete                      ( const SString& strFilename, bool bForce = true );
    bool            DelTree                         ( const SString& strPath, const SString& strInsideHere );
    bool            MkDir                           ( const SString& strInPath, bool bTree = true );
    bool            FileCopy                        ( const SString& strSrc, const SString& strDest, bool bForce = true );
    SString         GetCurrentWorkingDirectory      ( void );
    SString         GetCurrentDirectory             ( void );
    SString         GetWindowsDirectory             ( void );
    std::vector < SString > FindFiles               ( const SString& strMatch, bool bFiles, bool bDirectories );
    SString         MakeUniquePath                  ( const SString& strPathFilename );

    SString         GetMTALocalAppDataPath          ( void );
    SString         GetMTATempPath                  ( void );

}
