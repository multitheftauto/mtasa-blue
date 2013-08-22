/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/CChecksum.h
*  PURPOSE:     Checksum class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CChecksum_H
#define __CChecksum_H

// Depends on CMD5Hasher and CRCGenerator

class CChecksum
{
public:
    // Initialize to zeros
    CChecksum ()
    {
        ulCRC = 0;
        memset ( mD5, 0, sizeof ( mD5 ) );
    }

    // Comparison operators
    bool operator == ( const CChecksum& other ) const
    {
        return ulCRC == other.ulCRC
            && memcmp ( this, &other, sizeof ( CChecksum ) ) == 0;
    }

    bool operator != ( const CChecksum& other ) const
    {
        return !operator == ( other );
    }

    // If other MD5 is 0, only compare the CRC portions
    bool CompareWithLegacy ( const CChecksum& other ) const
    {
        if ( operator == ( other ) )
            return true;
        CChecksum legacy = CChecksum ();
        legacy.ulCRC = ulCRC;
        return legacy.operator == ( other );
    }

    // static generators
    static CChecksum GenerateChecksumFromFile ( const SString& strFilename )
    {
        CChecksum result;
        result.ulCRC = CRCGenerator::GetCRCFromFile ( strFilename );
        CMD5Hasher ().Calculate ( strFilename, (MD5&)result.mD5 );
        return result;
    }

    static CChecksum GenerateChecksumFromBuffer ( const char* cpBuffer, unsigned long ulLength )
    {
        CChecksum result;
        result.ulCRC = CRCGenerator::GetCRCFromBuffer ( cpBuffer, ulLength );
        CMD5Hasher ().Calculate ( cpBuffer, ulLength, (MD5&)result.mD5 );
        return result;
    }

    unsigned long ulCRC;
    unsigned char mD5 [16];
};

#endif
