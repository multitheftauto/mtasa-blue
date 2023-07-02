/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CRenderItem.Shader.cpp
 *  PURPOSE:
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CRenderItem.EffectCloner.h"
#include "CRenderItem.EffectTemplate.h"

uint CShaderItem::ms_uiCreateTimeCounter = 0;

////////////////////////////////////////////////////////////////
//
// CShaderItem::PostConstruct
//
//
//
////////////////////////////////////////////////////////////////
void CShaderItem::PostConstruct(CRenderItemManager* pManager, const SString& strFile, const SString& strRootPath, bool bIsRawData, SString& strOutStatus,
                                float fPriority, float fMaxDistance, bool bLayered, bool bDebug, int iTypeMask, const EffectMacroList& macros)
{
    m_fPriority = fPriority;
    m_uiCreateTime = ms_uiCreateTimeCounter++;            // Priority tie breaker
    m_fMaxDistanceSq = fMaxDistance * fMaxDistance;
    m_bLayered = bLayered;
    m_iTypeMask = iTypeMask;

    Super::PostConstruct(pManager);

    // Initial creation of d3d data
    CreateUnderlyingData(strFile, strRootPath, bIsRawData, strOutStatus, bDebug, macros);
}

////////////////////////////////////////////////////////////////
//
// CShaderItem::PreDestruct
//
//
//
////////////////////////////////////////////////////////////////
void CShaderItem::PreDestruct()
{
    ReleaseUnderlyingData();
    Super::PreDestruct();
}

////////////////////////////////////////////////////////////////
//
// CShaderItem::IsValid
//
// Check underlying data is present
//
////////////////////////////////////////////////////////////////
bool CShaderItem::IsValid()
{
    return m_pEffectWrap;
}

////////////////////////////////////////////////////////////////
//
// CShaderItem::OnLostDevice
//
// Release device stuff
//
////////////////////////////////////////////////////////////////
void CShaderItem::OnLostDevice()
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
void CShaderItem::OnResetDevice()
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
void CShaderItem::CreateUnderlyingData(const SString& strFile, const SString& strRootPath, bool bIsRawData, SString& strOutStatus, bool bDebug,
                                       const EffectMacroList& macros)
{
    assert(!m_pEffectWrap);
    assert(!m_pShaderInstance);

    m_pEffectWrap = m_pManager->GetEffectCloner()->CreateD3DEffect(strFile, strRootPath, bIsRawData, strOutStatus, bDebug, macros);
    if (!m_pEffectWrap)
        return;

    m_pManager->NotifyShaderItemUsesDepthBuffer(this, m_pEffectWrap->m_pEffectTemplate->m_bUsesDepthBuffer);
    m_pManager->NotifyShaderItemUsesMultipleRenderTargets(this, !m_pEffectWrap->m_pEffectTemplate->m_SecondaryRenderTargetList.empty());

    // Create instance to store param values
    RenewShaderInstance();
}

////////////////////////////////////////////////////////////////
//
// CShaderItem::ReleaseUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CShaderItem::ReleaseUnderlyingData()
{
    m_pManager->NotifyShaderItemUsesDepthBuffer(this, false);
    m_pManager->NotifyShaderItemUsesMultipleRenderTargets(this, false);
    if (m_pEffectWrap)
        m_pManager->GetEffectCloner()->ReleaseD3DEffect(m_pEffectWrap);
    SAFE_RELEASE(m_pShaderInstance);
}

////////////////////////////////////////////////////////////////
//
// CShaderItem::SetValue
//
// Set one texture
//
////////////////////////////////////////////////////////////////
bool CShaderItem::SetValue(const SString& strName, CTextureItem* pTextureItem)
{
    if (D3DXHANDLE* phParameter = MapFind(m_pEffectWrap->m_pEffectTemplate->m_textureHandleMap, strName.ToUpper()))
    {
        // Check if value is changing
        if (!m_pShaderInstance->CmpTextureValue(*phParameter, pTextureItem))
        {
            // Check if we need a new shader instance
            MaybeRenewShaderInstance();

            if (*phParameter == m_pEffectWrap->m_pEffectTemplate->m_hFirstTexture)
            {
                // Mirror size of first texture declared in effect file
                m_uiSizeX = pTextureItem->m_uiSizeX;
                m_uiSizeY = pTextureItem->m_uiSizeY;
                m_pShaderInstance->m_uiSizeX = m_uiSizeX;
                m_pShaderInstance->m_uiSizeY = m_uiSizeY;
            }

            m_pShaderInstance->SetTextureValue(*phParameter, pTextureItem);
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
bool CShaderItem::SetValue(const SString& strName, bool bValue)
{
    if (D3DXHANDLE* phParameter = MapFind(m_pEffectWrap->m_pEffectTemplate->m_valueHandleMap, strName.ToUpper()))
    {
        // Check if value is changing
        if (!m_pShaderInstance->CmpBoolValue(*phParameter, bValue))
        {
            // Check if we need a new shader instance
            MaybeRenewShaderInstance();
            m_pShaderInstance->SetBoolValue(*phParameter, bValue);
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
bool CShaderItem::SetValue(const SString& strName, const float* pfValues, uint uiCount)
{
    if (D3DXHANDLE* phParameter = MapFind(m_pEffectWrap->m_pEffectTemplate->m_valueHandleMap, strName.ToUpper()))
    {
        // Check if value is changing
        if (!m_pShaderInstance->CmpFloatsValue(*phParameter, pfValues, uiCount))
        {
            // Check if we need a new shader instance
            MaybeRenewShaderInstance();
            m_pShaderInstance->SetFloatsValue(*phParameter, pfValues, uiCount);
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
void CShaderItem::SetTessellation(uint uiTessellationX, uint uiTessellationY)
{
    // Check if value is changing
    if (uiTessellationX != m_pShaderInstance->m_uiTessellationX || uiTessellationY != m_pShaderInstance->m_uiTessellationY)
    {
        // Check if we need a new shader instance
        MaybeRenewShaderInstance();
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
void CShaderItem::SetTransform(const SShaderTransform& transform)
{
    // Check if value is changing
    if (memcmp(&m_pShaderInstance->m_Transform, &transform, sizeof(transform)) != 0)
    {
        // Check if we need a new shader instance
        MaybeRenewShaderInstance();
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
void CShaderItem::MaybeRenewShaderInstance()
{
    if (m_pShaderInstance->m_iRefCount > 1)
        RenewShaderInstance();
}

////////////////////////////////////////////////////////////////
//
// CShaderItem::RenewShaderInstance
//
// Create/clone a new instance
//
////////////////////////////////////////////////////////////////
void CShaderItem::RenewShaderInstance()
{
    CShaderInstance* pShaderInstance = new CShaderInstance();
    pShaderInstance->PostConstruct(m_pManager, this);
}

////////////////////////////////////////////////////////////////
//
// CShaderItem::GetUsesVertexShader
//
// Check if active technique uses a vertex shader
//
////////////////////////////////////////////////////////////////
bool CShaderItem::GetUsesVertexShader()
{
    return m_pEffectWrap->m_pEffectTemplate->m_bUsesVertexShader;
}
