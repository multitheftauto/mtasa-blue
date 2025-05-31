/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIMessageBox_Impl.h
 *  PURPOSE:     Message box class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUIMessageBox.h>
#include "CGUIButton_Impl.h"
#include "CGUILabel_Impl.h"
#include "CGUIStaticImage_Impl.h"
#include "CGUIWindow_Impl.h"

class CGUIMessageBox_Impl : public CGUIMessageBox
{
public:
    CGUIMessageBox_Impl(class CGUI_Impl* pGUI, const char* szTitle, const char* szCaption, unsigned int uiFlags);
    ~CGUIMessageBox_Impl();

    void SetAutoDestroy(bool bAutoDestroy);
    bool GetAutoDestroy();

    void SetVisible(bool bVisible);
    bool IsVisible();

    void SetClickHandler(GUI_CALLBACK Callback);

    CGUIWindow* GetWindow();

private:
    bool OnResponseHandler(CGUIElement* pElement);

    CGUIWindow*      m_pWindow;
    CGUIStaticImage* m_pIcon;
    CGUILabel*       m_pLabelCaption;
    CGUIButton*      m_pButton;

    bool m_bAutoDestroy;
};
