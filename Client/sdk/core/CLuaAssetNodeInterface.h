/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/core/CTrayIconInterface.h
 *  PURPOSE:     Asset node interface class
 *
 *****************************************************************************/

#pragma once

class CLuaAssetNodeInterface
{
public:
    virtual void Render(SRenderingSettings& settings) = 0;
};
