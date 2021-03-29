/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/luadefs/CJsUtilDefs.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#ifndef MTA_CLIENT
    #include <v8/CV8Base.h>
    #include <core/CServerInterface.h>

extern CServerInterface* g_pServerInterface;
#endif

void CJsUtilDefs::LoadFunctions()
{
#ifndef MTA_CLIENT
    CV8ModuleBase* pUtilsModule = g_pServerInterface->GetV8()->CreateModule("utils");

    constexpr static const std::pair<const char*, void (*)(CV8FunctionCallbackBase*)> functions[]{{"print", Print}, {"getTickCount", GetTickCount}};

    // Add functions
    for (const auto& [name, func] : functions)
        pUtilsModule->AddFunction(name, func);
#endif
}

#ifndef MTA_CLIENT
void CJsUtilDefs::GetTickCount(CV8FunctionCallbackBase* callback)
{
    callback->Return(static_cast<double>(GetTickCount64_()));
}

void CJsUtilDefs::Print(CV8FunctionCallbackBase* callback)
{
    //std::stringstream stream;
    //std::string       str;
    //int               count = callback->CountArguments();
    //for (int i = 0; i < count; i++)
    //{
    //    if (!strcmp(callback->GetType(), "string"))
    //    {
    //        if (callback->ReadString(str, false))
    //        {
    //            stream << "\"" << str << "\"";
    //        }
    //    }
    //    else
    //    {
    //        if (callback->ReadString(str, false))
    //        {
    //            stream << str;
    //        }
    //    }
    //    if (i + 1 != count)
    //    {
    //        stream << " ";
    //    }
    //}
    //printf("[%s] %s\n", *GetLocalTimeString(false), stream.str().c_str());
    //callback->Return(true);
}

#endif
