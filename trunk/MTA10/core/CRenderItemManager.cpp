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
    m_pDevice = NULL;
    m_pDefaultD3DRenderTarget = NULL;
    m_pDefaultD3DZStencilSurface = NULL;
    m_uiDefaultViewportSizeX = 0;
    m_uiDefaultViewportSizeY = 0;
    m_pBackBufferCopy = NULL;
    m_uiBackBufferCopyRevision = 0;
    m_bBackBufferCopyMaybeNeedsResize = false;
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
void CRenderItemManager::OnDeviceCreate ( IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY )
{
    m_pDevice = pDevice;
    m_uiDefaultViewportSizeX = fViewportSizeX;
    m_uiDefaultViewportSizeY = fViewportSizeY;
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
    pTextureItem->uiSizeX = imageInfo.Width;
    pTextureItem->uiSizeY = imageInfo.Height;

    // Update list of created items
    MapInsert ( m_CreatedItemList, pTextureItem );

    return pTextureItem;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CreateRenderTarget
//
//
//
////////////////////////////////////////////////////////////////
SRenderTargetItem* CRenderItemManager::CreateRenderTarget ( uint uiSizeX, uint uiSizeY, bool bWithAlphaChannel )
{
    IDirect3DTexture9* pD3DRenderTargetTexture = NULL;
    for ( uint i = 0 ; i < 4 ; i++ )
    {
        // 1st try -  i == 0  - 32 bit target
        // 2nd try -  i == 1  - 16 bit target
        //            i == 1  - EvictManagedResources
        // 3rd try -  i == 2  - 32 bit target
        // 4th try -  i == 3  - 16 bit target
	    D3DFORMAT Format = i & 1 ? D3DFMT_R5G6B5 : D3DFMT_X8R8G8B8;
        if ( bWithAlphaChannel )
            Format = D3DFMT_A8R8G8B8;
        if( SUCCEEDED( m_pDevice->CreateTexture( uiSizeX, uiSizeY, 1, D3DUSAGE_RENDERTARGET, Format, D3DPOOL_DEFAULT, &pD3DRenderTargetTexture, NULL ) ) )
            break;

        // c'mon
        if ( i == 1 )
            m_pDevice->EvictManagedResources ();
    }

    // Check texture created
    if ( !pD3DRenderTargetTexture )
        return NULL;


    IDirect3DSurface9* pD3DZStencilSurface = NULL;
    for ( uint i = 0 ; i < 4 ; i++ )
    {
        // 1st try -  i == 0  - 32 bit target
        // 2nd try -  i == 1  - 16 bit target
        //            i == 1  - EvictManagedResources
        // 3rd try -  i == 2  - 32 bit target
        // 4th try -  i == 3  - 16 bit target
	    D3DFORMAT Format = i & 1 ? D3DFMT_D16 : D3DFMT_D24X8;
        if( SUCCEEDED( m_pDevice->CreateDepthStencilSurface( uiSizeX, uiSizeY, Format, D3DMULTISAMPLE_NONE, 0, TRUE, &pD3DZStencilSurface, NULL ) ) )
            break;

        // c'mon c'mon
        if ( i == 1 )
            m_pDevice->EvictManagedResources ();
    }

    // Check depth buffer created created
    if ( !pD3DZStencilSurface )
    {
        pD3DRenderTargetTexture->Release ();
        return NULL;
    }

    // Get the render target surface here for convenience
    IDirect3DSurface9* pD3DRenderTargetSurface;
    pD3DRenderTargetTexture->GetSurfaceLevel ( 0, &pD3DRenderTargetSurface );

    // Clear incase it gets used before first copy
    m_pDevice->ColorFill( pD3DRenderTargetSurface, NULL, 0x00000000 );

    // Create the item
    SRenderTargetItem* pRenderTargetItem = new SRenderTargetItem ();
    pRenderTargetItem->pManager = this;
    pRenderTargetItem->iRefCount = 1;
    pRenderTargetItem->pD3DTexture = pD3DRenderTargetTexture;
    pRenderTargetItem->pD3DRenderTargetSurface = pD3DRenderTargetSurface;
    pRenderTargetItem->pD3DZStencilSurface = pD3DZStencilSurface;
    pRenderTargetItem->uiSizeX = uiSizeX;
    pRenderTargetItem->uiSizeY = uiSizeY;

    // Update list of created items
    MapInsert ( m_CreatedItemList, pRenderTargetItem );

    return pRenderTargetItem;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::CreateScreenSource
//
//
//
////////////////////////////////////////////////////////////////
SScreenSourceItem* CRenderItemManager::CreateScreenSource ( uint uiSizeX, uint uiSizeY )
{
    IDirect3DTexture9* pD3DRenderTargetTexture = NULL;
    for ( uint i = 0 ; i < 4 ; i++ )
    {
        // 1st try -  i == 0  - 32 bit target
        // 2nd try -  i == 1  - 16 bit target
        //            i == 1  - EvictManagedResources
        // 3rd try -  i == 2  - 32 bit target
        // 4th try -  i == 3  - 16 bit target
	    D3DFORMAT Format = i & 1 ? D3DFMT_R5G6B5 : D3DFMT_X8R8G8B8;
        if( SUCCEEDED( m_pDevice->CreateTexture( uiSizeX, uiSizeY, 1, D3DUSAGE_RENDERTARGET, Format, D3DPOOL_DEFAULT, &pD3DRenderTargetTexture, NULL ) ) )
            break;

        // c'mon
        if ( i == 1 )
            m_pDevice->EvictManagedResources ();
    }

    // Check texture created
    if ( !pD3DRenderTargetTexture )
        return NULL;

    // Get the render target surface here for convenience
    IDirect3DSurface9* pD3DRenderTargetSurface;
    pD3DRenderTargetTexture->GetSurfaceLevel ( 0, &pD3DRenderTargetSurface );

    // Clear incase it gets used before first copy
    m_pDevice->ColorFill( pD3DRenderTargetSurface, NULL, 0x00000000 );

    // Create the item
    SScreenSourceItem* pScreenSourceItem = new SScreenSourceItem ();
    pScreenSourceItem->pManager = this;
    pScreenSourceItem->iRefCount = 1;
    pScreenSourceItem->pD3DTexture = pD3DRenderTargetTexture;
    pScreenSourceItem->pD3DRenderTargetSurface = pD3DRenderTargetSurface;
    pScreenSourceItem->uiSizeX = uiSizeX;
    pScreenSourceItem->uiSizeY = uiSizeY;
    pScreenSourceItem->uiRevision = 0;

    // Update list of created items
    MapInsert ( m_CreatedItemList, pScreenSourceItem );

    m_bBackBufferCopyMaybeNeedsResize = true;

    return pScreenSourceItem;
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
    pShaderItem->uiSizeX = 256;
    pShaderItem->uiSizeY = 256;
    pShaderItem->pD3DEffect = pD3DEffect;
    pShaderItem->hFirstTexture = NULL;

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
        // Save handle to the first texture, as we'll be using it to work out the shader dimensions
        if ( Desc.Type == D3DXPT_TEXTURE && !pShaderItem->hFirstTexture )
            pShaderItem->hFirstTexture = hParameter;    
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
    // Ensure that any pending draws using this shader are done first
    if ( pShaderItem->bInQueue )
        CGraphics::GetSingleton ().DrawPreGUIQueue ();

    if ( D3DXHANDLE* phParameter = MapFind ( pShaderItem->parameterMap, strName.ToLower () ) )
    {
        if ( *phParameter == pShaderItem->hFirstTexture )
        {
            // Mirror size of first texture declared in effect file
            pShaderItem->uiSizeX = pTextureItem->uiSizeX;
            pShaderItem->uiSizeY = pTextureItem->uiSizeY;
        }
        if ( SUCCEEDED( pShaderItem->pD3DEffect->SetTexture( *phParameter, pTextureItem->pD3DTexture ) ) )
            return true;
    }
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
    // Ensure that any pending draws using this shader are done first
    if ( pShaderItem->bInQueue )
        CGraphics::GetSingleton ().DrawPreGUIQueue ();

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
    // Ensure that any pending draws using this shader are done first
    if ( pShaderItem->bInQueue )
        CGraphics::GetSingleton ().DrawPreGUIQueue ();

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

    // Release underlying data
    if ( SRenderTargetItem* pRenderTargetItem = DynamicCast < SRenderTargetItem > ( pItem ) )
        ReleaseRenderTargetData ( pRenderTargetItem );
    else
    if ( SScreenSourceItem* pScreenSourceItem = DynamicCast < SScreenSourceItem > ( pItem ) )
        ReleaseScreenSourceData ( pScreenSourceItem );
    else
    if ( SShaderItem* pShaderItem = DynamicCast < SShaderItem > ( pItem ) )
        ReleaseShaderData ( pShaderItem );
    else
    if ( SFontItem* pFontItem = DynamicCast < SFontItem > ( pItem ) )
        ReleaseFontData ( pFontItem );
    else
    if ( STextureItem* pTextureItem = DynamicCast < STextureItem > ( pItem ) )
        ReleaseTextureData ( pTextureItem );

    // Remove item from manager list
    assert ( MapContains ( m_CreatedItemList, pItem ) );
    MapRemove ( m_CreatedItemList, pItem );

    // Finally delete the render item
    delete pItem;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::ReleaseTextureData
//
// Release underlying texture data
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::ReleaseTextureData ( STextureItem* pTextureItem )
{
    // Release D3D resource
    SAFE_RELEASE ( pTextureItem->pD3DTexture );
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::ReleaseRenderTargetData
//
// Release underlying render target data
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::ReleaseRenderTargetData ( SRenderTargetItem* pRenderTargetItem )
{
    // If this is the current render target, restore default
    // Release D3D resources
    SAFE_RELEASE ( pRenderTargetItem->pD3DRenderTargetSurface );
    SAFE_RELEASE ( pRenderTargetItem->pD3DZStencilSurface );
    SAFE_RELEASE ( pRenderTargetItem->pD3DTexture );
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::ReleaseScreenSourceData
//
// Release underlying data
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::ReleaseScreenSourceData ( SScreenSourceItem* pScreenSourceItem )
{
    // Flag to re-assess back buffer copy settings
    m_bBackBufferCopyMaybeNeedsResize = true;

    // Release D3D resources
    SAFE_RELEASE ( pScreenSourceItem->pD3DRenderTargetSurface );
    SAFE_RELEASE ( pScreenSourceItem->pD3DTexture );
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::ReleaseShaderData
//
// Release underlying shader data
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::ReleaseShaderData ( SShaderItem* pShaderItem )
{
    // Release D3D resource
    SAFE_RELEASE ( pShaderItem->pD3DEffect );
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::ReleaseFontData
//
// Release underlying font data
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::ReleaseFontData ( SFontItem* pFontItem )
{
    // Release the CEGUI font data
    SAFE_DELETE( pFontItem->pFntCEGUI );

    // Release the DX font data
    CCore::GetSingleton ().GetGraphics()->DestroyAdditionalDXFont ( pFontItem->strFullFilePath, pFontItem->pFntBig, pFontItem->pFntNormal );
}


//
//
// Render targets and back buffers
//
//

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::UpdateBackBufferCopy
//
// Save back buffer pixels in our special place
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::UpdateBackBufferCopy ( void )
{
    if ( m_bBackBufferCopyMaybeNeedsResize )
        UpdateBackBufferCopySize ();

    // Don't bother doing this unless at least one screen stream in active
    if ( !m_pBackBufferCopy )
        return;

    // Try to get the back buffer
	IDirect3DSurface9* pD3DBackBufferSurface = NULL;
    m_pDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pD3DBackBufferSurface );
    if ( !pD3DBackBufferSurface )
        return;

    // Copy back buffer into our private render target
    D3DTEXTUREFILTERTYPE FilterType = D3DTEXF_LINEAR;
    HRESULT hr = m_pDevice->StretchRect( pD3DBackBufferSurface, NULL, m_pBackBufferCopy->pD3DRenderTargetSurface, NULL, FilterType );

    m_uiBackBufferCopyRevision++;

    // Clean up
	SAFE_RELEASE( pD3DBackBufferSurface );
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::UpdateScreenSource
//
// Copy from back buffer store to screen source
// TODO - Optimize the case where the screen source is the same size as the back buffer copy (i.e. Use back buffer copy resources instead)
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::UpdateScreenSource ( SScreenSourceItem* pScreenSourceItem )
{
    // Only do update if back buffer copy has changed
    if ( pScreenSourceItem->uiRevision == m_uiBackBufferCopyRevision )
        return;

    pScreenSourceItem->uiRevision = m_uiBackBufferCopyRevision;

    if ( m_pBackBufferCopy )
    {
        D3DTEXTUREFILTERTYPE FilterType = D3DTEXF_LINEAR;
        m_pDevice->StretchRect( m_pBackBufferCopy->pD3DRenderTargetSurface, NULL, pScreenSourceItem->pD3DRenderTargetSurface, NULL, FilterType );
    }
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::UpdateBackBufferCopySize
//
// Create/resize/destroy back buffer copy depending on what is required
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::UpdateBackBufferCopySize ( void )
{
    m_bBackBufferCopyMaybeNeedsResize = false;

    // Set what the max size requirement is for the back buffer copy
    uint uiSizeX = 0;
    uint uiSizeY = 0;
    for ( std::set < SRenderItem* >::iterator iter = m_CreatedItemList.begin () ; iter != m_CreatedItemList.end () ; iter++ )
    {
        if ( SScreenSourceItem* pScreenSourceItem = DynamicCast < SScreenSourceItem > ( *iter ) )
        {
            uiSizeX = Max ( uiSizeX, pScreenSourceItem->uiSizeX );
            uiSizeY = Max ( uiSizeY, pScreenSourceItem->uiSizeY );
        }
    }

    // Update?
    if ( !m_pBackBufferCopy || m_pBackBufferCopy->uiSizeX != uiSizeX || m_pBackBufferCopy->uiSizeY != uiSizeY )
    {
        // Delete old one if it exists
        if ( m_pBackBufferCopy )
        {
            ReleaseRenderItem ( m_pBackBufferCopy );
            m_pBackBufferCopy = NULL;
        }

        // Try to create new one if needed
        if ( uiSizeX > 0 )
            m_pBackBufferCopy = CreateRenderTarget ( uiSizeX, uiSizeY, false );
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::SetRenderTarget
//
// Set current render target to a custom one
//
////////////////////////////////////////////////////////////////
bool CRenderItemManager::SetRenderTarget ( SRenderTargetItem* pItem, bool bClear )
{
    if ( !CGraphics::GetSingleton().CanSetRenderTarget () )
        return false;

    // Save info on the default if not done yet
    if ( !m_pDefaultD3DRenderTarget )
    {
        m_pDevice->GetRenderTarget ( 0, &m_pDefaultD3DRenderTarget );
        m_pDevice->GetDepthStencilSurface ( &m_pDefaultD3DZStencilSurface );
    }

    ChangeRenderTarget ( pItem->uiSizeX, pItem->uiSizeY, pItem->pD3DRenderTargetSurface, pItem->pD3DZStencilSurface );

    if ( bClear )
        m_pDevice->Clear ( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0,0,0,0), 1, 0 );

    return true;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::RestoreDefaultRenderTarget
//
// Set render target back to the default one
//
////////////////////////////////////////////////////////////////
bool CRenderItemManager::RestoreDefaultRenderTarget ( void )
{
    if ( !CGraphics::GetSingleton().CanSetRenderTarget () )
        return false;

    // Only need to change if we have info
    if ( m_pDefaultD3DRenderTarget )
        ChangeRenderTarget ( m_uiDefaultViewportSizeX, m_uiDefaultViewportSizeY, m_pDefaultD3DRenderTarget, m_pDefaultD3DZStencilSurface );

    return true;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::ChangeRenderTarget
//
// Worker function to change the D3D render target
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::ChangeRenderTarget ( uint uiSizeX, uint uiSizeY, IDirect3DSurface9* pD3DRenderTarget, IDirect3DSurface9* pD3DZStencilSurface )
{
    // Check if we need to change
    IDirect3DSurface9* pCurrentRenderTarget;
    IDirect3DSurface9* pCurrentZStencilSurface;
    m_pDevice->GetRenderTarget ( 0, &pCurrentRenderTarget );
    m_pDevice->GetDepthStencilSurface ( &pCurrentZStencilSurface );

    bool bAlreadySet = ( pD3DRenderTarget == pCurrentRenderTarget && pD3DZStencilSurface == pCurrentZStencilSurface );

    SAFE_RELEASE ( pCurrentRenderTarget );
    SAFE_RELEASE ( pCurrentZStencilSurface );

    // Already set?
    if ( bAlreadySet )
        return;

    // Tell graphics things are about to change
    CGraphics::GetSingleton().OnChangingRenderTarget ( uiSizeX, uiSizeY );

    // Do change
    m_pDevice->SetRenderTarget ( 0, pD3DRenderTarget );
    m_pDevice->SetDepthStencilSurface ( pD3DZStencilSurface );

    D3DVIEWPORT9 viewport;
    viewport.X = 0;
    viewport.Y = 0;
    viewport.Width = uiSizeX;
    viewport.Height = uiSizeY;
    viewport.MinZ = 0.0f;
    viewport.MaxZ = 1.0f;
    m_pDevice->SetViewport ( &viewport );
}
