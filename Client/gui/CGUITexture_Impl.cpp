/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#include <core/CRenderItemManagerInterface.h>
#include <core/CGraphicsInterface.h>

CGUITexture_Impl::CGUITexture_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, CVector2D pos, CVector2D size, bool relative)
    : CGUIElement_Impl(pGUI, pParent, pos, size, relative)
{
    m_type = CGUIType::TEXTURE;

    m_textureItem = nullptr;
}

void CGUITexture_Impl::Begin()
{
    CGUIElement_Impl::Begin();
}

void CGUITexture_Impl::End()
{
    CGUIElement_Impl::End();
}

void CGUITexture_Impl::LoadFromFile(std::string path)
{
    CGraphicsInterface* graphics = m_pManager->GetGraphicsInterface();

    if (!graphics)
        return;

    CTextureItem* textureItem = graphics->GetRenderItemManager()->CreateTexture(path, NULL, false, m_size.fX, m_size.fY);

    if (!textureItem)
        return;

    textureItem->m_TextureAddress = TADDRESS_MIRROR;
    m_textureItem = textureItem;
}

IDirect3DBaseTexture9* CGUITexture_Impl::GetD3DTexture()
{
    if (m_textureItem)
        return m_textureItem->m_pD3DTexture;

    return nullptr;
}

CVector2D CGUITexture_Impl::GetNativeSize()
{
    if (!m_textureItem)
        return {};

    return CVector2D(m_textureItem->m_uiSurfaceSizeX, m_textureItem->m_uiSurfaceSizeY);
}

