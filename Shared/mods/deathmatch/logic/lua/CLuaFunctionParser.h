/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

class CLuaArgument;

#include <optional>
#include <variant>
#include <SharedUtil.Template.h>
#include "lua/CLuaOverloadParser.h"
#include "lua/CLuaFunctionParseHelpers.h"
#include "lua/CLuaStackChecker.h"
#include "lua/LuaBasic.h"
#include <lua/CLuaMultiReturn.h>


struct CLuaFunctionParserBase
{
    // iIndex is passed around by reference
    std::size_t iIndex = 1;

    std::string strError = "";
    std::string strErrorFoundType = "";

    // Translates a variant type to a list of names separated by slashes
    // std::variant<bool, int, float> => bool/int/float
    template <typename T>
    inline void TypeToNameVariant(SString& accumulator)
    {
        using param = typename is_variant<T>::param1_t;
        if (accumulator.length() == 0)
            accumulator = TypeToName<param>();
        else
            accumulator += "/" + TypeToName<param>();

        if constexpr (is_variant<T>::count != 1)
            return TypeToNameVariant<typename is_variant<T>::rest_t>(accumulator);
    }

    template <typename T>
    inline SString TypeToName()
    {
        if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>)
            return "string";
        else if constexpr (std::is_same_v<T, bool>)
            return "boolean";
        else if constexpr (std::is_arithmetic_v<T>)
            return "number";
        else if constexpr (std::is_enum_v<T>)
            return "enum";
        else if constexpr (is_specialization<T, std::optional>::value)
        {
            using param_t = typename is_specialization<T, std::optional>::param_t;
            return TypeToName<param_t>();
        }
        else if constexpr (std::is_same_v<T, CLuaArgument>)
            return "value";
        else if constexpr (is_2specialization<T, std::vector>::value)
            return "table";
        else if constexpr (is_5specialization<T, std::unordered_map>::value)
            return "table";
        else if constexpr (std::is_same_v<T, CLuaFunctionRef>)
            return "function";
        else if constexpr (std::is_same_v<T, CVector2D>)
            return "vector2";
        else if constexpr (std::is_same_v<T, CVector>)
            return "vector3";
        else if constexpr (std::is_same_v<T, CVector4D>)
            return "vector4";
        else if constexpr (std::is_same_v<T, CMatrix>)
            return "matrix";
        else if constexpr (std::is_same_v<T, SColor>)
            return "colour";
        else if constexpr (std::is_same_v<T, lua_State*>)
            return "";            // not reachable
        else if constexpr (is_variant<T>::value)
        {
            SString strTypes;
            TypeToNameVariant<T>(strTypes);
            return strTypes;
        }
        else if constexpr (std::is_pointer_v<T> && std::is_class_v<std::remove_pointer_t<T>>)
            return GetClassTypeName((T)0);
        else if constexpr (std::is_same_v<T, dummy_type>)
            return "";
        else if constexpr (std::is_same_v<T, std::monostate>)
            return "";
    }

    // Reads the parameter type (& value in some cases) at a given index
    // For example a 42 on the Lua stack is returned as 'number (42)'
    static SString ReadParameterAsString(lua_State* L, std::size_t index)
    {
        switch (lua_type(L, index))
        {
            case LUA_TNUMBER:
                return SString("number (%s)", lua_tostring(L, index));
            case LUA_TSTRING:
            {
                std::size_t iLen;
                const char* szValue = lua_tolstring(L, index, &iLen);
                std::string strValue(szValue, iLen);
                if (strValue.length() > 10)
                {
                    // Limit to 10 characters
                    strValue.resize(10);
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
    template <typename T>
    inline T Pop(lua_State* L, std::size_t& index)
    {
        if (!TypeMatch<T>(L, index))
        {
            SString strReceived = ReadParameterAsString(L, index);
            SString strExpected = TypeToName<T>();
            SetBadArgumentError(L, strExpected, index, strReceived);
            return T{};
        }
        return PopUnsafe<T>(L, index);
    }

    // Special type matcher for variants. Returns -1 if the type does not match
    // returns n if the nth type of the variant matches
    template <typename T>
    inline int TypeMatchVariant(lua_State* L, std::size_t index)
    {
        // If the variant is empty, we have exhausted all options
        // The type therefore doesn't match the variant
        if constexpr (std::is_same_v<T, std::variant<>>)
            return -1;
        else
        {
            // Try to match the first type of the variant
            // If it matches, we've found our index
            using first_t = typename is_variant<T>::param1_t;
            using next_t = typename is_variant<T>::rest_t;
            if (TypeMatch<first_t>(L, index))
                return 0;

            // Else try the remaining types of the variant
            int iResult = TypeMatchVariant<next_t>(L, index);
            if (iResult == -1)
                return -1;
            return 1 + iResult;
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
        // primitive types
        if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>)
            return (iArgument == LUA_TSTRING || iArgument == LUA_TNUMBER);
        if constexpr (std::is_same_v<T, bool>)
            return (iArgument == LUA_TBOOLEAN);
        if constexpr (std::is_arithmetic_v<T>)
            return lua_isnumber(L, index);

        // Advanced types
        // Enums are represented as strings to Lua
        if constexpr (std::is_enum_v<T>)
            return iArgument == LUA_TSTRING;

        // CLuaArgument can hold any value
        if constexpr (std::is_same_v<T, CLuaArgument>)
            return iArgument != LUA_TNONE;

        // All color classes are read as a single tocolor number
        // Do not be tempted to change this to is_base_of<SColor, T>
        // SColorARGB etc are only **constructors** for SColor!
        if constexpr (std::is_same_v<SColor, T>)
            return lua_isnumber(L, index);

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

        // lua_State* can be taken as first argument of any function
        if constexpr (std::is_same_v<T, lua_State*>)
            return index == 1;

        // variants can be used by any of the underlying types
        // thus recursively use this function
        if constexpr (is_variant<T>::value)
            return TypeMatchVariant<T>(L, index) != -1;

        // Vector2 may either be represented by CLuaVector or by two numbers
        if constexpr (std::is_same_v<T, CVector2D>)
        {
            if (lua_isnumber(L, index) && lua_isnumber(L, index + 1))
                return true;
            return iArgument == LUA_TUSERDATA || iArgument == LUA_TLIGHTUSERDATA;
        }

        // Vector3 may either be represented by CLuaVector or by three numbers
        if constexpr (std::is_same_v<T, CVector>)
        {
            if (lua_isnumber(L, index) && lua_isnumber(L, index + 1) && lua_isnumber(L, index + 2))
                return true;
            return iArgument == LUA_TUSERDATA || iArgument == LUA_TLIGHTUSERDATA;
        }

        // Vector4 may either be represented by CLuaVector or by three numbers
        if constexpr (std::is_same_v<T, CVector4D>)
        {
            if (lua_isnumber(L, index) && lua_isnumber(L, index + 1) && lua_isnumber(L, index + 2) && lua_isnumber(L, index + 3))
                return true;
            return iArgument == LUA_TUSERDATA || iArgument == LUA_TLIGHTUSERDATA;
        }
        // CMatrix may either be represented by 3 CLuaVector or by 12 numbers
        if constexpr (std::is_same_v<T, CMatrix>)
        {
            for (int i = 0; i < sizeof(CMatrix)/sizeof(float); i++)
            {
                if (!lua_isnumber(L, index + i))
                    return iArgument == LUA_TUSERDATA || iArgument == LUA_TLIGHTUSERDATA;
            }
            return true;
        }
        // Catch all for class pointer types, assume all classes are valid script entities
        // and can be fetched from a userdata
        if constexpr (std::is_pointer_v<T> && std::is_class_v<std::remove_pointer_t<T>>)
            return iArgument == LUA_TUSERDATA || iArgument == LUA_TLIGHTUSERDATA;

        // dummy type is used as overload extension if one overload has fewer arguments
        // thus it is only allowed if there are no further args on the Lua side
        if constexpr (std::is_same_v<T, dummy_type>)
            return iArgument == LUA_TNONE;

        // no value
        if constexpr (std::is_same_v<T, std::monostate>)
            return iArgument == LUA_TNONE;
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
            // If we haven't reached the target index go to the next type
            if (vindex != currIndex)
                return PopUnsafeVariant<T, currIndex + 1>(L, index, vindex);

            // Pop the actual type
            using type_t = std::remove_reference_t<decltype(std::get<currIndex>(T{}))>;
            return PopUnsafe<type_t>(L, index);
        }
    }

    template<typename T>
    void SetBadArgumentError(lua_State* L, int index, void* pReceived, bool isLightUserData)
    {
        SString strExpected = GetClassTypeName((T)0);
        SetBadArgumentError(L, strExpected, index, pReceived, isLightUserData);
    }


    void SetBadArgumentError(lua_State* L, SString strExpected, int index, void* pReceived, bool isLightUserData)
    {
        SString strReceived = isLightUserData ? GetUserDataClassName(pReceived, L) : GetUserDataClassName(*(void**)pReceived, L);
        // strReceived may be an empty string if we cannot resolve the class name for the internal ID
        // this happens if the script entity was destroyed before calling a function with an entity parameter
        if (strReceived == "")
            strReceived = "destroyed element";
        SetBadArgumentError(L, strExpected, index, strReceived);
    }

    void SetBadArgumentError(lua_State* L, const SString& strExpected, int index, const SString& strReceived)
    {
        strError =
            SString("Bad argument @ '%s' [Expected %s at argument %d, got %s]", lua_tostring(L, lua_upvalueindex(1)), strExpected.c_str(), index, strReceived.c_str());
    }

    // PopUnsafe pops a `T` from the stack at the specified index
    // For most types there is no additional validation present, which is why this function
    // should not be called without making sure that `T` is compatible with the lua type
    // at the given index. Hence this function is called unsafe.
    // Errors may still occur here, if the error condition cannot be caught by TypeMatch
    // For example this will happen if a function is called with an element of the wrong type
    // as this condition cannot be caught before actually reading the userdata from the Lua stack
    // On success, this function may also increment `index`
    template <typename T>
    inline T PopUnsafe(lua_State* L, std::size_t& index)
    {
        // Expect no change in stack size
        LUA_STACK_EXPECT(0);
        // the dummy type is not read from Lua
        if constexpr (std::is_same_v<T, dummy_type>)
            return dummy_type{};
        // primitive types are directly popped
        else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view> || std::is_integral_v<T>)
            return lua::PopPrimitive<T>(L, index);
        // floats/doubles may not be NaN
        else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
        {
            T value = lua::PopPrimitive<T>(L, index);
            if (std::isnan(value))
            {
                // Subtract one from the index, as the call to lua::PopPrimitive above increments the index, even if the
                // underlying element is of a wrong type
                SetBadArgumentError(L, "number", index - 1, "NaN");
            }
            return value;
        }
        else if constexpr (std::is_enum_v<T>)
        {
            // Enums are considered strings in Lua
            std::string strValue = lua::PopPrimitive<std::string>(L, index);
            T           eValue;
            if (StringToEnum(strValue, eValue))
                return eValue;
            else
            {
                // Subtract one from the index, as the call to lua::PopPrimitive above increments the index, even if the
                // underlying element is of a wrong type
                SString strReceived = ReadParameterAsString(L, index - 1);
                SString strExpected = GetEnumTypeName((T)0);
                SetBadArgumentError(L, strExpected, index - 1, strReceived);
                return static_cast<T>(0);
            }
        }
        else if constexpr (is_specialization<T, std::optional>::value)
        {
            // optionals may either type match the desired value, or be nullopt
            using param = typename is_specialization<T, std::optional>::param_t;
            if (TypeMatch<param>(L, index))
                return PopUnsafe<param>(L, index);

            if (!lua_isnoneornil(L, index))
            {
                SString strReceived = ReadParameterAsString(L, index);
                SString strExpected = TypeToName<param>();
                SetBadArgumentError(L, strExpected, index, strReceived);
            }

            // We didn't call PopUnsafe, so we need to manually increment the index
            index++;

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
        // Vectors may either be represented by CLuaVectorND or by N numbers
        else if constexpr (std::is_same_v<T, CVector2D>)
        {
            if (lua_isnumber(L, index))
            {
                CVector2D vec;
                vec.fX = lua::PopPrimitive<float>(L, index);
                vec.fY = lua::PopPrimitive<float>(L, index);
                return vec;
            }
            else
            {
                int   iType = lua_type(L, index);
                bool  isLightUserData = iType == LUA_TLIGHTUSERDATA;
                void* pValue = lua::PopPrimitive<void*>(L, index);
                auto cast = [isLightUserData, pValue, L](auto null) {
                    return isLightUserData ? UserDataCast<decltype(null)>(null, pValue, L)
                                           : UserDataCast<decltype(null)>(null, *reinterpret_cast<void**>(pValue), L);
                };
                // A vector2 may also be filled from a vector3/vector4
                if (CLuaVector2D* pVec2D = cast((CLuaVector2D*)0); pVec2D != nullptr)
                    return *pVec2D;
                if (CLuaVector3D* pVec3D = cast((CLuaVector3D*)0); pVec3D != nullptr)
                    return *pVec3D;
                if (CLuaVector4D* pVec4D = cast((CLuaVector4D*)0); pVec4D != nullptr)
                    return *pVec4D;

                // Subtract one from the index, as the call to lua::PopPrimitive above increments the index, even if the
                // underlying element is of a wrong type
                SetBadArgumentError(L, "vector2", index - 1, pValue, isLightUserData);
                return T{};
            }
        }
        else if constexpr (std::is_same_v<T, CVector>)
        {
            if (lua_isnumber(L, index))
            {
                CVector vec;
                vec.fX = lua::PopPrimitive<float>(L, index);
                vec.fY = lua::PopPrimitive<float>(L, index);
                vec.fZ = lua::PopPrimitive<float>(L, index);
                return vec;
            }
            else
            {
                int   iType = lua_type(L, index);
                bool  isLightUserData = iType == LUA_TLIGHTUSERDATA;
                void* pValue = lua::PopPrimitive<void*>(L, index);
                auto  cast = [isLightUserData, pValue, L](auto null) {
                    return isLightUserData ? UserDataCast<decltype(null)>(null, pValue, L)
                                           : UserDataCast<decltype(null)>(null, *reinterpret_cast<void**>(pValue), L);
                };
                // A vector3 may also be filled from a vector4
                if (CLuaVector3D* pVec3D = cast((CLuaVector3D*)0); pVec3D != nullptr)
                    return *pVec3D;
                if (CLuaVector4D* pVec4D = cast((CLuaVector4D*)0); pVec4D != nullptr)
                    return *pVec4D;

                // Subtract one from the index, as the call to lua::PopPrimitive above increments the index, even if the
                // underlying element is of a wrong type
                SetBadArgumentError(L, "vector3", index - 1, pValue, isLightUserData);
                return T{};
            }
        }
        else if constexpr (std::is_same_v<T, CVector4D>)
        {
            if (lua_isnumber(L, index))
            {
                CVector4D vec;
                vec.fX = lua::PopPrimitive<float>(L, index);
                vec.fY = lua::PopPrimitive<float>(L, index);
                vec.fZ = lua::PopPrimitive<float>(L, index);
                vec.fW = lua::PopPrimitive<float>(L, index);
                return vec;
            }
            else
            {
                int   iType = lua_type(L, index);
                bool  isLightUserData = iType == LUA_TLIGHTUSERDATA;
                void* pValue = lua::PopPrimitive<void*>(L, index);
                auto  cast = [isLightUserData, pValue, L](auto null) {
                    return isLightUserData ? UserDataCast<decltype(null)>(null, pValue, L)
                                           : UserDataCast<decltype(null)>(null, *reinterpret_cast<void**>(pValue), L);
                };
                // A vector3 may also be filled from a vector4
                if (CLuaVector4D* pVec4D = cast((CLuaVector4D*)0); pVec4D != nullptr)
                    return *pVec4D;
                if (CLuaMatrix* pMatrix = cast((CLuaMatrix*)0); pMatrix != nullptr)
                    return *pMatrix;

                // Subtract one from the index, as the call to lua::PopPrimitive above increments the index, even if the
                // underlying element is of a wrong type
                SetBadArgumentError(L, "vector4", index - 1, pValue, isLightUserData);
                return T{};
            }
        }
        else if constexpr (std::is_same_v<T, CMatrix>)
        {
            if (lua_isnumber(L, index))
            {
                CMatrix matrix;
                matrix.vRight.fX = lua::PopPrimitive<float>(L, index);
                matrix.vRight.fY = lua::PopPrimitive<float>(L, index);
                matrix.vRight.fZ = lua::PopPrimitive<float>(L, index);
                matrix.vFront.fX = lua::PopPrimitive<float>(L, index);
                matrix.vFront.fY = lua::PopPrimitive<float>(L, index);
                matrix.vFront.fZ = lua::PopPrimitive<float>(L, index);
                matrix.vUp.fX = lua::PopPrimitive<float>(L, index);
                matrix.vUp.fY = lua::PopPrimitive<float>(L, index);
                matrix.vUp.fZ = lua::PopPrimitive<float>(L, index);
                matrix.vPos.fX = lua::PopPrimitive<float>(L, index);
                matrix.vPos.fY = lua::PopPrimitive<float>(L, index);
                matrix.vPos.fZ = lua::PopPrimitive<float>(L, index);
                return matrix;
            }
            else
            {
                int   iType = lua_type(L, index);
                bool  isLightUserData = iType == LUA_TLIGHTUSERDATA;
                void* pValue = lua::PopPrimitive<void*>(L, index);
                auto  cast = [isLightUserData, pValue, L](auto null) {
                    return isLightUserData ? UserDataCast<decltype(null)>(null, pValue, L)
                                           : UserDataCast<decltype(null)>(null, *reinterpret_cast<void**>(pValue), L);
                };
                // A vector4 may also be filled from a CLuaMatrix
                if (CLuaMatrix* pMatrix = cast((CLuaMatrix*)0); pMatrix != nullptr)
                    return *pMatrix;

                // Subtract one from the index, as the call to lua::PopPrimitive above increments the index, even if the
                // underlying element is of a wrong type
                SetBadArgumentError(L, "matrix", index - 1, pValue, isLightUserData);
                return T{};
            }
        }
        // Catch all for class pointer types, assume all classes are valid script entities
        // and can be fetched from a userdata
        else if constexpr (std::is_pointer_v<T> && std::is_class_v<std::remove_pointer_t<T>>)
        {
            bool  isLightUserData = lua_type(L, index) == LUA_TLIGHTUSERDATA;
            void* pValue = lua::PopPrimitive<void*>(L, index);
            using class_t = std::remove_pointer_t<T>;
            auto result =
                isLightUserData ? UserDataCast<class_t>((class_t*)0, pValue, L) : UserDataCast<class_t>((class_t*)0, *reinterpret_cast<void**>(pValue), L);
            if (result == nullptr)
            {
                // Subtract one from the index, as the call to lua::PopPrimitive above increments the index, even if the
                // underlying element is of a wrong type
                SetBadArgumentError<T>(L, index - 1, pValue, isLightUserData);
                return nullptr;
            }
            return static_cast<T>(result);
        }
        else if constexpr (std::is_same_v<T, SColor>)
            return static_cast<unsigned long>(lua::PopPrimitive<int64_t>(L, index));
        else if constexpr (std::is_same_v<T, CLuaArgument>)
        {
            CLuaArgument argument;
            argument.Read(L, index++);
            return argument;
        }
        else if constexpr (std::is_same_v<T, std::monostate>)
        {
            return T{};
        }
    }
};

template <bool, auto, auto*>
struct CLuaFunctionParser
{
};

template <bool ErrorOnFailure, auto ReturnOnFailure, typename Ret, typename... Args, auto (*Func)(Args...)->Ret>
struct CLuaFunctionParser<ErrorOnFailure, ReturnOnFailure, Func> : CLuaFunctionParserBase
{
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
                return PushResult(L, Func(std::forward<Params>(ps)...));
            }
        }
        else
        {
            return Call(L, ps..., Pop<typename nth_element_impl<sizeof...(Params), Args...>::type>(L, iIndex));
        }
    }

    // Tuples can be used to return multiple results
    template <typename... Ts>
    inline int PushResult(lua_State* L, const CLuaMultiReturn<Ts...>& result)
    {
        // Call Push on each element of the tuple
        std::apply([L](const auto&... value) { (lua::Push(L, value), ...); }, result.values);
        return sizeof...(Ts);
    }

    
    // Variant
    template <typename... Ts>
    inline int PushResult(lua_State* L, const std::variant<Ts...>& result)
    {
        return std::visit([this, L](const auto& value) { return PushResult(L, value); }, result);
    }

    // If `T` is not a tuple, defer to Push to push the value onto the stack
    template <typename T>
    inline int PushResult(lua_State* L, const T& value)
    {
        lua::Push(L, value);
        return 1;
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
            // This exception can be thrown from the called function
            // as an additional way to provide further argument errors
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
                lua::Push(L, ReturnOnFailure);
            }
            return 1;
        }
        return iResult;
    }
};
