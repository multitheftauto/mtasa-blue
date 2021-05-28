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
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include "SString.h"

namespace SharedUtil
{
    inline SString Base64encode(const SString& data)
    {
        SString                result;
        CryptoPP::StringSource ss(data, true, new CryptoPP::Base64Encoder(new CryptoPP::StringSink(result), false));            // Memory is freed automatically

        return result;
    }

    inline SString Base64decode(const SString& data)
    {
        SString                result;
        CryptoPP::StringSource ss(data, true, new CryptoPP::Base64Decoder(new CryptoPP::StringSink(result)));            // Memory is freed automatically

        return result;
    }

    inline SString Aes128encode(const SString& sData, const SString& sKey, const SString& sIv)
    {
        SString result;

        CryptoPP::byte iv[ CryptoPP::AES::BLOCKSIZE ];
        CryptoPP::byte key[ CryptoPP::AES::DEFAULT_KEYLENGTH ];

        memcpy(iv, sIv.c_str(), sIv.length());
        memcpy(key, sKey.c_str(), sKey.length());

        CryptoPP::AES::Encryption aesEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
        CryptoPP::CTR_Mode_ExternalCipher::Encryption ctrEncryption(aesEncryption, iv);

        try
        {
            CryptoPP::StreamTransformationFilter stfEncryptor(ctrEncryption, new CryptoPP::StringSink(result));
            stfEncryptor.Put((const CryptoPP::byte*)&sData[0], sData.size());
            stfEncryptor.MessageEnd();
        }
        catch (const CryptoPP::Exception& exception)
        {
            return "";
        }

        return result;
    }

    inline SString Aes128decode(const SString& sData, const SString& sKey, const SString& sIv)
    {
        SString result;

        CryptoPP::byte iv[ CryptoPP::AES::BLOCKSIZE ];
        CryptoPP::byte key[ CryptoPP::AES::DEFAULT_KEYLENGTH ];

        memcpy(iv, sIv.c_str(), sIv.length());
        memcpy(key, sKey.c_str(), sKey.length());

        CryptoPP::AES::Encryption aesDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
        CryptoPP::CTR_Mode_ExternalCipher::Encryption ctrDecryption(aesDecryption, iv);

        try
        {
            CryptoPP::StreamTransformationFilter stfDecryptor(ctrDecryption, new CryptoPP::StringSink(result));
            stfDecryptor.Put((const CryptoPP::byte*)&sData[0], sData.size());
            stfDecryptor.MessageEnd();
        }
        catch(const CryptoPP::Exception& exception)
        {
            return "";
        }

        return result;
    }
}            // namespace SharedUtil
