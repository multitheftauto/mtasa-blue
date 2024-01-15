/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        launcher/CDynamicLibrary.cpp
 *  PURPOSE:     Dynamic library handling class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "CDynamicLibrary.h"

CDynamicLibrary::CDynamicLibrary() noexcept {}

CDynamicLibrary::~CDynamicLibrary() noexcept
{
    // Make sure we unload
    Unload();
}

bool CDynamicLibrary::Load(const char* szFilename) noexcept
{
    // Unload the previous library
    Unload();

    // Load the new library
    #ifdef _WIN32
    m_hModule = LoadLibrary(szFilename);
    #else
    m_hModule = dlopen(szFilename, RTLD_NOW);

    // Output error if needed
    if (!m_hModule)
        printf("%s\n", dlerror());
    #endif

    // Return whether we succeeded or not
    return m_hModule != 0;
}

void CDynamicLibrary::Unload() noexcept
{
    // Got a module?
    if (!m_hModule)
        return;

    #ifdef _WIN32
    FreeLibrary(m_hModule);
    #else
    dlclose(m_hModule);
    #endif

    // Zero out our library as it's no longer valid
    m_hModule = nullptr;
}

bool CDynamicLibrary::IsLoaded() const noexcept
{
    return m_hModule != nullptr;
}

void* CDynamicLibrary::GetProcedureAddress(const char* szProcName) noexcept
{
    // Got a module?
    if (!m_hModule)
        return nullptr;

    #ifdef _WIN32
    return GetProcAddress(m_hModule, szProcName);
    #else
    char* szError;
    dlerror();

    void* pFunc = dlsym(m_hModule, szProcName);
    if ((szError = dlerror()) != nullptr)
        return nullptr;

    return pFunc;
    #endif
}
