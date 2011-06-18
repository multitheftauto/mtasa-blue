/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRenderElement.h
*  PURPOSE:     Client module's version of something renderable
*  DEVELOPERS:  idiot
*
*****************************************************************************/


class CClientRenderElement : public CClientEntity
{
    DECLARE_CLASS( CClientRenderElement, CClientEntity )
public:
                            CClientRenderElement         ( CClientManager* pManager, ElementID ID );
                            ~CClientRenderElement        ( void );

    void                    Unlink                  ( void );
    void                    GetPosition             ( CVector& vecPosition ) const      { vecPosition = CVector (); }
    void                    SetPosition             ( const CVector& vecPosition )      {}

    // CClientRenderElement methods
    CRenderItem*            GetRenderItem           ( void )                            { return m_pRenderItem; }

protected:
    bool                            bDoneUnlink;
    CRenderItem*                    m_pRenderItem;
    CClientRenderElementManager*    m_pRenderElementManager;
};
