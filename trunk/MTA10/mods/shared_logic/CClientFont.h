/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientFont.h
*  PURPOSE:     Custom font bucket
*  DEVELOPERS:  qwerty
*
*****************************************************************************/


class CClientFont : public CClientRenderElement
{
    DECLARE_CLASS( CClientFont, CClientRenderElement )
public:
                            CClientFont             ( CClientManager* pManager, ElementID ID, SFontItem* pFontItem );
                            ~CClientFont            ( void );

    eClientEntityType       GetType                 ( void ) const                      { return CCLIENTFONT; }
    void                    Unlink                  ( void );

    // CClientFont methods
    SFontItem*              GetFontItem             ( void )                            { return (SFontItem*)m_pRenderItem; }
    const SString&          GetGUIFontName          ( void );
    ID3DXFont*              GetDXFont               ( float fScaleX, float fScaleY );
    void                    NotifyGUIElementAttach  ( CClientGUIElement* pGUIElement );
    void                    NotifyGUIElementDetach  ( CClientGUIElement* pGUIElement );

protected:
    std::set < CClientGUIElement* > m_GUIElementUserList;
};
