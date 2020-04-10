/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CCredits.h
 *  PURPOSE:     Header file for credits class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CCredits;

#pragma once

#include "CMainMenu.h"
#include <ctime>

class CCredits
{
public:
    CCredits();
    ~CCredits();

    void SetVisible(bool bVisible);
    bool IsVisible();

    void Update();

    bool OnOKButtonClick(CGUIElement* pElement);

private:
    CGUIWindow* m_pWindow;
    CGUILabel*  m_pLabels[30];
    CGUIButton* m_pButtonOK;
    SString     m_strCredits;

    clock_t m_clkStart;
};
