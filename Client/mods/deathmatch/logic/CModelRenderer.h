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

class CModelRenderer final
{
public:
    bool EnqueueModel(CModelInfo* pModelInfo, const CMatrix& matrix, EModelLoadingScheme scheme);

    void Update();

    void Render();

private:
    struct SModelToRender final
    {
        CModelInfo* pModelInfo;
        CMatrix matrix;
        EModelLoadingScheme scheme;
        bool bLoaded;

        SModelToRender(CModelInfo* pModelInfo, const CMatrix& matrix, EModelLoadingScheme scheme) :
            pModelInfo(pModelInfo),
            matrix(matrix),
            scheme(scheme),
            bLoaded(false)
        {
        }
    };

    std::vector<SModelToRender> m_Queue;
};
