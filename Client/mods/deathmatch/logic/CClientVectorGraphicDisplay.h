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
    CClientVectorGraphicDisplay(CClientDisplayManager* pDisplayManager, CClientVectorGraphic* pVectorGraphic, int ID = 0xFFFFFFFF);
    ~CClientVectorGraphicDisplay();

    eDisplayType GetType() { return DISPLAY_VECTORGRAPHIC; }

    void SetPosition(const CVector& vecPosition);

    void SetColor(const SColor color);
    void SetColorAlpha(unsigned char ucAlpha);
    void SetShadowAlpha(unsigned char ucShadowAlpha);

    float GetScale() { return m_fScale; };
    void  SetScale(float fScale);

    unsigned long GetFormat() { return m_ulFormat; };
    void          SetFormat(unsigned long ulFormat);

    void SetVisible(bool bVisible);

    void Render();

    void UpdateTexture();
    void ClearTexture();

    static void SetGlobalScale(float fScale) { m_fGlobalScale = fScale; }

    const bool IsCleared() { return m_bIsCleared; }
    const bool HasUpdated() { return m_bHasUpdated; }

    void Update();
private:
    CClientVectorGraphic* m_pVectorGraphic;
    SString m_strCaption;
    float   m_fScale;

    unsigned long m_ulFormat;
    unsigned char m_ucShadowAlpha;

    static float m_fGlobalScale;

    bool m_bIsCleared;
    bool m_bHasUpdated;
};
