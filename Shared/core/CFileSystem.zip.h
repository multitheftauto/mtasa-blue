/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/core/CFileSystem.zip.h
*  PURPOSE:     ZIP archive filesystem
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _FILESYSTEM_ZIP_
#define _FILESYSTEM_ZIP_

#include <time.h>

#pragma warning(push)
#pragma warning(disable:4250)

class CArchiveFileTranslator : public CSystemPathTranslator, public CArchiveTranslator
{
    friend class CFileSystem;
    friend class CArchiveFile;
public:
                    CArchiveFileTranslator( CFile& file );
                    ~CArchiveFileTranslator();

    bool            WriteData( const char *path, const char *buffer, size_t size );
    bool            CreateDir( const char *path );
    CFile*          Open( const char *path, const char *mode );
    bool            Exists( const char *path ) const;
    bool            Delete( const char *path );
    bool            Copy( const char *src, const char *dst );
    bool            Rename( const char *src, const char *dst );
    size_t          Size( const char *path ) const;
    bool            Stat( const char *path, struct stat *stats ) const;
    bool            ReadToBuffer( const char *path, std::vector <char>& output ) const;

    bool            ChangeDirectory( const char *path );

    void            ScanDirectory( const char *directory, const char *wildcard, bool recurse, 
                        pathCallback_t dirCallback, 
                        pathCallback_t fileCallback, 
                        void *userdata ) const;

    void            GetDirectories( const char *path, const char *wildcard, bool recurse, std::vector <filePath>& output ) const;
    void            GetFiles( const char *path, const char *wildcard, bool recurse, std::vector <filePath>& output ) const;

    void            Save();

    CFile&          m_file;

#pragma pack(1)
    struct _localHeader
    {
        unsigned int    signature;
        unsigned short  version;
        unsigned short  flags;
        unsigned short  compression;
        unsigned short  modTime;
        unsigned short  modDate;
        unsigned int    crc32;

        size_t          sizeCompressed;
        size_t          sizeReal;
        
        unsigned short  nameLen;
        unsigned short  commentLen;
    };
#pragma pack()

private:
    void            ReadFiles( unsigned int count );

    struct file;
    struct directory;

    class stream abstract : public CFile
    {
        friend class CArchiveFileTranslator;
    public:
        stream( CArchiveFileTranslator& zip, file& info, CFile& sysFile ) : m_info( info ), m_archive( zip ), m_sysFile( sysFile )
        {
            info.locks.push_back( this );
        }

        ~stream()
        {
            m_info.locks.remove( this );

            delete &m_sysFile;
        }

        const filePath& GetPath() const
        {
            return m_path;
        }

    private:
        CFile&                      m_sysFile;
        CArchiveFileTranslator&     m_archive;
        file&                       m_info;
        filePath                    m_path;
    };

    struct file
    {
        filePath        name;
        filePath        relPath;
        unsigned short  version;
        unsigned short  reqVersion;
        unsigned short  flags;
        unsigned short  compression;
        unsigned short  modTime;
        unsigned short  modDate;

        unsigned int    crc32;
        size_t          sizeCompressed;
        size_t          sizeReal;

        unsigned short  diskID;
        unsigned short  internalAttr;
        unsigned int    externalAttr;
        size_t          localHeaderOffset;

        std::string     extra;
        std::string     comment;

        bool            archived;
        bool            cached;
        bool            subParsed;
        
        typedef std::list <stream*> lockList;
        lockList        locks;
        directory*      dir;

        inline void SetModTime( const tm& date )
        {
            unsigned short year = date.tm_year;

            if ( date.tm_year > 1980 )
                year -= 1980;
            else if ( date.tm_year > 80 )
                year -= 80;

            modDate = date.tm_mday | ((date.tm_mon + 1) << 5) | (year << 9);
            modTime = date.tm_sec >> 1 | (date.tm_min << 5) | (date.tm_hour << 11);
        }

        inline void GetModTime( tm& date ) const
        {
            date.tm_mday = modDate & 0x1F;
            date.tm_mon = ((modDate & 0x1E0) >> 5) - 1;
            date.tm_year = ((modDate & 0x0FE00) >> 9) + 1980;
            
            date.tm_hour = (modTime & 0xF800) >> 11;
            date.tm_min = (modTime & 0x7E0) >> 5;
            date.tm_sec = (modTime & 0x1F) << 1;

            date.tm_wday = 0;
            date.tm_yday = 0;
        }

        inline void UpdateTime()
        {
            time_t curTime = time( NULL );
            SetModTime( *gmtime( &curTime ) );
        }

        inline void Reset()
        {
#ifdef _WIN32
            version = 10;
#endif //_WIN32
            reqVersion = 0x14;
            flags = 0;
            compression = 8;

            UpdateTime();

            crc32 = 0;
            sizeCompressed = 0;
            sizeReal = 0;
            diskID = 0;
            internalAttr = 0;
            externalAttr = 0;
            
            extra.clear();
            comment.clear();

            archived = false;
            cached = false;
            subParsed = false;
        }

        inline bool IsNative() const
        {
#ifdef _WIN32
            return version == 10;
#endif //_WIN32
        }
    };

    inline void seekFile( const file& info, _localHeader& header );

    // We need to cache data on the disk
    void            Extract( CFile& dstFile, file& info );

    inline const file*  GetFileEntry( const char *path ) const
    {
        dirTree tree;
        bool isFile;

        if ( !GetRelativePathTree( path, tree, isFile ) || !isFile )
            return NULL;

        filePath fileName = tree[ tree.size() - 1 ];
        tree.pop_back();

        const directory *dir = GetDeriviateDir( *m_curDirEntry, tree );

        if ( !dir )
            return NULL;

        return dir->GetFile( fileName );
    }

    class fileDeflate : public stream
    {
        friend class CArchiveFileTranslator;
    public:
                        fileDeflate( CArchiveFileTranslator& zip, file& info, CFile& sysFile );
                        ~fileDeflate();

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
        bool            IsReadable() const;
        bool            IsWriteable() const;

    private:
        inline void     Focus();

        bool            m_writeable;
        bool            m_readable;
    };

    typedef std::list <file*> fileList;

    struct directory
    {
        directory( filePath fileName, filePath path ) : name( fileName ), relPath( path )
        {
        }

        ~directory()
        {
            subDirs::iterator iter = children.begin();

            for ( ; iter != children.end(); iter++ )
                delete *iter;

            fileList::iterator fileIter = files.begin();

            for ( ; fileIter != files.end(); fileIter++ )
                delete *fileIter;
        }

        filePath name;
        filePath relPath;

        typedef std::list <directory*> subDirs;

        fileList files;
        subDirs children;

        directory* parent;

        inline directory*  FindDirectory( const filePath& dirName ) const
        {
            subDirs::const_iterator iter;

            for ( iter = children.begin(); iter != children.end(); iter++ )
            {
                if ( (*iter)->name == dirName )
                    return *iter;
            }

            return NULL;
        }

        inline directory&  GetDirectory( const filePath& dirName )
        {
            directory *dir = FindDirectory( dirName );

            if ( dir )
                return *dir;

            dir = new directory( dirName, relPath + dirName + "/" );
            dir->name = dirName;
            dir->parent = this;

            children.push_back( dir );
            return *dir;
        }

        inline void     PositionFile( file& entry )
        {
            entry.relPath = relPath;
            entry.relPath += entry.name;
        }

        inline file&    AddFile( const filePath& fileName )
        {
            file& entry = *new file;
            entry.name = fileName;
			entry.flags = 0;
            
            PositionFile( entry );

            entry.cached = false;
            entry.subParsed = false;
            entry.archived = false;
            entry.dir = this;

            files.push_back( &entry );
            return entry;
        }

        inline void     UnlinkFile( file& entry )
        {
            files.remove( &entry );
        }

        inline void     MoveTo( file& entry )
        {
            entry.dir->UnlinkFile( entry );

            entry.dir = this;

            files.push_back( &entry );

            PositionFile( entry );
        }

        inline bool     IsLocked()
        {
            fileList::const_iterator iter = files.begin();

            for ( ; iter != files.end(); iter++ )
            {
                if ( !(*iter)->locks.empty() )
                    return true;
            }

            return false;
        }

        inline file*    GetFile( const filePath& fileName )
        {
            fileList::const_iterator iter = files.begin();

            for ( ; iter != files.end(); iter++ )
            {
                if ( (*iter)->name == fileName )
                    return *iter;
            }

            return NULL;
        }

        inline file*    MakeFile( const filePath& fileName )
        {
            file *entry = GetFile( fileName );

            if ( entry )
            {
                if ( !entry->locks.empty() )
                    return NULL;

                entry->name = fileName;
                entry->Reset();
                return entry;
            }

            return &AddFile( fileName );
        }

        inline const file*  GetFile( const filePath& fileName ) const
        {
            fileList::const_iterator iter = files.begin();

            for ( ; iter != files.end(); iter++ )
            {
                if ( (*iter)->name == fileName )
                    return *iter;
            }

            return NULL;
        }

        inline bool     RemoveFile( file& entry )
        {
            if ( !entry.locks.empty() )
                return false;

            delete &entry;

            UnlinkFile( entry );
            return true;
        }

        inline bool     RemoveFile( const filePath& fileName )
        {
            fileList::const_iterator iter = files.begin();

            for ( ; iter != files.end(); iter++ )
            {
                if ( (*iter)->name == fileName )
                    return RemoveFile( **iter );
            }
            return false;
        }
    };

    directory m_root;

    void            CacheDirectory( const directory& dir );
    void            SaveDirectory( directory& dir, size_t& size );
    unsigned int    BuildCentralFileHeaders( const directory& dir, size_t& size );

    inline const directory* GetDirTree( const directory& root, dirTree::const_iterator& iter, dirTree::const_iterator& end ) const
    {
        const directory *curDir = &root;

        for ( ; iter != end; iter++ )
        {
            if ( !( curDir = root.FindDirectory( *iter ) ) )
                return NULL;
        }

        return curDir;
    }

    inline const directory* GetDirTree( const dirTree& tree ) const
    {
        return GetDirTree( m_root, tree.begin(), tree.end() );
    }

    inline const directory*     GetDeriviateDir( const directory& root, const dirTree& tree ) const;
    inline directory&           MakeDeriviateDir( directory& root, const dirTree& tree );

    directory&          _CreateDirTree( directory& root, const dirTree& tree );

    struct extraData
    {
        unsigned int    signature;
        size_t          size;

        //data
    };

    directory*  m_curDirEntry;
    std::string m_comment;

    CFileTranslator*    m_fileRoot;
    CFileTranslator*    m_unpackRoot;
    CFileTranslator*    m_realtimeRoot;

    size_t      m_structOffset;
};

#pragma warning(pop)

#endif //_FILESYSTEM_ZIP_