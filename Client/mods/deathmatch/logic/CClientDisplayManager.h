/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientDisplayManager.cpp
 *  PURPOSE:     Text display manager class
 *
 *****************************************************************************/

class CClientDisplayManager;

#pragma once

#include "CClientManager.h"

#include "CClientDisplay.h"
#include "CClientVectorGraphicDisplay.h"
#include "CClientTextDisplay.h"

#include <list>

class CClientDisplayManager
{
    friend class CClientManager;
    friend class CClientDisplay;

public:
    CClientDisplayManager() = default;
    ~CClientDisplayManager() = default;

    void DoPulse();

    unsigned int    Count() { return static_cast<unsigned int>(m_List.size()); };
    std::shared_ptr<CClientDisplay> Get(unsigned long ulID);

    void DrawText2D(const char* szCaption, const CVector& vecPosition, float fScale = 1.0f, RGBA rgbaColor = 0xFFFFFFFF);

    void AddToList(const std::shared_ptr<CClientDisplay>& display);

    std::shared_ptr<CClientVectorGraphicDisplay> CreateVectorGraphicDisplay(CClientVectorGraphic* svg);
    std::shared_ptr<CClientTextDisplay>          CreateTextDisplay(int ID = 0xFFFFFFFF);

    std::list<std::weak_ptr<CClientDisplay>> m_List;
};

