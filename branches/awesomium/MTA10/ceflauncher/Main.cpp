#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cef3/include/cef_app.h>
#include <string>
#include <sstream>

class CV8Handler : public CefV8Handler
{
public:
    typedef void(*JavascriptCallback)(CefRefPtr<CefFrame> frame, const CefV8ValueList& arguments); // C++98 :(

    CV8Handler ( CefRefPtr<CefFrame> frame ) : m_pFrame(frame)
    {
    }
    void Bind ( const std::string& name, JavascriptCallback callback )
    {
        m_FunctionMap[name] = callback;
    }
    virtual bool Execute ( const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception )
    {
        std::map<std::string, JavascriptCallback>::iterator iter = m_FunctionMap.find ( std::string(name) );
        if ( iter != m_FunctionMap.end () )
        {
            iter->second ( m_pFrame, arguments );
            return true;
        }
        return false;
    }
    void Clear ()
    {
        m_FunctionMap.clear ();
    }

    IMPLEMENT_REFCOUNTING ( CV8Handler );
private:
    CefRefPtr<CefFrame> m_pFrame;
    std::map<std::string, JavascriptCallback> m_FunctionMap;
};

class CApp : public CefApp, public CefRenderProcessHandler
{
public:
    CApp()
    {
    }
    virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler () override { return this; };

    // http://magpcss.org/ceforum/apidocs3/projects/(default)/CefRenderProcessHandler.html#OnContextCreated(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E,CefRefPtr%3CCefV8Context%3E) //
    virtual void OnContextCreated ( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context ) override
    {
        // Get global object and create a v8 handler instance
        CefRefPtr<CefV8Value> globalObject = context->GetGlobal ();
        CefRefPtr<CV8Handler> handler = new CV8Handler ( frame );

        // Create MTA object
        CefRefPtr<CefV8Value> mtaObject = CefV8Value::CreateObject ( NULL );

        // Bind C++ function
        handler->Bind ( "triggerEvent", Javascript_triggerEvent );

        // Create and register our functions
        CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction ( "triggerEvent", handler );
        mtaObject->SetValue ( "triggerEvent", func, V8_PROPERTY_ATTRIBUTE_NONE );

        // Assign mtaObject to global object
        globalObject->SetValue ( "mta", mtaObject, V8_PROPERTY_ATTRIBUTE_NONE );
    }

    static void Javascript_triggerEvent ( CefRefPtr<CefFrame> frame, const CefV8ValueList& arguments )
    {
        if ( arguments.size () == 0 )
            return;

        CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create ( "TriggerLuaEvent" );
        CefRefPtr<CefListValue> argList = message->GetArgumentList ();

        // Write the event name
        argList->SetString ( 0, arguments[0]->GetStringValue () );

        // Write number of arguments
        argList->SetInt ( 1, arguments.size () - 1 );

        std::stringstream sstream;

        // Write arguments
        for ( size_t i = 1; i < arguments.size (); ++i )
        {
            int argIndex = i + 1;
            CefRefPtr<CefV8Value> value = arguments[i];

            // Prepare stringstream for next argument (clear and reset states)
            sstream.str ( "" );
            sstream.clear ();

            if ( value->IsBool () )
                sstream << value->GetBoolValue ();
            else if ( value->IsDouble () )
                sstream << value->GetDoubleValue ();
            else if ( value->IsInt () )
                sstream << value->GetIntValue ();
            else if ( value->IsNull () )
                sstream << "nil";
            else if ( value->IsString () )
                sstream << std::string ( value->GetStringValue () );
            else if ( value->IsUInt () )
                sstream << value->GetUIntValue ();
            else
                sstream << "unsupported type";

            argList->SetString ( i + 1, sstream.str () );

        }

        frame->GetBrowser ()->SendProcessMessage ( PID_BROWSER, message );
    }

public:
    IMPLEMENT_REFCOUNTING(CApp);
};


int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdShow, int nCmdShow )
{
    CefMainArgs mainArgs ( hInstance );
    CefRefPtr<CApp> app ( new CApp );

    return CefExecuteProcess ( mainArgs, app, NULL );
}
