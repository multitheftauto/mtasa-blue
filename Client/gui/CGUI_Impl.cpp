/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

CGUI_Impl::CGUI_Impl(IDirect3DDevice9* pDevice)
{
    m_pDevice = pDevice;
    m_pGraphics = nullptr;

    m_hookedWindow = nullptr;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    m_context = ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    io.MouseDrawCursor = true;

    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplDX9_Init(m_pDevice);
}

CGUI_Impl::~CGUI_Impl()
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();
}

ImGuiContext* CGUI_Impl::GetCurrentContext()
{
    return m_context;
}

void CGUI_Impl::CreateDemo()
{
    // CGUIWindow* window1 = CreateWindow(CVector2D(0, 0), CVector2D(200, 200), nullptr, false, "Parent Window 1");

    // CGUIStaticImage* image1 = CreateStaticImage(CVector2D(50, 50), CVector2D(50, 50), window1, false);
    // image1->LoadFromFile(CalcMTASAPath(PathJoin("MTA", "cgui", "images", "error.png")));

    // CGUIStaticImage* image2 = CreateStaticImage(CVector2D(75, 75), CVector2D(50, 50), window1, false);
    // image2->LoadFromFile(CalcMTASAPath(PathJoin("MTA", "cgui", "images", "info.png")));

    // CGUIStaticImage* image3 = CreateStaticImage(CVector2D(100, 100), CVector2D(50, 50), window1, false);
    // image3->LoadFromFile(CalcMTASAPath(PathJoin("MTA", "cgui", "images", "question.png")));

    // CGUIWindow* window2 = CreateWindow(CVector2D(100, 100), CVector2D(200, 200), nullptr, false, "Parent Window 2");
    // CGUIWindow* window3 = CreateWindow(CVector2D(200, 200), CVector2D(200, 200), nullptr, false, "Parent Window 3");

    // image1->BringToFront();
    // window1->BringToFront();

    // image1->MoveToBack();
    // window1->MoveToBack();
}

bool CGUI_Impl::ProcessWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return ImGui_ImplWin32_WndProcHandler(wnd, msg, wParam, lParam) != 0;
}

void CGUI_Impl::SetGraphicsInterface(CGraphicsInterface* graphics)
{
    m_pGraphics = graphics;
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

void CGUI_Impl::OnMousePress(CGUIMouseButton button, CGUIElement* element)
{
    printf("pressed imgui element (%s) \n", element->GetID().c_str());
}

void CGUI_Impl::OnMouseRelease(CGUIMouseButton button, CGUIElement* element)
{
    printf("released imgui element (%s) \n", element->GetID().c_str());
}

void CGUI_Impl::Draw()
{
    if (m_hookedWindow == nullptr)
        return;

    // Check if any elements need destroying, savely remove
    std::vector<CGUIElement*>::iterator e = m_guiElements.begin();
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
        std::vector<CGUIElement*>::reverse_iterator e = m_guiElements.rbegin();
        for (; e != m_guiElements.rend(); e++)
        {
            CGUIElement* elem = (*e);

            if (elem->GetParent() == nullptr && elem->IsRenderingEnabled() && elem->IsVisible())
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

    if (ImGui::IsWindowHovered() || ImGui::IsItemHovered())
    {
        for (auto button : CGUIMouseButtons)
        {
            int i = static_cast<int>(button);

            if (ImGui::IsMouseClicked(i))
            {
                OnMousePress(button, element);
                break;
            }
            else if (ImGui::IsMouseReleased(i))
            {
                OnMouseRelease(button, element);
                break;
            }
        }
    }

    std::vector<CGUIElement*>           children = element->GetChildren();
    std::vector<CGUIElement*>::iterator c = children.begin();

    for (; c != children.end(); c++)
    {
        CGUIElement* child = (*c);

        if (child->IsRenderingEnabled() && child->IsVisible())
        {
            Draw(child);
        }
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
    if (m_hookedWindow == nullptr)
        return {};

    RECT rect;

    if (!GetWindowRect(m_hookedWindow, &rect))
        return {};

    return CVector2D(rect.right - rect.left, rect.bottom - rect.top);
}

void CGUI_Impl::AddElement(CGUIElement* element)
{
    m_guiElements.push_back(element);
}

void CGUI_Impl::RemoveElement(CGUIElement* element)
{
    std::vector<CGUIElement*>::iterator position = std::find(m_guiElements.begin(), m_guiElements.end(), element);

    if (position != m_guiElements.end())
        m_guiElements.erase(position);
}

void CGUI_Impl::SetElementIndex(CGUIElement* element, int index)
{
    // exclude all numbers below 1, excluding -1
    if (index < 1 && index != -1)
        return;

    std::string id = element->GetID();
    int         nonRootCount = 0;

    if (index == -1)
        index = m_guiElements.size();

    std::vector<CGUIElement*>::iterator e = m_guiElements.begin();
    while (e != m_guiElements.end())
    {
        CGUIElement* elem = (*e);

        if (elem->GetParent() == nullptr && elem->IsRenderingEnabled() && elem->GetID() == id)
        {
            m_guiElements.erase(e);
            m_guiElements.insert(m_guiElements.begin() + (index + nonRootCount) - 1, element);

            return;
        }
        else
            ++nonRootCount;

        ++e;
    }
}

int CGUI_Impl::GetElementIndex(CGUIElement* element)
{
    std::string id = element->GetID();
    int         nonRootCount = 0;

    std::vector<CGUIElement*>::iterator e = m_guiElements.begin();
    while (e != m_guiElements.end())
    {
        CGUIElement* elem = (*e);

        if (elem->GetParent() == nullptr && elem->IsRenderingEnabled())
        {
            if (elem->GetID() == id)
            {
                return std::distance(m_guiElements.begin(), e) - nonRootCount;
            }
        }
        else
            ++nonRootCount;

        ++e;
    }

    return 0;
}

CGUIBrowser* CGUI_Impl::CreateBrowser(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUIBrowser* element = new CGUIBrowser_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    AddElement(element);

    return element;
}

CGUIButton* CGUI_Impl::CreateButton(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative, std::string text)
{
    CGUIButton* element = new CGUIButton_Impl(this, parent, pos, size, relative, text);

    if (!element)
        return nullptr;

    AddElement(element);

    return element;
}

CGUICheckbox* CGUI_Impl::CreateCheckbox(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUICheckbox* element = new CGUICheckbox_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    AddElement(element);

    return element;
}

CGUICombobox* CGUI_Impl::CreateCombobox(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUICombobox* element = new CGUICombobox_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    AddElement(element);

    return element;
}

CGUIEdit* CGUI_Impl::CreateEdit(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUIEdit* element = new CGUIEdit_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    AddElement(element);

    return element;
}

CGUIGridList* CGUI_Impl::CreateGridList(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUIGridList* element = new CGUIGridList_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    AddElement(element);

    return element;
}

CGUILabel* CGUI_Impl::CreateLabel(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUILabel* element = new CGUILabel_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    AddElement(element);

    return element;
}

CGUIMemo* CGUI_Impl::CreateMemo(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUIMemo* element = new CGUIMemo_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    AddElement(element);

    return element;
}

CGUIProgressBar* CGUI_Impl::CreateProgressBar(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUIProgressBar* element = new CGUIProgressBar_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    AddElement(element);

    return element;
}

CGUIRadioButton* CGUI_Impl::CreateRadioButton(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUIRadioButton* element = new CGUIRadioButton_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    AddElement(element);

    return element;
}

CGUIScrollbar* CGUI_Impl::CreateScrollbar(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUIScrollbar* element = new CGUIScrollbar_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    AddElement(element);

    return element;
}

CGUIScrollpane* CGUI_Impl::CreateScrollpane(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUIScrollpane* element = new CGUIScrollpane_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    AddElement(element);

    return element;
}

CGUIStaticImage* CGUI_Impl::CreateStaticImage(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUIStaticImage* element = new CGUIStaticImage_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    AddElement(element);

    return element;
}

CGUITabPanel* CGUI_Impl::CreateTabPanel(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative)
{
    CGUITabPanel* element = new CGUITabPanel_Impl(this, parent, pos, size, relative);

    if (!element)
        return nullptr;

    AddElement(element);

    return element;
}

CGUITab* CGUI_Impl::CreateTab(CGUITabPanel* parent, std::string text)
{
    CGUITab* element = new CGUITab_Impl(this, reinterpret_cast<CGUIElement*>(parent), text);

    if (!element)
        return nullptr;

    AddElement(element);

    return element;
}

CGUITexture* CGUI_Impl::CreateTexture(CVector2D size, bool relative)
{
    CGUITexture* element = new CGUITexture_Impl(this, nullptr, {}, size, relative);
    element->SetRenderingEnabled(false);

    if (!element)
        return nullptr;

    AddElement(element);

    return element;
}

CGUIWindow* CGUI_Impl::CreateWindow(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative, std::string title)
{
    CGUIWindow* element = new CGUIWindow_Impl(this, parent, pos, size, relative, title);

    if (!element)
        return nullptr;

    AddElement(element);

    return element;
}
