/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        CTileBatcher.h
*  PURPOSE:
*  DEVELOPERS:  vidiot
*
*
*****************************************************************************/

// Vertex type used by the tile batcher
struct SPDTVertex
{
    static const uint FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
    float x,y,z;
    DWORD color;
    float u,v;
};

#define WRITE_PDT_VERTEX(buf,fX,fY,fZ,dwColor,fU,fV) \
        buf->x = fX; \
        buf->y = fY; \
        buf->z = fZ; \
        buf->color = dwColor; \
        buf->u = fU; \
        buf->v = fV; \
        buf++;


//
// Batches draws using the same material
// 
class CTileBatcher
{
public:
    ZERO_ON_NEW
                CTileBatcher     ( void );
                ~CTileBatcher    ( void );

        void    OnDeviceCreate      ( IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY );
        void    Flush               ( void );
        void    AddTile             ( float fX, float fY,
                                      float fWidth, float fHeight,
                                      float fU, float fV,
                                      float fSizeU, float fSizeV, 
                                      SMaterialItem* pMaterial,
                                      float fRotation,
                                      float fRotCenOffX,
                                      float fRotCenOffY,
                                      unsigned long ulColor );

        void    SetCurrentMaterial ( SMaterialItem* pMaterial );

protected:
    IDirect3DDevice9*           m_pDevice;
    SMaterialItem*              m_pCurrentMaterial;
    float                       m_fCurrentRotation;
    float                       m_fCurrentRotCenX;
    float                       m_fCurrentRotCenY;
    std::vector < SPDTVertex >  m_Vertices;
    D3DXMATRIX                  m_MatView;
    D3DXMATRIX                  m_MatProjection;
};
