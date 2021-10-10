/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUITexture.h>
#include "CGUIElement_Impl.h"

class CTextureItem;

class CGUITexture_Impl : public CGUITexture, public CGUIElement_Impl
{
public:
    CGUITexture_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, CVector2D pos, CVector2D size, bool relative);

    void Begin();
    void End();

    IDirect3DBaseTexture9* GetD3DTexture();

    void LoadFromFile(std::string path);

#include "CGUIElement_Inc.h"
private:
    CTextureItem* m_textureItem;
};
