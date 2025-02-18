/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CDynamicLibrary.cpp
 *  PURPOSE:     Dynamic libraries loader
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CDynamicLibrary.h"

CDynamicLibrary::CDynamicLibrary()
{
    // Init
    m_hModule = 0;
}

CDynamicLibrary::~CDynamicLibrary()
{
    // Make sure we unload
    Unload();
}

bool CDynamicLibrary::Load(const char* szFilename)
{
    // Unload the previous library
    Unload();

    // Load the new library
    #ifdef WIN32
    m_hModule = LoadLibrary(szFilename);
    #else
    m_hModule = dlopen(szFilename, RTLD_NOW);

    // Output error if needed
    if (!m_hModule)
    {
        Print("%s\n", dlerror());
    }
    #endif

    // Return whether we succeeded or not
    return m_hModule != 0;
}

void CDynamicLibrary::Unload()
{
    // Got a module?
    if (m_hModule != 0)
    {
        #ifdef WIN32
        FreeLibrary(m_hModule);
        #else
        dlclose(m_hModule);
        #endif

        // Zero out our library as it's no longer valid
        m_hModule = 0;
    }
}

bool CDynamicLibrary::IsLoaded()
{
    return m_hModule != 0;
}

void* CDynamicLibrary::GetProcedureAddress(const char* szProcName)
{
    // Got a module?
    if (m_hModule != 0)
    {
        #ifdef WIN32
        return GetProcAddress(m_hModule, szProcName);
        #else
        char* szError = NULL;
        dlerror();

        void* pFunc = dlsym(m_hModule, szProcName);
        if ((szError = dlerror()) != NULL)
        {
            return NULL;
        }

        return pFunc;
        #endif
    }

    return NULL;
}
