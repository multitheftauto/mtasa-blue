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

class CModelRenderer final
{
public:
    struct SModelToRender final
    {
        CModelInfo* pModelInfo;
        CMatrix matrix;

        SModelToRender(CModelInfo* pModelInfo, const CMatrix& matrix) :
            pModelInfo(pModelInfo),
            matrix(matrix)
        {
        }
    };

    bool EnqueueModel(CModelInfo* pModelInfo, const CMatrix& matrix);

    void Update();

    void Render();

    static void RenderEntity(SModelToRender* entity, float distance);

private:  

    std::vector<SModelToRender> m_Queue;
};
