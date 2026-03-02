/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CModelRenderer.h
 *  PURPOSE:     3D models renderer
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CModelRenderer final
{
public:
    struct SModelToRender final
    {
        CModelInfo* pModelInfo;
        CMatrix     matrix;
        float       lighting;

        SModelToRender(CModelInfo* pModelInfo, const CMatrix& matrix, float lighting = 0.0f) : pModelInfo(pModelInfo), matrix(matrix), lighting(lighting) {}
    };

    bool EnqueueModel(CModelInfo* pModelInfo, const CMatrix& matrix, float lighting);

    void Update();

    void Render();

    // Must be called after GTA's RenderFadingInEntities completes to release
    // the queue memory that the alpha entity list's callbacks reference.
    void NotifyFrameEnd();

    static void RenderEntity(SModelToRender* entity, float distance);

private:
    std::vector<SModelToRender> m_Queue;
    bool                        m_bAlphaRefsActive = false;
};
