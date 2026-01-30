/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        ceflauncher_DLL/CCefAppAuth.h
 *  PURPOSE:     IPC authentication helpers for render process
 *
 *****************************************************************************/
#pragma once

#include <cef3/cef/include/cef_process_message.h>
#include <cef3/cef/include/cef_values.h>
#include <string>

namespace CefAppAuth
{
    // Thread-local storage for auth code in render process
    inline std::string& AuthCodeStorage()
    {
        static std::string value;
        return value;
    }

    // Appends auth code to TriggerLuaEvent IPC message
    // Returns true if code was appended, false if message should be blocked
    [[nodiscard]] inline bool AppendAuthCodeToMessage(CefRefPtr<CefProcessMessage> message)
    {
        if (!message) [[unlikely]]
            return false;

        auto& authCode = AuthCodeStorage();

        // Block messages until initialized (prevents race condition)
        if (authCode.empty()) [[unlikely]]
            return false;

        CefRefPtr<CefListValue> args = message->GetArgumentList();
        const auto              size = args->GetSize();
        args->SetSize(size + 1);
        args->SetString(size, authCode.c_str());
        return true;
    }
}
