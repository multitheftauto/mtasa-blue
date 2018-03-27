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
    void         DoPulse(void);
    CEffectWrap* CreateD3DEffect(const SString& strFilename, const SString& strRootPath, SString& strOutStatus, bool bDebug);
    void         ReleaseD3DEffect(CEffectWrap* pD3DEffect);
    void         MaybeTidyUp(bool bForceDrasticMeasures = false);

    CElapsedTime                        m_TidyupTimer;
    CRenderItemManager*                 m_pManager;
    std::map<SString, CEffectTemplate*> m_ValidMap;            // Active and files not changed since first created
    std::vector<CEffectTemplate*>       m_OldList;             // Active but files changed since first created
};
