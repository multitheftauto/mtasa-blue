/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CPrimitive3DBatcher.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

// Vertex type used by the primitives 3d batcher
struct sPrimitive3D
{
    D3DPRIMITIVETYPE               eType;
    std::vector<PrimitiveVertice>* pVecVertices;
};

//
// Batches primitives drawing
//
class CPrimitive3DBatcher
{
public:
    CPrimitive3DBatcher(bool bPreGUI);
    void OnDeviceCreate(IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY);
    void Flush();
    void AddPrimitive(D3DPRIMITIVETYPE eType, std::vector<PrimitiveVertice>* pVecVertices);
    void DrawPrimitive(D3DPRIMITIVETYPE eType, size_t iCollectionSize, const void* pDataAddr, size_t uiVertexStride);
    bool HasItems() const { return !m_primitiveList.empty(); }

protected:
    bool                      m_bPreGUI;
    IDirect3DDevice9*         m_pDevice;
    std::vector<sPrimitive3D> m_primitiveList;
};
