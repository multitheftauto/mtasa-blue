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

    // Create the DX texture
    if ( FAILED( D3DXCreateTextureFromFile ( m_pDevice, strFilename, &m_pD3DTexture ) ) )
        return;

    // Get some info
    D3DXIMAGE_INFO imageInfo;
    D3DSURFACE_DESC surfaceDesc;
    if ( FAILED ( D3DXGetImageInfoFromFile( strFilename, &imageInfo ) )
        || FAILED ( m_pD3DTexture->GetLevelDesc( 0, &surfaceDesc ) ) )
    {
        SAFE_RELEASE ( m_pD3DTexture );
        return;
    }

    m_uiSizeX = imageInfo.Width;
    m_uiSizeY = imageInfo.Height;
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
