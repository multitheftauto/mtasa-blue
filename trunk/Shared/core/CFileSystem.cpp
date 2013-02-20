/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/core/CFileSystem.cpp
*  PURPOSE:     File management
*  DEVELOPERS:  S2Games <http://savage.s2games.com> (historical entry)
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

std::list <CFile*> *openFiles;

CFileSystem *fileSystem;
CFileTranslator *fileRoot;

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

CRawFile::~CRawFile()
{
#ifdef _WIN32
    CloseHandle( m_file );
#endif

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
#endif
}

size_t CRawFile::Write( const void *pBuffer, size_t sElement, unsigned long iNumElements )
{
#ifdef _WIN32
    DWORD dwBytesWritten;

    if (sElement == 0 || iNumElements == 0)
        return 0;

    WriteFile(m_file, pBuffer, sElement * iNumElements, &dwBytesWritten, NULL);
    return dwBytesWritten / sElement;
#endif
}

int CRawFile::Seek( long iOffset, int iType )
{
#ifdef _WIN32
    if (SetFilePointer(m_file, iOffset, NULL, iType) == INVALID_SET_FILE_POINTER)
        return -1;
    return 0;
#endif
}

long CRawFile::Tell() const
{
#ifdef _WIN32
    return SetFilePointer( m_file, 0, NULL, FILE_CURRENT );
#endif
}

bool CRawFile::IsEOF() const
{
#ifdef _WIN32
    return ( SetFilePointer( m_file, 0, NULL, FILE_CURRENT ) >= GetFileSize( m_file, NULL ) );
#endif
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
#endif
}

inline static void TimetToFileTime( time_t t, LPFILETIME pft )
{
    LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000;
    pft->dwLowDateTime = (DWORD) ll;
    pft->dwHighDateTime = ll >>32;
}

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
#endif //_WIN32
}

void CRawFile::SetSeekEnd()
{
#ifdef _WIN32
    SetEndOfFile( m_file );
#endif //_WIN32
}

size_t CRawFile::GetSize() const
{
#ifdef _WIN32
    return GetFileSize( m_file, NULL );
#endif
}

void CRawFile::SetSize( size_t size )
{
#ifdef _WIN32
    SetFilePointer( m_file, size, NULL, SEEK_SET );

    SetEndOfFile( m_file );
#endif
}

void CRawFile::Flush()
{
#ifdef _WIN32
    FlushFileBuffers( m_file );
#endif
}

const filePath& CRawFile::GetPath() const
{
    return m_path;
}

bool CRawFile::IsReadable() const
{
#ifdef _WIN32
    return ( m_access & GENERIC_READ ) != 0;
#endif
}

bool CRawFile::IsWriteable() const
{
#ifdef _WIN32
    return ( m_access & GENERIC_WRITE ) != 0;
#endif
}

/*=========================================
    CBufferedFile

    Loads a file at open and keeps it in a managed buffer.
=========================================*/

CBufferedFile::~CBufferedFile()
{
}

size_t CBufferedFile::Read( void *pBuffer, size_t sElement, unsigned long iNumElements )
{
    long iReadElements = min( ( m_sSize - m_iSeek ) / sElement, iNumElements );
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

    m_iSeek = max( 0, min( m_iSeek + iOffset, (long)m_sSize ) );
    return 0;
}

long CBufferedFile::Tell() const
{
    return m_iSeek;
}

bool CBufferedFile::IsEOF() const
{
    return ( m_iSeek == m_sSize );
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

}

size_t CBufferedFile::GetSize() const
{
    return m_sSize;
}

void CBufferedFile::SetSeekEnd()
{

}

void CBufferedFile::Flush()
{
    return;
}

const filePath& CBufferedFile::GetPath() const
{
    return m_path;
}

int CBufferedFile::ReadInt()
{
    int iResult;

    if ( ( m_sSize - m_iSeek ) < sizeof(int) )
        return 0;

    iResult = *(int*)( m_pBuffer + m_iSeek );
    m_iSeek += sizeof(int);
    return iResult;
}

short CBufferedFile::ReadShort()
{
    short iResult;

    if ( (m_sSize - m_iSeek) < sizeof(short) )
        return 0;

    iResult = *(short*)(m_pBuffer + m_iSeek);
    m_iSeek += sizeof(short);
    return iResult;
}

char CBufferedFile::ReadByte()
{
    if ( m_sSize == m_iSeek )
        return 0;

    return *(m_pBuffer + m_iSeek++);
}

bool CBufferedFile::IsReadable() const
{
    return true;
}

bool CBufferedFile::IsWriteable() const
{
    return false;
}

/*=======================================
    CSystemPathTranslator

    Filesystem path translation utility
=======================================*/

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

    if ( *path == '/' )
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
    if ( *path == '/' )
        return _File_ParseRelativePath( path + 1, tree, file );

    tree = m_curDirTree;
    return _File_ParseRelativePath( path, tree, file );
}

bool CSystemPathTranslator::GetRelativePathTree( const char *path, dirTree& tree, bool& file ) const
{
    if ( *path == '/' )
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
    switch( *path )
    {
    // Drive letters
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
    case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
    case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
        return ( path[1] == ':' && path[2] != 0 );
    }

    return false;
}

CSystemFileTranslator::~CSystemFileTranslator()
{
#ifdef _WIN32
    if ( m_curDirHandle )
        CloseHandle( m_curDirHandle );

    CloseHandle( m_rootHandle );
#endif
}

bool CSystemFileTranslator::WriteData( const char *path, const char *buffer, size_t size )
{
#ifdef _WIN32
    HANDLE file;
    filePath output = m_root;
    dirTree tree;
    bool isFile;

    if ( !GetRelativePathTreeFromRoot( path, tree, isFile ) || !isFile )
        return false;

    _File_OutputPathTree( tree, true, output );

    // Make sure directory exists
    tree.pop_back();
    _CreateDirTree( tree );

    if ( (file = CreateFile( output.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL )) == INVALID_HANDLE_VALUE )
        return false;

    DWORD numWritten;

    WriteFile( file, buffer, size, &numWritten, NULL );

    CloseHandle( file );
    return numWritten == size;
#endif
}

void CSystemFileTranslator::_CreateDirTree( const dirTree& tree )
{
    dirTree::const_iterator iter;
    filePath path = m_root;

    for ( iter = tree.begin(); iter != tree.end(); iter++ )
    {
        path += *iter;
        path += '/';

#ifdef _WIN32
        CreateDirectory( path.c_str(), NULL );
#else
        mkdir( path.c_str() );
#endif
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
    DWORD dwAccess = 0;
    DWORD dwCreate = 0;
    HANDLE sysHandle;
    bool file;

    if ( !GetRelativePathTreeFromRoot( path, tree, file ) )
        return NULL;

    // We can only open files!
    if ( !file )
        return NULL;

    _File_OutputPathTree( tree, true, output );

    if ( !_File_ParseMode( *this, path, mode, (unsigned int&)dwAccess, (unsigned int&)dwCreate ) )
        return NULL;

    // Creation requires the dir tree!
    if ( dwCreate == CREATE_ALWAYS )
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

    sysHandle = CreateFile( output.c_str(), dwAccess, (flags & FILE_FLAG_WRITESHARE) != 0 ? FILE_SHARE_READ | FILE_SHARE_WRITE : FILE_SHARE_READ, NULL, dwCreate, flagAttr, NULL );

    if ( sysHandle == INVALID_HANDLE_VALUE )
        return NULL;

    pFile = new CRawFile();
    pFile->m_file = sysHandle;
    pFile->m_access = dwAccess;
    pFile->m_path = output;
#endif //_WIN32

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

static void _deleteFileCallback( const filePath& path, void *ud )
{
#ifdef _WIN32
    DeleteFile( path );
#endif
}

static void _deleteDirCallback( const filePath& path, void *ud )
{
    ((CSystemFileTranslator*)ud)->ScanDirectory( path, "*", false, _deleteDirCallback, _deleteFileCallback, ud );

#ifdef _WIN32
    RemoveDirectory( path );
#endif
}

bool CSystemFileTranslator::Delete( const char *path )
{
    filePath output;

    if ( !GetFullPath( path, true, output ) )
        return false;

#ifdef _WIN32
    if ( output.at( output.size() - 1 ) == '/' )
    {
        if ( !File_IsDirectoryAbsolute( output.c_str() ) )
            return false;

        // Remove all files and directories inside
        ScanDirectory( output.c_str(), "*", false, _deleteDirCallback, _deleteFileCallback, this );
        return RemoveDirectory( output.c_str() ) != FALSE;
    }

    return DeleteFile( output.c_str() ) != FALSE;
#endif
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

#ifdef _WIN32
    return CopyFile( source.c_str(), target.c_str(), false ) != FALSE;
#endif
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
#endif
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

#ifdef _WIN32

bool CSystemFileTranslator::GetRelativePathTreeFromRoot( const char *path, dirTree& tree, bool& file ) const
{
    if ( _File_IsAbsolutePath( path ) )
    {
        if ( !_File_PathCharComp( path[0], m_root[0] ) )
            return false;   // drive mismatch
        
        return _File_ParseRelativeTree( path + 3, m_rootTree, tree, file );
    }

    return CSystemPathTranslator::GetRelativePathTreeFromRoot( path, tree, file );
}

bool CSystemFileTranslator::GetRelativePathTree( const char *path, dirTree& tree, bool& file ) const
{
    if ( _File_IsAbsolutePath( path ) )
    {
        if ( !_File_PathCharComp( path[0], m_root[0] ) )
            return false;   // drive mismatch
        
        return _File_ParseRelativeTreeDeriviate( path + 3, m_rootTree, m_curDirTree, tree, file );
    }

    return CSystemPathTranslator::GetRelativePathTree( path, tree, file );
}

bool CSystemFileTranslator::GetFullPathTree( const char *path, dirTree& tree, bool& file ) const
{
    if ( _File_IsAbsolutePath( path ) )
    {
        if ( !_File_PathCharComp( path[0], m_root[0] ) )
            return false;   // drive mismatch

        tree = m_rootTree;
        return _File_ParseRelativeTree( path + 3, m_rootTree, tree, file );
    }

    return CSystemPathTranslator::GetFullPathTree( path, tree, file );
}

bool CSystemFileTranslator::GetFullPath( const char *path, bool allowFile, filePath& output ) const
{
    if ( !CSystemPathTranslator::GetFullPath( path, allowFile, output ) )
        return false;

    output.insert( 0, m_root.c_str(), 3 );
    return true;
}

#endif //_WIN32

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
#else
    if ( !File_IsDirectoryAbsolute( absPath.c_str() ) )
        return false;
#endif //_WIN32

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
#ifdef _WIN32
    WIN32_FIND_DATA		finddata;
    HANDLE				handle;
    filePath            output;
    std::string         query;
    char				wcard[256];

    if ( !GetFullPath( directory, false, output ) )
        return;

    if ( !wildcard )
        strcpy(wcard, "*");
    else
        strncpy(wcard, wildcard, 255);

    try
    {
        //first search for files only
        if ( fileCallback )
        {
            query = std::string( output.c_str(), output.size() );
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
            return;

        //next search for subdirectories only
        query = std::string( output.c_str(), output.size() );
        query += '*';

        handle = FindFirstFile( query.c_str(), &finddata );

        if ( handle == INVALID_HANDLE_VALUE )
            return;

        do
        {
            if ( finddata.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY) )
                continue;

            if ( !(finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
                continue;
    		
            // Optimization :)
            if ( *(unsigned short*)finddata.cFileName == 0x002E || (*(unsigned short*)finddata.cFileName == 0x2E2E && *(unsigned char*)(finddata.cFileName + 2) == 0x00) )
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

    FindClose( handle );
#endif
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

    Management class
=======================================*/

#ifdef _WIN32

struct MySecurityAttributes
{
    DWORD count;
    LUID_AND_ATTRIBUTES attr[2];
};

#endif

CFileSystem::CFileSystem()
{
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
#endif

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

CFileSystem::~CFileSystem()
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

    if ( !*path )
        return NULL;

    // We have to handle absolute path, too
    if ( _File_IsAbsolutePath( path ) )
    {
        if (!_File_ParseRelativePath( path + 3, tree, bFile ))
            return NULL;

        root += path[0];
        root += ":/";
    }
    else
    {
        char pathBuffer[1024];
        GetCurrentDirectory( 1024, pathBuffer );

        root = pathBuffer;
        root += "\\";
        root += path;

        if (!_File_ParseRelativePath( root.c_str() + 3, tree, bFile ))
            return NULL;

        root.resize( 2 );
        root += "/";
    }

    if ( bFile )
        tree.pop_back();

    _File_OutputPathTree( tree, false, root );

#ifdef _WIN32
    HANDLE dir = CreateFile( root.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL );

    if ( dir == INVALID_HANDLE_VALUE )
        return NULL;
#else
    if ( !IsDirectory( root.c_str() ) )
        return NULL;
#endif //_WIN32

    pTranslator = new CSystemFileTranslator();
    pTranslator->m_root = root;
    pTranslator->m_rootTree = tree;

#ifdef _WIN32
    pTranslator->m_rootHandle = dir;
    pTranslator->m_curDirHandle = NULL;
#endif

    return pTranslator;
}

bool CFileSystem::IsDirectory( const char *path )
{
    return File_IsDirectoryAbsolute( path );
}

bool CFileSystem::WriteMiniDump( const char *path, _EXCEPTION_POINTERS *except )
{
#if 0
#ifdef _WIN32
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
#endif
    return true;
}

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

bool CFileSystem::ReadToBuffer( const char *path, std::vector <char>& output )
{
#ifdef _WIN32
    HANDLE file = CreateFile( path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );
    size_t size;
    DWORD _pf;

    if ( file == INVALID_HANDLE_VALUE )
        return false;

    size = GetFileSize( file, NULL );

    if ( size != 0 )
    {
        output.resize( size );
        output.reserve( size );

        ReadFile( file, &output[0], size, &_pf, NULL );
    }
    else
        output.clear();

    CloseHandle( file );
    return true;
#endif
}