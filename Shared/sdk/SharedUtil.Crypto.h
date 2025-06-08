/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sdk/SharedUtil.Crypto.hpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once
#include <cryptopp/base64.h>
#include <cryptopp/base32.h>
#include <cryptopp/aes.h>
#include <cryptopp/rsa.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hmac.h>
#include <cryptopp/hex.h>
#include <cryptopp/md5.h>
#include <zlib/zlib.h>
#include "SString.h"

namespace SharedUtil
{
    struct KeyPair
    {
        SString publicKey, privateKey;
    };

    inline SString Base64encode(const SString& data, const SString& variant = SString())
    {
        SString result;

        if (variant == "URL")
        {
            CryptoPP::StringSource ss(data, true, new CryptoPP::Base64URLEncoder(new CryptoPP::StringSink(result), false));
        }
        else
        {
            CryptoPP::StringSource ss(data, true, new CryptoPP::Base64Encoder(new CryptoPP::StringSink(result), false));
        }

        return result;
    }

    inline SString Base64decode(const SString& data, const SString& variant = SString())
    {
        SString result;

        if (variant == "URL")
        {
            CryptoPP::StringSource ss(data, true, new CryptoPP::Base64URLDecoder(new CryptoPP::StringSink(result)));
        }
        else
        {
            CryptoPP::StringSource ss(data, true, new CryptoPP::Base64Decoder(new CryptoPP::StringSink(result)));
        }

        return result;
    }

    inline SString Base32encode(const SString& data, const SString& variant = SString())
    {
        SString result;

        if (variant == "HEX")
        {
            CryptoPP::StringSource ss(data, true, new CryptoPP::Base32HexEncoder(new CryptoPP::StringSink(result), false));
        }
        else
        {
            CryptoPP::StringSource ss(data, true, new CryptoPP::Base32Encoder(new CryptoPP::StringSink(result), false));
        }

        return result;
    }

    inline SString Base32decode(const SString& data, const SString& variant = SString())
    {
        SString result;

        if (variant == "HEX")
        {
            CryptoPP::StringSource ss(data, true, new CryptoPP::Base32HexDecoder(new CryptoPP::StringSink(result)));
        }
        else
        {
            CryptoPP::StringSource ss(data, true, new CryptoPP::Base32Decoder(new CryptoPP::StringSink(result)));
        }

        return result;
    }

    template <class HashType>
    inline SString Hash(const SString& value)
    {
        SString  result;
        HashType hashType{};

        CryptoPP::StringSource ss(value, true, new CryptoPP::HashFilter(hashType, new CryptoPP::HexEncoder(new CryptoPP::StringSink(result))));

        return result;
    }

    template <class HmacType>
    inline SString Hmac(const SString& value, const SString& key)
    {
        SString mac;
        SString result;

        CryptoPP::HMAC<HmacType> hmac(reinterpret_cast<const CryptoPP::byte*>(key.c_str()), key.size());

        CryptoPP::StringSource ssMac(value, true, new CryptoPP::HashFilter(hmac, new CryptoPP::StringSink(mac)));
        CryptoPP::StringSource ssResult(mac, true, new CryptoPP::HexEncoder(new CryptoPP::StringSink(result)));

        return result;
    }

    inline KeyPair GenerateRsaKeyPair(const unsigned int size)
    {
        KeyPair rsaKeyPair;

        CryptoPP::AutoSeededRandomPool asrp;

        CryptoPP::InvertibleRSAFunction params;
        params.GenerateRandomWithKeySize(asrp, size);

        CryptoPP::RSA::PrivateKey rsaPrivateKey(params);
        CryptoPP::RSA::PublicKey  rsaPublicKey(params);

        CryptoPP::StringSink rawPrivateKeySink(rsaKeyPair.privateKey);
        rsaPrivateKey.DEREncode(rawPrivateKeySink);

        CryptoPP::StringSink rawPublicKeySink(rsaKeyPair.publicKey);
        rsaPublicKey.DEREncode(rawPublicKeySink);

        return rsaKeyPair;
    }

    inline SString RsaEncode(const SString& data, const SString& publicKey)
    {
        SString result;

        CryptoPP::RSA::PublicKey       rsaPublicKey;
        CryptoPP::AutoSeededRandomPool asrp;

        CryptoPP::StringSource rsaPublicKeySource(publicKey, true);
        rsaPublicKey.BERDecode(rsaPublicKeySource);

        CryptoPP::RSAES_OAEP_SHA_Encryptor rsaEncryptor(rsaPublicKey);
        CryptoPP::StringSource             ss(data, true, new CryptoPP::PK_EncryptorFilter(asrp, rsaEncryptor, new CryptoPP::StringSink(result)));

        return result;
    }

    inline SString RsaDecode(const SString& data, const SString& privateKey)
    {
        SString result;

        CryptoPP::RSA::PrivateKey      rsaPrivateKey;
        CryptoPP::AutoSeededRandomPool asrp;

        CryptoPP::StringSource rsaPrivateKeySource(privateKey, true);
        rsaPrivateKey.BERDecode(rsaPrivateKeySource);

        CryptoPP::RSAES_OAEP_SHA_Decryptor rsaDecryptor(rsaPrivateKey);
        CryptoPP::StringSource             ss(data, true, new CryptoPP::PK_DecryptorFilter(asrp, rsaDecryptor, new CryptoPP::StringSink(result)));

        return result;
    }

    inline std::pair<SString, SString> Aes128encode(const SString& sData, const SString& sKey)
    {
        using namespace CryptoPP;
        using CryptoPP::byte;

        AutoSeededRandomPool rnd;

        SString result;
        SString sIv;

        sIv.resize(AES::BLOCKSIZE);
        rnd.GenerateBlock((byte*)sIv.data(), sIv.size());

        CTR_Mode<AES>::Encryption aesEncryption;
        aesEncryption.SetKeyWithIV((byte*)sKey.data(), sKey.size(), (byte*)sIv.data());
        StringSource ss(sData, true, new StreamTransformationFilter(aesEncryption, new StringSink(result)));

        return {result, sIv};
    }

    inline SString Aes128decode(const SString& sData, const SString& sKey, SString sIv)
    {
        using namespace CryptoPP;
        using CryptoPP::byte;

        sIv.resize(AES::BLOCKSIZE);
        SString result;

        CTR_Mode<AES>::Decryption aesDecryption;
        aesDecryption.SetKeyWithIV((byte*)sKey.data(), sKey.size(), (byte*)sIv.data());
        StringSource ss(sData, true, new StreamTransformationFilter(aesDecryption, new StringSink(result)));

        return result;
    }

    inline bool StringToZLibFormat(const std::string& format, int& outResult)
    {
        int value = atoi(format.c_str());
        if ((value >= 9 && value <= 31) || (value >= -15 && value <= -9)) // allowed values: 9..31, -9..-15
        {
            outResult = value;
            return true;
        }
        return false;
    }

    inline int ZLibCompress(const std::string& input, std::string& output, const int windowBits = (int)ZLibFormat::GZIP, const int compression = 9,
                            const ZLibStrategy strategy = ZLibStrategy::DEFAULT)
    {
        z_stream stream{};

        int result = deflateInit2(&stream, compression, Z_DEFLATED, windowBits, MAX_MEM_LEVEL, (int)strategy);
        if (result != Z_OK)
            return result;

        output.resize(deflateBound(&stream, input.size())); // resize to the upper bound of what the compressed size might be

        stream.next_out = (Bytef*)output.data();
        stream.avail_out = output.size();

        stream.next_in = (z_const Bytef*)input.data();
        stream.avail_in = input.size();

        result = deflate(&stream, Z_FINISH);
        result |= deflateEnd(&stream);

        if (result == Z_STREAM_END)
            output.resize(stream.total_out); // resize to the actual size

        return result;
    }

    inline int ZLibUncompress(const std::string& input, std::string& output, int windowBits = 0)
    {
        if (windowBits == 0 && input.size() >= 2) // try to determine format automatically
        {
            if (input[0] == '\x1F' && input[1] == '\x8B')
                windowBits = (int)ZLibFormat::GZIP;
            else if (input[0] == '\x78')
                windowBits = (int)ZLibFormat::ZLIB;
            else
                windowBits = (int)ZLibFormat::ZRAW;
        }
        z_stream stream{};

        int result = inflateInit2(&stream, windowBits);
        if (result != Z_OK)
            return result;

        stream.next_in = (z_const Bytef*)input.data();
        stream.avail_in = input.size();

        // Uncompress in chunks
        std::string buffer;
        buffer.resize(std::min(stream.avail_in, 128000U)); // use input length for chunk size (capped to 128k bytes which should be efficient enough)
        while (true)
        {
            stream.next_out = (Bytef*)buffer.data();
            stream.avail_out = buffer.size();

            result = inflate(&stream, Z_NO_FLUSH);
            if (result != Z_OK && result != Z_STREAM_END)
                break;

            output.append(buffer, 0, stream.total_out - output.size()); // append only what was written to buffer

            if (result == Z_STREAM_END)
                break;
        }
        result |= inflateEnd(&stream);
        return result;
    }

}            // namespace SharedUtil
