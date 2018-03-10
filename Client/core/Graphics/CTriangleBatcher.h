/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        CTriangleBatcher.h
*  PURPOSE:
*  DEVELOPERS:  forkerer, based on CLine3DBatcher and CTileBatcher by vidiot
*
*
*****************************************************************************/

// Vertex type used by the triangle batcher
struct TriangleVertice
{
    float   x, y, z;
    SColor  color;
};


//
// Batches triangle drawing
// 
class CTriangleBatcher
{
public:
    ZERO_ON_NEW
                CTriangleBatcher        ( bool m_bZTest );
                ~CTriangleBatcher       ( void );

        void    OnDeviceCreate          ( IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY );
        void    OnChangingRenderTarget  ( uint uiNewViewportSizeX, uint uiNewViewportSizeY );
        void    UpdateMatrices          ( float fViewportSizeX, float fViewportSizeY );
        void    Flush                   ( void );
        void    ClearQueue              ( void );
        void    AddTriangle             ( CVector2D vecPos1,
                                          CVector2D vecPos2,
                                          CVector2D vecPos3,
                                          unsigned long ulColorVert1,
                                          unsigned long ulColorVert2,
                                          unsigned long ulColorVert3 );

protected:
    bool                            m_bZTest;
    IDirect3DDevice9*               m_pDevice;
    std::vector < TriangleVertice > m_triangleList;
    float                       m_fViewportSizeX;
    float                       m_fViewportSizeY;
    D3DXMATRIX                  m_MatView;
    D3DXMATRIX                  m_MatProjection;
};
