/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientCRC32Hasher.cpp
*  PURPOSE:     CRC32 hasher class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#include <StdInc.h>

CClientCRC32Hasher::CClientCRC32Hasher ( void )
{
    // Init
    m_bTableInitialized = false;
    m_bIsHashing = false;
    memset ( m_ulCRC32Table, 0, sizeof ( m_ulCRC32Table ) );
}


CClientCRC32Hasher::~CClientCRC32Hasher ( void )
{

}


bool CClientCRC32Hasher::Calculate ( const char* szFilename, CRC32& crcResult )
{
    //CRYPT_START
    // Initialize the buffer we got
    unsigned long ulResult = 0xFFFFFFFF;

    // Eventually calculate our CRC32 table
    CalculateTable ();

    // Open the file
    FILE* pFile = fopen ( szFilename, "rb" );
    if ( pFile )
    {
        // Read through the file hashing it
        unsigned char buffer [2048];
        size_t sizeIndex;
        size_t sizeBytesRead = fread ( buffer, 1, 2048, pFile );
        while ( sizeBytesRead > 0 )
        {
            for ( sizeIndex = 0; sizeIndex < sizeBytesRead; sizeIndex++ )
            {
                ulResult = ( ulResult >> 8 ) ^ m_ulCRC32Table [ ( ulResult & 0xFF ) ^ buffer [sizeIndex] ];
            }

            sizeBytesRead = fread ( buffer, 1, 2048, pFile );
        }


        // Close the file
        fclose ( pFile );

        // Put the CRC we got into the passed buffer and return success
        crcResult = ~ulResult;
        return true;
    }

    return false;
    //CRYPT_END
}


bool CClientCRC32Hasher::Calculate ( const char* pBuffer, size_t sizeLength, CRC32& crcResult )
{
    //CRYPT_START
    if ( sizeLength > 0 )
    {
        // Eventually calculate our CRC32 table
        CalculateTable ();

        // Initialize CRC variable
        unsigned long ulResult = 0xFFFFFFFF;

        // Read through the memory hashing it
        const unsigned char* pTemp = reinterpret_cast < const unsigned char* > ( pBuffer );
        do
        {
            ulResult = ( ulResult >> 8 ) ^ m_ulCRC32Table [ ( ulResult & 0xFF ) ^ *pTemp++ ];
        }
        while ( --sizeLength );

        // Put the CRC we got into the passed buffer and return success
        crcResult = ~ulResult;
    }

    return true;
    //CRYPT_END
}


bool CClientCRC32Hasher::Start ( void )
{
    // Eventually calculate our CRC32 table
    CalculateTable ();

    // Initialize the CRC variable
    m_ulHash = 0xFFFFFFFF;

    // We're now hashing
    m_bIsHashing = true;
    return true;
}


bool CClientCRC32Hasher::Append ( const char* pBuffer, size_t sizeLength )
{
    //CRYPT_START
    // Are we hashing?
    if ( m_bIsHashing )
    {
        // Read through the memory appending the hash to m_ulHash
        const unsigned char* pTemp = reinterpret_cast < const unsigned char* > ( pBuffer );
        do
        {
            m_ulHash = ( m_ulHash >> 8 ) ^ m_ulCRC32Table [ ( m_ulHash & 0xFF ) ^ *pTemp++ ];
        }
        while ( --sizeLength );

        // Success
        return true;
    }

    return false;
    //CRYPT_END
}


bool CClientCRC32Hasher::Finish ( CRC32& crcResult )
{
    // Are we hashing?
    if ( m_bIsHashing )
    {
        // Put the result in crcResult (inverted)
        crcResult = ~m_ulHash;

        // We're no longer hashing
        m_bIsHashing = false;
        return true;
    }

    return false;
}


void CClientCRC32Hasher::CalculateTable ( void )
{
    //CRYPT_START
    // No point doing this twice
    if ( !m_bTableInitialized )
    {
        unsigned long ulPolynomial = 0xEDB88320;
        unsigned long ulCRC;

        int i, j;
        for ( i = 0; i <= 0xFF; i++ )
        {
            ulCRC = i;
            for ( j = 8; j > 0; j-- )
            {
                if ( ulCRC & 1 )
                {
                    ulCRC = ( ulCRC >> 1 ) ^ ulPolynomial;
                }
                else
                {
                    ulCRC >>= 1;
                }
            }

            m_ulCRC32Table [i] = ulCRC;
        }
    }
    //CRYPT_END
}
