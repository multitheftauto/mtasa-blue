/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/core/CFileSystem.cpp
*  PURPOSE:     File management
*  DEVELOPERS:  S2 Games <http://savage.s2games.com> (historical entry)
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include <sys/stat.h>

#ifdef __linux__
#include <utime.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#endif //__linux__

std::list <CFile*> *openFiles;

CFileSystem *fileSystem = NULL;
CFileTranslator *fileRoot = NULL;

#include "CFileSystem.Utils.hxx"

/*===================================================
    File_IsDirectoryAbsolute

    Arguments:
        pPath - Absolute path pointing to an OS filesystem entry.
    Purpose:
        Checks the given path and returns true if it points
        to a directory, false if a file or no entry was found
        at the path.
===================================================*/
static inline bool File_IsDirectoryAbsolute( const char *pPath )
{
#ifdef _WIN32
    DWORD dwAttributes = GetFileAttributes(pPath);

    if (dwAttributes == INVALID_FILE_ATTRIBUTES)
        return false;

    return (dwAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
#elif defined(__linux__)
    struct stat dirInfo;

    if ( stat( pPath, &dirInfo ) != 0 )
        return false;

    return ( dirInfo.st_mode & S_IFDIR ) != 0;
#else
    return false;
#endif
}

/*===================================================
    CRawFile

    This class represents a file on the system.
    As long as it is present, the file is opened.

    fixme: Port to unix and mac
===================================================*/

CRawFile::~CRawFile( void )
{
#ifdef _WIN32
    CloseHandle( m_file );
#elif defined(__linux__)
    fclose( m_file );
#endif //OS DEPENDANT CODE

    openFiles->remove( this );
}

size_t CRawFile::Read( void *pBuffer, size_t sElement, unsigned long iNumElements )
{
#ifdef _WIN32
    DWORD dwBytesRead;

    if (sElement == 0 || iNumElements == 0)
        return 0;

    ReadFile(m_file, pBuffer, sElement * iNumElements, &dwBytesRead, NULL);
    return dwBytesRead / sElement;
#elif defined(__linux__)
    return fread( pBuffer, sElement, iNumElements, m_file );
#else
    return 0;
#endif //OS DEPENDANT CODE
}

size_t CRawFile::Write( const void *pBuffer, size_t sElement, unsigned long iNumElements )
{
#ifdef _WIN32
    DWORD dwBytesWritten;

    if (sElement == 0 || iNumElements == 0)
        return 0;

    WriteFile(m_file, pBuffer, sElement * iNumElements, &dwBytesWritten, NULL);
    return dwBytesWritten / sElement;
#elif defined(__linux__)
    return fwrite( pBuffer, sElement, iNumElements, m_file );
#else
    return 0;
#endif //OS DEPENDANT CODE
}

int CRawFile::Seek( long iOffset, int iType )
{
#ifdef _WIN32
    if (SetFilePointer(m_file, iOffset, NULL, iType) == INVALID_SET_FILE_POINTER)
        return -1;
    return 0;
#elif defined(__linux__)
    return fseek( m_file, iOffset, iType );
#else
    return -1;
#endif //OS DEPENDANT CODE
}

long CRawFile::Tell( void ) const
{
#ifdef _WIN32
    return SetFilePointer( m_file, 0, NULL, FILE_CURRENT );
#elif defined(__linux__)
    return ftell( m_file );
#else
    return -1;
#endif //OS DEPENDANT CODE
}

bool CRawFile::IsEOF() const
{
#ifdef _WIN32
    return ( SetFilePointer( m_file, 0, NULL, FILE_CURRENT ) >= GetFileSize( m_file, NULL ) );
#elif defined(__linux__)
    return feof( m_file );
#else
    return false;
#endif //OS DEPENDANT CODE
}

bool CRawFile::Stat( struct stat *pFileStats ) const
{
#ifdef _WIN32
    BY_HANDLE_FILE_INFORMATION info;

    if (!GetFileInformationByHandle( m_file, &info ))
        return false;

    pFileStats->st_size = info.nFileSizeLow;
    pFileStats->st_dev = info.nFileIndexLow;
    pFileStats->st_atime = info.ftLastAccessTime.dwLowDateTime;
    pFileStats->st_ctime = info.ftCreationTime.dwLowDateTime;
    pFileStats->st_mtime = info.ftLastWriteTime.dwLowDateTime;
    pFileStats->st_dev = info.dwVolumeSerialNumber;
    pFileStats->st_mode = 0;
    pFileStats->st_nlink = (unsigned short)info.nNumberOfLinks;
    pFileStats->st_rdev = 0;
    pFileStats->st_gid = 0;
    return true;
#elif __linux
    return fstat( fileno( m_file ), pFileStats ) != 0;
#else
    return false;
#endif //OS DEPENDANT CODE
}

#ifdef _WIN32
inline static void TimetToFileTime( time_t t, LPFILETIME pft )
{
    LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000;
    pft->dwLowDateTime = (DWORD) ll;
    pft->dwHighDateTime = ll >>32;
}
#endif //_WIN32

void CRawFile::PushStat( const struct stat *stats )
{
#ifdef _WIN32
    FILETIME ctime;
    FILETIME atime;
    FILETIME mtime;

    TimetToFileTime( stats->st_ctime, &ctime );
    TimetToFileTime( stats->st_atime, &atime );
    TimetToFileTime( stats->st_mtime, &mtime );

    SetFileTime( m_file, &ctime, &atime, &mtime );
#elif defined(__linux__)
    struct utimbuf timeBuf;
    timeBuf.actime = stats->st_atime;
    timeBuf.modtime = stats->st_mtime;

    utime( m_path.c_str(), &timeBuf );
#endif //OS DEPENDANT CODE
}

void CRawFile::SetSeekEnd( void )
{
#ifdef _WIN32
    SetEndOfFile( m_file );
#elif defined(__linux__)
    ftruncate64( fileno( m_file ), ftello64( m_file ) );
#endif //OS DEPENDANT CODE
}

size_t CRawFile::GetSize( void ) const
{
#ifdef _WIN32
    return GetFileSize( m_file, NULL );
#elif defined(__linux__)
    struct stat fileInfo;
    fstat( fileno( m_file ), &fileInfo );

    return fileInfo.st_size;
#else
    return 0;
#endif //OS DEPENDANT CODE
}

void CRawFile::SetSize( size_t size )
{
#ifdef _WIN32
    SetFilePointer( m_file, size, NULL, SEEK_SET );

    SetEndOfFile( m_file );
#elif defined(__linux__)
    ftruncate64( fileno( m_file ), size );
#endif //OS DEPENDANT CODE
}

void CRawFile::Flush( void )
{
#ifdef _WIN32
    FlushFileBuffers( m_file );
#elif defined(__linux__)
    fflush( m_file );
#endif //OS DEPENDANT CODE
}

const filePath& CRawFile::GetPath( void ) const
{
    return m_path;
}

bool CRawFile::IsReadable( void ) const
{
    return ( m_access & FILE_ACCESS_READ ) != 0;
}

bool CRawFile::IsWriteable( void ) const
{
    return ( m_access & FILE_ACCESS_WRITE ) != 0;
}

/*=========================================
    CBufferedFile

    Loads a file at open and keeps it in a managed buffer.
=========================================*/

CBufferedFile::~CBufferedFile( void )
{
    return;
}

size_t CBufferedFile::Read( void *pBuffer, size_t sElement, unsigned long iNumElements )
{
    long iReadElements = std::min( ( m_sSize - m_iSeek ) / sElement, iNumElements );
    size_t sRead = iReadElements * sElement;

    if ( iNumElements == 0 )
        return 0;

    memcpy( pBuffer, m_pBuffer + m_iSeek, sRead );
    m_iSeek += sRead;
    return iReadElements;
}

size_t CBufferedFile::Write( const void *pBuffer, size_t sElement, unsigned long iNumElements )
{
    return 0;
}

int CBufferedFile::Seek( long iOffset, int iType )
{
    switch( iType )
    {
    case SEEK_SET:
        m_iSeek = 0;
        break;
    case SEEK_END:
        m_iSeek = m_sSize;
        break;
    }

    m_iSeek = std::max( 0l, std::min( m_iSeek + iOffset, (long)m_sSize ) );
    return 0;
}

long CBufferedFile::Tell( void ) const
{
    return m_iSeek;
}

bool CBufferedFile::IsEOF( void ) const
{
    return ( (size_t)m_iSeek == m_sSize );
}

bool CBufferedFile::Stat( struct stat *stats ) const
{
    stats->st_dev = -1;
    stats->st_ino = -1;
    stats->st_mode = -1;
    stats->st_nlink = -1;
    stats->st_uid = -1;
    stats->st_gid = -1;
    stats->st_rdev = -1;
    stats->st_size = m_sSize;
    stats->st_atime = 0;
    stats->st_ctime = 0;
    stats->st_mtime = 0;
    return 0;
}

void CBufferedFile::PushStat( const struct stat *stats )
{
    // Does not do anything.
    return;
}

size_t CBufferedFile::GetSize( void ) const
{
    return m_sSize;
}

void CBufferedFile::SetSeekEnd( void )
{
    return;
}

void CBufferedFile::Flush( void )
{
    return;
}

const filePath& CBufferedFile::GetPath( void ) const
{
    return m_path;
}

int CBufferedFile::ReadInt( void )
{
    int iResult;

    if ( ( m_sSize - m_iSeek ) < sizeof(int) )
        return 0;

    iResult = *(int*)( m_pBuffer + m_iSeek );
    m_iSeek += sizeof(int);
    return iResult;
}

short CBufferedFile::ReadShort( void )
{
    short iResult;

    if ( (m_sSize - m_iSeek) < sizeof(short) )
        return 0;

    iResult = *(short*)(m_pBuffer + m_iSeek);
    m_iSeek += sizeof(short);
    return iResult;
}

char CBufferedFile::ReadByte( void )
{
    if ( m_sSize == (size_t)m_iSeek )
        return 0;

    return *(m_pBuffer + m_iSeek++);
}

bool CBufferedFile::IsReadable( void ) const
{
    return true;
}

bool CBufferedFile::IsWriteable( void ) const
{
    return false;
}

/*=======================================
    CSystemPathTranslator

    Filesystem path translation utility
=======================================*/

CSystemPathTranslator::CSystemPathTranslator( bool isSystemPath )
{
    m_isSystemPath = isSystemPath;
}

void CSystemPathTranslator::GetDirectory( filePath& output ) const
{
    output = m_currentDir;
}

bool CSystemPathTranslator::ChangeDirectory( const char *path )
{
    dirTree tree;
    bool file;

    if ( !GetRelativePathTreeFromRoot( path, tree, file ) )
        return false;

    if ( file )
        tree.pop_back();

    m_curDirTree = tree;

    m_currentDir.clear();
    _File_OutputPathTree( tree, false, m_currentDir );
    return true;
}

bool CSystemPathTranslator::GetFullPathTreeFromRoot( const char *path, dirTree& tree, bool& file ) const
{
    dirTree output;
    tree = m_rootTree;

    if ( !GetRelativePathTreeFromRoot( path, output, file ) )
        return false;

    tree.insert( tree.end(), output.begin(), output.end() );
    return true;
}

bool CSystemPathTranslator::GetFullPathTree( const char *path, dirTree& tree, bool& file ) const
{
    dirTree output;
    tree = m_rootTree;

    if ( IsTranslatorRootDescriptor( *path ) )
    {
        if ( !_File_ParseRelativePath( path + 1, output, file ) )
            return false;
    }
    else
    {
        output = m_curDirTree;

        if ( !_File_ParseRelativePath( path, output, file ) )
            return false;
    }

    tree.insert( tree.end(), output.begin(), output.end() );
    return true;
}

bool CSystemPathTranslator::GetRelativePathTreeFromRoot( const char *path, dirTree& tree, bool& file ) const
{
    if ( IsTranslatorRootDescriptor( *path ) )
        return _File_ParseRelativePath( path + 1, tree, file );

    tree = m_curDirTree;
    return _File_ParseRelativePath( path, tree, file );
}

bool CSystemPathTranslator::GetRelativePathTree( const char *path, dirTree& tree, bool& file ) const
{
    if ( IsTranslatorRootDescriptor( *path ) )
    {
        dirTree relTree;

        if ( !_File_ParseRelativePath( path + 1, relTree, file ) )
            return false;

        return _File_ParseDeriviateTreeRoot( relTree, m_curDirTree, tree, file );
    }

    return _File_ParseDeriviateTree( path, m_curDirTree, tree, file );
}

bool CSystemPathTranslator::GetFullPathFromRoot( const char *path, bool allowFile, filePath& output ) const
{
    output = m_root;
    return GetRelativePathFromRoot( path, allowFile, output );
}

bool CSystemPathTranslator::GetFullPath( const char *path, bool allowFile, filePath& output ) const
{
    dirTree tree;
    bool file;

    if ( !GetFullPathTree( path, tree, file ) )
        return false;

    if ( file && !allowFile )
    {
        tree.pop_back();

        file = false;
    }

    _File_OutputPathTree( tree, file, output );
    return true;
}

bool CSystemPathTranslator::GetRelativePathFromRoot( const char *path, bool allowFile, filePath& output ) const
{
    dirTree tree;
    bool file;

    if ( !GetRelativePathTreeFromRoot( path, tree, file ) )
        return false;

    if ( file && !allowFile )
    {
        tree.pop_back();

        file = false;
    }

    _File_OutputPathTree( tree, file, output );
    return true;
}

bool CSystemPathTranslator::GetRelativePath( const char *path, bool allowFile, filePath& output ) const
{
    dirTree tree;
    bool file;

    if ( !GetRelativePathTree( path, tree, file ) )
        return false;

    if ( file && !allowFile )
    {
        tree.pop_back();

        file = false;
    }

    _File_OutputPathTree( tree, file, output );
    return true;
}

/*=======================================
    CSystemFileTranslator

    Default file translator
=======================================*/

static inline bool _File_IsAbsolutePath( const char *path )
{
    char character = *path;

#ifdef _WIN32
    switch( character )
    {
    // Drive letters
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
    case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
    case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
        return ( path[1] == ':' && path[2] != 0 );
    }
#elif defined(__linux__)
    switch( character )
    {
    case '/':
    case '\\':
        return true;
    }
#endif //OS DEPENDANT CODE

    return false;
}

CSystemFileTranslator::~CSystemFileTranslator( void )
{
#ifdef _WIN32
    if ( m_curDirHandle )
        CloseHandle( m_curDirHandle );

    CloseHandle( m_rootHandle );
#elif defined(__linux__)
    if ( m_curDirHandle )
        closedir( m_curDirHandle );

    closedir( m_rootHandle );
#endif //OS DEPENDANT CODE
}

bool CSystemFileTranslator::WriteData( const char *path, const char *buffer, size_t size )
{
    filePath output = m_root;
    dirTree tree;
    bool isFile;

    if ( !GetRelativePathTreeFromRoot( path, tree, isFile ) || !isFile )
        return false;

    _File_OutputPathTree( tree, true, output );

    // Make sure directory exists
    tree.pop_back();
    _CreateDirTree( tree );

#ifdef _WIN32
    HANDLE file;

    if ( (file = CreateFile( output.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL )) == INVALID_HANDLE_VALUE )
        return false;

    DWORD numWritten;

    WriteFile( file, buffer, size, &numWritten, NULL );

    CloseHandle( file );
    return numWritten == size;
#elif defined(__linux__)
    int fileToken = open( output.c_str(), O_CREAT | O_WRONLY, FILE_ACCESS_FLAG );

    if ( fileToken == -1 )
        return false;

    ssize_t numWritten = write( fileToken, buffer, size );

    close( fileToken );
    return numWritten == size;
#else
    return 0;
#endif //OS DEPENDANT CODE
}

void CSystemFileTranslator::_CreateDirTree( const dirTree& tree )
{
    dirTree::const_iterator iter;
    filePath path = m_root;

    for ( iter = tree.begin(); iter != tree.end(); ++iter )
    {
        path += *iter;
        path += '/';

#ifdef _WIN32
        CreateDirectory( path.c_str(), NULL );
#else
        mkdir( path.c_str(), FILE_ACCESS_FLAG );
#endif //OS DEPENDANT CODE
    }
}

bool CSystemFileTranslator::CreateDir( const char *path )
{
    dirTree tree;
    bool file;

    if ( !GetRelativePathTreeFromRoot( path, tree, file ) )
        return false;

    if ( file )
        tree.pop_back();

    _CreateDirTree( tree );
    return true;
}

CFile* CSystemFileTranslator::OpenEx( const char *path, const char *mode, unsigned int flags )
{
    dirTree tree;
    filePath output = m_root;
    CRawFile *pFile;
    unsigned int dwAccess = 0;
    unsigned int dwCreate = 0;
    bool file;

    if ( !GetRelativePathTreeFromRoot( path, tree, file ) )
        return NULL;

    // We can only open files!
    if ( !file )
        return NULL;

    _File_OutputPathTree( tree, true, output );

    if ( !_File_ParseMode( *this, path, mode, dwAccess, dwCreate ) )
        return NULL;

    // Creation requires the dir tree!
    if ( dwCreate == FILE_MODE_CREATE )
    {
        tree.pop_back();

        _CreateDirTree( tree );
    }

#ifdef _WIN32
    DWORD flagAttr = 0;

    if ( flags & FILE_FLAG_TEMPORARY )
        flagAttr |= FILE_FLAG_DELETE_ON_CLOSE | FILE_ATTRIBUTE_TEMPORARY;

    if ( flags & FILE_FLAG_UNBUFFERED )
        flagAttr |= FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH;

    HANDLE sysHandle = CreateFile( output.c_str(), dwAccess, (flags & FILE_FLAG_WRITESHARE) != 0 ? FILE_SHARE_READ | FILE_SHARE_WRITE : FILE_SHARE_READ, NULL, dwCreate, flagAttr, NULL );

    if ( sysHandle == INVALID_HANDLE_VALUE )
        return NULL;

    pFile = new CRawFile();
    pFile->m_file = sysHandle;
#elif defined(__linux__)
    const char *openMode;

    // TODO: support flags parameter.

    if ( dwCreate == FILE_MODE_CREATE )
    {
        if ( dwAccess & FILE_ACCESS_READ )
            openMode = "w+";
        else
            openMode = "w";
    }
    else if ( dwCreate == FILE_MODE_OPEN )
    {
        if ( dwAccess & FILE_ACCESS_WRITE )
            openMode = "r+";
        else
            openMode = "r";
    }
    else
        return NULL;

    FILE *filePtr = fopen( output.c_str(), openMode );

    if ( !filePtr )
        return NULL;

    pFile = new CRawFile();
    pFile->m_file = filePtr;
#else
    return NULL;
#endif //OS DEPENDANT CODE

    // Write shared file properties.
    pFile->m_access = dwAccess;
    pFile->m_path = output;

    if ( *mode == 'a' )
        pFile->Seek( 0, SEEK_END );

    openFiles->push_back( pFile );
    return pFile;
}

CFile* CSystemFileTranslator::Open( const char *path, const char *mode )
{
    return OpenEx( path, mode, 0 );
}

bool CSystemFileTranslator::Exists( const char *path ) const
{
    filePath output;
    struct stat tmp;

    if ( !GetFullPath( path, true, output ) )
        return false;

    // The C API cannot cope with trailing slashes
    size_t outSize = output.size();

    if ( outSize && output[--outSize] == '/' )
        output.resize( outSize );

    return stat( output.c_str(), &tmp ) == 0;
}

inline bool _deleteFile( const char *path )
{
#ifdef _WIN32
    return DeleteFile( path ) != FALSE;
#elif defined(__linux__)
    return unlink( path ) == 0;
#else
    return false;
#endif //OS DEPENDANT CODE
}

static void _deleteFileCallback( const filePath& path, void *ud )
{
    _deleteFile( path.c_str() );
}

inline bool _deleteDir( const char *path )
{
#ifdef _WIN32
    return RemoveDirectory( path ) != FALSE;
#elif defined(__linux__)
    return rmdir( path ) == 0;
#else
    return false;
#endif //OS DEPENDANT CODE
}

static void _deleteDirCallback( const filePath& path, void *ud )
{
    // Delete all subdirectories too.
    ((CSystemFileTranslator*)ud)->ScanDirectory( path, "*", false, _deleteDirCallback, _deleteFileCallback, ud );

    _deleteDir( path.c_str() );
}

bool CSystemFileTranslator::Delete( const char *path )
{
    filePath output;

    if ( !GetFullPath( path, true, output ) )
        return false;

    if ( output.at( output.size() - 1 ) == '/' )
    {
        if ( !File_IsDirectoryAbsolute( output.c_str() ) )
            return false;

        // Remove all files and directories inside
        ScanDirectory( output.c_str(), "*", false, _deleteDirCallback, _deleteFileCallback, this );
        return _deleteDir( output.c_str() );
    }

    return _deleteFile( output.c_str() );
}

inline bool _File_Copy( const char *src, const char *dst )
{
#ifdef _WIN32
    return CopyFile( src, dst, false ) != FALSE;
#elif defined(__linux__)
    int iReadFile = open( src, O_RDONLY, 0 );

    if ( iReadFile == -1 )
        return false;

    int iWriteFile = open( dst, O_CREAT | O_WRONLY | O_ASYNC, FILE_ACCESS_FLAG );

    if ( iWriteFile == -1 )
        return false;

    struct stat read_info;
    if ( fstat( iReadFile, &read_info ) != 0 )
    {
        close( iReadFile );
        close( iWriteFile );
        return false;
    }

    sendfile( iWriteFile, iReadFile, NULL, read_info.st_size );

    close( iReadFile );
    close( iWriteFile );
    return true;
#else
    return false;
#endif //OS DEPENDANT CODE
}

bool CSystemFileTranslator::Copy( const char *src, const char *dst )
{
    filePath source;
    filePath target;
    dirTree dstTree;
    bool file;

    if ( !GetFullPath( src, true, source ) || !GetRelativePathTreeFromRoot( dst, dstTree, file ) || !file )
        return false;

    // We always start from root
    target = m_root;

    _File_OutputPathTree( dstTree, true, target );

    // Make sure dir exists
    dstTree.pop_back();
    _CreateDirTree( dstTree );

    // Copy data using quick kernel calls.
    return _File_Copy( source.c_str(), target.c_str() );
}

bool CSystemFileTranslator::Rename( const char *src, const char *dst )
{
    filePath source;
    filePath target;
    dirTree dstTree;
    bool file;

    if ( !GetFullPath( src, true, source ) || !GetRelativePathTreeFromRoot( dst, dstTree, file ) || !file )
        return false;

    // We always start from root
    target = m_root;

    _File_OutputPathTree( dstTree, true, target );

    // Make sure dir exists
    dstTree.pop_back();
    _CreateDirTree( dstTree );

#ifdef _WIN32
    return MoveFile( source.c_str(), target.c_str() ) != FALSE;
#elif defined(__linux__)
    return rename( source.c_str(), target.c_str() ) == 0;
#else
    return false;
#endif //OS DEPENDANT CODE
}

bool CSystemFileTranslator::Stat( const char *path, struct stat *stats ) const
{
    filePath output;

    if ( !GetFullPath( path, true, output ) )
        return false;

    return stat( output.c_str(), stats ) == 0;
}

size_t CSystemFileTranslator::Size( const char *path ) const
{
    struct stat fstats;

    if ( !Stat( path, &fstats ) )
        return 0;

    return fstats.st_size;
}

bool CSystemFileTranslator::ReadToBuffer( const char *path, std::vector <char>& output ) const
{
    filePath sysPath;

    if ( !GetFullPath( path, true, sysPath ) )
        return false;

    return fileSystem->ReadToBuffer( sysPath.c_str(), output );
}

// Handle absolute paths.

bool CSystemFileTranslator::GetRelativePathTreeFromRoot( const char *path, dirTree& tree, bool& file ) const
{
    if ( _File_IsAbsolutePath( path ) )
    {
#ifdef _WIN32
        if ( !_File_PathCharComp( path[0], m_root[0] ) )
            return false;   // drive mismatch

        return _File_ParseRelativeTree( path + 3, m_rootTree, tree, file );
#else
        return _File_ParseRelativeTree( path + 1, m_rootTree, tree, file );
#endif //OS DEPENDANT CODE
    }

    return CSystemPathTranslator::GetRelativePathTreeFromRoot( path, tree, file );
}

bool CSystemFileTranslator::GetRelativePathTree( const char *path, dirTree& tree, bool& file ) const
{
    if ( _File_IsAbsolutePath( path ) )
    {
#ifdef _WIN32
        if ( !_File_PathCharComp( path[0], m_root[0] ) )
            return false;   // drive mismatch

        return _File_ParseRelativeTreeDeriviate( path + 3, m_rootTree, m_curDirTree, tree, file );
#else
        return _File_ParseRelativeTreeDeriviate( path + 1, m_rootTree, m_curDirTree, tree, file );
#endif //OS DEPENDANT CODE
    }

    return CSystemPathTranslator::GetRelativePathTree( path, tree, file );
}

bool CSystemFileTranslator::GetFullPathTree( const char *path, dirTree& tree, bool& file ) const
{
    if ( _File_IsAbsolutePath( path ) )
    {
#ifdef _WIN32
        if ( !_File_PathCharComp( path[0], m_root[0] ) )
            return false;   // drive mismatch

        tree = m_rootTree;
        return _File_ParseRelativeTree( path + 3, m_rootTree, tree, file );
#else
        tree = m_rootTree;
        return _File_ParseRelativeTree( path + 1, m_rootTree, tree, file );
#endif //OS DEPENDANT CODE
    }

    return CSystemPathTranslator::GetFullPathTree( path, tree, file );
}

bool CSystemFileTranslator::GetFullPath( const char *path, bool allowFile, filePath& output ) const
{
    if ( !CSystemPathTranslator::GetFullPath( path, allowFile, output ) )
        return false;

#ifdef _WIN32
    output.insert( 0, m_root.c_str(), 3 );
#else
    output.insert( 0, "/", 1 );
#endif //_WIN32
    return true;
}

bool CSystemFileTranslator::ChangeDirectory( const char *path )
{
    dirTree tree;
    filePath absPath;
    bool file;

    if ( !GetRelativePathTreeFromRoot( path, tree, file ) )
        return false;

    if ( file )
        tree.pop_back();

    absPath = m_root;
    _File_OutputPathTree( tree, false, absPath );

#ifdef _WIN32
    HANDLE dir = CreateFile( absPath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL );

    if ( dir == INVALID_HANDLE_VALUE )
        return false;

    if ( m_curDirHandle )
        CloseHandle( m_curDirHandle );

    m_curDirHandle = dir;
#elif defined(__linux__)
    DIR *dir = opendir( absPath.c_str() );

    if ( dir == NULL )
        return false;

    if ( m_curDirHandle )
        closedir( m_curDirHandle );

    m_curDirHandle = dir;
#else
    if ( !File_IsDirectoryAbsolute( absPath.c_str() ) )
        return false;
#endif //OS DEPENDANT CODE

    m_currentDir.clear();
    _File_OutputPathTree( tree, false, m_currentDir );

    m_curDirTree = tree;
    return true;
}

void CSystemFileTranslator::ScanDirectory( const char *directory, const char *wildcard, bool recurse,
                                            pathCallback_t dirCallback,
                                            pathCallback_t fileCallback,
                                            void *userdata ) const
{
    filePath            output;
    char				wcard[256];

    if ( !GetFullPath( directory, false, output ) )
        return;

    if ( !wildcard )
        strcpy(wcard, "*");
    else
        strncpy(wcard, wildcard, 255);

#ifdef _WIN32
    WIN32_FIND_DATA		finddata;
    HANDLE				handle;

    try
    {
        //first search for files only
        if ( fileCallback )
        {
            std::string query = std::string( output.c_str(), output.size() );
            query += wcard;

            // I am unsure whether ".." could turn dangerous here (wcard)
            // My tests indicated that Windows secures against uprooting(!)

            handle = FindFirstFile( query.c_str(), &finddata );

            if ( handle != INVALID_HANDLE_VALUE )
            {
                do
                {
                    if ( finddata.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY | FILE_ATTRIBUTE_DIRECTORY) )
                        continue;

                    filePath filename = output;
                    filename += finddata.cFileName;

                    fileCallback( filename.c_str(), userdata );

                } while ( FindNextFile(handle, &finddata) );

                FindClose( handle );
            }
        }

        if ( !dirCallback && !recurse )
            goto endJump;

        //next search for subdirectories only
        std::string query = std::string( output.c_str(), output.size() );
        query += '*';

        handle = FindFirstFile( query.c_str(), &finddata );

        if ( handle == INVALID_HANDLE_VALUE )
            goto endJump;

        do
        {
            if ( finddata.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY) )
                continue;

            if ( !(finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
                continue;

            // Optimization :)
            if ( _File_IgnoreDirectoryScanEntry( finddata.cFileName ) )
                continue;

            filePath target = output;
            target += finddata.cFileName;
            target += '/';

            if ( dirCallback )
                dirCallback( target.c_str(), userdata );

            if ( recurse )
                ScanDirectory( target.c_str(), wcard, true, dirCallback, fileCallback, userdata );

        } while ( FindNextFile(handle, &finddata) );
    }
    catch( ... )
    {
        // Callbacks may throw exceptions
        FindClose( handle );
        throw;
    }

endJump:
    FindClose( handle );
#elif defined(__linux__)
    DIR *findDir = opendir( output.c_str() );

    if ( !findDir )
        return;

    filePattern_t *pattern = _File_CreatePattern( wildcard );

    try
    {
        //first search for files only
        if ( fileCallback )
        {
            while ( struct dirent *entry = readdir( findDir ) )
            {
                filePath path = output;
                path += entry->d_name;

                struct stat entry_stats;

                if ( stat( path.c_str(), &entry_stats ) == 0 )
                {
                    if ( !( S_ISDIR( entry_stats.st_mode ) ) && _File_MatchPattern( entry->d_name, pattern ) )
                    {
                        fileCallback( path.c_str(), userdata );
                    }
                }
            }
        }

        rewinddir( findDir );

        if ( dirCallback || recurse )
        {
            //next search for subdirectories only
            while ( struct dirent *entry = readdir( findDir ) )
            {
                const char *name = entry->d_name;

                if ( _File_IgnoreDirectoryScanEntry( name ) )
                    continue;

                filePath path = output;
                path += name;
                path += '/';

                struct stat entry_info;

                if ( stat( path.c_str(), &entry_info ) == 0 && S_ISDIR( entry_info.st_mode ) )
                {
                    if ( dirCallback && _File_MatchPattern( entry->d_name, pattern ) )
                    {
                        dirCallback( path.c_str(), userdata );
                    }

                    // TODO: this can be optimized by reusing the pattern structure.
                    if ( recurse )
                        ScanDirectory( path.c_str(), wcard, recurse, dirCallback, fileCallback, userdata );
                }
            }
        }
    }
    catch( ... )
    {
        // Callbacks may throw exceptions
        _File_DestroyPattern( pattern );

        closedir( findDir );
        throw;
    }

    _File_DestroyPattern( pattern );

    closedir( findDir );
#endif //OS DEPENDANT CODE
}

static void _scanFindCallback( const filePath& path, std::vector <filePath> *output )
{
    output->push_back( path );
}

void CSystemFileTranslator::GetDirectories( const char *path, const char *wildcard, bool recurse, std::vector <filePath>& output ) const
{
    ScanDirectory( path, wildcard, recurse, (pathCallback_t)_scanFindCallback, NULL, &output );
}

void CSystemFileTranslator::GetFiles( const char *path, const char *wildcard, bool recurse, std::vector <filePath>& output ) const
{
    ScanDirectory( path, wildcard, recurse, NULL, (pathCallback_t)_scanFindCallback, &output );
}

/*=======================================
    CFileSystem

    Management class with root-access functions.
    These methods are root-access. Exposing them
    to a security-critical user-space context is
    not viable.
=======================================*/

#ifdef _WIN32

struct MySecurityAttributes
{
    DWORD count;
    LUID_AND_ATTRIBUTES attr[2];
};

#endif //_WIN32

CFileSystem::CFileSystem( void )
{
    // We should set special priviledges for the application if
    // running under Win32.
#ifdef _WIN32
    HANDLE token;
    MySecurityAttributes priv;

    // We need SE_BACKUP_NAME to gain directory access on Windows
    if ( !OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token ) )
        throw std::exception( "failed to adjust fileSystem privileges" );

    priv.count = 2;

    if ( !LookupPrivilegeValue( NULL, SE_BACKUP_NAME, &priv.attr[0].Luid ) )
        throw std::exception( "failed to lookup privilege UID" );

    priv.attr[0].Attributes = SE_PRIVILEGE_ENABLED;

    if ( !LookupPrivilegeValue( NULL, SE_RESTORE_NAME, &priv.attr[1].Luid ) )
        throw std::exception( "failed to lookup privilege UID" );

    priv.attr[1].Attributes = SE_PRIVILEGE_ENABLED;

    if ( !AdjustTokenPrivileges( token, false, (TOKEN_PRIVILEGES*)&priv, sizeof( priv ), NULL, NULL ) )
        throw std::exception( "lacking administrator privileges for fileSystem" );

    CloseHandle( token );
#endif //_WIN32

    char cwd[1024];
    getcwd( cwd, 1023 );

    // Make sure it is a correct directory
    filePath cwd_ex = cwd;
    cwd_ex += '\\';

    openFiles = new std::list<CFile*>;

    // Every application should be able to access itself
    fileRoot = CreateTranslator( cwd_ex );

    // Init Addons
    InitZIP();

    fileSystem = this;
}

CFileSystem::~CFileSystem( void )
{
    DestroyZIP();

    delete openFiles;
}

CFileTranslator* CFileSystem::CreateTranslator( const char *path )
{
    CSystemFileTranslator *pTranslator;
    filePath root;
    dirTree tree;
    bool bFile;

#ifdef _WIN32
    // We have to handle absolute path, too
    if ( _File_IsAbsolutePath( path ) )
    {
        if (!_File_ParseRelativePath( path + 3, tree, bFile ))
            return NULL;

        root += path[0];
        root += ":/";
    }
#elif defined(__linux__)
    if ( *path == '/' || *path == '\\' )
    {
        if (!_File_ParseRelativePath( path + 1, tree, bFile ))
            return NULL;

        root = "/";
    }
#endif //OS DEPENDANT CODE
    else
    {
        char pathBuffer[1024];
        getcwd( pathBuffer, sizeof(pathBuffer) );

        root = pathBuffer;
        root += "\\";
        root += path;

#ifdef _WIN32
        if (!_File_ParseRelativePath( root.c_str() + 3, tree, bFile ))
            return NULL;

        root.resize( 2 );
        root += "/";
#elif defined(__linux__)
        if (!_File_ParseRelativePath( root.c_str() + 1, tree, bFile ))
            return NULL;

        root = "/";
#endif //OS DEPENDANT CODE
    }

    if ( bFile )
        tree.pop_back();

    _File_OutputPathTree( tree, false, root );

#ifdef _WIN32
    HANDLE dir = CreateFile( root.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL );

    if ( dir == INVALID_HANDLE_VALUE )
        return NULL;
#elif defined(__linux__)
    DIR *dir = opendir( root.c_str() );

    if ( dir == NULL )
        return NULL;
#else
    if ( !IsDirectory( root.c_str() ) )
        return NULL;
#endif //OS DEPENDANT CODE

    pTranslator = new CSystemFileTranslator();
    pTranslator->m_root = root;
    pTranslator->m_rootTree = tree;

#ifdef _WIN32
    pTranslator->m_rootHandle = dir;
    pTranslator->m_curDirHandle = NULL;
#elif defined(__linux__)
    pTranslator->m_rootHandle = dir;
    pTranslator->m_curDirHandle = NULL;
#endif //OS DEPENDANT CODE

    return pTranslator;
}

bool CFileSystem::IsDirectory( const char *path )
{
    return File_IsDirectoryAbsolute( path );
}

#ifdef _WIN32

// By definition, crash dumps are OS dependant.
// Currently we only support crash dumps on Windows OS.

bool CFileSystem::WriteMiniDump( const char *path, _EXCEPTION_POINTERS *except )
{
#if 0
    CRawFile *file = (CRawFile*)fileRoot->Open( path, "wb" );
    MINIDUMP_EXCEPTION_INFORMATION info;

    if ( !file )
        return false;

    // Create an exception information struct
    info.ThreadId = GetCurrentThreadId();
    info.ExceptionPointers = except;
    info.ClientPointers = false;

    // Write the dump
    MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), file->m_file, MiniDumpNormal, &info, NULL, NULL );

    delete file;
#endif

    return true;
}
#endif //_WIN32

bool CFileSystem::Exists( const char *path )
{
    struct stat tmp;

    return stat( path, &tmp ) == 0;
}

size_t CFileSystem::Size( const char *path )
{
    struct stat stats;

    if ( stat( path, &stats ) != 0 )
        return 0;

    return stats.st_size;
}

// Utility to quickly load data from files on the local filesystem.
// Do not export it into user-space since this function has no security restrictions.
bool CFileSystem::ReadToBuffer( const char *path, std::vector <char>& output )
{
#ifdef _WIN32
    HANDLE file = CreateFile( path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );

    if ( file == INVALID_HANDLE_VALUE )
        return false;

    size_t size = GetFileSize( file, NULL );

    if ( size != 0 )
    {
        output.resize( size );
        output.reserve( size );

        DWORD _pf;

        ReadFile( file, &output[0], size, &_pf, NULL );
    }
    else
        output.clear();

    CloseHandle( file );
    return true;
#elif defined(__linux__)
    int iReadFile = open( path, O_RDONLY, 0 );

    if ( iReadFile == -1 )
        return false;

    struct stat read_info;

    if ( fstat( iReadFile, &read_info ) != 0 )
        return false;

    if ( read_info.st_size != 0 )
    {
        output.resize( read_info.st_size );
        output.reserve( read_info.st_size );

        ssize_t numRead = read( iReadFile, &output[0], read_info.st_size );

        if ( numRead == 0 )
        {
            close( iReadFile );
            return false;
        }

        if ( numRead != read_info.st_size )
            output.resize( numRead );
    }
    else
        output.clear();

    close( iReadFile );
    return true;
#else
    return false;
#endif //OS DEPENDANT CODE
}
