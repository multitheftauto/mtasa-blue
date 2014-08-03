/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CWebView.cpp
*  PURPOSE:     Web view class
*
*****************************************************************************/
#include "StdInc.h"
#include "CWebView.h"

CWebView::CWebView ( unsigned int uiWidth, unsigned int uiHeight, bool bIsLocal )
{
    m_bIsLocal = bIsLocal;

    // Initialise the web session (which holds the actual settings) in in-memory mode
    Awesomium::WebPreferences preferences;
    CVARS_GET ( "browser_plugins", preferences.enable_plugins );
    if ( !bIsLocal )
        CVARS_GET ( "browser_remote_javascript", preferences.enable_javascript );

    Awesomium::WebSession* pWebSession = Awesomium::WebCore::instance ()->CreateWebSession ( Awesomium::WSLit(""), preferences );

    // Create Awesomium webview as offscreen webview
    m_pWebView = Awesomium::WebCore::instance()->CreateWebView ( uiWidth, uiHeight, pWebSession, Awesomium::kWebViewType_Offscreen );

    // Set handlers
    m_pWebView->set_load_listener ( this );
    m_pWebView->set_js_method_handler ( &m_JSMethodHandler );
    m_pWebView->set_view_listener ( this );
}

CWebView::~CWebView()
{
    // Release WebView
    m_pWebView->Destroy ();
}

bool CWebView::LoadURL ( const SString& strURL, bool bFilterEnabled )
{
    Awesomium::WebURL webURL ( CWebCore::ToWebString ( strURL ) );
    if ( bFilterEnabled && g_pCore->GetWebCore ()->GetURLState ( CWebCore::ToSString ( webURL.host() ) ) != eURLState::WEBPAGE_ALLOWED )
        return false;

    m_pWebView->LoadURL ( webURL );
    return true;
}

bool CWebView::IsLoading ()
{
    return m_pWebView->IsLoading ();
}

void CWebView::GetURL ( SString& outURL )
{
    if ( !m_bIsLocal )
        outURL = static_cast<SString> ( CWebCore::ToSString ( m_pWebView->url ().spec () ) );
    else
        outURL = m_strTempURL;
}

void CWebView::GetTitle ( SString& outTitle )
{
    outTitle = static_cast<SString> ( CWebCore::ToSString ( m_pWebView->title () ) );
}

void CWebView::SetRenderingPaused ( bool bPaused )
{
    if ( bPaused )
        m_pWebView->PauseRendering ();
    else
        m_pWebView->ResumeRendering ();
}

void CWebView::SetTransparent ( bool bTransparent )
{
    m_pWebView->SetTransparent ( bTransparent );
}

void CWebView::Focus ()
{
    m_pWebView->Focus ();
}

void CWebView::ExecuteJavascript ( const SString& strJavascriptCode )
{
    m_pWebView->ExecuteJavascript ( CWebCore::ToWebString ( strJavascriptCode ), CWebCore::ToWebString("") );
}

void CWebView::InjectMouseMove ( int iPosX, int iPosY )
{
    m_pWebView->InjectMouseMove ( iPosX, iPosY );
}

void CWebView::InjectMouseDown ( int mouseButton )
{
    m_pWebView->InjectMouseDown ( (Awesomium::MouseButton) mouseButton );
}

void CWebView::InjectMouseUp ( int mouseButton )
{
    m_pWebView->InjectMouseUp ( (Awesomium::MouseButton) mouseButton );
}

void CWebView::InjectMouseWheel ( int iScrollVert, int iScrollHorz )
{
    m_pWebView->InjectMouseWheel ( iScrollVert, iScrollHorz );
}

void CWebView::InjectKeyboardEvent ( const SString& strKey, bool bKeyDown, bool bCharacter )
{
    // Hack fix
    SString key = strKey;
    if (key == " ")
        key = "space";

    Awesomium::WebKeyboardEvent keyboardEvent;
    keyboardEvent.is_system_key = false;
    keyboardEvent.modifiers = Awesomium::WebKeyboardEvent::kModIsAutorepeat;

    if ( !bCharacter )
    {
        // Get BindableKey structure (to be able to process MTA's key names correctly)
        const SBindableKey* pBindableKey = g_pCore->GetKeyBinds ()->GetBindableFromKey ( key.c_str () );
        if ( !pBindableKey )
            return;

        keyboardEvent.virtual_key_code = pBindableKey->ulCode;
        keyboardEvent.type = bKeyDown ? Awesomium::WebKeyboardEvent::kTypeKeyDown : Awesomium::WebKeyboardEvent::kTypeKeyUp;
    }
    else
    {
        std::wstring strCharacter (MbUTF8ToUTF16(strKey));
        if ( strCharacter.length() > 4 )
            return;
        memcpy ( keyboardEvent.text, strCharacter.c_str (), strCharacter.length () * 2 );
        keyboardEvent.type = Awesomium::WebKeyboardEvent::kTypeChar;
    }
    
    m_pWebView->InjectKeyboardEvent ( keyboardEvent );
}

bool CWebView::SetAudioVolume ( float fVolume )
{
    if ( fVolume < 0.0f || fVolume > 1.0f )
        return false;

    // Since the necessary interfaces of the core audio API were introduced in Win7, we've to fallback to HTML5 audio
    SString strJSCode ( "var tags = document.getElementsByTagName('audio'); for (var i=0; i<tags.length; ++i) { tags[i].volume = %f;} " \
        "tags = document.getElementsByTagName('video'); for (var i=0; i<tags.length; ++i) { tags[i].volume = %f;}",
        fVolume, fVolume );

    m_pWebView->ExecuteJavascript ( CWebCore::ToWebString ( strJSCode ), Awesomium::WSLit("") );
    return m_pWebView->session ()->preferences ().enable_javascript;
}


////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: Awesomium::WebViewListener::Load:OnBeginLoadingFrame //
// http://www.awesomium.com/docs/1_7_2/cpp_api/class_awesomium_1_1_web_view_listener_1_1_load.html#ab511ebd71dc641c5df5fea8c30b58335 //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnBeginLoadingFrame ( Awesomium::WebView* pCaller, int64 frame_id, bool bMainFrame, const Awesomium::WebURL& url, bool bErrorPage )
{
    SString strURL;
    ConvertURL ( url, strURL );

    // Trigger navigate event
    m_pEventsInterface->Events_OnNavigate ( strURL, bMainFrame );
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: Awesomium::WebViewListener::Load:OnFinishLoadingFrame //
// http://www.awesomium.com/docs/1_7_2/cpp_api/class_awesomium_1_1_web_view_listener_1_1_load.html#a3cb1ee5563db02f90cd5562c6d8342b1 //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnFinishLoadingFrame ( Awesomium::WebView* pCaller, int64 iFrameId, bool bMainFrame, const Awesomium::WebURL& url )
{
    // Clear the old list
    m_JSMethodHandler.Clear ();

    // Bind javascript global object
    if ( IsLocal () )
    {
        Awesomium::JSValue jsMTA = pCaller->CreateGlobalJavascriptObject ( Awesomium::WSLit("mta")) ;
        if ( jsMTA.IsObject () )
        {
            Awesomium::JSObject& mtaObject = jsMTA.ToObject();
            m_JSMethodHandler.Bind ( mtaObject, Awesomium::WSLit("triggerEvent"), Javascript_triggerEvent );
        }
    }
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: Awesomium::WebViewListener::Load:OnDocumentReady //
// http://www.awesomium.com/docs/1_7_2/cpp_api/class_awesomium_1_1_web_view_listener_1_1_load.html#aed2fe4f10d72079ecd1b0d3006cdb8c2 //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnDocumentReady ( Awesomium::WebView* pCaller, const Awesomium::WebURL& url )
{
    SString strURL;
    ConvertURL ( url, strURL );
    m_pEventsInterface->Events_OnDocumentReady ( strURL );
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: Awesomium::WebViewListener::Load:OnFailLoadingFrame //
// http://www.awesomium.com/docs/1_7_2/cpp_api/class_awesomium_1_1_web_view_listener_1_1_load.html#add78ed509fbf9ae9428371bc5ef00323 //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnFailLoadingFrame ( Awesomium::WebView* pCaller, int64 frame_id, bool bMainFrame, const Awesomium::WebURL& url, int error_code, const Awesomium::WebString& error_desc )
{
    SString strURL;
    ConvertURL ( url, strURL );
    m_pEventsInterface->Events_OnLoadingFailed ( strURL, error_code, CWebCore::ToSString ( error_desc ) );
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: Awesomium::WebViewListener::View:OnChangeCursor //
// http://www.awesomium.com/docs/1_7_2/cpp_api/class_awesomium_1_1_web_view_listener_1_1_view.html#a47bcb396275c917b17358e1ddd0ef771 //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnChangeCursor ( Awesomium::WebView* pCaller, Awesomium::Cursor cursor )
{
    m_pEventsInterface->Events_OnChangeCursor ( static_cast < unsigned char > ( cursor ) );
}

////////////////////////////////////////////////////////////////////
//                                                                //
// Implementation: Awesomium::WebViewListener::View:OnShowCreatedWebView //
// http://www.awesomium.com/docs/1_7_2/cpp_api/class_awesomium_1_1_web_view_listener_1_1_view.html#af42b69bd383b5b69130feb990ad235e2 //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnShowCreatedWebView ( Awesomium::WebView* pCaller, Awesomium::WebView* pNewView, const Awesomium::WebURL& opener_url, const Awesomium::WebURL& target_url, const Awesomium::Rect& initial_pos, bool is_popup )
{
    // Trigger the popup/new tab event
    SString strTagetURL, strOpenerURL;
    ConvertURL ( target_url, strTagetURL );
    ConvertURL ( opener_url, strOpenerURL );
    m_pEventsInterface->Events_OnPopup ( strTagetURL, strOpenerURL, is_popup );

    // Destroy the new view immediately since we want the scripter to handle the popup event via Lua
    pNewView->Destroy ();
}

////////////////////////////////////////////////////////////////////
//                                                                //
//        Static Javascript methods: triggerEvent                 //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::Javascript_triggerEvent ( Awesomium::WebView* pWebView, const Awesomium::JSArray& args )
{
    if ( args.size() == 0 )
        return;

    // Convert JSArray to string array
    std::vector<SString> stringArray;

    for ( unsigned int i = 1; i < args.size(); ++i ) // Ignore first arg as it is the event name
    {
        stringArray.push_back ( CWebCore::ToSString ( args[i].ToString () ) );
    }

    // Pass string array to real triggerEvent
    SString strEventName = CWebCore::ToSString(args[0].ToString());
    CModManager::GetSingleton ().GetCurrentMod ()->WebsiteTriggerEventHandler ( strEventName, stringArray ); // Does anybody know a better way to trigger an event in the deathmatch module?
}

////////////////////////////////////////////////////////////////////
//                                                                //
//        Static Javascript methods: alert                        //
//        Exports the javascript alert to Lua                     //
//                                                                //
////////////////////////////////////////////////////////////////////
/*void CWebView::Javascript_alert ( Awesomium::WebView* pWebView, const Awesomium::JSArray& args )
{
    SString message = "";
    if ( args.size() > 0 )
        message = CWebCore::ToSString ( args[0].ToString () );

    CModManager::GetSingleton().GetCurrentMod()->WebsiteAlertHandler ( message );
}
*/


void CWebView::ConvertURL ( const Awesomium::WebURL& url, SString& convertedURL )
{
    bool isHTTP = url.scheme ().Compare ( Awesomium::WSLit("http") ) == 0 || url.scheme ().Compare ( Awesomium::WSLit("https") ) == 0;
    if ( !isHTTP )
    {
        convertedURL = CWebCore::ToSString ( url.filename () );
    }
    else
    {
        convertedURL = CWebCore::ToSString ( url.spec () );
    }
}
