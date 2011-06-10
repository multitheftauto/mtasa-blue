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
class CClientFont;
class CClientTexture;

class CClientRenderElementManager
{
public:
                            CClientRenderElementManager          ( CClientManager* pClientManager );
                            ~CClientRenderElementManager         ( void );

    CClientFont*            CreateFont                  ( const SString& strFullFilePath, const SString& strUniqueName, uint uiSize, bool bBold );
    CClientTexture*         CreateTexture               ( const SString& strFullFilePath );
    CClientTexture*         FindAutoTexture             ( const SString& strFullFilePath, const SString& strUniqueName );
    void                    Remove                      ( CClientRenderElement* pElement );

    uint                    GetFontCount                ( void )    { return m_uiStatsFontCount; }
    uint                    GetTextureCount             ( void )    { return m_uiStatsTextureCount; }
protected:
    CClientManager*                                     m_pClientManager;
    CRenderItemManagerInterface*                        m_pRenderItemManager;
    std::map < SString, CClientTexture* >               m_AutoTextureMap;
    std::map < SRenderItem*, CClientRenderElement* >    m_ItemElementMap;
    uint                                                m_uiStatsFontCount;
    uint                                                m_uiStatsTextureCount;
};
