/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUI_Impl.cpp
 *  PURPOSE:     Graphical User Interface module class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

using std::list;

CGUI_Impl::CGUI_Impl(IDirect3DDevice9* pDevice)
{
    m_pDevice = pDevice;
    m_hookedWindow = nullptr;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

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

void CGUI_Impl::SetHookedWindow(HWND window)
{
    if (m_hookedWindow != nullptr)
    {     
        ImGui_ImplWin32_Shutdown();
        delete m_hookedWindow;
    }

    if (ImGui_ImplWin32_Init(window))
        m_hookedWindow = window;
}

void CGUI_Impl::Draw()
{
    if (GetHookedWindow() == nullptr)
        return;
        
    // Start the Dear ImGui frame
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();

    ImGui::NewFrame();

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (m_bShowDemoWindow)
        ImGui::ShowDemoWindow(&m_bShowDemoWindow);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &m_bShowDemoWindow);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &m_bShowAnotherWindow);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&m_clearColor); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (m_bShowAnotherWindow)
    {
        ImGui::Begin("Another Window", &m_bShowAnotherWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            m_bShowAnotherWindow = false;
        ImGui::End();
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

