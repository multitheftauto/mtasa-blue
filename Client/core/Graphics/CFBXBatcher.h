/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CFBXBatcher.h
 *  PURPOSE:
 *
 *
 *****************************************************************************/


struct CUSTOMVERTEX
{
    float x, y, z;
    DWORD color;
};


//
// Batches fbx drawing
//
class CFBXBatcher
{
public:
    CFBXBatcher();
    ~CFBXBatcher();
    void OnDeviceCreate(IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY);
    void OnChangingRenderTarget(uint uiNewViewportSizeX, uint uiNewViewportSizeY);
    void UpdateMatrices(float fViewportSizeX, float fViewportSizeY);
    void SetDeviceStates();
    void Render();
    void DrawPrimitive(D3DPRIMITIVETYPE eType, size_t iSize, const void* pDataAddr, size_t iVertexStride);
    //void ClearQueue();
    //void AddPrimitive(D3DPRIMITIVETYPE eType, std::vector<PrimitiveVertice>* pVecVertices);

protected:
    IDirect3DDevice9*                m_pDevice;
    // fbx id, template id
    //std::vector<std::pair<int, int>> m_fbxList;
    float                            m_fViewportSizeX;
    float                            m_fViewportSizeY;
    D3DXMATRIX                       m_MatWorld;
    D3DXMATRIX                       m_MatView;
    D3DXMATRIX                       m_MatProjection;

    std::vector<CUSTOMVERTEX> vertices;
    std::vector<int> indices;
    LPDIRECT3DVERTEXBUFFER9 v_buffer = NULL;
    LPDIRECT3DINDEXBUFFER9  i_buffer = NULL;
    VOID*                   pVoid;
    
};
