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
void CShaderItem::PostConstruct ( CRenderItemManager* pManager, const SString& strFilename, SString& strOutStatus )
{
    Super::PostConstruct ( pManager );

    // Initial creation of d3d data
    CreateUnderlyingData ( strFilename, strOutStatus );
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
    return m_pD3DEffect != NULL;
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
    m_pD3DEffect->OnLostDevice ();
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
    m_pD3DEffect->OnResetDevice ();
}


////////////////////////////////////////////////////////////////
//
// CShaderItem::CreateUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CShaderItem::CreateUnderlyingData ( const SString& strFilename, SString& strOutStatus )
{
    assert ( !m_pD3DEffect );

    // Compile effect
    DWORD dwFlags = 0;
    dwFlags |= D3DXSHADER_DEBUG;
    dwFlags |= D3DXSHADER_PARTIALPRECISION;

    ID3DXInclude* pIncludeManager = NULL;   // TODO
    ID3DXEffect* pD3DEffect = NULL;
    LPD3DXBUFFER pBufferErrors = NULL;
    HRESULT Result = D3DXCreateEffectFromFile( m_pDevice, strFilename, NULL, pIncludeManager, dwFlags, NULL, &m_pD3DEffect, &pBufferErrors );            

    // Handle compile errors
    strOutStatus = "";
    if( pBufferErrors != NULL )
        strOutStatus = SStringX ( (CHAR*)pBufferErrors->GetBufferPointer() ).TrimEnd ( "\n" );
    SAFE_RELEASE( pBufferErrors );

    if( !m_pD3DEffect )
        return;

    // Find first valid technique
    D3DXHANDLE hTechnique = NULL;
    m_pD3DEffect->FindNextValidTechnique( NULL, &hTechnique );

    // Error if can't find a valid technique
    if ( !hTechnique )
    {
        strOutStatus = "No valid technique";
        SAFE_RELEASE ( m_pD3DEffect );
        return;
    }

    // Set technique
    m_pD3DEffect->SetTechnique( hTechnique );

    // Inform user of technique name
    D3DXTECHNIQUE_DESC Desc;
    m_pD3DEffect->GetTechniqueDesc( hTechnique, &Desc );
    strOutStatus = Desc.Name;

    // Add parameter handles
    D3DXEFFECT_DESC EffectDesc;
    m_pD3DEffect->GetDesc( &EffectDesc );
    for ( uint i = 0 ; i < EffectDesc.Parameters ; i++ )
    {
        D3DXHANDLE hParameter = m_pD3DEffect->GetParameter ( NULL, i );
        if ( !hParameter )
            break;
        D3DXPARAMETER_DESC Desc;
        m_pD3DEffect->GetParameterDesc( hParameter, &Desc );
        // Use semantic if it exists
        SString strName = Desc.Semantic ? Desc.Semantic : Desc.Name;
        // Add to correct lookup map
        if ( Desc.Type == D3DXPT_TEXTURE )
        {
            // Keep handle to first texture to get size of shader
            if ( !m_hFirstTexture )
                m_hFirstTexture = hParameter;
            MapSet ( m_texureHandleMap, strName.ToLower (), hParameter );
        }
        else
            MapSet ( m_valueHandleMap, strName.ToLower (), hParameter );
    }

    // Cache known parameters
    D3DXHANDLE* phWorld = MapFind ( m_valueHandleMap, "world" );
    m_hWorld = phWorld ? *phWorld : NULL;

    D3DXHANDLE* phView = MapFind ( m_valueHandleMap, "view" );
    m_hView = phView ? *phView : NULL;

    D3DXHANDLE* phProjection = MapFind ( m_valueHandleMap, "projection" );
    m_hProjection = phProjection ? *phProjection : NULL;

    D3DXHANDLE* phAll = MapFind ( m_valueHandleMap, "worldviewprojection" );
    m_hAll = phAll ? *phAll : NULL;

    D3DXHANDLE* phTime = MapFind ( m_valueHandleMap, "time" );
    m_hTime = phTime ? *phTime : NULL;

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
    SAFE_RELEASE( m_pD3DEffect )
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
    if ( D3DXHANDLE* phParameter = MapFind ( m_texureHandleMap, strName.ToLower () ) )
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
    if ( D3DXHANDLE* phParameter = MapFind ( m_valueHandleMap, strName.ToLower () ) )
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
    if ( D3DXHANDLE* phParameter = MapFind ( m_valueHandleMap, strName.ToLower () ) )
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
