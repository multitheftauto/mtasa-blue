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
#include <cryptopp/osrng.h>
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
