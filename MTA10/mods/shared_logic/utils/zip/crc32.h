/* CRC-32 calculator
 * Adapted from http://www.createwindow.org/programming/crc32/
 * Borrowed from Bochs x86 PC emulator(bochs)
 */

#ifndef _CRC_32_H
#define _CRC_32_H

class CRCGenerator {
    
protected:
    static class CRCGenerator *        instance;
public:
    static CRCGenerator *       GetInstance() { if ( !instance ) instance = new CRCGenerator(); return instance; }
    unsigned long               GetCRC ( char * szFilename );
    unsigned long               GetCRC ( char * szFilename, unsigned long ucOldCRC );
    unsigned long               GetBufferCRC ( const char* cpBuffer, unsigned long ulLength );


};

#endif 

