/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sdk/SharedUtil.Crypto.hpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
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
}            // namespace SharedUtil
