/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CWebApp.h
 *  PURPOSE:     Web app class
 *
 *****************************************************************************/
#pragma once
#include <cef3/cef/include/cef_app.h>
#include <cef3/cef/include/cef_scheme.h>

class CWebApp : public CefApp, public CefSchemeHandlerFactory
{
public:
    // Error Handler
    static CefRefPtr<CefResourceHandler> HandleError(const SString& strError, unsigned int uiError);

    virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) override;

    // CefSchemeHandlerFactory methods
    virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& scheme_name,
                                                 CefRefPtr<CefRequest> request) override;

    IMPLEMENT_REFCOUNTING(CWebApp);
};
