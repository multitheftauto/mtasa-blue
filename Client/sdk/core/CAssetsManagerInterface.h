/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/core/CAssetsManagerInterface.h
 *  PURPOSE:     Asset control interface class
 *
 *****************************************************************************/

#pragma once

class CAssetInstance;

class CAssetsManagerInterface
{
public:
    virtual DWORD GetAmbientColor() = 0;
    virtual bool RenderGroupExists(unsigned int idx) const = 0;
    virtual CAssetInstance* GetRenderGroup(unsigned int idx) = 0;
};
