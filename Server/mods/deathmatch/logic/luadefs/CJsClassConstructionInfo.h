/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CJsClassConstructionInfo.h
 *  PURPOSE:     Js class construction
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "v8/CV8ClassBase.h"

template <typename T>
class CJsClassConstructionInfo
{
public:
    template <typename... Ts>
    struct Func : std::monostate
    {
        std::function<T*(Ts...)> func;
    };
    CJsClassConstructionInfo(std::string name, CJsDefs::EClass classId) : classId(classId)
    {
        assert(classId != CJsDefs::EClass::Invalid);
        assert(classId < CJsDefs::EClass::Count);
        v8Class = g_pServerInterface->GetV8()->CreateClass(name, (size_t)classId);
        v8Class->SetSizeOf(sizeof(T));
    }

    //~CJsClassConstructionInfo() = delete;

    // Call when all native functionality get added, save classBase in defs static class.
    CV8ClassBase* Finilize()
    {
        assert(!finilized);
        finilized = true;
        return v8Class;
    }

private:
    template <typename Arg>
    static constexpr Arg ReadArgument(CV8FunctionCallbackBase& args)
    {
        if constexpr (std::is_same_v<Arg, float>)
        {
            float value;
            if (args.ReadNumber(value))
            {
                return value;
            }
            throw new std::invalid_argument("");
        }
        else
        {
            static_assert(0, "Not implemented");
        }
    }

    template <typename Arg, typename... Args>
    static constexpr Arg... ReadArguments(CV8FunctionCallbackBase& args)
    {
        if constexpr (sizeof...(Args) > 0)
        {
            return ReadArguments<Args...>(args);
        }
        return ReadArgument<Arg>(args);
    }

public:

    template<typename... Ts>
    constexpr void SetConstructor()
    {
        std::function<void*(CV8FunctionCallbackBase&, void*)> constructor = [](CV8FunctionCallbackBase& args, void* ptr) {
            if constexpr (sizeof...(Ts) > 0)
            {
                return (void*)new (ptr) T(std::forward<Ts>(ReadArguments<Ts>(args))...);
            }
            return (void*)new (ptr) T{};
        };
        v8Class->SetParametersCount(sizeof...(Ts));
        v8Class->SetConstructorFunction(constructor);
    }

    int              parameters = 0;
    CV8ClassBase*    v8Class;
    CJsDefs::EClass  classId;
    bool             finilized = false;
};
