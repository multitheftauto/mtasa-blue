/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientShader.h
*  PURPOSE:
*  DEVELOPERS:  vidiot
*
*****************************************************************************/


class CClientShader : public CClientMaterial
{
    DECLARE_CLASS( CClientShader, CClientMaterial )
public:
                            CClientShader           ( CClientManager* pManager, ElementID ID, CShaderItem* pShaderItem );

    eClientEntityType       GetType                 ( void ) const                      { return CCLIENTSHADER; }

    // CClientShader methods
    CShaderItem*            GetShaderItem           ( void )                            { return (CShaderItem*)m_pRenderItem; }

};
