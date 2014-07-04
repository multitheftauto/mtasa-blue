/* CRC-32 calculator
 * Adapted from http://www.createwindow.org/programming/crc32/
 * Borrowed from Bochs x86 PC emulator(bochs)
 */

#include "StdInc.h"
#include "crc32.h"
#include <stdio.h>
#include <cstring>
#include "zlib.h"

unsigned long CRCGenerator::GetCRCFromBuffer ( const char* pBuf, size_t sizeBuffer )
{
    return crc32 ( 0, (Bytef*) pBuf, sizeBuffer );
}


unsigned long CRCGenerator::GetCRCFromBuffer ( const char* pBuf, size_t sizeBuffer, unsigned long ulOldCRC )
{
    return crc32 ( ulOldCRC, (Bytef*) pBuf, sizeBuffer );
}


unsigned long CRCGenerator::GetCRCFromFile ( const char* szFilename )
{
    // Generate the CRC without the last CRC
    return GetCRCFromFile ( szFilename, 0 );
}


unsigned long CRCGenerator::GetCRCFromFile ( const char* szFilename, unsigned long ucOldCRC )
{
    // Open the file
    FILE* pFile = fopen ( szFilename, "rb" );
    if ( pFile )
    {
        // Start at the old CRC
        unsigned long ulCRC = ucOldCRC;

        // While we're not at the end
        char pBuffer [4096];
        do
        {
            // Try to read 4096 bytes. It returns number of bytes actually read.
            // Then CRC that using the CRC from last loop as beginning. This should
            // be faster/more compatible than allocating a huge buffer for the entire
            // file.
            size_t sizeRead = fread ( pBuffer, 1, 4096, pFile );
            ulCRC = crc32 ( ulCRC, (Bytef*) pBuffer, sizeRead );
        }
        while ( !feof ( pFile ) );

        // Close it and return the CRC
        fclose ( pFile );
        return ulCRC;
    }

    // Not exist
    return 0;
}
