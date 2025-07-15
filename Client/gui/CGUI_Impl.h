/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUI_Impl.h
 *  PURPOSE:     Graphical User Interface module class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

class CGUI_Impl;

#pragma once

#include <gui/CGUI.h>
#include <list>
#include <windows.h>

#define CGUI_CHAR_SIZE 6

class CGUIElement;
class CGUIElement_Impl;
class CGUIButton;
class CGUICheckBox;
class CGUIEdit;
class CGUIEvent;
class CGUIFont;
class CGUIFont_Impl;
class CGUIGridList;
class CGUILabel;
class CGUIMemo;
class CGUIMessageBox;
class CGUIProgressBar;
class CGUIRadioButton;
class CGUIStaticImage;
class CGUIScrollBar;
class CGUIScrollPane;
class CGUIComboBox;
class CGUITexture;
class CGUIWindow;
class CGUITab;
class CGUITabPanel;
struct IDirect3DDevice9;

namespace CEGUI
{
    class FontManager;
    class ImagesetManager;
    class Renderer;
    class System;
    class SchemeManager;
    class WindowManager;
    class Image;
    class EventArgs;
    class GUISheet;
    typedef GUISheet DefaultWindow;
}            // namespace CEGUI

class CGUI_Impl : public CGUI, public CGUITabList
{
public:
    CGUI_Impl(IDirect3DDevice9* pDevice);
    ~CGUI_Impl();

    void SetSkin(const char* szName);
    void SetBidiEnabled(bool bEnabled);

    void Draw();
    void Invalidate();
    void Restore();

    void DrawMouseCursor();

    void ProcessMouseInput(CGUIMouseInput eMouseInput, unsigned long ulX = 0, unsigned long ulY = 0, CGUIMouseButton eMouseButton = NoButton);
    void ProcessKeyboardInput(unsigned long ulKey, bool bIsDown);
    void ProcessCharacter(unsigned long ulCharacter);

    //
    bool                 GetGUIInputEnabled();
    void                 SetGUIInputMode(eInputMode a_eMode);
    eInputMode           GetGUIInputMode();
    static CEGUI::String GetUTFString(const char* szInput);
    static CEGUI::String GetUTFString(const std::string& strInput);
    static CEGUI::String GetUTFString(const CEGUI::String& strInput);            // Not defined

    //
    CGUIMessageBox* CreateMessageBox(const char* szTitle, const char* szMessage, unsigned int uiFlags);

    CGUIButton* CreateButton(CGUIElement* pParent = NULL, const char* szCaption = "");
    CGUIButton* CreateButton(CGUITab* pParent = NULL, const char* szCaption = "");

    CGUICheckBox* CreateCheckBox(CGUIElement* pParent = NULL, const char* szCaption = "", bool bChecked = false);
    CGUICheckBox* CreateCheckBox(CGUITab* pParent = NULL, const char* szCaption = "", bool bChecked = false);

    CGUIRadioButton* CreateRadioButton(CGUIElement* pParent = NULL, const char* szCaption = "");
    CGUIRadioButton* CreateRadioButton(CGUITab* pParent = NULL, const char* szCaption = "");

    CGUIEdit* CreateEdit(CGUIElement* pParent = NULL, const char* szText = "");
    CGUIEdit* CreateEdit(CGUITab* pParent = NULL, const char* szText = "");

    CGUIGridList* CreateGridList(CGUIElement* pParent = NULL, bool bFrame = true);
    CGUIGridList* CreateGridList(CGUITab* pParent = NULL, bool bFrame = true);

    CGUILabel* CreateLabel(CGUIElement* pParent, const char* szCaption = "");
    CGUILabel* CreateLabel(CGUITab* pParent, const char* szCaption = "");
    CGUILabel* CreateLabel(const char* szCaption = "");

    CGUIProgressBar* CreateProgressBar(CGUIElement* pParent);
    CGUIProgressBar* CreateProgressBar(CGUITab* pParent);

    CGUIMemo* CreateMemo(CGUIElement* pParent = NULL, const char* szText = "");
    CGUIMemo* CreateMemo(CGUITab* pParent = NULL, const char* szText = "");

    CGUIStaticImage* CreateStaticImage(CGUIElement* pParent);
    CGUIStaticImage* CreateStaticImage(CGUITab* pParent);
    CGUIStaticImage* CreateStaticImage(CGUIGridList* pParent);
    CGUIStaticImage* CreateStaticImage();

    CGUITabPanel* CreateTabPanel(CGUIElement* pParent);
    CGUITabPanel* CreateTabPanel(CGUITab* pParent);
    CGUITabPanel* CreateTabPanel();

    CGUIScrollPane* CreateScrollPane(CGUIElement* pParent);
    CGUIScrollPane* CreateScrollPane(CGUITab* pParent);
    CGUIScrollPane* CreateScrollPane();

    CGUIScrollBar* CreateScrollBar(bool bHorizontal, CGUIElement* pParent = NULL);
    CGUIScrollBar* CreateScrollBar(bool bHorizontal, CGUITab* pParent = NULL);

    CGUIComboBox* CreateComboBox(CGUIElement* pParent = NULL, const char* szCaption = "");
    CGUIComboBox* CreateComboBox(CGUIComboBox* pParent = NULL, const char* szCaption = "");

    CGUIWebBrowser* CreateWebBrowser(CGUIElement* pParent = nullptr);
    CGUIWebBrowser* CreateWebBrowser(CGUITab* pParent = nullptr);

    CGUIWindow* CreateWnd(CGUIElement* pParent = NULL, const char* szCaption = "");
    //

    CGUITexture* CreateTexture();
    CGUIFont*    CreateFnt(const char* szFontName, const char* szFontFile, unsigned int uSize = 8, unsigned int uFlags = 0, bool bAutoScale = false);

    void        SetCursorEnabled(bool bEnabled);
    bool        IsCursorEnabled();
    void        SetCursorAlpha(float fAlpha, bool bOnlyCurrentServer = false);
    void        SetCurrentServerCursorAlpha(float fAlpha);
    float       GetCurrentServerCursorAlpha();
    eCursorType GetCursorType();

    void                    AddChild(CGUIElement_Impl* pChild);
    CEGUI::FontManager*     GetFontManager();
    CEGUI::ImagesetManager* GetImageSetManager();
    CEGUI::Renderer*        GetRenderer();
    CEGUI::System*          GetGUISystem();
    CEGUI::SchemeManager*   GetSchemeManager();
    CEGUI::WindowManager*   GetWindowManager();
    void                    GetUniqueName(char* pBuf);
    CEGUI::Window*          GetMasterWindow(CEGUI::Window* Window);

    CVector2D GetResolution();
    void      SetResolution(float fWidth, float fHeight);

    CGUIFont* GetDefaultFont();
    CGUIFont* GetSmallFont();
    CGUIFont* GetBoldFont();
    CGUIFont* GetClearFont();
    CGUIFont* GetSAHeaderFont();
    CGUIFont* GetSAGothicFont();
    CGUIFont* GetSansFont();
    bool      IsFontPresent(const char* szFont) { return m_pFontManager->isFontPresent(szFont); }

    float GetTextExtent(const char* szText, const char* szFont = "default-normal");
    float GetMaxTextExtent(SString strFont, SString arg, ...);

    const SString& GetGuiWorkingDirectory() const;
    void           SetDefaultGuiWorkingDirectory(const SString& strDir);
    void           PushGuiWorkingDirectory(const SString& strDir);
    void           PopGuiWorkingDirectory(const SString& strDirCheck = "");

    void SetCharacterKeyHandler(eInputChannel channel, const GUI_CALLBACK_KEY& Callback)
    {
        CHECK_CHANNEL(channel);
        m_CharacterKeyHandlers[channel] = Callback;
    }
    void SetKeyDownHandler(eInputChannel channel, const GUI_CALLBACK_KEY& Callback)
    {
        CHECK_CHANNEL(channel);
        m_KeyDownHandlers[channel] = Callback;
    }
    void SetMouseClickHandler(eInputChannel channel, const GUI_CALLBACK_MOUSE& Callback)
    {
        CHECK_CHANNEL(channel);
        m_MouseClickHandlers[channel] = Callback;
    }
    void SetMouseDoubleClickHandler(eInputChannel channel, const GUI_CALLBACK_MOUSE& Callback)
    {
        CHECK_CHANNEL(channel);
        m_MouseDoubleClickHandlers[channel] = Callback;
    }
    void SetMouseButtonDownHandler(eInputChannel channel, const GUI_CALLBACK_MOUSE& Callback)
    {
        CHECK_CHANNEL(channel);
        m_MouseButtonDownHandlers[channel] = Callback;
    }
    void SetMouseButtonUpHandler(eInputChannel channel, const GUI_CALLBACK_MOUSE& Callback)
    {
        CHECK_CHANNEL(channel);
        m_MouseButtonUpHandlers[channel] = Callback;
    }
    void SetMouseMoveHandler(eInputChannel channel, const GUI_CALLBACK_MOUSE& Callback)
    {
        CHECK_CHANNEL(channel);
        m_MouseMoveHandlers[channel] = Callback;
    }
    void SetMouseEnterHandler(eInputChannel channel, const GUI_CALLBACK_MOUSE& Callback)
    {
        CHECK_CHANNEL(channel);
        m_MouseEnterHandlers[channel] = Callback;
    }
    void SetMouseLeaveHandler(eInputChannel channel, const GUI_CALLBACK_MOUSE& Callback)
    {
        CHECK_CHANNEL(channel);
        m_MouseLeaveHandlers[channel] = Callback;
    }
    void SetMouseWheelHandler(eInputChannel channel, const GUI_CALLBACK_MOUSE& Callback)
    {
        CHECK_CHANNEL(channel);
        m_MouseWheelHandlers[channel] = Callback;
    }
    void SetMovedHandler(eInputChannel channel, const GUI_CALLBACK& Callback)
    {
        CHECK_CHANNEL(channel);
        m_MovedHandlers[channel] = Callback;
    }
    void SetSizedHandler(eInputChannel channel, const GUI_CALLBACK& Callback)
    {
        CHECK_CHANNEL(channel);
        m_SizedHandlers[channel] = Callback;
    }
    void SetFocusGainedHandler(eInputChannel channel, const GUI_CALLBACK_FOCUS& Callback)
    {
        CHECK_CHANNEL(channel);
        m_FocusGainedHandlers[channel] = Callback;
    }
    void SetFocusLostHandler(eInputChannel channel, const GUI_CALLBACK_FOCUS& Callback)
    {
        CHECK_CHANNEL(channel);
        m_FocusLostHandlers[channel] = Callback;
    }

    void SelectInputHandlers(eInputChannel channel)
    {
        CHECK_CHANNEL(channel);
        m_Channel = channel;
    }
    void ClearInputHandlers(eInputChannel channel);
    void ClearSystemKeys();

    bool Event_CharacterKey(const CEGUI::EventArgs& e);
    bool Event_KeyDown(const CEGUI::EventArgs& e);
    bool Event_MouseClick(const CEGUI::EventArgs& e);
    bool Event_MouseDoubleClick(const CEGUI::EventArgs& e);
    bool Event_MouseButtonDown(const CEGUI::EventArgs& e);
    bool Event_MouseButtonUp(const CEGUI::EventArgs& e);
    bool Event_MouseWheel(const CEGUI::EventArgs& e);
    bool Event_MouseMove(const CEGUI::EventArgs& e);
    bool Event_MouseEnter(const CEGUI::EventArgs& e);
    bool Event_MouseLeave(const CEGUI::EventArgs& e);
    bool Event_Moved(const CEGUI::EventArgs& e);
    bool Event_Sized(const CEGUI::EventArgs& e);
    bool Event_RedrawRequested(const CEGUI::EventArgs& e);
    bool Event_FocusGained(const CEGUI::EventArgs& e);
    bool Event_FocusLost(const CEGUI::EventArgs& e);

    void AddToRedrawQueue(CGUIElement* pWindow);
    void RemoveFromRedrawQueue(CGUIElement* pWindow);

    void        CleanDeadPool();
    CGUIWindow* LoadLayout(CGUIElement* pParent, const SString& strFilename);
    bool        LoadImageset(const SString& strFilename);

private:
    CGUIButton*      _CreateButton(CGUIElement_Impl* pParent = NULL, const char* szCaption = "");
    CGUICheckBox*    _CreateCheckBox(CGUIElement_Impl* pParent = NULL, const char* szCaption = "", bool bChecked = false);
    CGUIRadioButton* _CreateRadioButton(CGUIElement_Impl* pParent = NULL, const char* szCaption = "");
    CGUIEdit*        _CreateEdit(CGUIElement_Impl* pParent = NULL, const char* szText = "");
    CGUIGridList*    _CreateGridList(CGUIElement_Impl* pParent = NULL, bool bFrame = true);
    CGUILabel*       _CreateLabel(CGUIElement_Impl* pParent = NULL, const char* szCaption = "");
    CGUIProgressBar* _CreateProgressBar(CGUIElement_Impl* pParent = NULL);
    CGUIMemo*        _CreateMemo(CGUIElement_Impl* pParent = NULL, const char* szText = "");
    CGUIStaticImage* _CreateStaticImage(CGUIElement_Impl* pParent = NULL);
    CGUITabPanel*    _CreateTabPanel(CGUIElement_Impl* pParent = NULL);
    CGUIScrollPane*  _CreateScrollPane(CGUIElement_Impl* pParent = NULL);
    CGUIScrollBar*   _CreateScrollBar(bool bHorizontal, CGUIElement_Impl* pParent = NULL);
    CGUIComboBox*    _CreateComboBox(CGUIElement_Impl* pParent = NULL, const char* szCaption = "");
    CGUIWebBrowser*  _CreateWebBrowser(CGUIElement_Impl* pParent = nullptr);

    void      SubscribeToMouseEvents();
    CGUIFont* CreateFntFromWinFont(const char* szFontName, const char* szFontWinReg, const char* szFontWinFile, unsigned int uSize = 8, unsigned int uFlags = 0,
                                   bool bAutoScale = false);
    void      ApplyGuiWorkingDirectory();

    IDirect3DDevice9* m_pDevice;

    CEGUI::Renderer*        m_pRenderer;
    CEGUI::System*          m_pSystem;
    CEGUI::FontManager*     m_pFontManager;
    CEGUI::ImagesetManager* m_pImageSetManager;
    CEGUI::SchemeManager*   m_pSchemeManager;
    CEGUI::WindowManager*   m_pWindowManager;

    CEGUI::DefaultWindow* m_pTop;
    const CEGUI::Image*   m_pCursor;
    float                 m_fCurrentServerCursorAlpha;

    CGUIFont_Impl* m_pDefaultFont;
    CGUIFont_Impl* m_pSmallFont;
    CGUIFont_Impl* m_pBoldFont;
    CGUIFont_Impl* m_pClearFont;
    CGUIFont_Impl* m_pSAHeaderFont;
    CGUIFont_Impl* m_pSAGothicFont;
    CGUIFont_Impl* m_pSansFont;
    CGUIFont_Impl* m_pUniFont;

    std::list<CGUIElement*> m_RedrawQueue;

    unsigned long m_ulPreviousUnique;

    eInputMode m_eInputMode;

    GUI_CALLBACK_KEY   m_CharacterKeyHandlers[INPUT_CHANNEL_COUNT];
    GUI_CALLBACK_KEY   m_KeyDownHandlers[INPUT_CHANNEL_COUNT];
    GUI_CALLBACK_MOUSE m_MouseClickHandlers[INPUT_CHANNEL_COUNT];
    GUI_CALLBACK_MOUSE m_MouseDoubleClickHandlers[INPUT_CHANNEL_COUNT];
    GUI_CALLBACK_MOUSE m_MouseButtonDownHandlers[INPUT_CHANNEL_COUNT];
    GUI_CALLBACK_MOUSE m_MouseButtonUpHandlers[INPUT_CHANNEL_COUNT];
    GUI_CALLBACK_MOUSE m_MouseMoveHandlers[INPUT_CHANNEL_COUNT];
    GUI_CALLBACK_MOUSE m_MouseEnterHandlers[INPUT_CHANNEL_COUNT];
    GUI_CALLBACK_MOUSE m_MouseLeaveHandlers[INPUT_CHANNEL_COUNT];
    GUI_CALLBACK_MOUSE m_MouseWheelHandlers[INPUT_CHANNEL_COUNT];
    GUI_CALLBACK       m_MovedHandlers[INPUT_CHANNEL_COUNT];
    GUI_CALLBACK       m_SizedHandlers[INPUT_CHANNEL_COUNT];
    GUI_CALLBACK_FOCUS m_FocusGainedHandlers[INPUT_CHANNEL_COUNT];
    GUI_CALLBACK_FOCUS m_FocusLostHandlers[INPUT_CHANNEL_COUNT];

    eInputChannel m_Channel;

    std::list<SString> m_GuiWorkingDirectoryStack;

    bool         m_HasSchemeLoaded;
    SString      m_CurrentSchemeName;
    CElapsedTime m_RenderOkTimer;
};
