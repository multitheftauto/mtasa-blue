/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientRenderElementManager.h
 *  PURPOSE:
 *
 *****************************************************************************/

class CResource;
class CClientRenderElement;
class CClientDxFont;
class CClientGuiFont;
class CClientTexture;
class CClientShader;
class CClientRenderTarget;
class CClientScreenSource;
class CClientWebBrowser;
class CClientVectorGraphic;

class CClientRenderElementManager
{
public:
    CClientRenderElementManager(CClientManager* pClientManager);
    ~CClientRenderElementManager();

    CClientDxFont*  CreateDxFont(const SString& strFullFilePath, uint uiSize, bool bBold, DWORD ulQuality = DEFAULT_QUALITY);
    CClientGuiFont* CreateGuiFont(const SString& strFullFilePath, const SString& strUniqueName, uint uiSize);
    CClientTexture* CreateTexture(const SString& strFullFilePath, const CPixels* pPixels = NULL, bool bMipMaps = true, uint uiSizeX = RDEFAULT,
                                  uint uiSizeY = RDEFAULT, ERenderFormat format = RFORMAT_UNKNOWN, ETextureAddress textureAddress = TADDRESS_WRAP,
                                  ETextureType textureType = TTYPE_TEXTURE, uint uiVolumeDepth = 1);
    CClientShader* CreateShader(const SString& strFile, const SString& strRootPath, bool bIsRawData, SString& strOutStatus, float fPriority, float fMaxDistance,
                                bool bLayered, bool bDebug, int iTypeMask, const EffectMacroList& macros);
    CClientRenderTarget*  CreateRenderTarget(uint uiSizeX, uint uiSizeY, bool bHasSurfaceFormat, bool bWithAlphaChannel, _D3DFORMAT surfaceFormat);
    CClientScreenSource*  CreateScreenSource(uint uiSizeX, uint uiSizeY);
    CClientWebBrowser*    CreateWebBrowser(uint uiSizeX, uint uiSizeY, bool bIsLocal, bool bTransparent);
    CClientVectorGraphic* CreateVectorGraphic(uint width, uint height);
    CClientTexture*       FindAutoTexture(const SString& strFullFilePath, const SString& strUniqueName);
    void                  Remove(CClientRenderElement* pElement);

    void                  DoPulse();

    uint GetDxFontCount() { return m_uiStatsDxFontCount; }
    uint GetGuiFontCount() { return m_uiStatsGuiFontCount; }
    uint GetTextureCount() { return m_uiStatsTextureCount; }
    uint GetShaderCount() { return m_uiStatsShaderCount; }
    uint GetRenderTargetCount() { return m_uiStatsRenderTargetCount; }
    uint GetScreenSourceCount() { return m_uiStatsScreenSourceCount; }
    uint GetWebBrowserCount() { return m_uiStatsWebBrowserCount; }
    uint GetVectorGraphicCount() { return m_uiStatsVectorGraphicCount; }

protected:
    struct SAutoTexture
    {
        CClientTexture* texture{};
        CTickCount lastUse;
    };

    CElapsedTime                                  m_texturePulseTimer;
    CClientManager*                               m_pClientManager;
    CRenderItemManagerInterface*                  m_pRenderItemManager;
    std::map<SString, SAutoTexture>               m_AutoTextureMap;
    std::map<CRenderItem*, CClientRenderElement*> m_ItemElementMap;
    uint                                          m_uiStatsGuiFontCount;
    uint                                          m_uiStatsDxFontCount;
    uint                                          m_uiStatsTextureCount;
    uint                                          m_uiStatsShaderCount;
    uint                                          m_uiStatsRenderTargetCount;
    uint                                          m_uiStatsScreenSourceCount;
    uint                                          m_uiStatsWebBrowserCount;
    uint                                          m_uiStatsVectorGraphicCount;
};
