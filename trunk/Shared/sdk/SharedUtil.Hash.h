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
        CMD5Hasher ( void );
        ~CMD5Hasher ( void );

        bool                            Calculate ( const char* szFilename, MD5& md5Result );
        bool                            Calculate ( const void* pBuffer, size_t sizeLength, MD5& md5Result );

        static void                     ConvertToHex ( const MD5& Input, char* pBuffer );
        static SString                  CalculateHexString ( const char* szFilename );
        static SString                  CalculateHexString ( const void* pBuffer, size_t sizeLength );

    private:
        void                            Init ( void );
        void                            Update ( unsigned char* input, unsigned int input_length );
        void                            Finalize ( void );

        void                            Transform ( unsigned char* pBuffer );

        static void                     Encode ( unsigned char *dest, unsigned int  *src, unsigned long length );
        static void                     Decode ( unsigned int  *dest, unsigned char *src, unsigned long length );

        static inline unsigned int      RotateLeft ( unsigned int  x, unsigned int  n );
        static inline unsigned int      F ( unsigned int  x, unsigned int  y, unsigned int  z );
        static inline unsigned int      G ( unsigned int  x, unsigned int  y, unsigned int  z );
        static inline unsigned int      H ( unsigned int  x, unsigned int  y, unsigned int  z );
        static inline unsigned int      I ( unsigned int  x, unsigned int  y, unsigned int  z );
        static inline void              FF ( unsigned int & a, unsigned int  b, unsigned int  c, unsigned int  d, unsigned int  x, unsigned int  s, unsigned int  ac );
        static inline void              GG ( unsigned int & a, unsigned int  b, unsigned int  c, unsigned int  d, unsigned int  x, unsigned int  s, unsigned int  ac );
        static inline void              HH ( unsigned int & a, unsigned int  b, unsigned int  c, unsigned int  d, unsigned int  x, unsigned int  s, unsigned int  ac );
        static inline void              II ( unsigned int & a, unsigned int  b, unsigned int  c, unsigned int  d, unsigned int  x, unsigned int  s, unsigned int  ac );

        unsigned int                    m_state[4];
        unsigned int                    m_count[2];
        unsigned char                   m_buffer[64];
        unsigned char                   m_digest[16];
    };

    void            encodeXtea                  ( unsigned int* v, unsigned int* w, unsigned int* k );
    void            decodeXTea                  ( unsigned int* v, unsigned int* w, unsigned int* k );

    void            TeaEncode                   ( const SString& str, const SString& key, SString* out );
    void            TeaDecode                   ( const SString& str, const SString& key, SString* out );

    unsigned int    HashString                  ( const char* szString );
    unsigned int    HashString                  ( const char* szString, unsigned int length );

    SString         ConvertDataToHexString      ( const void* pData, uint uiLength );
    void            ConvertHexStringToData      ( const SString& strString, void* pOutData, uint uiLength );
    void            GenerateSha256              ( const void* pData, uint uiLength, uchar output[32] );
    SString         GenerateSha256HexString     ( const void* pData, uint uiLength );
    SString         GenerateSha256HexString     ( const SString& strData );
}
