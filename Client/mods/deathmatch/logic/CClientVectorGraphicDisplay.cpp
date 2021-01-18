/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientVectorGraphicDisplay.cpp
 *  PURPOSE:     Client vector graphic display base class
 *
 *****************************************************************************/

#include "StdInc.h"

using std::list;

float CClientVectorGraphicDisplay::m_fGlobalScale = 1.0f;

CClientVectorGraphicDisplay::CClientVectorGraphicDisplay(CClientDisplayManager* pDisplayManager, CClientVectorGraphic* pVectorGraphic, int ID) : CClientDisplay(pDisplayManager, ID)
{
    // Init
    m_pVectorGraphic = pVectorGraphic;
    m_fScale = 1;
    m_ulFormat = 0;
    m_bVisible = true;
    m_bIsCleared = false;
}

CClientVectorGraphicDisplay::~CClientVectorGraphicDisplay()
{
}

void CClientVectorGraphicDisplay::SetPosition(const CVector& vecPosition)
{
    m_vecPosition = vecPosition;
}

void CClientVectorGraphicDisplay::Render()
{
    if (!m_bVisible)
    {
        if (!IsCleared())
        {
            m_pVectorGraphic->ClearTexture();
            m_bIsCleared = true;
        }
            
        return;
    }
        
    if (IsCleared())
        m_bIsCleared = false;

    m_pVectorGraphic->UpdateTexture();
}

void CClientVectorGraphicDisplay::SetColor(const SColor color)
{
    m_Color = color;
}

void CClientVectorGraphicDisplay::SetColorAlpha(unsigned char ucAlpha)
{
    m_Color.A = ucAlpha;
}

void CClientVectorGraphicDisplay::SetShadowAlpha(unsigned char ucShadowAlpha)
{
    m_ucShadowAlpha = ucShadowAlpha;
}

void CClientVectorGraphicDisplay::SetScale(float fScale)
{
    m_fScale = fScale;
}

void CClientVectorGraphicDisplay::SetFormat(unsigned long ulFormat)
{
    m_ulFormat = ulFormat;
}

void CClientVectorGraphicDisplay::SetVisible(bool bVisible)
{
    CClientDisplay::SetVisible(bVisible);
}
