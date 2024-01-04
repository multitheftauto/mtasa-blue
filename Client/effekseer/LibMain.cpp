
#include "StdInc.h"

#include "CEffekseerManager.h"

static CEffekseerManager* pCurrentManager = nullptr;

MTAEXPORT CEffekseerManagerInterface* InitInterface(IDirect3DDevice9* pDevice)
{
    if (pCurrentManager)
        return pCurrentManager;

    pCurrentManager = new CEffekseerManager();

    pCurrentManager->Init(pDevice);

    return pCurrentManager;
}
