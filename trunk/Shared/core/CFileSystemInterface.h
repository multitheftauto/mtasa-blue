/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/core/CFileSystemInterface.h
*  PURPOSE:     File management
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CFileSystemInterface_
#define _CFileSystemInterface_

typedef std::vector <filePath> dirTree;

enum eFileException
{
    FILE_STREAM_TERMINATED  // user attempts to perform on a terminated file stream, ie. http timeout
};

/*===================================================
    CFile

    This is the access interface to files/streams. You can read,
    write to and obtain information from this. Once destroyed, the
    connection is unlinked. During class life-time, the file is locked
    for deletion. Other locks depend on the nature of the stream.
===================================================*/
class CFile abstract
{
public:
    virtual                 ~CFile()
    {
    }

    /*===================================================
        CFile::Read

        Arguments:
            buffer - memory location to write data to
            sElement - size of data chunks to be read
            iNumElements - number of data chunks
        Purpose:
            Requests data from the file/stream.
    ===================================================*/
    virtual	size_t          Read( void *buffer, size_t sElement, unsigned long iNumElements ) = 0;

    /*===================================================
        CFile::Write

        Arguments:
            buffer - memory location to read data from
            sElement - size of data chunks to be written
            iNumElements - number of data chunks
        Purpose:
            Reads data chunks from buffer per sElement stride and
            forwards it to the file/stream.
    ===================================================*/
    virtual	size_t          Write( const void *buffer, size_t sElement, unsigned long iNumElements ) = 0;

    /*===================================================
        CFile::Seek

        Arguments:
            iOffset - positive or negative value to offset the stream by
            iType - SET_* ANSI enum to specify the procedure
        Purpose:
            Relocates the position of the file/stream.
    ===================================================*/
    virtual	int             Seek( long iOffset, int iType ) = 0;

    /*===================================================
        CFile::Tell

        Purpose:
            Returns the absolute file/stream location.
    ===================================================*/
    virtual	long            Tell() const = 0;

    /*===================================================
        CFile::IsEOF

        Purpose:
            Returns whether the file/stream has reached it's end. Other
            than the ANSI feof, this is not triggered by reaching over
            the file/stream boundary.
    ===================================================*/
    virtual	bool            IsEOF() const = 0;

    /*===================================================
        CFile::Stat

        Arguments:
            stats - ANSI file information struct
        Purpose:
            Returns true whether it could request information
            about the file/stream. If successful, stats has been
            filled with useful information.
    ===================================================*/
    virtual	bool            Stat( struct stat *stats ) const = 0;

    /*===================================================
        CFile::PushStat

        Arguments:
            stats - ANSI file information struct
        Purpose:
            Updates the file/stream meta information.
    ===================================================*/
    virtual void            PushStat( const struct stat *stats ) = 0;

    /*===================================================
        CFile::SetSeekEnd

        Purpose:
            Sets the file/stream end at the current seek location.
            It effectively cuts off bytes beyond that.
    ===================================================*/
    virtual void            SetSeekEnd() = 0;

    /*===================================================
        CFile::GetSize

        Purpose:
            Returns the total file/stream size if available.
            Otherwise it should return 0.
    ===================================================*/
    virtual	size_t          GetSize() const = 0;

    /*===================================================
        CFile::Flush

        Purpose:
            Writes pending file/stream buffers to disk, thus having
            an updated representation in the filesystem to be read
            by different applications.
    ===================================================*/
    virtual	void            Flush() = 0;

    /*===================================================
        CFile::GetPath

        Purpose:
            Returns the unique filesystem location descriptor of this
            file/stream.
    ===================================================*/
    virtual const filePath& GetPath() const = 0;

    /*===================================================
        CFile::IsReadable

        Purpose:
            Returns whether read operations are possible on this
            file/stream. If not, all attempts to request data
            from this are going to fail.
    ===================================================*/
    virtual bool            IsReadable() const = 0;

    /*===================================================
        CFile::IsWriteable

        Purpose:
            Returns whether write operations are possible on this
            file/stream. If not, all attempts to push data into
            this are going to fail.
    ===================================================*/
    virtual bool            IsWriteable() const = 0;

    // Utility definitions, mostly self-explanatory
    virtual	int             ReadInt()
    {
        int i;

        Read( &i, sizeof(int), 1 );
        return i;
    }

    virtual	short           ReadShort()
    {
        short i;

        Read( &i, sizeof(short), 1 );
        return i;
    }

    virtual	char            ReadByte()
    {
        char i;

        Read( &i, sizeof(char), 1 );
        return i;
    }

    virtual	float           ReadFloat()
    {
        float f;

        Read( &f, sizeof(float), 1 );
        return f;
    }

    virtual	size_t          WriteInt( int iInt )
    {
        return Write( &iInt, sizeof(int), 1 );
    }

    virtual size_t          WriteShort( unsigned short iShort )
    {
        return Write( &iShort, sizeof(short), 1 );
    }

    virtual size_t          WriteByte( unsigned char cByte )
    {
        return Write( &cByte, sizeof(char), 1 );
    }

    virtual size_t          WriteFloat( float fFloat )
    {
        return Write( &fFloat, sizeof(float), 1 );
    }

    /*===================================================
        CFile::Printf

        Arguments:
            pFormat - ANSI string implementation pattern.
            ... - VARG of necessary string implementation components.
        Purpose:
            ANSI C style interface which can be used to write
            formatted strings into this file/stream. Returns the
            amount of bytes written.
    ===================================================*/
    virtual	size_t          Printf( const char *pFormat, ... )
    {
        va_list args;
        char cBuffer[1024];

        va_start(args, pFormat);
        _vsnprintf(cBuffer, 1023, pFormat, args);
        va_end(args);

        return Write(cBuffer, 1, strlen(cBuffer));
    }

    size_t                  WriteString( const std::string& input )
    {
        return Write( input.c_str(), 1, input.size() );
    }

    /*===================================================
        CFile::GetString

        Arguments:
            output - std::string type to write the string to
        Purpose:
            Reads a line from this file/stream. Lines are seperated
            by \n. Returns whether anything could be read.
    ===================================================*/
    bool                    GetString( std::string& output )
    {
        if ( IsEOF() )
            return false;

        do
        {
            unsigned char c = ReadByte();

            if ( !c || c == '\n' )
                return true;

            output += c;
        }
        while ( !IsEOF() );

        return true;
    }

    /*===================================================
        CFile::GetString

        Arguments:
            buf - memory location to write a C string to
            max - has to be >1; maximum valid range of the
                  memory area pointed to by buf.
        Purpose:
            Same as above, but C-style interface. Automatically
            terminates buf contents by \n if successful.
    ===================================================*/
    bool                    GetString( char *buf, const size_t max )
    {
        size_t n = 0;

        if ( max < 2 || IsEOF() )
            return false;

        do
        {
            unsigned char c = ReadByte();

            if ( !c || c == '\n' )
                goto finish;

            buf[n++] = c;

            if ( n == max - 1 )
                goto finish;
        }
        while ( !IsEOF() );

finish:
        buf[n] = '\0';
        return true;
    }

    template <class type>
    bool    ReadStruct( type& buf )
    {
        return Read( &buf, 1, sizeof( type ) ) == sizeof( type );
    }

    template <class type>
    bool    WriteStruct( type& buf )
    {
        return Write( &buf, 1, sizeof( type ) ) == sizeof( type );
    }
};

typedef void (*pathCallback_t)( const filePath& path, void *userdata );

/*===================================================
    CFileTranslator

    A file translator is an access point to filesystems on the local
    filesystem, the network or archives. Before destroying this, all files
    created by this have to be previously destroyed.

    It resides in a root directory and can change it's current directory.
    All these directories are locked for deletion for security reasons.
===================================================*/
class CFileTranslator abstract
{
public:
    virtual                 ~CFileTranslator()
    {
    }

    /*===================================================
        CFileTranslator::WriteData

        Arguments:
            path - target path to put data at
            buffer - source of the data
            size - size of the data
        Purpose:
            Writes memory to a target pointed to by path. It
            returns whether the write was successful.
    ===================================================*/
    virtual bool            WriteData( const char *path, const char *buffer, size_t size ) = 0;

    /*===================================================
        CFileTranslator::CreateDir

        Arguments:
            path - target path for directory creation
        Purpose:
            Attempts to create the directory tree pointed at by
            path. Returns whether the operation was successful.
            It creates all directory along the way, so if path
            is valid, the operation will most-likely succeed.
    ===================================================*/
    virtual bool            CreateDir( const char *path ) = 0;

    /*===================================================
        CFileTranslator::Open

        Arguments:
            path - target path to attempt access to
            mode - ANSI C mode descriptor ("w", "rb+", "a", ...)
        Purpose:
            Attempt to access resources located at path. The access type
            is described by mode. If the operation fails, NULL is returned.
            Failure is either caused due to locks set by the filesystem
            or by an invalid path or invalid mode descriptor.
    ===================================================*/
    virtual CFile*          Open( const char *path, const char *mode ) = 0;

    /*===================================================
        CFileTranslator::Exists

        Arguments:
            path - target path
        Purpose:
            Returns whether the resource at path exists.
    ===================================================*/
    virtual bool            Exists( const char *path ) const = 0;

    /*===================================================
        CFileTranslator::Delete

        Arguments:
            path - target path
        Purpose:
            Attempts to delete the resources located at path. If it is a single
            resource, it is deleted. If it is a directory, all contents are
            recursively deleted and finally the diretory entry itself. If any
            resource fails to be deleted, false is returned.
    ===================================================*/
    virtual bool            Delete( const char *path ) = 0;

    /*===================================================
        CFileTranslator::Copy

        Arguments:
            src - location of the source resource
            dst - location to copy the resource to
        Purpose:
            Creates another copy of the resource pointed at by src
            at the dst location. Returns whether the operation
            was successful.
    ===================================================*/
    virtual bool            Copy( const char *src, const char *dst ) = 0;

    /*===================================================
        CFileTranslator::Rename

        Arguments:
            src - location of the source resource
            dst - location to move the resource to
        Purpose:
            Moves the resource pointed to by src to dst location.
            Returns whether the operation was successful.
    ===================================================*/
    virtual bool            Rename( const char *src, const char *dst ) = 0;

    /*===================================================
        CFileTranslator::Size

        Arguments:
            path - path of the query resource
        Purpose:
            Returns the size of the resource at path. The result
            is zero if an error occurred.
    ===================================================*/
    virtual size_t          Size( const char *path ) const = 0;

    /*===================================================
        CFileTranslator::Stat

        Arguments:
            path - path of the query resource
            stats - ANSI C structure for file information
        Purpose:
            Attempts to receive resource meta information at path.
            Returns false if operation failed; then stats remains
            unchanged.
    ===================================================*/
    virtual bool            Stat( const char *path, struct stat *stats ) const = 0;

    /*===================================================
        CFileTranslator::ReadToBuffer

        Arguments:
            path - target location
            output - buffer which shall receive the data
        Purpose:
            Attempts to read data from the resource located at path.
            Returns whether the operation was successful. output is
            only modified if it was successful.
    ===================================================*/
    virtual bool            ReadToBuffer( const char *path, std::vector <char>& output ) const = 0;

    /*==============================================================
        Path Translation functions

        Any path provided to these functions is valid, if it does not
        uproot. It may not leave the translator's root directory.
    ==============================================================*/

    /*===================================================
        CFileTranslator::GetFullPathTreeFromRoot

        Arguments:
            path - target path
            tree - list of path items
            file - whether path points to a file or not
        Purpose:
            Attempts to parse a provided path into a unique absolute path.
            In a Windows OS, the path starts from the root of the drive.
            The path is not constructed, but rather seperated into the list
            pointed to at variable tree. Paths are based against the root
            of this translator.
    ===================================================*/
    virtual bool            GetFullPathTreeFromRoot( const char *path, dirTree& tree, bool& file ) const = 0;

    /*===================================================
        CFileTranslator::GetFullPathTree

        Arguments:
            path - target path
            tree - list of path items
            file - whether path points to a file or not
        Purpose:
            Does the same as GetFullPathTreeFromRoot, but the path is based
            against the current directory of the translator.
    ===================================================*/
    virtual bool            GetFullPathTree( const char *path, dirTree& tree, bool& file ) const = 0;

    /*===================================================
        CFileTranslator::GetRelativePathTreeFromRoot

        Arguments:
            path - target path
            tree - list of path items
            file - whether path points to a file or not
        Purpose:
            Attempts to parse the provided path to a unique representation
            based on the root directory of the translator. The resulting path
            can be considered a unique representation for this translator.
            The resulting path is split into it's components at the tree list.
    ===================================================*/
    virtual bool            GetRelativePathTreeFromRoot( const char *path, dirTree& tree, bool& file ) const = 0;

    /*===================================================
        CFileTranslator::GetRelativePathTree

        Arguments:
            path - target path
            tree - list of path items
            file - whether path points to a file or not
        Purpose:
            Does the same as GetRelativePathTreeFromRoot, but bases the
            resulting path on the translator's current directory.
    ===================================================*/
    virtual bool            GetRelativePathTree( const char *path, dirTree& tree, bool& file ) const = 0;

    /*===================================================
        CFileTranslator::GetFullPathFromRoot

        Arguments:
            path - target path
            allowFile - if false only directory paths are returned
            output - write location for output path
        Purpose:
            Executes GetFullPathTreeFromRoot and parses it's output
            into a full (system) path. That is to convert path into a
            full (system) path based on the translator's root.
    ===================================================*/
    virtual bool            GetFullPathFromRoot( const char *path, bool allowFile, filePath& output ) const = 0;

    /*===================================================
        CFileTranslator::GetFullPath

        Arguments:
            path - target path
            allowFile - if false only directory paths are returned
            output - write location for output path
        Purpose:
            Executes GetFullPathTree and parses it's output into a full
            (system) path. This translation is based on the translator's
            current directory.
    ===================================================*/
    virtual bool            GetFullPath( const char *path, bool allowFile, filePath& output ) const = 0;

    /*===================================================
        CFileTranslator::GetRelativePathFromRoot

        Arguments:
            path - target path
            allowFile - if false only directory paths are returned
            output - write location for output path
        Purpose:
            Executes GetRelativePathTreeFromRoot and parses it's output
            into a path relative to the translator's root directory.
    ===================================================*/
    virtual bool            GetRelativePathFromRoot( const char *path, bool allowFile, filePath& output ) const = 0;

    /*===================================================
        CFileTranslator::GetRelativePath

        Arguments:
            path - target path
            allowFile - if false only directory paths are returned
            output - write location for output path
        Purpose:
            Executes GetRelativePathTree and parses it's output
            into a path relative to the translator's current directory.
    ===================================================*/
    virtual bool            GetRelativePath( const char *path, bool allowFile, filePath& output ) const = 0;

    /*===================================================
        CFileTranslator::ChangeDirectory

        Arguments:
            path - target path
        Purpose:
            Attempts to change the current directory of the translator.
            Returns whether the operation succeeded.
    ===================================================*/
    virtual bool            ChangeDirectory( const char *path ) = 0;

    /*===================================================
        CFileTranslator::GetDirectory

        Arguments:
            output - structure to save path at
        Purpose:
            Writes the current directory of the translator into output.
    ===================================================*/
    virtual void            GetDirectory( filePath& output ) const = 0;

    /*===================================================
        CFileTranslator::ScanDirectory

        Arguments:
            directory - location of the scan
            wildcard - pattern to check filenames onto
            recurse - if true then sub directories are scanned, too
            dirCallback - executed for every sub directory found
            fileCallback - executed for every file found
            userdata - passed to every callback
        Purpose:
            Scans the designated directory for files and directories.
            The callback is passed the full path of the found resource
            and the userdata.
    ===================================================*/
    virtual void            ScanDirectory( const char *directory, const char *wildcard, bool recurse, 
                                pathCallback_t dirCallback, 
                                pathCallback_t fileCallback, 
                                void *userdata ) const = 0;

    // These functions are easy helpers for ScanDirectory.
    virtual void            GetDirectories( const char *path, const char *wildcard, bool recurse, std::vector <filePath>& output ) const = 0;
    virtual void            GetFiles( const char *path, const char *wildcard, bool recurse, std::vector <filePath>& output ) const = 0;
};

class CArchiveTranslator abstract : public virtual CFileTranslator
{
public:
    virtual void            Save() = 0;
};

class CFileSystemInterface
{
public:
    virtual CFileTranslator*    CreateTranslator( const char *path ) = 0;
    virtual CArchiveTranslator* OpenArchive( CFile& file ) = 0;

    virtual CArchiveTranslator* CreateZIPArchive( CFile& file ) = 0;

    // Insecure, use with caution!
    virtual bool                IsDirectory( const char *path ) = 0;
    virtual bool                Exists( const char *path ) = 0;
    virtual size_t              Size( const char *path ) = 0;
    virtual bool                ReadToBuffer( const char *path, std::vector <char>& output ) = 0;
};

namespace FileSystem
{
    // These functions are not for noobs.
    template <class t, typename F>
    inline bool MappedReaderReverse( CFile& file, F f )
    {
        t buf;
        long off;

        file.Seek( -(long)sizeof( buf ), SEEK_END );

        do
        {
            size_t readCount = file.Read( &buf, 1, sizeof( buf ) );

            if ( f( buf, readCount, off ) )
            {
                file.Seek( -(long)readCount + off, SEEK_CUR );
                return true;
            }

        } while ( file.Seek( -(long)sizeof( buf ) * 2 + (long)sizeof( buf ) / 2, SEEK_CUR ) == 0 );

        return false;
    }

    inline void StreamCopy( CFile& src, CFile& dst )
    {
        char buf[8096];

        while ( !src.IsEOF() )
        {
            size_t rb = src.Read( buf, 1, sizeof( buf ) );
            dst.Write( buf, 1, rb );
        }

        dst.SetSeekEnd();
    }

    inline void StreamCopyCount( CFile& src, CFile& dst, size_t cnt )
    {
        size_t toRead;
        char buf[8096];

        while ( ( toRead = min( sizeof( buf ), cnt ) ) != 0 )
        {
            size_t rb = src.Read( buf, 1, toRead );

            cnt -= rb;

            dst.Write( buf, 1, rb );
        }

        dst.SetSeekEnd();
    }

    template <class cb>
    inline void StreamParser( CFile& src, CFile& dst, cb& f )
    {
        char buf[8096];
        char outBuf[16192];
        size_t outSize;

        for (;;)
        {
            size_t rb = src.Read( buf, 1, sizeof( buf ) );

            bool eof = src.IsEOF();
            f.prepare( buf, rb, eof );

            for (;;)
            {
                bool cnt = f.parse( outBuf, sizeof( outBuf ), outSize );
                dst.Write( outBuf, 1, outSize );

                if ( !cnt )
                    break;
            }

            if ( eof )
                break;
        }

        dst.SetSeekEnd();
    }

    template <class cb>
    inline void StreamParserCount( CFile& src, CFile& dst, size_t cnt, cb& f )
    {
        char buf[8096];
        char outBuf[16192];
        size_t outSize;
        size_t toRead;

        for (;;)
        {
            bool eof;

            if ( sizeof( buf ) >= cnt )
            {
                eof = true;

                toRead = cnt;
            }
            else
            {
                eof = false;

                cnt -= toRead = sizeof( buf );
            }

            size_t rb = src.Read( buf, 1, toRead );

            f.prepare( buf, rb, eof );

            for (;;)
            {
                bool cnt = f.parse( outBuf, sizeof( outBuf ), outSize );
                dst.Write( outBuf, 1, outSize );

                if ( !cnt )
                    break;
            }

            if ( eof )
                break;
        }
        
        dst.SetSeekEnd();
    }
}

#endif //_CFileSystemInterface_