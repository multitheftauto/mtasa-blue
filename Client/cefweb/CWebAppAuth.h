/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        cefweb/CWebAppAuth.h
 *  PURPOSE:     IPC authentication code generation for browser process
 *
 *****************************************************************************/
#pragma once

#include <cef3/cef/include/cef_command_line.h>
#include <array>
#include <chrono>
#include <mutex>
#include <random>
#include <string>

// Forward declarations
class CWebCore;
class CCoreInterface;
extern CCoreInterface* g_pCore;

// Note: IsReadablePointer is available via StdInc.h which includes SharedUtil.h

namespace WebAppAuth
{
    // Shared auth code storage (used by both generation and validation)
    inline std::string& GetSharedAuthCode()
    {
        static std::string sharedAuthCode;
        return sharedAuthCode;
    }

    inline std::mutex& GetSharedAuthMutex()
    {
        static std::mutex sharedMutex;
        return sharedMutex;
    }

    inline bool& GetSyncedToWebCoreFlag()
    {
        static bool syncedToWebCore = false;
        return syncedToWebCore;
    }

    // Thread-safe auth code generation
    inline std::mutex& GetAuthCodeMutex()
    {
        static std::mutex mutex;
        return mutex;
    }

    // Auth code configuration
    inline constexpr std::size_t AUTH_CODE_LENGTH = 30;
    inline constexpr char        AUTH_CODE_MIN_CHAR = 'A';
    inline constexpr char        AUTH_CODE_MAX_CHAR = 'Z';

    // Generates random 30-character auth code (A-Z)
    [[nodiscard]] inline std::string GenerateAuthCode()
    {
        std::array<char, AUTH_CODE_LENGTH> buffer{};

        // Use mt19937 with time-based seed (fast, cryptographic strength not needed for DoS prevention)
        static std::mt19937                rng(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
        std::uniform_int_distribution<int> dist(0, AUTH_CODE_MAX_CHAR - AUTH_CODE_MIN_CHAR);

        for (auto& ch : buffer)
            ch = static_cast<char>(AUTH_CODE_MIN_CHAR + dist(rng));

        return std::string(buffer.data(), buffer.size());
    }

    // Generates and attaches auth code to child process command line
    inline void AttachAuthCodeToCommandLine(CefRefPtr<CefCommandLine> commandLine)
    {
        if (!commandLine) [[unlikely]]
            return;

        const std::lock_guard<std::mutex> lock{GetSharedAuthMutex()};

        // Always use webCore->m_AuthCode (already populated in CWebCore constructor)
        // No need for fallback - webCore is guaranteed to exist before this is called
        if (!::g_pCore || !IsReadablePointer(::g_pCore, sizeof(void*))) [[unlikely]]
            return;

        auto* const webCore = static_cast<CWebCore*>(::g_pCore->GetWebCore());
        if (!webCore || !IsReadablePointer(webCore, sizeof(void*))) [[unlikely]]
            return;

        // webCore->m_AuthCode already populated in CWebCore::CWebCore()
        if (webCore->m_AuthCode.empty()) [[unlikely]]
            return;

        commandLine->AppendSwitchWithValue("kgfiv8n", webCore->m_AuthCode);
    }
}
