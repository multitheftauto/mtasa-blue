/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        CefWeb.cpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "SharedUtil.hpp"

CCoreInterface* g_pCore = NULL;

extern "C" _declspec(dllexport)
CWebCoreInterface* InitWebCoreInterface(CCoreInterface* pCore)
{
    g_pCore = pCore;

    // Ensure main thread identification is consistent
    IsMainThread();

    CWebCore* pWebCore = new CWebCore;
    pWebCore->Initialise();
    return pWebCore;
}
