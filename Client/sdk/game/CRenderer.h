/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CRenderer.h
 *  PURPOSE:     Renderer interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CModelInfo;
class CMatrix;

class CRenderer
{
public:
    virtual ~CRenderer() {}

    virtual void RenderModel(CModelInfo* pModelInfo, const CMatrix& matrix, float lighting) = 0;
};
