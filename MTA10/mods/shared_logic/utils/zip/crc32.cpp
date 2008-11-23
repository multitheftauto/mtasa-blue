/* CRC-32 calculator
 * Adapted from http://www.createwindow.org/programming/crc32/
 * Borrowed from Bochs x86 PC emulator(bochs)
 */

#include <StdInc.h>

#include "crc32.h"
#include "zlib.h"

CRCGenerator* CRCGenerator::instance = 0;
CRCGenerator::CRCGenerator() {
	Init();
}

void CRCGenerator::Init ( void ) {
    unsigned long POLYNOMIAL = 0x04c11db7;
    int i;

    for(i = 0; i<0xFF; i++) {
        int j;
        crc32_table[i]=Reflect(i,8) << 24;

        for(j=0; j<8; j++)
            crc32_table[i] = (crc32_table[i]<<1)^(crc32_table[i] & (1<<31) ? POLYNOMIAL : 0);

        crc32_table[i] = Reflect(crc32_table[i], 32);
    }
}

unsigned long CRCGenerator::Reflect ( unsigned long ref, unsigned char ch ) {
    unsigned long value(0);
    int i;

    for(i=1; i<(ch+1); i++) {
        if(ref & 1)
            value |= 1 << (ch-i);
        ref >>= 1;
    }

    return value;
}

unsigned long CRCGenerator::GetCRC ( unsigned long * buf, unsigned long buflen ) {
    unsigned long ulCRC = 0xFFFFFFFF;
    unsigned long len = buflen;
    unsigned char * buffer = (unsigned char *)buf;

    while(len--)
        ulCRC=(ulCRC>>8)^crc32_table[(ulCRC & 0xFF)^*buffer++];

    return ulCRC ^ 0xFFFFFFFF;
}

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
        // I assume the zlib version is faster, uncomment the second part if not
        unsigned long ulCRC = crc32 ( ucOldCRC, (Bytef *)buffer, ulLength);// GetCRC ( (unsigned long *)buffer, strlen(buffer) + 1 );
        delete[] buffer;
        return ulCRC;
    }
    return NULL;
}
