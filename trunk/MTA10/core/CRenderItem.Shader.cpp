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


////////////////////////////////////////////////////////////////
//
// CShaderItem::PostConstruct
//
//
//
////////////////////////////////////////////////////////////////
void CShaderItem::PostConstruct ( CRenderItemManager* pManager, const SString& strFilename, const SString& strRootPath, SString& strOutStatus, bool bDebug )
{
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


    m_pEffectWrap = new CEffectWrap ();
    m_pEffectWrap->PostConstruct ( m_pManager, strFilename, strRootPath, strOutStatus, bDebug );

    if ( !m_pEffectWrap->IsValid () )
    {
        SAFE_RELEASE( m_pEffectWrap );
        return;
    }


    // Add parameter handles
    D3DXEFFECT_DESC EffectDesc;
    m_pEffectWrap->m_pD3DEffect->GetDesc( &EffectDesc );
    for ( uint i = 0 ; i < EffectDesc.Parameters ; i++ )
    {
        D3DXHANDLE hParameter = m_pEffectWrap->m_pD3DEffect->GetParameter ( NULL, i );
        if ( !hParameter )
            break;
        D3DXPARAMETER_DESC Desc;
        m_pEffectWrap->m_pD3DEffect->GetParameterDesc( hParameter, &Desc );
        // Use semantic if it exists
        SString strName = Desc.Semantic ? Desc.Semantic : Desc.Name;
        // Add to correct lookup map
        if ( Desc.Type == D3DXPT_TEXTURE )
        {
            // Keep handle to first texture to get size of shader
            if ( !m_hFirstTexture )
                m_hFirstTexture = hParameter;
            MapSet ( m_texureHandleMap, strName.ToUpper (), hParameter );
        }
        else
            MapSet ( m_valueHandleMap, strName.ToUpper (), hParameter );
    }


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
        m_pEffectWrap->hViewInv, "VIEWINV",
        m_pEffectWrap->hWorldInvTr, "WORLDINVTRANSPOSE",
        m_pEffectWrap->hViewInvTr, "VIEWINVTRANSPOSE",
        m_pEffectWrap->hCamPos, "CAMERAPOSITION",
        m_pEffectWrap->hCamDir, "CAMERADIRECTION",
        m_pEffectWrap->hTime, "TIME",
        m_pEffectWrap->hMaterialAmbient, "MATERIALAMBIENT",
        m_pEffectWrap->hMaterialDiffuse, "MATERIALDIFFUSE",
        m_pEffectWrap->hMaterialEmissive, "MATERIALEMISSIVE",
        m_pEffectWrap->hMaterialSpecular, "MATERIALSPECULAR",
        m_pEffectWrap->hMaterialSpecPower, "MATERIALSPECULARPOWER",
        m_pEffectWrap->hGlobalAmbient, "GLOBALAMBIENT",
        m_pEffectWrap->hLightAmbient, "LIGHTAMBIENT",
        m_pEffectWrap->hLightDiffuse, "LIGHTDIFFUSE",
        m_pEffectWrap->hLightSpecular, "LIGHTSPECULAR",
        m_pEffectWrap->hLightDirection, "LIGHTDIRECTION",
        m_pEffectWrap->hTexture0, "TEXTURE0",
        m_pEffectWrap->hTexture1, "TEXTURE1",
    };

    for ( uint h = 0 ; h < NUMELMS( handleNames ) ; h++ )
    {
        std::vector < SString > parts;
        handleNames[h].strNames.Split( ",", parts );
        D3DXHANDLE* phHandle = NULL;
        for ( uint n = 0 ; n < parts.size () && phHandle == NULL ; n++ )
        {
            phHandle = MapFind ( m_valueHandleMap, parts[n] );
            if ( !phHandle )
                phHandle = MapFind ( m_texureHandleMap, parts[n] );
        }
        handleNames[h].hHandle = phHandle ? *phHandle : NULL;
    }

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
    if ( D3DXHANDLE* phParameter = MapFind ( m_texureHandleMap, strName.ToUpper () ) )
    {
        // Check if value is changing
        if ( !m_pShaderInstance->CmpTextureValue( *phParameter, pTextureItem ) )
        {
            // Check if we need a new shader instance
            MaybeRenewShaderInstance ();
    
            if ( *phParameter == m_hFirstTexture )
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
    if ( D3DXHANDLE* phParameter = MapFind ( m_valueHandleMap, strName.ToUpper () ) )
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
    if ( D3DXHANDLE* phParameter = MapFind ( m_valueHandleMap, strName.ToUpper () ) )
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
