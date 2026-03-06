/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CModelRenderer.cpp
 *  PURPOSE:     3D models renderer
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "game\CRenderer.h"
#include "game\CVisibilityPlugins.h"

bool CModelRenderer::EnqueueModel(CModelInfo* pModelInfo, const CMatrix& matrix, float lighting)
{
    if (g_pCore->IsWindowMinimized())
        return false;

    // Reject enqueues while the alpha entity list holds pointers into m_Queue.
    // An emplace_back could reallocate the vector, dangling those pointers.
    if (m_bAlphaRefsActive)
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
            const CVector& vecCameraPosition = *(CVector*)0xB76870;  // CRenderer::ms_vecCameraPosition
            const float    fDistance = (modelDesc.matrix.GetPosition() - vecCameraPosition).Length();

            pVisibilityPlugins->InsertEntityIntoEntityList(&modelDesc, fDistance, RenderEntity);
        }
    }

    m_bAlphaRefsActive = true;
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

    // m_Queue must NOT be cleared here: GTA's RenderFadingInEntities runs later
    // in the same frame and calls RenderEntity with pointers into m_Queue elements
    // that were inserted by Update() into the alpha entity list.
}

void CModelRenderer::NotifyFrameEnd()
{
    m_Queue.clear();
    m_bAlphaRefsActive = false;
}

void CModelRenderer::RenderEntity(SModelToRender* modelDesc, float distance)
{
    // GTA's alpha entity list stores raw pointers from InsertEntityIntoEntityList.
    // Verify modelDesc actually points into our queue before dereferencing it,
    // in case heap corruption overwrote the entity field in a list node.
    CModelRenderer* pModelRenderer = g_pClientGame->GetModelRenderer();
    if (!pModelRenderer || pModelRenderer->m_Queue.empty())
        return;

    const SModelToRender* pQueueData = pModelRenderer->m_Queue.data();
    if (modelDesc < pQueueData || modelDesc >= pQueueData + pModelRenderer->m_Queue.size())
        return;

    if (!modelDesc->pModelInfo->IsLoaded())
        return;

    CRenderer* pRenderer = g_pGame->GetRenderer();
    assert(pRenderer);

    pRenderer->RenderModel(modelDesc->pModelInfo, modelDesc->matrix, modelDesc->lighting);
}
