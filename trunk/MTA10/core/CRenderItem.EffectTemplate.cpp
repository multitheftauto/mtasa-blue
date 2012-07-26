/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CRenderItem.EffectTemplate.cpp
*  PURPOSE:
*
*****************************************************************************/

#include "StdInc.h"
#include "CRenderItem.EffectCloner.h"


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CEffectTemplateImpl
//
class CEffectTemplateImpl : public CEffectTemplate
{
public:
    ZERO_ON_NEW

    virtual void            PostConstruct           ( CRenderItemManager* pManager, const SString& strFilename, const SString& strRootPath, SString& strOutStatus, bool bDebug );
    virtual void            PreDestruct             ( void );
    virtual bool            IsValid                 ( void );
    virtual void            OnLostDevice            ( void );
    virtual void            OnResetDevice           ( void );

    // CEffectTemplate
    virtual bool            HaveFilesChanged        ( void );
    virtual int             GetTicksSinceLastUsed   ( void );
    virtual ID3DXEffect*    CloneD3DEffect          ( SString& strOutStatus, bool& bOutUsesVertexShader );
    virtual void            UnCloneD3DEffect        ( ID3DXEffect* pD3DEffect );

    // CEffectTemplateImpl
    void                    CreateUnderlyingData    ( const SString& strFilename, const SString& strRootPath, SString& strOutStatus, bool bDebug );
    void                    ReleaseUnderlyingData   ( void );

    ID3DXEffect*                        m_pD3DEffect;
    bool                                m_bHaveFilesChanged;
    std::map < SString, SString >       m_FileMD5Map;
    CTickCount                          m_TickCountLastUsed;
    std::set < ID3DXEffect* >           m_CloneList;
};


///////////////////////////////////////////////////////////////
//
// CEffectTemplate instantiation
//
///////////////////////////////////////////////////////////////
CEffectTemplate* NewEffectTemplate ( CRenderItemManager* pManager, const SString& strFilename, const SString& strRootPath, SString& strOutStatus, bool bDebug )
{
    CEffectTemplateImpl* m_pEffectTemplate = new CEffectTemplateImpl ();
    m_pEffectTemplate->PostConstruct ( pManager, strFilename, strRootPath, strOutStatus, bDebug );
    return m_pEffectTemplate;
}


namespace
{
    ////////////////////////////////////////////////////////////////////////////////////
    //
    // Helper class for D3DXCreateEffectFromFile() to ensure includes are correctly found and don't go outside the resource directory
    //
    ////////////////////////////////////////////////////////////////////////////////////
    class CIncludeManager : public ID3DXInclude
    {
        SString m_strRootPath;
        SString m_strCurrentPath;
    public:
        SString m_strReport;
        std::map < SString, SString > m_FileMD5Map;

        CIncludeManager::CIncludeManager( const SString& strRootPath, const SString& strCurrentPath )
        {
            m_strRootPath = strRootPath;
            m_strCurrentPath = strCurrentPath;
        }

        // * Include paths in the source are relative to the first .fx file
        // * Except include paths that starts with a \ are based from the resource's directory
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
                SString strMsg ( "[CIncludeManager: Illegal path %s]", *strPathFilename );
                m_strReport += strMsg;
                OutputDebugLine ( SStringX ( "[Shader] " ) + strMsg );
                return E_FAIL;
            }

            // Load file
            std::vector < char > buffer;
            if ( !FileLoad ( strPathFilename, buffer ) || buffer.empty () )
            {
                SString strMsg ( "[CIncludeManager: Can't find %s]", *strPathFilename );
                m_strReport += strMsg;
                OutputDebugLine ( SStringX ( "[Shader] " ) + strMsg );
                return E_FAIL;
            }


            // Allocate memory for file contents
            uint uiSize = buffer.size ();
            BYTE* pData = static_cast < BYTE* > ( malloc ( uiSize ) );
            memcpy( pData, &buffer[0], uiSize );

            // Set result
            *ppData = pData;
            *pBytes = uiSize;

            if ( !MapContains ( m_FileMD5Map, ConformPathForSorting ( strPathFilename ) ) )
            {
                // Add MD5 for this file
                SString strMD5 = CMD5Hasher::CalculateHexString ( pData, uiSize );
                MapSet ( m_FileMD5Map, ConformPathForSorting ( strPathFilename ), strMD5 );

                m_strReport += SString ( "[Loaded '%s' (%d bytes)]", *strPathFilename, uiSize );
            }

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
// CEffectTemplateImpl::PostConstruct
//
//
//
////////////////////////////////////////////////////////////////
void CEffectTemplateImpl::PostConstruct ( CRenderItemManager* pManager, const SString& strFilename, const SString& strRootPath, SString& strOutStatus, bool bDebug )
{
    Super::PostConstruct ( pManager );

    // Initial creation of d3d data
    CreateUnderlyingData ( strFilename, strRootPath, strOutStatus, bDebug );
}


////////////////////////////////////////////////////////////////
//
// CEffectTemplateImpl::PreDestruct
//
//
//
////////////////////////////////////////////////////////////////
void CEffectTemplateImpl::PreDestruct ( void )
{
    ReleaseUnderlyingData ();
    Super::PreDestruct ();
}


////////////////////////////////////////////////////////////////
//
// CEffectTemplateImpl::GetTicksSinceLastBeingUsed
//
//
//
////////////////////////////////////////////////////////////////
int CEffectTemplateImpl::GetTicksSinceLastUsed ( void )
{
    if ( !m_CloneList.empty () )
        return 0;   // Used right now

    CTickCount delta = CTickCount::Now ( true ) - m_TickCountLastUsed;
    return static_cast < int > ( delta.ToLongLong () );
}


////////////////////////////////////////////////////////////////
//
// CEffectTemplateImpl::IsValid
//
// Check underlying data is present
//
////////////////////////////////////////////////////////////////
bool CEffectTemplateImpl::IsValid ( void )
{
    return m_pD3DEffect != NULL;
}


////////////////////////////////////////////////////////////////
//
// CEffectTemplateImpl::OnLostDevice
//
// Release device stuff
//
////////////////////////////////////////////////////////////////
void CEffectTemplateImpl::OnLostDevice ( void )
{
    m_pD3DEffect->OnLostDevice ();
}


////////////////////////////////////////////////////////////////
//
// CEffectTemplateImpl::OnResetDevice
//
// Recreate device stuff
//
////////////////////////////////////////////////////////////////
void CEffectTemplateImpl::OnResetDevice ( void )
{
    m_pD3DEffect->OnResetDevice ();
}


////////////////////////////////////////////////////////////////
//
// CEffectTemplateImpl::CreateUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CEffectTemplateImpl::CreateUnderlyingData ( const SString& strFilename, const SString& strRootPath, SString& strOutStatus, bool bDebug )
{
    assert ( !m_pD3DEffect );

    // Compile effect
    DWORD dwFlags = 0;      // D3DXSHADER_PARTIALPRECISION, D3DXSHADER_DEBUG, D3DXFX_NOT_CLONEABLE;
    if ( bDebug )
        dwFlags |= D3DXSHADER_DEBUG;

    SString strMetaPath = strFilename.Right ( strFilename.length () - strRootPath.length () );
    CIncludeManager IncludeManager ( strRootPath, ExtractPath ( strMetaPath ) );
    LPD3DXBUFFER pBufferErrors = NULL;
    HRESULT hr = D3DXCreateEffectFromFile( m_pDevice, ExtractFilename ( strMetaPath ), NULL, &IncludeManager, dwFlags, NULL, &m_pD3DEffect, &pBufferErrors );            

    // Handle compile errors
    strOutStatus = "";
    if( pBufferErrors != NULL )
    {
        strOutStatus = SStringX ( (CHAR*)pBufferErrors->GetBufferPointer() ).TrimEnd ( "\n" );

        // Error messages sometimes contain the current directory. Remove that here.
        SString strCurrentDirectory = SharedUtil::GetCurrentDirectory ();
        strOutStatus = strOutStatus.ReplaceI ( strCurrentDirectory + "\\", "" );
        strOutStatus = strOutStatus.ReplaceI ( strCurrentDirectory, "" );
    }
    SAFE_RELEASE( pBufferErrors );

    if( !m_pD3DEffect )
    {
        if ( strOutStatus.empty () )
            strOutStatus = SString ( "[D3DXCreateEffectFromFile failed (%08x)%s]", hr, *IncludeManager.m_strReport );
        return;
    }

    // Find first valid technique
    D3DXHANDLE hTechnique = NULL;
    D3DXEFFECT_DESC EffectDesc;
    m_pD3DEffect->GetDesc ( &EffectDesc );

    for ( uint uiAttempt = 0 ; true ; uiAttempt++ )
    {
        SString strProblemInfo = "";
        for ( uint i = 0 ; i < EffectDesc.Techniques ; i++ )
        {
            D3DXHANDLE hTemp = m_pD3DEffect->GetTechnique ( i );
            HRESULT hr = m_pD3DEffect->ValidateTechnique ( hTemp );
            if ( SUCCEEDED( hr ) )
            {
                hTechnique = hTemp;
                break;
            }

            // Update problem string
            D3DXTECHNIQUE_DESC TechniqueDesc;
            m_pD3DEffect->GetTechniqueDesc( hTemp, &TechniqueDesc );
            strProblemInfo += SString ( "['%s' (%d/%d) failed (%08x)]", TechniqueDesc.Name, i, EffectDesc.Techniques, hr );
        }

        // Found valid technique
        if ( hTechnique )
            break;

        // Error if can't find a valid technique after 2nd attempt
        if ( uiAttempt > 0 )
        {
            strOutStatus = SString ( "No valid technique; [Techniques:%d %s]%s", EffectDesc.Techniques, *strProblemInfo, *IncludeManager.m_strReport );
            SAFE_RELEASE ( m_pD3DEffect );
            return;
        }

        // Try resetting samplers if 1st attempt failed
        LPDIRECT3DDEVICE9 pDevice;
        m_pD3DEffect->GetDevice ( &pDevice );
        for ( uint i = 0 ; i < 16 ; i++ )
        {
            pDevice->SetSamplerState ( i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
            pDevice->SetSamplerState ( i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
            pDevice->SetSamplerState ( i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
        }
    }


    // Set technique
    m_pD3DEffect->SetTechnique( hTechnique );

    // Inform user of technique name
    D3DXTECHNIQUE_DESC TechniqueDesc;
    m_pD3DEffect->GetTechniqueDesc( hTechnique, &TechniqueDesc );
    strOutStatus = TechniqueDesc.Name;

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

    // Copy MD5s of all loaded files
    m_FileMD5Map = IncludeManager.m_FileMD5Map;
    dassert ( !HaveFilesChanged() );
}


////////////////////////////////////////////////////////////////
//
// CEffectTemplateImpl::ReleaseUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CEffectTemplateImpl::ReleaseUnderlyingData ( void )
{
    SAFE_RELEASE ( m_pD3DEffect );
}


////////////////////////////////////////////////////////////////
//
// CEffectTemplateImpl::CloneD3DEffect
//
// Clone the d3d effect
//
////////////////////////////////////////////////////////////////
ID3DXEffect* CEffectTemplateImpl::CloneD3DEffect ( SString& strOutStatus, bool& bOutUsesVertexShader )
{
    // Clone D3DXEffect
    ID3DXEffect* pNewD3DEffect = NULL;
    LPDIRECT3DDEVICE9 pDevice = NULL;
    m_pD3DEffect->GetDevice ( &pDevice );
    m_pD3DEffect->CloneEffect ( pDevice, &pNewD3DEffect );

    if ( !pNewD3DEffect )
        return NULL;

    // Set the same technique
    D3DXHANDLE hTechnique = m_pD3DEffect->GetCurrentTechnique ();
    D3DXTECHNIQUE_DESC TechniqueDesc;
    m_pD3DEffect->GetTechniqueDesc( hTechnique, &TechniqueDesc );
    pNewD3DEffect->SetTechnique ( pNewD3DEffect->GetTechniqueByName ( TechniqueDesc.Name ) );

    // Output technique name
    strOutStatus = TechniqueDesc.Name;

    // Check if it uses a vertex shader
    bOutUsesVertexShader = false;
    for ( uint i = 0 ; i < TechniqueDesc.Passes ; i++ )
    {
        D3DXPASS_DESC PassDesc;
        m_pD3DEffect->GetPassDesc ( m_pD3DEffect->GetPass ( hTechnique, i ), &PassDesc );
        if ( PassDesc.pVertexShaderFunction )
            bOutUsesVertexShader = true;
    }

    // Add to list of clones
    assert ( !MapContains ( m_CloneList, pNewD3DEffect ) );
    MapInsert ( m_CloneList, pNewD3DEffect );
    return pNewD3DEffect;
}


////////////////////////////////////////////////////////////////
//
// CEffectTemplateImpl::UnCloneD3DEffect
//
// Remove from list and release
//
////////////////////////////////////////////////////////////////
void CEffectTemplateImpl::UnCloneD3DEffect ( ID3DXEffect* pOldD3DEffect )
{
    assert ( MapContains ( m_CloneList, pOldD3DEffect ) );
    MapRemove ( m_CloneList, pOldD3DEffect );

    if ( m_CloneList.empty () )
        m_TickCountLastUsed = CTickCount::Now ( true );

    SAFE_RELEASE( pOldD3DEffect );
}


////////////////////////////////////////////////////////////////
//
// CEffectTemplateImpl::HaveFilesChanged
//
// Recheck MD5's to see if the content has changed
//
////////////////////////////////////////////////////////////////
bool CEffectTemplateImpl::HaveFilesChanged ( void )
{
    if ( !m_bHaveFilesChanged )
    {
        for ( std::map < SString, SString >::const_iterator iter = m_FileMD5Map.begin () ; iter != m_FileMD5Map.end () ; ++iter )
        {
            const SString& strPathFilename = iter->first;
            const SString& strMD5 = iter->second;
            SString strNewMD5 = CMD5Hasher::CalculateHexString ( strPathFilename );
            if ( strNewMD5 != strMD5 )
            {
                OutputDebugLine ( SString ( "[Shader] %s file has changed from %s to %s", *strPathFilename, *strMD5, *strNewMD5 ) );
                m_bHaveFilesChanged = true;
            }
        }
    }
    return m_bHaveFilesChanged;
}
