/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CRenderItem.EffectCloner.h
 *  PURPOSE:
 *
 *****************************************************************************/

struct SEffectInvariant
{
    SString      strFile;
    EffectMacroList macros;

    bool operator ==(const SEffectInvariant& e) const
    {
        return strFile == e.strFile && macros == e.macros;
    }
};

template<>
struct std::hash<SEffectInvariant>
{
    size_t operator()(const SEffectInvariant& entry) const
    {
        using EffectHash = std::size_t;

        EffectHash uiHash = std::hash<SString>{}(entry.strFile);

        for (const auto& [name, definition] : entry.macros)
        {
            // Hash function from boost
            uiHash ^= std::hash<SString>{}(name)       + 0x9e3779b9 + (uiHash << 6) + (uiHash >> 2);
            uiHash ^= std::hash<SString>{}(definition) + 0x9e3779b9 + (uiHash << 6) + (uiHash >> 2);
        }

        return uiHash;
    }
};

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
    CEffectWrap* CreateD3DEffect(const SString& strFile, const SString& strRootPath, bool bIsRawData, SString& strOutStatus, bool bDebug, const EffectMacroList& macros);
    void         ReleaseD3DEffect(CEffectWrap* pD3DEffect);
private:
    void         MaybeTidyUp(bool bForceDrasticMeasures = false);

    CElapsedTime                                            m_TidyupTimer;
    CRenderItemManager*                                     m_pManager;
    std::unordered_map<SEffectInvariant, CEffectTemplate*>  m_ValidMap;      // Active and files not changed since first created
    std::vector<CEffectTemplate*>                           m_OldList;       // Active but files changed since first created
};
