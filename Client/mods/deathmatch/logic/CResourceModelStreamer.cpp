/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResourceModelStreamer.cpp
 *  PURPOSE:     Resource model manager
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#include "CResourceModelStreamer.h"
#include "CClientGame.h"
#include <game/CStreaming.h>

bool CResourceModelStreamer::RequestModel(std::uint16_t modelId, bool addRef, bool blocking)
{
    CModelInfo* model = g_pGame->GetModelInfo(modelId);

    if (!model)
        return false;

    if (addRef)
    {
        std::uint16_t refsCount = ++m_requestedModels[modelId];
        if (refsCount == 1)
        {
            model->ModelAddRef(blocking ? EModelRequestType::BLOCKING : EModelRequestType::NON_BLOCKING, "CResourceModelStreamer::RequestModel With reference");
            return true;
        }
        return false;
    }
    else
    {
        if (model->IsLoaded())
        {
            return false;
        }
        else
        {
            model->Request(blocking ? EModelRequestType::BLOCKING : EModelRequestType::NON_BLOCKING, "CResourceModelStreamer::RequestModel With out reference");
            return true;
        }
    }
}

// Return true if model was unloaded
bool CResourceModelStreamer::ReleaseModel(std::uint16_t modelId, bool removeRef)
{
    if (removeRef)
    {
        auto refs = m_requestedModels.find(modelId);
        if (refs == m_requestedModels.end())
            return false;

        std::uint16_t& refsCount = (*refs).second;

        if (refsCount == 0)
            return false;

        refsCount--;

        if (refsCount != 0)
            return false;

        CModelInfo* model = g_pGame->GetModelInfo(modelId);

        if (!model)
            return false;

        // Hack
        // This check will update models pending references
        model->IsLoaded();

        // This call can unload the model
        model->RemoveRef();

        return !model->IsLoaded();
    }
    else
    {
        CModelInfo* model = g_pGame->GetModelInfo(modelId);

        if (!model)
            return false;

        return model->UnloadUnused();
    }
}

void CResourceModelStreamer::ReleaseAll()
{
    for (const auto &modelRefs : m_requestedModels)
    {
        if (modelRefs.second > 0)
        {
            CModelInfo* model = g_pGame->GetModelInfo(modelRefs.first);
            model->RemoveRef();
        }
    }

    m_requestedModels.clear();
}

void CResourceModelStreamer::FullyReleaseModel(std::uint16_t modelId)
{
    auto refs = m_requestedModels.find(modelId);
    if (refs == m_requestedModels.end())
        return;

    std::uint16_t& refsCount = refs->second;

    if (refsCount > 0)
    {
        refsCount = 0;

        CModelInfo* model = g_pGame->GetModelInfo(modelId);

        if (!model)
            return;

        model->RemoveRef();
    }
}
