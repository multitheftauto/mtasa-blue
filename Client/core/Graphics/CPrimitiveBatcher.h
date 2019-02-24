/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CPrimitiveBatcher.h
 *  PURPOSE:
 *
 *
 *****************************************************************************/
// Vertex type used by the primitives batcher
struct sPrimitive
{
    D3DPRIMITIVETYPE               eType;
    std::vector<PrimitiveVertice>* pVecVertices;
};

//
// Batches primitives drawing
//
class CPrimitiveBatcher
{
public:
    ZERO_ON_NEW
    CPrimitiveBatcher();
    ~CPrimitiveBatcher();
    void OnDeviceCreate(IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY);
    void OnChangingRenderTarget(uint uiNewViewportSizeX, uint uiNewViewportSizeY);
    void UpdateMatrices(float fViewportSizeX, float fViewportSizeY);
    void SetDeviceStates();
    void Flush();
    void DrawPrimitive(D3DPRIMITIVETYPE eType, size_t iSize, const void* pDataAddr, size_t iVertexStride);
    void ClearQueue();
    void AddPrimitive(D3DPRIMITIVETYPE eType, std::vector<PrimitiveVertice>* pVecVertices);

protected:
    IDirect3DDevice9*       m_pDevice;
    std::vector<sPrimitive> m_primitiveList;
    float                   m_fViewportSizeX;
    float                   m_fViewportSizeY;
    D3DXMATRIX              m_MatWorld;
    D3DXMATRIX              m_MatView;
    D3DXMATRIX              m_MatProjection;
};
