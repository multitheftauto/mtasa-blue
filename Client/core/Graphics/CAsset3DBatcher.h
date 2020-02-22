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

struct SRenderingSettings
{
    CMatrix                 matrix;
    CLuaAssetNodeInterface* assetNode;
};

//
// Batches primitives drawing
//
class CAsset3DBatcher
{
public:
    CAsset3DBatcher(CGraphics* pGraphics);
    void OnDeviceCreate(IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY);
    void Flush();
    void AddAsset(SRenderingSettings& settings);
    void DrawPrimitive(D3DPRIMITIVETYPE eType, size_t iCollectionSize, const void* pDataAddr, size_t uiVertexStride);
    bool HasItems() const { return !m_vecRenderList.empty(); }
    void ClearQueue();

protected:
    IDirect3DDevice9*                     m_pDevice;
    CGraphics*                            m_pGraphics;
    std::vector<SRenderingSettings>       m_vecRenderList;
};
