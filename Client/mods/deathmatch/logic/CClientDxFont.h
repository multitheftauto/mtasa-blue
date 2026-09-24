/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientDxFont.h
 *  PURPOSE:     Custom font bucket
 *
 *****************************************************************************/

#include "enums/ElementType.h"

class CClientDxFont : public CClientRenderElement
{
    DECLARE_CLASS(CClientDxFont, CClientRenderElement)
public:
    CClientDxFont(CClientManager* pManager, ElementID ID, CDxFontItem* pFontItem);

    ElementType::Enum GetType() const { return ElementType::DXFONT; }

    // CClientDxFont methods
    CDxFontItem* GetDxFontItem() { return (CDxFontItem*)m_pRenderItem; }
    ID3DXFont*   GetD3DXFont();
};
