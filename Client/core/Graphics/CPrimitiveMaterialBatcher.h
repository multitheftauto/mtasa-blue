/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        CPrimitiveMaterialBatcher.h
*  PURPOSE:
*  DEVELOPERS:  forkerer, based on CLine3DBatcher and CTileBatcher by vidiot
*
*
*****************************************************************************/
// Vertex type used by the primitives batcher
struct sDrawQueuePrimitiveMaterial
{
	D3DPRIMITIVETYPE						type;
	CMaterialItem*							material;
	std::vector<PrimitiveMaterialVertice>   vertices;
};
//
// Batches primitives drawing
// 
class CPrimitiveMaterialBatcher
{
	friend CGraphics;
public:
	ZERO_ON_NEW
		CPrimitiveMaterialBatcher (bool m_bZTest, CGraphics* graphics);
	~CPrimitiveMaterialBatcher (void);
	void    OnDeviceCreate (IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY);
	void    OnChangingRenderTarget (uint uiNewViewportSizeX, uint uiNewViewportSizeY);
	void    UpdateMatrices (float fViewportSizeX, float fViewportSizeY);
	void    Flush (void);
	void	CPrimitiveMaterialBatcher::DrawPrimitive (D3DPRIMITIVETYPE eType, size_t iSize, const void* pDataAddr, size_t iVertexStride);
	void    ClearQueue (void);
	void    AddPrimitive (sDrawQueuePrimitiveMaterial primitive);
protected:
	bool                             m_bZTest;
	IDirect3DDevice9*                m_pDevice;
	CGraphics*						 m_pGraphics;
	std::vector < sDrawQueuePrimitiveMaterial > m_primitiveList;
	float                       m_fViewportSizeX;
	float                       m_fViewportSizeY;
	D3DXMATRIX                  m_MatView;
	D3DXMATRIX                  m_MatProjection;
};
