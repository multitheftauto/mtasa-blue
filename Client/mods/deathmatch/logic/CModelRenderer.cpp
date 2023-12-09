/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CModelRenderer.cpp
 *  PURPOSE:     3D models renderer
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "game\CRenderer.h"

bool CModelRenderer::EnqueueModel(CModelInfo* pModelInfo, const CMatrix& matrix, EModelLoadingScheme scheme)
{
    if (!pModelInfo || g_pCore->IsWindowMinimized())
        return false;

    m_Queue.emplace_back(pModelInfo, matrix, scheme);
    return true;
}

void CModelRenderer::Update()
{
    for (auto& modelDesc : m_Queue)
    {
        if (modelDesc.bLoaded = modelDesc.pModelInfo->IsLoaded(); !modelDesc.bLoaded)
        {
            if (modelDesc.scheme != EModelLoadingScheme::Loaded)
                modelDesc.pModelInfo->Request(modelDesc.scheme == EModelLoadingScheme::Blocking ?
                    EModelRequestType::BLOCKING : EModelRequestType::NON_BLOCKING, "Lua::DxDrawModel3D");

            if (modelDesc.scheme == EModelLoadingScheme::Blocking)
                modelDesc.bLoaded = modelDesc.pModelInfo->IsLoaded();
        }
    }
}

void CModelRenderer::Render()
{
    CRenderer* pRenderer = g_pGame->GetRenderer();
    assert(pRenderer);

    for (const auto& modelDesc : m_Queue)
    {
        if (modelDesc.bLoaded)
            pRenderer->RenderModel(modelDesc.pModelInfo, modelDesc.matrix);
    }

    m_Queue.clear();
}
