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

template <typename T>
class CJsClassConstructionInfo
{
public:
    CJsClassConstructionInfo(std::string name, EClass classId) : classId(classId)
    {
        assert(classId != EClass::Invalid);
        assert(classId < EClass::Count);
#ifdef MTA_CLIENT
        v8Class = g_pCore->GetV8()->CreateClass(name, (uint16_t)classId);
#else
        v8Class = g_pServerInterface->GetV8()->CreateClass(name, (uint16_t)classId);
#endif
        v8Class->SetSizeOf(sizeof(T));
    }

private:
    template <typename Arg>
    static constexpr Arg ReadArgument(CV8FunctionCallbackBase& args, int index)
    {
        if constexpr (std::is_same_v<Arg, float>)
        {
            float value;
            if (args.ReadNumber(value, index))
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
    static constexpr Arg ReadArguments(CV8FunctionCallbackBase& args, int index)
    {
        index++;
        if constexpr (sizeof...(Args) > 0)
        {
            return ReadArguments<Args...>(args, index);
        }
        return ReadArgument<Arg>(args, index);
    }

public:
    void SetConstructor(void (*callback)(CV8FunctionCallbackBase*)) { v8Class->SetConstructorFunction(callback); }

    void SetInheritance(EClass eBaseClass) { v8Class->SetInheritance((uint16_t)eBaseClass); }

    template <auto P, typename U>
    constexpr void SetAccessor(std::string name)
    {
        if constexpr (std::is_same_v<U, float>)
        {
            v8Class->AddAccessor(
                name, [](void* ptr) { return (T*)ptr->*P; }, [](void* ptr, float value) { (T*)ptr->*P = value; });
        }
        if constexpr (std::is_same_v<U, unsigned char>)
        {
            v8Class->AddAccessor(
                name, [](void* ptr) { return (T*)ptr->*P; }, [](void* ptr, unsigned char value) { (T*)ptr->*P = value; });
        }
    }

    CV8ExportClassBase* v8Class;
    EClass              classId;
};
