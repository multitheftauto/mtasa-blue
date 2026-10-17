/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CRendererSA.h
 *  PURPOSE:     Game renderer class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CRenderer.h>

#define FUNC_CRenderer_RequestObjectsInDirection 0x555CB0

class CRendererSA : public CRenderer
{
public:
    CRendererSA();
    ~CRendererSA();

    void RenderModel(CModelInfo* pModelInfo, const CMatrix& matrix, float lighting) override;
    void RequestObjectsInDirection(const CVector& position, float headingRadians) override;
};
