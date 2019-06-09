/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CPrimitiveMaterialBatcher.h
 *  PURPOSE:
 *
 *
 *****************************************************************************/
// Vertex type used by the primitives batcher
struct sPrimitiveMaterial
{
    D3DPRIMITIVETYPE                        eType;
    CMaterialItem*                          pMaterial;
    std::vector<PrimitiveMaterialVertice>*  pVecVertices;
};
//
// Batches primitives drawing
//
class CPrimitiveMaterialBatcher
{
    friend CGraphics;

public:
    CPrimitiveMaterialBatcher(CGraphics* graphics);
    ~CPrimitiveMaterialBatcher();
    void OnDeviceCreate(IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY);
    void OnChangingRenderTarget(uint uiNewViewportSizeX, uint uiNewViewportSizeY);
    void UpdateMatrices(float fViewportSizeX, float fViewportSizeY);
    void SetDeviceStates();
    void Flush();
    void DrawPrimitive(D3DPRIMITIVETYPE eType, size_t iSize, const void* pDataAddr, size_t iVertexStride);
    void ClearQueue();
    void AddPrimitive(D3DPRIMITIVETYPE eType, CMaterialItem* pMaterial, std::vector<PrimitiveMaterialVertice>* pVecVertices);

protected:
    IDirect3DDevice9*               m_pDevice = nullptr;
    CGraphics*                      m_pGraphics = nullptr;
    std::vector<sPrimitiveMaterial> m_primitiveList;
    float                           m_fViewportSizeX = 0.0f;
    float                           m_fViewportSizeY = 0.0f;
    D3DXMATRIX                      m_MatWorld = {};
    D3DXMATRIX                      m_MatView = {};
    D3DXMATRIX                      m_MatProjection = {};
};
