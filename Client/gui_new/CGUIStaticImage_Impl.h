/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIStaticImage_Impl.h
 *  PURPOSE:     Static image widget class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui_new/CGUIStaticImage.h>
#include "CGUITexture_Impl.h"

class GUINew::CGUITexture;
class GUINew::CGUITexture_Impl;

namespace GUINew{
    class CGUIStaticImage_Impl;
}


class GUINew::CGUIStaticImage_Impl : public CGUIStaticImage, public CGUIElement_Impl, public CGUITabList
{
public:
    CGUIStaticImage_Impl(class CGUI_Impl* pGUI, CGUIElement* pParent = NULL);
    ~CGUIStaticImage_Impl();

    bool LoadFromFile(const char* szFilename, const char* szResourceGroup = "mta");
    bool LoadFromTexture(CGUITexture* pTexture);
    bool GetNativeSize(CVector2D& vecSize);
    void Clear();

    void SetFrameEnabled(bool bFrameEnabled);
    bool IsFrameEnabled();

    CEGUI::Image* GetDirectImage();

    void Render();

    eCGUIType GetType() { return CGUI_STATICIMAGE; }

private:
    CGUI_Impl*                   m_pGUI;
    CGUITexture_Impl*            m_pTexture;
    CEGUI::BasicImage*           m_pImage;
    CEGUI::ImageManager*         m_pImagesetManager;

    #include "CGUIElement_Inc.h"
};
