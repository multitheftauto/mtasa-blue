/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/luadefs/CJsCryptDefs.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"
#include <SharedUtil.Crypto.h>
#include <functional>

#ifndef MTA_CLIENT
    #include <v8/CV8Base.h>
    #include "./../Shared/Mtav8/include/async/functions/CV8InlineAsyncFunction.h"
    #include "./../Shared/Mtav8/include/async/functions/CV8PasswordHash.h"
    #include <core/CServerInterface.h>

extern CServerInterface* g_pServerInterface;
#endif

void CJsCryptDefs::LoadFunctions()
{
#ifndef MTA_CLIENT
    CV8ModuleBase* pHashModule = g_pServerInterface->GetV8()->CreateModule("crypt");

    constexpr static const std::pair<const char*, void(*)(CV8FunctionCallbackBase*)> functions[]{
        {"md5", Md5}, {"BCrypt", BCrypt}
    };

    // Add functions
    for (const auto& [name, func] : functions)
        pHashModule->AddFunction(name, func);
#endif
}

#ifndef MTA_CLIENT
void CJsCryptDefs::Md5(CV8FunctionCallbackBase* callback)
{
    std::string str;
    if (callback->ReadString(str))
    {
        callback->ReturnPromise(std::make_unique<CV8InlineAsyncFunction>([str](CV8DelegateBase* delegate) { delegate->Resolve(CLuaCryptDefs::Md5(str)); }));
        return;
    }
    callback->Return(false);
}

void CJsCryptDefs::BCrypt(CV8FunctionCallbackBase* callback)
{
    std::string password;
    double      value;
    if (callback->ReadString(password) && callback->ReadNumber(value))
    {
        callback->ReturnPromise(std::make_unique<CV8PasswordHash>(password, (int)value));
        return;
    }
    callback->Return(false);
}

#endif
