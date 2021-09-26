/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CGUIElement_Impl::CGUIElement_Impl()
{
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

bool CGUIElement_Impl::IsDeleted()
{
    return m_deleted;
}

void CGUIElement_Impl::Begin()
{
    const char* id = (m_title + "###" + m_uid).c_str();
    ImGui::Begin(id);
}

void CGUIElement_Impl::End()
{
    ImGui::End();
    m_initialized = true;
}

void CGUIElement_Impl::SetPosition(CVector2D pos)
{
    m_pendingPosition = pos;
    m_updatePosition = true;
}

void CGUIElement_Impl::SetSize(CVector2D size)
{
    m_pendingSize = size;
    m_updateSize = true;
}

CVector2D CGUIElement_Impl::GetInitialPosition() const
{
    return m_initialPosition;
}

CVector2D CGUIElement_Impl::GetInitialSize() const
{
    return m_initialSize;
}

void CGUIElement_Impl::ProcessPosition()
{
    if (!m_initialized)
        return;

    if (m_updatePosition)
        ImGui::SetNextWindowPos(ImVec2(m_pendingPosition.fX, m_pendingPosition.fY));

    m_updatePosition = false;
}

void CGUIElement_Impl::ProcessSize()
{
    if (!m_initialized)
        return;

    if (m_updateSize)
        ImGui::SetNextWindowSize(ImVec2(m_pendingSize.fX, m_pendingSize.fY));

    m_updateSize = false;
}
