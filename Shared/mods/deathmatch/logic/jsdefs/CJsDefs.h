/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CJsDefs.h
 *  PURPOSE:     Js definitions base class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CV8FunctionCallbackBase;
class CV8AsyncContextBase;
class CV8FunctionBase;
class CV8ModuleBase;
class CV8Base;

// Use as a return type and return lambda with `CV8AsyncContextBase*` at first argument
// It is not guarantee that lambda will called because promise can be rejected, canceled
// Content of lambda is executed by worker thread!
// Every path of lambda must resolve, reject promise or throw an exception otherwise assert will triggered.
typedef std::function<void(CV8AsyncContextBase*)> Promise;

#ifndef MTA_CLIENT
    #include <core/CServerInterface.h>
extern CServerInterface* g_pServerInterface;
#endif

class CJsDefs : CApiDefs
{
public:
    static void Initialize()
    {
    #ifndef MTA_CLIENT
        m_pJs = g_pServerInterface->GetV8();
    #endif
    }

    // use `throw std::invalid_argument(...)` to throw JS exception
    //
    // C++ type              == JS Type
    // std::string              == String
    // int/float/double   == Number
    // Promise                 == Promise<T>
    template <auto T>
    static inline void JsArgumentParser(CV8FunctionCallbackBase* Js)
    {
        CLuaFunctionParser<true, nullptr, T > ()(Js, m_pScriptDebugging);
    }

protected:
    static CV8Base* m_pJs;

    // Module declared by that class.
    static CV8ModuleBase* m_pModule;
};
