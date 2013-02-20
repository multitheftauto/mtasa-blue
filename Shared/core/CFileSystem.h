/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/core/CFileSystem.h
*  PURPOSE:     File management
*  DEVELOPERS:  S2Games <http://savage.s2games.com> (historical entry)
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CFileSystem_
#define _CFileSystem_

#include <direct.h>

#ifndef _WIN32
#define FILE_MODE_CREATE    0x01
#define FILE_MODE_OPEN      0x02

#define FILE_ACCESS_WRITE   0x01
#define FILE_ACCESS_READ    0x02
#else
#define FILE_MODE_CREATE    CREATE_ALWAYS
#define FILE_MODE_OPEN      OPEN_EXISTING

#define FILE_ACCESS_WRITE   GENERIC_WRITE
#define FILE_ACCESS_READ    GENERIC_READ
#endif



class CRawFile : public CFile
{
public:
                    ~CRawFile();

    size_t          Read( void *buffer, size_t sElement, unsigned long iNumElements );
    size_t          Write( const void *buffer, size_t sElement, unsigned long iNumElements );
    int             Seek( long iOffset, int iType );
    long            Tell() const;
    bool            IsEOF() const;
    bool            Stat( struct stat *stats ) const;
    void            PushStat( const struct stat *stats );
    void            SetSeekEnd();
    size_t          GetSize() const;
    void            SetSize( size_t size );
    void            Flush();
    const filePath& GetPath() const;
    bool            IsReadable() const;
    bool            IsWriteable() const;

private:
    friend class CSystemFileTranslator;
    friend class CFileSystem;
	
#ifdef _WIN32
    HANDLE          m_file;
    DWORD           m_access;
#endif
    filePath        m_path;
};

class CBufferedFile : public CFile
{
public:
                    ~CBufferedFile();

    size_t          Read( void *buffer, size_t sElement, unsigned long iNumElements );
    size_t          Write( const void *buffer, size_t sElement, unsigned long iNumElements );
    int             Seek( long iOffset, int iType );
    long            Tell() const;
    bool            IsEOF() const;
    bool            Stat( struct stat *stats ) const;
    void            PushStat( const struct stat *stats );
    void            SetSeekEnd();
    size_t          GetSize() const;
    void            Flush();
    const filePath& GetPath() const;
    bool            IsReadable() const;
    bool            IsWriteable() const;

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
    bool            GetFullPathTreeFromRoot( const char *path, dirTree& tree, bool& file ) const;
    bool            GetFullPathTree( const char *path, dirTree& tree, bool& file ) const;
    bool            GetRelativePathTreeFromRoot( const char *path, dirTree& tree, bool& file ) const;
    bool            GetRelativePathTree( const char *path, dirTree& tree, bool& file ) const;
    bool            GetFullPathFromRoot( const char *path, bool allowFile, filePath& output ) const;
    bool            GetFullPath( const char *path, bool allowFile, filePath& output ) const;
    bool            GetRelativePathFromRoot( const char *path, bool allowFile, filePath& output ) const;
    bool            GetRelativePath( const char *path, bool allowFile, filePath& output ) const;
    bool            ChangeDirectory( const char *path );
    void            GetDirectory( filePath& output ) const;

protected:
    friend class CFileSystem;

    filePath        m_root;
    filePath        m_currentDir;
    dirTree         m_rootTree;
    dirTree         m_curDirTree;
};

#define FILE_FLAG_TEMPORARY     0x00000001
#define FILE_FLAG_UNBUFFERED    0x00000002
#define FILE_FLAG_GRIPLOCK      0x00000004
#define FILE_FLAG_WRITESHARE    0x00000008

class CSystemFileTranslator : public CSystemPathTranslator
{
public:
                    ~CSystemFileTranslator();

    bool            WriteData( const char *path, const char *buffer, size_t size );
    bool            CreateDir( const char *path );
    CFile*          Open( const char *path, const char *mode );
    CFile*          OpenEx( const char *path, const char *mode, unsigned int flags );
    bool            Exists( const char *path ) const;
    bool            Delete( const char *path );
    bool            Copy( const char *src, const char *dst );
    bool            Rename( const char *src, const char *dst );
    size_t          Size( const char *path ) const;
    bool            Stat( const char *path, struct stat *stats ) const;
    bool            ReadToBuffer( const char *path, std::vector <char>& output ) const;

#ifdef _WIN32
    bool            GetRelativePathTreeFromRoot( const char *path, dirTree& tree, bool& file ) const;
    bool            GetRelativePathTree( const char *path, dirTree& tree, bool& file ) const;
    bool            GetFullPathTree( const char *path, dirTree& tree, bool& file ) const;
    bool            GetFullPath( const char *path, bool allowFile, filePath& output ) const;
#endif //_WIN32
    bool            ChangeDirectory( const char *path );

    void            ScanDirectory( const char *directory, const char *wildcard, bool recurse, 
                        pathCallback_t dirCallback, 
                        pathCallback_t fileCallback, 
                        void *userdata ) const;

    void            GetDirectories( const char *path, const char *wildcard, bool recurse, std::vector <filePath>& output ) const;
    void            GetFiles( const char *path, const char *wildcard, bool recurse, std::vector <filePath>& output ) const;

private:
    friend class CFileSystem;

    void            _CreateDirTree( const dirTree& tree );

#ifdef _WIN32
    HANDLE          m_rootHandle;
    HANDLE          m_curDirHandle;
#endif
};

// Include extensions
#include "CFileSystem.zip.h"

class CFileSystem : public CFileSystemInterface
{
public:
                            CFileSystem();
                            ~CFileSystem();

    void                    InitZIP();
    void                    DestroyZIP();
                            
    CFileTranslator*        CreateTranslator( const char *path );
    CArchiveTranslator*     OpenArchive( CFile& file );

    CArchiveTranslator*     CreateZIPArchive( CFile& file );

    // Insecure functions
    bool                    IsDirectory( const char *path );
    bool                    WriteMiniDump( const char *path, _EXCEPTION_POINTERS *except );
    bool                    Exists( const char *path );
    size_t                  Size( const char *path );
    bool                    ReadToBuffer( const char *path, std::vector <char>& output );
};

extern CFileTranslator *fileRoot;

#endif //_CFileSystem_