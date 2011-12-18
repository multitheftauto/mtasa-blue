/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientDxFont.cpp
*  PURPOSE:     Custom font bucket
*  DEVELOPERS:  qwerty
*
*****************************************************************************/

#include <StdInc.h>


////////////////////////////////////////////////////////////////
//
// CClientDxFont::CClientDxFont
//
//
//
////////////////////////////////////////////////////////////////
CClientDxFont::CClientDxFont ( CClientManager* pManager, ElementID ID, CDxFontItem* pFontItem ) : ClassInit ( this ), CClientRenderElement ( pManager, ID )
{
    SetTypeName ( "dx-font" );
    m_pRenderItem = pFontItem;
}


////////////////////////////////////////////////////////////////
//
// CClientDxFont::GetD3DXFont
//
// Get D3DXFont for this custom font
//
////////////////////////////////////////////////////////////////
ID3DXFont* CClientDxFont::GetD3DXFont ( void )
{
    return GetDxFontItem ()->m_pFntNormal;
}
