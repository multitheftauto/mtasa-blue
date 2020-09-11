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

class CClientMeshBuffer;
class CClientMultiMaterialMeshBuffer;
class CLuaAssetNodeInterface
{
public:
    virtual CClientMeshBuffer* GetMeshBuffer(int idx) = 0;
    virtual CClientMultiMaterialMeshBuffer* GetMeshBuffer() const = 0;
    virtual CMaterialItem*     GetTexture(int idx) = 0;
    virtual size_t             GetMeshNum() = 0;
};
