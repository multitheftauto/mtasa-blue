/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CRenderItem.EffectCloner.h
 *  PURPOSE:
 *
 *****************************************************************************/

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CEffectCloner
//
// Maintain a list of active CEffectTemplate objects and clone d3d effects from them when needed
//
class CEffectCloner
{
public:
    CEffectCloner(CRenderItemManager* pManager);
    void         DoPulse();
    CEffectWrap* CreateD3DEffect(const SString& strFile, const SString& strRootPath, bool bIsRawData, SString& strOutStatus,
        bool bDebug, const std::vector<std::pair<SString, SString>>& macros);
    void         ReleaseD3DEffect(CEffectWrap* pD3DEffect);
    void         MaybeTidyUp(bool bForceDrasticMeasures = false);
    std::size_t  CalculateInvariantHash(const SString& strFile, const std::vector<std::pair<SString, SString>>& macros);

    CElapsedTime                            m_TidyupTimer;
    CRenderItemManager*                     m_pManager;
    std::map<std::size_t, CEffectTemplate*> m_ValidMap;            // Active and files not changed since first created
    std::vector<CEffectTemplate*>           m_OldList;             // Active but files changed since first created
};
