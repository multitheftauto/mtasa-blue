/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIElement_Impl.cpp
 *  PURPOSE:     Element derived class inclusion header
 *               (fixes C++-language issues regarding inheritance)
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

void SetProperty(const char* szProperty, const char* szValue)
{
    CGUIElement_Impl::SetProperty(szProperty, szValue);
};
std::string GetProperty(const char* szProperty)
{
    return CGUIElement_Impl::GetProperty(szProperty);
};

CGUIPropertyIter GetPropertiesBegin()
{
    return CGUIElement_Impl::GetPropertiesBegin();
};
CGUIPropertyIter GetPropertiesEnd()
{
    return CGUIElement_Impl::GetPropertiesEnd();
};

bool SetFont(const char* szFontName)
{
    return CGUIElement_Impl::SetFont(szFontName);
};
std::string GetFont()
{
    return CGUIElement_Impl::GetFont();
};

#ifndef SETVISIBLE_HACK
void SetVisible(bool bVisible)
{
    CGUIElement_Impl::SetVisible(bVisible);
};
bool IsVisible()
{
    return CGUIElement_Impl::IsVisible();
};
#endif

#ifndef SETENABLED_HACK
void SetEnabled(bool bEnabled)
{
    CGUIElement_Impl::SetEnabled(bEnabled);
};
bool IsEnabled()
{
    return CGUIElement_Impl::IsEnabled();
};
#endif
void SetZOrderingEnabled(bool bZOrderingEnabled)
{
    CGUIElement_Impl::SetZOrderingEnabled(bZOrderingEnabled);
};
bool IsZOrderingEnabled()
{
    return CGUIElement_Impl::IsZOrderingEnabled();
};

void BringToFront()
{
    CGUIElement_Impl::BringToFront();
};
void MoveToBack()
{
    CGUIElement_Impl::MoveToBack();
};

void SetPosition(const CVector2D& vecPosition, bool bRelative = false)
{
    CGUIElement_Impl::SetPosition(vecPosition, bRelative);
};
CVector2D GetPosition(bool bRelative = false)
{
    return CGUIElement_Impl::GetPosition(bRelative);
};
void GetPosition(CVector2D& vecPosition, bool bRelative = false)
{
    CGUIElement_Impl::GetPosition(vecPosition, bRelative);
};

void SetWidth(float fX, bool bRelative = false)
{
    CGUIElement_Impl::SetWidth(fX, bRelative);
};
void SetHeight(float fY, bool bRelative = false)
{
    CGUIElement_Impl::SetHeight(fY, bRelative);
};

#ifndef EXCLUDE_SET_SIZE // WTF? TODO: Refactor this
void SetSize(const CVector2D& vecSize, bool bRelative = false)
{
    CGUIElement_Impl::SetSize(vecSize, bRelative);
};
#endif
CVector2D GetSize(bool bRelative = false)
{
    return CGUIElement_Impl::GetSize(bRelative);
};
void GetSize(CVector2D& vecSize, bool bRelative = false)
{
    CGUIElement_Impl::GetSize(vecSize, bRelative);
};

void AutoSize(const char* Text = NULL, float fPaddingX = 0.0f, float fPaddingY = 2.0f)
{
    CGUIElement_Impl::AutoSize(Text, fPaddingX, fPaddingY);
};

void SetMinimumSize(const CVector2D& vecSize)
{
    CGUIElement_Impl::SetMinimumSize(vecSize);
};
CVector2D GetMinimumSize()
{
    return CGUIElement_Impl::GetMinimumSize();
};
void GetMinimumSize(CVector2D& vecSize)
{
    CGUIElement_Impl::GetMinimumSize(vecSize);
};

void SetMaximumSize(const CVector2D& vecSize)
{
    CGUIElement_Impl::SetMaximumSize(vecSize);
};
CVector2D GetMaximumSize()
{
    return CGUIElement_Impl::GetMaximumSize();
};
void GetMaximumSize(CVector2D& vecSize)
{
    CGUIElement_Impl::GetMaximumSize(vecSize);
};

#ifndef EXCLUDE_SET_TEXT
void SetText(const char* Text)
{
    CGUIElement_Impl::SetText(Text);
};
#endif

std::string GetText()
{
    return CGUIElement_Impl::GetText();
};

void SetAlpha(float fAlpha)
{
    CGUIElement_Impl::SetAlpha(fAlpha);
};
float GetAlpha()
{
    return CGUIElement_Impl::GetAlpha();
};
float GetEffectiveAlpha()
{
    return CGUIElement_Impl::GetEffectiveAlpha();
};
void SetInheritsAlpha(bool bInheritsAlpha)
{
    CGUIElement_Impl::SetInheritsAlpha(bInheritsAlpha);
};
bool GetInheritsAlpha()
{
    return CGUIElement_Impl::GetInheritsAlpha();
};

void Activate()
{
    CGUIElement_Impl::Activate();
};
void Deactivate()
{
    CGUIElement_Impl::Deactivate();
};
bool IsActive()
{
    return CGUIElement_Impl::IsActive();
};

void ForceRedraw()
{
    CGUIElement_Impl::ForceRedraw();
};
void SetAlwaysOnTop(bool bAlwaysOnTop)
{
    CGUIElement_Impl::SetAlwaysOnTop(bAlwaysOnTop);
};
bool IsAlwaysOnTop()
{
    return CGUIElement_Impl::IsAlwaysOnTop();
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
    CGUIElement_Impl::SetParent(pParent);
};
CGUIElement* GetParent()
{
    return CGUIElement_Impl::GetParent();
};

CEGUI::Window* GetWindow()
{
    return CGUIElement_Impl::GetWindow();
};

void SetUserData(void* pData)
{
    CGUIElement_Impl::SetUserData(pData);
};
void* GetUserData()
{
    return CGUIElement_Impl::GetUserData();
};

void SetClickHandler(GUI_CALLBACK Callback)
{
    CGUIElement_Impl::SetClickHandler(Callback);
};
void SetClickHandler(const GUI_CALLBACK_MOUSE& Callback)
{
    CGUIElement_Impl::SetClickHandler(Callback);
};
void SetDoubleClickHandler(GUI_CALLBACK Callback)
{
    CGUIElement_Impl::SetDoubleClickHandler(Callback);
};
void SetMovedHandler(GUI_CALLBACK Callback)
{
    CGUIElement_Impl::SetMovedHandler(Callback);
};
void SetSizedHandler(GUI_CALLBACK Callback)
{
    CGUIElement_Impl::SetSizedHandler(Callback);
};
void SetMouseEnterHandler(GUI_CALLBACK Callback)
{
    CGUIElement_Impl::SetMouseEnterHandler(Callback);
};
void SetMouseLeaveHandler(GUI_CALLBACK Callback)
{
    CGUIElement_Impl::SetMouseLeaveHandler(Callback);
};
void SetMouseButtonDownHandler(GUI_CALLBACK Callback)
{
    CGUIElement_Impl::SetMouseButtonDownHandler(Callback);
};
void SetActivateHandler(GUI_CALLBACK Callback)
{
    CGUIElement_Impl::SetActivateHandler(Callback);
};
void SetDeactivateHandler(GUI_CALLBACK Callback)
{
    CGUIElement_Impl::SetDeactivateHandler(Callback);
};
void SetKeyDownHandler(GUI_CALLBACK Callback)
{
    CGUIElement_Impl::SetKeyDownHandler(Callback);
};
void SetKeyDownHandler(const GUI_CALLBACK_KEY& Callback)
{
    CGUIElement_Impl::SetKeyDownHandler(Callback);
};
void SetEnterKeyHandler(GUI_CALLBACK Callback)
{
    CGUIElement_Impl::SetEnterKeyHandler(Callback);
};

bool Event_OnClick(const CEGUI::EventArgs& e)
{
    return CGUIElement_Impl::Event_OnClick(e);
};
bool Event_OnDoubleClick()
{
    return CGUIElement_Impl::Event_OnDoubleClick();
};
bool Event_OnMouseEnter()
{
    return CGUIElement_Impl::Event_OnMouseEnter();
};
bool Event_OnMouseLeave()
{
    return CGUIElement_Impl::Event_OnMouseLeave();
};
bool Event_OnMouseButtonDown()
{
    return CGUIElement_Impl::Event_OnMouseButtonDown();
};
