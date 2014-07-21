/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTexture.h
*  PURPOSE:
*  DEVELOPERS:  idiot
*
*****************************************************************************/


class CClientTexture : public CClientMaterial
{
    DECLARE_CLASS( CClientTexture, CClientMaterial )
public:
                            CClientTexture         ( CClientManager* pManager, ElementID ID, CTextureItem* pTextureItem );

    eClientEntityType       GetType                 ( void ) const                      { return CCLIENTTEXTURE; }

    // CClientTexture methods
    CTextureItem*           GetTextureItem          ( void )                            { return (CTextureItem*)m_pRenderItem; }

};


class CClientRenderTarget : public CClientTexture
{
    DECLARE_CLASS( CClientRenderTarget, CClientTexture )
public:
                            CClientRenderTarget         ( CClientManager* pManager, ElementID ID, CRenderTargetItem* pRenderTargetItem ) : ClassInit ( this ), CClientTexture ( pManager, ID, pRenderTargetItem ) {}

    eClientEntityType       GetType                     ( void ) const                      { return CCLIENTRENDERTARGET; }
                            
    // CClientRenderTarget methods
    CRenderTargetItem*      GetRenderTargetItem          ( void )                       { return (CRenderTargetItem*)m_pRenderItem; }
};


class CClientScreenSource : public CClientTexture
{
    DECLARE_CLASS( CClientScreenSource, CClientTexture )
public:
                            CClientScreenSource         ( CClientManager* pManager, ElementID ID, CScreenSourceItem* pScreenSourceItem ) : ClassInit ( this ), CClientTexture ( pManager, ID, pScreenSourceItem ) {}

    eClientEntityType       GetType                     ( void ) const                      { return CCLIENTSCREENSOURCE; }
                            
    // CClientScreenSource methods
    CScreenSourceItem*      GetScreenSourceItem          ( void )                       { return (CScreenSourceItem*)m_pRenderItem; }
};
