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

		AutoSeededRandomPool rnd;

        SString result;
		SString sIv;

		SecByteBlock key(0x00, AES::DEFAULT_KEYLENGTH);
		SecByteBlock iv(0x00, AES::BLOCKSIZE);

       	memcpy(key, sKey.c_str(), sKey.size());

        sIv.resize(sizeof(iv));
        rnd.GenerateBlock((CryptoPP::byte*)sIv.data(), sIv.size());
        memcpy(iv, sIv.c_str(), sIv.size());

		CTR_Mode<AES>::Encryption aesEncryption;
        aesEncryption.SetKeyWithIV(key, sizeof(key), iv);
        StringSource ss(sData, true, new StreamTransformationFilter(aesEncryption, new StringSink(result)));

        return std::pair<SString, SString>(result, sIv);
    }

    inline SString Aes128decode(const SString& sData, const SString& sKey, const SString& sIv)
    {
		using namespace CryptoPP;

        SString result;

		SecByteBlock key(0x00, AES::DEFAULT_KEYLENGTH);
		SecByteBlock iv(0x00, AES::BLOCKSIZE);

       	memcpy(key, sKey.c_str(), sKey.size());
        memcpy(iv, sIv.c_str(), sIv.size());

		CTR_Mode<AES>::Decryption aesDecryption;
        aesDecryption.SetKeyWithIV(key, sizeof(key), iv);
        StringSource ss(sData, true, new StreamTransformationFilter(aesDecryption, new StringSink(result)));

        return result;
    }
}            // namespace SharedUtil
