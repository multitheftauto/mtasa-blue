/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        ceflauncher/CCefApp.h
*  PURPOSE:     CefApp implementation
*
*****************************************************************************/
#include <cef3/include/cef_app.h>
#include <string>
#include <sstream>
#include "V8Helpers.h"
using V8Helpers::CV8Handler;

class CCefApp : public CefApp, public CefRenderProcessHandler
{
public:
    CCefApp()
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

        // Bind V8 --> C++ functions
        V8Helpers::BindV8Function ( handler, mtaObject, "triggerEvent", Javascript_triggerEvent );
        V8Helpers::BindV8Function ( handler, mtaObject, "triggerServerEvent", Javascript_triggerServerEvent );

        // Assign mtaObject to global object
        globalObject->SetValue ( "mta", mtaObject, V8_PROPERTY_ATTRIBUTE_NONE );
    }

    static void Javascript_triggerEvent ( CefRefPtr<CefFrame> frame, const CefV8ValueList& arguments )
    {
        if ( arguments.size () == 0 )
            return;

        CefRefPtr<CefProcessMessage> message = V8Helpers::SerialiseV8Arguments ( "TriggerLuaEvent", arguments );
        frame->GetBrowser ()->SendProcessMessage ( PID_BROWSER, message );
    }

    static void Javascript_triggerServerEvent ( CefRefPtr<CefFrame> frame, const CefV8ValueList& arguments )
    {
        if ( arguments.size () == 0 )
            return;

        CefRefPtr<CefProcessMessage> message = V8Helpers::SerialiseV8Arguments ( "TriggerServerLuaEvent", arguments );
        frame->GetBrowser ()->SendProcessMessage ( PID_BROWSER, message );
    }

public:
    IMPLEMENT_REFCOUNTING(CCefApp);
};
