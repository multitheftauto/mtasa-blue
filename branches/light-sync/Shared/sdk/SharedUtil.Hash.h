/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.Hash.h
*  PURPOSE:
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

namespace SharedUtil
{
    struct MD5
    {
        uchar data [16];
    };

    class CMD5Hasher
    {
    public:
                                        CMD5Hasher              ( void );
                                        ~CMD5Hasher             ( void );

        bool                            Calculate               ( const char* szFilename, MD5& md5Result );
        bool                            Calculate               ( const void* pBuffer, size_t sizeLength, MD5& md5Result );

        static void                     ConvertToHex            ( const MD5& Input, char* pBuffer );
        static SString                  CalculateHexString      ( const char* szFilename );
        static SString                  CalculateHexString      ( const void* pBuffer, size_t sizeLength );

    private:
        void                            Init                    ( void );
        void                            Update                  ( unsigned char* input, unsigned int input_length );
        void                            Finalize                ( void );

        void                            Transform               ( unsigned char* pBuffer );

        static void                     Encode                  ( unsigned char *dest, unsigned long *src, unsigned long length );
        static void                     Decode                  ( unsigned long *dest, unsigned char *src, unsigned long length );

        static inline unsigned long     RotateLeft             ( unsigned long x, unsigned long n );
        static inline unsigned long     F                       ( unsigned long x, unsigned long y, unsigned long z );
        static inline unsigned long     G                       ( unsigned long x, unsigned long y, unsigned long z );
        static inline unsigned long     H                       ( unsigned long x, unsigned long y, unsigned long z );
        static inline unsigned long     I                       ( unsigned long x, unsigned long y, unsigned long z );
        static inline void              FF                      ( unsigned long& a, unsigned long b, unsigned long c, unsigned long d, unsigned long x, unsigned long s, unsigned long ac );
        static inline void              GG                      ( unsigned long& a, unsigned long b, unsigned long c, unsigned long d, unsigned long x, unsigned long s, unsigned long ac );
        static inline void              HH                      ( unsigned long& a, unsigned long b, unsigned long c, unsigned long d, unsigned long x, unsigned long s, unsigned long ac );
        static inline void              II                      ( unsigned long& a, unsigned long b, unsigned long c, unsigned long d, unsigned long x, unsigned long s, unsigned long ac );

        unsigned long                   m_state [4];
        unsigned long                   m_count [2];
        unsigned char                   m_buffer [64];
        unsigned char                   m_digest [16];
    };

}
