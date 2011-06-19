/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientDxFont.h
*  PURPOSE:     Custom font bucket
*  DEVELOPERS:  qwerty
*
*****************************************************************************/


class CClientDxFont : public CClientRenderElement
{
    DECLARE_CLASS( CClientDxFont, CClientRenderElement )
public:
                            CClientDxFont           ( CClientManager* pManager, ElementID ID, CDxFontItem* pFontItem );

    eClientEntityType       GetType                 ( void ) const                      { return CCLIENTDXFONT; }

    // CClientDxFont methods
    CDxFontItem*            GetDxFontItem           ( void )                            { return (CDxFontItem*)m_pRenderItem; }
    ID3DXFont*              GetD3DXFont             ( float& fScaleX, float& fScaleY );
};
