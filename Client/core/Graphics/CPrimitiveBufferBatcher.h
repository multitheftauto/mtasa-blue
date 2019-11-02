/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CPrimitiveBufferBatcher.h
 *  PURPOSE:
 *
 *
 *****************************************************************************/

enum ePrimitiveView
{
    PRIMITIVE_VIEW_PERSPECTIVE,
    PRIMITIVE_VIEW_ORTHOGRAPHIC,
};

class CClientPrimitiveBufferInterface;

class CPrimitiveBufferBatcher
{
    friend CGraphics;

public:
    CPrimitiveBufferBatcher(CGraphics* graphics);
    ~CPrimitiveBufferBatcher();
    void OnDeviceCreate(IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY);
    void OnChangingRenderTarget(uint uiNewViewportSizeX, uint uiNewViewportSizeY);
    void UpdateMatrices(float fViewportSizeX, float fViewportSizeY);
    void SetDeviceStates();
    void SetDeviceMaterialStates();
    void Flush();
    void ClearQueue();
    void AddPrimitiveBuffer(CClientPrimitiveBufferInterface* pPrimitiveBuffer, PrimitiveBufferSettings& bufferSettings);

protected:
    void FlushMaterialPrimitives();
    void FlushPrimitives();

    CGraphics*                                                                                 m_pGraphics;
    IDirect3DDevice9*                                                                          m_pDevice;
    std::unordered_map<CClientPrimitiveBufferInterface*, std::vector<PrimitiveBufferSettings>> m_primitiveBufferMap;
    std::unordered_map<CClientPrimitiveBufferInterface*, std::vector<PrimitiveBufferSettings>> m_materialPrimitiveBufferMap;
    float                                                                                      m_fViewportSizeX;
    float                                                                                      m_fViewportSizeY;
    D3DXMATRIX                                                                                 m_MatWorld;
    D3DXMATRIX                                                                                 m_MatView;
    D3DXMATRIX                                                                                 m_MatProjection;
    D3DXMATRIX                                                                                 m_MatProjectionOrtho;
    ETextureAddress                                                                            m_CurrentTextureAddress;
};
