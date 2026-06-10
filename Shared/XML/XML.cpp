/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/XML.cpp
 *  PURPOSE:     XML module entry
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CXMLImpl* pXMLInterface = NULL;

MTAEXPORT CXML* InitXMLInterface(const char* szSaveFlagDirectory)
{
    // Initialize and do any file recovery as necessary
    CXMLFileImpl::InitFileRecovery(szSaveFlagDirectory);

    // Whitespace is handled differently in tinyxml2 (PRESERVE_WHITESPACE is
    // the default, and COLLAPSE_WHITESPACE from the old SetCondenseWhiteSpace
    // call matches tinyxml2's PRESERVE_WHITESPACE more closely than it does
    // the old behavior, so the call is simply removed).

    pXMLInterface = new CXMLImpl;
    return pXMLInterface;
}

MTAEXPORT void ReleaseXMLInterface()
{
    if (pXMLInterface)
    {
        delete pXMLInterface;
        pXMLInterface = NULL;
    }
}

#ifdef WIN32

int WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, PVOID pvNothing)
{
    // Kill the interface on detach
    if (dwReason == DLL_PROCESS_DETACH)
    {
        ReleaseXMLInterface();
    }

    return TRUE;
}

#else

void __attribute__((destructor)) ReleaseXMLInterface(void);

#endif
