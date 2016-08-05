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
    CCefApp() {}
    virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler () override { return this; };

    // http://magpcss.org/ceforum/apidocs3/projects/(default)/CefRenderProcessHandler.html#OnFocusedNodeChanged(CefRefPtr%3CCefBrowser%3E,CefRefPtr%3CCefFrame%3E,CefRefPtr%3CCefDOMNode%3E)
    virtual void OnFocusedNodeChanged ( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefDOMNode> node ) override
    {
        if ( m_bHasInputFocus )
        {
            if ( node )
                return;

            // Tell MTA that we lost input focus
            auto message = CefProcessMessage::Create ( "InputFocus" );
            message->GetArgumentList ()->SetBool ( 0, false );
            browser->SendProcessMessage ( PID_BROWSER, message );

            // Set variable to ensure that the event does not trigger twice
            m_bHasInputFocus = false;
            return;
        }
        else
        {
            if ( !node )
                return;

            if ( node->GetType () == CefDOMNode::Type::DOM_NODE_TYPE_ELEMENT && !node->GetFormControlElementType ().empty () )
            {
                auto message = CefProcessMessage::Create ( "InputFocus" );
                message->GetArgumentList ()->SetBool ( 0, true );
                browser->SendProcessMessage ( PID_BROWSER, message );

                // Set variable to ensure that the event does not trigger twice
                m_bHasInputFocus = true;
            }
        }
    }

    virtual void OnRegisterCustomSchemes ( CefRefPtr < CefSchemeRegistrar > registrar ) override
    {
        // Register custom MTA scheme (has to be called in all proceseses)
        registrar->AddCustomScheme ( "mtalocal", false, false, false );
    }


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

public:
    IMPLEMENT_REFCOUNTING(CCefApp);

private:
    bool m_bHasInputFocus = false;
};
