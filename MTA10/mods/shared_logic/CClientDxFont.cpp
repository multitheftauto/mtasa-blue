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
// May change the scale
//
////////////////////////////////////////////////////////////////
ID3DXFont* CClientDxFont::GetD3DXFont ( float& fScaleX, float& fScaleY )
{
    if ( fScaleX > 1.1f || fScaleY > 1.1f )
    {
        // Adjust scale to compensate for higher res font
        fScaleX *= 0.25f;
        if ( &fScaleX != &fScaleY )     // Check fScaleY is not the same variable
            fScaleY *= 0.25f;
        return GetDxFontItem ()->m_pFntBig;
    }
    return GetDxFontItem ()->m_pFntNormal;
}
