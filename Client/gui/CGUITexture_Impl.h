/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUITexture_Impl.h
 *  PURPOSE:     Texture handling class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUITexture.h>

namespace CEGUI
{
    class Texture;
    class Renderer;
}            // namespace CEGUI

class CGUITexture_Impl : public CGUITexture
{
public:
    CGUITexture_Impl(class CGUI_Impl* pGUI);
    ~CGUITexture_Impl();

    bool LoadFromFile(const char* szFilename);
    void LoadFromMemory(const void* pBuffer, std::uint32_t uiWidth, std::uint32_t uiHeight);
    void Clear();

    CEGUI::Texture*    GetTexture();
    void               SetTexture(CEGUI::Texture* pTexture);
    LPDIRECT3DTEXTURE9 GetD3DTexture();

    void CreateTexture(std::uint32_t width, std::uint32_t height);

private:
    CEGUI::Renderer* m_pRenderer;
    CEGUI::Texture*  m_pTexture;
};
