/* CRC-32 calculator
 * Adapted from http://www.createwindow.org/programming/crc32/
 * Borrowed from Bochs x86 PC emulator(bochs)
 */

#include "StdInc.h"
#include "bochs_crc32.h"
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
    FILE* pFile = File::FopenExclusive ( szFilename, "rb" );
    if ( pFile )
    {
        unsigned long ulCRC = ucOldCRC;

        constexpr size_t bufferSize = 65536;
        char pBuffer[bufferSize];
        while (true)
        {
            size_t sizeRead = fread ( pBuffer, 1, bufferSize, pFile );
            if (sizeRead == 0)
            {
                if (ferror(pFile))
                {
                    fclose(pFile);
                    errno = EIO;
                    return 0;
                }
                break;
            }
            ulCRC = crc32 ( ulCRC, (Bytef*) pBuffer, sizeRead );
        }

        fclose ( pFile );
        errno = 0;
        return ulCRC;
    }

    return 0;
}
