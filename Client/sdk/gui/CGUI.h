/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CGUIBrowser.h"
#include "CGUIButton.h"
#include "CGUICheckbox.h"
#include "CGUICombobox.h"
#include "CGUIEdit.h"
#include "CGUIElement.h"
#include "CGUIGridList.h"
#include "CGUILabel.h"
#include "CGUIMemo.h"
#include "CGUIProgressBar.h"
#include "CGUIRadioButton.h"
#include "CGUIScrollbar.h"
#include "CGUIScrollpane.h"
#include "CGUIStaticImage.h"
#include "CGUITabPanel.h"
#include "CGUITexture.h"
#include "CGUIWindow.h"

#include <../Shared/sdk/CRect2D.h>
#include <../Shared/sdk/CVector2D.h>
#include <../Shared/sdk/CColor.h>

#undef CreateWindow            // mAcrosoft strikes again

class CGraphicsInterface;

class CGUI
{
public:
    virtual void Draw() = 0;
    virtual void Draw(CGUIElement* element) = 0;

    virtual void Invalidate() = 0;
    virtual void Restore() = 0;

    virtual void SetHookedWindow(HWND window) = 0;
    virtual HWND GetHookedWindow() const = 0;

    virtual void                SetGraphicsInterface(CGraphicsInterface* graphics) = 0;
    virtual CGraphicsInterface* GetGraphicsInterface() = 0;

    virtual bool ProcessWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;

    virtual void SetElementIndex(CGUIElement* element, int index) = 0;
    virtual int  GetElementIndex(CGUIElement* element) = 0;

    virtual CVector2D GetResolution() = 0;

    virtual CGUIBrowser*     CreateBrowser(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false) = 0;
    virtual CGUIButton*      CreateButton(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false) = 0;
    virtual CGUICheckbox*    CreateCheckbox(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false) = 0;
    virtual CGUICombobox*    CreateCombobox(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false) = 0;
    virtual CGUIEdit*        CreateEdit(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false) = 0;
    virtual CGUIGridList*    CreateGridList(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false) = 0;
    virtual CGUILabel*       CreateLabel(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false) = 0;
    virtual CGUIMemo*        CreateMemo(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false) = 0;
    virtual CGUIProgressBar* CreateProgressBar(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false) = 0;
    virtual CGUIRadioButton* CreateRadioButton(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false) = 0;
    virtual CGUIScrollbar*   CreateScrollbar(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false) = 0;
    virtual CGUIScrollpane*  CreateScrollpane(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false) = 0;
    virtual CGUIStaticImage* CreateStaticImage(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false) = 0;
    virtual CGUITabPanel*    CreateTabPanel(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false) = 0;
    virtual CGUITexture*     CreateTexture(CVector2D size, bool relative = false) = 0;
    virtual CGUIWindow* CreateWindow(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false, std::string title = "Window") = 0;

    virtual void CreateDemo() = 0;
};
