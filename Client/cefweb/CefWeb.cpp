/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CefWeb.cpp
 *  PURPOSE:     CEF web browser module initialization entry point
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <memory>
#include <SharedUtil.Memory.h>

CCoreInterface*         g_pCore = nullptr;
CLocalizationInterface* g_pLocalization = nullptr;

namespace
{
    [[nodiscard]] inline bool InitializeGlobalInterfaces(CCoreInterface* pCore) noexcept
    {
        if (!pCore) [[unlikely]]
            return false;

        g_pCore = pCore;
        g_pLocalization = pCore->GetLocalization();

        // Localization is critical for browser GUI (request dialogs, etc.)
        if (!g_pLocalization) [[unlikely]]
            return false;

        return true;
    }

    //
    // Performs required runtime initialization
    // Must be called on the main thread before creating CWebCore
    //
    inline void PerformRuntimeInitialization() noexcept
    {
        // Ensure main thread identification is consistent
        IsMainThread();

        // Set up memory allocation failure handler for CEF processes
        SharedUtil::SetMemoryAllocationFailureHandler();
    }
}  // namespace

//
// DLL export: Initialize the web browser subsystem
// Called by CCore::GetWebCore() during initialization
// Returns CWebCoreInterface pointer on success, nullptr on failure
// Thread safety: Must be called from the main thread only
//
extern "C" _declspec(dllexport) CWebCoreInterface* InitWebCoreInterface(CCoreInterface* pCore)
{
    // Validate and initialize global interfaces
    if (!InitializeGlobalInterfaces(pCore)) [[unlikely]]
        return nullptr;

    // Perform runtime initialization
    PerformRuntimeInitialization();

    // Create and return the web core instance
    // Using make_unique for exception safety, then releasing ownership to caller
    return std::make_unique<CWebCore>().release();
}
