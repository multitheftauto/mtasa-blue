/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CDynamicLibrary.cpp
 *  PURPOSE:     Dynamic library handling class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CDynamicLibrary.h"
#include "MTAPlatform.h"

#ifdef WIN32
#include <stdio.h>
#endif

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

#ifdef WIN32

    // Copy the filename
    char szFilenameBuffer[MAX_PATH];
    strncpy(szFilenameBuffer, szFilename, MAX_PATH);

    // Replace forward slashes with backward slashes or the windows function will fail.
    size_t sizeFilename = strlen(szFilenameBuffer);
    for (size_t i = 0; i < sizeFilename; i++)
    {
        if (szFilenameBuffer[i] == '/')
            szFilenameBuffer[i] = '\\';
    }

    // Use this pointer now
    szFilename = szFilenameBuffer;

#endif

    // Load the new library
    #ifdef WIN32
    m_hModule = LoadLibraryEx(szFilename, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

    // Failed?
    if (!m_hModule)
    {
        // Grab the error message
        LPVOID lpMsgBuf;
        DWORD  dw = GetLastError();

        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw,
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

        // Display the error message and exit the process
        printf("Loading library %s failed; %s\n", szFilename, static_cast<const char*>(lpMsgBuf));

        // Free the error message buffer
        LocalFree(lpMsgBuf);
    }
    #else
    m_hModule = dlopen(szFilename, RTLD_NOW);

    // Output error if needed
    if (!m_hModule)
    {
        const char* szError = dlerror();
        if (szError)
            Print("%s\n", szError);
        else
            Print("Loading %s failed\n", szFilename);
    }
    #endif

    // Check for version mismatch
    if (!CheckMtaVersion(ExtractFilename(szFilename)))
    {
        return false;
    }

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

FuncPtr_t CDynamicLibrary::GetProcedureAddress(const char* szProcName)
{
    // Got a module?
    if (m_hModule != 0)
    {
        #ifdef WIN32
        return (FuncPtr_t)GetProcAddress(m_hModule, szProcName);
        #else
        char* szError = NULL;
        dlerror();

        FuncPtr_t pFunc = (FuncPtr_t)dlsym(m_hModule, szProcName);
        if ((szError = dlerror()) != NULL)
        {
            return NULL;
        }

        return pFunc;
        #endif
    }

    return NULL;
}

bool CDynamicLibrary::CheckMtaVersion(const char* szLibName)
{
#if MTASA_VERSION_TYPE >= VERSION_TYPE_UNSTABLE
// define MTASA_SKIP_VERSION_CHECKS in "Shared/build_overrides.h" to skip version checks
#ifndef MTASA_SKIP_VERSION_CHECKS

    if (m_hModule == 0)
        return false;

    char buffer[256];
    buffer[0] = 0;
    GetLibMtaVersion(buffer, sizeof(buffer));
    SString strVersionCore = buffer;

    buffer[0] = 0;
    FUNC_GetMtaVersion* pfnGetMtaVersion = (FUNC_GetMtaVersion*)(GetProcedureAddress("GetLibMtaVersion"));
    if (pfnGetMtaVersion)
        pfnGetMtaVersion(buffer, sizeof(buffer));
    SString strVersionLibrary = buffer;
    if (strVersionCore != strVersionLibrary)
    {
        Print("ERROR: '%s' library version is '%s' (Expected '%s')\n", szLibName, *strVersionLibrary, *strVersionCore);
        if (MTASA_VERSION_BUILD == 0)
        {
            int iBuildLibrary = atoi(strVersionLibrary.SubStr(8));
            Print("\n(If this is a custom build,\n");
            Print(" 1. Set MTASA_VERSION_BUILD in version.h to %d\n", iBuildLibrary);
            Print(" 2. Rebuild)\n\n\n");
        }
        else
            Print("\n** REINSTALL MTA **\n\n");
        return false;
    }

#endif
#endif
    return true;
}
