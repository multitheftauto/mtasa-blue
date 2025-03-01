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

bool CModelRenderer::EnqueueModel(CModelInfo* pModelInfo, const CMatrix& matrix, float lighting)
{
    if (g_pCore->IsWindowMinimized())
        return false;

    if (pModelInfo && pModelInfo->IsLoaded())
    {
        m_Queue.emplace_back(pModelInfo, matrix, lighting);
        return true;
    }

    return false;
}

void CModelRenderer::Update()
{
    CVisibilityPlugins* pVisibilityPlugins = g_pGame->GetVisibilityPlugins();
    assert(pVisibilityPlugins);

    for (auto& modelDesc : m_Queue)
    {
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

    // Draw opaque entities
    for (auto& modelDesc : m_Queue)
    {
        if (modelDesc.pModelInfo->IsLoaded() && !modelDesc.pModelInfo->GetIdeFlag(eModelIdeFlag::DRAW_LAST))
            pRenderer->RenderModel(modelDesc.pModelInfo, modelDesc.matrix, modelDesc.lighting);
    }

    m_Queue.clear();
}

void CModelRenderer::RenderEntity(SModelToRender* modelDesc, float distance)
{
    if (!modelDesc->pModelInfo->IsLoaded())
        return;

    CRenderer* pRenderer = g_pGame->GetRenderer();
    assert(pRenderer);

    pRenderer->RenderModel(modelDesc->pModelInfo, modelDesc->matrix, modelDesc->lighting);
}
