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
    // Returns true if the file exists
    //
    bool FileExists ( const SString& strFilename );

    //
    // Load binary data from a file into an array
    //
    bool FileLoad ( const SString& strFilename, std::vector < char >& buffer );

    //
    // Save binary data to a file
    //
    bool FileSave ( const SString& strFilename, const void* pBuffer, unsigned long ulSize );

    //
    // Append binary data to a file
    //
    bool FileAppend ( const SString& strFilename, const void* pBuffer, unsigned long ulSize );

    //
    // Ensure all directories exist to the file
    //
    void MakeSureDirExists ( const SString& strPath );

    SString PathConform ( const SString& strInPath );
    SString PathJoin ( const SString& str1, const SString& str2, const SString& str3 = "", const SString& str4 = "", const SString& str5 = "" );

    SString GetMTAAppDataPath ( void );

    bool DelTree ( const SString& strPath, const SString& strInsideHere );
    bool MkDir ( const SString& strInPath, bool bTree = true );
    bool FileCopy ( const SString& strSrc, const SString& strDest );
    SString GetCurrentWorkingDirectory ( void );
    std::vector < SString > FindFiles ( const SString& strMatch, bool bFiles, bool bDirectories );
    SString MakeUniquePath ( const SString& strPathFilename );
    void MoveFileOrDirOutOfTheWay ( const SString& strPath );

    SString GetMTAAppDataPath ( void );
    SString GetMTATempPath ( void );

}
