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
void CFileTextureItem::PostConstruct ( CRenderItemManager* pManager, const SString& strFilename )
{
    Super::PostConstruct ( pManager );

    // Initial creation of d3d data
    CreateUnderlyingData ( strFilename );
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
void CFileTextureItem::CreateUnderlyingData ( const SString& strFilename )
{
    assert ( !m_pD3DTexture );

    D3DXIMAGE_INFO imageInfo;
    if ( FAILED ( D3DXGetImageInfoFromFile( strFilename, &imageInfo ) ) )
        return;

    if ( imageInfo.ResourceType == D3DRTYPE_VOLUMETEXTURE )
    {
        // It's a volume texture!
        if ( FAILED( D3DXCreateVolumeTextureFromFile ( m_pDevice, strFilename, (IDirect3DVolumeTexture9**)&m_pD3DTexture ) ) )
            return;
    }
    else
    if ( imageInfo.ResourceType == D3DRTYPE_CUBETEXTURE )
    {
        // It's a cubemap texture!
        if ( FAILED( D3DXCreateCubeTextureFromFile ( m_pDevice, strFilename, (IDirect3DCubeTexture9**)&m_pD3DTexture ) ) )
            return;
    }
    else
    {
        // It's none of the above!
        if ( FAILED( D3DXCreateTextureFromFile ( m_pDevice, strFilename, (IDirect3DTexture9**)&m_pD3DTexture ) ) )
            return;
    }

    m_uiSizeX = imageInfo.Width;
    m_uiSizeY = imageInfo.Height;

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
