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
#include "game\CVisibilityPlugins.h"

bool CModelRenderer::EnqueueModel(CModelInfo* pModelInfo, const CMatrix& matrix, EModelLoadingScheme scheme)
{
    if (!pModelInfo || g_pCore->IsWindowMinimized())
        return false;

    m_Queue.emplace_back(pModelInfo, matrix, scheme);
    return true;
}

void CModelRenderer::Update()
{
    CVisibilityPlugins* pVisibilityPlugins = g_pGame->GetVisibilityPlugins();
    assert(pVisibilityPlugins);

    for (auto& modelDesc : m_Queue)
    {
        // Make sure that a model is loaded
        modelDesc.bLoaded = modelDesc.pModelInfo->IsLoaded();
        if (!modelDesc.bLoaded)
        {     
            if (modelDesc.scheme != EModelLoadingScheme::Loaded)
                modelDesc.pModelInfo->Request(modelDesc.scheme == EModelLoadingScheme::Blocking ?
                    EModelRequestType::BLOCKING : EModelRequestType::NON_BLOCKING, "Lua::DxDrawModel3D");

            if (modelDesc.scheme == EModelLoadingScheme::Blocking)
                modelDesc.bLoaded = modelDesc.pModelInfo->IsLoaded();
        }

        // Insert transparent entities into a sorted list
        if (modelDesc.pModelInfo->GetIdeFlag(eModelIdeFlag::DRAW_LAST))
        {
            const CVector& vecCameraPosition = *(CVector*)0xB76870; // CRenderer::ms_vecCameraPosition
            const float fDistance = (modelDesc.matrix.GetPosition() - vecCameraPosition).Length();

            pVisibilityPlugins->InsertEntityIntoEntityList(&modelDesc, fDistance, RenderEntity);
        }
    }
}

void CModelRenderer::Render()
{
    CRenderer* pRenderer = g_pGame->GetRenderer();
    assert(pRenderer);    

    for (auto& modelDesc : m_Queue)
    {
        if (modelDesc.bLoaded && !modelDesc.pModelInfo->GetIdeFlag(eModelIdeFlag::DRAW_LAST))
            pRenderer->RenderModel(modelDesc.pModelInfo, modelDesc.matrix);
    }

    m_Queue.clear();
}

void CModelRenderer::RenderEntity(SModelToRender* modelDesc, float distance)
{
    if (!modelDesc->bLoaded)
        return;

    CRenderer* pRenderer = g_pGame->GetRenderer();
    assert(pRenderer);

    pRenderer->RenderModel(modelDesc->pModelInfo, modelDesc->matrix);
}
