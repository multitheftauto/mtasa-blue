/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CRenderItem.ShaderInstance.cpp
 *  PURPOSE:
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CRenderItem.EffectTemplate.h"

////////////////////////////////////////////////////////////////
//
// CShaderInstance::PostConstruct
//
//
//
////////////////////////////////////////////////////////////////
void CShaderInstance::PostConstruct(CRenderItemManager* pManager, CShaderItem* pShaderItem)
{
    Super::PostConstruct(pManager);
    m_uiTessellationX = 1;
    m_uiTessellationY = 1;
    // Initial creation of d3d data
    CreateUnderlyingData(pShaderItem);
}

////////////////////////////////////////////////////////////////
//
// CShaderInstance::PreDestruct
//
//
//
////////////////////////////////////////////////////////////////
void CShaderInstance::PreDestruct()
{
    ReleaseUnderlyingData();
    Super::PreDestruct();
}

////////////////////////////////////////////////////////////////
//
// CShaderInstance::IsValid
//
// Check underlying data is present
//
////////////////////////////////////////////////////////////////
bool CShaderInstance::IsValid()
{
    return m_pEffectWrap && m_pEffectWrap->m_pD3DEffect;
}

////////////////////////////////////////////////////////////////
//
// CShaderInstance::OnLostDevice
//
// Release device stuff
//
////////////////////////////////////////////////////////////////
void CShaderInstance::OnLostDevice()
{
    // Nothing required for CShaderInstance
}

////////////////////////////////////////////////////////////////
//
// CShaderInstance::OnResetDevice
//
// Recreate device stuff
//
////////////////////////////////////////////////////////////////
void CShaderInstance::OnResetDevice()
{
    // Nothing required for CShaderInstance
}

////////////////////////////////////////////////////////////////
//
// CShaderInstance::CreateUnderlyingData
//
// Create/clone shader instance
//
////////////////////////////////////////////////////////////////
void CShaderInstance::CreateUnderlyingData(CShaderItem* pShaderItem)
{
    // Clone data from shader
    m_uiSizeX = pShaderItem->m_uiSizeX;
    m_uiSizeY = pShaderItem->m_uiSizeY;
    m_pEffectWrap = pShaderItem->m_pEffectWrap;
    m_pEffectWrap->AddRef();

    // If shader had a previous instance, clone parameter values then release it
    if (pShaderItem->m_pShaderInstance)
    {
        // Clone values from previous instance
        m_currentSetValues = pShaderItem->m_pShaderInstance->m_currentSetValues;
        m_uiTessellationX = pShaderItem->m_pShaderInstance->m_uiTessellationX;
        m_uiTessellationY = pShaderItem->m_pShaderInstance->m_uiTessellationY;
        m_Transform = pShaderItem->m_pShaderInstance->m_Transform;
        m_bHasModifiedTransform = pShaderItem->m_pShaderInstance->m_bHasModifiedTransform;
        m_uiModifiedParametersRevision = pShaderItem->m_pShaderInstance->m_uiModifiedParametersRevision;
        m_requiredDefaultValuesList = pShaderItem->m_pShaderInstance->m_requiredDefaultValuesList;

        // Increment refs on cloned texture values
        for (std::map<D3DXHANDLE, SShaderValue>::iterator iter = m_currentSetValues.begin(); iter != m_currentSetValues.end(); ++iter)
            if (iter->second.cType == 't')
                iter->second.pTextureItem->AddRef();

        // Detach old instance
        SAFE_RELEASE(pShaderItem->m_pShaderInstance)
    }

    // Set the shader instance to the new one
    pShaderItem->m_pShaderInstance = this;
}

////////////////////////////////////////////////////////////////
//
// CShaderInstance::ReleaseUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CShaderInstance::ReleaseUnderlyingData()
{
    SAFE_RELEASE(m_pEffectWrap);

    // Decrement refs on all textures
    for (std::map<D3DXHANDLE, SShaderValue>::iterator iter = m_currentSetValues.begin(); iter != m_currentSetValues.end(); ++iter)
        if (iter->second.cType == 't')
            SAFE_RELEASE(iter->second.pTextureItem)
}

////////////////////////////////////////////////////////////////
//
// CShaderInstance::SetTextureValue
//
// Set a shader parameter value
//
////////////////////////////////////////////////////////////////
void CShaderInstance::SetTextureValue(D3DXHANDLE hHandle, CTextureItem* pTextureItem)
{
    SShaderValue* pParam = GetParam(hHandle);

    // Release any previous texture
    if (pParam->cType == 't')
        SAFE_RELEASE(pParam->pTextureItem)

    // Set as texture
    pParam->cType = 't';
    pParam->pTextureItem = pTextureItem;
    pParam->pTextureItem->AddRef();
}

////////////////////////////////////////////////////////////////
//
// CShaderInstance::SetBoolValue
//
// Set a shader parameter value
//
////////////////////////////////////////////////////////////////
void CShaderInstance::SetBoolValue(D3DXHANDLE hHandle, bool bValue)
{
    SShaderValue* pParam = GetParam(hHandle);
    // Set as bool
    pParam->cType = 'b';
    pParam->bValue = bValue;
}

////////////////////////////////////////////////////////////////
//
// CShaderInstance::SetFloatsValue
//
// Set a shader parameter value
//
////////////////////////////////////////////////////////////////
void CShaderInstance::SetFloatsValue(D3DXHANDLE hHandle, const float* pfValues, uint uiCount)
{
    SShaderValue* pParam = GetParam(hHandle);
    // Set as float
    uiCount = std::min(uiCount, NUMELMS(pParam->floatList));
    pParam->cType = 'f';
    memcpy(pParam->floatList, pfValues, sizeof(float) * uiCount);
    pParam->cCount = uiCount;
}

////////////////////////////////////////////////////////////////
//
// CShaderInstance::CmpTextureValue
//
//
//
////////////////////////////////////////////////////////////////
bool CShaderInstance::CmpTextureValue(D3DXHANDLE hHandle, CTextureItem* pTextureItem)
{
    SShaderValue* pParam = GetParam(hHandle);
    return pParam->cType == 't' && pParam->pTextureItem == pTextureItem;
}

////////////////////////////////////////////////////////////////
//
// CShaderInstance::CmpBoolValue
//
//
//
////////////////////////////////////////////////////////////////
bool CShaderInstance::CmpBoolValue(D3DXHANDLE hHandle, bool bValue)
{
    SShaderValue* pParam = GetParam(hHandle);
    return pParam->cType == 'b' && pParam->bValue == bValue;
}

////////////////////////////////////////////////////////////////
//
// CShaderInstance::CmpFloatsValue
//
//
//
////////////////////////////////////////////////////////////////
bool CShaderInstance::CmpFloatsValue(D3DXHANDLE hHandle, const float* pfValues, uint uiCount)
{
    SShaderValue* pParam = GetParam(hHandle);
    return pParam->cType == 'f' && pParam->cCount == uiCount && memcmp(pParam->floatList, pfValues, sizeof(float) * uiCount) == 0;
}

////////////////////////////////////////////////////////////////
//
// CShaderInstance::GetParam
//
//
//
////////////////////////////////////////////////////////////////
SShaderValue* CShaderInstance::GetParam(D3DXHANDLE hHandle)
{
    // Find item
    SShaderValue* pParam = MapFind(m_currentSetValues, hHandle);
    if (!pParam)
    {
        m_pEffectWrap->m_pEffectTemplate->NotifyModifiedParameter(hHandle);
        MapSet(m_currentSetValues, hHandle, SShaderValue());
        pParam = MapFind(m_currentSetValues, hHandle);
        pParam->cType = 0;
    }
    return pParam;
}

////////////////////////////////////////////////////////////////
//
// CShaderInstance::ApplyShaderParameters
//
//
//
////////////////////////////////////////////////////////////////
void CShaderInstance::ApplyShaderParameters()
{
    // Update list of values that will require the default setting
    if (m_pEffectWrap->m_pEffectTemplate->m_uiModifiedParametersRevision != m_uiModifiedParametersRevision)
    {
        m_uiModifiedParametersRevision = m_pEffectWrap->m_pEffectTemplate->m_uiModifiedParametersRevision;
        m_requiredDefaultValuesList.clear();
        const std::set<D3DXHANDLE>& modifiedParametersList = m_pEffectWrap->m_pEffectTemplate->GetModifiedParameters();
        if (m_currentSetValues.size() < modifiedParametersList.size())
        {
            for (auto hParameter : modifiedParametersList)
            {
                if (!MapContains(m_currentSetValues, hParameter))
                {
                    // If another CShaderInstance is using this value, but this CShaderInstance is not, then will need to set default here
                    m_requiredDefaultValuesList.push_back(hParameter);
                }
            }
        }
    }

    // Apply default values
    m_pEffectWrap->m_pEffectTemplate->RestoreParametersDefaultValue(m_requiredDefaultValuesList);

    // Apply custom values
    ID3DXEffect* pD3DEffect = m_pEffectWrap->m_pD3DEffect;
    for (std::map<D3DXHANDLE, SShaderValue>::iterator iter = m_currentSetValues.begin(); iter != m_currentSetValues.end(); ++iter)
    {
        switch (iter->second.cType)
        {
            case 't':
                pD3DEffect->SetTexture(iter->first, iter->second.pTextureItem->m_pD3DTexture);
                break;

            case 'b':
                pD3DEffect->SetBool(iter->first, iter->second.bValue);
                break;

            case 'f':
                pD3DEffect->SetFloatArray(iter->first, iter->second.floatList, iter->second.cCount);
                break;
        }
    }
}
