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
