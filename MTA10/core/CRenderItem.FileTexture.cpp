/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CRenderItem.FileTexture.cpp
*  PURPOSE:
*  DEVELOPERS:  xidiot
*
*****************************************************************************/

#include "StdInc.h"


////////////////////////////////////////////////////////////////
//
// CFileTextureItem::PostConstruct
//
//
//
////////////////////////////////////////////////////////////////
void CFileTextureItem::PostConstruct ( CRenderItemManager* pManager, const SString& strFilename, bool bMipMaps, uint uiSizeX, uint uiSizeY, ERenderFormat format )
{
    Super::PostConstruct ( pManager );

    // Initial creation of d3d data
    CreateUnderlyingData ( strFilename, bMipMaps, uiSizeX, uiSizeY, format );
}


////////////////////////////////////////////////////////////////
//
// CFileTextureItem::PreDestruct
//
//
//
////////////////////////////////////////////////////////////////
void CFileTextureItem::PreDestruct ( void )
{
    ReleaseUnderlyingData ();
    Super::PreDestruct ();
}


////////////////////////////////////////////////////////////////
//
// CFileTextureItem::IsValid
//
// Check underlying data is present
//
////////////////////////////////////////////////////////////////
bool CFileTextureItem::IsValid ( void )
{
    return m_pD3DTexture != NULL;
}


////////////////////////////////////////////////////////////////
//
// CFileTextureItem::OnLostDevice
//
// Release device stuff
//
////////////////////////////////////////////////////////////////
void CFileTextureItem::OnLostDevice ( void )
{
    // Nothing required for CFileTextureItem
}


////////////////////////////////////////////////////////////////
//
// CFileTextureItem::OnResetDevice
//
// Recreate device stuff
//
////////////////////////////////////////////////////////////////
void CFileTextureItem::OnResetDevice ( void )
{
    // Nothing required for CFileTextureItem
}


////////////////////////////////////////////////////////////////
//
// CFileTextureItem::CreateUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CFileTextureItem::CreateUnderlyingData ( const SString& strFilename, bool bMipMaps, uint uiSizeX, uint uiSizeY, ERenderFormat format )
{
    assert ( !m_pD3DTexture );

    D3DXIMAGE_INFO imageInfo;
    if ( FAILED ( D3DXGetImageInfoFromFile( strFilename, &imageInfo ) ) )
        return;

    D3DFORMAT D3DFormat = (D3DFORMAT)format;
    int iMipMaps = bMipMaps ? D3DX_DEFAULT : 1;
    if ( uiSizeX != D3DX_DEFAULT )
        imageInfo.Width = uiSizeX;
    if ( uiSizeY != D3DX_DEFAULT )
        imageInfo.Height = uiSizeY;

    m_uiSizeX = imageInfo.Width;
    m_uiSizeY = imageInfo.Height;
    m_uiSurfaceSizeX = imageInfo.Width;
    m_uiSurfaceSizeY = imageInfo.Height;

    if ( imageInfo.ResourceType == D3DRTYPE_VOLUMETEXTURE )
    {
        // It's a volume texture!
        if ( FAILED( D3DXCreateVolumeTextureFromFileEx ( m_pDevice, strFilename, uiSizeX, uiSizeY, D3DX_DEFAULT, iMipMaps, 0, D3DFormat, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, (IDirect3DVolumeTexture9**)&m_pD3DTexture ) ) )
            return;
    }
    else
    if ( imageInfo.ResourceType == D3DRTYPE_CUBETEXTURE )
    {
        // It's a cubemap texture!
        if ( FAILED( D3DXCreateCubeTextureFromFileEx ( m_pDevice, strFilename, uiSizeX, iMipMaps, 0, D3DFormat, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, (IDirect3DCubeTexture9**)&m_pD3DTexture ) ) )
            return;
    }
    else
    {
        // It's none of the above!
        if ( FAILED( D3DXCreateTextureFromFileEx ( m_pDevice, strFilename, uiSizeX, uiSizeY, iMipMaps, 0, D3DFormat, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, (IDirect3DTexture9**)&m_pD3DTexture ) ) )
            return;

        // Update surface size if it's a normal texture
        D3DSURFACE_DESC desc;
        ((IDirect3DTexture9*)m_pD3DTexture)->GetLevelDesc ( 0, &desc );
        m_uiSurfaceSizeX = desc.Width;
        m_uiSurfaceSizeY = desc.Height;
    }

    // Calc memory usage
    m_iMemoryKBUsed = CRenderItemManager::CalcD3DResourceMemoryKBUsage ( m_pD3DTexture );
}


////////////////////////////////////////////////////////////////
//
// CFileTextureItem::ReleaseUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CFileTextureItem::ReleaseUnderlyingData ( void )
{
    SAFE_RELEASE( m_pD3DTexture );
}
