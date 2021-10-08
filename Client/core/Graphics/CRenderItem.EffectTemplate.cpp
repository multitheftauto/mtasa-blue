/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CRenderItem.EffectTemplate.cpp
 *  PURPOSE:     A compiled effect to clone d3d effects from
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CRenderItem.EffectTemplate.h"
#include <DXHook/CProxyDirect3DEffect.h>

///////////////////////////////////////////////////////////////
//
// CEffectTemplate instantiation
//
///////////////////////////////////////////////////////////////
CEffectTemplate* NewEffectTemplate(CRenderItemManager* pManager, const SString& strFile, const SString& strRootPath, bool bIsRawData, SString& strOutStatus,
                                   bool bDebug, const EffectMacroList& macros, HRESULT& outHResult)
{
    CEffectTemplate* pEffectTemplate = new CEffectTemplate();
    pEffectTemplate->PostConstruct(pManager, strFile, strRootPath, bIsRawData, strOutStatus, bDebug, macros);

    outHResult = pEffectTemplate->m_CreateHResult;
    if (!pEffectTemplate->IsValid())
    {
        SAFE_RELEASE(pEffectTemplate);
    }

    return pEffectTemplate;
}

namespace
{
    ////////////////////////////////////////////////////////////////////////////////////
    //
    // Helper class for D3DXCreateEffect functions to ensure includes are correctly found and don't go outside the resource directory
    //
    ////////////////////////////////////////////////////////////////////////////////////
    class CIncludeManager : public ID3DXInclude
    {
        SString m_strRootPath;
        SString m_strCurrentPath;

    public:
        SString                    m_strReport;
        std::map<SString, SString> m_FileMD5Map;

        CIncludeManager::CIncludeManager(const SString& strRootPath, const SString& strCurrentPath)
        {
            m_strRootPath = strRootPath;
            m_strCurrentPath = strCurrentPath;
        }

        // * Include paths in the source are relative to the first .fx file
        // * Except include paths that starts with a \ are based from the resource's directory
        STDMETHOD(Open)(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
        {
            SString strPathFilename = PathConform(pFileName);

            // Prepend file name with m_strRootPath + m_strCurrentPath, unless it starts with a PATH_SEPERATOR, then just prepend m_strRootPath
            if (strPathFilename.Left(1) == PATH_SEPERATOR)
                strPathFilename = PathJoin(m_strRootPath, strPathFilename);
            else
                strPathFilename = PathJoin(m_strRootPath, m_strCurrentPath, strPathFilename);

            // Check for illegal characters
            if (strPathFilename.Contains(".."))
            {
                SString strMsg("[CIncludeManager: Illegal path %s]", *strPathFilename);
                m_strReport += strMsg;
                OutputDebugLine(SStringX("[Shader] ") + strMsg);
                return E_FAIL;
            }

            // Load file
            std::vector<char> buffer;
            if (!FileLoad(strPathFilename, buffer) || buffer.empty())
            {
                SString strMsg("[CIncludeManager: Can't find %s]", *strPathFilename);
                m_strReport += strMsg;
                OutputDebugLine(SStringX("[Shader] ") + strMsg);
                return E_FAIL;
            }

            // Allocate memory for file contents
            uint  uiSize = buffer.size();
            BYTE* pData = static_cast<BYTE*>(malloc(uiSize));
            memcpy(pData, &buffer[0], uiSize);

            // Set result
            *ppData = pData;
            *pBytes = uiSize;

            if (!MapContains(m_FileMD5Map, ConformPathForSorting(strPathFilename)))
            {
                // Add MD5 for this file
                SString strMD5 = CMD5Hasher::CalculateHexString(pData, uiSize);
                MapSet(m_FileMD5Map, ConformPathForSorting(strPathFilename), strMD5);

                m_strReport += SString("[Loaded '%s' (%d bytes)]", *strPathFilename, uiSize);
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
}            // namespace

////////////////////////////////////////////////////////////////
//
// CEffectTemplate::PostConstruct
//
//
//
////////////////////////////////////////////////////////////////
void CEffectTemplate::PostConstruct(CRenderItemManager* pManager, const SString& strFile, const SString& strRootPath, bool bIsRawData, SString& strOutStatus,
                                    bool bDebug, const EffectMacroList& macros)
{
    Super::PostConstruct(pManager);

    // Initial creation of d3d data
    CreateUnderlyingData(strFile, strRootPath, bIsRawData, strOutStatus, bDebug, macros);
}

////////////////////////////////////////////////////////////////
//
// CEffectTemplate::PreDestruct
//
//
//
////////////////////////////////////////////////////////////////
void CEffectTemplate::PreDestruct()
{
    ReleaseUnderlyingData();
    Super::PreDestruct();
}

////////////////////////////////////////////////////////////////
//
// CEffectTemplate::GetTicksSinceLastBeingUsed
//
//
//
////////////////////////////////////////////////////////////////
int CEffectTemplate::GetTicksSinceLastUsed()
{
    if (m_uiCloneCount != 0)
        return 0;            // Used right now

    CTickCount delta = CTickCount::Now(true) - m_TickCountLastUsed;
    return static_cast<int>(delta.ToLongLong()) + 1;
}

////////////////////////////////////////////////////////////////
//
// CEffectTemplate::IsValid
//
// Check underlying data is present
//
////////////////////////////////////////////////////////////////
bool CEffectTemplate::IsValid()
{
    return m_pD3DEffect != NULL;
}

////////////////////////////////////////////////////////////////
//
// CEffectTemplate::OnLostDevice
//
// Release device stuff
//
////////////////////////////////////////////////////////////////
void CEffectTemplate::OnLostDevice()
{
    m_pD3DEffect->OnLostDevice();
}

////////////////////////////////////////////////////////////////
//
// CEffectTemplate::OnResetDevice
//
// Recreate device stuff
//
////////////////////////////////////////////////////////////////
void CEffectTemplate::OnResetDevice()
{
    m_pD3DEffect->OnResetDevice();
}

////////////////////////////////////////////////////////////////
//
// CEffectTemplate::CreateUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CEffectTemplate::CreateUnderlyingData(const SString& strFile, const SString& strRootPath, bool bIsRawData, SString& strOutStatus, bool bDebug,
                                           const EffectMacroList& macros)
{
    assert(!m_pD3DEffect);

    // Make defines
    bool bUsesRAWZ = CGraphics::GetSingleton().GetRenderItemManager()->GetDepthBufferFormat() == RFORMAT_RAWZ;

    std::vector<D3DXMACRO> macroList;

    macroList.push_back(D3DXMACRO());
    macroList.back().Name = "IS_DEPTHBUFFER_RAWZ";
    macroList.back().Definition = bUsesRAWZ ? "1" : "0";

    for (const auto& [name, definition] : macros)
        macroList.push_back(std::move(D3DXMACRO{name.c_str(), definition.c_str()}));

    macroList.push_back(D3DXMACRO());
    macroList.back().Name = NULL;
    macroList.back().Definition = NULL;

    // Compile effect
    DWORD dwFlags = D3DXFX_LARGEADDRESSAWARE;            // D3DXSHADER_PARTIALPRECISION, D3DXSHADER_DEBUG, D3DXFX_NOT_CLONEABLE;
    if (bDebug)
        dwFlags |= D3DXSHADER_DEBUG;

    SString         strMetaPath = bIsRawData ? "" : strFile.Right(strFile.length() - strRootPath.length());
    CIncludeManager IncludeManager(strRootPath, ExtractPath(strMetaPath));
    LPD3DXBUFFER    pBufferErrors = NULL;
    if (bIsRawData)
        m_CreateHResult =
            MyD3DXCreateEffect(m_pDevice, strFile, strFile.length(), &macroList[0], &IncludeManager, dwFlags, NULL, &m_pD3DEffect, &pBufferErrors);
    else
        m_CreateHResult =
            MyD3DXCreateEffectFromFile(m_pDevice, ExtractFilename(strMetaPath), &macroList[0], &IncludeManager, dwFlags, NULL, &m_pD3DEffect, &pBufferErrors);

    // Handle compile errors
    strOutStatus = "";
    if (pBufferErrors != NULL)
    {
        strOutStatus = SStringX((CHAR*)pBufferErrors->GetBufferPointer()).TrimEnd("\n");

        // Error messages sometimes contain the current directory. Remove that here.
        SString strCurrentDirectory = GetSystemCurrentDirectory();
        strOutStatus = strOutStatus.ReplaceI(strCurrentDirectory + "\\", "");
        strOutStatus = strOutStatus.ReplaceI(strCurrentDirectory, "");
    }
    SAFE_RELEASE(pBufferErrors);

    if (!m_pD3DEffect)
    {
        if (strOutStatus.empty())
            strOutStatus = SString("[D3DXCreateEffectFromFile failed (%08x)%s]", m_CreateHResult, *IncludeManager.m_strReport);
        return;
    }

    // Find first valid technique
    D3DXHANDLE      hTechnique = NULL;
    D3DXEFFECT_DESC EffectDesc;
    m_pD3DEffect->GetDesc(&EffectDesc);

    for (uint uiAttempt = 0; true; uiAttempt++)
    {
        SString strProblemInfo = "";
        for (uint i = 0; i < EffectDesc.Techniques; i++)
        {
            SString    strErrorExtra;
            D3DXHANDLE hTemp = m_pD3DEffect->GetTechnique(i);
            HRESULT    hr = m_pD3DEffect->ValidateTechnique(hTemp);
            if (SUCCEEDED(hr))
            {
                // Check depth buffer rules
                if (ValidateDepthBufferUsage(hTemp, strErrorExtra))
                {
                    hTechnique = hTemp;
                    break;
                }
            }

            // Update problem string
            D3DXTECHNIQUE_DESC TechniqueDesc;
            m_pD3DEffect->GetTechniqueDesc(hTemp, &TechniqueDesc);
            strProblemInfo += SString("['%s' (%d/%d) failed (%08x)%s]", TechniqueDesc.Name, i, EffectDesc.Techniques, hr, *strErrorExtra);
        }

        // Found valid technique
        if (hTechnique)
            break;

        // Error if can't find a valid technique after 2nd attempt
        if (uiAttempt > 0)
        {
            strOutStatus = SString("No valid technique; [Techniques:%d %s]%s", EffectDesc.Techniques, *strProblemInfo, *IncludeManager.m_strReport);
            SAFE_RELEASE(m_pD3DEffect);
            return;
        }

        // Try resetting samplers if 1st attempt failed
        LPDIRECT3DDEVICE9 pDevice;
        m_pD3DEffect->GetDevice(&pDevice);
        for (uint i = 0; i < 16; i++)
        {
            pDevice->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
            pDevice->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
            pDevice->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
        }
    }

    // Set technique
    m_pD3DEffect->SetTechnique(hTechnique);

    // Get/cache parameter handles
    ReadParameterHandles();

    // Inform user of technique name
    D3DXTECHNIQUE_DESC TechniqueDesc;
    m_pD3DEffect->GetTechniqueDesc(hTechnique, &TechniqueDesc);
    m_strTechniqueName = TechniqueDesc.Name;
    strOutStatus = m_strTechniqueName;

    // Check if any technique uses a vertex shader
    m_bUsesVertexShader = false;
    for (uint i = 0; i < EffectDesc.Techniques; i++)
    {
        D3DXHANDLE         hTechnique = m_pD3DEffect->GetTechnique(i);
        D3DXTECHNIQUE_DESC TechniqueDesc;
        m_pD3DEffect->GetTechniqueDesc(hTechnique, &TechniqueDesc);
        for (uint i = 0; i < TechniqueDesc.Passes; i++)
        {
            D3DXPASS_DESC PassDesc;
            m_pD3DEffect->GetPassDesc(m_pD3DEffect->GetPass(hTechnique, i), &PassDesc);
            if (PassDesc.pVertexShaderFunction)
                m_bUsesVertexShader = true;
        }
    }

    if (bDebug)
    {
        // Disassemble effect
        LPD3DXBUFFER pDisassembly = NULL;
        if (SUCCEEDED(D3DXDisassembleEffect(m_pD3DEffect, false, &pDisassembly)) && pDisassembly)
        {
            LPVOID pData = pDisassembly->GetBufferPointer();
            DWORD  Size = pDisassembly->GetBufferSize();

            if (pData && Size)
            {
                SString strDisassemblyContents;
                strDisassemblyContents.assign((const char*)pData, Size - 1);
                FileSave(bIsRawData ? strRootPath + "raw_data.dis" : strFile + ".dis", strDisassemblyContents);
            }

            SAFE_RELEASE(pDisassembly);
        }
    }

    // Copy MD5s of all loaded files
    m_FileMD5Map = IncludeManager.m_FileMD5Map;
    dassert(!HaveFilesChanged());
}

////////////////////////////////////////////////////////////////
//
// CEffectTemplate::ValidateDepthBufferUsage
//
// Check if technique passes our rules
// Returns false if should fail validation
//
////////////////////////////////////////////////////////////////
bool CEffectTemplate::ValidateDepthBufferUsage(D3DXHANDLE hTechnique, SString& strOutErrorExtra)
{
    m_bUsesDepthBuffer = false;

    // Check depthbuffer handle
    D3DXHANDLE hDepthBuffer = m_pD3DEffect->GetParameterByName(NULL, "DEPTHBUFFER");
    if (!hDepthBuffer)
        hDepthBuffer = m_pD3DEffect->GetParameterBySemantic(NULL, "DEPTHBUFFER");
    if (hDepthBuffer)
    {
        D3DXPARAMETER_DESC ParameterDesc;
        m_pD3DEffect->GetParameterDesc(hDepthBuffer, &ParameterDesc);
        if (ParameterDesc.Type >= D3DXPT_TEXTURE && ParameterDesc.Type <= D3DXPT_TEXTURECUBE)
        {
            if (m_pD3DEffect->IsParameterUsed(hDepthBuffer, hTechnique))
            {
                ERenderFormat depthBufferFormat = m_pManager->GetDepthBufferFormat();
                if (depthBufferFormat == RFORMAT_UNKNOWN)
                {
                    strOutErrorExtra += " DEPTHBUFFER used, but readable depth buffer is not available";
                    return false;
                }
                m_bUsesDepthBuffer = true;
            }
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////
//
// CEffectTemplate::ReleaseUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CEffectTemplate::ReleaseUnderlyingData()
{
    SAFE_RELEASE(m_pD3DEffect);
}

////////////////////////////////////////////////////////////////
//
// CEffectTemplate::CloneD3DEffect
//
// Clone the d3d effect
//
////////////////////////////////////////////////////////////////
CEffectWrap* CEffectTemplate::CloneD3DEffect(SString& strOutStatus)
{
    CEffectWrap* pEffectWrap = new CEffectWrap();
    pEffectWrap->PostConstruct(m_pManager, this);
    strOutStatus = m_strTechniqueName;
    m_uiCloneCount++;
    return pEffectWrap;
}

////////////////////////////////////////////////////////////////
//
// CEffectTemplate::UnCloneD3DEffect
//
// Remove from list and release
//
////////////////////////////////////////////////////////////////
void CEffectTemplate::UnCloneD3DEffect(CEffectWrap* pEffectWrap)
{
    assert(m_uiCloneCount > 0);
    m_uiCloneCount--;

    if (m_uiCloneCount == 0)
        m_TickCountLastUsed = CTickCount::Now(true);

    SAFE_RELEASE(pEffectWrap);
}

////////////////////////////////////////////////////////////////
//
// CEffectTemplate::HaveFilesChanged
//
// Recheck MD5's to see if the content has changed
//
////////////////////////////////////////////////////////////////
bool CEffectTemplate::HaveFilesChanged()
{
    if (!m_bHaveFilesChanged)
    {
        for (std::map<SString, SString>::const_iterator iter = m_FileMD5Map.begin(); iter != m_FileMD5Map.end(); ++iter)
        {
            const SString& strPathFilename = iter->first;
            const SString& strMD5 = iter->second;
            SString        strNewMD5 = CMD5Hasher::CalculateHexString(strPathFilename);
            if (strNewMD5 != strMD5)
            {
                OutputDebugLine(SString("[Shader] %s file has changed from %s to %s", *strPathFilename, *strMD5, *strNewMD5));
                m_bHaveFilesChanged = true;
            }
        }
    }
    return m_bHaveFilesChanged;
}
