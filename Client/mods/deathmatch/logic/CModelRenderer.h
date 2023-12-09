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

    std::vector<SModelToRender> m_Queue;
};
