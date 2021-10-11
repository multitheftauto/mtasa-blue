/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CGUIStaticImage_Impl::CGUIStaticImage_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, CVector2D pos, CVector2D size, bool relative)
    : CGUIElement_Impl(pGUI, pParent, pos, size, relative)
{
    m_type = CGUIType::STATICIMAGE;

    m_texture = nullptr;
}

void CGUIStaticImage_Impl::Begin()
{
    CGUIElement_Impl::Begin();

    if (m_texture)
    {
        IDirect3DBaseTexture9* d3dTexture = m_texture->GetD3DTexture();

        if (d3dTexture)
            ImGui::Image((void*)d3dTexture, ImVec2(m_size.fX, m_size.fY), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, GetAlpha() / 255), ImVec4(0, 0, 0, 0));
    }
}

void CGUIStaticImage_Impl::End()
{
    CGUIElement_Impl::End();
}

void CGUIStaticImage_Impl::LoadFromFile(std::string path)
{
    CGUITexture* texture = m_pManager->CreateTexture(m_size);

    if (!texture)
        return;

    texture->LoadFromFile(path);
    m_texture = texture;
}

void CGUIStaticImage_Impl::LoadFromTexture(CGUITexture* texture)
{
    m_texture = texture;
}
