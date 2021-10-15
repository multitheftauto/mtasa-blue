/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUIStaticImage.h>
#include "CGUIElement_Impl.h"

class CGUITexture;

class CGUIStaticImage_Impl : public CGUIStaticImage, public CGUIElement_Impl
{
public:
    CGUIStaticImage_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, CVector2D pos, CVector2D size, bool relative);

    void Begin();
    void End();

    CGUITexture* GetTexture() { return m_texture; }

    bool LoadFromFile(std::string path);
    void LoadFromTexture(CGUITexture* texture);

#include "CGUIElement_Inc.h"
private:
    CGUITexture* m_texture;
};
