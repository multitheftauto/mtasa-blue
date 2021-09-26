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

    // Windows test
    CGUIWindow* window = CreateGUIWindow(CVector2D(0, 0), CVector2D(500, 500), nullptr, false, "Parent Window");

    CGUIWindow* window2 = CreateGUIWindow(CVector2D(50, 50), CVector2D(150, 150), window, false, "Child Window");

    /* Tested functions */

    //window2->SetFrameEnabled(false);
    //window2->SetPosition(CVector2D(75, 125));
    //window2->SetSize(CVector2D(200, 200));
    //window2->SetParent(nullptr);
}

CGUI_Impl::~CGUI_Impl()
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();
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
    if (GetHookedWindow() == nullptr)
        return;

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

            if (elem->GetParent() == nullptr && !elem->IsDeleted())
            {
                elem->Begin();

                list<CGUIElement*> children = elem->GetChildren();
                list<CGUIElement*>::const_iterator c = children.begin();

                for (; c != children.end(); c++)
                {
                    CGUIElement* child = (*c);

                    child->Begin();
                    child->End();
                }

                elem->End();
            }
        }
    }

    ImGui::EndFrame();

    // Rendering
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void CGUI_Impl::Invalidate()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
}

void CGUI_Impl::Restore()
{
    ImGui_ImplDX9_CreateDeviceObjects();
}

void CGUI_Impl::OnElementDestroy(CGUIElement* element)
{
    m_guiElements.remove(element);
}

CGUIWindow* CGUI_Impl::CreateGUIWindow(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative, std::string title)
{
    CGUIWindow* element = new CGUIWindow_Impl(this, parent, pos, size, relative, title);

    if (!element)
        return nullptr;

    m_guiElements.push_back(reinterpret_cast<CGUIElement*>(element));

    return element;
}
