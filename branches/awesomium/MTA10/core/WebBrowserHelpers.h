/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/WebBrowserHelpers.h
*  PURPOSE:     Webbrowser helper classes and functions
*
*****************************************************************************/
#ifndef __WEBBROWSERHELPERS
#define __WEBBROWSERHELPERS

typedef void (*JavascriptCallback)(Awesomium::WebView*, const Awesomium::JSArray& args); // C++98 :(

class CJSMethodHandler : public Awesomium::JSMethodHandler
{
public:
    void Bind ( Awesomium::JSObject& object, const Awesomium::WebString& name, JavascriptCallback callback )
    {
        // Don't handle local jsobjects
        if ( object.type() == Awesomium::kJSObjectType_Local )
            return;

        object.SetCustomMethod ( name, false );
        std::pair<uint, Awesomium::WebString> key ( object.remote_id (), name );
        m_FunctionMap[key] = callback;
    }
    void OnMethodCall ( Awesomium::WebView* pCaller, uint remote_object_id, const Awesomium::WebString& method_name, const Awesomium::JSArray& args )
    {
        std::pair<uint, Awesomium::WebString> key ( remote_object_id, method_name );
        std::map<std::pair<uint, Awesomium::WebString>, JavascriptCallback>::iterator iter = m_FunctionMap.find ( key );
        if ( iter != m_FunctionMap.end () )
        {
            iter->second ( pCaller, args );
        }
    }
    Awesomium::JSValue OnMethodCallWithReturnValue ( Awesomium::WebView* pCaller, unsigned int remote_object_id, const Awesomium::WebString& method_name, const Awesomium::JSArray& args )
    {
        // Todo: Implement this
        return Awesomium::JSValue ( Awesomium::WSLit ("Not implemented") );
    }
    void Clear ()
    {
        m_FunctionMap.clear ();
    }
private:
    std::map<std::pair<uint, Awesomium::WebString>, JavascriptCallback> m_FunctionMap;
};

#endif
