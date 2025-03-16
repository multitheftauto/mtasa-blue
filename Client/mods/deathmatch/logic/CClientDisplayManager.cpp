/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientDisplayManager.cpp
 *  PURPOSE:     Text display manager class
 *
 *****************************************************************************/

#include "StdInc.h"

using std::list;

std::shared_ptr<CClientDisplay> CClientDisplayManager::Get(unsigned long ulID)
{
    // Find the display with the given id
    auto iter = m_List.begin();

    for (; iter != m_List.end(); iter++) // Iterate weak_ptr list
    {
        if (const auto& display = (*iter).lock())  // Make sure the shared_ptr still exists
        {
            if (display->GetID() == ulID)
            {
                return display;
            }
        }

    }

    return NULL;
}

void CClientDisplayManager::DrawText2D(const char* szCaption, const CVector& vecPosition, float fScale, RGBA rgbaColor)
{
    // Grab the resolution as a float
    float fResWidth = static_cast<float>(g_pCore->GetGraphics()->GetViewportWidth());
    float fResHeight = static_cast<float>(g_pCore->GetGraphics()->GetViewportHeight());

    // Render
    g_pCore->GetGraphics()->DrawString(static_cast<int>(vecPosition.fX * fResWidth), static_cast<int>(vecPosition.fY * fResHeight), static_cast<int>(fResWidth),
                                       static_cast<int>(fResHeight), rgbaColor, szCaption, fScale, fScale, 0);
}

void CClientDisplayManager::AddToList(const std::shared_ptr<CClientDisplay>& display)
{
    m_List.push_back(display);
}

void CClientDisplayManager::DoPulse()
{
    // Render all our displays
    auto iter = m_List.begin();

    // Clean up expired weak_ptr
    m_List.remove_if([](const std::weak_ptr<CClientDisplay>& wp) { return wp.expired(); });

    for (; iter != m_List.end(); iter++) // Iterate weak_ptr list
    {
        if (const auto& display = (*iter).lock())            // Make sure the shared_ptr still exists
        {
            display->Render();
        }
    }
}

std::shared_ptr<CClientVectorGraphicDisplay> CClientDisplayManager::CreateVectorGraphicDisplay(CClientVectorGraphic* svg)
{
    auto display = std::make_shared<CClientVectorGraphicDisplay>(svg);
    AddToList(display);
    return display;
}

std::shared_ptr<CClientTextDisplay> CClientDisplayManager::CreateTextDisplay(int ID)
{
    auto display = std::make_shared<CClientTextDisplay>(ID);
    AddToList(display);
    return display;
}
