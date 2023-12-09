/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CModelRenderer.h
 *  PURPOSE:     3D models renderer
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

enum class EModelLoadingScheme : uint8_t
{
    Loaded = 0,
    Async,
    Blocking
};

struct SModelToRender final
{
    EModelLoadingScheme scheme{EModelLoadingScheme::Loaded};    
    CMatrix matrix;

    // Invariants
private:
    friend class CModelRenderer;
    CModelInfo* pModelInfo{};
    bool bLoaded{};
};

class CModelRenderer final
{
public:
    SModelToRender& EnqueueModel(CModelInfo* pModelInfo);

    void Update();

    void Render();

private:
    std::vector<SModelToRender> m_Queue;
};
