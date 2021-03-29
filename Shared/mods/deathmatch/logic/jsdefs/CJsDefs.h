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
class CV8FunctionBase;

class CJsDefs : CDefs
{
public:
    
    // hard error on usage mistakes
    template <auto T>
    static inline void JsArgumentParser(CV8FunctionCallbackBase* Js)
    {
        CLuaFunctionParser<eRuntime::JS, true, nullptr, T > ()(Js, m_pScriptDebugging);
    }
};
