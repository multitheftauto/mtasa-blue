/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        ceflauncher/V8Helpers.h
*  PURPOSE:     V8 helper classes and functions
*
*****************************************************************************/
#include <string>
#include <cef3/include/cef_app.h>
typedef void ( *JavascriptCallback )( CefRefPtr<CefFrame> frame, const CefV8ValueList& arguments );

namespace V8Helpers
{
    class CV8Handler : public CefV8Handler
    {
    public:
        CV8Handler ( CefRefPtr<CefFrame> frame ) : m_pFrame ( frame )
        {
        }
        void Bind ( const std::string& name, JavascriptCallback callback )
        {
            m_FunctionMap[name] = callback;
        }
        virtual bool Execute ( const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception )
        {
            std::map<std::string, JavascriptCallback>::iterator iter = m_FunctionMap.find ( std::string ( name ) );
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

    std::stringstream conversionStream;
    void ConvertV8ArgToString ( std::string& outResult, CefRefPtr<CefV8Value> cefValue )
    {
        // Prepare stringstream for next argument (clear and reset states)
        conversionStream.str ( "" );
        conversionStream.clear ();

        if ( cefValue->IsBool () )
            conversionStream << cefValue->GetBoolValue ();
        else if ( cefValue->IsDouble () )
            conversionStream << cefValue->GetDoubleValue ();
        else if ( cefValue->IsInt () )
            conversionStream << cefValue->GetIntValue ();
        else if ( cefValue->IsNull () )
            conversionStream << "nil";
        else if ( cefValue->IsString () )
            conversionStream << std::string ( cefValue->GetStringValue () );
        else if ( cefValue->IsUInt () )
            conversionStream << cefValue->GetUIntValue ();
        else
            conversionStream << "unsupported type";

        outResult = conversionStream.str ();
    }

    CefRefPtr<CefProcessMessage> SerialiseV8Arguments ( const std::string& messageName, const CefV8ValueList& arguments )
    {
        // Create the process message
        CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create ( messageName );
        CefRefPtr<CefListValue> argList = message->GetArgumentList ();

        // Write the event name
        argList->SetString ( 0, arguments[0]->GetStringValue () );

        // Write number of arguments
        argList->SetInt ( 1, arguments.size () - 1 );

        // Write arguments
        for ( size_t i = 1; i < arguments.size (); ++i )
        {
            std::string strArgument;
            V8Helpers::ConvertV8ArgToString ( strArgument, arguments[i] );
            argList->SetString ( i + 1, strArgument );
        }

        return message;
    }

    void BindV8Function ( CefRefPtr<CV8Handler> handler, CefRefPtr<CefV8Value> object, const std::string& name, JavascriptCallback cb )
    {
        handler->Bind ( name, cb );
        CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction ( name, handler );
        object->SetValue ( name, func, V8_PROPERTY_ATTRIBUTE_NONE );
    }
}