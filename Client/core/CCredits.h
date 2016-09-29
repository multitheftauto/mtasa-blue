/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CCredits.h
*  PURPOSE:     Header file for credits class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CCredits;

#ifndef __CCREDITS_H
#define __CCREDITS_H

#include "CMainMenu.h"
#include <ctime>

class CCredits
{
public:
                    CCredits            ( void );

    void            SetVisible          ( bool bVisible );
    bool            IsVisible           ( void );

    void            Update              ( void );

    bool            OnOKButtonClick     ( CGUIElement* pElement );

private:
    std::unique_ptr<CGUIWindow> m_pWindow;
    std::unique_ptr<CGUILabel>  m_pLabels [30];
    std::unique_ptr<CGUIButton> m_pButtonOK;
    SString                 m_strCredits;

    clock_t                 m_clkStart;
};

#endif
