/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.File.hpp
*  PURPOSE:
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifdef WIN32
    #include "shellapi.h"
    #include "shlobj.h"
    #include <shlwapi.h>
    #pragma comment(lib, "Shlwapi.lib")
#else
    #include <dirent.h>
    #include <sys/stat.h>
#endif

//
// Returns true if the file exists
//
bool SharedUtil::FileExists ( const SString& strFilename )
{
#ifdef WIN32
    DWORD dwAtr = GetFileAttributes ( strFilename );
    if ( dwAtr == INVALID_FILE_ATTRIBUTES )
        return false;
    return ( ( dwAtr & FILE_ATTRIBUTE_DIRECTORY) == 0 );     
#else
    struct stat Info;
    if ( stat ( strFilename, &Info ) == -1 )
        return false;
    return !( S_ISDIR ( Info.st_mode ) );
#endif
}


//
// Returns true if the directory exists
//
bool SharedUtil::DirectoryExists ( const SString& strPath )
{
#ifdef WIN32
    DWORD dwAtr = GetFileAttributes ( strPath );
    if ( dwAtr == INVALID_FILE_ATTRIBUTES )
        return false;
    return ( ( dwAtr & FILE_ATTRIBUTE_DIRECTORY) != 0 );     
#else
    struct stat Info;
    if ( stat ( strPath, &Info ) == -1 )
        return false;
    return ( S_ISDIR ( Info.st_mode ) );
#endif
}


bool SharedUtil::FileLoad ( const SString& strFilename, SString& strBuffer, int iMaxSize )
{
    strBuffer = "";
    std::vector < char > buffer;
    if ( !FileLoad ( strFilename, buffer, iMaxSize ) )
        return false;
    if ( buffer.size () )
        strBuffer = std::string ( &buffer.at ( 0 ), buffer.size () );

    return true;
}

bool SharedUtil::FileSave ( const SString& strFilename, const SString& strBuffer, bool bForce )
{
    return FileSave ( strFilename, strBuffer.length () ? &strBuffer.at ( 0 ) : NULL, strBuffer.length (), bForce );
}

bool SharedUtil::FileAppend ( const SString& strFilename, const SString& strBuffer, bool bForce )
{
    return FileAppend ( strFilename, strBuffer.length () ? &strBuffer.at ( 0 ) : NULL, strBuffer.length (), bForce );
}

bool SharedUtil::FileDelete ( const SString& strFilename, bool bForce )
{
#ifdef WIN32
    if ( bForce )
        SetFileAttributes ( strFilename, FILE_ATTRIBUTE_NORMAL );
#endif
    return unlink ( strFilename ) == 0;
}

bool SharedUtil::FileRename ( const SString& strFilenameOld, const SString& strFilenameNew )
{
#ifdef WIN32
    return MoveFile ( strFilenameOld, strFilenameNew ) != 0;
#else
    return rename ( strFilenameOld, strFilenameNew ) == 0;
#endif
}

//
// Load binary data from a file into an array
//
bool SharedUtil::FileLoad ( const SString& strFilename, std::vector < char >& buffer, int iMaxSize )
{
    buffer.clear ();
    // Open
    FILE* fh = fopen ( strFilename, "rb" );
    if ( !fh )
        return false;
    // Get size
    fseek ( fh, 0, SEEK_END );
    int size = ftell ( fh );
    rewind ( fh );

    int bytesRead = 0;
    if ( size > 0 && size < 1e9 )
    {
        size = Min ( size, iMaxSize );
        // Allocate space
        buffer.assign ( size, 0 );
        // Read into buffer
        bytesRead = fread ( &buffer.at ( 0 ), 1, size, fh );
    }
    // Close
    fclose ( fh );
    return bytesRead == size;
}


//
// Save binary data to a file
//
bool SharedUtil::FileSave ( const SString& strFilename, const void* pBuffer, unsigned long ulSize, bool bForce )
{
#ifdef WIN32
    if ( bForce )
        SetFileAttributes ( strFilename, FILE_ATTRIBUTE_NORMAL );
#endif

    if ( bForce )
        MakeSureDirExists ( strFilename );

    FILE* fh = fopen ( strFilename, "wb" );
    if ( !fh )
        return false;

    bool bSaveOk = true;
    if ( ulSize )
        bSaveOk = ( fwrite ( pBuffer, 1, ulSize, fh ) == ulSize );
    fclose ( fh );
    return bSaveOk;
}


//
// Append binary data to a file
//
bool SharedUtil::FileAppend ( const SString& strFilename, const void* pBuffer, unsigned long ulSize, bool bForce )
{
#ifdef WIN32
    if ( bForce )
        SetFileAttributes ( strFilename, FILE_ATTRIBUTE_NORMAL );
#endif

    FILE* fh = fopen ( strFilename, "ab" );
    if ( !fh )
        return false;

    bool bSaveOk = true;
    if ( ulSize )
        bSaveOk = ( fwrite ( pBuffer, 1, ulSize, fh ) == ulSize );
    fclose ( fh );
    return bSaveOk;
}


//
// Get a file size
//
uint SharedUtil::FileSize ( const SString& strFilename  )
{
    // Open
    FILE* fh = fopen ( strFilename, "rb" );
    if ( !fh )
        return 0;
    // Get size
    fseek ( fh, 0, SEEK_END );
    uint size = ftell ( fh );
    // Close
    fclose ( fh );
    return size;
}


//
// Ensure all directories exist to the file
//
void SharedUtil::MakeSureDirExists ( const SString& strPath )
{
    std::vector < SString > parts;
    PathConform ( strPath ).Split ( PATH_SEPERATOR, parts );

    // Find first dir that already exists
    int idx = parts.size () - 1;
    for ( ; idx >= 0 ; idx-- )
    {
        SString strTemp = SString::Join ( PATH_SEPERATOR, parts, 0, idx );
        if ( DirectoryExists ( strTemp ) )
            break;        
    }

    // Make non existing dirs only
    idx++;
    for ( ; idx < (int)parts.size () ; idx++ )
    {
        SString strTemp = SString::Join ( PATH_SEPERATOR, parts, 0, idx );
        // Call mkdir on this path
        #ifdef WIN32
            mkdir ( strTemp );
        #else
            mkdir ( strTemp ,0775 );
        #endif
    }
}


SString SharedUtil::PathConform ( const SString& strPath )
{
    // Make slashes the right way and remove duplicates, except for UNC type indicators
#if WIN32
    SString strTemp = strPath.Replace ( "/", PATH_SEPERATOR );
#else
    SString strTemp = strPath.Replace ( "\\", PATH_SEPERATOR );
#endif
    // Remove slash duplicates
    size_t iFirstDoubleSlash = strTemp.find ( PATH_SEPERATOR PATH_SEPERATOR );
    if ( iFirstDoubleSlash == std::string::npos )
        return strTemp;     // No duplicates present

    // If first double slash is not at the start, then treat as a normal duplicate if:
    //      1. It is not preceeded by a colon, or
    //      2. Another single slash is before it
    if ( iFirstDoubleSlash > 0 )
    {
        if ( iFirstDoubleSlash == 2 && strTemp[1] == ':' )
        {
            // Replace all duplicate slashes
            return strTemp.Replace ( PATH_SEPERATOR PATH_SEPERATOR, PATH_SEPERATOR, true );
        }

        if ( strTemp.SubStr ( iFirstDoubleSlash - 1, 1 ) != ":" || strTemp.find ( PATH_SEPERATOR ) < iFirstDoubleSlash  )
        {
            // Replace all duplicate slashes
            return strTemp.Replace ( PATH_SEPERATOR PATH_SEPERATOR, PATH_SEPERATOR, true );
        }
    }

    return strTemp.Left ( iFirstDoubleSlash + 1 ) + strTemp.SubStr ( iFirstDoubleSlash + 1 ).Replace ( PATH_SEPERATOR PATH_SEPERATOR, PATH_SEPERATOR, true );
}

SString SharedUtil::PathJoin ( const SString& str1, const SString& str2 )
{
    return PathConform ( str1 + PATH_SEPERATOR + str2 );
}

SString SharedUtil::PathJoin ( const SString& str1, const SString& str2, const SString& str3, const SString& str4, const SString& str5 )
{
    SString strResult = str1 + PATH_SEPERATOR + str2 + PATH_SEPERATOR + str3;
    if ( str4.length () )
       strResult += PATH_SEPERATOR + str4; 
    if ( str5.length () )
       strResult += PATH_SEPERATOR + str5;
    return PathConform ( strResult );
}

// Remove base path from the start of abs path
SString SharedUtil::PathMakeRelative ( const SString& strInBasePath, const SString& strInAbsPath )
{
    SString strBasePath = PathConform ( strInBasePath );
    SString strAbsPath = PathConform ( strInAbsPath );
    if ( strAbsPath.BeginsWithI ( strBasePath ) )
    {
        return strAbsPath.SubStr ( strBasePath.length () ).TrimStart ( PATH_SEPERATOR );
    }
    return strAbsPath;
}



SString SharedUtil::GetSystemCurrentDirectory ( void )
{
#ifdef WIN32
    wchar_t szResult [ 1024 ] = L"";
    GetCurrentDirectoryW ( NUMELMS ( szResult ), szResult );
    if ( IsShortPathName( szResult ) )
        return GetSystemLongPathName( ToUTF8( szResult ) );
    return ToUTF8( szResult );
#else
    char szBuffer[ MAX_PATH ];
    getcwd ( szBuffer, MAX_PATH - 1 );
    return szBuffer;
#endif
}

#ifdef WIN32
#ifdef MTA_CLIENT

SString SharedUtil::GetSystemDllDirectory ( void )
{
    wchar_t szResult [ 1024 ] = L"";
    GetDllDirectoryW ( NUMELMS ( szResult ), szResult );
    if ( IsShortPathName( szResult ) )
        return GetSystemLongPathName( ToUTF8( szResult ) );
    return ToUTF8( szResult );
}

SString SharedUtil::GetSystemLocalAppDataPath ( void )
{
    wchar_t szResult[MAX_PATH] = L"";
    SHGetFolderPathW( NULL, CSIDL_LOCAL_APPDATA, NULL, 0, szResult );
    if ( IsShortPathName( szResult ) )
        return GetSystemLongPathName( ToUTF8( szResult ) );
    return ToUTF8( szResult );
}

SString SharedUtil::GetSystemCommonAppDataPath ( void )
{
    wchar_t szResult[MAX_PATH] = L"";
    SHGetFolderPathW( NULL, CSIDL_COMMON_APPDATA, NULL, 0, szResult );
    if ( IsShortPathName( szResult ) )
        return GetSystemLongPathName( ToUTF8( szResult ) );
    return ToUTF8( szResult );
}

SString SharedUtil::GetSystemPersonalPath ( void )
{
    wchar_t szResult[MAX_PATH] = L"";
    SHGetFolderPathW( NULL, CSIDL_PERSONAL, NULL, 0, szResult );
    if ( IsShortPathName( szResult ) )
        return GetSystemLongPathName( ToUTF8( szResult ) );
    return ToUTF8( szResult );
}

SString SharedUtil::GetSystemWindowsPath ( void )
{
    wchar_t szResult[MAX_PATH] = L"";
    SHGetFolderPathW( NULL, CSIDL_WINDOWS, NULL, 0, szResult );
    if ( IsShortPathName( szResult ) )
        return GetSystemLongPathName( ToUTF8( szResult ) );
    return ToUTF8( szResult );
}

SString SharedUtil::GetSystemSystemPath ( void )
{
    wchar_t szResult[MAX_PATH] = L"";
    SHGetFolderPathW( NULL, CSIDL_SYSTEM, NULL, 0, szResult );
    if ( IsShortPathName( szResult ) )
        return GetSystemLongPathName( ToUTF8( szResult ) );
    return ToUTF8( szResult );
}

SString SharedUtil::GetSystemTempPath ( void )
{
    wchar_t szResult[4030] = L"";
    GetTempPathW( 4000, szResult );
    if ( IsShortPathName( szResult ) )
        return GetSystemLongPathName( ToUTF8( szResult ) );
    return ToUTF8( szResult );
}

SString SharedUtil::GetMTADataPath ( void )
{
    return PathJoin ( GetSystemCommonAppDataPath(), "MTA San Andreas All", GetMajorVersionString () );
}

SString SharedUtil::GetMTADataPathCommon ( void )
{
    return PathJoin ( GetSystemCommonAppDataPath(), "MTA San Andreas All", "Common" );
}

SString SharedUtil::GetMTATempPath ( void )
{
    return PathJoin ( GetSystemTempPath(), "MTA" + GetMajorVersionString () );
}


// C:\Program Files\gta_sa.exe
SString SharedUtil::GetLaunchPathFilename( void )
{
    static SString strLaunchPathFilename;
    if ( strLaunchPathFilename.empty() )
    {
        wchar_t szBuffer[2048];
        GetModuleFileNameW( NULL, szBuffer, NUMELMS(szBuffer) - 1 );
        if ( IsShortPathName( szBuffer ) )
            return GetSystemLongPathName( ToUTF8( szBuffer ) );
        strLaunchPathFilename = ToUTF8( szBuffer );
    }
    return strLaunchPathFilename;
}

// C:\Program Files
SString SharedUtil::GetLaunchPath( void )
{
    return ExtractPath( GetLaunchPathFilename() );
}

// gta_sa.exe
SString SharedUtil::GetLaunchFilename( void )
{
    return ExtractFilename( GetLaunchPathFilename() );
}

// Get drive root from path
SString SharedUtil::GetPathDriveName( const SString& strPath )
{
    wchar_t szDrive[4] = L"";
    int iDriveNumber = PathGetDriveNumberW( FromUTF8( strPath ) );
    if ( iDriveNumber > -1 )
        PathBuildRootW( szDrive, iDriveNumber );
    return ToUTF8( szDrive );
}

// Get drive free bytes available to the current user
uint SharedUtil::GetPathFreeSpaceMB( const SString& strPath )
{
    SString strDrive = GetPathDriveName( strPath );
    if ( !strDrive.empty() )
    {
        ULARGE_INTEGER llUserFreeBytesAvailable;
        if ( GetDiskFreeSpaceExW( FromUTF8( strDrive ), &llUserFreeBytesAvailable, NULL, NULL ) )
        {
            llUserFreeBytesAvailable.QuadPart /= 1048576UL;
            if ( llUserFreeBytesAvailable.HighPart == 0 )
                return llUserFreeBytesAvailable.LowPart;
        }
    }
    return -1;
}

SString SharedUtil::GetDriveNameWithNotEnoughSpace( uint uiResourcesPathMinMB, uint uiDataPathMinMB )
{
    if ( GetPathFreeSpaceMB( GetMTASABaseDir() ) < uiResourcesPathMinMB )
        return GetPathDriveName( GetMTASABaseDir() );
    if ( GetPathFreeSpaceMB( GetSystemCommonAppDataPath() ) < uiDataPathMinMB )
        return GetPathDriveName( GetSystemCommonAppDataPath() );
   return ""; 
}


#endif  // #ifdef MTA_CLIENT
#endif  // #ifdef WIN32


WString SharedUtil::FromUTF8( const SString& strPath )
{
#ifdef WIN32_TESTING   // This might be faster - Needs testing
    const char* szSrc = strPath;
    uint cCharacters = strlen ( szSrc ) + 1 ;
    uint cbUnicode = cCharacters * 4;
    wchar_t* Dest = (wchar_t*)alloca ( cbUnicode );

    if ( MultiByteToWideChar ( CP_UTF8, 0, szSrc, -1, Dest, (int)cbUnicode ) == 0 )
    {
        return WString();
    }
    else
    {
        return Dest;
    }
#else
    return MbUTF8ToUTF16( strPath );
#endif
}


SString SharedUtil::ToUTF8( const WString& strPath )
{
#ifdef WIN32_TESTING   // This might be faster - Needs testing
    const wchar_t* pszW = strPath;
    uint cCharacters = wcslen(pszW)+1;
    uint cbAnsi = cCharacters * 6;
    char* pData = (char*)alloca ( cbAnsi );

    if (0 == WideCharToMultiByte(CP_UTF8, 0, pszW, cCharacters, pData, cbAnsi, NULL, NULL))
    {
        return "";
    }
    return pData;
#else
    return UTF16ToMbUTF8( strPath );
#endif
}


#ifdef WIN32
///////////////////////////////////////////////////////////////
//
// DelTree
//
//
//
///////////////////////////////////////////////////////////////
bool SharedUtil::DelTree ( const SString& strPath, const SString& strInsideHere )
{
    // Safety: Make sure strPath is inside strInsideHere
    WString wstrPath = FromUTF8( strPath );
    WString wstrInsideHere = FromUTF8( strInsideHere );
    if ( !wstrPath.BeginsWithI( wstrInsideHere ) )
    {
        assert ( 0 );
        return false;
    }

    DWORD dwBufferSize = ( wstrPath.length() + 3 ) * sizeof( wchar_t );
    wchar_t *szBuffer = static_cast < wchar_t* > ( alloca ( dwBufferSize ) );
    memset ( szBuffer, 0, dwBufferSize );
    wcsncpy ( szBuffer, wstrPath, wstrPath.length() );
    SHFILEOPSTRUCTW sfos;

    sfos.hwnd = NULL;
    sfos.wFunc = FO_DELETE;
    sfos.pFrom = szBuffer;  // Double NULL terminated
    sfos.pTo = NULL;
    sfos.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT | FOF_ALLOWUNDO;

    int status = SHFileOperationW(&sfos);
    return status == 0;
}
#endif


///////////////////////////////////////////////////////////////
//
// MkDir
//
// Returns true if the directory is created or already exists
// TODO: Make bTree off option work
//
///////////////////////////////////////////////////////////////
bool SharedUtil::MkDir ( const SString& strInPath, bool bTree )
{
    SString strPath = PathConform ( strInPath );
    MakeSureDirExists ( strPath + PATH_SEPERATOR );
    return DirectoryExists ( strPath );
}


///////////////////////////////////////////////////////////////
//
// FileCopy
//
// Copies a single file.
//
///////////////////////////////////////////////////////////////
bool SharedUtil::FileCopy ( const SString& strSrc, const SString& strDest, bool bForce )
{
    if ( bForce )
        MakeSureDirExists ( strDest );

#ifdef WIN32
    if ( bForce )
        SetFileAttributes ( strDest, FILE_ATTRIBUTE_NORMAL );
#endif

    FILE* fhSrc = fopen ( strSrc, "rb" );
    if ( !fhSrc )
    {
        return false;
    }

    FILE* fhDst = fopen ( strDest, "wb" );
    if ( !fhDst )
    {
        fclose ( fhSrc );
        return false;
    }

    char cBuffer[65536];
    while ( true )
    {
        size_t dataLength = fread ( cBuffer, 1, 65536, fhSrc );
        if ( dataLength == 0 )
            break;
        fwrite ( cBuffer, 1, dataLength, fhDst );
    }

    fclose ( fhSrc );
    fclose ( fhDst );
    return true;
}


#ifdef WIN32
///////////////////////////////////////////////////////////////
//
// FindFiles
//
// Find all files or directories at a path
// If sorted by date, returns last modified last
//
///////////////////////////////////////////////////////////////
std::vector < SString > SharedUtil::FindFiles ( const SString& strInMatch, bool bFiles, bool bDirectories, bool bSortByDate )
{
    std::vector < SString > strResult;
    std::multimap < uint64, SString > sortMap;

    SString strMatch = PathConform ( strInMatch );
    if ( strMatch.Right ( 1 ) == PATH_SEPERATOR )
        strMatch += "*";

    WIN32_FIND_DATAW findData;
    HANDLE hFind = FindFirstFileW ( FromUTF8( strMatch ), &findData );
    if( hFind != INVALID_HANDLE_VALUE )
    {
        do
        {
            if ( ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ? bDirectories : bFiles )
                if ( wcscmp ( findData.cFileName, L"." ) && wcscmp ( findData.cFileName, L".." ) )
                {
                    if ( bSortByDate )
                        MapInsert( sortMap, (uint64&)findData.ftLastWriteTime, ToUTF8( findData.cFileName ) );
                    else
                        strResult.push_back ( ToUTF8( findData.cFileName ) );
                }
        }
        while( FindNextFileW( hFind, &findData ) );
        FindClose( hFind );
    }

    // Resolve sorted map if required
    if ( !sortMap.empty() )
    {
        for ( std::multimap < uint64, SString >::iterator iter = sortMap.begin() ; iter != sortMap.end() ; ++iter )
            strResult.push_back ( iter->second );
    }

    return strResult;
}

#else

std::vector < SString > SharedUtil::FindFiles ( const SString& strMatch, bool bFiles, bool bDirectories, bool bSortByDate )
{
    std::vector < SString > strResult;
    std::multimap < uint64, SString > sortMap;

    DIR *Dir;
    struct dirent *DirEntry;

    // Remove any filename matching characters
    SString strSearchDirectory = PathJoin( strMatch.SplitLeft( "/", NULL, -1 ), "/" );

    if ( ( Dir = opendir ( strMatch ) ) )
    {
        while ( ( DirEntry = readdir ( Dir ) ) != NULL )
        {
            // Skip dotted entries
            if ( strcmp ( DirEntry->d_name, "." ) && strcmp ( DirEntry->d_name, ".." ) )
            {
                struct stat Info;
                bool bIsDir = false;

                SString strPath = PathJoin ( strMatch, DirEntry->d_name );

                // Determine the file stats
                if ( lstat ( strPath, &Info ) != -1 )
                    bIsDir = S_ISDIR ( Info.st_mode );

                if ( bIsDir ? bDirectories : bFiles )
                {
                    if ( bSortByDate )
                    {
                        SString strAbsPath = strSearchDirectory + DirEntry->d_name;
                        struct stat attrib;
                        stat( strAbsPath, &attrib );
                        MapInsert( sortMap, (uint64)attrib.st_mtime, DirEntry->d_name );
                    }
                    else
                        strResult.push_back ( DirEntry->d_name );
                }
            }
        }
        closedir ( Dir );
    }

    // Resolve sorted map if required
    if ( !sortMap.empty() )
    {
        for ( std::multimap < uint64, SString >::iterator iter = sortMap.begin() ; iter != sortMap.end() ; ++iter )
            strResult.push_back ( iter->second );
    }

    return strResult;
}
#endif


void SharedUtil::ExtractFilename ( const SString& strInPathFilename, SString* strPath, SString* strFilename )
{
    const SString strPathFilename = PathConform ( strInPathFilename );
    if ( !strPathFilename.Split ( PATH_SEPERATOR, strPath, strFilename, -1 ) )
        if ( strFilename )
            *strFilename = strPathFilename;
}


bool SharedUtil::ExtractExtension ( const SString& strFilename, SString* strMain, SString* strExt )
{
    return strFilename.Split ( ".", strMain, strExt, -1 );
}


SString SharedUtil::ExtractPath ( const SString& strPathFilename )
{
    SString strPath;
    ExtractFilename ( strPathFilename, &strPath, NULL );
    return strPath;
}


SString SharedUtil::ExtractFilename ( const SString& strPathFilename )
{
    SString strFilename;
    ExtractFilename ( strPathFilename, NULL, &strFilename );
    return strFilename;
}


SString SharedUtil::ExtractExtension ( const SString& strPathFilename )
{
    SString strExt;
    ExtractExtension ( strPathFilename, NULL, &strExt );
    return strExt;
}


SString SharedUtil::ExtractBeforeExtension ( const SString& strPathFilename )
{
    SString strMain;
    ExtractExtension ( strPathFilename, &strMain, NULL );
    return strMain;
}


SString SharedUtil::MakeUniquePath ( const SString& strInPathFilename )
{
    const SString strPathFilename = PathConform ( strInPathFilename );

    SString strBeforeUniqueChar, strAfterUniqueChar;

    SString strPath, strFilename;
    ExtractFilename ( strPathFilename, &strPath, &strFilename );

    SString strMain, strExt;
    if ( ExtractExtension ( strFilename, &strMain, &strExt ) )
    {
        strBeforeUniqueChar = PathJoin ( strPath, strMain );
        strAfterUniqueChar = "." + strExt;
    }
    else
    {
        strBeforeUniqueChar = strPathFilename;
        strAfterUniqueChar = "";
    }

    SString strTest = strPathFilename;
    int iCount = 1;
#ifdef WIN32
    while ( GetFileAttributes ( strTest ) != INVALID_FILE_ATTRIBUTES )
#else
    while ( DirectoryExists ( strTest ) || FileExists ( strTest ) )
#endif
    {
        strTest = SString ( "%s_%d%s", strBeforeUniqueChar.c_str (), iCount++, strAfterUniqueChar.c_str () );
    }
    return strTest;
}

// Conform a path string for sorting
SString SharedUtil::ConformPathForSorting ( const SString& strPathFilename )
{
    LOCAL_FUNCTION_START
        static int mytolower( int c )
        {
            // Ignores locale and always does this:
            if ( c >= 'A' && c <= 'Z' )
                c = c - 'A' + 'a';
            return c;
        }
    LOCAL_FUNCTION_END

    SString strResult = strPathFilename;
    std::transform ( strResult.begin(), strResult.end(), strResult.begin(), LOCAL_FUNCTION::mytolower );
    return strResult;
}

// Return true if path is not relative
bool SharedUtil::IsAbsolutePath ( const SString& strInPath )
{
    const SString strPath = PathConform ( strInPath );

    if ( strPath.BeginsWith( PATH_SEPERATOR ) )
        return true;
#ifdef WIN32
    if ( strPath.length() > 0 && strPath[1] == ':' )
        return true;
#endif
    return false;
}

#ifdef WIN32
bool SharedUtil::IsShortPathName( const char* szPath )
{
    return strchr( szPath, '~' ) != NULL;
}

bool SharedUtil::IsShortPathName( const wchar_t* szPath )
{
    return wcschr( szPath, '~' ) != NULL;
}

SString SharedUtil::GetSystemShortPathName( const SString& strPath )
{
    wchar_t szBuffer[32000];
    szBuffer[0] = 0;
    GetShortPathNameW( FromUTF8( strPath ), szBuffer, NUMELMS( szBuffer ) - 1 );
    return ToUTF8( szBuffer );
}

SString SharedUtil::GetSystemLongPathName( const SString& strPath )
{
    wchar_t szBuffer[32000];
    szBuffer[0] = 0;
    GetLongPathNameW( FromUTF8( strPath ), szBuffer, NUMELMS( szBuffer ) - 1 );
    return ToUTF8( szBuffer );
}
#endif // WIN32
