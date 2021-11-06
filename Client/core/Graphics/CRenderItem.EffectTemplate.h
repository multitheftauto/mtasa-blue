/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CRenderItem.EffectTemplate.h
 *  PURPOSE:     A compiled effect to clone d3d effects from
 *
 *****************************************************************************/

#include "CRenderItem.EffectParameters.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CEffectTemplate
//
// A compiled effect to clone d3d effects from
//
class CEffectTemplate : public CEffectParameters
{
    DECLARE_CLASS(CEffectTemplate, CEffectParameters)
    CEffectTemplate() : ClassInit(this) {}
    virtual void PostConstruct(CRenderItemManager* pManager, const SString& strFile, const SString& strRootPath, bool bIsRawData, SString& strOutStatus,
                               bool bDebug, const EffectMacroList& macros);
    virtual void PreDestruct();
    virtual bool IsValid();
    virtual void OnLostDevice();
    virtual void OnResetDevice();
    bool         HaveFilesChanged();
    int          GetTicksSinceLastUsed();
    CEffectWrap* CloneD3DEffect(SString& strOutStatus);
    void         UnCloneD3DEffect(CEffectWrap* pD3DEffect);
    void         CreateUnderlyingData(const SString& strFile, const SString& strRootPath, bool bIsRawData, SString& strOutStatus, bool bDebug,
                                      const EffectMacroList& macros);
    void         ReleaseUnderlyingData();
    bool         ValidateDepthBufferUsage(D3DXHANDLE hTechnique, SString& strOutErrorExtra);

    bool                       m_bHaveFilesChanged;
    SString                    m_strTechniqueName;
    std::map<SString, SString> m_FileMD5Map;
    CTickCount                 m_TickCountLastUsed;
    uint                       m_uiCloneCount;
    HRESULT                    m_CreateHResult;
};

CEffectTemplate* NewEffectTemplate(CRenderItemManager* pManager, const SString& strFile, const SString& strRootPath, bool bIsRawData, SString& strOutStatus,
                                   bool bDebug, const EffectMacroList& macros, HRESULT& outHResult);
