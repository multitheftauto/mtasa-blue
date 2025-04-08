/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIWebBrowser_Impl.h
 *  PURPOSE:     WebBrowser CGUI class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <gui/CGUIWebBrowser.h>
#include "CGUITexture_Impl.h"
#include <renderers/directx9GUIRenderer/d3d9texture.h>

// Use StaticImage here as we'd have to add the same definition twice to the Falagard definition file otherwise
#define CGUIWEBBROWSER_NAME "CGUI/StaticImage"

class CGUITexture;
class CGUITexture_Impl;
class CGUI_Impl;
class CWebViewInterface;

class CGUIWebBrowser_Impl : public CGUIWebBrowser, public CGUIElement_Impl
{
public:
    CGUIWebBrowser_Impl(CGUI_Impl* pGUI, CGUIElement* pParent = nullptr);
    ~CGUIWebBrowser_Impl();
    void Clear();

    void LoadFromWebView(CWebViewInterface* pWebView);

    void SetFrameEnabled(bool bFrameEnabled);
    bool IsFrameEnabled();

    CEGUI::Image* GetDirectImage();
    void          Render();

    virtual eCGUIType GetType() override { return CGUI_WEBBROWSER; }

    bool HasInputFocus();

    virtual void SetSize(const CVector2D& vecSize, bool bRelative = false) override;

protected:
    bool Event_MouseButtonDown(const CEGUI::EventArgs& e);
    bool Event_MouseButtonUp(const CEGUI::EventArgs& e);
    bool Event_MouseDoubleClick(const CEGUI::EventArgs& e);
    bool Event_MouseWheel(const CEGUI::EventArgs& e);
    bool Event_MouseMove(const CEGUI::EventArgs& e);
    bool Event_Activated(const CEGUI::EventArgs& e);
    bool Event_Deactivated(const CEGUI::EventArgs& e);

private:
    CGUI_Impl*              m_pGUI;
    CEGUI::ImagesetManager* m_pImagesetManager;
    CEGUI::Imageset*        m_pImageset;
    CEGUI::Image*           m_pImage;

    CWebViewInterface* m_pWebView;

    #define EXCLUDE_SET_SIZE // WTF? TODO: Refactor this
    #include "CGUIElement_Inc.h"
    #undef EXCLUDE_SET_SIZE
};

// The purpose of this class is to provide an externally managed DirectX texture
class CGUIWebBrowserTexture : public CEGUI::DirectX9Texture
{
public:
    CGUIWebBrowserTexture(CEGUI::Renderer* pOwner, CWebViewInterface* pWebView);

    virtual ushort getWidth() const override;
    virtual ushort getHeight() const override;

    // Override with empty function (--> eliminate the functinions from DirectX9Texture)
    virtual void loadFromFile(const CEGUI::String& filename, const CEGUI::String& resourceGroup) override{};
    virtual void loadFromMemory(const void* buffPtr, uint buffWidth, uint buffHeight) override{};

    virtual LPDIRECT3DTEXTURE9 getD3DTexture() const override;
    virtual void               preD3DReset(){};
    virtual void               postD3DReset(){};

private:
    CWebViewInterface* m_pWebView;
};
