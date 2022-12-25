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
}            // namespace SharedUtil
