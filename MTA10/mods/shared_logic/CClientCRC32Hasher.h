/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientCRC32Hasher.h
*  PURPOSE:     CRC32 hasher class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

// ## TODO ## : Find out why it doesn't give the correct hash

#ifndef __CCLIENTCRC32HASHER_H
#define __CCLIENTCRC32HASHER_H

#include <cstdlib>

typedef unsigned long CRC32;

class CClientCRC32Hasher
{
public:
                    CClientCRC32Hasher      ( void );
                    ~CClientCRC32Hasher     ( void );

    bool            Calculate               ( const char* szFilename, CRC32& crcResult );
    bool            Calculate               ( const char* pBuffer, size_t sizeLength, CRC32& crcResult );

    bool            Start                   ( void );
    bool            Append                  ( const char* pBuffer, size_t sizeLength );
    bool            Finish                  ( CRC32& crcResult );

private:
    void            CalculateTable          ( void );

    bool            m_bTableInitialized;
    unsigned long   m_ulCRC32Table [256];

    // Variables used for continous hashing
    bool            m_bIsHashing;
    unsigned long   m_ulHash;
};

#endif
