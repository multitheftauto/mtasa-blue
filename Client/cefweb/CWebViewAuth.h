/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        cefweb/CWebViewAuth.h
 *  PURPOSE:     IPC authentication helpers for browser message validation
 *
 *****************************************************************************/
#pragma once

#include "CWebView.h"
#include "CWebCore.h"
#include <cef3/cef/include/cef_values.h>
#include <algorithm>   // For std::min
#include <climits>     // For INT_MAX
#include <functional>  // For std::bind
#include <string>
#include <vector>

// Note: These inline functions access private members of CWebView (m_bHasInputFocus, m_pEventsInterface).
// This is valid because this header is #included in CWebView.cpp, making these functions part of that
// translation unit where they have access to CWebView's private members (similar to friend functions).
namespace WebViewAuth
{
    // Validates IPC auth code for TriggerLuaEvent messages
    [[nodiscard]] inline bool ValidateTriggerEventAuthCode(CefRefPtr<CefListValue> argList)
    {
        if (!argList) [[unlikely]]
            return false;

        const auto argCount = argList->GetSize();
        if (argCount < 3) [[unlikely]]  // Need at least: eventName, numArgs, authCode
            return false;

        // SECURITY: Type validation BEFORE expensive auth operations (DoS prevention)
        // Reject malformed messages early without crypto comparison overhead
        if (argList->GetType(0) != VTYPE_STRING) [[unlikely]]  // eventName must be STRING
            return false;
        if (argList->GetType(1) != VTYPE_INT) [[unlikely]]  // numArgs must be INT
            return false;

        const auto authCodeIndex = static_cast<int>(argCount - 1);
        if (argList->GetType(authCodeIndex) != VTYPE_STRING) [[unlikely]]
            return false;

        // Must read from webCore->m_AuthCode (browser process storage)
        // Cannot use WebAppAuth::GetSharedAuthCode() - that's render process memory!
        if (!g_pCore || !IsReadablePointer(g_pCore, sizeof(void*))) [[unlikely]]
            return false;

        const auto* const webCore = static_cast<CWebCore*>(g_pCore->GetWebCore());
        if (!webCore || !IsReadablePointer(webCore, sizeof(void*))) [[unlikely]]
            return false;

        const auto& authCode = webCore->m_AuthCode;
        if (authCode.empty()) [[unlikely]]
            return false;

        // Compare auth codes (use ToString() which returns std::string)
        const std::string receivedCode = argList->GetString(authCodeIndex).ToString();
        if (authCode != receivedCode) [[unlikely]]
            return false;

        return true;
    }

    // Gets the maximum argument index, excluding the auth code
    [[nodiscard]] inline int GetTriggerEventMaxArgIndex(CefRefPtr<CefListValue> argList, const int numArgs) noexcept
    {
        if (!argList) [[unlikely]]
            return 0;

        const auto argCount = argList->GetSize();
        const auto authCodeIndex = static_cast<int>(argCount - 1);

        // Overflow protection: ensure numArgs + 2 won't overflow
        if (numArgs < 0 || numArgs > INT_MAX - 2) [[unlikely]]
            return authCodeIndex;

        return std::min(numArgs + 2, authCodeIndex);
    }

    // Handles TriggerLuaEvent message with auth validation
    inline bool HandleTriggerLuaEvent(CWebView* pWebView, CefRefPtr<CefListValue> argList, const bool isLocal)
    {
        if (!pWebView || !argList) [[unlikely]]
            return true;  // Return true = handled (rejected), not false which would bypass

        if (!isLocal) [[unlikely]]
            return true;

        if (!ValidateTriggerEventAuthCode(argList)) [[unlikely]]
            return true;

        // Type validation already done in ValidateTriggerEventAuthCode
        const CefString eventName = argList->GetString(0);
        const int       numArgs = argList->GetInt(1);

        const auto argCount = argList->GetSize();

        // Validate numArgs matches actual arguments present in message
        // Expected structure: [eventName, numArgs, arg0..argN-1, authCode]
        // Therefore: argCount should equal numArgs + 3 (eventName + numArgs + authCode)
        if (const auto expectedArgCount = numArgs + 3; expectedArgCount < 3 || expectedArgCount != static_cast<int>(argCount)) [[unlikely]]
            return true;  // Reject: numArgs mismatch (confusion attack prevention)

        std::vector<std::string> args;
        args.reserve(numArgs > 0 ? static_cast<std::size_t>(numArgs) : 0);

        const int maxArgIndex = GetTriggerEventMaxArgIndex(argList, numArgs);

        for (int i = 2; i < maxArgIndex; ++i)
        {
            // Type check: all arguments must be STRING
            if (argList->GetType(i) != VTYPE_STRING) [[unlikely]]
                return true;

            args.emplace_back(argList->GetString(i).ToString());
        }

        // Use QueueBrowserEvent for UAF-safe event dispatch (generation token pattern)
        pWebView->QueueBrowserEvent("OnProcessMessageReceived1",
                                    [eventNameStr = SString(eventName), args = std::move(args)](CWebBrowserEventsInterface* iface) mutable
                                    { iface->Events_OnTriggerEvent(eventNameStr, args); });
        return true;
    }

    // Handles InputFocus message (no auth needed - internal CEF event, not user-triggered)
    inline bool HandleInputFocus(CWebView* pWebView, CefRefPtr<CefListValue> argList, const bool isLocal)
    {
        if (!pWebView || !argList) [[unlikely]]
            return true;  // Return true = handled (rejected), not false which would bypass

        if (!isLocal) [[unlikely]]
            return true;

        if (argList->GetSize() < 1 || argList->GetType(0) != VTYPE_BOOL) [[unlikely]]
            return true;

        pWebView->SetInputFocus(argList->GetBool(0));

        // Use QueueBrowserEvent for UAF-safe event dispatch (generation token pattern)
        const bool hasFocus = pWebView->HasInputFocus();
        pWebView->QueueBrowserEvent("OnProcessMessageReceived2",
                                    [focus = hasFocus](CWebBrowserEventsInterface* iface) { iface->Events_OnInputFocusChanged(focus); });
        return true;
    }
}
