/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#include <iostream>
#include <sstream>

CGUIElement_Impl::CGUIElement_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, CVector2D pos, CVector2D size, bool relative)
{
    m_pManager = pGUI;

    // Create a unique ID for this element
    const void* address = static_cast<const void*>(this);

    std::stringstream ss;
    ss << address;

    m_uid = std::to_string(GetTickCount32()) + ss.str();

    // Set flags
    SetFlag(ImGuiWindowFlags_NoBackground, true);
    SetFlag(ImGuiWindowFlags_NoDecoration, true);
    SetFlag(ImGuiWindowFlags_NoMove, true);
    SetFlag(ImGuiWindowFlags_NoResize, true);
    SetFlag(ImGuiWindowFlags_NoBringToFrontOnFocus, true);

    SetParent(pParent);
    SetPosition(pos, relative);
    SetSize(size, relative);
}

void CGUIElement_Impl::Destroy()
{
    m_deleted = true;
}

CGUIElement_Impl::~CGUIElement_Impl()
{
    std::vector<CGUIElement*>::iterator c = m_children.begin();
    while (c != m_children.end())
    {
        CGUIElement* child = (*c);
        m_children.erase(c++);
        child->Destroy();
    }

    m_renderFunctions.clear();
    m_renderFunctionIndexMap.clear();
}

std::string CGUIElement_Impl::GetID(bool imgui)
{
    return imgui ? (m_text + "###" + m_uid) : m_uid;
}

void CGUIElement_Impl::SetParent(CGUIElement* parent)
{
    if (parent == m_pParent)
        return;

    if (m_pParent)
    {
        m_pParent->RemoveChild(this);
    }

    m_pParent = parent;

    if (parent)
    {
        parent->AddChild(this);
    }
}

CGUIElement* CGUIElement_Impl::GetParent()
{
    return m_pParent;
}

void CGUIElement_Impl::AddChild(CGUIElement* child)
{
    bool hasChild = (std::find(m_children.begin(), m_children.end(), child) != m_children.end());

    if (hasChild)
        return;

    m_children.push_back(child);
}

void CGUIElement_Impl::RemoveChild(CGUIElement* child)
{
    auto iter = std::find(m_children.begin(), m_children.end(), child);

    if (iter == m_children.end())
        return;

    m_children.erase(iter);
}

std::vector<CGUIElement*> CGUIElement_Impl::GetChildren()
{
    return m_children;
}

void CGUIElement_Impl::SetIndex(int index)
{
    if (m_pParent == nullptr)
        m_pManager->SetElementIndex(this, index);
    else
        m_pParent->SetChildIndex(this, index);
}

int CGUIElement_Impl::GetIndex()
{
    if (m_pParent == nullptr)
        return m_pManager->GetElementIndex(this);

    return m_pParent->GetChildIndex(this);
}

int CGUIElement_Impl::GetChildIndex(CGUIElement* child)
{
    auto iter = std::find(m_children.begin(), m_children.end(), child);

    if (iter != m_children.end())
        return std::distance(m_children.begin(), iter);

    return 0;
}

void CGUIElement_Impl::SetChildIndex(CGUIElement* child, int index)
{
    std::string id = child->GetID();

    // exclude all numbers below 1, excluding -1
    if (index < 1 && index != -1)
        return;

    if (index == -1)
        index = m_children.size();

    std::vector<CGUIElement*>::iterator e = m_children.begin();
    while (e != m_children.end())
    {
        CGUIElement* elem = (*e);

        if (elem == child)
        {
            m_children.erase(e);
            m_children.insert(m_children.begin() + (index - 1), child);

            return;
        }

        ++e;
    }
}

bool CGUIElement_Impl::IsDestroyed()
{
    return m_deleted;
}

void CGUIElement_Impl::BringToFront()
{
    SetIndex(-1);
}

void CGUIElement_Impl::MoveToBack()
{
    SetIndex(1);
}

void CGUIElement_Impl::SetFlag(ImGuiWindowFlags flag, bool state)
{
    auto iter = m_flags.find(flag);
    bool hasFlag = (iter != m_flags.end());

    if (!hasFlag && state)
        m_flags.insert(flag);
    else if (hasFlag && !state)
        m_flags.erase(iter);
}

void CGUIElement_Impl::ResetFlags()
{
    m_flags.erase(m_flags.begin(), m_flags.end());
}

int CGUIElement_Impl::GetFlagBits()
{
    int bits = 0;

    std::set<ImGuiWindowFlags>::iterator flag;
    for (flag = m_flags.begin(); flag != m_flags.end(); ++flag)
    {
        bits |= (*flag);
    }

    return bits;
}

std::set<ImGuiWindowFlags> CGUIElement_Impl::GetFlags()
{
    return m_flags;
}

void CGUIElement_Impl::Begin()
{
    m_rendering = true;

    const char* id = GetID(true).c_str();

    ProcessPosition();

    // Call PRE render functions (before Begin/BeginChild)
    std::list<std::function<void()>>::const_iterator preFunc = m_preRenderFunctions.begin();

    for (; preFunc != m_preRenderFunctions.end(); preFunc++)
    {
        if (IsDestroyed())            // Maybe we got destroyed in the previous render function
            return;

        (*preFunc)();
    }

    if (m_pParent == nullptr)
    {
        ProcessSize();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, m_borderSize);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(m_padding.fX, m_padding.fY));
        {
            ImGui::Begin(id, (bool*)0, GetFlagBits());
        }
        ImGui::PopStyleVar(2);
    }
    else
    {
        if (!ImGui::BeginChild(id, ImVec2(m_size.fX, m_size.fY), m_hasFrame))
            return;
    }

    // Call render functions - you can 'hook' and modify this element (useful for script customization)
    std::list<std::function<void()>>::const_iterator func = m_renderFunctions.begin();

    for (; func != m_renderFunctions.end(); func++)
    {
        if (IsDestroyed())            // Maybe we got destroyed in the previous render function
            return;

        (*func)();
    }
}

void CGUIElement_Impl::End()
{
    if (m_pParent == nullptr)
        ImGui::End();
    else
        ImGui::EndChild();

    m_rendering = false;
}

int CGUIElement_Impl::AddRenderFunction(std::function<void()> renderFunction, bool preRender)
{
    std::list<std::function<void()>>& list = preRender ? m_preRenderFunctions : m_renderFunctions;
    std::map<int, decltype(list.cbegin())>& map = preRender ? m_preRenderFunctionIndexMap : m_renderFunctionIndexMap;

    auto iter = list.insert(list.end(), renderFunction);
    int  index = preRender ? ++m_numLifetimePreRenderFunctions : ++m_numLifetimeRenderFunctions;

    map[index] = iter;
    return index;
}

bool CGUIElement_Impl::RemoveRenderFunction(int index, bool preRender)
{
    std::list<std::function<void()>>& list = preRender ? m_preRenderFunctions : m_renderFunctions;
    std::map<int, decltype(list.cbegin())>& map = preRender ? m_preRenderFunctionIndexMap : m_renderFunctionIndexMap;

    if (map.count(index) <= 0)
        return false;

    auto iter = map.at(index);

    if (iter == list.end())
        return false;

    list.erase(iter);
    map.erase(index);

    return true;
}

std::list<std::function<void()>>& CGUIElement_Impl::GetRenderFunctions(bool preRender)
{
    return preRender ? m_preRenderFunctions : m_renderFunctions;
}

CGUIType CGUIElement_Impl::GetType()
{
    return m_type;
}

void CGUIElement_Impl::SetPosition(CVector2D pos, bool relative)
{
    if (relative)
    {
        CVector2D parentSize = m_pParent ? m_pParent->GetSize() : m_pManager->GetResolution();
        pos = CVector2D(pos.fX * parentSize.fX, pos.fY * parentSize.fY);
    }

    m_position = pos;
    m_updatePosition = true;
}

void CGUIElement_Impl::SetSize(CVector2D size, bool relative)
{
    if (m_autoSize)
        return;

    if (relative)
    {
        CVector2D parentSize = m_pParent ? m_pParent->GetSize() : m_pManager->GetResolution();
        size = CVector2D(size.fX * parentSize.fX, size.fY * parentSize.fY);
    }

    m_size = size;

    if (m_autoSize)
    {
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();

        ImGui::NewFrame();

        Begin();

        ImVec2 textSize = ImGui::CalcTextSize(m_text.c_str());

        End();

        ImGui::EndFrame();

        m_size.fX = (size.fX > textSize.x) ? size.fX : textSize.x;
        m_size.fY = (size.fY > textSize.y) ? size.fY : textSize.y;
    }

    m_updateSize = true;
}

CVector2D CGUIElement_Impl::GetPosition()
{
    return m_position;
}

CVector2D CGUIElement_Impl::GetSize()
{
    return m_size;
}

void CGUIElement_Impl::ProcessPosition()
{
    if (m_pParent == nullptr && m_updatePosition)
    {
        ImGui::SetNextWindowPos(ImVec2(m_position.fX, m_position.fY));
        m_updatePosition = false;
    }
    else if (m_pParent && !m_hasDynamicPosition)
    {
        ImGui::SetCursorPosX(m_position.fX);
        ImGui::SetCursorPosY(m_position.fY);
    }
}

void CGUIElement_Impl::ProcessSize()
{
    if (m_updateSize && m_pParent == nullptr)
        ImGui::SetNextWindowSize(ImVec2(m_size.fX, m_size.fY));



    m_updateSize = false;
}

void CGUIElement_Impl::SetFrameEnabled(bool state)
{
    m_hasFrame = state;
}

bool CGUIElement_Impl::GetFrameEnabled()
{
    return m_hasFrame;
}

void CGUIElement_Impl::SetDynamicPositionEnabled(bool state)
{
    m_hasDynamicPosition = state;
}

bool CGUIElement_Impl::GetDynamicPositionEnabled()
{
    return m_hasDynamicPosition;
}

void CGUIElement_Impl::SetText(std::string text)
{
    m_text = text;
}

std::string CGUIElement_Impl::GetText()
{
    return m_text;
}

void CGUIElement_Impl::SetRenderingEnabled(bool state)
{
    m_render = state;
}

bool CGUIElement_Impl::IsRenderingEnabled() const
{
    return m_render;
}

void CGUIElement_Impl::SetVisible(bool state)
{
    m_visible = state;
}

bool CGUIElement_Impl::IsVisible()
{
    return m_visible;
}

void CGUIElement_Impl::SetAlpha(float alpha)
{
    m_alpha = alpha;
}

float CGUIElement_Impl::GetAlpha(bool clamp)
{
    return clamp ? Clamp<float>(0.f, m_alpha, 1.0f) : m_alpha;
}

CVector2D CGUIElement_Impl::GetTextSize()
{
    // Start the Dear ImGui frame
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();

    ImGui::NewFrame();

    Begin();

    ImVec2 size = ImGui::CalcTextSize(m_text.c_str());

    End();

    ImGui::EndFrame();

    return CVector2D(size.x, size.y);
}

float CGUIElement_Impl::GetTextExtent()
{
    return GetTextSize().fX;
}

CColor CGUIElement_Impl::GetTextColor()
{
    return m_textColor;
}

void CGUIElement_Impl::SetTextColor(CColor color)
{
    m_textColor = color;
}

void CGUIElement_Impl::SetTextColor(int r, int g, int b, int a)
{
    m_textColor = CColor(Clamp(0, r, 255), Clamp(0, g, 255), Clamp(0, b, 255), Clamp(0, a, 255));
}

CGUITextAlignHorizontal CGUIElement_Impl::GetTextHorizontalAlign()
{
    return m_textAlign.first;
}

CGUITextAlignVertical CGUIElement_Impl::GetTextVerticalAlign()
{
    return m_textAlign.second;
}

void CGUIElement_Impl::SetTextHorizontalAlign(CGUITextAlignHorizontal align)
{
    m_textAlign.first = align;
}

void CGUIElement_Impl::SetTextVerticalAlign(CGUITextAlignVertical align)
{
    m_textAlign.second = align;
}

void CGUIElement_Impl::SetAutoSizingEnabled(bool state)
{
    m_autoSize = state;
}

bool CGUIElement_Impl::IsAutoSizingEnabled()
{
    return m_autoSize;
}
