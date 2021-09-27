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

    SetParent(pParent);

    SetPosition(pos, relative);
    SetSize(size, relative);

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

    ProcessPosition();

    if (m_pParent == nullptr)
    {
        ProcessSize();

        ImGui::Begin(id);
    }
    else
    {
        if (!ImGui::BeginChild(id, ImVec2(m_size.fX, m_size.fY), m_frame))
            return;

        ImGui::Text(m_title.c_str());
    }
}

void CGUIElement_Impl::End()
{
    m_position = CVector2D(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
    m_size = CVector2D(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);

    if (m_pParent == nullptr)
        ImGui::End();
    else
        ImGui::EndChild();
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

    m_offsetPosition = pos;
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
    if (m_updatePosition)
    {
        ImGui::SetNextWindowPos(ImVec2(m_position.fX, m_position.fY));
    }

    if (m_pParent)
    {
        CVector2D parentOffset = m_pParent->GetOffset();

        ImGui::SetCursorPosX(m_offsetPosition.fX + parentOffset.fX);
        ImGui::SetCursorPosY(m_offsetPosition.fY + parentOffset.fY);
    }

    m_updatePosition = false;
}

void CGUIElement_Impl::ProcessSize()
{
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

CVector2D CGUIElement_Impl::GetOffset()
{
    return {};
}
