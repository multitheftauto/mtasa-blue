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

CGUIStaticImage_Impl::CGUIStaticImage_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, CVector2D pos, CVector2D size, bool relative)
    : CGUIElement_Impl(pGUI, pParent, pos, size, relative)
{
    m_type = CGUIType::STATICIMAGE;

    m_textureItem = nullptr;
}

void CGUIStaticImage_Impl::Begin()
{
    CGUIElement_Impl::Begin();

    if (m_textureItem != nullptr)
        ImGui::Image((void*)m_textureItem->m_pD3DTexture, ImVec2(m_size.fX, m_size.fY));
}

void CGUIStaticImage_Impl::End()
{
    CGUIElement_Impl::End();
}

bool CGUIStaticImage_Impl::LoadFromFile(std::string path)
{
    CGraphicsInterface* graphics = m_pManager->GetGraphicsInterface();

    if (!graphics)
        return false;

    CTextureItem* textureItem = graphics->GetRenderItemManager()->CreateTexture(path, NULL, false, m_size.fX, m_size.fY);

    if (!textureItem)
        return false;

    m_textureItem = textureItem;
    return true;
}
