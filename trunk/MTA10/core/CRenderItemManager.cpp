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
void CRenderItemManager::OnDeviceCreate ( IDirect3DDevice9* pDevice )
{
    m_pDevice = pDevice;
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
    if ( FAILED( D3DXCreateTextureFromFile ( m_pDevice, strFullFilePath, &pD3DTexture ) ) )
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
// CRenderItemManager::CreateShader
//
// Create a D3DX effect from .fx file
//
////////////////////////////////////////////////////////////////
SShaderItem* CRenderItemManager::CreateShader ( const SString& strFullFilePath, SString& strOutStatus )
{
    // Compile effect
    DWORD dwFlags = 0;
    dwFlags |= D3DXSHADER_DEBUG;
    dwFlags |= D3DXSHADER_PARTIALPRECISION;

    ID3DXInclude* pIncludeManager = NULL;   // TODO
    ID3DXEffect* pD3DEffect = NULL;
    LPD3DXBUFFER pBufferErrors = NULL;
    HRESULT Result = D3DXCreateEffectFromFile( m_pDevice, strFullFilePath, NULL, pIncludeManager, dwFlags, NULL, &pD3DEffect, &pBufferErrors );            

    // Handle compile errors
    strOutStatus = "";
    if( pBufferErrors != NULL )
        strOutStatus = SStringX ( (CHAR*)pBufferErrors->GetBufferPointer() ).TrimEnd ( "\n" );
    SAFE_RELEASE( pBufferErrors );

    if( !pD3DEffect )
        return NULL;

    // Find first valid technique
    D3DXHANDLE hTechnique = NULL;
    pD3DEffect->FindNextValidTechnique( NULL, &hTechnique );

    // Error if can't find a valid technique
    if ( !hTechnique )
    {
        strOutStatus = "No valid technique";
        SAFE_RELEASE ( pD3DEffect );
        return NULL;
    }

    // Set technique
    pD3DEffect->SetTechnique( hTechnique );

    // Inform user of technique name
    D3DXTECHNIQUE_DESC Desc;
    pD3DEffect->GetTechniqueDesc( hTechnique, &Desc );
    strOutStatus = Desc.Name;

    // Create the item
    SShaderItem* pShaderItem = new SShaderItem ();
    pShaderItem->pManager = this;
    pShaderItem->iRefCount = 1;
    pShaderItem->pD3DEffect = pD3DEffect;

    // Add parameter handles
    D3DXEFFECT_DESC EffectDesc;
    pD3DEffect->GetDesc( &EffectDesc );
    for ( uint i = 0 ; i < EffectDesc.Parameters ; i++ )
    {
        D3DXHANDLE hParameter = pD3DEffect->GetParameter ( NULL, i );
        if ( !hParameter )
            break;
        D3DXPARAMETER_DESC Desc;
        pD3DEffect->GetParameterDesc( hParameter, &Desc );
        // Use semantic if it exists
        SString strName = Desc.Semantic ? Desc.Semantic : Desc.Name;
        // Add to lookup map
        MapSet ( pShaderItem->parameterMap, strName.ToLower (), hParameter );
    }

    // Cache known parameters
    D3DXHANDLE* phWorld = MapFind ( pShaderItem->parameterMap, "world" );
    pShaderItem->hWorld = phWorld ? *phWorld : NULL;

    D3DXHANDLE* phView = MapFind ( pShaderItem->parameterMap, "view" );
    pShaderItem->hView = phView ? *phView : NULL;

    D3DXHANDLE* phProjection = MapFind ( pShaderItem->parameterMap, "projection" );
    pShaderItem->hProjection = phProjection ? *phProjection : NULL;

    D3DXHANDLE* phAll = MapFind ( pShaderItem->parameterMap, "worldviewprojection" );
    pShaderItem->hAll = phAll ? *phAll : NULL;

    D3DXHANDLE* phTime = MapFind ( pShaderItem->parameterMap, "time" );
    pShaderItem->hTime = phTime ? *phTime : NULL;


    // Update list of created items
    MapInsert ( m_CreatedItemList, pShaderItem );

    return pShaderItem;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::SetShaderValue
//
// Set one texture
//
////////////////////////////////////////////////////////////////
bool CRenderItemManager::SetShaderValue ( SShaderItem* pShaderItem, const SString& strName, STextureItem* pTextureItem )
{
    if ( D3DXHANDLE* phParameter = MapFind ( pShaderItem->parameterMap, strName.ToLower () ) )
        if ( SUCCEEDED( pShaderItem->pD3DEffect->SetTexture( *phParameter, pTextureItem->pD3DTexture ) ) )
            return true;
    return false;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::SetShaderValue
//
// Set one bool
//
////////////////////////////////////////////////////////////////
bool CRenderItemManager::SetShaderValue ( SShaderItem* pShaderItem, const SString& strName, bool bValue )
{
    if ( D3DXHANDLE* phParameter = MapFind ( pShaderItem->parameterMap, strName.ToLower () ) )
        if ( SUCCEEDED( pShaderItem->pD3DEffect->SetBool( *phParameter, bValue ) ) )
            return true;
    return false;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::SetShaderValue
//
// Set up to 16 floats
//
////////////////////////////////////////////////////////////////
bool CRenderItemManager::SetShaderValue ( SShaderItem* pShaderItem, const SString& strName, const float* pfValues, uint uiCount )
{
    if ( D3DXHANDLE* phParameter = MapFind ( pShaderItem->parameterMap, strName.ToLower () ) )
        if ( SUCCEEDED( pShaderItem->pD3DEffect->SetFloatArray( *phParameter, pfValues, uiCount ) ) )
            return true;
    return false;
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
    if ( SShaderItem* pShaderItem = DynamicCast < SShaderItem > ( pItem ) )
        DestroyShader ( pShaderItem );
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
// CRenderItemManager::DestroyShader
//
//
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::DestroyShader ( SShaderItem* pShaderItem )
{
    // Release D3D resource
    SAFE_RELEASE ( pShaderItem->pD3DEffect );

    // Remove item from manager list
    assert ( MapContains ( m_CreatedItemList, pShaderItem ) );
    MapRemove ( m_CreatedItemList, pShaderItem );
    delete pShaderItem;
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
    // Destroy the CEGUI font data
    SAFE_DELETE( pFontItem->pFntCEGUI );

    // Destroy the DX font data
    CCore::GetSingleton ().GetGraphics()->DestroyAdditionalDXFont ( pFontItem->strFullFilePath, pFontItem->pFntBig, pFontItem->pFntNormal );

    // Remove item from manager list
    assert ( MapContains ( m_CreatedItemList, pFontItem ) );
    MapRemove ( m_CreatedItemList, pFontItem );
    delete pFontItem;
}
