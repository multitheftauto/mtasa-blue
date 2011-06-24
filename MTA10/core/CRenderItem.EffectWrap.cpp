/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CRenderItem.ShaderHandles.cpp
*  PURPOSE:
*  DEVELOPERS:  zidiot
*
*****************************************************************************/

#include "StdInc.h"


namespace
{
    // Helper class for D3DXCreateEffectFromFile() to ensure includes are correctly found and don't go outside the resource directory
    class CIncludeManager : public ID3DXInclude
    {
        SString m_strRootPath;
        SString m_strCurrentPath;
    public:

        CIncludeManager::CIncludeManager( const SString& strRootPath, const SString& strCurrentPath )
        {
            m_strRootPath = strRootPath;
            m_strCurrentPath = strCurrentPath;
        }

        STDMETHOD(Open)(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
        {
            SString strPathFilename = PathConform ( pFileName );

            // Prepend file name with m_strRootPath + m_strCurrentPath, unless it starts with a PATH_SEPERATOR, then just prepend m_strRootPath
            if ( strPathFilename.Left ( 1 ) == PATH_SEPERATOR )
                strPathFilename = PathJoin ( m_strRootPath, strPathFilename );
            else
                strPathFilename = PathJoin ( m_strRootPath, m_strCurrentPath, strPathFilename );

            // Check for illegal characters
            if ( strPathFilename.Contains ( ".." ) )
            {
                OutputDebugLine ( SString ( "CIncludeManager: Illegal path %s", *strPathFilename ) );
                return E_FAIL;
            }

            // Load file
            std::vector < char > buffer;
            if ( !FileLoad ( strPathFilename, buffer ) )
            {
                OutputDebugLine ( SString ( "CIncludeManager: Can't find %s", *strPathFilename ) );
                return E_FAIL;
            }

            // Allocate memory for file contents
            uint uiSize = buffer.size ();
            BYTE* pData = static_cast < BYTE* > ( malloc ( uiSize ) );
            memcpy( pData, &buffer[0], uiSize );

            // Set result
            *ppData = pData;
            *pBytes = uiSize;

            return S_OK;
        }

        STDMETHOD(Close)(LPCVOID pData)
        {
            // Free memory allocated for file contents
            delete pData;
            return S_OK;
        }
    };
}


////////////////////////////////////////////////////////////////
//
// CEffectWrap::PostConstruct
//
//
//
////////////////////////////////////////////////////////////////
void CEffectWrap::PostConstruct ( CRenderItemManager* pManager, const SString& strFilename, const SString& strRootPath, SString& strOutStatus, bool bDebug )
{
    Super::PostConstruct ( pManager );
    // Initial creation of d3d data
    CreateUnderlyingData ( strFilename, strRootPath, strOutStatus, bDebug );
}


////////////////////////////////////////////////////////////////
//
// CEffectWrap::PreDestruct
//
//
//
////////////////////////////////////////////////////////////////
void CEffectWrap::PreDestruct ( void )
{
    ReleaseUnderlyingData ();
    Super::PreDestruct ();
}


////////////////////////////////////////////////////////////////
//
// CEffectWrap::IsValid
//
// Check underlying data is present
//
////////////////////////////////////////////////////////////////
bool CEffectWrap::IsValid ( void )
{
    return m_pD3DEffect != NULL;
}


////////////////////////////////////////////////////////////////
//
// CEffectWrap::OnLostDevice
//
// Release device stuff
//
////////////////////////////////////////////////////////////////
void CEffectWrap::OnLostDevice ( void )
{
    m_pD3DEffect->OnLostDevice ();
}


////////////////////////////////////////////////////////////////
//
// CEffectWrap::OnResetDevice
//
// Recreate device stuff
//
////////////////////////////////////////////////////////////////
void CEffectWrap::OnResetDevice ( void )
{
    m_pD3DEffect->OnResetDevice ();
}


////////////////////////////////////////////////////////////////
//
// CEffectWrap::CreateUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CEffectWrap::CreateUnderlyingData ( const SString& strFilename, const SString& strRootPath, SString& strOutStatus, bool bDebug )
{
    assert ( !m_pD3DEffect );

    // Compile effect
    DWORD dwFlags = D3DXSHADER_PARTIALPRECISION;
    if ( bDebug )
        dwFlags |= D3DXSHADER_DEBUG;
    else
        dwFlags |= D3DXFX_NOT_CLONEABLE;

    SString strMetaPath = strFilename.Right ( strFilename.length () - strRootPath.length () );
    CIncludeManager IncludeManager ( strRootPath, ExtractPath ( strMetaPath ) );
    LPD3DXBUFFER pBufferErrors = NULL;
    D3DXCreateEffectFromFile( m_pDevice, ExtractFilename ( strMetaPath ), NULL, &IncludeManager, dwFlags, NULL, &m_pD3DEffect, &pBufferErrors );            

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

    if ( bDebug )
    {
        // Disassemble effect
	    LPD3DXBUFFER pDisassembly = NULL;
	    if ( SUCCEEDED( D3DXDisassembleEffect( m_pD3DEffect, false, &pDisassembly ) ) && pDisassembly )
        {
	        LPVOID pData = pDisassembly->GetBufferPointer();
	        DWORD Size = pDisassembly->GetBufferSize();

	        if( pData && Size )
	        {
                SString strDisassemblyContents;
                strDisassemblyContents.assign ( (const char*)pData, Size - 1 );
                FileSave ( strFilename + ".dis", strDisassemblyContents );
	        }

	        SAFE_RELEASE( pDisassembly );
        }
    }
}


////////////////////////////////////////////////////////////////
//
// CEffectWrap::ReleaseUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CEffectWrap::ReleaseUnderlyingData ( void )
{
    SAFE_RELEASE ( m_pD3DEffect );
}


////////////////////////////////////////////////////////////////
//
// CEffectWrap::ApplyCommonHandles
//
//
//
////////////////////////////////////////////////////////////////
void CEffectWrap::ApplyCommonHandles ( void )
{
    ID3DXEffect* pD3DEffect = m_pD3DEffect;

    CEffectWrap& m_CommonHandles = *this;

    LPDIRECT3DDEVICE9 pDevice;
    pD3DEffect->GetDevice ( &pDevice );

    D3DXMATRIX matWorld, matView, matProjection;
    pDevice->GetTransform ( D3DTS_WORLD, &matWorld );
    pDevice->GetTransform ( D3DTS_VIEW, &matView );
    pDevice->GetTransform ( D3DTS_PROJECTION, &matProjection );

    // Set matrices
    if ( m_CommonHandles.hWorld )
        pD3DEffect->SetMatrix ( m_CommonHandles.hWorld, &matWorld );

    if ( m_CommonHandles.hView )
        pD3DEffect->SetMatrix ( m_CommonHandles.hView, &matView );

    if ( m_CommonHandles.hProjection )
        pD3DEffect->SetMatrix ( m_CommonHandles.hProjection, &matProjection );

    if ( m_CommonHandles.hWorldView )
    {
        D3DXMATRIX& matWorldView = matWorld * matView;
        pD3DEffect->SetMatrix ( m_CommonHandles.hWorldView, &matWorldView );
    }

    if ( m_CommonHandles.hWorldViewProj )
    {
        D3DXMATRIX matWorldViewProj = matWorld * matView * matProjection;
        pD3DEffect->SetMatrix ( m_CommonHandles.hWorldViewProj, &matWorldViewProj );
    }

    if ( m_CommonHandles.hViewProj )
    {
        D3DXMATRIX& matViewProj = matView * matProjection;
        pD3DEffect->SetMatrix ( m_CommonHandles.hViewProj, &matViewProj );
    }

    D3DXMATRIX matViewInv;
    D3DXMATRIX* pmatViewInv = NULL;
    if ( m_CommonHandles.hViewInv )
    {
        pmatViewInv = D3DXMatrixInverse ( &matViewInv, NULL, &matView );
        pD3DEffect->SetMatrix ( m_CommonHandles.hViewInv, &matViewInv );
    }

    if ( m_CommonHandles.hWorldInvTr )
    {
        D3DXMATRIX matWorldTr;
        D3DXMatrixTranspose ( &matWorldTr, &matWorld );
        D3DXMATRIX matWorldInvTr;
        D3DXMatrixInverse ( &matWorldInvTr, NULL, &matWorldTr );
        pD3DEffect->SetMatrix ( m_CommonHandles.hWorldInvTr, &matWorldInvTr );
    }

    if ( m_CommonHandles.hViewInvTr )
    {
        D3DXMATRIX matViewTr;
        D3DXMatrixTranspose ( &matViewTr, &matView );
        D3DXMATRIX matViewInvTr;
        D3DXMatrixInverse ( &matViewInvTr, NULL, &matViewTr );
        pD3DEffect->SetMatrix ( m_CommonHandles.hViewInvTr, &matViewInvTr );
    }

    // Set camera position
    if ( m_CommonHandles.hCamPos )
    {
        if ( !pmatViewInv )
            pmatViewInv = D3DXMatrixInverse ( &matViewInv, NULL, &matView );

        D3DXVECTOR3 vecCamPos ( matViewInv.m[3][0], matViewInv.m[3][1], matViewInv.m[3][2] );
        pD3DEffect->SetFloatArray ( m_CommonHandles.hCamPos, (float*)&vecCamPos, 3 );
    }

    // Set camera direction
    if ( m_CommonHandles.hCamDir )
    {
        if ( !pmatViewInv )
            pmatViewInv = D3DXMatrixInverse ( &matViewInv, NULL, &matView );

        D3DXVECTOR3 vecCamDir ( matViewInv.m[2][0], matViewInv.m[2][1], matViewInv.m[2][2] );
        pD3DEffect->SetFloatArray ( m_CommonHandles.hCamDir, (float*)&vecCamDir, 3 );
    }

    // Set time
    if ( m_CommonHandles.hTime )
        pD3DEffect->SetFloat ( m_CommonHandles.hTime, GetTickCount64_ () * 0.001f );


    // Set textures
    if ( m_CommonHandles.hTexture0 )
    {
        IDirect3DBaseTexture9* pD3DTexture;
        pDevice->GetTexture ( 0, &pD3DTexture );
        pD3DEffect->SetTexture ( m_CommonHandles.hTexture0, pD3DTexture );
    }

    if ( m_CommonHandles.hTexture1 )
    {
        IDirect3DBaseTexture9* pD3DTexture;
        pDevice->GetTexture ( 1, &pD3DTexture );
        pD3DEffect->SetTexture ( m_CommonHandles.hTexture0, pD3DTexture );
    }

    // Set material
    if ( m_CommonHandles.hMaterialAmbient || m_CommonHandles.hMaterialDiffuse || m_CommonHandles.hMaterialEmissive || m_CommonHandles.hMaterialSpecular || m_CommonHandles.hMaterialSpecPower )
    {
        D3DMATERIAL9 D3DMaterial;
        pDevice->GetMaterial ( &D3DMaterial );

        if ( m_CommonHandles.hMaterialDiffuse )
            pD3DEffect->SetFloatArray ( m_CommonHandles.hMaterialDiffuse, (float*)&D3DMaterial.Diffuse, 4 );

        if ( m_CommonHandles.hMaterialAmbient )
            pD3DEffect->SetFloatArray ( m_CommonHandles.hMaterialAmbient, (float*)&D3DMaterial.Ambient, 4 );

        if ( m_CommonHandles.hMaterialEmissive )
            pD3DEffect->SetFloatArray ( m_CommonHandles.hMaterialEmissive, (float*)&D3DMaterial.Emissive, 4 );

        if ( m_CommonHandles.hMaterialSpecular )
            pD3DEffect->SetFloatArray ( m_CommonHandles.hMaterialSpecular, (float*)&D3DMaterial.Specular, 4 );

        if ( m_CommonHandles.hMaterialSpecPower )
            pD3DEffect->SetFloat ( m_CommonHandles.hMaterialSpecPower, D3DMaterial.Power );
    }

    // Set ambient
    if ( m_CommonHandles.hGlobalAmbient )
    {
        DWORD dwGlobalAmbient = 0;
        pDevice->GetRenderState ( D3DRS_AMBIENT, &dwGlobalAmbient );
        D3DXCOLOR globalAmbient ( dwGlobalAmbient );
        pD3DEffect->SetFloatArray ( m_CommonHandles.hGlobalAmbient, (float*)&globalAmbient, 4 );
    }

    // Set light
    if ( m_CommonHandles.hLightAmbient || m_CommonHandles.hLightDiffuse || m_CommonHandles.hLightSpecular || m_CommonHandles.hLightDirection )
    {
        D3DXCOLOR totalAmbient ( 0, 0, 0, 0 );
        float fStrongestDiffuseScore = 0;
        D3DXCOLOR strongestDiffuse ( 0, 0, 0, 0 );
        D3DXCOLOR strongestSpecular ( 0, 0, 0, 0 );
        D3DXVECTOR3 strongestDirection ( 0, 0, 1 );
        for ( uint i = 0 ; i < 4 ; i++ )
        {
            BOOL bEnabled;
            if ( SUCCEEDED( pDevice->GetLightEnable ( i, &bEnabled ) ) && bEnabled )
            {
                D3DLIGHT9 D3DLight;
                pDevice->GetLight ( i, &D3DLight );
                if ( D3DLight.Type == D3DLIGHT_DIRECTIONAL )
                {
                    totalAmbient += D3DLight.Ambient;
                    float fScore = D3DLight.Diffuse.r + D3DLight.Diffuse.g + D3DLight.Diffuse.b;
                    if ( fScore > fStrongestDiffuseScore )
                    {
                        fStrongestDiffuseScore = fScore;
                        strongestDiffuse = D3DLight.Diffuse;
                        strongestSpecular = D3DLight.Specular;
                        strongestDirection = D3DLight.Direction;
                    }
                }
            }
        }

        if ( m_CommonHandles.hLightAmbient )
        {
            totalAmbient.a = Min ( totalAmbient.a, 1.0f );
            totalAmbient.r = Min ( totalAmbient.r, 1.0f );
            totalAmbient.g = Min ( totalAmbient.g, 1.0f );
            totalAmbient.b = Min ( totalAmbient.b, 1.0f );
            pD3DEffect->SetFloatArray ( m_CommonHandles.hLightAmbient, (float*)&totalAmbient, 4 );
        }

        if ( m_CommonHandles.hLightDiffuse )
            pD3DEffect->SetFloatArray ( m_CommonHandles.hLightDiffuse, (float*)&strongestDiffuse, 4 );

        if ( m_CommonHandles.hLightSpecular )
            pD3DEffect->SetFloatArray ( m_CommonHandles.hLightSpecular, (float*)&strongestSpecular, 4 );

        if ( m_CommonHandles.hLightDirection )
            pD3DEffect->SetFloatArray ( m_CommonHandles.hLightDirection, (float*)&strongestDirection, 4 );
    }
};
