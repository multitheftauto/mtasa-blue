/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/gui/CGUIElement.h
 *  PURPOSE:     Base element (widget) interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

class CGUIElement;

#pragma once

#include "CGUIEvent.h"
#include "CGUICallback.h"
#include <string>
#include "CGUITypes.h"

// Forward declaration
namespace CEGUI
{
    class EventArgs;
}

enum eCGUIType
{
    CGUI_BUTTON,
    CGUI_CHECKBOX,
    CGUI_EDIT,
    CGUI_GRIDLIST,
    CGUI_LABEL,
    CGUI_MEMO,
    CGUI_PROGRESSBAR,
    CGUI_RADIOBUTTON,
    CGUI_STATICIMAGE,
    CGUI_TAB,
    CGUI_TABPANEL,
    CGUI_WINDOW,
    CGUI_SCROLLPANE,
    CGUI_SCROLLBAR,
    CGUI_COMBOBOX,
    CGUI_WEBBROWSER
};

class CGUIElement
{
public:
    virtual ~CGUIElement(){};

    virtual void SetVisible(bool bVisible) = 0;
    virtual bool IsVisible() = 0;

    virtual void SetEnabled(bool bEnabled) = 0;
    virtual bool IsEnabled() = 0;
    virtual void SetZOrderingEnabled(bool bZOrderingEnabled) = 0;
    virtual bool IsZOrderingEnabled() = 0;

    virtual void BringToFront() = 0;
    virtual void MoveToBack() = 0;

    virtual void      SetPosition(const CVector2D& vecPosition, bool bRelative = false) = 0;
    virtual CVector2D GetPosition(bool bRelative = false) = 0;
    virtual void      GetPosition(CVector2D& vecPosition, bool bRelative = false) = 0;

    virtual void SetWidth(float fX, bool bRelative = false) = 0;
    virtual void SetHeight(float fY, bool bRelative = false) = 0;

    virtual void      SetSize(const CVector2D& vecSize, bool bRelative = false) = 0;
    virtual CVector2D GetSize(bool bRelative = false) = 0;
    virtual void      GetSize(CVector2D& vecSize, bool bRelative = false) = 0;

    virtual void AutoSize(const char* Text = NULL, float fPaddingX = 0.0f, float fPaddingY = 2.0f) = 0;

    virtual void        SetProperty(const char* szProperty, const char* szValue) = 0;
    virtual std::string GetProperty(const char* szProperty) = 0;

    virtual void      SetMinimumSize(const CVector2D& vecSize) = 0;
    virtual CVector2D GetMinimumSize() = 0;
    virtual void      GetMinimumSize(CVector2D& vecSize) = 0;

    virtual void      SetMaximumSize(const CVector2D& vecSize) = 0;
    virtual CVector2D GetMaximumSize() = 0;
    virtual void      GetMaximumSize(CVector2D& vecSize) = 0;

    virtual void        SetText(const char* Text) = 0;
    virtual std::string GetText() = 0;

    virtual bool        SetFont(const char* szFontName) = 0;
    virtual std::string GetFont() = 0;

    virtual void  SetAlpha(float fAlpha) = 0;
    virtual float GetAlpha() = 0;
    virtual float GetEffectiveAlpha() = 0;
    virtual void  SetInheritsAlpha(bool bInheritsAlpha) = 0;
    virtual bool  GetInheritsAlpha() = 0;

    virtual void Activate() = 0;
    virtual void Deactivate() = 0;
    virtual bool IsActive() = 0;

    virtual void SetAlwaysOnTop(bool bAlwaysOnTop) = 0;
    virtual bool IsAlwaysOnTop() = 0;

    virtual void ForceRedraw() = 0;

    virtual CRect2D   AbsoluteToRelative(const CRect2D& Rect) = 0;
    virtual CVector2D AbsoluteToRelative(const CVector2D& Vector) = 0;

    virtual CRect2D   RelativeToAbsolute(const CRect2D& Rect) = 0;
    virtual CVector2D RelativeToAbsolute(const CVector2D& Vector) = 0;

    virtual void         SetParent(CGUIElement* pParent) = 0;
    virtual CGUIElement* GetParent() = 0;

    virtual void  SetUserData(void* pData) = 0;
    virtual void* GetUserData() = 0;

    virtual eCGUIType GetType() = 0;

    virtual CGUIPropertyIter GetPropertiesBegin() = 0;
    virtual CGUIPropertyIter GetPropertiesEnd() = 0;

    virtual void SetMovedHandler(GUI_CALLBACK Callback) = 0;
    virtual void SetSizedHandler(GUI_CALLBACK Callback) = 0;
    virtual void SetClickHandler(GUI_CALLBACK Callback) = 0;
    virtual void SetClickHandler(const GUI_CALLBACK_MOUSE& Callback) = 0;
    virtual void SetDoubleClickHandler(GUI_CALLBACK Callback) = 0;
    virtual void SetMouseEnterHandler(GUI_CALLBACK Callback) = 0;
    virtual void SetMouseLeaveHandler(GUI_CALLBACK Callback) = 0;
    virtual void SetMouseButtonDownHandler(GUI_CALLBACK Callback) = 0;
    virtual void SetActivateHandler(GUI_CALLBACK Callback) = 0;
    virtual void SetDeactivateHandler(GUI_CALLBACK Callback) = 0;
    virtual void SetKeyDownHandler(GUI_CALLBACK Callback) = 0;
    virtual void SetKeyDownHandler(const GUI_CALLBACK_KEY& Callback) = 0;
    virtual void SetEnterKeyHandler(GUI_CALLBACK Callback) = 0;

    virtual bool Event_OnClick(const CEGUI::EventArgs& e) = 0;
    virtual bool Event_OnDoubleClick() = 0;
    virtual bool Event_OnMouseEnter() = 0;
    virtual bool Event_OnMouseLeave() = 0;
    virtual bool Event_OnMouseButtonDown() = 0;
};
