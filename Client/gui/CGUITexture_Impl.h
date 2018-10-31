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
    ~CGUITexture_Impl(void);

    bool LoadFromFile(const char* szFilename);
    void LoadFromMemory(const void* pBuffer, unsigned int uiWidth, unsigned int uiHeight);
    void Clear(void);

    CEGUI::Texture*    GetTexture(void);
    void               SetTexture(CEGUI::Texture* pTexture);
    LPDIRECT3DTEXTURE9 GetD3DTexture(void);

    void CreateTexture(unsigned int width, unsigned int height);

private:
    CEGUI::Renderer* m_pRenderer;
    CEGUI::Texture*  m_pTexture;
};
