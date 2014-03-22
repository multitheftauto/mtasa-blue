/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/core/CFileSystem.h
*  PURPOSE:     File management
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CFileSystem_
#define _CFileSystem_

#ifndef _WIN32
#define FILE_MODE_CREATE    0x01
#define FILE_MODE_OPEN      0x02

#define FILE_ACCESS_WRITE   0x01
#define FILE_ACCESS_READ    0x02

#define MAX_PATH 260
#else
#include <direct.h>

#define FILE_MODE_CREATE    CREATE_ALWAYS
#define FILE_MODE_OPEN      OPEN_EXISTING

#define FILE_ACCESS_WRITE   GENERIC_WRITE
#define FILE_ACCESS_READ    GENERIC_READ
#endif

#ifdef __linux__
#include <unistd.h>
#include <dirent.h>
#endif //__linux__


class CRawFile : public CFile
{
public:
                    ~CRawFile       ( void );

    size_t          Read            ( void *buffer, size_t sElement, unsigned long iNumElements );
    size_t          Write           ( const void *buffer, size_t sElement, unsigned long iNumElements );
    int             Seek            ( long iOffset, int iType );
    long            Tell            ( void ) const;
    bool            IsEOF           ( void ) const;
    bool            Stat            ( struct stat *stats ) const;
    void            PushStat        ( const struct stat *stats );
    void            SetSeekEnd      ( void );
    size_t          GetSize         ( void ) const;
    void            SetSize         ( size_t size );
    void            Flush           ( void );
    const filePath& GetPath         ( void ) const;
    bool            IsReadable      ( void ) const;
    bool            IsWriteable     ( void ) const;

private:
    friend class CSystemFileTranslator;
    friend class CFileSystem;

#ifdef _WIN32
    HANDLE          m_file;
#elif defined(__linux__)
    FILE*           m_file;
#endif //OS DEPENDANT CODE
    DWORD           m_access;
    filePath        m_path;
};

class CBufferedFile : public CFile
{
public:
                    ~CBufferedFile  ( void );

    size_t          Read            ( void *buffer, size_t sElement, unsigned long iNumElements );
    size_t          Write           ( const void *buffer, size_t sElement, unsigned long iNumElements );
    int             Seek            ( long iOffset, int iType );
    long            Tell            ( void ) const;
    bool            IsEOF           ( void ) const;
    bool            Stat            ( struct stat *stats ) const;
    void            PushStat        ( const struct stat *stats );
    void            SetSeekEnd      ( void );
    size_t          GetSize         ( void ) const;
    void            Flush           ( void );
    const filePath& GetPath         ( void ) const;
    bool            IsReadable      ( void ) const;
    bool            IsWriteable     ( void ) const;

    int             ReadInt();
    short           ReadShort();
    char            ReadByte();

private:
    char*           m_pBuffer;
    long            m_iSeek;
    size_t          m_sSize;
    filePath        m_path;
};

class CSystemPathTranslator : public virtual CFileTranslator
{
public:
                    CSystemPathTranslator           ( bool isSystemPath );

    bool            GetFullPathTreeFromRoot         ( const char *path, dirTree& tree, bool& file ) const;
    bool            GetFullPathTree                 ( const char *path, dirTree& tree, bool& file ) const;
    bool            GetRelativePathTreeFromRoot     ( const char *path, dirTree& tree, bool& file ) const;
    bool            GetRelativePathTree             ( const char *path, dirTree& tree, bool& file ) const;
    bool            GetFullPathFromRoot             ( const char *path, bool allowFile, filePath& output ) const;
    bool            GetFullPath                     ( const char *path, bool allowFile, filePath& output ) const;
    bool            GetRelativePathFromRoot         ( const char *path, bool allowFile, filePath& output ) const;
    bool            GetRelativePath                 ( const char *path, bool allowFile, filePath& output ) const;
    bool            ChangeDirectory                 ( const char *path );
    void            GetDirectory                    ( filePath& output ) const;

    inline bool IsTranslatorRootDescriptor( char character ) const
    {
        if ( !m_isSystemPath )
        {
            // On Windows platforms, absolute paths are based on drive letters (C:/).
            // This means we can use the linux way of absolute fs root linkling for the translators.
            // But this may confuse Linux users; on Linux this convention cannot work (reserved for abs paths).
            // Hence, usage of the '/' or '\\' descriptor is discouraged (only for backwards compatibility).
            // Disregard this thought for archive file systems, all file systems which are the root themselves.
            // They use the linux addressing convention.
            if ( character == '/' || character == '\\' )
                return true;
        }

        // Just like MTA:SA is using the '' character for private directories, we use it
        // for a platform independent translator root basing.
        // 'textfile.txt' will address 'textfile.txt' in the translator root, ignoring
        // the current directory setting.
        return character == '' || character == '@';
    }

protected:
    friend class CFileSystem;

    filePath        m_root;
    filePath        m_currentDir;
    dirTree         m_rootTree;
    dirTree         m_curDirTree;

private:
    bool            m_isSystemPath;
};

#define FILE_FLAG_TEMPORARY     0x00000001
#define FILE_FLAG_UNBUFFERED    0x00000002
#define FILE_FLAG_GRIPLOCK      0x00000004
#define FILE_FLAG_WRITESHARE    0x00000008

class CSystemFileTranslator : public CSystemPathTranslator
{
public:
    // We need to distinguish between Windows and other OSes here.
    // Windows uses driveletters (C:/) and Unix + Mac do not.
    // This way, on Windows we can use the '/' or '\\' character
    // to trace paths from the translator root.
    // On Linux and Mac, these characters are for addressing
    // absolute paths.
    CSystemFileTranslator( void ) :
#ifdef _WIN32
        CSystemPathTranslator( false )
#else
        CSystemPathTranslator( true )
#endif //OS DEPENDANT CODE
    { }

                    ~CSystemFileTranslator          ( void );

    bool            WriteData                       ( const char *path, const char *buffer, size_t size );
    bool            CreateDir                       ( const char *path );
    CFile*          Open                            ( const char *path, const char *mode );
    CFile*          OpenEx                          ( const char *path, const char *mode, unsigned int flags );
    bool            Exists                          ( const char *path ) const;
    bool            Delete                          ( const char *path );
    bool            Copy                            ( const char *src, const char *dst );
    bool            Rename                          ( const char *src, const char *dst );
    size_t          Size                            ( const char *path ) const;
    bool            Stat                            ( const char *path, struct stat *stats ) const;
    bool            ReadToBuffer                    ( const char *path, std::vector <char>& output ) const;

    // Used to handle absolute paths
    bool            GetRelativePathTreeFromRoot     ( const char *path, dirTree& tree, bool& file ) const;
    bool            GetRelativePathTree             ( const char *path, dirTree& tree, bool& file ) const;
    bool            GetFullPathTree                 ( const char *path, dirTree& tree, bool& file ) const;
    bool            GetFullPath                     ( const char *path, bool allowFile, filePath& output ) const;

    bool            ChangeDirectory                 ( const char *path );

    void            ScanDirectory( const char *directory, const char *wildcard, bool recurse,
                        pathCallback_t dirCallback,
                        pathCallback_t fileCallback,
                        void *userdata ) const;

    void            GetDirectories                  ( const char *path, const char *wildcard, bool recurse, std::vector <filePath>& output ) const;
    void            GetFiles                        ( const char *path, const char *wildcard, bool recurse, std::vector <filePath>& output ) const;

private:
    friend class CFileSystem;

    void            _CreateDirTree                  ( const dirTree& tree );

#ifdef _WIN32
    HANDLE          m_rootHandle;
    HANDLE          m_curDirHandle;
#elif defined(__linux__)
    DIR*            m_rootHandle;
    DIR*            m_curDirHandle;
#endif //OS DEPENDANT CODE
};

// Include extensions
#include "CFileSystem.zip.h"

class CFileSystem : public CFileSystemInterface
{
public:
                            CFileSystem             ( void );
                            ~CFileSystem            ( void );

    void                    InitZIP                 ( void );
    void                    DestroyZIP              ( void );

    CFileTranslator*        CreateTranslator        ( const char *path );
    CArchiveTranslator*     OpenArchive             ( CFile& file );

    CArchiveTranslator*     CreateZIPArchive        ( CFile& file );

    // Insecure functions
    bool                    IsDirectory             ( const char *path );
#ifdef _WIN32
    bool                    WriteMiniDump           ( const char *path, _EXCEPTION_POINTERS *except );
#endif //_WIN32
    bool                    Exists                  ( const char *path );
    size_t                  Size                    ( const char *path );
    bool                    ReadToBuffer            ( const char *path, std::vector <char>& output );

    // Settings.
    void                    SetIncludeAllDirectoriesInScan  ( bool enable )             { m_includeAllDirsInScan = enable; }
    bool                    GetIncludeAllDirectoriesInScan  ( void ) const              { return m_includeAllDirsInScan; }

    // Members.
    bool                    m_includeAllDirsInScan;     // decides whether ScanDir implementations should apply patterns on directories
};

extern CFileSystem *fileSystem;
extern CFileTranslator *fileRoot;

#endif //_CFileSystem_
