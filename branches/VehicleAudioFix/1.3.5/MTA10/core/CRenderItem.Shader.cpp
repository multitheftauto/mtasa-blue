/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CRenderItem.Shader.cpp
*  PURPOSE:
*  DEVELOPERS:  xidiot
*
*****************************************************************************/

#include "StdInc.h"

uint CShaderItem::ms_uiCreateTimeCounter = 0;

////////////////////////////////////////////////////////////////
//
// CShaderItem::PostConstruct
//
//
//
////////////////////////////////////////////////////////////////
void CShaderItem::PostConstruct ( CRenderItemManager* pManager, const SString& strFilename, const SString& strRootPath, SString& strOutStatus, float fPriority, float fMaxDistance, bool bLayered, bool bDebug, int iTypeMask )
{
    m_fPriority = fPriority;
    m_uiCreateTime = ms_uiCreateTimeCounter++;      // Priority tie breaker
    m_fMaxDistanceSq = fMaxDistance * fMaxDistance;
    m_bLayered = bLayered;
    m_iTypeMask = iTypeMask;

    Super::PostConstruct ( pManager );

    // Initial creation of d3d data
    CreateUnderlyingData ( strFilename, strRootPath, strOutStatus, bDebug );
}


////////////////////////////////////////////////////////////////
//
// CShaderItem::PreDestruct
//
//
//
////////////////////////////////////////////////////////////////
void CShaderItem::PreDestruct ( void )
{
    ReleaseUnderlyingData ();
    Super::PreDestruct ();
}


////////////////////////////////////////////////////////////////
//
// CShaderItem::IsValid
//
// Check underlying data is present
//
////////////////////////////////////////////////////////////////
bool CShaderItem::IsValid ( void )
{
    return m_pEffectWrap && m_pEffectWrap->m_pD3DEffect;
}


////////////////////////////////////////////////////////////////
//
// CShaderItem::OnLostDevice
//
// Release device stuff
//
////////////////////////////////////////////////////////////////
void CShaderItem::OnLostDevice ( void )
{
    // Nothing required for CShaderItem
}


////////////////////////////////////////////////////////////////
//
// CShaderItem::OnResetDevice
//
// Recreate device stuff
//
////////////////////////////////////////////////////////////////
void CShaderItem::OnResetDevice ( void )
{
    // Nothing required for CShaderItem
}


////////////////////////////////////////////////////////////////
//
// CShaderItem::CreateUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CShaderItem::CreateUnderlyingData ( const SString& strFilename, const SString& strRootPath, SString& strOutStatus, bool bDebug )
{
    assert ( !m_pEffectWrap );
    assert ( !m_pShaderInstance );


    m_pEffectWrap = NewEffectWrap ( m_pManager, strFilename, strRootPath, strOutStatus, bDebug );

    if ( !m_pEffectWrap->IsValid () )
    {
        SAFE_RELEASE( m_pEffectWrap );
        return;
    }

    m_pEffectWrap->ReadParameterHandles ();

    struct
    {
        D3DXHANDLE& hHandle;
        SString strNames;
    } handleNames [] = {
        m_pEffectWrap->hWorld, "WORLD",
        m_pEffectWrap->hView, "VIEW",
        m_pEffectWrap->hProjection, "PROJECTION",
        m_pEffectWrap->hWorldView, "WORLDVIEW",
        m_pEffectWrap->hWorldViewProj, "WORLDVIEWPROJECTION",
        m_pEffectWrap->hViewProj, "VIEWPROJECTION",
        m_pEffectWrap->hViewInv, "VIEWINVERSE",
        m_pEffectWrap->hWorldInvTr, "WORLDINVERSETRANSPOSE",
        m_pEffectWrap->hViewInvTr, "VIEWINVERSETRANSPOSE",
        m_pEffectWrap->hCamPos, "CAMERAPOSITION",
        m_pEffectWrap->hCamDir, "CAMERADIRECTION",
        m_pEffectWrap->hTime, "TIME",
        m_pEffectWrap->hLightAmbient, "LIGHTAMBIENT",
        m_pEffectWrap->hLightDiffuse, "LIGHTDIFFUSE",
        m_pEffectWrap->hLightSpecular, "LIGHTSPECULAR",
        m_pEffectWrap->hLightDirection, "LIGHTDIRECTION",
        m_pEffectWrap->hDepthBuffer, "DEPTHBUFFER",
        m_pEffectWrap->hViewMainScene, "VIEW_MAIN_SCENE",
        m_pEffectWrap->hWorldMainScene, "WORLD_MAIN_SCENE",
        m_pEffectWrap->hProjectionMainScene, "PROJECTION_MAIN_SCENE",
    };

    for ( uint h = 0 ; h < NUMELMS( handleNames ) ; h++ )
    {
        std::vector < SString > parts;
        handleNames[h].strNames.Split( ",", parts );
        D3DXHANDLE* phHandle = NULL;
        for ( uint n = 0 ; n < parts.size () && phHandle == NULL ; n++ )
        {
            phHandle = MapFind ( m_pEffectWrap->m_valueHandleMap, parts[n] );
            if ( !phHandle )
                phHandle = MapFind ( m_pEffectWrap->m_texureHandleMap, parts[n] );
        }
        handleNames[h].hHandle = phHandle ? *phHandle : NULL;
    }

    m_pManager->NotifyShaderItemUsesDepthBuffer ( this, m_pEffectWrap->m_bUsesDepthBuffer );
       
    // Create instance to store param values
    RenewShaderInstance ();
}


////////////////////////////////////////////////////////////////
//
// CShaderItem::ReleaseUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CShaderItem::ReleaseUnderlyingData ( void )
{
    m_pManager->NotifyShaderItemUsesDepthBuffer ( this, false );
    SAFE_RELEASE( m_pEffectWrap )
    SAFE_RELEASE( m_pShaderInstance );
}


////////////////////////////////////////////////////////////////
//
// CShaderItem::SetValue
//
// Set one texture
//
////////////////////////////////////////////////////////////////
bool CShaderItem::SetValue ( const SString& strName, CTextureItem* pTextureItem )
{
    if ( D3DXHANDLE* phParameter = MapFind ( m_pEffectWrap->m_texureHandleMap, strName.ToUpper () ) )
    {
        // Check if value is changing
        if ( !m_pShaderInstance->CmpTextureValue( *phParameter, pTextureItem ) )
        {
            // Check if we need a new shader instance
            MaybeRenewShaderInstance ();
    
            if ( *phParameter == m_pEffectWrap->m_hFirstTexture )
            {
                // Mirror size of first texture declared in effect file
                m_uiSizeX = pTextureItem->m_uiSizeX;
                m_uiSizeY = pTextureItem->m_uiSizeY;
                m_pShaderInstance->m_uiSizeX = m_uiSizeX;
                m_pShaderInstance->m_uiSizeY = m_uiSizeY;
            }
    
            m_pShaderInstance->SetTextureValue( *phParameter, pTextureItem );
        }
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////
//
// CShaderItem::SetValue
//
// Set one bool
//
////////////////////////////////////////////////////////////////
bool CShaderItem::SetValue ( const SString& strName, bool bValue )
{
    if ( D3DXHANDLE* phParameter = MapFind ( m_pEffectWrap->m_valueHandleMap, strName.ToUpper () ) )
    {
        // Check if value is changing
        if ( !m_pShaderInstance->CmpBoolValue( *phParameter, bValue ) )
        {
            // Check if we need a new shader instance
            MaybeRenewShaderInstance ();
            m_pShaderInstance->SetBoolValue( *phParameter, bValue );
        }
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////
//
// CShaderItem::SetValue
//
// Set up to 16 floats
//
////////////////////////////////////////////////////////////////
bool CShaderItem::SetValue ( const SString& strName, const float* pfValues, uint uiCount )
{
    if ( D3DXHANDLE* phParameter = MapFind ( m_pEffectWrap->m_valueHandleMap, strName.ToUpper () ) )
    {
        // Check if value is changing
        if ( !m_pShaderInstance->CmpFloatsValue( *phParameter, pfValues, uiCount ) )
        {
            // Check if we need a new shader instance
            MaybeRenewShaderInstance ();
            m_pShaderInstance->SetFloatsValue( *phParameter, pfValues, uiCount );
        }
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////
//
// CShaderItem::SetTessellation
//
//
//
////////////////////////////////////////////////////////////////
void CShaderItem::SetTessellation ( uint uiTessellationX, uint uiTessellationY )
{
    // Check if value is changing
    if ( uiTessellationX != m_pShaderInstance->m_uiTessellationX || uiTessellationY != m_pShaderInstance->m_uiTessellationY )
    {
        // Check if we need a new shader instance
        MaybeRenewShaderInstance ();
        m_pShaderInstance->m_uiTessellationX = uiTessellationX;
        m_pShaderInstance->m_uiTessellationY = uiTessellationY;
    }
}


////////////////////////////////////////////////////////////////
//
// CShaderItem::SetTransform
//
//
//
////////////////////////////////////////////////////////////////
void CShaderItem::SetTransform ( const SShaderTransform& transform )
{
    // Check if value is changing
    if ( memcmp ( &m_pShaderInstance->m_Transform, &transform, sizeof ( transform ) ) != 0 )
    {
        // Check if we need a new shader instance
        MaybeRenewShaderInstance ();
        m_pShaderInstance->m_Transform = transform;
        m_pShaderInstance->m_bHasModifiedTransform = true;
    }
}


////////////////////////////////////////////////////////////////
//
// CShaderItem::MaybeRenewShaderInstance
//
// If current instance is in use by something else (i.e. in draw queue), we must create a new instance before changing parameter values
//
////////////////////////////////////////////////////////////////
void CShaderItem::MaybeRenewShaderInstance ( void )
{
    if ( m_pShaderInstance->m_iRefCount > 1 )
        RenewShaderInstance ();
}


////////////////////////////////////////////////////////////////
//
// CShaderItem::RenewShaderInstance
//
// Create/clone a new instance
//
////////////////////////////////////////////////////////////////
void CShaderItem::RenewShaderInstance ( void )
{
    CShaderInstance* pShaderInstance = new CShaderInstance ();
    pShaderInstance->PostConstruct ( m_pManager, this );
}


////////////////////////////////////////////////////////////////
//
// CShaderItem::GetUsesVertexShader
//
// Check if active technique uses a vertex shader
//
////////////////////////////////////////////////////////////////
bool CShaderItem::GetUsesVertexShader ( void )
{
    return m_pEffectWrap->m_bUsesVertexShader;
}
