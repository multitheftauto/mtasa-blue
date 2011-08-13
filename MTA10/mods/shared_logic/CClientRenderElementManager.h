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


class CClientRenderElementManager
{
public:
                            CClientRenderElementManager          ( CClientManager* pClientManager );
                            ~CClientRenderElementManager         ( void );

    CClientDxFont*          CreateDxFont                ( const SString& strFullFilePath, uint uiSize, bool bBold );
    CClientGuiFont*         CreateGuiFont               ( const SString& strFullFilePath, const SString& strUniqueName, uint uiSize );
    CClientTexture*         CreateTexture               ( const SString& strFullFilePath );
    CClientShader*          CreateShader                ( const SString& strFullFilePath, const SString& strRootPath, SString& strOutStatus, bool bDebug );
    CClientRenderTarget*    CreateRenderTarget          ( uint uiSizeX, uint uiSizeY, bool bWithAlphaChannel );
    CClientScreenSource*    CreateScreenSource          ( uint uiSizeX, uint uiSizeY );
    CClientTexture*         FindAutoTexture             ( const SString& strFullFilePath, const SString& strUniqueName );
    void                    Remove                      ( CClientRenderElement* pElement );

    uint                    GetDxFontCount              ( void )    { return m_uiStatsDxFontCount; }
    uint                    GetGuiFontCount             ( void )    { return m_uiStatsGuiFontCount; }
    uint                    GetTextureCount             ( void )    { return m_uiStatsTextureCount; }
    uint                    GetShaderCount              ( void )    { return m_uiStatsShaderCount; }
    uint                    GetRenderTargetCount        ( void )    { return m_uiStatsRenderTargetCount; }
    uint                    GetScreenSourceCount        ( void )    { return m_uiStatsScreenSourceCount; }
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
};
