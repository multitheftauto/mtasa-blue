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

namespace GUINew{
    class CGUITexture_Impl;
}

class GUINew::CGUITexture_Impl : public CGUITexture
{
public:
    CGUITexture_Impl(class CGUI_Impl* pGUI);
    ~CGUITexture_Impl();

    bool LoadFromFile(const char* szFilename, const char* szResourceGroup = "mta");
    void LoadFromMemory(const void* pBuffer, unsigned int uiWidth, unsigned int uiHeight);
    void Clear();

    CEGUI::Texture*    GetTexture();
    void               SetTexture(CEGUI::Texture* pTexture);
    LPDIRECT3DTEXTURE9 GetD3DTexture();

    void CreateTexture(unsigned int width, unsigned int height);

private:
    class CGUI_Impl*           m_pGUI;
    CEGUI::Direct3D9Renderer*  m_pRenderer;
    CEGUI::Texture*            m_pTexture;
};
