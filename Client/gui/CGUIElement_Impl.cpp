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

void CGUIElement_Impl::DemoHookTest()
{
    ImGui::Text("Testing hook");

    ImGui::SameLine();

    if (ImGui::Button("Recreate demo", ImVec2(100, 30)))
    {
        // Destroy demo and create it again (assume we're the root demo window)
        Destroy();
        m_pManager->CreateDemo();
    }
}

std::string CGUIElement_Impl::GetID()
{
    return m_uid;
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

    const char* id = (m_text + "###" + m_uid).c_str();

    ProcessPosition();

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

int CGUIElement_Impl::AddRenderFunction(std::function<void()> renderFunction)
{
    auto iter = m_renderFunctions.insert(m_renderFunctions.end(), renderFunction);
    int  index = ++m_numLifetimeRenderFunctions;

    m_renderFunctionIndexMap[index] = iter;
    return index;
}

void CGUIElement_Impl::RemoveRenderFunction(int index)
{
    if (m_renderFunctionIndexMap.count(index) <= 0)
        return;

    auto iter = m_renderFunctionIndexMap.at(index);

    m_renderFunctions.erase(iter);
    m_renderFunctionIndexMap.erase(index);
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
    if (relative)
    {
        CVector2D parentSize = m_pParent ? m_pParent->GetSize() : m_pManager->GetResolution();
        size = CVector2D(size.fX * parentSize.fX, size.fY * parentSize.fY);
    }

    m_size = size;
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
    if (m_updateSize)
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

void CGUIElement_Impl::SetAlpha(int alpha)
{
    m_alpha = alpha;
}

int CGUIElement_Impl::GetAlpha()
{
    return m_alpha;
}
