/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUI_Impl.cpp
 *  PURPOSE:     Graphical User Interface module class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CEGUIExceptions.h"

using std::list;

#define CGUI_MTA_DEFAULT_FONT       "tahoma.ttf"        // %WINDIR%/font/<...>
#define CGUI_MTA_DEFAULT_FONT_BOLD  "tahomabd.ttf"      // %WINDIR%/font/<...>
#define CGUI_MTA_CLEAR_FONT         "verdana.ttf"       // %WINDIR%/font/<...>

#define CGUI_MTA_DEFAULT_REG        "Tahoma (TrueType)"
#define CGUI_MTA_DEFAULT_REG_BOLD   "Tahoma Bold (TrueType)"
#define CGUI_MTA_CLEAR_REG          "Verdana (TrueType)"

#define CGUI_MTA_SUBSTITUTE_FONT    "cgui/unifont.ttf"  // GTA/MTA/<...>
#define CGUI_MTA_SANS_FONT          "cgui/sans.ttf"     // GTA/MTA/<...>
#define CGUI_SA_HEADER_FONT         "cgui/saheader.ttf" // GTA/MTA/<...>
#define CGUI_SA_GOTHIC_FONT         "cgui/sagothic.ttf" // GTA/MTA/<...>
#define CGUI_SA_HEADER_SIZE         26
#define CGUI_SA_GOTHIC_SIZE         47
#define CGUI_MTA_SANS_FONT_SIZE     9

CGUI_Impl::CGUI_Impl(IDirect3DDevice9* pDevice) : m_HasSchemeLoaded(false), m_fCurrentServerCursorAlpha(1.0f)
{
    m_RenderOkTimer.SetMaxIncrement(100);

    // Init
    m_pDevice = pDevice;
    /*
    m_pCharacterKeyHandler = NULL;
    m_pKeyDownHandler = NULL;
    m_pMouseClickHandler = NULL;
    m_pMouseDoubleClickHandler = NULL;
    m_pMouseWheelHandler = NULL;
    m_pMouseMoveHandler = NULL;
    m_pMouseEnterHandler = NULL;
    m_pMouseLeaveHandler = NULL;
    m_pMovedHandler = NULL;
    m_pSizedHandler = NULL;
    */
    m_Channel = INPUT_CORE;

    // Create a GUI system and get the windowmanager
    m_pRenderer = new CEGUI::DirectX9Renderer(pDevice, 0);
    m_pSystem = new CEGUI::System(m_pRenderer, CEGUI::String(CalcMTASAPath(PathJoin("MTA", "logs", "CEGUI.log"))).data());

    // Get pointers to various stuff from CEGUI singletons
    m_pFontManager = CEGUI::FontManager::getSingletonPtr();
    m_pImageSetManager = CEGUI::ImagesetManager::getSingletonPtr();
    m_pSchemeManager = CEGUI::SchemeManager::getSingletonPtr();
    m_pWindowManager = CEGUI::WindowManager::getSingletonPtr();

    SetDefaultGuiWorkingDirectory(CalcMTASAPath("MTA"));

    // Set logging to Informative for debug and Standard for release
#if defined(_DEBUG) || defined(DEBUG)
    CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Informative);
#else
    CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Standard);
#endif

    // Load our fonts
    SString strFontsPath = PathJoin(GetSystemWindowsPath(), "fonts");

    try
    {
        m_pUniFont = (CGUIFont_Impl*)CreateFnt("unifont", CGUI_MTA_SUBSTITUTE_FONT, 9, 0, false);
        m_pFontManager->setSubstituteFont(m_pUniFont->GetFont());
    }
    catch (CEGUI::InvalidRequestException e)
    {
        SString strMessage = e.getMessage().c_str();
        BrowseToSolution("create-fonts", EXIT_GAME_FIRST | ASK_GO_ONLINE, SString("Error loading fonts!\n\n%s", *strMessage));
    }

    // Window fonts first
    m_pDefaultFont = (CGUIFont_Impl*)CreateFntFromWinFont("default-normal", CGUI_MTA_DEFAULT_REG, CGUI_MTA_DEFAULT_FONT, 9, 0);
    m_pSmallFont = (CGUIFont_Impl*)CreateFntFromWinFont("default-small", CGUI_MTA_DEFAULT_REG, CGUI_MTA_DEFAULT_FONT, 7, 0);
    m_pBoldFont = (CGUIFont_Impl*)CreateFntFromWinFont("default-bold-small", CGUI_MTA_DEFAULT_REG_BOLD, CGUI_MTA_DEFAULT_FONT_BOLD, 8, 0);
    m_pClearFont = (CGUIFont_Impl*)CreateFntFromWinFont("clear-normal", CGUI_MTA_CLEAR_REG, CGUI_MTA_CLEAR_FONT, 9);

    try
    {
        m_pSAHeaderFont = (CGUIFont_Impl*)CreateFnt("sa-header", CGUI_SA_HEADER_FONT, CGUI_SA_HEADER_SIZE, 0, true);
        m_pSAGothicFont = (CGUIFont_Impl*)CreateFnt("sa-gothic", CGUI_SA_GOTHIC_FONT, CGUI_SA_GOTHIC_SIZE, 0, true);
        m_pSansFont = (CGUIFont_Impl*)CreateFnt("sans", CGUI_MTA_SANS_FONT, CGUI_MTA_SANS_FONT_SIZE, 0, false);
    }
    catch (CEGUI::InvalidRequestException e)
    {
        SString strMessage = e.getMessage().c_str();
        BrowseToSolution("create-fonts", EXIT_GAME_FIRST | ASK_GO_ONLINE, SString("Error loading fonts!\n\n%s", *strMessage));
    }
}

CGUI_Impl::~CGUI_Impl()
{
    delete CEGUI::System::getSingletonPtr();
}

void CGUI_Impl::SetSkin(const char* szName)
{
    if (m_HasSchemeLoaded)
    {
        CEGUI::GlobalEventSet::getSingletonPtr()->removeAllEvents();
        CEGUI::SchemeManager::getSingleton().unloadScheme(m_CurrentSchemeName);
    }

    PushGuiWorkingDirectory(CalcMTASAPath(PathJoin("skins", szName)));

    CEGUI::Scheme* scheme = CEGUI::SchemeManager::getSingleton().loadScheme("CGUI.xml");
    m_CurrentSchemeName = scheme->getName().c_str();
    m_HasSchemeLoaded = true;

    PopGuiWorkingDirectory();

    CEGUI::System::getSingleton().setDefaultMouseCursor("CGUI-Images", "MouseArrow");

    // Destroy any windows we already have
    CEGUI::WindowManager::getSingleton().destroyAllWindows();

    // Create dummy GUI root
    m_pTop = reinterpret_cast<CEGUI::DefaultWindow*>(m_pWindowManager->createWindow("DefaultWindow", "guiroot"));
    m_pSystem->setGUISheet(m_pTop);

    // Disable single click timeouts
    m_pSystem->setSingleClickTimeout(100000000.0f);

    // Set our default font
    m_pSystem->setDefaultFont(m_pDefaultFont->GetFont());

    // Grab our default cursor
    m_pCursor = m_pSystem->getDefaultMouseCursor();

    // Used to create unique names for widget instances
    m_ulPreviousUnique = 0;

    SubscribeToMouseEvents();

    // Disallow input routing to the GUI unless edit box has focus
    m_eInputMode = INPUTMODE_NO_BINDS_ON_EDIT;
}

void CGUI_Impl::SetBidiEnabled(bool bEnabled)
{
    m_pSystem->SetBidiEnabled(bEnabled);
}

void CGUI_Impl::SubscribeToMouseEvents()
{
    // Mouse events
    CEGUI::GlobalEventSet* pEvents = CEGUI::GlobalEventSet::getSingletonPtr();

    pEvents->subscribeEvent("Window/" + CEGUI::Window::EventCharacterKey, CEGUI::Event::Subscriber(&CGUI_Impl::Event_CharacterKey, this));
    pEvents->subscribeEvent("Window/" + CEGUI::Window::EventKeyDown, CEGUI::Event::Subscriber(&CGUI_Impl::Event_KeyDown, this));
    pEvents->subscribeEvent("Window/" + CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&CGUI_Impl::Event_MouseClick, this));
    pEvents->subscribeEvent("Window/" + CEGUI::Window::EventMouseDoubleClick, CEGUI::Event::Subscriber(&CGUI_Impl::Event_MouseDoubleClick, this));
    pEvents->subscribeEvent("Window/" + CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&CGUI_Impl::Event_MouseButtonDown, this));
    pEvents->subscribeEvent("Window/" + CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&CGUI_Impl::Event_MouseButtonUp, this));
    pEvents->subscribeEvent("Window/" + CEGUI::Window::EventMouseWheel, CEGUI::Event::Subscriber(&CGUI_Impl::Event_MouseWheel, this));
    pEvents->subscribeEvent("Window/" + CEGUI::Window::EventMouseMove, CEGUI::Event::Subscriber(&CGUI_Impl::Event_MouseMove, this));
    pEvents->subscribeEvent("Window/" + CEGUI::Window::EventMouseEnters, CEGUI::Event::Subscriber(&CGUI_Impl::Event_MouseEnter, this));
    pEvents->subscribeEvent("Window/" + CEGUI::Window::EventMouseLeaves, CEGUI::Event::Subscriber(&CGUI_Impl::Event_MouseLeave, this));
    pEvents->subscribeEvent("Window/" + CEGUI::Window::EventMoved, CEGUI::Event::Subscriber(&CGUI_Impl::Event_Moved, this));
    pEvents->subscribeEvent("Window/" + CEGUI::Window::EventSized, CEGUI::Event::Subscriber(&CGUI_Impl::Event_Sized, this));
    pEvents->subscribeEvent("Window/" + CEGUI::Window::EventRedrawRequested, CEGUI::Event::Subscriber(&CGUI_Impl::Event_RedrawRequested, this));
    pEvents->subscribeEvent("Window/" + CEGUI::Window::EventActivated, CEGUI::Event::Subscriber(&CGUI_Impl::Event_FocusGained, this));
    pEvents->subscribeEvent("Window/" + CEGUI::Window::EventDeactivated, CEGUI::Event::Subscriber(&CGUI_Impl::Event_FocusLost, this));
}

CVector2D CGUI_Impl::GetResolution()
{
    return CVector2D(m_pRenderer->getWidth(), m_pRenderer->getHeight());
}

void CGUI_Impl::SetResolution(float fWidth, float fHeight)
{
    reinterpret_cast<CEGUI::DirectX9Renderer*>(m_pRenderer)->setDisplaySize(CEGUI::Size(fWidth, fHeight));
}

void CGUI_Impl::Draw()
{
    // Redraw the changed elements
    if (!m_RedrawQueue.empty())
    {
        list<CGUIElement*>::const_iterator iter = m_RedrawQueue.begin();
        for (; iter != m_RedrawQueue.end(); iter++)
        {
            (*iter)->ForceRedraw();
        }
        m_RedrawQueue.clear();
    }

    if (!m_pSystem->renderGUI())
    {
        if (m_RenderOkTimer.Get() > 4000)
        {
            // 4 seconds and over 40 failed calls means we have a problem
            BrowseToSolution("gui-render", EXIT_GAME_FIRST, "Some sort of DirectX problem has occurred");
        }
    }
    else
        m_RenderOkTimer.Reset();
}

void CGUI_Impl::Invalidate()
{
    reinterpret_cast<CEGUI::DirectX9Renderer*>(m_pRenderer)->preD3DReset();
}

void CGUI_Impl::Restore()
{
    try
    {
        reinterpret_cast<CEGUI::DirectX9Renderer*>(m_pRenderer)->postD3DReset();
    }
    catch (CEGUI::RendererException& exception)
    {
        MessageBox(0, exception.getMessage().c_str(), "CEGUI Exception", MB_OK | MB_ICONERROR | MB_TOPMOST);
        TerminateProcess(GetCurrentProcess(), 1);
    }
}

void CGUI_Impl::DrawMouseCursor()
{
    CEGUI::MouseCursor::getSingleton().draw();
}

void CGUI_Impl::ProcessMouseInput(CGUIMouseInput eMouseInput, unsigned long ulX, unsigned long ulY, CGUIMouseButton eMouseButton)
{
    switch (eMouseInput)
    {
        case CGUI_MI_MOUSEMOVE:
            m_pSystem->injectMouseMove(static_cast<float>(ulX), static_cast<float>(ulY));
            break;

        case CGUI_MI_MOUSEPOS:
            m_pSystem->injectMousePosition(static_cast<float>(ulX), static_cast<float>(ulY));
            break;

        case CGUI_MI_MOUSEDOWN:
            m_pSystem->injectMouseButtonDown(static_cast<CEGUI::MouseButton>(eMouseButton));
            break;

        case CGUI_MI_MOUSEUP:
            m_pSystem->injectMouseButtonUp(static_cast<CEGUI::MouseButton>(eMouseButton));
            break;

        case CGUI_MI_MOUSEWHEEL:
            if ((signed long)ulX > 0)
                m_pSystem->injectMouseWheelChange(+1);
            else
                m_pSystem->injectMouseWheelChange(-1);
            break;
    }
}

void CGUI_Impl::ProcessKeyboardInput(unsigned long ulKey, bool bIsDown)
{
    if (bIsDown)
    {
        m_pSystem->injectKeyDown(ulKey);
    }
    else
    {
        m_pSystem->injectKeyUp(ulKey);
    }
}

bool CGUI_Impl::GetGUIInputEnabled()
{
    switch (m_eInputMode)
    {
        case INPUTMODE_ALLOW_BINDS:
            return false;
            break;
        case INPUTMODE_NO_BINDS:
            return true;
            break;
        case INPUTMODE_NO_BINDS_ON_EDIT:
        {
            CEGUI::Window* pActiveWindow = m_pTop->getActiveChild();
            if (!pActiveWindow || pActiveWindow == m_pTop || !pActiveWindow->isVisible())
            {
                return false;
            }
            if (pActiveWindow->getType() == "CGUI/Editbox")
            {
                CEGUI::Editbox* pEditBox = reinterpret_cast<CEGUI::Editbox*>(pActiveWindow);
                return (!pEditBox->isReadOnly() && pEditBox->hasInputFocus());
            }
            else if (pActiveWindow->getType() == "CGUI/MultiLineEditbox")
            {
                CEGUI::MultiLineEditbox* pMultiLineEditBox = reinterpret_cast<CEGUI::MultiLineEditbox*>(pActiveWindow);
                return (!pMultiLineEditBox->isReadOnly() && pMultiLineEditBox->hasInputFocus());
            }
            else if (pActiveWindow->getType() == CGUIWEBBROWSER_NAME)
            {
                auto pElement = reinterpret_cast<CGUIElement_Impl*>(pActiveWindow->getUserData());
                if (pElement->GetType() == CGUI_WEBBROWSER)
                {
                    auto pWebBrowser = reinterpret_cast<CGUIWebBrowser_Impl*>(pElement);
                    return pWebBrowser->HasInputFocus();
                }
            }
            return false;
        }
        break;
        default:
            return false;
    }
}

void CGUI_Impl::SetGUIInputMode(eInputMode a_eMode)
{
    m_eInputMode = a_eMode;
}

eInputMode CGUI_Impl::GetGUIInputMode()
{
    return m_eInputMode;
}

CEGUI::String CGUI_Impl::GetUTFString(const char* szInput)
{
    CEGUI::String strUTF = (CEGUI::utf8*)szInput;            // Convert into a CEGUI String
    return strUTF;
}

CEGUI::String CGUI_Impl::GetUTFString(const std::string& strInput)
{
    CEGUI::String strUTF = (CEGUI::utf8*)strInput.c_str();            // Convert into a CEGUI String
    return strUTF;
}

void CGUI_Impl::ProcessCharacter(unsigned long ulCharacter)
{
    m_pSystem->injectChar(ulCharacter);
}

CGUIMessageBox* CGUI_Impl::CreateMessageBox(const char* szTitle, const char* szMessage, unsigned int uiFlags)
{
    return new CGUIMessageBox_Impl(this, szTitle, szMessage, uiFlags);
}

CGUIButton* CGUI_Impl::_CreateButton(CGUIElement_Impl* pParent, const char* szCaption)
{
    return new CGUIButton_Impl(this, pParent, szCaption);
}

CGUICheckBox* CGUI_Impl::_CreateCheckBox(CGUIElement_Impl* pParent, const char* szCaption, bool bChecked)
{
    return new CGUICheckBox_Impl(this, pParent, szCaption, bChecked);
}

CGUIRadioButton* CGUI_Impl::_CreateRadioButton(CGUIElement_Impl* pParent, const char* szCaption)
{
    return new CGUIRadioButton_Impl(this, pParent, szCaption);
}

CGUIEdit* CGUI_Impl::_CreateEdit(CGUIElement_Impl* pParent, const char* szText)
{
    return new CGUIEdit_Impl(this, pParent, szText);
}

CGUIFont* CGUI_Impl::CreateFnt(const char* szFontName, const char* szFontFile, unsigned int uSize, unsigned int uFlags, bool bAutoScale)
{
    return new CGUIFont_Impl(this, szFontName, szFontFile, uSize, uFlags, bAutoScale);
}

CGUIFont* CGUI_Impl::CreateFntFromWinFont(const char* szFontName, const char* szFontWinReg, const char* szFontWinFile, unsigned int uSize, unsigned int uFlags,
                                          bool bAutoScale)
{
    SString strFontWinRegName = GetSystemRegistryValue((uint)HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", szFontWinReg);
    SString strWinFontsPath = PathJoin(GetSystemWindowsPath(), "fonts");

    // Compile a list of places to look
    std::vector<SString> lookList;
    if (strFontWinRegName.Contains(":") || strFontWinRegName.BeginsWith("\\") || strFontWinRegName.BeginsWith("/"))
        lookList.push_back(strFontWinRegName);
    lookList.push_back(PathJoin(strWinFontsPath, strFontWinRegName));
    lookList.push_back(PathJoin("cgui", szFontWinFile));
    lookList.push_back(PathJoin(strWinFontsPath, szFontWinFile));

    // Try each place
    CGUIFont* pResult = NULL;
    for (uint i = 0; i < lookList.size(); i++)
    {
        if (FileExists(lookList[i]))
        {
            try
            {
                pResult = (CGUIFont_Impl*)CreateFnt(szFontName, lookList[i], uSize, uFlags, bAutoScale);
            }
            catch (CEGUI::Exception e)
            {
            }
        }

        if (pResult)
            break;
    }
    if (!pResult)
    {
        BrowseToSolution("create-fonts", EXIT_GAME_FIRST | ASK_GO_ONLINE, SString("Error loading font!\n\n(%s)", szFontWinFile));
    }

    return pResult;
}

CGUIGridList* CGUI_Impl::_CreateGridList(CGUIElement_Impl* pParent, bool bFrame)
{
    return new CGUIGridList_Impl(this, pParent, bFrame);
}

CGUILabel* CGUI_Impl::_CreateLabel(CGUIElement_Impl* pParent, const char* szCaption)
{
    return new CGUILabel_Impl(this, pParent, szCaption);
}

CGUIProgressBar* CGUI_Impl::_CreateProgressBar(CGUIElement_Impl* pParent)
{
    return new CGUIProgressBar_Impl(this, pParent);
}

CGUIMemo* CGUI_Impl::_CreateMemo(CGUIElement_Impl* pParent, const char* szText)
{
    return new CGUIMemo_Impl(this, pParent, szText);
}

CGUIStaticImage* CGUI_Impl::_CreateStaticImage(CGUIElement_Impl* pParent)
{
    return new CGUIStaticImage_Impl(this, pParent);
}

CGUITabPanel* CGUI_Impl::_CreateTabPanel(CGUIElement_Impl* pParent)
{
    return new CGUITabPanel_Impl(this, pParent);
}

CGUIScrollPane* CGUI_Impl::_CreateScrollPane(CGUIElement_Impl* pParent)
{
    return new CGUIScrollPane_Impl(this, pParent);
}

CGUIScrollBar* CGUI_Impl::_CreateScrollBar(bool bHorizontal, CGUIElement_Impl* pParent)
{
    return new CGUIScrollBar_Impl(this, bHorizontal, pParent);
}

CGUIComboBox* CGUI_Impl::_CreateComboBox(CGUIElement_Impl* pParent, const char* szCaption)
{
    return new CGUIComboBox_Impl(this, pParent, szCaption);
}

CGUIWebBrowser* CGUI_Impl::_CreateWebBrowser(CGUIElement_Impl* pParent)
{
    return new CGUIWebBrowser_Impl(this, pParent);
}

CGUITexture* CGUI_Impl::CreateTexture()
{
    return new CGUITexture_Impl(this);
}

CGUIWindow* CGUI_Impl::CreateWnd(CGUIElement* pParent, const char* szCaption)
{
    return new CGUIWindow_Impl(this, pParent, szCaption);
}

void CGUI_Impl::SetCursorEnabled(bool bEnabled)
{
    if (bEnabled)
    {
        CEGUI::MouseCursor::getSingleton().show();
    }
    else
    {
        CEGUI::MouseCursor::getSingleton().hide();
    }
}

bool CGUI_Impl::IsCursorEnabled()
{
    return CEGUI::MouseCursor::getSingleton().isVisible();
}

void CGUI_Impl::SetCursorAlpha(float fAlpha, bool bOnlyCurrentServer)
{
    CEGUI::MouseCursor::getSingleton().setAlpha(fAlpha);

    if (bOnlyCurrentServer)
        SetCurrentServerCursorAlpha(fAlpha);
}

void CGUI_Impl::SetCurrentServerCursorAlpha(float fAlpha)
{
    m_fCurrentServerCursorAlpha = fAlpha;
}

float CGUI_Impl::GetCurrentServerCursorAlpha()
{
    return m_fCurrentServerCursorAlpha;
}

eCursorType CGUI_Impl::GetCursorType()
{
    auto image = CEGUI::MouseCursor::getSingleton().getImage();

    if (image == nullptr)
        return CURSORTYPE_NONE;

    auto imageName = image->getName();

    if (!imageName.compare("MouseArrow"))
        return CURSORTYPE_DEFAULT;
    else if (!imageName.compare("NSSizingCursorImage"))
        return CURSORTYPE_SIZING_NS;
    else if (!imageName.compare("EWSizingCursorImage"))
        return CURSORTYPE_SIZING_EW;
    else if (!imageName.compare("NWSESizingCursorImage"))
        return CURSORTYPE_SIZING_NWSE;
    else if (!imageName.compare("NESWSizingCursorImage"))
        return CURSORTYPE_SIZING_NESW;
    else if (!imageName.compare("MouseEsWeCursor"))
        return CURSORTYPE_SIZING_ESWE;
    else if (!imageName.compare("MouseMoveCursor"))
        return CURSORTYPE_MOVE;
    else
        return CURSORTYPE_DEFAULT;
}

void CGUI_Impl::AddChild(CGUIElement_Impl* pChild)
{
    m_pTop->addChildWindow(pChild->GetWindow());
}

CGUIWindow* CGUI_Impl::LoadLayout(CGUIElement* pParent, const SString& strFilename)
{
    try
    {
        return new CGUIWindow_Impl(this, pParent, "szCaption", strFilename);
    }
    catch (...)
    {
        return NULL;
    }
}

bool CGUI_Impl::LoadImageset(const SString& strFilename)
{
    try
    {
        return GetImageSetManager()->createImageset(strFilename, "", true) != NULL;
    }
    catch (CEGUI::AlreadyExistsException exc)
    {
        return true;
    }
    catch (...)
    {
        return false;
    }
}

CEGUI::FontManager* CGUI_Impl::GetFontManager()
{
    return m_pFontManager;
}

CEGUI::ImagesetManager* CGUI_Impl::GetImageSetManager()
{
    return m_pImageSetManager;
}

CEGUI::System* CGUI_Impl::GetGUISystem()
{
    return m_pSystem;
}

CEGUI::Renderer* CGUI_Impl::GetRenderer()
{
    return m_pRenderer;
}

CEGUI::SchemeManager* CGUI_Impl::GetSchemeManager()
{
    return m_pSchemeManager;
}

CEGUI::WindowManager* CGUI_Impl::GetWindowManager()
{
    return m_pWindowManager;
}

void CGUI_Impl::GetUniqueName(char* pBuf)
{
    snprintf(pBuf, CGUI_CHAR_SIZE, "%x", m_ulPreviousUnique);
    m_ulPreviousUnique++;
}

bool CGUI_Impl::Event_CharacterKey(const CEGUI::EventArgs& Args)
{
    if (m_CharacterKeyHandlers[m_Channel])
    {
        const CEGUI::KeyEventArgs& e = reinterpret_cast<const CEGUI::KeyEventArgs&>(Args);
        CGUIKeyEventArgs           NewArgs;

        // copy the variables
        NewArgs.codepoint = e.codepoint;
        NewArgs.scancode = (CGUIKeys::Scan)e.scancode;
        NewArgs.sysKeys = e.sysKeys;

        // get the CGUIElement
        CGUIElement* pElement = reinterpret_cast<CGUIElement*>((e.window)->getUserData());
        NewArgs.pWindow = pElement;

        m_CharacterKeyHandlers[m_Channel](NewArgs);
    }
    return true;
}

CGUIFont* CGUI_Impl::GetDefaultFont()
{
    return m_pDefaultFont;
}

CGUIFont* CGUI_Impl::GetSmallFont()
{
    return m_pSmallFont;
}

CGUIFont* CGUI_Impl::GetBoldFont()
{
    return m_pBoldFont;
}

CGUIFont* CGUI_Impl::GetClearFont()
{
    return m_pClearFont;
}

CGUIFont* CGUI_Impl::GetSAHeaderFont()
{
    return m_pSAHeaderFont;
}

CGUIFont* CGUI_Impl::GetSAGothicFont()
{
    return m_pSAGothicFont;
}

CGUIFont* CGUI_Impl::GetSansFont()
{
    return m_pSansFont;
}

float CGUI_Impl::GetTextExtent(const char* szText, const char* szFont)
{
    return m_pFontManager->getFont(szFont)->getTextExtent(CGUI_Impl::GetUTFString(szText));
}

float CGUI_Impl::GetMaxTextExtent(SString strFont, SString arg, ...)
{
    float   fMaxTextExtent = NULL;
    va_list arguments;
    for (va_start(arguments, arg); arg != ""; arg = va_arg(arguments, SString))
    {
        float fExtent = m_pFontManager->getFont(strFont)->getTextExtent(CGUI_Impl::GetUTFString(arg));
        if (fExtent > fMaxTextExtent)
            fMaxTextExtent = fExtent;
    }
    va_end(arguments);
    return fMaxTextExtent;
}

bool CGUI_Impl::Event_KeyDown(const CEGUI::EventArgs& Args)
{
    // Cast it to a set of keyboard arguments
    const CEGUI::KeyEventArgs& KeyboardArgs = reinterpret_cast<const CEGUI::KeyEventArgs&>(Args);

    // Call the callback if present
    if (m_KeyDownHandlers[m_Channel])
    {
        const CEGUI::KeyEventArgs& e = reinterpret_cast<const CEGUI::KeyEventArgs&>(Args);
        CGUIKeyEventArgs           NewArgs;

        // copy the variables
        NewArgs.codepoint = e.codepoint;
        NewArgs.scancode = (CGUIKeys::Scan)e.scancode;
        NewArgs.sysKeys = e.sysKeys;

        // get the CGUIElement
        CGUIElement* pElement = reinterpret_cast<CGUIElement*>((e.window)->getUserData());
        NewArgs.pWindow = pElement;

        m_KeyDownHandlers[m_Channel](NewArgs);
    }

    switch (KeyboardArgs.scancode)
    {
        // Cut/Copy keys
        case CEGUI::Key::X:
        case CEGUI::Key::C:
        {
            if (KeyboardArgs.sysKeys & CEGUI::Control)
            {
                // Data to copy
                CEGUI::String strTemp;

                // Edit boxes
                CEGUI::Window* Wnd = reinterpret_cast<CEGUI::Window*>(KeyboardArgs.window);
                if (Wnd->getType() == "CGUI/Editbox")
                {
                    // Turn our event window into an editbox
                    CEGUI::Editbox* WndEdit = reinterpret_cast<CEGUI::Editbox*>(Wnd);

                    // Don't allow cutting/copying if the editbox is masked
                    if (!WndEdit->isTextMasked())
                    {
                        // Get the text from the editbox
                        size_t sizeSelectionStart = WndEdit->getSelectionStartIndex();
                        size_t sizeSelectionLength = WndEdit->getSelectionLength();
                        strTemp = WndEdit->getText().substr(sizeSelectionStart, sizeSelectionLength);

                        // If the user cut, remove the text too
                        if (KeyboardArgs.scancode == CEGUI::Key::X)
                        {
                            // Read only?
                            if (!WndEdit->isReadOnly())
                            {
                                // Remove the text from the source
                                CEGUI::String strTemp2 = WndEdit->getText();
                                strTemp2.replace(sizeSelectionStart, sizeSelectionLength, "", 0);
                                WndEdit->setText(strTemp2);
                            }
                        }
                    }
                }

                // Multiline editboxes
                if (Wnd->getType() == "CGUI/MultiLineEditbox")
                {
                    // Turn our event window into an editbox
                    CEGUI::MultiLineEditbox* WndEdit = reinterpret_cast<CEGUI::MultiLineEditbox*>(Wnd);

                    // Get the text from the editbox
                    size_t sizeSelectionStart = WndEdit->getSelectionStartIndex();
                    size_t sizeSelectionLength = WndEdit->getSelectionLength();
                    strTemp = WndEdit->getText().substr(sizeSelectionStart, sizeSelectionLength);

                    // If the user cut, remove the text too
                    if (KeyboardArgs.scancode == CEGUI::Key::X)
                    {
                        // Read only?
                        if (!WndEdit->isReadOnly())
                        {
                            // Remove the text from the source
                            CEGUI::String strTemp2 = WndEdit->getText();
                            strTemp2.replace(sizeSelectionStart, sizeSelectionLength, "", 0);
                            WndEdit->setText(strTemp2);
                        }
                    }
                }

                // If we got something to copy
                if (strTemp.length() > 0)
                {
                    // Convert it to Unicode
                    std::wstring strUTF = MbUTF8ToUTF16(strTemp.c_str());

                    // Open and empty the clipboard
                    OpenClipboard(NULL);
                    EmptyClipboard();

                    // Allocate the clipboard buffer and copy the data
                    HGLOBAL  hBuf = GlobalAlloc(GMEM_DDESHARE, strUTF.length() * sizeof(wchar_t) + sizeof(wchar_t));
                    wchar_t* buf = reinterpret_cast<wchar_t*>(GlobalLock(hBuf));
                    wcscpy(buf, strUTF.c_str());
                    GlobalUnlock(hBuf);

                    // Copy the data into the clipboard
                    SetClipboardData(CF_UNICODETEXT, hBuf);

                    // Close the clipboard
                    CloseClipboard();
                }
            }

            break;
        }

        // Paste keys
        case CEGUI::Key::V:
        {
            if (KeyboardArgs.sysKeys & CEGUI::Control)
            {
                CEGUI::Window* Wnd = reinterpret_cast<CEGUI::Window*>(KeyboardArgs.window);
                if (Wnd->getType() == "CGUI/Editbox" || Wnd->getType() == "CGUI/MultiLineEditbox")
                {
                    // Open the clipboard
                    OpenClipboard(NULL);

                    // Get the clipboard's data and lock it
                    HANDLE        hClipData = GetClipboardData(CF_UNICODETEXT);
                    const wchar_t* ClipboardBuffer = nullptr;
                    if (hClipData)
                        ClipboardBuffer = static_cast<const wchar_t*>(GlobalLock(hClipData));

                    // Check to make sure we have valid data.
                    if (ClipboardBuffer)
                    {
                        size_t        iSelectionStart, iSelectionLength, iMaxLength, iCaratIndex;
                        CEGUI::String strEditText;
                        bool          bReplaceNewLines = true;
                        bool          bIsBoxFull = false;

                        if (Wnd->getType() == "CGUI/Editbox")
                        {
                            // Turn our event window into an editbox
                            CEGUI::Editbox* WndEdit = reinterpret_cast<CEGUI::Editbox*>(Wnd);
                            // Don't paste if we're read only
                            if (WndEdit->isReadOnly())
                            {
                                if (hClipData)
                                    GlobalUnlock(hClipData);
                                CloseClipboard();
                                return true;
                            }
                            strEditText = WndEdit->getText();
                            iSelectionStart = WndEdit->getSelectionStartIndex();
                            iSelectionLength = WndEdit->getSelectionLength();
                            iMaxLength = WndEdit->getMaxTextLength();
                            iCaratIndex = WndEdit->getCaratIndex();
                        }
                        else
                        {
                            CEGUI::MultiLineEditbox* WndEdit = reinterpret_cast<CEGUI::MultiLineEditbox*>(Wnd);
                            // Don't paste if we're read only
                            if (WndEdit->isReadOnly())
                            {
                                if (hClipData)
                                    GlobalUnlock(hClipData);
                                CloseClipboard();
                                return true;
                            }

                            strEditText = WndEdit->getText();
                            iSelectionStart = WndEdit->getSelectionStartIndex();
                            iSelectionLength = WndEdit->getSelectionLength();
                            iMaxLength = WndEdit->getMaxTextLength();
                            iCaratIndex = WndEdit->getCaratIndex();
                            bReplaceNewLines = false;

                            // Plus one character, because there is always an extra '\n' in
                            // MultiLineEditbox's text data and it causes MaxLength limit to
                            // be exceeded during pasting the text
                            iMaxLength += 1;
                        }

                        std::wstring strClipboardText = ClipboardBuffer;
                        size_t       iNewlineIndex;

                        // Remove the newlines inserting spaces instead
                        if (bReplaceNewLines)
                        {
                            do
                            {
                                iNewlineIndex = strClipboardText.find('\n');
                                if (iNewlineIndex != SString::npos)
                                {
                                    if (iNewlineIndex > 0 && strClipboardText[iNewlineIndex - 1] == '\r')
                                    {
                                        // \r\n
                                        strClipboardText[iNewlineIndex - 1] = ' ';
                                        strClipboardText.replace(iNewlineIndex, strClipboardText.length() - iNewlineIndex, strClipboardText.c_str(),
                                                                 iNewlineIndex + 1, strClipboardText.length() - iNewlineIndex - 1);
                                    }
                                    else
                                    {
                                        strClipboardText[iNewlineIndex] = ' ';
                                    }
                                }
                            } while (iNewlineIndex != SString::npos);
                        }

                        // Put the editbox's data into a string and insert the data if it has not reached it's maximum text length
                        std::wstring tmp = MbUTF8ToUTF16(strEditText.c_str());
                        if ((strClipboardText.length() + tmp.length() - iSelectionLength) <= iMaxLength)
                        {
                            // Are there characters selected?
                            size_t sizeCaratIndex = 0;
                            if (iSelectionLength > 0)
                            {
                                // Replace what's selected with the pasted buffer and set the new carat index
                                tmp.replace(iSelectionStart, iSelectionLength, strClipboardText.c_str(), strClipboardText.length());
                                sizeCaratIndex = iSelectionStart + strClipboardText.length();
                            }
                            else
                            {
                                // If not, insert the clipboard buffer where we were and set the new carat index
                                tmp.insert(iSelectionStart, strClipboardText.c_str(), strClipboardText.length());
                                sizeCaratIndex = iCaratIndex + strClipboardText.length();
                            }

                            // Set the new text and move the carat at the end of what we pasted
                            CEGUI::String strText((CEGUI::utf8*)UTF16ToMbUTF8(tmp).c_str());
                            strEditText = strText;
                            iCaratIndex = sizeCaratIndex;
                        }
                        else
                        {
                            bIsBoxFull = true;
                        }
                        if (bIsBoxFull)
                        {
                            // Fire an event if the editbox is full
                            if (Wnd->getType() == "CGUI/Editbox")
                            {
                                CEGUI::Editbox*        WndEdit = reinterpret_cast<CEGUI::Editbox*>(Wnd);
                                CEGUI::WindowEventArgs args(WndEdit);
                                WndEdit->fireEvent(CEGUI::Editbox::EventEditboxFull, args);
                            }
                            else
                            {
                                CEGUI::MultiLineEditbox* WndEdit = reinterpret_cast<CEGUI::MultiLineEditbox*>(Wnd);
                                CEGUI::WindowEventArgs   args(WndEdit);
                                WndEdit->fireEvent(CEGUI::Editbox::EventEditboxFull, args);
                            }
                        }
                        else
                        {
                            if (Wnd->getType() == "CGUI/Editbox")
                            {
                                CEGUI::Editbox* WndEdit = reinterpret_cast<CEGUI::Editbox*>(Wnd);
                                WndEdit->setText(strEditText);
                                WndEdit->setCaratIndex(iCaratIndex);
                            }
                            else
                            {
                                CEGUI::MultiLineEditbox* WndEdit = reinterpret_cast<CEGUI::MultiLineEditbox*>(Wnd);
                                WndEdit->setText(strEditText);
                                WndEdit->setCaratIndex(iCaratIndex);
                            }
                        }
                    }

                    if (hClipData)
                        GlobalUnlock(hClipData);
                    
                    // Close the clipboard
                    CloseClipboard();
                }
            }

            break;
        }

        // Select all key
        case CEGUI::Key::A:
        {
            if (KeyboardArgs.sysKeys & CEGUI::Control)
            {
                // Edit boxes
                CEGUI::Window* Wnd = reinterpret_cast<CEGUI::Window*>(KeyboardArgs.window);
                if (Wnd->getType() == "CGUI/Editbox")
                {
                    // Turn our event window into an editbox
                    CEGUI::Editbox* WndEdit = reinterpret_cast<CEGUI::Editbox*>(Wnd);
                    WndEdit->setSelection(0, WndEdit->getText().size());
                }
                else if (Wnd->getType() == "CGUI/MultiLineEditbox")
                {
                    // Turn our event window into a multiline editbox
                    CEGUI::MultiLineEditbox* WndEdit = reinterpret_cast<CEGUI::MultiLineEditbox*>(Wnd);
                    WndEdit->setSelection(0, WndEdit->getText().size());
                }
            }

            break;
        }
    }

    return true;
}

void CGUI_Impl::SetDefaultGuiWorkingDirectory(const SString& strDir)
{
    assert(m_GuiWorkingDirectoryStack.empty());
    m_GuiWorkingDirectoryStack.push_back(PathConform(strDir + "\\"));
    ApplyGuiWorkingDirectory();
}

void CGUI_Impl::PushGuiWorkingDirectory(const SString& strDir)
{
    m_GuiWorkingDirectoryStack.push_back(PathConform(strDir + "\\"));
    ApplyGuiWorkingDirectory();
}

void CGUI_Impl::PopGuiWorkingDirectory(const SString& strDirCheck)
{
    if (m_GuiWorkingDirectoryStack.size() < 2)
    {
        OutputDebugLine(SString("CGUI_Impl::PopWorkingDirectory - Stack empty. Expected '%s'", *strDirCheck));
    }
    else
    {
        if (!strDirCheck.empty())
        {
            const SString& strWas = m_GuiWorkingDirectoryStack.back();
            if (strDirCheck != strWas)
            {
                OutputDebugLine(SString("CGUI_Impl::PopWorkingDirectory - Mismatch. Got '%s', expected '%s'", *strWas, *strDirCheck));
            }
        }
        m_GuiWorkingDirectoryStack.pop_back();
    }
    ApplyGuiWorkingDirectory();
}

void CGUI_Impl::ApplyGuiWorkingDirectory()
{
    CEGUI::System::getSingleton().SetGuiWorkingDirectory(m_GuiWorkingDirectoryStack.back());
}

const SString& CGUI_Impl::GetGuiWorkingDirectory() const
{
    dassert(!m_GuiWorkingDirectoryStack.empty());
    return m_GuiWorkingDirectoryStack.back();
}

bool CGUI_Impl::Event_MouseClick(const CEGUI::EventArgs& Args)
{
    const CEGUI::MouseEventArgs& e = reinterpret_cast<const CEGUI::MouseEventArgs&>(Args);

    // get the approriate cegui window
    CEGUI::Window* wnd = e.window;

    // if its a title- or scrollbar, get the appropriate parent
    wnd = GetMasterWindow(wnd);

    // get the CGUIElement
    CGUIElement* pElement = reinterpret_cast<CGUIElement*>(wnd->getUserData());

    // Call global and object handlers
    if (pElement)
        pElement->Event_OnClick(Args);

    if (m_MouseClickHandlers[m_Channel])
    {
        CGUIMouseEventArgs NewArgs;

        // copy the variables
        NewArgs.button = static_cast<CGUIMouse::MouseButton>(e.button);
        NewArgs.moveDelta = CVector2D(e.moveDelta.d_x, e.moveDelta.d_y);
        NewArgs.position = CGUIPosition(e.position.d_x, e.position.d_y);
        NewArgs.sysKeys = e.sysKeys;
        NewArgs.wheelChange = e.wheelChange;
        NewArgs.pWindow = pElement;

        m_MouseClickHandlers[m_Channel](NewArgs);
    }
    return true;
}

bool CGUI_Impl::Event_MouseDoubleClick(const CEGUI::EventArgs& Args)
{
    const CEGUI::MouseEventArgs& e = reinterpret_cast<const CEGUI::MouseEventArgs&>(Args);

    // get the approriate cegui window
    CEGUI::Window* wnd = e.window;

    // if its a title- or scrollbar, get the appropriate parent
    wnd = GetMasterWindow(wnd);

    // get the CGUIElement
    CGUIElement* pElement = reinterpret_cast<CGUIElement*>(wnd->getUserData());

    // Call global and object handlers
    if (pElement)
        pElement->Event_OnDoubleClick();

    if (m_MouseDoubleClickHandlers[m_Channel])
    {
        CGUIMouseEventArgs NewArgs;

        // copy the variables
        NewArgs.button = static_cast<CGUIMouse::MouseButton>(e.button);
        NewArgs.moveDelta = CVector2D(e.moveDelta.d_x, e.moveDelta.d_y);
        NewArgs.position = CGUIPosition(e.position.d_x, e.position.d_y);
        NewArgs.sysKeys = e.sysKeys;
        NewArgs.wheelChange = e.wheelChange;
        NewArgs.pWindow = pElement;

        m_MouseDoubleClickHandlers[m_Channel](NewArgs);
    }
    return true;
}

bool CGUI_Impl::Event_MouseButtonDown(const CEGUI::EventArgs& Args)
{
    const CEGUI::MouseEventArgs& e = reinterpret_cast<const CEGUI::MouseEventArgs&>(Args);

    // get the approriate cegui window
    CEGUI::Window* wnd = e.window;

    // if its a title- or scrollbar, get the appropriate parent
    wnd = GetMasterWindow(wnd);

    // get the CGUIElement
    CGUIElement* pElement = reinterpret_cast<CGUIElement*>(wnd->getUserData());

    // Call global and object handlers
    if (pElement)
        pElement->Event_OnMouseButtonDown();
    else
    {
        // If there's no element, we're probably dealing with the root element
        CEGUI::Window* pActiveWindow = m_pTop->getActiveChild();
        if (m_pTop == wnd && pActiveWindow)
        {
            // Deactivate active window to trigger onClientGUIBlur
            pActiveWindow->deactivate();
        }
    }

    if (m_MouseButtonDownHandlers[m_Channel])
    {
        CGUIMouseEventArgs NewArgs;

        // copy the variables
        NewArgs.button = static_cast<CGUIMouse::MouseButton>(e.button);
        NewArgs.moveDelta = CVector2D(e.moveDelta.d_x, e.moveDelta.d_y);
        NewArgs.position = CGUIPosition(e.position.d_x, e.position.d_y);
        NewArgs.sysKeys = e.sysKeys;
        NewArgs.wheelChange = e.wheelChange;
        NewArgs.pWindow = pElement;

        m_MouseButtonDownHandlers[m_Channel](NewArgs);
    }

    return true;
}

bool CGUI_Impl::Event_MouseButtonUp(const CEGUI::EventArgs& Args)
{
    if (m_MouseButtonUpHandlers[m_Channel])
    {
        const CEGUI::MouseEventArgs& e = reinterpret_cast<const CEGUI::MouseEventArgs&>(Args);
        CGUIMouseEventArgs           NewArgs;

        // get the approriate cegui window
        CEGUI::Window* wnd = e.window;

        // if its a title- or scrollbar, get the appropriate parent
        wnd = GetMasterWindow(wnd);

        // copy the variables
        NewArgs.button = static_cast<CGUIMouse::MouseButton>(e.button);
        NewArgs.moveDelta = CVector2D(e.moveDelta.d_x, e.moveDelta.d_y);
        NewArgs.position = CGUIPosition(e.position.d_x, e.position.d_y);
        NewArgs.sysKeys = e.sysKeys;
        NewArgs.wheelChange = e.wheelChange;

        // get the CGUIElement
        CGUIElement* pElement = reinterpret_cast<CGUIElement*>(wnd->getUserData());
        NewArgs.pWindow = pElement;

        m_MouseButtonUpHandlers[m_Channel](NewArgs);
    }
    return true;
}

bool CGUI_Impl::Event_MouseWheel(const CEGUI::EventArgs& Args)
{
    if (m_MouseWheelHandlers[m_Channel])
    {
        const CEGUI::MouseEventArgs& e = reinterpret_cast<const CEGUI::MouseEventArgs&>(Args);
        CGUIMouseEventArgs           NewArgs;

        // get the approriate cegui window
        CEGUI::Window* wnd = e.window;

        // if its a title- or scrollbar, get the appropriate parent
        wnd = GetMasterWindow(wnd);

        // copy the variables
        NewArgs.button = static_cast<CGUIMouse::MouseButton>(e.button);
        NewArgs.moveDelta = CVector2D(e.moveDelta.d_x, e.moveDelta.d_y);
        NewArgs.position = CGUIPosition(e.position.d_x, e.position.d_y);
        NewArgs.sysKeys = e.sysKeys;
        NewArgs.wheelChange = e.wheelChange;

        // get the CGUIElement
        CGUIElement* pElement = reinterpret_cast<CGUIElement*>(wnd->getUserData());
        NewArgs.pWindow = pElement;

        m_MouseWheelHandlers[m_Channel](NewArgs);
    }
    return true;
}

bool CGUI_Impl::Event_MouseMove(const CEGUI::EventArgs& Args)
{
    if (m_MouseMoveHandlers[m_Channel])
    {
        const CEGUI::MouseEventArgs& e = reinterpret_cast<const CEGUI::MouseEventArgs&>(Args);
        CGUIMouseEventArgs           NewArgs;

        // get the approriate cegui window
        CEGUI::Window* wnd = e.window;

        // if its a title- or scrollbar, get the appropriate parent
        wnd = GetMasterWindow(wnd);

        // copy the variables
        NewArgs.button = static_cast<CGUIMouse::MouseButton>(e.button);
        NewArgs.moveDelta = CVector2D(e.moveDelta.d_x, e.moveDelta.d_y);
        NewArgs.position = CGUIPosition(e.position.d_x, e.position.d_y);
        NewArgs.sysKeys = e.sysKeys;
        NewArgs.wheelChange = e.wheelChange;

        // get the CGUIElement
        CGUIElement* pElement = reinterpret_cast<CGUIElement*>(wnd->getUserData());
        NewArgs.pWindow = pElement;

        m_MouseMoveHandlers[m_Channel](NewArgs);
    }
    return true;
}

bool CGUI_Impl::Event_MouseEnter(const CEGUI::EventArgs& Args)
{
    const CEGUI::MouseEventArgs& e = reinterpret_cast<const CEGUI::MouseEventArgs&>(Args);

    // get the approriate cegui window
    CEGUI::Window* wnd = e.window;

    // if its a title- or scrollbar, get the appropriate parent
    wnd = GetMasterWindow(wnd);

    // get the CGUIElement
    CGUIElement* pElement = reinterpret_cast<CGUIElement*>(wnd->getUserData());

    // Call global and object handlers
    if (pElement)
        pElement->Event_OnMouseEnter();

    if (m_MouseEnterHandlers[m_Channel])
    {
        CGUIMouseEventArgs NewArgs;

        // copy the variables
        NewArgs.pWindow = pElement;
        NewArgs.button = static_cast<CGUIMouse::MouseButton>(e.button);
        NewArgs.moveDelta = CVector2D(e.moveDelta.d_x, e.moveDelta.d_y);
        NewArgs.position = CGUIPosition(e.position.d_x, e.position.d_y);
        NewArgs.sysKeys = e.sysKeys;
        NewArgs.wheelChange = e.wheelChange;
        NewArgs.clickCount = e.clickCount;
        if (e.switchedWindow)
        {
            CEGUI::Window* Master = GetMasterWindow(e.switchedWindow);
            // If the source and target windows are the same, don't bother triggering this
            if (Master == wnd)
                return true;
            NewArgs.pSwitchedWindow = reinterpret_cast<CGUIElement*>(Master->getUserData());
        }
        else
            NewArgs.pSwitchedWindow = NULL;

        m_MouseEnterHandlers[m_Channel](NewArgs);
    }

    return true;
}

bool CGUI_Impl::Event_MouseLeave(const CEGUI::EventArgs& Args)
{
    const CEGUI::MouseEventArgs& e = reinterpret_cast<const CEGUI::MouseEventArgs&>(Args);

    // get the approriate cegui window
    CEGUI::Window* wnd = e.window;

    // if its a title- or scrollbar, get the appropriate parent
    wnd = GetMasterWindow(wnd);

    // get the CGUIElement
    // ChrML: Need to nullcheck wnd again or it crashes if the window is destroyed
    //        while it is dragged.
    CGUIElement* pElement = NULL;
    if (wnd)
    {
        pElement = reinterpret_cast<CGUIElement*>(wnd->getUserData());
        if (pElement)
            pElement->Event_OnMouseLeave();
    }

    if (m_MouseLeaveHandlers[m_Channel])
    {
        CGUIMouseEventArgs NewArgs;

        // copy the variables
        NewArgs.pWindow = pElement;
        NewArgs.button = static_cast<CGUIMouse::MouseButton>(e.button);
        NewArgs.moveDelta = CVector2D(e.moveDelta.d_x, e.moveDelta.d_y);
        NewArgs.position = CGUIPosition(e.position.d_x, e.position.d_y);
        NewArgs.sysKeys = e.sysKeys;
        NewArgs.wheelChange = e.wheelChange;
        NewArgs.clickCount = e.clickCount;
        if (e.switchedWindow)
        {
            CEGUI::Window* Master = GetMasterWindow(e.switchedWindow);
            // If the source and target windows are the same, don't bother triggering this
            if (Master == wnd)
                return true;
            NewArgs.pSwitchedWindow = reinterpret_cast<CGUIElement*>(Master->getUserData());
        }
        else
            NewArgs.pSwitchedWindow = NULL;

        m_MouseLeaveHandlers[m_Channel](NewArgs);
    }

    return true;
}

bool CGUI_Impl::Event_Moved(const CEGUI::EventArgs& Args)
{
    if (m_MovedHandlers[m_Channel])
    {
        const CEGUI::WindowEventArgs& e = reinterpret_cast<const CEGUI::WindowEventArgs&>(Args);

        // get the CGUIElement
        CGUIElement* pElement = reinterpret_cast<CGUIElement*>((e.window)->getUserData());

        m_MovedHandlers[m_Channel](pElement);
    }
    return true;
}

bool CGUI_Impl::Event_Sized(const CEGUI::EventArgs& Args)
{
    if (m_SizedHandlers[m_Channel])
    {
        const CEGUI::WindowEventArgs& e = reinterpret_cast<const CEGUI::WindowEventArgs&>(Args);

        // get the CGUIElement
        CGUIElement* pElement = reinterpret_cast<CGUIElement*>((e.window)->getUserData());

        m_SizedHandlers[m_Channel](pElement);
    }
    return true;
}

bool CGUI_Impl::Event_RedrawRequested(const CEGUI::EventArgs& Args)
{
    const CEGUI::WindowEventArgs& e = reinterpret_cast<const CEGUI::WindowEventArgs&>(Args);

    CGUIElement* pElement = reinterpret_cast<CGUIElement*>((e.window)->getUserData());
    if (pElement)
        AddToRedrawQueue(pElement);
    else
        e.window->forceRedraw();

    return true;
}

bool CGUI_Impl::Event_FocusGained(const CEGUI::EventArgs& Args)
{
    if (m_FocusGainedHandlers[m_Channel])
    {
        const CEGUI::ActivationEventArgs& e = reinterpret_cast<const CEGUI::ActivationEventArgs&>(Args);

        CGUIFocusEventArgs NewArgs;

        // get the newly actived CGUIElement
        NewArgs.pActivatedWindow = reinterpret_cast<CGUIElement*>((e.window)->getUserData());

        // get the newly deactivated CGUIElement
        NewArgs.pDeactivatedWindow = NULL;
        if (e.otherWindow)
        {
            NewArgs.pDeactivatedWindow = reinterpret_cast<CGUIElement*>((e.otherWindow)->getUserData());
        }

        m_FocusGainedHandlers[m_Channel](NewArgs);
    }
    return true;
}

bool CGUI_Impl::Event_FocusLost(const CEGUI::EventArgs& Args)
{
    if (m_FocusLostHandlers[m_Channel])
    {
        const CEGUI::ActivationEventArgs& e = reinterpret_cast<const CEGUI::ActivationEventArgs&>(Args);

        CGUIFocusEventArgs NewArgs;

        // get the newly deactived CGUIElement
        NewArgs.pDeactivatedWindow = reinterpret_cast<CGUIElement*>((e.window)->getUserData());

        // get the newly activated CGUIElement
        NewArgs.pActivatedWindow = NULL;
        if (e.otherWindow)
        {
            NewArgs.pActivatedWindow = reinterpret_cast<CGUIElement*>((e.otherWindow)->getUserData());
        }

        m_FocusLostHandlers[m_Channel](NewArgs);
    }
    return true;
}

void CGUI_Impl::AddToRedrawQueue(CGUIElement* pWindow)
{
    // Manage the redraw queue, if we redraw the parent of the window passed,
    // we should not add it to the redraw queue, and if the children are queued,
    // remove them.
    list<CGUIElement*>::const_iterator iter = m_RedrawQueue.begin();
    for (; iter != m_RedrawQueue.end(); iter++)
    {
        if (pWindow->GetParent() == *iter)
        {
            return;
        }
        else if ((*iter)->GetParent() == pWindow)
        {
            m_RedrawQueue.remove(*iter);
            if (m_RedrawQueue.empty())
                return;
            iter = m_RedrawQueue.begin();
        }
        else if (*iter == pWindow)
        {
            return;
        }
    }
    m_RedrawQueue.push_back(pWindow);
}

void CGUI_Impl::RemoveFromRedrawQueue(CGUIElement* pWindow)
{
    m_RedrawQueue.remove(pWindow);
}

CGUIButton* CGUI_Impl::CreateButton(CGUIElement* pParent, const char* szCaption)
{
    CGUIWindow_Impl* wnd = reinterpret_cast<CGUIWindow_Impl*>(pParent);
    return _CreateButton(wnd, szCaption);
}

CGUIButton* CGUI_Impl::CreateButton(CGUITab* pParent, const char* szCaption)
{
    CGUITab_Impl* wnd = reinterpret_cast<CGUITab_Impl*>(pParent);
    return _CreateButton(wnd, szCaption);
}

CGUICheckBox* CGUI_Impl::CreateCheckBox(CGUIElement* pParent, const char* szCaption, bool bChecked)
{
    CGUIWindow_Impl* wnd = reinterpret_cast<CGUIWindow_Impl*>(pParent);
    return _CreateCheckBox(wnd, szCaption, bChecked);
}

CGUICheckBox* CGUI_Impl::CreateCheckBox(CGUITab* pParent, const char* szCaption, bool bChecked)
{
    CGUITab_Impl* wnd = reinterpret_cast<CGUITab_Impl*>(pParent);
    return _CreateCheckBox(wnd, szCaption, bChecked);
}

CGUIRadioButton* CGUI_Impl::CreateRadioButton(CGUIElement* pParent, const char* szCaption)
{
    CGUIWindow_Impl* wnd = reinterpret_cast<CGUIWindow_Impl*>(pParent);
    return _CreateRadioButton(wnd, szCaption);
}

CGUIRadioButton* CGUI_Impl::CreateRadioButton(CGUITab* pParent, const char* szCaption)
{
    CGUITab_Impl* wnd = reinterpret_cast<CGUITab_Impl*>(pParent);
    return _CreateRadioButton(wnd, szCaption);
}

CGUIEdit* CGUI_Impl::CreateEdit(CGUIElement* pParent, const char* szText)
{
    CGUIWindow_Impl* wnd = reinterpret_cast<CGUIWindow_Impl*>(pParent);
    return _CreateEdit(wnd, szText);
}

CGUIEdit* CGUI_Impl::CreateEdit(CGUITab* pParent, const char* szText)
{
    CGUITab_Impl* wnd = reinterpret_cast<CGUITab_Impl*>(pParent);
    return _CreateEdit(wnd, szText);
}

CGUIGridList* CGUI_Impl::CreateGridList(CGUIElement* pParent, bool bFrame)
{
    CGUIWindow_Impl* wnd = reinterpret_cast<CGUIWindow_Impl*>(pParent);
    return _CreateGridList(wnd, bFrame);
}

CGUIGridList* CGUI_Impl::CreateGridList(CGUITab* pParent, bool bFrame)
{
    CGUITab_Impl* wnd = reinterpret_cast<CGUITab_Impl*>(pParent);
    return _CreateGridList(wnd, bFrame);
}

CGUILabel* CGUI_Impl::CreateLabel(CGUIElement* pParent, const char* szCaption)
{
    CGUIWindow_Impl* wnd = reinterpret_cast<CGUIWindow_Impl*>(pParent);
    return _CreateLabel(wnd, szCaption);
}

CGUILabel* CGUI_Impl::CreateLabel(CGUITab* pParent, const char* szCaption)
{
    CGUITab_Impl* wnd = reinterpret_cast<CGUITab_Impl*>(pParent);
    return _CreateLabel(wnd, szCaption);
}

CGUILabel* CGUI_Impl::CreateLabel(const char* szCaption)
{
    return _CreateLabel(NULL, szCaption);
}

CGUIProgressBar* CGUI_Impl::CreateProgressBar(CGUIElement* pParent)
{
    CGUIWindow_Impl* wnd = reinterpret_cast<CGUIWindow_Impl*>(pParent);
    return _CreateProgressBar(wnd);
}

CGUIProgressBar* CGUI_Impl::CreateProgressBar(CGUITab* pParent)
{
    CGUITab_Impl* wnd = reinterpret_cast<CGUITab_Impl*>(pParent);
    return _CreateProgressBar(wnd);
}

CGUIMemo* CGUI_Impl::CreateMemo(CGUIElement* pParent, const char* szText)
{
    CGUIWindow_Impl* wnd = reinterpret_cast<CGUIWindow_Impl*>(pParent);
    return _CreateMemo(wnd, szText);
}

CGUIMemo* CGUI_Impl::CreateMemo(CGUITab* pParent, const char* szText)
{
    CGUITab_Impl* wnd = reinterpret_cast<CGUITab_Impl*>(pParent);
    return _CreateMemo(wnd, szText);
}

CGUIStaticImage* CGUI_Impl::CreateStaticImage(CGUIElement* pParent)
{
    CGUIWindow_Impl* wnd = reinterpret_cast<CGUIWindow_Impl*>(pParent);
    return _CreateStaticImage(wnd);
}

CGUIStaticImage* CGUI_Impl::CreateStaticImage(CGUITab* pParent)
{
    CGUITab_Impl* wnd = reinterpret_cast<CGUITab_Impl*>(pParent);
    return _CreateStaticImage(wnd);
}

CGUIStaticImage* CGUI_Impl::CreateStaticImage(CGUIGridList* pParent)
{
    CGUIGridList_Impl* wnd = reinterpret_cast<CGUIGridList_Impl*>(pParent);
    return _CreateStaticImage(wnd);
}

CGUIStaticImage* CGUI_Impl::CreateStaticImage()
{
    return _CreateStaticImage(NULL);
}

CGUITabPanel* CGUI_Impl::CreateTabPanel(CGUIElement* pParent)
{
    CGUIWindow_Impl* wnd = reinterpret_cast<CGUIWindow_Impl*>(pParent);
    return _CreateTabPanel(wnd);
}

CGUITabPanel* CGUI_Impl::CreateTabPanel(CGUITab* pParent)
{
    CGUITab_Impl* wnd = reinterpret_cast<CGUITab_Impl*>(pParent);
    return _CreateTabPanel(wnd);
}

CGUITabPanel* CGUI_Impl::CreateTabPanel()
{
    return _CreateTabPanel(NULL);
}

CGUIScrollPane* CGUI_Impl::CreateScrollPane()
{
    return _CreateScrollPane(NULL);
}

CGUIScrollPane* CGUI_Impl::CreateScrollPane(CGUIElement* pParent)
{
    CGUIWindow_Impl* wnd = reinterpret_cast<CGUIWindow_Impl*>(pParent);
    return _CreateScrollPane(wnd);
}

CGUIScrollPane* CGUI_Impl::CreateScrollPane(CGUITab* pParent)
{
    CGUITab_Impl* wnd = reinterpret_cast<CGUITab_Impl*>(pParent);
    return _CreateScrollPane(wnd);
}

CGUIScrollBar* CGUI_Impl::CreateScrollBar(bool bHorizontal, CGUIElement* pParent)
{
    CGUIWindow_Impl* wnd = reinterpret_cast<CGUIWindow_Impl*>(pParent);
    return _CreateScrollBar(bHorizontal, wnd);
}

CGUIScrollBar* CGUI_Impl::CreateScrollBar(bool bHorizontal, CGUITab* pParent)
{
    CGUITab_Impl* wnd = reinterpret_cast<CGUITab_Impl*>(pParent);
    return _CreateScrollBar(bHorizontal, wnd);
}

CGUIComboBox* CGUI_Impl::CreateComboBox(CGUIElement* pParent, const char* szCaption)
{
    CGUIWindow_Impl* wnd = reinterpret_cast<CGUIWindow_Impl*>(pParent);
    return _CreateComboBox(wnd, szCaption);
}

CGUIComboBox* CGUI_Impl::CreateComboBox(CGUIComboBox* pParent, const char* szCaption)
{
    CGUIComboBox_Impl* wnd = reinterpret_cast<CGUIComboBox_Impl*>(pParent);
    return _CreateComboBox(wnd, szCaption);
}

CGUIWebBrowser* CGUI_Impl::CreateWebBrowser(CGUIElement* pParent)
{
    CGUIWindow_Impl* wnd = reinterpret_cast<CGUIWindow_Impl*>(pParent);
    return _CreateWebBrowser(wnd);
}

CGUIWebBrowser* CGUI_Impl::CreateWebBrowser(CGUITab* pParent)
{
    CGUITab_Impl* wnd = reinterpret_cast<CGUITab_Impl*>(pParent);
    return _CreateWebBrowser(wnd);
}

void CGUI_Impl::CleanDeadPool()
{
    if (m_pWindowManager)
        m_pWindowManager->cleanDeadPool();
}

void CGUI_Impl::ClearInputHandlers(eInputChannel channel)
{
    CHECK_CHANNEL(channel);
    m_CharacterKeyHandlers[channel] = GUI_CALLBACK_KEY();
    m_KeyDownHandlers[channel] = GUI_CALLBACK_KEY();
    m_MouseClickHandlers[channel] = GUI_CALLBACK_MOUSE();
    m_MouseDoubleClickHandlers[channel] = GUI_CALLBACK_MOUSE();
    m_MouseButtonDownHandlers[channel] = GUI_CALLBACK_MOUSE();
    m_MouseButtonUpHandlers[channel] = GUI_CALLBACK_MOUSE();
    m_MouseMoveHandlers[channel] = GUI_CALLBACK_MOUSE();
    m_MouseEnterHandlers[channel] = GUI_CALLBACK_MOUSE();
    m_MouseLeaveHandlers[channel] = GUI_CALLBACK_MOUSE();
    m_MouseWheelHandlers[channel] = GUI_CALLBACK_MOUSE();
    m_MovedHandlers[channel] = GUI_CALLBACK();
    m_SizedHandlers[channel] = GUI_CALLBACK();
    m_FocusGainedHandlers[channel] = GUI_CALLBACK_FOCUS();
    m_FocusLostHandlers[channel] = GUI_CALLBACK_FOCUS();
}

void CGUI_Impl::ClearSystemKeys()
{
    // Unpress any held system keys
    unsigned int uiSysKeys = CEGUI::System::getSingleton().getSystemKeys();

    if (uiSysKeys & CEGUI::Control)
        ProcessKeyboardInput(CGUIKeys::LeftControl, false);
    if (uiSysKeys & CEGUI::Shift)
        ProcessKeyboardInput(CGUIKeys::LeftShift, false);
    if (uiSysKeys & CEGUI::Alt)
        ProcessKeyboardInput(CGUIKeys::LeftAlt, false);
}

CEGUI::Window* CGUI_Impl::GetMasterWindow(CEGUI::Window* wnd)
{
    // A titlebar should always return the parent (i.e. the frame window)
    if (wnd->testClassName(CEGUI::Titlebar::EventNamespace))
    {
        if (wnd->getParent())
            return wnd->getParent();
        return wnd;
    }

    // if there's no CEGUI userdata, we deduce that it's not an MTA gui element
    if (!wnd->getUserData())
    {
        CEGUI::Window* parent = wnd->getParent();
        // It was created by CEGUI, probably as a child widget.
        // So keep propogating upwards until we find an MTA element
        while (parent)
        {
            if (parent->getUserData())
                return parent;
            parent = parent->getParent();
        }
    }
    return wnd;
}
