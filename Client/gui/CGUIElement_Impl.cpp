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

    SetParent(pParent);
    SetPosition(pos, relative);
    SetSize(size, relative);
}

CGUIElement_Impl::~CGUIElement_Impl()
{
    m_deleted = true;
    m_pManager->OnElementDestroy(this);
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
        if (!ImGui::BeginChild(id, ImVec2(m_size.fX, m_size.fY), m_hasFrame))
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
