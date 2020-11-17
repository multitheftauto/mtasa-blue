/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIElement_Inc.h
 *  PURPOSE:     Element derived class inclusion header
 *               (fixes C++-language issues regarding inheritance)
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

void SetProperty(const char* szProperty, const char* szValue)
{
    GUINew::CGUIElement_Impl::SetProperty(szProperty, szValue);
};
std::string GetProperty(const char* szProperty)
{
    return GUINew::CGUIElement_Impl::GetProperty(szProperty);
};

CGUIPropertyIter GetPropertiesBegin()
{
    return GUINew::CGUIElement_Impl::GetPropertiesBegin();
};
CGUIPropertyIter GetPropertiesEnd()
{
    return GUINew::CGUIElement_Impl::GetPropertiesEnd();
};

bool SetFont(const char* szFontName)
{
    return GUINew::CGUIElement_Impl::SetFont(szFontName);
};
std::string GetFont()
{
    return GUINew::CGUIElement_Impl::GetFont();
};

#ifndef SETVISIBLE_HACK
void SetVisible(bool bVisible)
{
    GUINew::CGUIElement_Impl::SetVisible(bVisible);
};
bool IsVisible()
{
    return GUINew::CGUIElement_Impl::IsVisible();
};
#endif

#ifndef SETENABLED_HACK
void SetEnabled(bool bEnabled)
{
    GUINew::CGUIElement_Impl::SetEnabled(bEnabled);
};
bool IsEnabled()
{
    return GUINew::CGUIElement_Impl::IsEnabled();
};
#endif
void SetZOrderingEnabled(bool bZOrderingEnabled)
{
    GUINew::CGUIElement_Impl::SetZOrderingEnabled(bZOrderingEnabled);
};
bool IsZOrderingEnabled()
{
    return GUINew::CGUIElement_Impl::IsZOrderingEnabled();
};

void BringToFront()
{
    GUINew::CGUIElement_Impl::BringToFront();
};
void MoveToBack()
{
    GUINew::CGUIElement_Impl::MoveToBack();
};

void SetPosition(const CVector2D& vecPosition, bool bRelative = false)
{
    GUINew::CGUIElement_Impl::SetPosition(vecPosition, bRelative);
};
CVector2D GetPosition(bool bRelative = false)
{
    return GUINew::CGUIElement_Impl::GetPosition(bRelative);
};
void GetPosition(CVector2D& vecPosition, bool bRelative = false)
{
    GUINew::CGUIElement_Impl::GetPosition(vecPosition, bRelative);
};

void SetWidth(float fX, bool bRelative = false)
{
    GUINew::CGUIElement_Impl::SetWidth(fX, bRelative);
};
void SetHeight(float fY, bool bRelative = false)
{
    GUINew::CGUIElement_Impl::SetHeight(fY, bRelative);
};

#ifndef EXCLUDE_SET_SIZE // WTF? TODO: Refactor this
void SetSize(const CVector2D& vecSize, bool bRelative = false)
{
    GUINew::CGUIElement_Impl::SetSize(vecSize, bRelative);
};
#endif
CVector2D GetSize(bool bRelative = false)
{
    return GUINew::CGUIElement_Impl::GetSize(bRelative);
};
void GetSize(CVector2D& vecSize, bool bRelative = false)
{
    GUINew::CGUIElement_Impl::GetSize(vecSize, bRelative);
};

void AutoSize(const char* Text = NULL, float fPaddingX = 0.0f, float fPaddingY = 2.0f)
{
    GUINew::CGUIElement_Impl::AutoSize(Text, fPaddingX, fPaddingY);
};

void SetMinimumSize(const CVector2D& vecSize)
{
    GUINew::CGUIElement_Impl::SetMinimumSize(vecSize);
};
CVector2D GetMinimumSize()
{
    return GUINew::CGUIElement_Impl::GetMinimumSize();
};
void GetMinimumSize(CVector2D& vecSize)
{
    GUINew::CGUIElement_Impl::GetMinimumSize(vecSize);
};

void SetMaximumSize(const CVector2D& vecSize)
{
    GUINew::CGUIElement_Impl::SetMaximumSize(vecSize);
};
CVector2D GetMaximumSize()
{
    return GUINew::CGUIElement_Impl::GetMaximumSize();
};
void GetMaximumSize(CVector2D& vecSize)
{
    GUINew::CGUIElement_Impl::GetMaximumSize(vecSize);
};

#ifndef EXCLUDE_SET_TEXT
void SetText(const char* Text)
{
    GUINew::CGUIElement_Impl::SetText(Text);
};
#endif

std::string GetText()
{
    return GUINew::CGUIElement_Impl::GetText();
};

void SetAlpha(float fAlpha)
{
    GUINew::CGUIElement_Impl::SetAlpha(fAlpha);
};
float GetAlpha()
{
    return GUINew::CGUIElement_Impl::GetAlpha();
};
float GetEffectiveAlpha()
{
    return GUINew::CGUIElement_Impl::GetEffectiveAlpha();
};
void SetInheritsAlpha(bool bInheritsAlpha)
{
    GUINew::CGUIElement_Impl::SetInheritsAlpha(bInheritsAlpha);
};
bool GetInheritsAlpha()
{
    return GUINew::CGUIElement_Impl::GetInheritsAlpha();
};

void Activate()
{
    GUINew::CGUIElement_Impl::Activate();
};
void Deactivate()
{
    GUINew::CGUIElement_Impl::Deactivate();
};
bool IsActive()
{
    return GUINew::CGUIElement_Impl::IsActive();
};

void ForceRedraw()
{
    GUINew::CGUIElement_Impl::ForceRedraw();
};
void SetAlwaysOnTop(bool bAlwaysOnTop)
{
    GUINew::CGUIElement_Impl::SetAlwaysOnTop(bAlwaysOnTop);
};
bool IsAlwaysOnTop()
{
    return GUINew::CGUIElement_Impl::IsAlwaysOnTop();
};

CRect2D AbsoluteToRelative(const CRect2D& Rect)
{
    return CGUIElement_Impl::AbsoluteToRelative(Rect);
};
CVector2D AbsoluteToRelative(const CVector2D& Vector)
{
    return CGUIElement_Impl::AbsoluteToRelative(Vector);
};

CRect2D RelativeToAbsolute(const CRect2D& Rect)
{
    return CGUIElement_Impl::RelativeToAbsolute(Rect);
};
CVector2D RelativeToAbsolute(const CVector2D& Vector)
{
    return CGUIElement_Impl::RelativeToAbsolute(Vector);
};

void SetParent(CGUIElement* pParent)
{
    GUINew::CGUIElement_Impl::SetParent(pParent);
};
CGUIElement* GetParent()
{
    return GUINew::CGUIElement_Impl::GetParent();
};

CEGUI::Window* GetWindow()
{
    return GUINew::CGUIElement_Impl::GetWindow();
};

void SetUserData(void* pData)
{
    GUINew::CGUIElement_Impl::SetUserData(pData);
};
void* GetUserData()
{
    return GUINew::CGUIElement_Impl::GetUserData();
};

void SetClickHandler(GUI_CALLBACK Callback)
{
    GUINew::CGUIElement_Impl::SetClickHandler(Callback);
};
void SetClickHandler(const GUI_CALLBACK_MOUSE& Callback)
{
    GUINew::CGUIElement_Impl::SetClickHandler(Callback);
};
void SetDoubleClickHandler(GUI_CALLBACK Callback)
{
    GUINew::CGUIElement_Impl::SetDoubleClickHandler(Callback);
};
void SetMovedHandler(GUI_CALLBACK Callback)
{
    GUINew::CGUIElement_Impl::SetMovedHandler(Callback);
};
void SetSizedHandler(GUI_CALLBACK Callback)
{
    GUINew::CGUIElement_Impl::SetSizedHandler(Callback);
};
void SetMouseEnterHandler(GUI_CALLBACK Callback)
{
    GUINew::CGUIElement_Impl::SetMouseEnterHandler(Callback);
};
void SetMouseLeaveHandler(GUI_CALLBACK Callback)
{
    GUINew::CGUIElement_Impl::SetMouseLeaveHandler(Callback);
};
void SetMouseButtonDownHandler(GUI_CALLBACK Callback)
{
    GUINew::CGUIElement_Impl::SetMouseButtonDownHandler(Callback);
};
void SetActivateHandler(GUI_CALLBACK Callback)
{
    GUINew::CGUIElement_Impl::SetActivateHandler(Callback);
};
void SetDeactivateHandler(GUI_CALLBACK Callback)
{
    GUINew::CGUIElement_Impl::SetDeactivateHandler(Callback);
};
void SetKeyDownHandler(GUI_CALLBACK Callback)
{
    GUINew::CGUIElement_Impl::SetKeyDownHandler(Callback);
};
void SetKeyDownHandler(const GUI_CALLBACK_KEY& Callback)
{
    GUINew::CGUIElement_Impl::SetKeyDownHandler(Callback);
};
void SetEnterKeyHandler(GUI_CALLBACK Callback)
{
    GUINew::CGUIElement_Impl::SetEnterKeyHandler(Callback);
};

bool Event_OnClick(const CEGUI::EventArgs& e)
{
    return GUINew::CGUIElement_Impl::Event_OnClick(e);
};
bool Event_OnDoubleClick()
{
    return GUINew::CGUIElement_Impl::Event_OnDoubleClick();
};
bool Event_OnMouseEnter()
{
    return GUINew::CGUIElement_Impl::Event_OnMouseEnter();
};
bool Event_OnMouseLeave()
{
    return GUINew::CGUIElement_Impl::Event_OnMouseLeave();
};
bool Event_OnMouseButtonDown()
{
    return GUINew::CGUIElement_Impl::Event_OnMouseButtonDown();
};
