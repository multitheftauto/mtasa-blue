/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.Buffer.h
*  PURPOSE:
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

namespace SharedUtil
{
    //
    // Heap memory which is auto-deleted on scope exit
    // Designed as a replacement for alloca(), so allocated size is in bytes
    //
    template < class T >
    class CScopeAlloc
    {
        std::vector < char > buffer;
    public:
        CScopeAlloc( size_t sizeInBytes )
        {
            buffer.resize( sizeInBytes );
        }

        void resize( size_t newSizeInBytes )
        {
            buffer.resize( newSizeInBytes );
        }

        operator T*( void )
        {
            return buffer.empty() ? nullptr : reinterpret_cast < T* >( &buffer.at( 0 ) );
        }
    };

    // Assuming compiled on little endian machine
    #define CBUFFER_LITTLE_ENDIAN
    //#define CBUFFER_BIG_ENDIAN

    //////////////////////////////////////////////////////
    //
    // class CBuffer
    //
    // Wrapped byte array
    //
    //////////////////////////////////////////////////////
    class CBuffer : protected std::vector < char >
    {
    public:
        CBuffer ( void )
        {}
        CBuffer ( const void* pData, uint uiSize )
        {
            AddBytes ( pData, uiSize, 0 );
        }

        void Clear ( void )
        {
            clear ();
        }

        bool IsEmpty ( void ) const
        {
            return empty ();
        }

        void Reserve ( uint uiSize )
        {
            return reserve ( uiSize );
        }

        // Comparison
        bool operator== ( const CBuffer& other ) const
        {
            return size () == other.size ()
                && std::equal( begin(), end(), other.begin() );
        }

        bool operator!= ( const CBuffer& other ) const
        {
            return !operator== ( other );
        }

        // Status
        void SetSize ( uint uiSize, bool bZeroPad = false )
        {
            uint uiOldSize = (uint)size ();
            resize ( uiSize );
            if ( bZeroPad && uiSize > uiOldSize )
                memset ( GetData () + uiOldSize, 0, uiSize - uiOldSize );
        }

        uint GetSize ( void ) const
        {
            return (uint)size ();
        }

        // Access
        char* GetData ( uint uiOffset = 0 )
        {
            return size () ? &at ( uiOffset ) : NULL;
        }

        const char* GetData ( uint uiOffset = 0 ) const
        {
            return size () ? &at ( uiOffset ) : NULL;
        }

        // Joining
        CBuffer operator+( const CBuffer& other ) const
        {
            CBuffer temp = *this;
            return temp += other;
        }

        CBuffer& operator+=( const CBuffer& other )
        {
            AddBytes ( other.GetData (), other.GetSize (), GetSize () );
            return *this;
        }

        // Splitting
        CBuffer Mid( int iOffset, int iSize ) const
        {
            iOffset = Clamp < int > ( 0, iOffset, GetSize () );
            iSize   = Clamp < int > ( 0, iSize,   GetSize () - iOffset );
            return CBuffer ( GetData () + iOffset, iSize );
        }

        CBuffer Head( uint uiAmount ) const
        {
            return Mid ( 0, uiAmount );
        }

        CBuffer Tail( uint uiAmount ) const
        {
            uiAmount = Min ( uiAmount, GetSize () );
            return Mid ( GetSize () - uiAmount, uiAmount );
        }

        // Util
        bool LoadFromFile ( const char* szFilename )
        {
            return FileLoad ( szFilename, *this );
        }
        bool SaveToFile ( const char* szFilename ) const
        {
            return FileSave ( szFilename, GetData (), GetSize () );
        }


    protected:
        void AddBytes ( const void* pData, uint uiLength, int iOffset, bool bToFromNetwork = false )
        {
            // More room required?
            if ( iOffset + uiLength > GetSize () )
                SetSize ( iOffset + uiLength );
#ifdef CBUFFER_LITTLE_ENDIAN
            if ( bToFromNetwork )
            {
                char* pDest      = GetData () + iOffset + uiLength;
                const char* pSrc = (const char*)pData;
                while ( uiLength-- )
                    *--pDest = *pSrc++;
            }
            else
#endif
                memcpy ( GetData () + iOffset, pData, uiLength );
        }

        bool GetBytes ( void* pData, uint uiLength, int iOffset, bool bToFromNetwork = false ) const
        {
            // Not enough data to get?
            if ( iOffset + uiLength > GetSize () )
                return false;

#ifdef CBUFFER_LITTLE_ENDIAN
            if ( bToFromNetwork )
            {
                char* pDest      = (char*)pData;
                const char* pSrc = GetData () + iOffset + uiLength;
                while ( uiLength-- )
                    *pDest++ = *--pSrc;
            }
            else
#endif
                memcpy ( pData, GetData () + iOffset, uiLength );
            return true;
        }

        friend class CBufferWriteStream;
        friend class CBufferReadStream;
    };


    //////////////////////////////////////////////////////
    //
    // class CBufferStream
    //
    // Buffer access
    //
    //////////////////////////////////////////////////////
    class CBufferStream
    {
    public:
        CBufferStream ( bool bToFromNetwork )
            : m_iPos ( 0 )
            , m_uiVersion ( 0 )
            , m_bToFromNetwork ( bToFromNetwork )
        {
        }
        void            Seek ( int iPos )               { m_iPos = Clamp ( 0, iPos, GetSize () ); }
        int             Tell ( void ) const             { return m_iPos; }
        virtual int     GetSize ( void ) const = 0;
        bool            AtEnd ( int iOffset = 0 ) const { return m_iPos + iOffset >= GetSize (); }
        void            SetVersion ( uint uiVersion )   { m_uiVersion = uiVersion; }
        uint            Version ( void ) const          { return m_uiVersion; }

    protected:
        int m_iPos;
        int m_uiVersion;
        bool m_bToFromNetwork;
    };


    //////////////////////////////////////////////////////
    //
    // class CBufferReadStream
    //
    // Used to stream bytes from a buffer
    //
    //////////////////////////////////////////////////////
    class CBufferReadStream : public CBufferStream
    {
        const CBuffer* pBuffer;
    public:
        CBufferReadStream ( const CBuffer& source, bool bToFromNetwork = false )
            : CBufferStream ( bToFromNetwork )
            , pBuffer ( &source )
        {
        }

        virtual int             GetSize ( void ) const          { return pBuffer->GetSize (); }
        virtual const char*     GetData ( void ) const          { return pBuffer->GetData (); }

        // Return true if enough bytes left in the buffer
        bool CanReadNumberOfBytes( int iLength )
        {
            Seek( Tell() );
            return iLength >= 0 && iLength <= ( GetSize() - Tell() );
        }

        bool ReadBytes ( void* pData, int iLength, bool bToFromNetwork = false )
        {
            // Validate pos
            Seek ( Tell () );

            if ( !pBuffer->GetBytes ( pData, iLength, Tell (), bToFromNetwork ) )
                return false;   // Not enough bytes left to fill request

            // Adjust pos
            Seek ( Tell () + iLength );
            return true;
        }

        void Read ( SString& );     // Not defined as it won't work
        void Read ( CBuffer& );     // Not defined as it won't work
        bool ReadString ( SString& result, bool bByteLength = false, bool bDoesLengthIncludeLengthOfLength = false )
        {
            result = "";

            // Get the length
            ushort usLength = 0;
            if ( bByteLength )
            {
                uchar ucLength = 0;
                Read ( ucLength );
                usLength = ucLength;
            }
            else
                Read ( usLength );

            if ( bDoesLengthIncludeLengthOfLength && usLength )
                usLength -= bByteLength ? 1 : 2;

            if ( usLength )
            {
                // Check has enough data
                if ( !CanReadNumberOfBytes( usLength ) )
                    return false;
                // Read the data
                CScopeAlloc < char > buffer( usLength );
                if ( !ReadBytes ( buffer, usLength, false ) )
                    return false;

                result = std::string ( buffer, usLength );
            }
            return true;
        }

        bool ReadBuffer ( CBuffer& outResult )
        {
            outResult.Clear ();

            // Get the length
            ushort usLength = 0;
            if ( !Read ( usLength ) )
                return false;

            uint uiLength = usLength;
            if ( uiLength == 65535 )
                if ( !Read ( uiLength ) )
                    return false;

            if ( uiLength )
            {
                // Check has enough data
                if ( !CanReadNumberOfBytes( uiLength ) )
                    return false;
                // Read the data
                outResult.SetSize ( uiLength );
                if ( !ReadBytes ( outResult.GetData (), uiLength, false ) )
                {
                    outResult.Clear ();
                    return false;
                }
            }
            return true;
        }

        template < class T >
        bool Read ( T& e )
        {
            return ReadBytes ( &e, sizeof ( e ), m_bToFromNetwork );
        }

#ifdef ANY_x64
        // Force all these types to use 4 bytes
        bool Read( unsigned long& e )
        {
            uint temp;
            bool bResult = Read( temp );
            e = temp;
            return bResult;
        }
        bool Read( long& e )
        {
            int temp;
            bool bResult = Read( temp );
            e = temp;
            return bResult;
        }
    #ifdef WIN_x64
        bool Read( size_t& e )
        {
            uint temp;
            bool bResult = Read( temp );
            e = temp;
            return bResult;
        }
    #endif
#endif
    };


    //////////////////////////////////////////////////////
    //
    // class CBufferReadStream
    //
    // Used to stream bytes into a buffer
    //
    //////////////////////////////////////////////////////
    class CBufferWriteStream : public CBufferStream
    {
        CBuffer* pBuffer;
    public:
        CBufferWriteStream ( CBuffer& source, bool bToFromNetwork = false )
            : CBufferStream ( bToFromNetwork )
            , pBuffer ( &source )
        {
        }

        virtual int             GetSize ( void ) const          { return pBuffer->GetSize (); }

        void WriteBytes ( const void* pData, int iLength, bool bToFromNetwork = false )
        {
            // Validate pos
            Seek ( Tell () );
            // Add data
            pBuffer->AddBytes ( pData, iLength, Tell (), bToFromNetwork );
            // Adjust pos
            Seek ( Tell () + iLength );
        }

        void Write ( const SString& );     // Not defined as it won't work
        void Write ( const CBuffer& );     // Not defined as it won't work
        void WriteString ( const SString& str, bool bByteLength = false, bool bDoesLengthIncludeLengthOfLength = false )
        {
            ushort usLength = (ushort)str.length ();
            if ( bDoesLengthIncludeLengthOfLength && usLength )
                usLength += bByteLength ? 1 : 2;

            if ( bByteLength )
                Write ( (uchar)usLength );
            else
                Write ( usLength );

            if ( usLength )
                WriteBytes ( &str.at ( 0 ), usLength, false );
        }

        void WriteBuffer ( const CBuffer& inBuffer )
        {
            // Write the length
            uint uiLength = (uint)inBuffer.GetSize ();
            if ( uiLength > 65534 )
            {
                Write ( (ushort)65535 );
                Write ( uiLength );
            }
            else
            {
                Write ( (ushort)uiLength );
            }

            // Write the data
            if ( uiLength )
                WriteBytes ( inBuffer.GetData (), uiLength, false );
        }

        template < class T >
        void Write ( const T& e )
        {
            WriteBytes ( &e, sizeof ( e ), m_bToFromNetwork );
        }

#ifdef ANY_x64
        // Force all these types to use 4 bytes
        void Write( unsigned long e )
        {
            Write( (uint)e );
        }
        void Write( long e )
        {
            Write( (int)e );
        }
    #ifdef WIN_x64
        void Write( size_t e )
        {
            Write( (uint)e );
        }
    #endif
#endif
    };

}
