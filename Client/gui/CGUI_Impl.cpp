/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

using std::list;

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

CGUI_Impl::CGUI_Impl(IDirect3DDevice9* pDevice)
{
    m_pDevice = pDevice;
    m_hookedWindow = nullptr;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplDX9_Init(m_pDevice);

    // Demo stuff & testing
    CreateDemo();
}

CGUI_Impl::~CGUI_Impl()
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();
}

void CGUI_Impl::CreateDemo()
{
    CGUIWindow* window = CreateWindow(CVector2D(0, 0), CVector2D(500, 500), nullptr, false, "Parent Window");
    CGUIWindow* window2 = CreateWindow(CVector2D(50, 100), CVector2D(300, 300), window, false, "Child Window");
    CGUIWindow* window3 = CreateWindow(CVector2D(50, 50), CVector2D(200, 200), window2, false, "Child Window 2");

    int funcIndex = window->AddRenderFunction(std::bind(&CGUIElement::DemoHookTest, window));
    // window->RemoveRenderFunction(funcIndex);

    window2->SetDynamicPositionEnabled(true);
    // window2->SetFrameEnabled(false);
    // window2->SetPosition(CVector2D(75, 75));
    // window2->SetSize(CVector2D(100, 100));
    // window2->SetParent(nullptr);
}

bool CGUI_Impl::ProcessWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return ImGui_ImplWin32_WndProcHandler(wnd, msg, wParam, lParam) != 0;
}

void CGUI_Impl::SetHookedWindow(HWND window)
{
    if (m_hookedWindow != nullptr)
    {
        ImGui_ImplWin32_Shutdown();
        delete m_hookedWindow;
    }

    if (ImGui_ImplWin32_Init(window))
    {
        m_hookedWindow = window;
    }
}

void CGUI_Impl::Draw()
{
    if (m_hookedWindow == nullptr)
        return;

    // Check if any elements need destroying, savely remove
    std::list<CGUIElement*>::iterator e = m_guiElements.begin();
    while (e != m_guiElements.end())
    {
        CGUIElement* elem = (*e);

        if (elem->IsDestroyed())
        {
            m_guiElements.erase(e);
            delete elem;
        }

        ++e;
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();

    ImGui::NewFrame();

    // Draw ImGui elements
    if (!m_guiElements.empty())
    {
        list<CGUIElement*>::const_iterator e = m_guiElements.begin();
        for (; e != m_guiElements.end(); e++)
        {
            CGUIElement* elem = (*e);

            if (elem->GetParent() == nullptr)
            {
                Draw(elem);
            }
        }
    }

    ImGui::EndFrame();

    // Rendering
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void CGUI_Impl::Draw(CGUIElement* element)
{
    if (element == nullptr)
        return;

    // Draw ImGui element
    element->Begin();

    list<CGUIElement*>                 children = element->GetChildren();
    list<CGUIElement*>::const_iterator c = children.begin();

    for (; c != children.end(); c++)
    {
        CGUIElement* child = (*c);
        Draw(child);
    }

    element->End();
}

void CGUI_Impl::Invalidate()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
}

void CGUI_Impl::Restore()
{
    ImGui_ImplDX9_CreateDeviceObjects();
}

CVector2D CGUI_Impl::GetResolution()
{
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    return CVector2D(io.DisplaySize.x, io.DisplaySize.y);
}

CGUIBrowser* CGUI_Impl::CreateBrowser(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUIBrowser* element = new CGUIBrowser_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    m_guiElements.push_back(reinterpret_cast<CGUIElement*>(element));

    return element;
}

CGUIButton* CGUI_Impl::CreateButton(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUIButton* element = new CGUIButton_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    m_guiElements.push_back(reinterpret_cast<CGUIElement*>(element));

    return element;
}

CGUICheckbox* CGUI_Impl::CreateCheckbox(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUICheckbox* element = new CGUICheckbox_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    m_guiElements.push_back(reinterpret_cast<CGUIElement*>(element));

    return element;
}

CGUICombobox* CGUI_Impl::CreateCombobox(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUICombobox* element = new CGUICombobox_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    m_guiElements.push_back(reinterpret_cast<CGUIElement*>(element));

    return element;
}

CGUIEdit* CGUI_Impl::CreateEdit(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUIEdit* element = new CGUIEdit_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    m_guiElements.push_back(reinterpret_cast<CGUIElement*>(element));

    return element;
}

CGUIGridList* CGUI_Impl::CreateGridList(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUIGridList* element = new CGUIGridList_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    m_guiElements.push_back(reinterpret_cast<CGUIElement*>(element));

    return element;
}

CGUILabel* CGUI_Impl::CreateLabel(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUILabel* element = new CGUILabel_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    m_guiElements.push_back(reinterpret_cast<CGUIElement*>(element));

    return element;
}

CGUIMemo* CGUI_Impl::CreateMemo(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUIMemo* element = new CGUIMemo_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    m_guiElements.push_back(reinterpret_cast<CGUIElement*>(element));

    return element;
}

CGUIProgressBar* CGUI_Impl::CreateProgressBar(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUIProgressBar* element = new CGUIProgressBar_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    m_guiElements.push_back(reinterpret_cast<CGUIElement*>(element));

    return element;
}

CGUIRadioButton* CGUI_Impl::CreateRadioButton(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUIRadioButton* element = new CGUIRadioButton_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    m_guiElements.push_back(reinterpret_cast<CGUIElement*>(element));

    return element;
}

CGUIScrollbar* CGUI_Impl::CreateScrollbar(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUIScrollbar* element = new CGUIScrollbar_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    m_guiElements.push_back(reinterpret_cast<CGUIElement*>(element));

    return element;
}

CGUIScrollpane* CGUI_Impl::CreateScrollpane(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUIScrollpane* element = new CGUIScrollpane_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    m_guiElements.push_back(reinterpret_cast<CGUIElement*>(element));

    return element;
}

CGUIStaticImage* CGUI_Impl::CreateStaticImage(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUIStaticImage* element = new CGUIStaticImage_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    m_guiElements.push_back(reinterpret_cast<CGUIElement*>(element));

    return element;
}


CGUITabPanel* CGUI_Impl::CreateTabPanel(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUITabPanel* element = new CGUITabPanel_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    m_guiElements.push_back(reinterpret_cast<CGUIElement*>(element));

    return element;
}


CGUIWindow* CGUI_Impl::CreateWindow(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative, std::string title)
{
    CGUIWindow* element = new CGUIWindow_Impl(this, parent, pos, size, relative, title);

    if (!element)
        return nullptr;

    m_guiElements.push_back(reinterpret_cast<CGUIElement*>(element));

    return element;
}
