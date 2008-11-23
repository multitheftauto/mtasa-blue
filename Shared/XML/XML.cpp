/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        xml/XML.cpp
*  PURPOSE:     XML module entry
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CXMLImpl* pXMLInterface = NULL;

extern "C" _declspec(dllexport)
CXML* InitXMLInterface()
{
    pXMLInterface = new CXMLImpl;
	return pXMLInterface;
}

extern "C" _declspec(dllexport)
void ReleaseXMLInterface()
{
	if ( pXMLInterface )
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
		ReleaseXMLInterface ();
	}

	return TRUE;
}

#else

void __attribute__ ((destructor)) ReleaseXMLInterface(void);

#endif

