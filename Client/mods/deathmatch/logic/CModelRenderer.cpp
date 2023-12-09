#include "StdInc.h"
#include "game\CRenderer.h"

SModelToRender& CModelRenderer::EnqueueModel(CModelInfo* pModelInfo)
{
    static SModelToRender DEFAULT_MODEL_TO_RENDER{};

    if (!pModelInfo || g_pCore->IsWindowMinimized())
        return DEFAULT_MODEL_TO_RENDER;

    SModelToRender& entry = m_Queue.emplace_back();
    entry.pModelInfo = pModelInfo;
    return entry;
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
