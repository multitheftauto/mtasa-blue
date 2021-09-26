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

    m_initialPosition = pos;
    m_initialSize = size;

    SetParent(pParent);

    m_relative = relative;

    // Create a unique ID for this element
    const void* address = static_cast<const void*>(this);

    std::stringstream ss;
    ss << address;

    m_uid = std::to_string(GetTickCount32()) + ss.str();
}

CGUIElement_Impl::~CGUIElement_Impl()
{
    m_deleted = true;
    m_pManager->OnElementDestroy(this);
}

void CGUIElement_Impl::SetParent(CGUIElement* parent)
{
    if (parent == m_pParent)
        return;

    if (m_pParent)
    {
        m_pParent->RemoveChild(this);
        m_pParent = nullptr;
    }

    m_pParent = parent;

    if (parent)
    {
        parent->AddChild(this);

        SetPosition(m_initialPosition);
        SetSize(m_initialSize);
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
    m_children.remove(child);
}

std::list<CGUIElement*> CGUIElement_Impl::GetChildren()
{
    return m_children;
}

bool CGUIElement_Impl::IsDeleted()
{
    return m_deleted;
}

void CGUIElement_Impl::Begin()
{
    const char* id = (m_title + "###" + m_uid).c_str();

    if (m_pParent == nullptr)
    {
        ProcessPosition();
        ProcessSize();

        ImGui::Begin(id);
    }
    else
    {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + m_position.fX);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_position.fY);

        if (!ImGui::BeginChild(id, ImVec2(m_size.fX, m_size.fY), m_frame))
            return;

        ImGui::Text(m_title.c_str());
    }
}

void CGUIElement_Impl::End()
{
    if (m_pParent == nullptr)
        ImGui::End();
    else
        ImGui::EndChild();

    m_initialized = true;
}

std::string CGUIElement_Impl::GetType()
{
    return m_type;
}

void CGUIElement_Impl::SetPosition(CVector2D pos)
{
    m_position = pos;
    m_updatePosition = true;
}

void CGUIElement_Impl::SetSize(CVector2D size)
{
    m_size = size;
    m_updateSize = true;
}

void CGUIElement_Impl::ProcessPosition()
{
    if (!m_initialized)
    {
        ImGui::SetNextWindowPos(ImVec2(m_initialPosition.fX, m_initialPosition.fY), ImGuiCond_Always);
        return;
    }

    if (m_updatePosition)
        ImGui::SetNextWindowPos(ImVec2(m_position.fX, m_position.fY));

    m_updatePosition = false;
}

void CGUIElement_Impl::ProcessSize()
{
    if (!m_initialized)
    {
        ImGui::SetNextWindowSize(ImVec2(m_initialSize.fX, m_initialSize.fY), ImGuiCond_Always);
        return;
    }

    if (m_updateSize)
        ImGui::SetNextWindowSize(ImVec2(m_size.fX, m_size.fY));

    m_updateSize = false;
}

void CGUIElement_Impl::SetFrameEnabled(bool state)
{
    m_frame = state;
}

bool CGUIElement_Impl::GetFrameEnabled()
{
    return m_frame;
}
