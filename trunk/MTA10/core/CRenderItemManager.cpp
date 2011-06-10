/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CRenderItemManager.cpp
*  PURPOSE:
*  DEVELOPERS:  idiot
*
*****************************************************************************/

#include "StdInc.h"
#include "utils/XFont.h"


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CRenderItemManager
//
//
//
////////////////////////////////////////////////////////////////
CRenderItemManager::CRenderItemManager ( void )
{
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::~CRenderItemManager
//
//
//
////////////////////////////////////////////////////////////////
CRenderItemManager::~CRenderItemManager ( void )
{
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::OnDeviceCreate
//
//
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::OnDeviceCreate ( IDirect3DDevice9* pDirect3DDevice9 )
{
    m_pD3DDevice = pDirect3DDevice9;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CreateTexture
//
// TODO: Make underlying data for textures shared
//
////////////////////////////////////////////////////////////////
STextureItem* CRenderItemManager::CreateTexture ( const SString& strFullFilePath )
{
    // Create the DX texture
    IDirect3DTexture9* pD3DTexture = NULL;
    if ( FAILED( D3DXCreateTextureFromFile ( m_pD3DDevice, strFullFilePath, &pD3DTexture ) ) )
        return NULL;

    // Get some info
    D3DXIMAGE_INFO imageInfo;
    D3DSURFACE_DESC surfaceDesc;
    if ( FAILED ( D3DXGetImageInfoFromFile( strFullFilePath, &imageInfo ) )
        || FAILED ( pD3DTexture->GetLevelDesc( 0, &surfaceDesc ) ) )
    {
        SAFE_RELEASE ( pD3DTexture );
        return NULL;
    }

    // Create the item
    STextureItem* pTextureItem = new STextureItem ();
    pTextureItem->pManager = this;
    pTextureItem->iRefCount = 1;
    pTextureItem->pD3DTexture = pD3DTexture;
    pTextureItem->uiFileWidth = imageInfo.Width;
    pTextureItem->uiFileHeight = imageInfo.Height;
    pTextureItem->uiSurfaceWidth = surfaceDesc.Width;
    pTextureItem->uiSurfaceHeight = surfaceDesc.Height;

    // Update list of created items
    MapInsert ( m_CreatedItemList, pTextureItem );

    return pTextureItem;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CreateFont
//
// TODO: Make underlying data for fonts shared
//
////////////////////////////////////////////////////////////////
SFontItem* CRenderItemManager::CreateFont ( const SString& strFullFilePath, const SString& strFontName, uint uiSize, bool bBold )
{
    uiSize = ( uiSize < 5 ) ? 5 : ( ( uiSize > 150 ) ? 150 : uiSize );

    // Find unused font name
    SString strUniqueName;
    int iCounter = 0;
    do
        strUniqueName = SString ( "%s*%d*%d_%d", *strFontName, uiSize, bBold, iCounter );
    while ( CCore::GetSingleton ().GetGUI ()->IsFontPresent ( strUniqueName ) );

    // Create the CEGUI font
    CGUIFont* fntCEGUI = CCore::GetSingleton ().GetGUI ()->CreateFnt ( strUniqueName, strFullFilePath, uiSize );
    if ( !fntCEGUI )
        return NULL;

    // Create ths DX fonts
    ID3DXFont *pFntNormal = NULL,*pFntBig = NULL;
    FONT_PROPERTIES sFontProps;
    if ( GetFontProperties ( LPCTSTR ( strFullFilePath.c_str () ), &sFontProps ) )
        CCore::GetSingleton ().GetGraphics()->LoadAdditionalDXFont ( strFullFilePath, sFontProps.csName, uiSize, bBold, &pFntNormal, &pFntBig );

    if ( !pFntNormal )
    {
        delete fntCEGUI;
        return NULL;
    }

    // Create the item
    SFontItem* pFontItem = new SFontItem ();
    pFontItem->pManager = this;
    pFontItem->iRefCount = 1;
    pFontItem->strCEGUIFontName = strUniqueName;
    pFontItem->pFntCEGUI = fntCEGUI;
    pFontItem->strFullFilePath = strFullFilePath;
    pFontItem->pFntNormal = pFntNormal;
    pFontItem->pFntBig = pFntBig;

    // Update list of created items
    MapInsert ( m_CreatedItemList, pFontItem );

    return pFontItem;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::ReleaseRenderItem
//
// Decrement reference count on a render item, and delete if necessary
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::ReleaseRenderItem ( SRenderItem* pItem )
{
    if ( --pItem->iRefCount > 0 )
        return;

    // Which type to delete?
    if ( STextureItem* pTextureItem = DynamicCast < STextureItem > ( pItem ) )
        DestroyTexture ( pTextureItem );
    else
    if ( SFontItem* pFontItem = DynamicCast < SFontItem > ( pItem ) )
        DestroyFont ( pFontItem );
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::DestroyTexture
//
//
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::DestroyTexture ( STextureItem* pTextureItem )
{
    // Release D3D resource
    SAFE_RELEASE ( pTextureItem->pD3DTexture );

    // Remove item from manager list
    assert ( MapContains ( m_CreatedItemList, pTextureItem ) );
    MapRemove ( m_CreatedItemList, pTextureItem );
    delete pTextureItem;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::DestroyFont
//
//
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::DestroyFont ( SFontItem* pFontItem )
{
    if ( --pFontItem->iRefCount > 0 )
        return;

    // Destroy the CEGUI font data
    SAFE_DELETE( pFontItem->pFntCEGUI );

    // Destroy the DX font data
    CCore::GetSingleton ().GetGraphics()->DestroyAdditionalDXFont ( pFontItem->strFullFilePath, pFontItem->pFntBig, pFontItem->pFntNormal );

    // Remove item from manager list
    assert ( MapContains ( m_CreatedItemList, pFontItem ) );
    MapRemove ( m_CreatedItemList, pFontItem );
    delete pFontItem;
}
