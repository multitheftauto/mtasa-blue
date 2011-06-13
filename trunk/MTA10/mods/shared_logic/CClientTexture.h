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
                            CClientTexture         ( CClientManager* pManager, ElementID ID, STextureItem* pTextureItem );

    eClientEntityType       GetType                 ( void ) const                      { return CCLIENTTEXTURE; }

    // CClientTexture methods
    STextureItem*           GetTextureItem          ( void )                            { return (STextureItem*)m_pRenderItem; }

};


class CClientRenderTarget : public CClientTexture
{
    DECLARE_CLASS( CClientRenderTarget, CClientTexture )
public:
                            CClientRenderTarget         ( CClientManager* pManager, ElementID ID, SRenderTargetItem* pRenderTargetItem ) : ClassInit ( this ), CClientTexture ( pManager, ID, pRenderTargetItem ) {}

    // CClientRenderTarget methods
    SRenderTargetItem*      GetRenderTargetItem          ( void )                       { return (SRenderTargetItem*)m_pRenderItem; }
};


class CClientScreenSource : public CClientTexture
{
    DECLARE_CLASS( CClientScreenSource, CClientTexture )
public:
                            CClientScreenSource         ( CClientManager* pManager, ElementID ID, SScreenSourceItem* pScreenSourceItem ) : ClassInit ( this ), CClientTexture ( pManager, ID, pScreenSourceItem ) {}

    // CClientScreenSource methods
    SScreenSourceItem*      GetScreenSourceItem          ( void )                       { return (SScreenSourceItem*)m_pRenderItem; }
};
