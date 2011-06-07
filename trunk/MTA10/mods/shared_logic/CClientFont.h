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


class CClientFont : public CClientEntity
{
    friend CClientFontManager;

public:
                            CClientFont             ( CClientManager* pManager, ElementID ID, const SString& strGUIFontName, ID3DXFont* pDXFontNormal, ID3DXFont* pDXFontBig );
                            ~CClientFont            ( void );

    eClientEntityType       GetType                 ( void ) const                      { return CCLIENTFONT; }

    void                    Unlink                  ( void );
    void                    GetPosition             ( CVector& vecPosition ) const      { vecPosition = CVector (); }
    void                    SetPosition             ( const CVector& vecPosition )      {}

    // CClientFont methods
    const SString&          GetGUIFontName          ( void );
    ID3DXFont*              GetDXFont               ( float fScaleX, float fScaleY );

    void                    NotifyGUIElementAttach  ( CClientGUIElement* pGUIElement );
    void                    NotifyGUIElementDetach  ( CClientGUIElement* pGUIElement );

protected:
    CClientManager*         m_pManager;
    CClientFontManager*     m_pFontManager;
    SString                 m_strGUIFontName;
    ID3DXFont*              m_pDXFontNormal;
    ID3DXFont*              m_pDXFontBig;
    std::set < CClientGUIElement* > m_GUIElementUserList;
};
