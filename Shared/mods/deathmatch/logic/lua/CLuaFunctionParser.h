/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <optional>
#include <variant>
#include <SharedUtil.Template.h>
#include "lua/CLuaFunctionParseHelpers.h"
#include "lua/CLuaStackChecker.h"
#include "lua/LuaBasic.h"

template <bool, auto*>
struct CLuaFunctionParser
{
};

template <bool ErrorOnFailure, typename Ret, typename... Args, auto (*Func)(Args...)->Ret>
struct CLuaFunctionParser<ErrorOnFailure, Func>
{
    std::size_t iIndex = 1;
    std::string strError = "";
    std::string strErrorFoundType = "";

    template <typename T>
    inline void typeToNameVariant(SString& accumulator)
    {
        using param = typename is_variant<T>::param1_t;
        if (accumulator.length() == 0)
            accumulator = typeToName<param>();
        else 
            accumulator += "/" + typeToName<param>();

        if constexpr (is_variant<T>::count != 1)
            return typeToNameVariant<is_variant<T>::rest_t>(accumulator);
    }

    template <typename T>
    inline SString typeToName()
    {
        if constexpr (std::is_same_v<T, std::string>)
            return "string";
        else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, double> || std::is_same_v<T, short> ||
                           std::is_same_v<T, unsigned int> || std::is_same_v<T, unsigned short>)
            return "number";
        else if constexpr (std::is_same_v<T, bool>)
            return "boolean";
        else if constexpr (std::is_enum_v<T>)
            return "enum";
        else if constexpr (is_specialization<T, std::optional>::value)
        {
            using param_t = typename is_specialization<T, std::optional>::param_t;
            return typeToName<param_t>();
        }
        else if constexpr (is_2specialization<T, std::vector>::value)
            return "table";
        else if constexpr (is_5specialization<T, std::unordered_map>::value)
            return "table";
        else if constexpr (std::is_same_v<T, CLuaFunctionRef>)
            return "function";
        else if constexpr (std::is_same_v<T, lua_State*>)
            return "";            // not reachable
        else if constexpr (is_variant<T>::value)
        {
            SString strTypes;
            typeToNameVariant<T>(strTypes);    
            return strTypes;
        }

        else if constexpr (std::is_pointer_v<T> && std::is_class_v<std::remove_pointer_t<T>>)
            return GetClassTypeName((T)0);
    }

    static SString ResolveParameter(lua_State* L, std::size_t index)
    {
        switch (lua_type(L, index))
        {
            case LUA_TNUMBER:
                return SString("number (%d)", lua_tonumber(L, index));
            case LUA_TSTRING:
            {
                std::size_t iLen;
                const char* szValue = lua_tolstring(L, index, &iLen);
                std::string strValue(szValue, iLen);
                if (strValue.length() > 10)
                {
                    strValue.resize(10);            // Limit to 10 characters
                    strValue[9] = '.';
                    strValue[8] = '.';
                    strValue[7] = '.';
                }
                // Avoid printing binary data
                if (std::find_if(strValue.begin(), strValue.end(), [](char ch) { return !(std::isprint(ch)); }) != strValue.end())
                    return "string";
                else
                {
                    return SString("string (\"%s\")", strValue.c_str());
                }
            }
            case LUA_TBOOLEAN:
                return SString("boolean (%s)", lua_toboolean(L, index) == 1 ? "true" : "false");
            case LUA_TNIL:
                return "nil";
            case LUA_TNONE:
                return "none";
            case LUA_TTABLE:
                return "table";
            case LUA_TFUNCTION:
                return "function";
            case LUA_TTHREAD:
                return "coroutine";
            case LUA_TUSERDATA:
                return GetUserDataClassName(*((void**)lua_touserdata(L, index)), L);
            case LUA_TLIGHTUSERDATA:
                return GetUserDataClassName(lua_touserdata(L, index), L);
        }
        return "";
    }

    // Pop should remove a T from the Lua Stack after verifying that it is a valid type
    // Pop may also throw a LuaArgumentError to indicate failure
    template <typename T>
    inline T Pop(lua_State* L, std::size_t& index)
    {
        if (!TypeMatch<T>(L, index))
        {
            SString strReceived = ResolveParameter(L, index);
            SString strExpected = typeToName<T>();
            SString strMessage("Expected %s at argument %d, got %s", strExpected.c_str(), index, strReceived.c_str());
            strError = strMessage;
            return T{};
        }
        return PopUnsafe<T>(L, index);
    }

    // Special type matcher for variants. Returns -1 if the type does not match
    // returns n if the nth type of the variant matches
    template <typename T>
    inline int TypeMatchVariant(lua_State* L, std::size_t index)
    {
        if constexpr (std::is_same_v<T, std::variant<>>)
            return -1;
        else
        {
            using first_t = typename is_variant<T>::param1_t;
            using next_t = typename is_variant<T>::rest_t;
            if (TypeMatch<first_t>(L, index))
                return 0;
            else
            {
                int iResult = TypeMatchVariant<next_t>(L, index);
                if (iResult == -1)
                    return -1;
                return 1 + iResult;
            }
        }
    }

    // TypeMatch<T> should return true if the value on top of the Lua stack can be popped via
    // PopUnsafe<T>. This must accurately reflect the associated PopUnsafe. Note that TypeMatch
    // should only check for obvious type violations (e.g. false is not a string) but not
    // for internal type errors (passing a vehicle to a function expecting a ped)
    template <typename T>
    inline bool TypeMatch(lua_State* L, std::size_t index)
    {
        int iArgument = lua_type(L, index);
        // trivial types
        if constexpr (std::is_same_v<T, std::string>)
            return (iArgument == LUA_TSTRING || iArgument == LUA_TNUMBER);
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, double> || std::is_same_v<T, short> ||
                      std::is_same_v<T, unsigned int> || std::is_same_v<T, unsigned short>)
            return (iArgument == LUA_TSTRING || iArgument == LUA_TNUMBER);
        if constexpr (std::is_same_v<T, bool>)
            return (iArgument == LUA_TBOOLEAN);

        // Advanced types
        // Enums are represented as strings to Lua
        if constexpr (std::is_enum_v<T>)
            return iArgument == LUA_TSTRING;
        // std::optional is used for optional parameters
        // which may also be in the middle of a parameter list
        // therefore it is always valid to attempt to read an
        // optional
        if constexpr (is_specialization<T, std::optional>::value)
            return true;

        // std::vector is used for arrays built from tables
        if constexpr (is_2specialization<T, std::vector>::value)
            return iArgument == LUA_TTABLE;

        // std::unordered_map<k,v> is used for maps built from tables
        if constexpr (is_5specialization<T, std::unordered_map>::value)
            return iArgument == LUA_TTABLE;

        // CLuaFunctionRef is used for functions
        if constexpr (std::is_same_v<T, CLuaFunctionRef>)
            return iArgument == LUA_TFUNCTION;

        // lua_State* can be taken as argument anywhere
        if constexpr (std::is_same_v<T, lua_State*>)
            return true;

        // variants can be used by any of the underlying types
        // thus recursively use this function
        if constexpr (is_variant<T>::value)
            return TypeMatchVariant<T>(L, index) != -1;

        // Catch all for class pointer types, assume all classes are valid script entities
        // and can be fetched from a userdata
        if constexpr (std::is_pointer_v<T> && std::is_class_v<std::remove_pointer_t<T>>)
            return iArgument == LUA_TUSERDATA || iArgument == LUA_TLIGHTUSERDATA;
    }

    // Special PopUnsafe for variants
    template <typename T, std::size_t currIndex = 0>
    inline T PopUnsafeVariant(lua_State* L, std::size_t& index, int vindex)
    {
        // As std::variant<> cannot be constructed, we simply return the first value
        // in the error case. This is actually unreachable in the regular path,
        // due to TypeMatch making sure that vindex < is_variant<T>::count
        if constexpr (is_variant<T>::count == currIndex)
        {
            using type_t = typename is_variant<T>::param1_t;
            return type_t{};
        }
        else
        {
            // If we have reached the target index, pop the right value
            // else go to the next type
            if (vindex == currIndex)
            {
                using type_t = std::remove_reference_t<decltype(std::get<currIndex>(T{}))>;
                return PopUnsafe<type_t>(L, index);
            }
            else
            {
                return PopUnsafeVariant<T, currIndex + 1>(L, index, vindex);
            }
        }
    }

    template <typename T>
    inline T PopUnsafe(lua_State* L, std::size_t& index)
    {
        // Expect no change in stack size
        LUA_STACK_EXPECT(0); 
        // trivial types are directly popped
        if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, double> ||
                      std::is_same_v<T, short> || std::is_same_v<T, unsigned int> || std::is_same_v<T, unsigned short> || std::is_same_v<T, bool>)
            return lua::PopTrivial<T>(L, index);
        else if constexpr (std::is_enum_v<T>)
        {
            // Enums are considered strings in Lua
            std::string strValue = lua::PopTrivial<std::string>(L, index);
            T           eValue;
            if (StringToEnum(strValue, eValue))
                return eValue;
            else
            {
                SString strReceived = ResolveParameter(L, index);
                SString strExpected = GetEnumTypeName((T)0);
                SString strMessage("Expected %s at argument %d, got %s", strExpected.c_str(), index, strReceived.c_str());
                strError = strMessage;
                return static_cast<T>(0);
            }
        }
        else if constexpr (is_specialization<T, std::optional>::value)
        {
            // optionals may either type match the desired value, or be nullopt
            using param = typename is_specialization<T, std::optional>::param_t;
            if (TypeMatch<param>(L, index))
                return PopUnsafe<param>(L, index);
            else
                return std::nullopt;
        }

        else if constexpr (is_2specialization<T, std::vector>::value)            // 2 specialization due to allocator
        {
            using param = typename is_2specialization<T, std::vector>::param1_t;
            T vecData;
            lua_pushnil(L); /* first key */
            while (lua_next(L, index) != 0)
            {
                if (!TypeMatch<param>(L, -1))
                {
                    // skip
                    lua_pop(L, 1);
                    continue;
                }

                std::size_t i = -1;
                vecData.emplace_back(PopUnsafe<param>(L, i));
                lua_pop(L, 1);            // drop value, keep key for lua_next
            }
            ++index;
            return vecData;
        }
        else if constexpr (is_5specialization<T, std::unordered_map>::value)
        {
            using key_t = typename is_5specialization<T, std::unordered_map>::param1_t;
            using value_t = typename is_5specialization<T, std::unordered_map>::param2_t;
            T map;
            lua_pushnil(L); /* first key */
            while (lua_next(L, index) != 0)
            {
                if (!TypeMatch<value_t>(L, -1) || !TypeMatch<key_t>(L, -2))
                {
                    // skip
                    lua_pop(L, 1);
                    continue;
                }

                std::size_t i = -2;
                auto        k = PopUnsafe<key_t>(L, i);
                auto        v = PopUnsafe<value_t>(L, i);
                map.emplace(std::move(k), std::move(v));
                lua_pop(L, 1);            // drop value, keep key for lua_next
            }
            ++index;
            return map;
        }
        else if constexpr (std::is_same_v<T, CLuaFunctionRef>)
        {
            return luaM_toref(L, index++);
        }
        else if constexpr (std::is_same_v<T, lua_State*>)
            return L;
        // variants can be used by any of the underlying types
        // thus recursively use this function
        else if constexpr (is_variant<T>::value)
        {
            int iMatch = TypeMatchVariant<T>(L, index);
            return PopUnsafeVariant<T>(L, index, iMatch);
        }

        // Catch all for class pointer types, assume all classes are valid script entities
        // and can be fetched from a userdata
        else if constexpr (std::is_pointer_v<T> && std::is_class_v<std::remove_pointer_t<T>>)
        {
            bool  isLightUserData = lua_type(L, index) == LUA_TLIGHTUSERDATA;
            void* pValue = lua::PopTrivial<void*>(L, index);
            using class_t = std::remove_pointer_t<T>;
            auto result =
                isLightUserData ? UserDataCast<class_t>((class_t*)0, pValue, L) : UserDataCast<class_t>((class_t*)0, *reinterpret_cast<void**>(pValue), L);
            if (result == nullptr)
            {
                SString strReceived = isLightUserData ? GetUserDataClassName(pValue, L) : GetUserDataClassName(*(void**)pValue, L);
                SString strExpected = GetClassTypeName((T)0);
                SString strMessage("Expected %s at argument %d, got %s", strExpected.c_str(), index, strReceived.c_str());
                strError = strMessage;
                return nullptr;
            }
            return static_cast<T>(result);
        }
    }

    template <typename... Params>
    inline auto Call(lua_State* L, Params&&... ps)
    {
        if (strError.length() != 0)
        {
            return -1;
        }
        if constexpr (sizeof...(Params) == sizeof...(Args))
        {
            if constexpr (std::is_same_v<Ret, void>)
            {
                Func(std::forward<Params>(ps)...);
                return 0;
            }
            else
            {
                return lua::Push(L, Func(std::forward<Params>(ps)...));
            }
        }
        else
        {
            return Call(L, ps..., Pop<typename nth_element_impl<sizeof...(Params), Args...>::type>(L, iIndex));
        }
    }

    inline int operator()(lua_State* L, CScriptDebugging* pScriptDebugging)
    {
        int iResult = 0;
        try
        {
            iResult = Call(L);
        }
        catch (std::invalid_argument& e)
        {
            strError = e.what();
        }
        if (strError.length() != 0)
        {
            if constexpr (ErrorOnFailure)
            {
                luaL_error(L, strError.c_str());
            }
            else
            {
                pScriptDebugging->LogCustom(L, strError.c_str());
                lua_pushboolean(L, false);
            }
            return 1;
        }
        return iResult;
    }
};
