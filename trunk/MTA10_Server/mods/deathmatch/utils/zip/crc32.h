/* CRC-32 calculator
 * Adapted from http://www.createwindow.org/programming/crc32/
 * Borrowed from Bochs x86 PC emulator(bochs)
 */

#ifndef _CRCGENERATOR_H
#define _CRCGENERATOR_H

#include <cstring>

class CRCGenerator
{
public:
    static unsigned long    GetCRCFromBuffer    ( const char* pBuf, size_t sizeBuffer );
    static unsigned long    GetCRCFromBuffer    ( const char* pBuf, size_t sizeBuffer, unsigned long ulOldCRC );

    static unsigned long    GetCRCFromFile      ( const char* szFilename );
    static unsigned long    GetCRCFromFile      ( const char* szFilename, unsigned long ucOldCRC );
};

#endif 
