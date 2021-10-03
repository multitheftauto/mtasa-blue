/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CGUIButton.h"
#include "CGUICheckbox.h"
#include "CGUIEdit.h"
#include "CGUIElement.h"
#include "CGUILabel.h"
#include "CGUIMemo.h"
#include "CGUIProgressBar.h"
#include "CGUIRadioButton.h"
#include "CGUIScrollbar.h"
#include "CGUIStaticImage.h"
#include "CGUIWindow.h"

#include <../Shared/sdk/CRect2D.h>
#include <../Shared/sdk/CVector2D.h>

#undef CreateWindow            // mAcrosoft strikes again

class CGUI
{
public:
    virtual void Draw() = 0;
    virtual void Draw(CGUIElement* element) = 0;

    virtual void Invalidate() = 0;
    virtual void Restore() = 0;

    virtual void SetHookedWindow(HWND window) = 0;
    virtual HWND GetHookedWindow() const = 0;

    virtual bool             ProcessWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;
    virtual CGUIWindow*      CreateWindow(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative, std::string title) = 0;
    virtual CGUIButton*      CreateButton(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false) = 0;
    virtual CGUICheckbox*    CreateCheckbox(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false) = 0;
    virtual CGUIEdit*        CreateEdit(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false) = 0;
    virtual CGUILabel*       CreateLabel(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false) = 0;
    virtual CGUIMemo*        CreateMemo(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false) = 0;
    virtual CGUIProgressBar* CreateProgressBar(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false) = 0;
    virtual CGUIRadioButton* CreateRadioButton(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false) = 0;
    virtual CGUIScrollbar*   CreateScrollbar(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false) = 0;
    virtual CGUIStaticImage* CreateStaticImage(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false) = 0;

    virtual void CreateDemo() = 0;
};
