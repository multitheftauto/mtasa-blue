/* CRC-32 calculator
 * Adapted from http://www.createwindow.org/programming/crc32/
 * Borrowed from Bochs x86 PC emulator(bochs)
 */

#ifndef _CRC_32_H
#define _CRC_32_H

class CRCGenerator {
    
protected:
    static class CRCGenerator *        instance;
                                CRCGenerator();
private:
    unsigned long               crc32_table[256];
    unsigned long               Reflect ( unsigned long ref, unsigned char ch );
public:
    static CRCGenerator *       GetInstance() { if ( !instance ) instance = new CRCGenerator(); return instance; }
    void                        Init(void);
    unsigned long               GetCRC ( unsigned long * buf, unsigned long buflen);
    unsigned long               GetCRC ( char * szFilename );
    unsigned long               GetCRC ( char * szFilename, unsigned long ucOldCRC );

};

#endif 

