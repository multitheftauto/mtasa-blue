/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        launcher/CDynamicLibrary.h
 *  PURPOSE:     Dynamic library handling class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#ifdef _WIN32
#   include <windows.h>
#else
#   include <dlfcn.h>
using HMODULE = void*;
#endif

class CDynamicLibrary
{
public:
    CDynamicLibrary() noexcept;
    ~CDynamicLibrary() noexcept;

    bool Load(const char* szFilename) noexcept;
    void Unload() noexcept;
    bool IsLoaded() const noexcept;

    void* GetProcedureAddress(const char* szProcName) noexcept;

private:
    HMODULE m_hModule = nullptr;
};
