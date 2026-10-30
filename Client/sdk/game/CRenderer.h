/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CRenderer.h
 *  PURPOSE:     Renderer interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CModelInfo;
class CMatrix;
class CVector;

class CRenderer
{
public:
    virtual ~CRenderer() {}

    virtual void RenderModel(CModelInfo* pModelInfo, const CMatrix& matrix, float lighting) = 0;

    virtual void RequestObjectsInDirection(const CVector& position, float headingRadians) = 0;
};
