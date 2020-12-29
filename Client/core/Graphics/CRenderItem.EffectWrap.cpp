/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CRenderItem.EffectMan.cpp
 *  PURPOSE:
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CRenderItem.EffectTemplate.h"

////////////////////////////////////////////////////////////////
//
// CEffectWrap::PostConstruct
//
//
//
////////////////////////////////////////////////////////////////
void CEffectWrap::PostConstruct(CRenderItemManager* pManager, CEffectTemplate* pEffectTemplate)
{
    Super::PostConstruct(pManager);
    m_pEffectTemplate = pEffectTemplate;
    m_pEffectTemplate->AddRef();
    m_pD3DEffect = pEffectTemplate->m_pD3DEffect;
}

////////////////////////////////////////////////////////////////
//
// CEffectWrap::PreDestruct
//
//
//
////////////////////////////////////////////////////////////////
void CEffectWrap::PreDestruct()
{
    SAFE_RELEASE(m_pEffectTemplate);
    Super::PreDestruct();
}

////////////////////////////////////////////////////////////////
//
// CEffectWrap::IsValid
//
// Check underlying data is present
//
////////////////////////////////////////////////////////////////
bool CEffectWrap::IsValid()
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
void CEffectWrap::OnLostDevice()
{
    // Handled in CEffectTemplate
}

////////////////////////////////////////////////////////////////
//
// CEffectWrap::OnResetDevice
//
// Recreate device stuff
//
////////////////////////////////////////////////////////////////
void CEffectWrap::OnResetDevice()
{
    // Handled in CEffectTemplate
}

////////////////////////////////////////////////////////////////
//
// CEffectWrap::Begin
//
// Wrap for ID3DEffect::Begin
// Ensures secondary render targets are set if required
//
////////////////////////////////////////////////////////////////
HRESULT CEffectWrap::Begin(UINT* pPasses, DWORD Flags, bool bWorldRender)
{
    return m_pEffectTemplate->Begin(pPasses, Flags, bWorldRender);
}

////////////////////////////////////////////////////////////////
//
// CEffectWrap::End
//
// Wrap for ID3DEffect::End
// Ensures secondary render targets are unset
//
////////////////////////////////////////////////////////////////
HRESULT CEffectWrap::End()
{
    return m_pEffectTemplate->End();
}

////////////////////////////////////////////////////////////////
//
// CEffectWrap::ApplyCommonHandles
//
// Set parameters for MTA known names
// Called before render
//
////////////////////////////////////////////////////////////////
bool CEffectWrap::ApplyCommonHandles()
{
    return m_pEffectTemplate->ApplyCommonHandles();
}

////////////////////////////////////////////////////////////////
//
// CEffectWrap::ApplyMappedHandles
//
// Set parameters which are tied to D3D registers
// Called before render
//
////////////////////////////////////////////////////////////////
bool CEffectWrap::ApplyMappedHandles()
{
    return m_pEffectTemplate->ApplyMappedHandles();
}
