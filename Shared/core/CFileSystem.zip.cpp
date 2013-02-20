/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/core/CFileSystem.zip.cpp
*  PURPOSE:     ZIP archive filesystem
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Docu: https://users.cs.jmu.edu/buchhofp/forensics/formats/pkzip.html
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include <zlib.h>

#define ZIP_SIGNATURE               0x06054B50
#define ZIP_FILE_SIGNATURE          0x02014B50
#define ZIP_LOCAL_FILE_SIGNATURE    0x04034B50

static CFileTranslator *sysTmp;
static CFileTranslator *sysTmpRoot;
static unsigned int sysTmpCnt = 0;

extern CFileSystem *fileSystem;

#include "CFileSystem.Utils.hxx"

/*=======================================
    CArchiveFile

    ZIP file seeking and handling
=======================================*/

CArchiveFileTranslator::fileDeflate::fileDeflate( CArchiveFileTranslator& zip, file& info, CFile& sysFile ) : stream( zip, info, sysFile )
{
}

CArchiveFileTranslator::fileDeflate::~fileDeflate()
{
}

size_t CArchiveFileTranslator::fileDeflate::Read( void *buffer, size_t sElement, unsigned long iNumElements )
{
    if ( !m_readable )
        return 0;

    return m_sysFile.Read( buffer, sElement, iNumElements );
}

size_t CArchiveFileTranslator::fileDeflate::Write( const void *buffer, size_t sElement, unsigned long iNumElements )
{
    if ( !m_writeable )
        return 0;

    m_info.UpdateTime();
    return m_sysFile.Write( buffer, sElement, iNumElements );
}

int CArchiveFileTranslator::fileDeflate::Seek( long iOffset, int iType )
{
    return m_sysFile.Seek( iOffset, iType );
}

long CArchiveFileTranslator::fileDeflate::Tell() const
{
    return m_sysFile.Tell();
}

bool CArchiveFileTranslator::fileDeflate::IsEOF() const
{
    return m_sysFile.IsEOF();
}

bool CArchiveFileTranslator::fileDeflate::Stat( struct stat *stats ) const
{
    tm date;

    m_info.GetModTime( date );

    date.tm_year -= 1900;

    stats->st_mtime = stats->st_atime = stats->st_ctime = mktime( &date );
    return true;
}

void CArchiveFileTranslator::fileDeflate::PushStat( const struct stat *stats )
{
    tm *date = gmtime( &stats->st_mtime );
    
    m_info.SetModTime( *date );
}

void CArchiveFileTranslator::fileDeflate::SetSeekEnd()
{
    
}

size_t CArchiveFileTranslator::fileDeflate::GetSize() const
{
    return m_sysFile.GetSize();
}

void CArchiveFileTranslator::fileDeflate::Flush()
{
    m_sysFile.Flush();
}

bool CArchiveFileTranslator::fileDeflate::IsReadable() const
{
    return m_readable;
}

bool CArchiveFileTranslator::fileDeflate::IsWriteable() const
{
    return m_writeable;
}

/*=======================================
    CArchiveFileTranslator

    ZIP translation utility
=======================================*/

CArchiveFileTranslator::CArchiveFileTranslator( CFile& file ) : m_file( file ), m_root( filePath(), filePath() )
{
    filePath path;
    std::stringstream stream;

    // TODO: Get real .zip structure offset
    m_structOffset = 0;

    // Current directory starts at root
    m_curDirEntry = &m_root;
    m_root.parent = NULL;

    // Create temporary storage
    stream << sysTmpCnt++;

    std::string dirName( "/" );
    dirName += stream.str();
    dirName += "/"; 

    sysTmpRoot->CreateDir( dirName.c_str() );
    sysTmpRoot->GetFullPathFromRoot( dirName.c_str(), false, path );

    m_fileRoot = fileSystem->CreateTranslator( path.c_str() );
    m_fileRoot->CreateDir( "unpack/" );
    m_fileRoot->CreateDir( "realtime/" );

    m_unpackRoot = fileSystem->CreateTranslator( path + "unpack/" );
    m_realtimeRoot = fileSystem->CreateTranslator( path + "realtime/" );
}

CArchiveFileTranslator::~CArchiveFileTranslator()
{
    filePath path;
    m_fileRoot->GetFullPath( "/", false, path );

    delete m_unpackRoot;
    delete m_realtimeRoot;
    delete m_fileRoot;

    // Delete all temporary files
    sysTmpRoot->Delete( path.c_str() );
}

inline const CArchiveFileTranslator::directory* CArchiveFileTranslator::GetDeriviateDir( const directory& root, const dirTree& tree ) const
{
    const directory *curDir = &root;
    dirTree::const_iterator iter = tree.begin();

    for ( ; iter != tree.end() && ( *iter == _dirBack ); iter++ )
    {
        curDir = curDir->parent;

        if ( !curDir )
            return NULL;
    }

    return GetDirTree( *curDir, iter, tree.end() );
}

inline CArchiveFileTranslator::directory& CArchiveFileTranslator::MakeDeriviateDir( directory& root, const dirTree& tree )
{
    directory *curDir = &root;
    dirTree::const_iterator iter = tree.begin();

    for ( ; iter != tree.end() && ( *iter == _dirBack ); iter++ )
        curDir = curDir->parent;

    for ( ; iter != tree.end(); iter++ )
        curDir = &curDir->GetDirectory( *iter );

    return *curDir;
}

bool CArchiveFileTranslator::WriteData( const char *path, const char *buffer, size_t size )
{
    return false;
}

bool CArchiveFileTranslator::CreateDir( const char *path )
{
    dirTree tree;
    bool file;

    if ( !GetRelativePathTree( path, tree, file ) )
        return false;

    if ( file )
        tree.pop_back();

    _CreateDirTree( *m_curDirEntry, tree );
    return true;
}

CFile* CArchiveFileTranslator::Open( const char *path, const char *mode )
{
    dirTree tree;
    bool isFile;

    if ( !GetRelativePathTree( path, tree, isFile ) || !isFile )
        return NULL;

    filePath name = tree[ tree.size() - 1 ];
    tree.pop_back();

    unsigned int m;
    unsigned int access;

    if ( !_File_ParseMode( *this, path, mode, access, m ) )
        return NULL;

    directory *dir;
    file *entry;
    CFile *dstFile;

    // We require the offset path, too
    filePath relPath;
    GetRelativePathFromRoot( path, true, relPath );

    switch( m )
    {
    case FILE_MODE_OPEN:
        dir = (directory*)GetDeriviateDir( *m_curDirEntry, tree );

        if ( !dir )
            return NULL;

        entry = dir->GetFile( name );

        if ( !entry )
            return NULL;

        if ( entry->archived && !entry->cached )
        {
            dstFile = m_realtimeRoot->Open( relPath.c_str(), "wb+" );

            Extract( *dstFile, *entry );
            entry->cached = true;
        }
        else
            dstFile = m_realtimeRoot->Open( relPath.c_str(), "rb+" );

        break;
    case FILE_MODE_CREATE:
        entry = MakeDeriviateDir( *m_curDirEntry, tree ).MakeFile( name );

        if ( !entry )
            return NULL;

		// Files start off clean
		entry->Reset();

        dstFile = m_realtimeRoot->Open( relPath.c_str(), "wb+" );

        entry->cached = true;
        break;
    }

    // Seek it
    dstFile->Seek( 0, *mode == 'a' ? SEEK_END : SEEK_SET );

    fileDeflate *f = new fileDeflate( *this, *entry, *dstFile );
    f->m_path = relPath;
    f->m_writeable = ( access & FILE_ACCESS_WRITE ) != 0;
    f->m_readable = ( access & FILE_ACCESS_READ ) != 0;

    // Update the stat
    struct stat info;
    f->Stat( &info );
    dstFile->PushStat( &info );

    return f;
}

bool CArchiveFileTranslator::Exists( const char *path ) const
{
    dirTree tree;
    bool isFile;

    if ( !GetRelativePathTree( path, tree, isFile ) )
        return false;

    if ( isFile )
    {
        filePath name = tree[ tree.size() - 1 ];
        tree.pop_back();

        const directory *dir = GetDeriviateDir( *m_curDirEntry, tree );
        return dir && dir->GetFile( name ) != NULL;
    }

    return GetDeriviateDir( *m_curDirEntry, tree ) != NULL;
}

bool CArchiveFileTranslator::Delete( const char *path )
{
    dirTree tree;
    bool isFile;

    if ( !GetRelativePathTree( path, tree, isFile ) )
        return false;

    directory *dir;

    if ( !isFile )
    {
        if ( !( dir = (directory*)GetDeriviateDir( *m_curDirEntry, tree ) ) )
            return false;

        if ( dir->IsLocked() )
            return false;

        delete dir;
        
        return true;
    }

    filePath name = tree[ tree.size() - 1 ];
    tree.pop_back();

    return ( dir = (directory*)GetDeriviateDir( *m_curDirEntry, tree ) ) && dir->RemoveFile( name );
}

bool CArchiveFileTranslator::Copy( const char *src, const char *dst )
{
    file *srcEntry = (file*)GetFileEntry( src );

    if ( !srcEntry )
        return false;

    dirTree tree;
    bool isFile;

    if ( !GetRelativePathTree( dst, tree, isFile ) || !isFile )
        return false;

    filePath fileName = tree[ tree.size() - 1 ];
    tree.pop_back();

    file *dstEntry = _CreateDirTree( *m_curDirEntry, tree ).MakeFile( fileName );

    if ( !dstEntry )
        return false;

    // Copy over general attributes
    dstEntry->flags = srcEntry->flags;
    dstEntry->compression = srcEntry->compression;
    dstEntry->modTime = srcEntry->modTime;
    dstEntry->modDate = srcEntry->modDate;
    dstEntry->diskID = srcEntry->diskID;
    dstEntry->internalAttr = srcEntry->internalAttr;
    dstEntry->externalAttr = srcEntry->externalAttr;
    dstEntry->extra = srcEntry->extra;
    dstEntry->comment = srcEntry->comment;
    
    if ( !srcEntry->cached )
    {
        dstEntry->version = srcEntry->version;
        dstEntry->reqVersion = srcEntry->reqVersion;
        dstEntry->crc32 = srcEntry->crc32;
        dstEntry->sizeCompressed = srcEntry->sizeCompressed;
        dstEntry->sizeReal = srcEntry->sizeReal;
        dstEntry->subParsed = true;

        if ( !srcEntry->subParsed )
        {
            _localHeader header;
            seekFile( *srcEntry, header );

            CFile *dstFile = m_unpackRoot->Open( dstEntry->relPath.c_str(), "wb" );

            FileSystem::StreamCopyCount( m_file, *dstFile, header.sizeCompressed );

            delete dstFile;
        }
        else
            m_unpackRoot->Copy( srcEntry->relPath.c_str(), dstEntry->relPath.c_str() );
    }
    else
    {
        dstEntry->cached = true;

        m_realtimeRoot->Copy( srcEntry->relPath.c_str(), dstEntry->relPath.c_str() );
    }

    return true;
}

bool CArchiveFileTranslator::Rename( const char *src, const char *dst )
{
    file *entry = (file*)GetFileEntry( src );

    if ( !entry )
        return false;

    dirTree tree;
    bool isFile;

    if ( !GetRelativePathTree( dst, tree, isFile ) || !isFile )
        return false;

    filePath fileName = tree[ tree.size() - 1 ];
    tree.pop_back();

    if ( !entry->cached )
    {
        if ( !entry->subParsed )
        {
            _localHeader header;
            seekFile( *entry, header );

            CFile *dstFile = m_unpackRoot->Open( dst, "wb" );

            FileSystem::StreamCopyCount( m_file, *dstFile, header.sizeCompressed );

            delete dstFile;
        }
        else
            m_unpackRoot->Rename( entry->relPath.c_str(), dst );
    }
    else
    {
        if ( !entry->locks.empty() )
            return false;

        m_realtimeRoot->Rename( entry->relPath.c_str(), dst );
    }

    // Give it a new name
    entry->name = fileName;

    _CreateDirTree( *m_curDirEntry, tree ).MoveTo( *entry );
    return true;
}

size_t CArchiveFileTranslator::Size( const char *path ) const
{
    const file *entry = GetFileEntry( path );

    if ( !entry )
        return 0;

    if ( !entry->cached )
        return entry->sizeReal;

    return m_realtimeRoot->Size( path );
}

bool CArchiveFileTranslator::Stat( const char *path, struct stat *stats ) const
{
    const file *entry = GetFileEntry( path );

    if ( !entry )
        return false;

    tm date;
    entry->GetModTime( date );

    date.tm_year -= 1900;

    stats->st_mtime = stats->st_ctime = stats->st_atime = mktime( &date );
    stats->st_dev = entry->diskID;
    stats->st_rdev = 0;
    stats->st_gid = 0;
    stats->st_ino = 0;
    stats->st_mode = 0;
    stats->st_nlink = 0;
    stats->st_uid = 0;

    if ( !entry->cached )
        stats->st_size = entry->sizeReal;
    else
        stats->st_size = m_realtimeRoot->Size( entry->relPath.c_str() );

    return true;
}

bool CArchiveFileTranslator::ReadToBuffer( const char *path, std::vector <char>& output ) const
{
    return false;
}

bool CArchiveFileTranslator::ChangeDirectory( const char *path )
{
    dirTree tree;
    bool file;

    if ( !GetRelativePathTreeFromRoot( path, tree, file ) )
        return false;

    if ( file )
        tree.pop_back();

    directory *dir = (directory*)GetDirTree( tree );

    if ( !dir )
        return false;

    m_curDirEntry = dir;
    m_curDirTree = tree;

    m_currentDir.clear();
    _File_OutputPathTree( tree, false, m_currentDir );
    return true;
}

void CArchiveFileTranslator::ScanDirectory( const char *directory, const char *wildcard, bool recurse, pathCallback_t dirCallback, pathCallback_t fileCallback, void *userdata ) const
{

}

static void _scanFindCallback( const filePath& path, std::vector <filePath> *output )
{
    output->push_back( path );
}

void CArchiveFileTranslator::GetDirectories( const char *path, const char *wildcard, bool recurse, std::vector <filePath>& output ) const
{
    ScanDirectory( path, wildcard, recurse, (pathCallback_t)_scanFindCallback, NULL, &output );
}

void CArchiveFileTranslator::GetFiles( const char *path, const char *wildcard, bool recurse, std::vector <filePath>& output ) const
{
    ScanDirectory( path, wildcard, recurse, NULL, (pathCallback_t)_scanFindCallback, &output );
}

struct zip_mapped_rdircheck
{
    inline unsigned short operator() ( const char map[1024], size_t count, long& off ) const
    {
        unsigned short n = (unsigned int)count + 1;
        const char *cur = map + n;

        while ( n )
        {
            if ( *(unsigned int*)--cur == ZIP_SIGNATURE )
            {
                off = n + 3;
                return true;
            }

            n--;
        }

        return false;
    }
};

#pragma pack(1)

struct _endDir
{
    unsigned short  diskID;
    unsigned short  diskAlign;
    unsigned short  entries;
    unsigned short  totalEntries;
    size_t          centralDirectorySize;
    size_t          centralDirectoryOffset;

    unsigned short  commentLen;
};

struct _centralFile
{
    unsigned int    signature;
    unsigned short  version;
    unsigned short  reqVersion;
    unsigned short  flags;
    unsigned short  compression;
    unsigned short  modTime;
    unsigned short  modDate;
    unsigned int    crc32;

    size_t          sizeCompressed;
    size_t          sizeReal;

    unsigned short  nameLen;
    unsigned short  extraLen;
    unsigned short  commentLen;

    unsigned short  diskID;
    unsigned short  internalAttr;
    unsigned int    externalAttr;
    size_t          localHeaderOffset;
};

#pragma pack()

CArchiveFileTranslator::directory& CArchiveFileTranslator::_CreateDirTree( directory& dir, const dirTree& tree )
{
    directory *curDir = &dir;
    dirTree::const_iterator iter = tree.begin();

    for ( ; iter != tree.end(); iter++ )
        curDir = &curDir->GetDirectory( *iter );

    return *curDir;
}

void CArchiveFileTranslator::ReadFiles( unsigned int count )
{
    char buf[65536];

    while ( count-- )
    {
        _centralFile header;

        if ( !m_file.ReadStruct( header ) || header.signature != ZIP_FILE_SIGNATURE )
            throw;

        // Acquire the path
        m_file.Read( buf, 1, header.nameLen );
        buf[ header.nameLen ] = 0;

        dirTree tree;
        bool isFile;

        if ( !GetRelativePathTreeFromRoot( buf, tree, isFile ) )
            throw;

        if ( isFile )
        {
            filePath name = tree[ tree.size() - 1 ];
            tree.pop_back();

            // Deposit in the correct directory
            file& entry = _CreateDirTree( m_root, tree ).AddFile( name );

            // Store attributes
            entry.version = header.version;
            entry.reqVersion = header.reqVersion;
            entry.flags = header.flags;
            entry.compression = header.compression;
            entry.modTime = header.modTime;
            entry.modDate = header.modDate;
            entry.crc32 = header.crc32;
            entry.sizeCompressed = header.sizeCompressed;
            entry.sizeReal = header.sizeReal;
            entry.diskID = header.diskID;
            entry.internalAttr = header.internalAttr;
            entry.externalAttr = header.externalAttr;
            entry.localHeaderOffset = header.localHeaderOffset;
            entry.archived = true;

            if ( header.extraLen )
            {
                m_file.Read( buf, 1, header.extraLen );
                entry.extra = std::string( buf, header.extraLen );
            }

            if ( header.commentLen )
            {
                m_file.Read( buf, 1, header.commentLen );
                entry.comment = std::string( buf, header.commentLen );
            }
        }
        else
        {
            _CreateDirTree( m_root, tree );

            m_file.Seek( header.commentLen + header.extraLen, SEEK_CUR );
        }
    }
}

inline void CArchiveFileTranslator::seekFile( const file& info, _localHeader& header )
{
    m_file.Seek( info.localHeaderOffset, SEEK_SET );

    if ( !m_file.ReadStruct( header ) )
        throw;

    if ( header.signature != ZIP_LOCAL_FILE_SIGNATURE )
        throw;

    m_file.Seek( header.nameLen + header.commentLen, SEEK_CUR );
}

struct zip_inflate_decompression
{
    zip_inflate_decompression()
    {
        m_stream.zalloc = NULL;
        m_stream.zfree = NULL;
        m_stream.opaque = NULL;

        if ( inflateInit2( &m_stream, -MAX_WBITS ) != Z_OK )
            throw;
    }

    ~zip_inflate_decompression()
    {
        inflateEnd( &m_stream );
    }

    inline void prepare( const char *buf, size_t size, bool eof )
    {
        m_stream.avail_in = size;
        m_stream.next_in = (Bytef*)buf;
    }

    inline bool parse( char *buf, size_t size, size_t& sout )
    {
        m_stream.avail_out = size;
        m_stream.next_out = (Bytef*)buf;

        switch( inflate( &m_stream, Z_NO_FLUSH ) )
        {
        case Z_DATA_ERROR:
        case Z_NEED_DICT:
        case Z_MEM_ERROR:
            throw;
        }

        sout = size - m_stream.avail_out;
        return m_stream.avail_out == 0;
    }

    z_stream m_stream;
};

void CArchiveFileTranslator::Extract( CFile& dstFile, file& info )
{
    CFile *from;
    size_t comprSize = info.sizeCompressed;

    if ( info.subParsed )
        from = m_unpackRoot->Open( info.relPath.c_str(), "rb" );
    else
    {
        _localHeader header;
        seekFile( info, header );

        from = &m_file;
    }

    switch( info.compression )
    {
    case 0:
        FileSystem::StreamCopyCount( *from, dstFile, comprSize );
        break;
    case 8:
        FileSystem::StreamParserCount( *from, dstFile, comprSize, zip_inflate_decompression() );
        break;
    default:
        assert( 0 );
    }

    if ( info.subParsed )
        delete from;
}

CArchiveTranslator* CFileSystem::CreateZIPArchive( CFile& file )
{
    if ( !file.IsWriteable() )
        return NULL;

    return new CArchiveFileTranslator( file );
}

CArchiveTranslator* CFileSystem::OpenArchive( CFile& file )
{
    size_t size = file.GetSize();

    if ( !FileSystem::MappedReaderReverse <char [1024]>( file, zip_mapped_rdircheck() ) )
        return NULL;

    _endDir dirEnd;

    if ( !file.ReadStruct( dirEnd ) )
        return NULL;

    file.Seek( -(long)dirEnd.centralDirectorySize - (long)sizeof( dirEnd ) - 4, SEEK_CUR );

    CArchiveFileTranslator *zip = new CArchiveFileTranslator( file );

    try
    {
        zip->ReadFiles( dirEnd.entries );
    }
    catch( ... )
    {
        delete zip;

        return NULL;
    }

    return zip;
}

void CArchiveFileTranslator::CacheDirectory( const directory& dir )
{
    fileList::const_iterator fileIter = dir.files.begin();

    for ( ; fileIter != dir.files.end(); fileIter++ )
    {
        if ( (*fileIter)->cached || (*fileIter)->subParsed )
            continue;

        // Dump the compressed content
        _localHeader header;
        seekFile( **fileIter, header );

        CFile *dst = m_unpackRoot->Open( (*fileIter)->relPath.c_str(), "wb" );

        FileSystem::StreamCopyCount( m_file, *dst, (*fileIter)->sizeCompressed );

        delete dst;

        (*fileIter)->subParsed = true;
    }

    directory::subDirs::const_iterator iter = dir.children.begin();

    for ( ; iter != dir.children.end(); iter++ )
        CacheDirectory( **iter );
}

struct zip_stream_compression
{
    zip_stream_compression( CArchiveFileTranslator::_localHeader& header ) : m_header( header )
    {
        m_header.sizeCompressed = 0;
    }

    inline void checksum( const char *buf, size_t size )
    {
        m_header.crc32 = crc32( m_header.crc32, (Bytef*)buf, size );
    }

    inline void prepare( const char *buf, size_t size, bool eof )
    {
        m_rcv = size;
        m_buf = buf;

        checksum( buf, size );
    }

    inline bool parse( char *buf, size_t size, size_t& sout )
    {
        size_t toWrite = min( size, m_rcv );
        memcpy( buf, m_buf, toWrite );

        m_header.sizeCompressed += toWrite;

        m_buf += toWrite;
        m_rcv -= toWrite;

        sout = size;
        return m_rcv != 0;
    }

    size_t m_rcv;
    const char* m_buf;
    CArchiveFileTranslator::_localHeader& m_header;
};

struct zip_deflate_compression : public zip_stream_compression
{
    zip_deflate_compression( CArchiveFileTranslator::_localHeader& header, int level ) : zip_stream_compression( header )
    {
        m_stream.zalloc = NULL;
        m_stream.zfree = NULL;
        m_stream.opaque = NULL;

        if ( deflateInit2( &m_stream, level, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY ) != Z_OK )
            throw;
    }

    ~zip_deflate_compression()
    {
        deflateEnd( &m_stream );
    }

    inline void prepare( const char *buf, size_t size, bool eof )
    {
        m_flush = eof ? Z_FINISH : Z_NO_FLUSH;

        m_stream.avail_in = size;
        m_stream.next_in = (Bytef*)buf;

        zip_stream_compression::checksum( buf, size );
    }

    inline bool parse( char *buf, size_t size, size_t& sout )
    {
        m_stream.avail_out = size;
        m_stream.next_out = (Bytef*)buf;

        int ret = deflate( &m_stream, m_flush );

        sout = size - m_stream.avail_out;

        m_header.sizeCompressed += sout;

        if ( ret == Z_STREAM_ERROR )
            throw;

        return m_stream.avail_out == 0;
    }

    int m_flush;
    z_stream m_stream;
};

void CArchiveFileTranslator::SaveDirectory( directory& dir, size_t& size )
{
    directory::subDirs::iterator iter = dir.children.begin();

    for ( ; iter != dir.children.end(); iter++ )
        SaveDirectory( **iter, size );

    fileList::iterator fileIter = dir.files.begin();

    for ( ; fileIter != dir.files.end(); fileIter++ )
    {
        file& info = **fileIter;
        _localHeader header;
        header.signature = ZIP_LOCAL_FILE_SIGNATURE;
        header.modTime = info.modTime;
        header.modDate = info.modDate;
        header.nameLen = info.relPath.size();
        header.commentLen = info.comment.size();

        // Update the offset
        info.localHeaderOffset = m_file.Tell() - m_structOffset;

        size += sizeof( header ) + header.nameLen + header.commentLen;

        if ( !info.cached )
        {
            header.version = info.version;
            header.flags = info.flags;
            header.compression = info.compression;
            header.crc32 = info.crc32;
            header.sizeCompressed = info.sizeCompressed;
            header.sizeReal = info.sizeReal;

            size += info.sizeCompressed;

            m_file.WriteStruct( header );
            m_file.WriteString( info.relPath.c_str() );
            m_file.WriteString( info.comment );

            CFile *src = m_unpackRoot->Open( info.relPath.c_str(), "rb" );

            FileSystem::StreamCopy( *src, m_file );

            delete src;
        }
        else    // has to be cached
        {
            header.version = 10;    // WINNT
            header.flags = info.flags;
            header.compression = info.compression = 8; // deflate
            header.crc32 = 0;

            m_file.WriteStruct( header );
            m_file.WriteString( info.relPath.c_str() );
            m_file.WriteString( info.comment );

            CFile *src = dynamic_cast <CSystemFileTranslator*> ( m_realtimeRoot )->OpenEx( info.relPath.c_str(), "rb", FILE_FLAG_WRITESHARE );

            info.sizeReal = header.sizeReal = src->GetSize();

            switch( header.compression )
            {
            case 0:
                FileSystem::StreamParser( *src, m_file, zip_stream_compression( header ) );
                break;
            case 8:
                FileSystem::StreamParser( *src, m_file, zip_deflate_compression( header, Z_DEFAULT_COMPRESSION ) );
                break;
            default:
                assert( 0 );
            }

            delete src;

            size += info.sizeCompressed = header.sizeCompressed;
            info.crc32 = header.crc32;

            long wayOff = header.sizeCompressed + header.nameLen + header.commentLen;

            m_file.Seek( -wayOff - (long)sizeof( header ), SEEK_CUR );
            m_file.WriteStruct( header );
            m_file.Seek( wayOff, SEEK_CUR );
        }
    }
}

unsigned int CArchiveFileTranslator::BuildCentralFileHeaders( const directory& dir, size_t& size )
{
    directory::subDirs::const_iterator iter = dir.children.begin();
    unsigned int cnt = 0;

    for ( ; iter != dir.children.end(); iter++ )
        cnt += BuildCentralFileHeaders( **iter, size );

    fileList::const_iterator fileIter = dir.files.begin();

    for ( ; fileIter != dir.files.end(); fileIter++ )
    {
        const file& info = **fileIter;
        _centralFile header;

        header.signature = ZIP_FILE_SIGNATURE;
        header.version = info.version;
        header.reqVersion = info.reqVersion;
        header.flags = info.flags;
        header.compression = info.compression;
        header.modTime = info.modTime;
        header.modDate = info.modDate;
        header.crc32 = info.crc32;
        header.sizeCompressed = info.sizeCompressed;
        header.sizeReal = info.sizeReal;
        header.nameLen = info.relPath.size();
        header.extraLen = info.extra.size();
        header.commentLen = info.comment.size();
        header.diskID = info.diskID;
        header.internalAttr = info.internalAttr;
        header.externalAttr = info.externalAttr;
        header.localHeaderOffset = info.localHeaderOffset;

        m_file.WriteStruct( header );
        m_file.WriteString( info.relPath.c_str() );
        m_file.WriteString( info.extra );
        m_file.WriteString( info.comment );

        size += sizeof( header ) + header.nameLen + header.extraLen + header.commentLen;
        cnt++;
    }

    return cnt;
}

void CArchiveFileTranslator::Save()
{
    if ( !m_file.IsWriteable() )
        return;

    // Cache the .zip content
    CacheDirectory( m_root );

    // Rewrite the archive
    m_file.Seek( m_structOffset, SEEK_SET );

    size_t fileSize = 0;
    SaveDirectory( m_root, fileSize );

    // Create the central directory
    size_t centralSize = 0;
    unsigned int entryCount = BuildCentralFileHeaders( m_root, centralSize );

    // Finishing entry
    m_file.WriteInt( ZIP_SIGNATURE );

    _endDir tail;
    tail.diskID = 0;
    tail.diskAlign = 0;
    tail.entries = entryCount;
    tail.totalEntries = entryCount;
    tail.centralDirectorySize = centralSize;
    tail.centralDirectoryOffset = fileSize; // we might need the offset of the .zip here
    tail.commentLen = m_comment.size();

    m_file.WriteStruct( tail );
    m_file.WriteString( m_comment );

    // Cap the stream
    m_file.SetSeekEnd();
}

void CFileSystem::InitZIP()
{
    char buf[1024];
    filePath tmpDir;

#ifdef _WIN32
    GetTempPath( sizeof( buf ), buf );
    tmpDir = buf;
    tmpDir += '/';

    sysTmp = CreateTranslator( tmpDir.c_str() );

    std::stringstream stream;

    stream.precision( 0 );
    stream << ( rand() % 647251833 );

    tmpDir += "&$!reAr";
    tmpDir += stream.str();
    tmpDir += "_/";

    CreateDirectory( tmpDir.c_str(), NULL );
#endif //_WIN32

    // Create the .zip temporary root
    sysTmpRoot = CreateTranslator( tmpDir.c_str() );
}

void CFileSystem::DestroyZIP()
{
    filePath tmpDir;
    sysTmpRoot->GetFullPath( "/", false, tmpDir );

    delete sysTmpRoot;

    sysTmp->Delete( tmpDir.c_str() );

    delete sysTmp;
}