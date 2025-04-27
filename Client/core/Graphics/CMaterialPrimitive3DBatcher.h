/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CMaterialPrimitive3DBatcher.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

// Vertex type used by the primitives 3d batcher
struct sPrimitiveMaterial3D
{
    D3DPRIMITIVETYPE                       eType;
    CMaterialItem*                         pMaterial;
    std::vector<PrimitiveMaterialVertice>* pVecVertices;
};

//
// Batches primitives drawing
//
class CMaterialPrimitive3DBatcher
{
public:
    CMaterialPrimitive3DBatcher(bool bPreGUI, CGraphics* pGraphics);
    void OnDeviceCreate(IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY);
    void Flush();
    void AddPrimitive(D3DPRIMITIVETYPE eType, CMaterialItem* pMaterial, std::vector<PrimitiveMaterialVertice>* pVecVertices);
    void DrawPrimitive(D3DPRIMITIVETYPE eType, size_t iCollectionSize, const void* pDataAddr, size_t uiVertexStride);
    bool HasItems() const { return !m_primitiveList.empty(); }
    void ClearQueue();

protected:
    bool                              m_bPreGUI;
    IDirect3DDevice9*                 m_pDevice;
    CGraphics*                        m_pGraphics;
    std::vector<sPrimitiveMaterial3D> m_primitiveList;
};
