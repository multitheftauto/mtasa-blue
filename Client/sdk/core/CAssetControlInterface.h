/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/core/CAssetControlInterface.h
 *  PURPOSE:     Asset control interface class
 *
 *****************************************************************************/

#pragma once

class CAssetsRenderGroup;

class CAssetControlInterface
{
public:
    virtual DWORD GetAmbientColor() = 0;
    virtual bool RenderGroupExists(unsigned int idx) const = 0;
    virtual CAssetsRenderGroup* GetRenderGroup(unsigned int idx) = 0;
};
