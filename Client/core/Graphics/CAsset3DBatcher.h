/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CAsset3DBatcher.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

struct SRenderAssetItem
{
    CMatrix                 matrix;
    CLuaAssetNodeInterface* assetNode;
};

struct SRenderAssetItemGroup
{
    std::vector<CMatrix> vecMatrix;
    CLuaAssetNodeInterface* assetNode;
};

//
// Batches primitives drawing
//
class CAsset3DBatcher
{
public:
    CAsset3DBatcher(CGraphics* pGraphics);
    ~CAsset3DBatcher();
    void OnDeviceCreate(IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY);
    void Flush();
    void AddAsset(std::unique_ptr<SRenderAssetItem> assetRenderItem);
    bool HasItems() const { return !m_mapRenderList.empty(); }
    void ClearQueue();

protected:
    IDirect3DDevice9*                     m_pDevice = nullptr;
    CGraphics*                            m_pGraphics;
    std::unordered_map<CLuaAssetNodeInterface*, std::vector<CMatrix>> m_mapRenderList;
    CTextureItem*                                                     m_pMissingTexture = nullptr;
};
