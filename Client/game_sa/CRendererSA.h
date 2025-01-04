/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CRendererSA.h
 *  PURPOSE:     Game renderer class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CRenderer.h>

class CRendererSA : public CRenderer
{
public:
    CRendererSA();
    ~CRendererSA();

    void RenderModel(CModelInfo* pModelInfo, const CMatrix& matrix, float lighting) override;
};
