/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        effekseer/Main.cpp
 *  PURPOSE:     Effekseer module entry point
 *
 *****************************************************************************/

#include "StdInc.h"

#include "CEffekseerManager.h"

MTAEXPORT CEffekseer* InitInterface(IDirect3DDevice9* pDevice)
{
    CEffekseerManager* pCurrentManager = new CEffekseerManager();

    pCurrentManager->Init(pDevice);

    return pCurrentManager;
}
