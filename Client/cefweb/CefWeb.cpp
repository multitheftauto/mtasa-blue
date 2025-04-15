/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CefWeb.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "SharedUtil.hpp"

CCoreInterface*         g_pCore = NULL;
CLocalizationInterface* g_pLocalization = nullptr;

extern "C" _declspec(dllexport) CWebCoreInterface* InitWebCoreInterface(CCoreInterface* pCore)
{
    g_pCore = pCore;
    g_pLocalization = pCore->GetLocalization();

    // Ensure main thread identification is consistent
    IsMainThread();

    CWebCore* pWebCore = new CWebCore;
    return pWebCore;
}
