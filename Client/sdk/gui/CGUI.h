/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/gui/CGUI.h
 *  PURPOSE:     Graphical User Interface module interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CGUI;

#include "CGUIEnumDefs.h"
#include "CGUIElement.h"
#include "CGUIButton.h"
#include "CGUICallback.h"
#include "CGUICheckBox.h"
#include "CGUIEdit.h"
#include "CGUIEvent.h"
#include "CGUIFont.h"
#include "CGUIGridList.h"
#include "CGUILabel.h"
#include "CGUIMemo.h"
#include "CGUIMessageBox.h"
#include "CGUIProgressBar.h"
#include "CGUIRadioButton.h"
#include "CGUIStaticImage.h"
#include "CGUIScrollBar.h"
#include "CGUIScrollPane.h"
#include "CGUITexture.h"
#include "CGUIWindow.h"
#include "CGUIWebBrowser.h"
#include "CGUITabPanel.h"
#include "CGUIComboBox.h"
#include "CGUITypes.h"

// Path defines for CGUI
#define CGUI_ICON_MESSAGEBOX_INFO       "cgui\\images\\info.png"
#define CGUI_ICON_MESSAGEBOX_QUESTION   "cgui\\images\\question.png"
#define CGUI_ICON_MESSAGEBOX_WARNING    "cgui\\images\\warning.png"
#define CGUI_ICON_MESSAGEBOX_ERROR      "cgui\\images\\error.png"
#define CGUI_ICON_SERVER_PASSWORD       "cgui\\images\\locked.png"
#define CGUI_GetMaxTextExtent(...) GetMaxTextExtent(__VA_ARGS__, SString())

#define CHECK_CHANNEL(channel) assert ( (channel) >= 0 && (channel) < INPUT_CHANNEL_COUNT )

class CGUI
{
public:
    virtual void SetSkin(const char* szName) = 0;
    virtual void SetBidiEnabled(bool bEnabled) = 0;

    virtual void Draw() = 0;
    virtual void Invalidate() = 0;
    virtual void Restore() = 0;

    virtual void DrawMouseCursor() = 0;

    virtual void ProcessMouseInput(CGUIMouseInput eMouseInput, unsigned long ulX = 0, unsigned long ulY = 0, CGUIMouseButton eMouseButton = NoButton) = 0;
    virtual void ProcessKeyboardInput(unsigned long ulKey, bool bIsDown) = 0;
    virtual void ProcessCharacter(unsigned long ulCharacter) = 0;

    //
    virtual bool       GetGUIInputEnabled() = 0;
    virtual void       SetGUIInputMode(eInputMode a_eMode) = 0;
    virtual eInputMode GetGUIInputMode() = 0;

    //
    virtual CGUIMessageBox* CreateMessageBox(const char* szTitle, const char* szMessage, unsigned int uiFlags) = 0;
    virtual CGUIButton*     CreateButton(CGUIElement* pParent = NULL, const char* szCaption = "") = 0;
    virtual CGUIButton*     CreateButton(CGUITab* pParent = NULL, const char* szCaption = "") = 0;

    virtual CGUICheckBox* CreateCheckBox(CGUIElement* pParent = NULL, const char* szCaption = "", bool bChecked = false) = 0;
    virtual CGUICheckBox* CreateCheckBox(CGUITab* pParent = NULL, const char* szCaption = "", bool bChecked = false) = 0;

    virtual CGUIRadioButton* CreateRadioButton(CGUIElement* pParent = NULL, const char* szCaption = "") = 0;
    virtual CGUIRadioButton* CreateRadioButton(CGUITab* pParent = NULL, const char* szCaption = "") = 0;

    virtual CGUIEdit* CreateEdit(CGUIElement* pParent = NULL, const char* szText = "") = 0;
    virtual CGUIEdit* CreateEdit(CGUITab* pParent = NULL, const char* szText = "") = 0;

    virtual CGUIGridList* CreateGridList(CGUIElement* pParent = NULL, bool bFrame = true) = 0;
    virtual CGUIGridList* CreateGridList(CGUITab* pParent = NULL, bool bFrame = true) = 0;

    virtual CGUILabel* CreateLabel(CGUIElement* pParent, const char* szCaption = "") = 0;
    virtual CGUILabel* CreateLabel(CGUITab* pParent, const char* szCaption = "") = 0;
    virtual CGUILabel* CreateLabel(const char* szCaption = "") = 0;

    virtual CGUIProgressBar* CreateProgressBar(CGUIElement* pParent) = 0;
    virtual CGUIProgressBar* CreateProgressBar(CGUITab* pParent) = 0;

    virtual CGUIMemo* CreateMemo(CGUIElement* pParent = NULL, const char* szText = "") = 0;
    virtual CGUIMemo* CreateMemo(CGUITab* pParent = NULL, const char* szText = "") = 0;

    virtual CGUIStaticImage* CreateStaticImage(CGUIElement* pParent) = 0;
    virtual CGUIStaticImage* CreateStaticImage(CGUITab* pParent) = 0;
    virtual CGUIStaticImage* CreateStaticImage(CGUIGridList* pParent) = 0;
    virtual CGUIStaticImage* CreateStaticImage() = 0;

    virtual CGUITabPanel* CreateTabPanel(CGUIElement* pParent) = 0;
    virtual CGUITabPanel* CreateTabPanel(CGUITab* pParent) = 0;
    virtual CGUITabPanel* CreateTabPanel() = 0;

    virtual CGUIScrollPane* CreateScrollPane(CGUIElement* pParent) = 0;
    virtual CGUIScrollPane* CreateScrollPane(CGUITab* pParent) = 0;
    virtual CGUIScrollPane* CreateScrollPane() = 0;

    virtual CGUIScrollBar* CreateScrollBar(bool bHorizontal, CGUIElement* pParent = NULL) = 0;
    virtual CGUIScrollBar* CreateScrollBar(bool bHorizontal, CGUITab* pParent = NULL) = 0;

    virtual CGUIComboBox* CreateComboBox(CGUIElement* pParent = NULL, const char* szCaption = "") = 0;
    virtual CGUIComboBox* CreateComboBox(CGUIComboBox* pParent = NULL, const char* szCaption = "") = 0;

    virtual CGUIWebBrowser* CreateWebBrowser(CGUIElement* pParent = nullptr) = 0;
    virtual CGUIWebBrowser* CreateWebBrowser(CGUITab* pParent = nullptr) = 0;

    //

    virtual CGUIWindow* CreateWnd(CGUIElement* pParent = NULL, const char* szCaption = "") = 0;
    virtual CGUIFont*   CreateFnt(const char* szFontName, const char* szFontFile, unsigned int uSize = 8, unsigned int uFlags = 0, bool bAutoScale = false) = 0;
    virtual CGUITexture* CreateTexture() = 0;

    virtual void        SetCursorEnabled(bool bEnabled) = 0;
    virtual bool        IsCursorEnabled() = 0;
    virtual void        SetCursorAlpha(float fAlpha, bool bOnlyCurrentServer = false) = 0;
    virtual void        SetCurrentServerCursorAlpha(float fAlpha) = 0;
    virtual float       GetCurrentServerCursorAlpha() = 0;
    virtual eCursorType GetCursorType() = 0;

    virtual CVector2D GetResolution() = 0;
    virtual void      SetResolution(float fWidth, float fHeight) = 0;

    virtual CGUIFont* GetDefaultFont() = 0;
    virtual CGUIFont* GetSmallFont() = 0;
    virtual CGUIFont* GetBoldFont() = 0;
    virtual CGUIFont* GetClearFont() = 0;
    virtual CGUIFont* GetSansFont() = 0;
    virtual bool      IsFontPresent(const char* szFont) = 0;

    virtual float GetTextExtent(const char* szText, const char* szFont = "default-normal") = 0;
    virtual float GetMaxTextExtent(SString strFont, SString arg, ...) = 0;

    virtual void PushGuiWorkingDirectory(const SString& strDir) = 0;
    virtual void PopGuiWorkingDirectory(const SString& strDirCheck = "") = 0;

    virtual void SetCharacterKeyHandler(eInputChannel channel, const GUI_CALLBACK_KEY& Callback) = 0;
    virtual void SetKeyDownHandler(eInputChannel channel, const GUI_CALLBACK_KEY& Callback) = 0;
    virtual void SetMouseClickHandler(eInputChannel channel, const GUI_CALLBACK_MOUSE& Callback) = 0;
    virtual void SetMouseDoubleClickHandler(eInputChannel channel, const GUI_CALLBACK_MOUSE& Callback) = 0;
    virtual void SetMouseButtonDownHandler(eInputChannel channel, const GUI_CALLBACK_MOUSE& Callback) = 0;
    virtual void SetMouseButtonUpHandler(eInputChannel channel, const GUI_CALLBACK_MOUSE& Callback) = 0;
    virtual void SetMouseMoveHandler(eInputChannel channel, const GUI_CALLBACK_MOUSE& Callback) = 0;
    virtual void SetMouseEnterHandler(eInputChannel channel, const GUI_CALLBACK_MOUSE& Callback) = 0;
    virtual void SetMouseLeaveHandler(eInputChannel channel, const GUI_CALLBACK_MOUSE& Callback) = 0;
    virtual void SetMouseWheelHandler(eInputChannel channel, const GUI_CALLBACK_MOUSE& Callback) = 0;
    virtual void SetMovedHandler(eInputChannel channel, const GUI_CALLBACK& Callback) = 0;
    virtual void SetSizedHandler(eInputChannel channel, const GUI_CALLBACK& Callback) = 0;
    virtual void SetFocusGainedHandler(eInputChannel channel, const GUI_CALLBACK_FOCUS& Callback) = 0;
    virtual void SetFocusLostHandler(eInputChannel channel, const GUI_CALLBACK_FOCUS& Callback) = 0;

    virtual void SelectInputHandlers(eInputChannel channel) = 0;
    virtual void ClearInputHandlers(eInputChannel channel) = 0;
    virtual void ClearSystemKeys() = 0;

    virtual void CleanDeadPool() = 0;

    virtual CGUIWindow* LoadLayout(CGUIElement* pParent, const SString& strFilename) = 0;
    virtual bool        LoadImageset(const SString& strFilename) = 0;
};
