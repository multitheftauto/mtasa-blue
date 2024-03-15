/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.Hash.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include "SharedUtil.IntTypes.h"
#include "SString.h"

namespace EHashFunction
{
    enum EHashFunctionType
    {
        MD5,
        SHA1,
        SHA224,
        SHA256,
        SHA384,
        SHA512,
    };
}
using EHashFunction::EHashFunctionType;

enum class HashFunctionType
{
    MD5,
    SHA1,
    SHA224,
    SHA256,
    SHA384,
    SHA512,
    HMAC,
};

enum class PasswordHashFunction
{
    Bcrypt
};

enum class StringEncodeFunction
{
    TEA,
    AES128,
    RSA,
    BASE64,
    BASE32
};

enum class KeyPairAlgorithm
{
    RSA
};

enum class HmacAlgorithm
{
    MD5,
    SHA1,
    SHA224,
    SHA256,
    SHA384,
    SHA512,
};

namespace SharedUtil
{
    struct MD5
    {
        uchar data[16];
    };

    class CMD5Hasher
    {
    public:
        CMD5Hasher();
        ~CMD5Hasher();

        bool Calculate(const char* szFilename, MD5& md5Result);
        bool Calculate(const void* pBuffer, size_t sizeLength, MD5& md5Result);

        static void    ConvertToHex(const MD5& Input, char* pBuffer);
        static SString CalculateHexString(const char* szFilename);
        static SString CalculateHexString(const void* pBuffer, size_t sizeLength);

        void                 Init();
        void                 Update(std::uint8_t* input, std::uint32_t input_length);
        void                 Finalize();
        const std::uint8_t* GetResult() const;

    private:
        void Transform(std::uint8_t* pBuffer);

        static void Encode(std::uint8_t* dest, std::uint32_t* src, std::uint32_t length);
        static void Decode(std::uint32_t* dest, std::uint8_t* src, std::uint32_t length);

        static inline std::uint32_t RotateLeft(std::uint32_t x, std::uint32_t n);
        static inline std::uint32_t F(std::uint32_t x, std::uint32_t y, std::uint32_t z);
        static inline std::uint32_t G(std::uint32_t x, std::uint32_t y, std::uint32_t z);
        static inline std::uint32_t H(std::uint32_t x, std::uint32_t y, std::uint32_t z);
        static inline std::uint32_t I(std::uint32_t x, std::uint32_t y, std::uint32_t z);
        static inline void         FF(std::uint32_t& a, std::uint32_t b, std::uint32_t c, std::uint32_t d, std::uint32_t x, std::uint32_t s, std::uint32_t ac);
        static inline void         GG(std::uint32_t& a, std::uint32_t b, std::uint32_t c, std::uint32_t d, std::uint32_t x, std::uint32_t s, std::uint32_t ac);
        static inline void         HH(std::uint32_t& a, std::uint32_t b, std::uint32_t c, std::uint32_t d, std::uint32_t x, std::uint32_t s, std::uint32_t ac);
        static inline void         II(std::uint32_t& a, std::uint32_t b, std::uint32_t c, std::uint32_t d, std::uint32_t x, std::uint32_t s, std::uint32_t ac);

        std::uint32_t  m_state[4];
        std::uint32_t  m_count[2];
        std::uint8_t m_buffer[64];
        std::uint8_t m_digest[16];
    };

    void encodeXtea(std::uint32_t* v, std::uint32_t* w, std::uint32_t* k);
    void decodeXTea(std::uint32_t* v, std::uint32_t* w, std::uint32_t* k);

    void TeaEncode(const SString& str, const SString& key, SString* out);
    void TeaDecode(const SString& str, const SString& key, SString* out);

    std::uint32_t HashString(const char* szString);
    std::uint32_t HashString(const char* szString, std::uint32_t length);

#ifdef SDK_WITH_BCRYPT
    SString BcryptHash(const SString& password, SString salt = "", std::size_t cost = 10);
    bool    BcryptVerify(const SString& password, const SString& hash);
#endif

    SString ConvertDataToHexString(const void* pData, uint uiLength);
    void    ConvertHexStringToData(const SString& strString, void* pOutData, uint uiLength);
    void    GenerateSha256(const void* pData, uint uiLength, uchar output[32]);
    SString GenerateSha256HexString(const void* pData, uint uiLength);
    SString GenerateSha256HexString(const SString& strData);
    SString GenerateSha256HexStringFromFile(const SString& strFilename);
    SString GenerateHashHexString(EHashFunctionType hashFunction, const void* pData, uint uiLength);
    SString GenerateHashHexString(EHashFunctionType hashFunction, const SString& strData);
    SString GenerateHashHexStringFromFile(EHashFunctionType hashFunction, const SString& strFilename, int iMaxSize = INT_MAX, int iOffset = 0);
}            // namespace SharedUtil
