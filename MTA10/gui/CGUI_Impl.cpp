/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUI_Impl.cpp
*  PURPOSE:     Graphical User Interface module class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CEGUIExceptions.h"

using std::list;

#define CGUI_MTA_DEFAULT_FONT       "tahoma.ttf"        // %WINDIR%/font/<...>
#define CGUI_MTA_DEFAULT_FONT_BOLD  "tahomabd.ttf"      // %WINDIR%/font/<...>
#define CGUI_MTA_CLEAR_FONT         "verdana.ttf"       // %WINDIR%/font/<...>
#define CGUI_MTA_SANS_FONT          "cgui/sans.ttf"     // GTA/MTA/<...>
#define CGUI_SA_HEADER_FONT         "cgui/saheader.ttf" // GTA/MTA/<...>
#define CGUI_SA_GOTHIC_FONT         "cgui/sagothic.ttf" // GTA/MTA/<...>
#define CGUI_SA_HEADER_SIZE         26
#define CGUI_SA_GOTHIC_SIZE         47
#define CGUI_MTA_SANS_FONT_SIZE     9

CGUI_Impl::CGUI_Impl ( IDirect3DDevice9* pDevice )
{
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
    m_pRenderer = new CEGUI::DirectX9Renderer ( pDevice, 0 );
    m_pSystem = new CEGUI::System ( m_pRenderer );

    // Get pointers to various stuff from CEGUI singletons
    m_pFontManager = CEGUI::FontManager::getSingletonPtr ();
    m_pImageSetManager = CEGUI::ImagesetManager::getSingletonPtr ();
    m_pSchemeManager = CEGUI::SchemeManager::getSingletonPtr ();
    m_pWindowManager = CEGUI::WindowManager::getSingletonPtr ();

    // Set logging to Informative for debug and Standard for release
#if _DEBUG
    CEGUI::Logger::getSingleton().setLoggingLevel ( CEGUI::Informative );
#else
    CEGUI::Logger::getSingleton().setLoggingLevel ( CEGUI::Standard );
#endif
    CEGUI::Logger::getSingleton().setLogFilename ( "CEGUI.log" );

    // Load our fonts
    SString strFontsPath = PathJoin ( SharedUtil::GetWindowsDirectory (), "fonts" );
	try
	{

        m_pDefaultFont = (CGUIFont_Impl*) CreateFnt ( "default-normal", PathJoin ( strFontsPath, CGUI_MTA_DEFAULT_FONT ), 9, 0 );
        m_pSmallFont = (CGUIFont_Impl*) CreateFnt ( "default-small", PathJoin ( strFontsPath, CGUI_MTA_DEFAULT_FONT ), 7, 0 );

        m_pBoldFont = (CGUIFont_Impl*) CreateFnt ( "default-bold-small", PathJoin ( strFontsPath, CGUI_MTA_DEFAULT_FONT_BOLD ), 8, 0 );

        m_pClearFont = (CGUIFont_Impl*) CreateFnt ( "clear-normal", PathJoin ( strFontsPath, CGUI_MTA_CLEAR_FONT ), 9 );
        m_pSAHeaderFont = (CGUIFont_Impl*) CreateFnt ( "sa-header", CGUI_SA_HEADER_FONT, CGUI_SA_HEADER_SIZE, 0, NULL, true );
        m_pSAGothicFont = (CGUIFont_Impl*) CreateFnt ( "sa-gothic", CGUI_SA_GOTHIC_FONT, CGUI_SA_GOTHIC_SIZE, 0, NULL, true );
        m_pSansFont = (CGUIFont_Impl*) CreateFnt ( "sans", CGUI_MTA_SANS_FONT, CGUI_MTA_SANS_FONT_SIZE, 0, NULL, false );
	}
	catch ( CEGUI::InvalidRequestException e )
	{
        SString strMessage = e.getMessage ().c_str ();
        BrowseToSolution ( "create-fonts", true, true, true, SString ( "Error loading fonts!\n\n%s", *strMessage ) );
	}

    // Load bluescheme
    CEGUI::SchemeManager::getSingleton().loadScheme("cgui/CGUI.xml");
    CEGUI::System::getSingleton().setDefaultMouseCursor("CGUI-Images", "MouseArrow");

    // Create dummy GUI root
    m_pTop = reinterpret_cast < CEGUI::DefaultWindow* > ( m_pWindowManager->createWindow ( "DefaultWindow", "guiroot" ) );
    m_pSystem->setGUISheet ( m_pTop );

    // Disable single click timeouts
    m_pSystem->setSingleClickTimeout ( 100000000.0f );

    // Set our default font
    m_pSystem->setDefaultFont ( m_pDefaultFont->GetFont () );

    // Grab our default cursor
    m_pCursor = m_pSystem->getDefaultMouseCursor ();

    // Used to create unique names for widget instances
    m_ulPreviousUnique = 0;

    // Mouse events
    CEGUI::GlobalEventSet * pEvents = CEGUI::GlobalEventSet::getSingletonPtr ();

    pEvents->subscribeEvent ( "Window/" + CEGUI::Window::EventCharacterKey      , CEGUI::Event::Subscriber ( &CGUI_Impl::Event_CharacterKey, this ) );
    pEvents->subscribeEvent ( "Window/" + CEGUI::Window::EventKeyDown           , CEGUI::Event::Subscriber ( &CGUI_Impl::Event_KeyDown, this ) );
    pEvents->subscribeEvent ( "Window/" + CEGUI::Window::EventMouseClick        , CEGUI::Event::Subscriber ( &CGUI_Impl::Event_MouseClick, this ) );
    pEvents->subscribeEvent ( "Window/" + CEGUI::Window::EventMouseDoubleClick  , CEGUI::Event::Subscriber ( &CGUI_Impl::Event_MouseDoubleClick, this ) );
    pEvents->subscribeEvent ( "Window/" + CEGUI::Window::EventMouseButtonDown   , CEGUI::Event::Subscriber ( &CGUI_Impl::Event_MouseButtonDown, this ) );
    pEvents->subscribeEvent ( "Window/" + CEGUI::Window::EventMouseButtonUp     , CEGUI::Event::Subscriber ( &CGUI_Impl::Event_MouseButtonUp, this ) );
    pEvents->subscribeEvent ( "Window/" + CEGUI::Window::EventMouseWheel        , CEGUI::Event::Subscriber ( &CGUI_Impl::Event_MouseWheel, this ) );
    pEvents->subscribeEvent ( "Window/" + CEGUI::Window::EventMouseMove         , CEGUI::Event::Subscriber ( &CGUI_Impl::Event_MouseMove, this ) );
    pEvents->subscribeEvent ( "Window/" + CEGUI::Window::EventMouseEnters       , CEGUI::Event::Subscriber ( &CGUI_Impl::Event_MouseEnter, this ) );
    pEvents->subscribeEvent ( "Window/" + CEGUI::Window::EventMouseLeaves       , CEGUI::Event::Subscriber ( &CGUI_Impl::Event_MouseLeave, this ) );
    pEvents->subscribeEvent ( "Window/" + CEGUI::Window::EventMoved             , CEGUI::Event::Subscriber ( &CGUI_Impl::Event_Moved, this ) );
    pEvents->subscribeEvent ( "Window/" + CEGUI::Window::EventSized             , CEGUI::Event::Subscriber ( &CGUI_Impl::Event_Sized, this ) );
    pEvents->subscribeEvent ( "Window/" + CEGUI::Window::EventRedrawRequested   , CEGUI::Event::Subscriber ( &CGUI_Impl::Event_RedrawRequested, this ) );

    // Disallow input routing to the GUI
    m_bSwitchGUIInput = false;

    // Reset the working directory
    m_szWorkingDirectory[MAX_PATH] = 0;

    // The transfer box is not visible by default
    m_bTransferBoxVisible = false;
}


CGUI_Impl::~CGUI_Impl ( void )
{

}


CVector2D CGUI_Impl::GetResolution ( void )
{
    return CVector2D ( m_pRenderer->getWidth(), m_pRenderer->getHeight() );
}


void CGUI_Impl::SetResolution ( float fWidth, float fHeight )
{
    reinterpret_cast < CEGUI::DirectX9Renderer* > ( m_pRenderer ) -> setDisplaySize ( CEGUI::Size ( fWidth, fHeight ) );
}


void CGUI_Impl::Draw ( void )
{
    // Redraw the changed elements
    if ( !m_RedrawQueue.empty() )
    {
        list < CGUIElement* > ::const_iterator iter = m_RedrawQueue.begin ();
        for ( ; iter != m_RedrawQueue.end (); iter++ )
        {
            (*iter)->ForceRedraw();
        }
        m_RedrawQueue.clear ();
    }

    m_pSystem->renderGUI ();
}


void CGUI_Impl::Invalidate ( void )
{
    reinterpret_cast < CEGUI::DirectX9Renderer* > ( m_pRenderer ) -> preD3DReset ();
}


void CGUI_Impl::Restore ( void )
{
    try
    {
        reinterpret_cast < CEGUI::DirectX9Renderer* > ( m_pRenderer ) -> postD3DReset ();
    }
    catch ( CEGUI::RendererException& exception )
    {
        MessageBox ( 0, exception.getMessage().c_str (), "CEGUI Exception", MB_OK|MB_ICONERROR );
        TerminateProcess ( GetCurrentProcess (), 1 );
    }
}

void CGUI_Impl::DrawMouseCursor ( void )
{
    CEGUI::MouseCursor::getSingleton ().draw ();
}

void CGUI_Impl::ProcessMouseInput ( CGUIMouseInput eMouseInput, unsigned long ulX, unsigned long ulY, CGUIMouseButton eMouseButton )
{
    switch ( eMouseInput )
    {
        case CGUI_MI_MOUSEMOVE:
            m_pSystem->injectMouseMove ( static_cast < float > ( ulX ), static_cast < float > ( ulY ) );
            break;

        case CGUI_MI_MOUSEPOS:
            m_pSystem->injectMousePosition ( static_cast < float > ( ulX ), static_cast < float > ( ulY ) );
            break;

        case CGUI_MI_MOUSEDOWN:
            m_pSystem->injectMouseButtonDown ( static_cast < CEGUI::MouseButton > ( eMouseButton ) );
            break;

        case CGUI_MI_MOUSEUP:
            m_pSystem->injectMouseButtonUp ( static_cast < CEGUI::MouseButton > ( eMouseButton ) );
            break;

        case CGUI_MI_MOUSEWHEEL:
            if ( (signed long)ulX > 0 )
                m_pSystem->injectMouseWheelChange ( +1 );
            else
                m_pSystem->injectMouseWheelChange ( -1 );
            break;
    }
}


void CGUI_Impl::ProcessKeyboardInput ( unsigned long ulKey, bool bIsDown )
{
    if ( bIsDown )
    {
        m_pSystem->injectKeyDown ( ulKey );
    }
    else
    {
        m_pSystem->injectKeyUp ( ulKey );
    }
}


void CGUI_Impl::SetGUIInputEnabled ( bool bEnabled )
{   // inline?
    m_bSwitchGUIInput = bEnabled;
}


bool CGUI_Impl::GetGUIInputEnabled ( void )
{   // inline?
    return m_bSwitchGUIInput;
}


void CGUI_Impl::ProcessCharacter ( unsigned long ulCharacter )
{
    m_pSystem->injectChar ( ulCharacter );
}


CGUIMessageBox* CGUI_Impl::CreateMessageBox ( const char* szTitle, const char* szMessage, unsigned int uiFlags )
{
    return new CGUIMessageBox_Impl ( this, szTitle, szMessage, uiFlags );
}


CGUIButton* CGUI_Impl::_CreateButton ( CGUIElement_Impl* pParent, const char* szCaption )
{
    return new CGUIButton_Impl ( this, pParent, szCaption );
}


CGUICheckBox* CGUI_Impl::_CreateCheckBox ( CGUIElement_Impl* pParent, const char* szCaption, bool bChecked )
{
    return new CGUICheckBox_Impl ( this, pParent, szCaption, bChecked );
}


CGUIRadioButton* CGUI_Impl::_CreateRadioButton ( CGUIElement_Impl* pParent, const char* szCaption )
{
    return new CGUIRadioButton_Impl ( this, pParent, szCaption );
}


CGUIEdit* CGUI_Impl::_CreateEdit ( CGUIElement_Impl* pParent, const char* szText )
{
    return new CGUIEdit_Impl ( this, pParent, szText );
}


CGUIFont* CGUI_Impl::CreateFnt ( const char* szFontName, const char* szFontFile, unsigned int uSize, unsigned int uFlags, unsigned int uExtraGlyphs[], bool bAutoScale )
{
    return new CGUIFont_Impl ( this, szFontName, szFontFile, uSize, uFlags, uExtraGlyphs, bAutoScale );
}


CGUIGridList* CGUI_Impl::_CreateGridList ( CGUIElement_Impl* pParent, bool bFrame )
{
    return new CGUIGridList_Impl ( this, pParent, bFrame );
}


CGUILabel* CGUI_Impl::_CreateLabel ( CGUIElement_Impl* pParent, const char* szCaption )
{
    return new CGUILabel_Impl ( this, pParent, szCaption );
}


CGUIProgressBar* CGUI_Impl::_CreateProgressBar ( CGUIElement_Impl* pParent )
{
    return new CGUIProgressBar_Impl ( this, pParent );
}


CGUIMemo* CGUI_Impl::_CreateMemo ( CGUIElement_Impl* pParent, const char* szText )
{
    return new CGUIMemo_Impl ( this, pParent, szText );
}


CGUIStaticImage* CGUI_Impl::_CreateStaticImage ( CGUIElement_Impl* pParent )
{
    return new CGUIStaticImage_Impl ( this, pParent );
}


CGUITabPanel* CGUI_Impl::_CreateTabPanel ( CGUIElement_Impl* pParent )
{
    return new CGUITabPanel_Impl ( this, pParent );
}


CGUIScrollPane* CGUI_Impl::_CreateScrollPane ( CGUIElement_Impl* pParent )
{
    return new CGUIScrollPane_Impl ( this, pParent );
}


CGUIScrollBar* CGUI_Impl::_CreateScrollBar ( bool bHorizontal, CGUIElement_Impl* pParent )
{
    return new CGUIScrollBar_Impl ( this, bHorizontal, pParent );
}


CGUIComboBox* CGUI_Impl::_CreateComboBox ( CGUIElement_Impl* pParent, const char* szCaption )
{
    return new CGUIComboBox_Impl ( this, pParent, szCaption );
}


CGUITexture* CGUI_Impl::CreateTexture ( void )
{
    return new CGUITexture_Impl ( this );
}


CGUIWindow* CGUI_Impl::CreateWnd ( CGUIElement* pParent, const char* szCaption )
{
    return new CGUIWindow_Impl ( this, pParent, szCaption );
}


void CGUI_Impl::SetCursorEnabled ( bool bEnabled )
{
    if ( bEnabled )
    {
        CEGUI::MouseCursor::getSingleton ().show ();
    }
    else
    {
        CEGUI::MouseCursor::getSingleton ().hide ();
    }
}


bool CGUI_Impl::IsCursorEnabled ( void )
{
    return CEGUI::MouseCursor::getSingleton ().isVisible ();
}


void CGUI_Impl::AddChild ( CGUIElement_Impl* pChild )
{
    m_pTop->addChildWindow ( pChild->GetWindow () );
}

CGUIWindow* CGUI_Impl::LoadLayout ( CGUIElement* pParent, const SString& strFilename )
{
    try
    {
        return new CGUIWindow_Impl ( this, pParent, "szCaption", strFilename );
    }
	catch (...)
    {
        return NULL;
    }
}

bool CGUI_Impl::LoadImageset ( const SString& strFilename )
{
    try
    {
        return GetImageSetManager()->createImageset ( strFilename ) != NULL;
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


CEGUI::FontManager* CGUI_Impl::GetFontManager ( void )
{
    return m_pFontManager;
}


CEGUI::ImagesetManager* CGUI_Impl::GetImageSetManager ( void )
{
    return m_pImageSetManager;
}


CEGUI::System* CGUI_Impl::GetGUISystem ( void )
{
    return m_pSystem;
}


CEGUI::Renderer* CGUI_Impl::GetRenderer ( void )
{
    return m_pRenderer;
}


CEGUI::SchemeManager* CGUI_Impl::GetSchemeManager ( void )
{
    return m_pSchemeManager;
}


CEGUI::WindowManager* CGUI_Impl::GetWindowManager ( void )
{
    return m_pWindowManager;
}


void CGUI_Impl::GetUniqueName ( char* pBuf )
{
    _snprintf ( pBuf, CGUI_CHAR_SIZE, "%x", m_ulPreviousUnique );
    m_ulPreviousUnique++;
}


bool CGUI_Impl::Event_CharacterKey ( const CEGUI::EventArgs& Args )
{
    if ( m_CharacterKeyHandlers[ m_Channel ] )
    {
        const CEGUI::KeyEventArgs& e = reinterpret_cast < const CEGUI::KeyEventArgs& > ( Args );
        CGUIKeyEventArgs NewArgs;

        // copy the variables
        NewArgs.codepoint = e.codepoint;
        NewArgs.scancode = (CGUIKeys::Scan) e.scancode;
        NewArgs.sysKeys = e.sysKeys;

        // get the CGUIElement
        CGUIElement * pElement = reinterpret_cast < CGUIElement* > ( ( e.window )->getUserData () );
        NewArgs.pWindow = pElement;

        m_CharacterKeyHandlers[ m_Channel ] ( NewArgs );
    }
    return true;
}


CGUIFont* CGUI_Impl::GetDefaultFont ( void )
{
    return m_pDefaultFont;
}


CGUIFont* CGUI_Impl::GetSmallFont ( void )
{
    return m_pSmallFont;
}


CGUIFont* CGUI_Impl::GetBoldFont ( void )
{
    return m_pBoldFont;
}


CGUIFont* CGUI_Impl::GetClearFont ( void )
{
    return m_pClearFont;
}


CGUIFont* CGUI_Impl::GetSAHeaderFont ( void )
{
    return m_pSAHeaderFont;
}


CGUIFont* CGUI_Impl::GetSAGothicFont ( void )
{
    return m_pSAGothicFont;
}

CGUIFont* CGUI_Impl::GetSansFont ( void )
{
    return m_pSansFont;
}

bool CGUI_Impl::Event_KeyDown ( const CEGUI::EventArgs& Args )
{
    // Cast it to a set of keyboard arguments
    const CEGUI::KeyEventArgs& KeyboardArgs = reinterpret_cast < const CEGUI::KeyEventArgs& > ( Args );

    switch ( KeyboardArgs.scancode )
    {
        // Cut/Copy keys
        case CEGUI::Key::Scan::X:
        case CEGUI::Key::Scan::C:
        {
            if ( KeyboardArgs.sysKeys & CEGUI::Control )
            {
                // Data to copy
                CEGUI::String strTemp;

                // Edit boxes
                CEGUI::Window* Wnd = reinterpret_cast < CEGUI::Window* > ( KeyboardArgs.window );
                if ( Wnd->getType () == "CGUI/Editbox" )
                {
                    // Turn our event window into an editbox
                    CEGUI::Editbox* WndEdit = reinterpret_cast < CEGUI::Editbox* > ( Wnd );

                    // Get the text from the editbox
                    size_t sizeSelectionStart = WndEdit->getSelectionStartIndex ();
                    size_t sizeSelectionLength = WndEdit->getSelectionLength ();
                    strTemp = WndEdit->getText ().substr ( sizeSelectionStart, sizeSelectionLength );

                    // If the user cut, remove the text too
                    if ( KeyboardArgs.scancode == CEGUI::Key::Scan::X )
                    {
                        // Read only?
                        if ( !WndEdit->isReadOnly () )
                        {
                            // Remove the text from the source
                            CEGUI::String strTemp2 = WndEdit->getText ();
                            strTemp2.replace ( sizeSelectionStart, sizeSelectionLength, "", 0 );
                            WndEdit->setText ( strTemp2 );
                        }
                    }
                }

                // Multiline editboxes
                if ( Wnd->getType () == "CGUI/MultiLineEditbox" )
                {
                    // Turn our event window into an editbox
                    CEGUI::MultiLineEditbox* WndEdit = reinterpret_cast < CEGUI::MultiLineEditbox* > ( Wnd );

                    // Get the text from the editbox
                    size_t sizeSelectionStart = WndEdit->getSelectionStartIndex ();
                    size_t sizeSelectionLength = WndEdit->getSelectionLength ();
                    strTemp = WndEdit->getText ().substr ( sizeSelectionStart, sizeSelectionLength );

                    // If the user cut, remove the text too
                    if ( KeyboardArgs.scancode == CEGUI::Key::Scan::X )
                    {
                        // Read only?
                        if ( !WndEdit->isReadOnly () )
                        {
                            // Remove the text from the source
                            CEGUI::String strTemp2 = WndEdit->getText ();
                            strTemp2.replace ( sizeSelectionStart, sizeSelectionLength, "", 0 );
                            WndEdit->setText ( strTemp2 );
                        }
                    }
                }

                // If we got something to copy
                if ( strTemp.length () > 0 )
                {
                    // Open and empty the clipboard
                    OpenClipboard ( NULL );
                    EmptyClipboard ();

                    // Allocate the clipboard buffer and copy the data
                    HGLOBAL hBuf = GlobalAlloc ( GMEM_DDESHARE, strTemp.utf8_stream_len () + 1 );
                    char* buf = reinterpret_cast < char* > ( GlobalLock ( hBuf ) );
                    strcpy ( buf , strTemp.c_str () );
                    GlobalUnlock ( hBuf );

                    // Copy the data into the clipboard
                    SetClipboardData ( CF_TEXT , hBuf );

                    // Close the clipboard
                    CloseClipboard( );
                }
            }

            break;
        }

        // Paste keys
        case CEGUI::Key::Scan::V:
        {
            if ( KeyboardArgs.sysKeys & CEGUI::Control )
            {
                CEGUI::Window* Wnd = reinterpret_cast < CEGUI::Window* > ( KeyboardArgs.window );
                if ( Wnd->getType ( ) == "CGUI/Editbox" )
                {
                    // Open the clipboard
                    OpenClipboard( NULL );

                    // Turn our event window into an editbox
                    CEGUI::Editbox* WndEdit = reinterpret_cast < CEGUI::Editbox* > ( Wnd );

                    // Get the clipboard's data and put it into a char array
                    const char * ClipboardBuffer = reinterpret_cast < const char* > ( GetClipboardData ( CF_TEXT ) );

                    // Check to make sure we have valid data.
                    if ( ClipboardBuffer )
                    {
                        SString strClipboardText ( "%s", ClipboardBuffer );
                        size_t iNewlineIndex;

                        // Remove the newlines inserting spaces instead
                        do
                        {
                            iNewlineIndex = strClipboardText.find ( '\n' );
                            if ( iNewlineIndex != SString::npos )
                            {
                                if ( iNewlineIndex > 0 && strClipboardText[ iNewlineIndex - 1 ] == '\r' )
                                {
                                    // \r\n
                                    strClipboardText [ iNewlineIndex - 1 ] = ' ';
                                    strClipboardText.replace ( iNewlineIndex, strClipboardText.length () - iNewlineIndex,
                                                              (const std::string&)strClipboardText, iNewlineIndex + 1,
                                                              strClipboardText.length () - iNewlineIndex - 1 );
                                }
                                else
                                {
                                    strClipboardText [ iNewlineIndex ] = ' ';
                                }
                            }
                        } while ( iNewlineIndex != SString::npos );

                        // Put the editbox's data into a string and insert the data if it has not reached it's maximum text length
                        CEGUI::String tmp = WndEdit->getText ();
                        if ( ( strClipboardText.length () + tmp.length () ) < WndEdit->getMaxTextLength( ) )
                        {
                            // Are there characters selected?
                            size_t sizeCaratIndex = 0;
                            if ( WndEdit->getSelectionLength () > 0 )
                            {
                                // Replace what's selected with the pasted buffer and set the new carat index
                                tmp.replace ( WndEdit->getSelectionStartIndex (), WndEdit->getSelectionLength (),
                                              strClipboardText.c_str (), strClipboardText.length () );
                                sizeCaratIndex = WndEdit->getSelectionStartIndex () + strClipboardText.length ();
                            }
                            else
                            {
                                // If not, insert the clipboard buffer where we were and set the new carat index
                                tmp.insert ( WndEdit->getSelectionStartIndex (), strClipboardText.c_str (), strClipboardText.length () );
                                sizeCaratIndex = WndEdit->getCaratIndex () + strClipboardText.length ();
                            }

                            // Set the new text and move the carat at the end of what we pasted
                            WndEdit->setText ( tmp );
                            WndEdit->setCaratIndex ( sizeCaratIndex );
                        }
                        else
                        {
                            // Fire an event if the editbox is full
                            WndEdit->fireEvent ( CEGUI::Editbox::EventEditboxFull , CEGUI::WindowEventArgs ( WndEdit ) );
                        }
                    }

                    // Close the clipboard
                    CloseClipboard( );
                }
            }

            break;
        }

        // Select all key
        case CEGUI::Key::Scan::A:
        {
            if ( KeyboardArgs.sysKeys & CEGUI::Control )
            {
                // Edit boxes
                CEGUI::Window* Wnd = reinterpret_cast < CEGUI::Window* > ( KeyboardArgs.window );
                if ( Wnd->getType () == "CGUI/Editbox" )
                {
                    // Turn our event window into an editbox
                    CEGUI::Editbox* WndEdit = reinterpret_cast < CEGUI::Editbox* > ( Wnd );
                    WndEdit->setSelection ( 0, WndEdit->getText ().size () );
                }
                else if ( Wnd->getType () == "CGUI/MultiLineEditbox" )
                {
                    // Turn our event window into a multiline editbox
                    CEGUI::MultiLineEditbox* WndEdit = reinterpret_cast < CEGUI::MultiLineEditbox* > ( Wnd );
                    WndEdit->setSelection ( 0, WndEdit->getText ().size () );
                }
            }

            break;
        }
    }

    // Call the callback if present
    if ( m_KeyDownHandlers[ m_Channel ] )
    {
        const CEGUI::KeyEventArgs& e = reinterpret_cast < const CEGUI::KeyEventArgs& > ( Args );
        CGUIKeyEventArgs NewArgs;

        // copy the variables
        NewArgs.codepoint = e.codepoint;
        NewArgs.scancode = (CGUIKeys::Scan) e.scancode;
        NewArgs.sysKeys = e.sysKeys;

        // get the CGUIElement
        CGUIElement * pElement = reinterpret_cast < CGUIElement* > ( ( e.window )->getUserData () );
        NewArgs.pWindow = pElement;

        m_KeyDownHandlers[ m_Channel ] ( NewArgs );
    }
    return true;
}


void CGUI_Impl::SetWorkingDirectory ( const char * szDir )
{
    unsigned int uiDirLen = ( unsigned int ) strlen ( szDir );

    // Check if the worst possible size fits in the buffer
    if ( ( uiDirLen + 1 ) > MAX_PATH ) return;

    strncpy ( m_szWorkingDirectory, szDir, MAX_PATH );

    // We need a trailing slash, so check for one
    if ( szDir [ uiDirLen - 1 ] != '/' && szDir [ uiDirLen - 1 ] != '\\' ) {
        m_szWorkingDirectory [ uiDirLen ] = '/';
        m_szWorkingDirectory [ uiDirLen + 1 ] = NULL;
    }
}


bool CGUI_Impl::Event_MouseClick ( const CEGUI::EventArgs& Args )
{
    if ( m_MouseClickHandlers[ m_Channel ] )
    {
        const CEGUI::MouseEventArgs& e = reinterpret_cast < const CEGUI::MouseEventArgs& > ( Args );
        CGUIMouseEventArgs NewArgs;

        // get the approriate cegui window
        CEGUI::Window * wnd = e.window;

        // if its a title- or scrollbar, get the appropriate parent
        if ( wnd->testClassName ( CEGUI::Titlebar::EventNamespace ) ||
             wnd->testClassName ( CEGUI::Scrollbar::EventNamespace ) )
             wnd = wnd->getParent ();

        // copy the variables
        NewArgs.button = static_cast < CGUIMouse::MouseButton > ( e.button );
        NewArgs.moveDelta = CVector2D ( e.moveDelta.d_x, e.moveDelta.d_y );
        NewArgs.position = CGUIPosition ( e.position.d_x, e.position.d_y );
        NewArgs.sysKeys = e.sysKeys;
        NewArgs.wheelChange = e.wheelChange;

        // get the CGUIElement
        CGUIElement * pElement = reinterpret_cast < CGUIElement* > ( wnd->getUserData () );
        NewArgs.pWindow = pElement;
        
        m_MouseClickHandlers[ m_Channel ] ( NewArgs );
    }
    return true;
}


bool CGUI_Impl::Event_MouseDoubleClick ( const CEGUI::EventArgs& Args )
{
    if ( m_MouseDoubleClickHandlers[ m_Channel ] )
    {
        const CEGUI::MouseEventArgs& e = reinterpret_cast < const CEGUI::MouseEventArgs& > ( Args );
        CGUIMouseEventArgs NewArgs;

        // get the approriate cegui window
        CEGUI::Window * wnd = e.window;

        // if its a title- or scrollbar, get the appropriate parent
        if ( wnd->testClassName ( CEGUI::Titlebar::EventNamespace ) ||
             wnd->testClassName ( CEGUI::Scrollbar::EventNamespace ) )
             wnd = wnd->getParent ();

        // copy the variables
        NewArgs.button = static_cast < CGUIMouse::MouseButton > ( e.button );
        NewArgs.moveDelta = CVector2D ( e.moveDelta.d_x, e.moveDelta.d_y );
        NewArgs.position = CGUIPosition ( e.position.d_x, e.position.d_y );
        NewArgs.sysKeys = e.sysKeys;
        NewArgs.wheelChange = e.wheelChange;

        // get the CGUIElement
        CGUIElement * pElement = reinterpret_cast < CGUIElement* > ( wnd->getUserData () );
        NewArgs.pWindow = pElement;
        
        m_MouseDoubleClickHandlers[ m_Channel ] ( NewArgs );
    }
    return true;
}

bool CGUI_Impl::Event_MouseButtonDown ( const CEGUI::EventArgs& Args )
{
    if ( m_MouseButtonDownHandlers[ m_Channel ] )
    {
        const CEGUI::MouseEventArgs& e = reinterpret_cast < const CEGUI::MouseEventArgs& > ( Args );
        CGUIMouseEventArgs NewArgs;

        // get the approriate cegui window
        CEGUI::Window * wnd = e.window;

        // if its a title- or scrollbar, get the appropriate parent
        if ( wnd->testClassName ( CEGUI::Titlebar::EventNamespace ) ||
             wnd->testClassName ( CEGUI::Scrollbar::EventNamespace ) )
             wnd = wnd->getParent ();

        // copy the variables
        NewArgs.button = static_cast < CGUIMouse::MouseButton > ( e.button );
        NewArgs.moveDelta = CVector2D ( e.moveDelta.d_x, e.moveDelta.d_y );
        NewArgs.position = CGUIPosition ( e.position.d_x, e.position.d_y );
        NewArgs.sysKeys = e.sysKeys;
        NewArgs.wheelChange = e.wheelChange;

        // get the CGUIElement
        CGUIElement * pElement = reinterpret_cast < CGUIElement* > ( wnd->getUserData () );
        NewArgs.pWindow = pElement;
        
        m_MouseButtonDownHandlers[ m_Channel ] ( NewArgs );
    }
    return true;
}

bool CGUI_Impl::Event_MouseButtonUp ( const CEGUI::EventArgs& Args )
{
    if ( m_MouseButtonUpHandlers[ m_Channel ] )
    {
        const CEGUI::MouseEventArgs& e = reinterpret_cast < const CEGUI::MouseEventArgs& > ( Args );
        CGUIMouseEventArgs NewArgs;

        // get the approriate cegui window
        CEGUI::Window * wnd = e.window;

        // if its a title- or scrollbar, get the appropriate parent
        if ( wnd->testClassName ( CEGUI::Titlebar::EventNamespace ) ||
             wnd->testClassName ( CEGUI::Scrollbar::EventNamespace ) )
             wnd = wnd->getParent ();

        // copy the variables
        NewArgs.button = static_cast < CGUIMouse::MouseButton > ( e.button );
        NewArgs.moveDelta = CVector2D ( e.moveDelta.d_x, e.moveDelta.d_y );
        NewArgs.position = CGUIPosition ( e.position.d_x, e.position.d_y );
        NewArgs.sysKeys = e.sysKeys;
        NewArgs.wheelChange = e.wheelChange;

        // get the CGUIElement
        CGUIElement * pElement = reinterpret_cast < CGUIElement* > ( wnd->getUserData () );
        NewArgs.pWindow = pElement;
        
        m_MouseButtonUpHandlers[ m_Channel ] ( NewArgs );
    }
    return true;
}

bool CGUI_Impl::Event_MouseWheel ( const CEGUI::EventArgs& Args )
{
    if ( m_MouseWheelHandlers[ m_Channel ] )
    {
        const CEGUI::MouseEventArgs& e = reinterpret_cast < const CEGUI::MouseEventArgs& > ( Args );
        CGUIMouseEventArgs NewArgs;

        // get the approriate cegui window
        CEGUI::Window * wnd = e.window;

        // if its a title- or scrollbar, get the appropriate parent
        if ( wnd->testClassName ( CEGUI::Titlebar::EventNamespace ) ||
             wnd->testClassName ( CEGUI::Scrollbar::EventNamespace ) )
             wnd = wnd->getParent ();

        // copy the variables
        NewArgs.button = static_cast < CGUIMouse::MouseButton > ( e.button );
        NewArgs.moveDelta = CVector2D ( e.moveDelta.d_x, e.moveDelta.d_y );
        NewArgs.position = CGUIPosition ( e.position.d_x, e.position.d_y );
        NewArgs.sysKeys = e.sysKeys;
        NewArgs.wheelChange = e.wheelChange;

        // get the CGUIElement
        CGUIElement * pElement = reinterpret_cast < CGUIElement* > ( wnd->getUserData () );
        NewArgs.pWindow = pElement;
        
        m_MouseWheelHandlers[ m_Channel ] ( NewArgs );
    }
    return true;
}


bool CGUI_Impl::Event_MouseMove ( const CEGUI::EventArgs& Args )
{
    if ( m_MouseMoveHandlers[ m_Channel ] )
    {
        const CEGUI::MouseEventArgs& e = reinterpret_cast < const CEGUI::MouseEventArgs& > ( Args );
        CGUIMouseEventArgs NewArgs;

        // get the approriate cegui window
        CEGUI::Window * wnd = e.window;

        // if its a title- or scrollbar, get the appropriate parent
        if ( wnd->testClassName ( CEGUI::Titlebar::EventNamespace ) ||
             wnd->testClassName ( CEGUI::Scrollbar::EventNamespace ) )
             wnd = wnd->getParent ();

        // copy the variables
        NewArgs.button = static_cast < CGUIMouse::MouseButton > ( e.button );
        NewArgs.moveDelta = CVector2D ( e.moveDelta.d_x, e.moveDelta.d_y );
        NewArgs.position = CGUIPosition ( e.position.d_x, e.position.d_y );
        NewArgs.sysKeys = e.sysKeys;
        NewArgs.wheelChange = e.wheelChange;

        // get the CGUIElement
        CGUIElement * pElement = reinterpret_cast < CGUIElement* > ( wnd->getUserData () );
        NewArgs.pWindow = pElement;
        
        m_MouseMoveHandlers[ m_Channel ] ( NewArgs );
    }
    return true;
}


bool CGUI_Impl::Event_MouseEnter ( const CEGUI::EventArgs& Args )
{
    if ( m_MouseEnterHandlers[ m_Channel ] )
    {
        const CEGUI::MouseEventArgs& e = reinterpret_cast < const CEGUI::MouseEventArgs& > ( Args );
        CGUIMouseEventArgs NewArgs;

        // get the approriate cegui window
        CEGUI::Window * wnd = e.window;

        // if its a title- or scrollbar, get the appropriate parent
        if ( wnd->testClassName ( CEGUI::Titlebar::EventNamespace ) ||
             wnd->testClassName ( CEGUI::Scrollbar::EventNamespace ) )
             wnd = wnd->getParent ();

        // copy the variables
        NewArgs.button = static_cast < CGUIMouse::MouseButton > ( e.button );
        NewArgs.moveDelta = CVector2D ( e.moveDelta.d_x, e.moveDelta.d_y );
        NewArgs.position = CGUIPosition ( e.position.d_x, e.position.d_y );
        NewArgs.sysKeys = e.sysKeys;
        NewArgs.wheelChange = e.wheelChange;

        // get the CGUIElement
        CGUIElement * pElement = reinterpret_cast < CGUIElement* > ( wnd->getUserData () );
        NewArgs.pWindow = pElement;
        
        m_MouseEnterHandlers[ m_Channel ] ( NewArgs );
    }
    return true;
}


bool CGUI_Impl::Event_MouseLeave ( const CEGUI::EventArgs& Args )
{
    if ( m_MouseLeaveHandlers[ m_Channel ] )
    {
        const CEGUI::MouseEventArgs& e = reinterpret_cast < const CEGUI::MouseEventArgs& > ( Args );
        CGUIMouseEventArgs NewArgs;

        // get the approriate cegui window
        CEGUI::Window * wnd = e.window;

        // if its a title- or scrollbar, get the appropriate parent
        if ( wnd->testClassName ( CEGUI::Titlebar::EventNamespace ) ||
             wnd->testClassName ( CEGUI::Scrollbar::EventNamespace ) )
             wnd = wnd->getParent ();

        // copy the variables
        NewArgs.button = static_cast < CGUIMouse::MouseButton > ( e.button );
        NewArgs.moveDelta = CVector2D ( e.moveDelta.d_x, e.moveDelta.d_y );
        NewArgs.position = CGUIPosition ( e.position.d_x, e.position.d_y );
        NewArgs.sysKeys = e.sysKeys;
        NewArgs.wheelChange = e.wheelChange;

        // get the CGUIElement
        // ChrML: Need to nullcheck wnd again or it crashes if the window is destroyed
        //        while it is dragged.
        CGUIElement * pElement = NULL;
        if ( wnd )
        {
            pElement = reinterpret_cast < CGUIElement* > ( wnd->getUserData () );
        }

        
        NewArgs.pWindow = pElement;
        
        m_MouseLeaveHandlers[ m_Channel ] ( NewArgs );
    }
    return true;
}


bool CGUI_Impl::Event_Moved ( const CEGUI::EventArgs& Args )
{
    if ( m_MovedHandlers[ m_Channel ] )
    {
        const CEGUI::WindowEventArgs& e = reinterpret_cast < const CEGUI::WindowEventArgs& > ( Args );

        // get the CGUIElement
        CGUIElement * pElement = reinterpret_cast < CGUIElement* > ( ( e.window )->getUserData () );
        
        m_MovedHandlers[ m_Channel ] ( pElement );
    }
    return true;
}


bool CGUI_Impl::Event_Sized ( const CEGUI::EventArgs& Args )
{
    if ( m_SizedHandlers[ m_Channel ] )
    {
        const CEGUI::WindowEventArgs& e = reinterpret_cast < const CEGUI::WindowEventArgs& > ( Args );

        // get the CGUIElement
        CGUIElement * pElement = reinterpret_cast < CGUIElement* > ( ( e.window )->getUserData () );
        
        m_SizedHandlers[ m_Channel ] ( pElement );
    }
    return true;
}


bool CGUI_Impl::Event_RedrawRequested ( const CEGUI::EventArgs& Args )
{
    const CEGUI::WindowEventArgs& e = reinterpret_cast < const CEGUI::WindowEventArgs& > ( Args );
    
    CGUIElement * pElement = reinterpret_cast < CGUIElement* > ( ( e.window )->getUserData () );
    if ( pElement )
        AddToRedrawQueue ( pElement );
    else
        e.window->forceRedraw();

    return true;
}


void CGUI_Impl::AddToRedrawQueue ( CGUIElement* pWindow )
{
    // Manage the redraw queue, if we redraw the parent of the window passed,
    // we should not add it to the redraw queue, and if the children are queued,
    // remove them.
    list < CGUIElement* > ::const_iterator iter = m_RedrawQueue.begin ();
    for ( ; iter != m_RedrawQueue.end (); iter++ )
    {
        if ( pWindow->GetParent() == *iter )
        {
            return;
        }
        else if ( (*iter)->GetParent() == pWindow )
        {
            m_RedrawQueue.remove ( *iter );
            if ( m_RedrawQueue.empty() )
                return;
            iter = m_RedrawQueue.begin ();
        }
        else if ( *iter == pWindow )
        {
            return;
        }
    }
    m_RedrawQueue.push_back ( pWindow );
}


void CGUI_Impl::RemoveFromRedrawQueue ( CGUIElement* pWindow )
{
    m_RedrawQueue.remove ( pWindow );
}


CGUIButton* CGUI_Impl::CreateButton ( CGUIElement* pParent, const char* szCaption )
{
    CGUIWindow_Impl* wnd = reinterpret_cast < CGUIWindow_Impl* > ( pParent );
    return _CreateButton ( wnd, szCaption );
}


CGUIButton* CGUI_Impl::CreateButton ( CGUITab* pParent, const char* szCaption )
{
    CGUITab_Impl* wnd = reinterpret_cast < CGUITab_Impl* > ( pParent );
    return _CreateButton ( wnd, szCaption );
}


CGUICheckBox* CGUI_Impl::CreateCheckBox ( CGUIElement* pParent, const char* szCaption, bool bChecked )
{
    CGUIWindow_Impl* wnd = reinterpret_cast < CGUIWindow_Impl* > ( pParent );
    return _CreateCheckBox ( wnd, szCaption, bChecked );
}


CGUICheckBox* CGUI_Impl::CreateCheckBox ( CGUITab* pParent, const char* szCaption, bool bChecked )
{
    CGUITab_Impl* wnd = reinterpret_cast < CGUITab_Impl* > ( pParent );
    return _CreateCheckBox ( wnd, szCaption, bChecked );
}


CGUIRadioButton* CGUI_Impl::CreateRadioButton ( CGUIElement* pParent, const char* szCaption )
{
    CGUIWindow_Impl* wnd = reinterpret_cast < CGUIWindow_Impl* > ( pParent );
    return _CreateRadioButton ( wnd, szCaption );
}


CGUIRadioButton* CGUI_Impl::CreateRadioButton ( CGUITab* pParent, const char* szCaption )
{
    CGUITab_Impl* wnd = reinterpret_cast < CGUITab_Impl* > ( pParent );
    return _CreateRadioButton ( wnd, szCaption );
}


CGUIEdit* CGUI_Impl::CreateEdit ( CGUIElement* pParent, const char* szText )
{
    CGUIWindow_Impl* wnd = reinterpret_cast < CGUIWindow_Impl* > ( pParent );
    return _CreateEdit ( wnd, szText );
}


CGUIEdit* CGUI_Impl::CreateEdit ( CGUITab* pParent, const char* szText )
{
    CGUITab_Impl* wnd = reinterpret_cast < CGUITab_Impl* > ( pParent );
    return _CreateEdit ( wnd, szText );
}


CGUIGridList* CGUI_Impl::CreateGridList ( CGUIElement* pParent, bool bFrame )
{
    CGUIWindow_Impl* wnd = reinterpret_cast < CGUIWindow_Impl* > ( pParent );
    return _CreateGridList ( wnd, bFrame );
}


CGUIGridList* CGUI_Impl::CreateGridList ( CGUITab* pParent, bool bFrame )
{
    CGUITab_Impl* wnd = reinterpret_cast < CGUITab_Impl* > ( pParent );
    return _CreateGridList ( wnd, bFrame );
}


CGUILabel* CGUI_Impl::CreateLabel ( CGUIElement* pParent, const char* szCaption )
{
    CGUIWindow_Impl* wnd = reinterpret_cast < CGUIWindow_Impl* > ( pParent );
    return _CreateLabel ( wnd, szCaption );
}


CGUILabel* CGUI_Impl::CreateLabel ( CGUITab* pParent, const char* szCaption )
{
    CGUITab_Impl* wnd = reinterpret_cast < CGUITab_Impl* > ( pParent );
    return _CreateLabel ( wnd, szCaption );
}


CGUILabel* CGUI_Impl::CreateLabel ( const char* szCaption )
{
    return _CreateLabel ( NULL, szCaption );
}


CGUIProgressBar* CGUI_Impl::CreateProgressBar ( CGUIElement* pParent )
{
    CGUIWindow_Impl* wnd = reinterpret_cast < CGUIWindow_Impl* > ( pParent );
    return _CreateProgressBar ( wnd );
}


CGUIProgressBar* CGUI_Impl::CreateProgressBar ( CGUITab* pParent )
{
    CGUITab_Impl* wnd = reinterpret_cast < CGUITab_Impl* > ( pParent );
    return _CreateProgressBar ( wnd );
}


CGUIMemo* CGUI_Impl::CreateMemo ( CGUIElement* pParent, const char* szText )
{
    CGUIWindow_Impl* wnd = reinterpret_cast < CGUIWindow_Impl* > ( pParent );
    return _CreateMemo ( wnd, szText );
}


CGUIMemo* CGUI_Impl::CreateMemo ( CGUITab* pParent, const char* szText )
{
    CGUITab_Impl* wnd = reinterpret_cast < CGUITab_Impl* > ( pParent );
    return _CreateMemo ( wnd, szText );
}


CGUIStaticImage* CGUI_Impl::CreateStaticImage ( CGUIElement* pParent )
{
    CGUIWindow_Impl* wnd = reinterpret_cast < CGUIWindow_Impl* > ( pParent );
    return _CreateStaticImage ( wnd );
}


CGUIStaticImage* CGUI_Impl::CreateStaticImage ( CGUITab* pParent )
{
    CGUITab_Impl* wnd = reinterpret_cast < CGUITab_Impl* > ( pParent );
    return _CreateStaticImage ( wnd );
}


CGUIStaticImage* CGUI_Impl::CreateStaticImage ( CGUIGridList* pParent )
{
    CGUIGridList_Impl* wnd = reinterpret_cast < CGUIGridList_Impl* > ( pParent );
    return _CreateStaticImage ( wnd );
}


CGUIStaticImage* CGUI_Impl::CreateStaticImage ( void )
{
    return _CreateStaticImage ( NULL );
}


CGUITabPanel* CGUI_Impl::CreateTabPanel ( CGUIElement* pParent )
{
    CGUIWindow_Impl* wnd = reinterpret_cast < CGUIWindow_Impl* > ( pParent );
    return _CreateTabPanel ( wnd );
}


CGUITabPanel* CGUI_Impl::CreateTabPanel ( CGUITab* pParent )
{
    CGUITab_Impl* wnd = reinterpret_cast < CGUITab_Impl* > ( pParent );
    return _CreateTabPanel ( wnd );
}


CGUIScrollPane* CGUI_Impl::CreateScrollPane ( CGUIElement* pParent )
{
    CGUIWindow_Impl* wnd = reinterpret_cast < CGUIWindow_Impl* > ( pParent );
    return _CreateScrollPane ( wnd );
}


CGUIScrollPane* CGUI_Impl::CreateScrollPane ( CGUITab* pParent )
{
    CGUITab_Impl* wnd = reinterpret_cast < CGUITab_Impl* > ( pParent );
    return _CreateScrollPane ( wnd );
}


CGUIScrollBar* CGUI_Impl::CreateScrollBar ( bool bHorizontal, CGUIElement* pParent )
{
    CGUIWindow_Impl* wnd = reinterpret_cast < CGUIWindow_Impl* > ( pParent );
    return _CreateScrollBar ( bHorizontal, wnd );
}


CGUIScrollBar* CGUI_Impl::CreateScrollBar ( bool bHorizontal, CGUITab* pParent )
{
    CGUITab_Impl* wnd = reinterpret_cast < CGUITab_Impl* > ( pParent );
    return _CreateScrollBar ( bHorizontal, wnd );
}


CGUIComboBox* CGUI_Impl::CreateComboBox ( CGUIElement* pParent, const char* szCaption )
{
    CGUIWindow_Impl* wnd = reinterpret_cast < CGUIWindow_Impl* > ( pParent );
    return _CreateComboBox ( wnd, szCaption );
}


CGUIComboBox* CGUI_Impl::CreateComboBox ( CGUIComboBox* pParent, const char* szCaption )
{
    CGUIComboBox_Impl* wnd = reinterpret_cast < CGUIComboBox_Impl* > ( pParent );
    return _CreateComboBox ( wnd, szCaption );
}

void CGUI_Impl::CleanDeadPool ()
{
    if ( m_pWindowManager )
        m_pWindowManager->cleanDeadPool ();
}

void CGUI_Impl::ClearInputHandlers ( eInputChannel channel )
{
    CHECK_CHANNEL ( channel );
    m_CharacterKeyHandlers[ channel ]       = GUI_CALLBACK_KEY ();
    m_KeyDownHandlers[ channel ]            = GUI_CALLBACK_KEY ();
    m_MouseClickHandlers[ channel ]         = GUI_CALLBACK_MOUSE ();
    m_MouseDoubleClickHandlers[ channel ]   = GUI_CALLBACK_MOUSE ();
    m_MouseButtonDownHandlers[ channel ]    = GUI_CALLBACK_MOUSE ();
    m_MouseButtonUpHandlers[ channel ]      = GUI_CALLBACK_MOUSE ();
    m_MouseMoveHandlers[ channel ]          = GUI_CALLBACK_MOUSE ();
    m_MouseEnterHandlers[ channel ]         = GUI_CALLBACK_MOUSE ();
    m_MouseLeaveHandlers[ channel ]         = GUI_CALLBACK_MOUSE ();
    m_MouseWheelHandlers[ channel ]         = GUI_CALLBACK_MOUSE ();
    m_MovedHandlers[ channel ]              = GUI_CALLBACK ();
    m_SizedHandlers[ channel ]              = GUI_CALLBACK ();
}

void CGUI_Impl::ClearSystemKeys ( void )
{
    // Unpress any held system keys
    unsigned int uiSysKeys = CEGUI::System::getSingleton().getSystemKeys();

    if ( uiSysKeys & CEGUI::Control )
        ProcessKeyboardInput ( CGUIKeys::LeftControl, false );
    if ( uiSysKeys & CEGUI::Shift )
        ProcessKeyboardInput ( CGUIKeys::LeftShift, false );
    if ( uiSysKeys & CEGUI::Alt )
        ProcessKeyboardInput ( CGUIKeys::LeftAlt, false );
}