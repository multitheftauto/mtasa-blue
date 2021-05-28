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

		memcpy(iv, sIv.c_str(), CryptoPP::AES::BLOCKSIZE );
		memcpy(key, sKey.c_str(), CryptoPP::AES::DEFAULT_KEYLENGTH);

		CryptoPP::AES::Encryption aesEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
		CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption( aesEncryption, iv );

		try
		{
			CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink( result ) );
			stfEncryptor.Put( reinterpret_cast<const unsigned char*>( sData.c_str() ), sData.length() );
			stfEncryptor.MessageEnd();
		}
		catch(const CryptoPP::Exception& exception) {
			return exception.what();
		}

		return result;
    }

    inline SString Aes128decode(const SString& sData, const SString& sKey, const SString& sIv)
    {
		SString result;

		CryptoPP::byte iv[ CryptoPP::AES::BLOCKSIZE ];
		CryptoPP::byte key[ CryptoPP::AES::DEFAULT_KEYLENGTH ];

		memcpy(iv, sIv.c_str(), CryptoPP::AES::BLOCKSIZE );
		memcpy(key, sKey.c_str(), CryptoPP::AES::DEFAULT_KEYLENGTH);

		CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
    	CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption( aesDecryption, iv );

		try
		{
			CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink( result ) );
			stfDecryptor.Put( reinterpret_cast<const unsigned char*>( sData.c_str() ), sData.size() );
			stfDecryptor.MessageEnd();
		}
		catch(const CryptoPP::Exception& exception) {
			return exception.what();
		}

		return result;
    }
}            // namespace SharedUtil
