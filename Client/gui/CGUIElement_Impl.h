/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIElement_Impl.h
 *  PURPOSE:     Element (widget) base class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUIElement.h>

#include <list>

namespace CEGUI
{
    class Window;
    class EventArgs;
}            // namespace CEGUI
class CGUI_Impl;

class CGUIElement_Impl : public CGUIElement
{
public:
    CGUIElement_Impl();

    void SetVisible(bool bVisible);
    bool IsVisible();

    void SetEnabled(bool bEnabled);
    bool IsEnabled();
    void SetZOrderingEnabled(bool bZOrderingEnabled);
    bool IsZOrderingEnabled();

    void BringToFront();
    void MoveToBack();

    void      SetPosition(const CVector2D& vecPosition, bool bRelative = false);
    CVector2D GetPosition(bool bRelative = false);
    void      GetPosition(CVector2D& vecPosition, bool bRelative = false);

    void SetWidth(float fX, bool bRelative = false);
    void SetHeight(float fY, bool bRelative = false);

    void      SetSize(const CVector2D& vecSize, bool bRelative = false);
    CVector2D GetSize(bool bRelative = false);
    void      GetSize(CVector2D& vecSize, bool bRelative = false);

    void AutoSize(const char* Text = NULL, float fPaddingX = 0.0f, float fPaddingY = 2.0f);

    bool        SetFont(const char* szFontName);
    std::string GetFont();

    void        SetProperty(const char* szProperty, const char* szValue);
    std::string GetProperty(const char* szProperty);

    void      SetMinimumSize(const CVector2D& vecSize);
    CVector2D GetMinimumSize();
    void      GetMinimumSize(CVector2D& vecSize);

    void      SetMaximumSize(const CVector2D& vecSize);
    CVector2D GetMaximumSize();
    void      GetMaximumSize(CVector2D& vecSize);

    void        SetText(const char* Text);
    std::string GetText();

    void  SetAlpha(float fAlpha);
    float GetAlpha();
    float GetEffectiveAlpha();
    void  SetInheritsAlpha(bool bInheritsAlpha);
    bool  GetInheritsAlpha();

    void Activate();
    void Deactivate();
    bool IsActive();

    void SetAlwaysOnTop(bool bAlwaysOnTop);
    bool IsAlwaysOnTop();

    CRect2D   AbsoluteToRelative(const CRect2D& Rect);
    CVector2D AbsoluteToRelative(const CVector2D& Vector);

    CRect2D   RelativeToAbsolute(const CRect2D& Rect);
    CVector2D RelativeToAbsolute(const CVector2D& Vector);

    void         SetParent(CGUIElement* pParent);
    CGUIElement* GetParent();

    CEGUI::Window* GetWindow();

    void ForceRedraw();

    void  SetUserData(void* pData) { m_pData = pData; }
    void* GetUserData() { return m_pData; }

    CGUIPropertyIter GetPropertiesBegin();
    CGUIPropertyIter GetPropertiesEnd();

    void SetMovedHandler(GUI_CALLBACK Callback);
    void SetSizedHandler(GUI_CALLBACK Callback);
    void SetClickHandler(GUI_CALLBACK Callback);
    void SetClickHandler(const GUI_CALLBACK_MOUSE& Callback);
    void SetDoubleClickHandler(GUI_CALLBACK Callback);
    void SetMouseEnterHandler(GUI_CALLBACK Callback);
    void SetMouseLeaveHandler(GUI_CALLBACK Callback);
    void SetMouseButtonDownHandler(GUI_CALLBACK Callback);
    void SetActivateHandler(GUI_CALLBACK Callback);
    void SetDeactivateHandler(GUI_CALLBACK Callback);
    void SetKeyDownHandler(GUI_CALLBACK Callback);
    void SetKeyDownHandler(const GUI_CALLBACK_KEY& Callback);
    void SetEnterKeyHandler(GUI_CALLBACK Callback);

    bool Event_OnClick(const CEGUI::EventArgs& e);
    bool Event_OnDoubleClick();
    bool Event_OnMouseEnter();
    bool Event_OnMouseLeave();
    bool Event_OnMouseButtonDown();

protected:
    void DestroyElement();

    void CorrectEdges();
    void FillProperties();
    void EmptyProperties();

    void AddEvents();

    bool Event_OnMoved(const CEGUI::EventArgs& e);
    bool Event_OnSized(const CEGUI::EventArgs& e);
    bool Event_OnActivated(const CEGUI::EventArgs& e);
    bool Event_OnDeactivated(const CEGUI::EventArgs& e);
    bool Event_OnKeyDown(const CEGUI::EventArgs& e);

    CEGUI::Window* m_pWindow;
    CGUIElement*   m_pParent;

    CGUI_Impl* m_pManager;

    void* m_pData;

    std::list<CGUIProperty*> m_Properties;

    GUI_CALLBACK m_OnClick;
    GUI_CALLBACK m_OnDoubleClick;
    GUI_CALLBACK m_OnMoved;
    GUI_CALLBACK m_OnSized;
    GUI_CALLBACK m_OnMouseEnter;
    GUI_CALLBACK m_OnMouseLeave;
    GUI_CALLBACK m_OnMouseDown;
    GUI_CALLBACK m_OnActivate;
    GUI_CALLBACK m_OnDeactivate;
    GUI_CALLBACK m_OnKeyDown;
    GUI_CALLBACK m_OnEnter;

    GUI_CALLBACK_MOUSE m_OnClickWithArgs;
    GUI_CALLBACK_KEY   m_OnKeyDownWithArgs;
};
