/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CClientVectorGraphicDisplay;

#pragma once

#include "CClientDisplay.h"
#include "CClientDisplayManager.h"

class CClientVectorGraphicDisplay : public CClientDisplay
{
    friend class CClientDisplayManager;

public:
    CClientVectorGraphicDisplay(CClientDisplayManager* pDisplayManager, CClientVectorGraphic* pVectorGraphic, int ID = DISPLAY_VECTORGRAPHIC);
    ~CClientVectorGraphicDisplay() = default;

    eDisplayType GetType() { return DISPLAY_VECTORGRAPHIC; }

    void Render();

    void UpdateTexture();
    void ClearTexture();

    const bool IsCleared() { return m_bIsCleared; }
    const bool HasUpdated() { return m_bHasUpdated; }

    void Update();
private:
    CClientVectorGraphic* m_pVectorGraphic;

    bool m_bIsCleared;
    bool m_bHasUpdated;
};
