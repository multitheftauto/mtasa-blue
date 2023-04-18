/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/core/DXHook/CDirect3DHookManager.cpp
 *  PURPOSE:     Direct3D version independant hook manager
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CDirect3DHookManager.h"
#include "CDirect3DHook9.h"
#include <filesystem>

namespace fs = std::filesystem;

extern fs::path g_gtaDirectory;

CDirect3DHookManager::~CDirect3DHookManager()
{
    if (m_hook)
    {
        m_hook->RemoveHook();
        delete m_hook;
    }
}

void CDirect3DHookManager::ApplyHook()
{
    if (m_hook || UsingAltD3DSetup())
        return;

    HMODULE d3d9{};

    // Try to load d3d9.dll from GTA directory to enable pre-installed graphic mods.
    {
        const fs::path dll = g_gtaDirectory / "d3d9.dll";

        if (std::error_code ignore{}; fs::is_regular_file(dll, ignore))
        {
            d3d9 = LoadLibraryW(dll.wstring().c_str());
        }
    }

    if (d3d9 || LoadLibraryW(L"d3d9.dll"))
    {
        m_hook = new CDirect3DHook9();
        m_hook->ApplyHook();
    }
}

void CDirect3DHookManager::RemoveHook()
{
    if (m_hook)
    {
        m_hook->RemoveHook();
    }
}
