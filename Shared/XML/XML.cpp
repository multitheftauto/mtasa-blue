/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/XML.cpp
 *  PURPOSE:     XML module entry
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CXMLImpl* xmlInterface = nullptr;

MTAEXPORT CXML* InitXMLInterface(const char* saveFlagDirectory)
{
    xmlInterface = new CXMLImpl;
    return xmlInterface;
}

MTAEXPORT void ReleaseXMLInterface()
{
    if (xmlInterface)
    {
        delete xmlInterface;
        xmlInterface = nullptr;
    }
}

#ifdef WIN32

int WINAPI DllMain(HINSTANCE module, DWORD reason, PVOID nothing)
{
    // Kill the interface on detach
    if (reason == DLL_PROCESS_DETACH)
        ReleaseXMLInterface();

    return TRUE;
}

#else

void __attribute__((destructor)) ReleaseXMLInterface();

#endif
