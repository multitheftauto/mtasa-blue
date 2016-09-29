/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRenderElementManager.h
*  PURPOSE:
*  DEVELOPERS:  idiot
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


class CClientRenderElementManager
{
public:
                            CClientRenderElementManager          ( CClientManager* pClientManager );
                            ~CClientRenderElementManager         ( void );

    CClientDxFont*          CreateDxFont                ( const SString& strFullFilePath, uint uiSize, bool bBold, DWORD ulQuality = DEFAULT_QUALITY );
    CClientGuiFont*         CreateGuiFont               ( const SString& strFullFilePath, const SString& strUniqueName, uint uiSize );
    CClientTexture*         CreateTexture               ( const SString& strFullFilePath, const CPixels* pPixels = NULL, bool bMipMaps = true, uint uiSizeX = RDEFAULT, uint uiSizeY = RDEFAULT, ERenderFormat format = RFORMAT_UNKNOWN, ETextureAddress textureAddress = TADDRESS_WRAP, ETextureType textureType = TTYPE_TEXTURE, uint uiVolumeDepth = 1 );
    CClientShader*          CreateShader                ( const SString& strFullFilePath, const SString& strRootPath, SString& strOutStatus, float fPriority, float fMaxDistance, bool bLayered, bool bDebug, int iTypeMask );
    CClientRenderTarget*    CreateRenderTarget          ( uint uiSizeX, uint uiSizeY, bool bWithAlphaChannel );
    CClientScreenSource*    CreateScreenSource          ( uint uiSizeX, uint uiSizeY );
    CClientWebBrowser*      CreateWebBrowser            ( uint uiSizeX, uint uiSizeY, bool bIsLocal, bool bTransparent );
    CClientTexture*         FindAutoTexture             ( const SString& strFullFilePath, const SString& strUniqueName );
    void                    Remove                      ( CClientRenderElement* pElement );

    uint                    GetDxFontCount              ( void )    { return m_uiStatsDxFontCount; }
    uint                    GetGuiFontCount             ( void )    { return m_uiStatsGuiFontCount; }
    uint                    GetTextureCount             ( void )    { return m_uiStatsTextureCount; }
    uint                    GetShaderCount              ( void )    { return m_uiStatsShaderCount; }
    uint                    GetRenderTargetCount        ( void )    { return m_uiStatsRenderTargetCount; }
    uint                    GetScreenSourceCount        ( void )    { return m_uiStatsScreenSourceCount; }
    uint                    GetWebBrowserCount          ( void )    { return m_uiStatsWebBrowserCount; }
protected:
    CClientManager*                                     m_pClientManager;
    CRenderItemManagerInterface*                        m_pRenderItemManager;
    std::map < SString, CClientTexture* >               m_AutoTextureMap;
    std::map < CRenderItem*, CClientRenderElement* >    m_ItemElementMap;
    uint                                                m_uiStatsGuiFontCount;
    uint                                                m_uiStatsDxFontCount;
    uint                                                m_uiStatsTextureCount;
    uint                                                m_uiStatsShaderCount;
    uint                                                m_uiStatsRenderTargetCount;
    uint                                                m_uiStatsScreenSourceCount;
    uint                                                m_uiStatsWebBrowserCount;
};
