/* CRC-32 calculator
 * Adapted from http://www.createwindow.org/programming/crc32/
 * Borrowed from Bochs x86 PC emulator(bochs)
 */

#include <StdInc.h>

#include "crc32.h"
#include "zlib.h"

CRCGenerator* CRCGenerator::instance = 0;

unsigned long CRCGenerator::GetCRC ( char * szFilename )
{
    return GetCRC ( szFilename, 0 );
}

unsigned long CRCGenerator::GetCRC ( char * szFilename, unsigned long ucOldCRC )
{
    FILE * file = fopen ( szFilename, "rb" );
    if ( file )
    {
        fseek ( file, 0, SEEK_END );
        unsigned long ulLength = ftell ( file );

        rewind ( file );

        char * buffer = new char [ ulLength + 1 ];
        memset(buffer, 0, ulLength + 1);
        fread ( buffer, 1, ulLength, file );
     
        fclose ( file );

        unsigned long ulCRC = crc32 ( ucOldCRC, (Bytef *)buffer, ulLength);
        delete[] buffer;
        return ulCRC;
    }
    return NULL;
}

unsigned long CRCGenerator::GetBufferCRC ( const char* cpBuffer, unsigned long ulLength )
{
    return crc32 ( 0, (Bytef *)cpBuffer, ulLength);
}