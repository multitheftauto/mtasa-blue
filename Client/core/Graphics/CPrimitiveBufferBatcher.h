/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CPrimitiveBufferBatcher.h
 *  PURPOSE:
 *
 *
 *****************************************************************************/

class CClientPrimitiveBufferInterface;
    //
// Batches primitives drawing
//
class CPrimitiveBufferBatcher
{
public:
    CPrimitiveBufferBatcher(CGraphics* graphics);
    ~CPrimitiveBufferBatcher();
    void OnDeviceCreate(IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY);
    void OnChangingRenderTarget(uint uiNewViewportSizeX, uint uiNewViewportSizeY);
    void UpdateMatrices(float fViewportSizeX, float fViewportSizeY);
    void SetDeviceStates();
    void Flush(bool bPostGui);
    void ClearQueue();
    void AddPrimitiveBuffer(CClientPrimitiveBufferInterface* pPrimitiveBuffer, CMatrix matrix);

protected:
    CGraphics*                                                                 m_pGraphics;
    IDirect3DDevice9*                                                          m_pDevice;
    std::unordered_map<CClientPrimitiveBufferInterface*, std::vector<CMatrix>> m_primitiveBufferMap;
    float                                                                      m_fViewportSizeX;
    float                                                                      m_fViewportSizeY;
    D3DXMATRIX                                                                 m_MatWorld;
    D3DXMATRIX                                                                 m_MatView;
    D3DXMATRIX                                                                 m_MatProjection;
};
