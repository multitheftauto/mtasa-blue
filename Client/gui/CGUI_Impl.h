/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUI.h>
#include <list>

class CGUI_Impl : public CGUI
{
public:
    CGUI_Impl(IDirect3DDevice9* pDevice);
    ~CGUI_Impl();

    void Draw();
    void Draw(CGUIElement* element);

    void Invalidate();
    void Restore();

    void SetHookedWindow(HWND window);
    HWND GetHookedWindow() const { return m_hookedWindow; }

    void                SetGraphicsInterface(CGraphicsInterface* graphics);
    CGraphicsInterface* GetGraphicsInterface() { return m_pGraphics; }

    CGUIBrowser*     CreateBrowser(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false);
    CGUIButton*      CreateButton(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false, std::string text = "");
    CGUICheckbox*    CreateCheckbox(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false);
    CGUICombobox*    CreateCombobox(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false);
    CGUIEdit*        CreateEdit(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false);
    CGUIGridList*    CreateGridList(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false);
    CGUILabel*       CreateLabel(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false);
    CGUIMemo*        CreateMemo(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false);
    CGUIProgressBar* CreateProgressBar(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false);
    CGUIRadioButton* CreateRadioButton(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false);
    CGUIScrollbar*   CreateScrollbar(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false);
    CGUIScrollpane*  CreateScrollpane(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false);
    CGUIStaticImage* CreateStaticImage(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false);
    CGUITab*         CreateTab(CGUITabPanel* parent, std::string text);
    CGUITabPanel*    CreateTabPanel(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false);
    CGUITexture*     CreateTexture(CVector2D size, bool relative = false);
    CGUIWindow*      CreateWindow(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false, std::string title = "Window");

    bool ProcessWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

    CVector2D GetResolution();

    void SetElementIndex(CGUIElement* element, int index);
    int  GetElementIndex(CGUIElement* element);

    ImGuiContext* GetCurrentContext();

    void OnMousePress(CGUIMouseButton button, CGUIElement* element = nullptr);
    void OnMouseRelease(CGUIMouseButton button, CGUIElement* element = nullptr);

    void CreateDemo();

private:
    CGraphicsInterface* m_pGraphics;

    IDirect3DDevice9* m_pDevice;
    HWND              m_hookedWindow;

    ImGuiContext* m_context = nullptr;

    std::vector<CGUIElement*> m_guiElements = {};

    bool m_bShowDemoWindow = true;
    bool m_bShowAnotherWindow = false;

    ImVec4 m_clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    void AddElement(CGUIElement* element);
    void RemoveElement(CGUIElement* element);
};
