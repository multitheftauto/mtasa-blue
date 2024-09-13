/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CScriptArgReader.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CVector2D.h"
#include <limits>
#include <type_traits>
#include <cfloat>
#include "CStringMap.h"
#include "CScriptDebugging.h"

#ifndef MTA_CLIENT
    #include "CGame.h"
#endif

/////////////////////////////////////////////////////////////////////////
//
// CScriptArgReader
//
//
// Attempt to simplify the reading of arguments from a script call
//
//////////////////////////////////////////////////////////////////////
class CScriptArgReader
{
public:
    CScriptArgReader(lua_State* luaVM)
    {
        m_luaVM = luaVM;
        m_iIndex = 1;
        m_iErrorIndex = 0;
        m_bError = false;
        m_pPendingFunctionOutValue = NULL;
        m_pPendingFunctionIndex = -1;
        m_bResolvedErrorGotArgumentTypeAndValue = false;
        m_bHasCustomMessage = false;
    }

    ~CScriptArgReader() { assert(!IsReadFunctionPending()); }

    //
    // Read next number
    //
    template <typename T>
    void ReadNumber(T& outValue, bool checkSign = true)
    {
        int iArgument = lua_type(m_luaVM, m_iIndex);
        if (iArgument == LUA_TNUMBER || iArgument == LUA_TSTRING)
        {
            // The string received may not actually be a number
            if (!lua_isnumber(m_luaVM, m_iIndex))
            {
                SetCustomError("Expected number, got non-convertible string", "Bad argument");
                return;
            }

            // Returns 0 even if the string cannot be parsed as a number
            lua_Number number = lua_tonumber(m_luaVM, m_iIndex++);

            if (std::isnan(number))
            {
                SetCustomError("Expected number, got NaN", "Bad argument");
                outValue = 0;
                return;
            }

            if (std::is_unsigned<T>())
            {
                if (checkSign && number < -FLT_EPSILON)
                {
                    SetCustomError("Expected positive value, got negative", "Bad argument");
                    return;
                }
                outValue = static_cast<T>(static_cast<int64_t>(number));
                return;
            }

            outValue = static_cast<T>(number);
            return;
        }

        outValue = 0;
        SetTypeError("number");
        m_iIndex++;
    }

    //
    // Read next number, using default if needed
    //
    template <typename T, typename U>
    void ReadNumber(T& outValue, const U& defaultValue, bool checkSign = true)
    {
        int iArgument = lua_type(m_luaVM, m_iIndex);
        if (iArgument == LUA_TNUMBER || iArgument == LUA_TSTRING)
        {
            // The string received may not actually be a number
            if (!lua_isnumber(m_luaVM, m_iIndex))
            {
                SetCustomError("Expected number, got non-convertible string", "Bad argument");
                return;
            }

            // Returns 0 even if the string cannot be parsed as a number
            lua_Number number = lua_tonumber(m_luaVM, m_iIndex++);

            if (std::isnan(number))
            {
                SetCustomError("Expected number, got NaN", "Bad argument");
                outValue = 0;
                return;
            }

            if (checkSign && std::is_unsigned<T>() && number < -FLT_EPSILON)
            {
                SetCustomError("Expected positive value, got negative", "Bad argument");
                return;
            }

            outValue = static_cast<T>(number);
            return;
        }
        else if (iArgument == LUA_TNONE || iArgument == LUA_TNIL)
        {
            outValue = static_cast<T>(defaultValue);
            m_iIndex++;
            return;
        }

        outValue = 0;
        SetTypeError("number");
        m_iIndex++;
    }

    //
    // Read next Vector2d
    //
    void ReadVector2D(CVector2D& outValue)
    {
        int iArgument = lua_type(m_luaVM, m_iIndex);
        if (iArgument == LUA_TSTRING || iArgument == LUA_TNUMBER)
        {
            ReadNumber(outValue.fX);
            ReadNumber(outValue.fY);
            return;
        }
        else if (iArgument == LUA_TUSERDATA)
        {
            if (NextIsUserDataOfType<CLuaVector2D>())
            {
                // we don't pass around the pointer as it may get destroyed any time
                CLuaVector2D* pVector = NULL;
                ReadUserData(pVector);
                if (pVector)
                {
                    outValue = static_cast<CVector2D&>(*pVector);
                    return;
                }
                outValue = CVector2D();
                return;            // Error set in ReadUserData
            }
            else if (NextIsUserDataOfType<CLuaVector3D>())
            {
                // we don't pass around the pointer as it may get destroyed any time
                CLuaVector3D* pVector = NULL;
                ReadUserData(pVector);
                if (pVector)
                {
                    outValue = *pVector;
                    return;
                }
                outValue = CVector2D();
                return;            // Error set in ReadUserData
            }
            else if (NextIsUserDataOfType<CLuaVector4D>())
            {
                // we don't pass around the pointer as it may get destroyed any time
                CLuaVector4D* pVector = NULL;
                ReadUserData(pVector);
                if (pVector)
                {
                    outValue = *pVector;
                    return;
                }
                outValue = CVector2D();
                return;            // Error set in ReadUserData
            }
        }

        outValue = CVector2D();
        SetTypeError("vector2");
        m_iIndex++;
    }

    //
    // Read next Vector2d, using default if needed
    //
    void ReadVector2D(CVector2D& outValue, const CVector2D& vecDefault)
    {
        outValue = vecDefault;
        int iArgument = lua_type(m_luaVM, m_iIndex);
        if (iArgument == LUA_TSTRING || iArgument == LUA_TNUMBER)
        {
            ReadNumber(outValue.fX, vecDefault.fX);
            ReadNumber(outValue.fY, vecDefault.fY);
            return;
        }
        else if (iArgument == LUA_TUSERDATA)
        {
            if (NextIsUserDataOfType<CLuaVector2D>())
            {
                // we don't pass around the pointer as it may get destroyed any time
                CLuaVector2D* pVector = NULL;
                ReadUserData(pVector);
                if (pVector)
                {
                    outValue = static_cast<CVector2D&>(*pVector);
                    return;
                }
                outValue = CVector2D();
                return;            // Error set in ReadUserData
            }
            else if (NextIsUserDataOfType<CLuaVector3D>())
            {
                // we don't pass around the pointer as it may get destroyed any time
                CLuaVector3D* pVector = NULL;
                ReadUserData(pVector);
                if (pVector)
                {
                    outValue = *pVector;
                    return;
                }
                outValue = CVector2D();
                return;            // Error set in ReadUserData
            }
            else if (NextIsUserDataOfType<CLuaVector4D>())
            {
                // we don't pass around the pointer as it may get destroyed any time
                CLuaVector4D* pVector = NULL;
                ReadUserData(pVector);
                if (pVector)
                {
                    outValue = *pVector;
                    return;
                }
                outValue = CVector2D();
                return;            // Error set in ReadUserData
            }
        }
        else if (iArgument == LUA_TNIL || iArgument == LUA_TNONE)
        {
            m_iIndex++;
            return;
        }

        SetTypeError("vector2");
        m_iIndex++;
    }

    //
    // Read next Vector3d
    //
    void ReadVector3D(CVector& outValue)
    {
        int iArgument = lua_type(m_luaVM, m_iIndex);
        if (iArgument == LUA_TSTRING || iArgument == LUA_TNUMBER)
        {
            ReadNumber(outValue.fX);
            ReadNumber(outValue.fY);
            ReadNumber(outValue.fZ);
            return;
        }
        else if (iArgument == LUA_TUSERDATA)
        {
            if (NextIsUserDataOfType<CLuaVector3D>())
            {
                // we don't pass around the pointer as it may get destroyed any time
                CLuaVector3D* pVector = NULL;
                ReadUserData(pVector);
                if (pVector)
                {
                    outValue = static_cast<CVector&>(*pVector);
                    return;
                }
                outValue = CVector();
                return;            // Error set in ReadUserData
            }
            else if (NextIsUserDataOfType<CLuaVector4D>())
            {
                // we don't pass around the pointer as it may get destroyed any time
                CLuaVector4D* pVector = NULL;
                ReadUserData(pVector);
                if (pVector)
                {
                    outValue = *pVector;
                    return;
                }
                outValue = CVector4D();
                return;            // Error set in ReadUserData
            }
        }

        outValue = CVector();
        SetTypeError("vector3");
        m_iIndex++;
    }

    //
    // Read next Vector3d, using default if needed
    //
    void ReadVector3D(CVector& outValue, const CVector& vecDefault)
    {
        outValue = vecDefault;
        int iArgument = lua_type(m_luaVM, m_iIndex);
        if (iArgument == LUA_TSTRING || iArgument == LUA_TNUMBER)
        {
            ReadNumber(outValue.fX, vecDefault.fX);
            ReadNumber(outValue.fY, vecDefault.fY);
            ReadNumber(outValue.fZ, vecDefault.fZ);
            return;
        }
        else if (iArgument == LUA_TUSERDATA)
        {
            if (NextIsUserDataOfType<CLuaVector3D>())
            {
                // we don't pass around the pointer as it may get destroyed any time
                CLuaVector3D* pVector = NULL;
                ReadUserData(pVector);
                if (pVector)
                {
                    outValue = static_cast<CVector&>(*pVector);
                    return;
                }
                outValue = CVector();
                return;            // Error set in ReadUserData
            }
            else if (NextIsUserDataOfType<CLuaVector4D>())
            {
                // we don't pass around the pointer as it may get destroyed any time
                CLuaVector4D* pVector = NULL;
                ReadUserData(pVector);
                if (pVector)
                {
                    outValue = *pVector;
                    return;
                }
                outValue = CVector4D();
                return;            // Error set in ReadUserData
            }
        }
        else if (iArgument == LUA_TNIL || iArgument == LUA_TNONE)
        {
            m_iIndex++;
            return;
        }

        SetTypeError("vector3");
        m_iIndex++;
    }

    //
    // Read next Vector4d
    //
    void ReadVector4D(CVector4D& outValue)
    {
        int iArgument = lua_type(m_luaVM, m_iIndex);
        if (iArgument == LUA_TSTRING || iArgument == LUA_TNUMBER)
        {
            ReadNumber(outValue.fX);
            ReadNumber(outValue.fY);
            ReadNumber(outValue.fZ);
            ReadNumber(outValue.fW);
            return;
        }
        else if (iArgument == LUA_TUSERDATA)
        {
            if (NextIsUserDataOfType<CLuaVector4D>())
            {
                // we don't pass around the pointer as it may get destroyed any time
                CLuaVector4D* pVector = NULL;
                ReadUserData(pVector);
                if (pVector)
                {
                    outValue = static_cast<CVector4D&>(*pVector);
                    return;
                }
                outValue = CVector4D();
                return;            // Error set in ReadUserData
            }
        }

        outValue = CVector4D();
        SetTypeError("vector4");
        m_iIndex++;
    }

    //
    // Read next Vector4d, using default if needed
    //
    void ReadVector4D(CVector4D& outValue, const CVector4D& vecDefault)
    {
        outValue = vecDefault;
        int iArgument = lua_type(m_luaVM, m_iIndex);
        if (iArgument == LUA_TSTRING || iArgument == LUA_TNUMBER)
        {
            ReadNumber(outValue.fX, vecDefault.fX);
            ReadNumber(outValue.fY, vecDefault.fY);
            ReadNumber(outValue.fZ, vecDefault.fZ);
            ReadNumber(outValue.fW, vecDefault.fW);
            return;
        }
        else if (iArgument == LUA_TUSERDATA)
        {
            if (NextIsUserDataOfType<CLuaVector4D>())
            {
                // we don't pass around the pointer as it may get destroyed any time
                CLuaVector4D* pVector = NULL;
                ReadUserData(pVector);
                if (pVector)
                {
                    outValue = static_cast<CVector4D&>(*pVector);
                    return;
                }
                outValue = CVector4D();
                return;            // Error set in ReadUserData
            }
        }
        else if (iArgument == LUA_TNIL || iArgument == LUA_TNONE)
        {
            m_iIndex++;
            return;
        }

        SetTypeError("vector4");
        m_iIndex++;
    }

    //
    // Read next Matrix
    //
    void ReadMatrix(CMatrix& outValue)
    {
        int iArgument = lua_type(m_luaVM, m_iIndex);
        if (iArgument == LUA_TSTRING || iArgument == LUA_TNUMBER)
        {
            ReadNumber(outValue.vFront.fX);
            ReadNumber(outValue.vFront.fY);
            ReadNumber(outValue.vFront.fZ);
            ReadNumber(outValue.vPos.fX);
            ReadNumber(outValue.vPos.fY);
            ReadNumber(outValue.vPos.fZ);
            ReadNumber(outValue.vRight.fX);
            ReadNumber(outValue.vRight.fY);
            ReadNumber(outValue.vRight.fZ);
            ReadNumber(outValue.vUp.fX);
            ReadNumber(outValue.vUp.fY);
            ReadNumber(outValue.vUp.fZ);
            return;
        }
        else if (iArgument == LUA_TUSERDATA)
        {
            // we don't pass around the pointer as it may get destroyed any time
            CLuaMatrix* pMatrix = NULL;
            ReadUserData(pMatrix);
            if (pMatrix)
            {
                outValue = static_cast<CMatrix&>(*pMatrix);
                return;
            }
            outValue = CMatrix();
            return;            // Error set in ReadUserData
        }

        outValue = CMatrix();
        SetTypeError("matrix");
        m_iIndex++;
    }

    //
    // Read next color
    //
    void ReadColor(SColor& outValue)
    {
        lua_Number color;
        ReadNumber(color);

        if (!m_bError)
            outValue = static_cast<unsigned long>(static_cast<int64_t>(color));
    }

    //
    // Read next color
    //
    void ReadColor(SColor& outValue, const SColor& defaultValue)
    {
        lua_Number color;
        ReadNumber(color, static_cast<lua_Number>(defaultValue));

        if (!m_bError)
            outValue = static_cast<unsigned long>(static_cast<int64_t>(color));
    }

    //
    // Read next bool
    //
    void ReadBool(bool& bOutValue)
    {
        int iArgument = lua_type(m_luaVM, m_iIndex);
        if (iArgument == LUA_TBOOLEAN)
        {
            bOutValue = lua_toboolean(m_luaVM, m_iIndex++) ? true : false;
            return;
        }

        bOutValue = false;
        SetTypeError("bool");
        m_iIndex++;
    }

    //
    // Read next bool or false if failed
    // Intended for use in pair with NextIsBool()
    //
    bool ReadBool()
    {
        int iArgument = lua_type(m_luaVM, m_iIndex);
        if (iArgument == LUA_TBOOLEAN)
        {
            return lua_toboolean(m_luaVM, m_iIndex++) ? true : false;
        }
        
        m_iIndex++;
        return false;
    }

    //
    // Read next bool, using default if needed
    //
    void ReadBool(bool& bOutValue, const bool bDefaultValue)
    {
        int iArgument = lua_type(m_luaVM, m_iIndex);
        if (iArgument == LUA_TBOOLEAN)
        {
            bOutValue = lua_toboolean(m_luaVM, m_iIndex++) ? true : false;
            return;
        }
        else if (iArgument == LUA_TNONE || iArgument == LUA_TNIL)
        {
            bOutValue = bDefaultValue;
            m_iIndex++;
            return;
        }

        bOutValue = false;
        SetTypeError("bool");
        m_iIndex++;
    }

    //
    // Read next string, using default if needed
    //
    void ReadString(SString& outValue, const char* defaultValue = NULL)
    {
        int iArgument = lua_type(m_luaVM, m_iIndex);

        if (iArgument == LUA_TSTRING || iArgument == LUA_TNUMBER)
        {
            size_t length = lua_strlen(m_luaVM, m_iIndex);

            try
            {
                outValue.assign(lua_tostring(m_luaVM, m_iIndex++), length);
            }
            catch (const std::bad_alloc&)
            {
                SetCustomError("out of memory", "Memory allocation");
            }

            return;
        }
        else if (iArgument == LUA_TNONE || iArgument == LUA_TNIL)
        {
            if (defaultValue)
            {
                m_iIndex++;

                try
                {
                    outValue.assign(defaultValue);
                }
                catch (const std::bad_alloc&)
                {
                    SetCustomError("out of memory", "Memory allocation");
                }

                return;
            }
        }

        outValue = "";
        SetTypeError("string");
        m_iIndex++;
    }

    //
    // Force-reads next argument as string
    //
    void ReadAnyAsString(SString& outValue)
    {
        if (luaL_callmeta(m_luaVM, m_iIndex, "__tostring"))
        {
            auto oldIndex = m_iIndex;
            m_iIndex = -1;
            ReadAnyAsString(outValue);

            lua_pop(m_luaVM, 1);            // Clean up stack
            m_iIndex = oldIndex + 1;
            return;
        }

        switch (lua_type(m_luaVM, m_iIndex))
        {
            case LUA_TNUMBER:
            case LUA_TSTRING:
                outValue = lua_tostring(m_luaVM, m_iIndex);
                break;
            case LUA_TBOOLEAN:
                outValue = lua_toboolean(m_luaVM, m_iIndex) ? "true" : "false";
                break;
            case LUA_TNIL:
                outValue = "nil";
                break;
            case LUA_TNONE:
                outValue = "";
                SetTypeError("non-none");
                break;
            default:
                outValue = SString("%s: %p", luaL_typename(m_luaVM, m_iIndex), lua_topointer(m_luaVM, m_iIndex));
                break;
        }

        ++m_iIndex;
    }

    //
    // Read next string as a string reference
    //
    void ReadCharStringRef(SCharStringRef& outValue)
    {
        int iArgument = lua_type(m_luaVM, m_iIndex);
        if (iArgument == LUA_TSTRING)
        {
            outValue.pData = (char*)lua_tolstring(m_luaVM, m_iIndex++, &outValue.uiSize);
            return;
        }

        outValue.pData = NULL;
        outValue.uiSize = 0;
        SetTypeError("string");
        m_iIndex++;
    }

    //
    // Read next string as an enum
    //
    template <class T>
    void ReadEnumString(T& outValue)
    {
        int iArgument = lua_type(m_luaVM, m_iIndex);
        if (iArgument == LUA_TSTRING)
        {
            SString strValue = lua_tostring(m_luaVM, m_iIndex);
            if (StringToEnum(strValue, outValue))
            {
                m_iIndex++;
                return;
            }
        }

        outValue = (T)0;
        SetTypeError(GetEnumTypeName(outValue));
        m_iIndex++;
    }

    //
    // Read next string as an enum, using default if needed
    //
    template <class T>
    void ReadEnumString(T& outValue, const T& defaultValue)
    {
        int iArgument = lua_type(m_luaVM, m_iIndex);
        if (iArgument == LUA_TSTRING)
        {
            SString strValue = lua_tostring(m_luaVM, m_iIndex);
            if (StringToEnum(strValue, outValue))
            {
                m_iIndex++;
                return;
            }
        }
        else if (iArgument == LUA_TNONE || iArgument == LUA_TNIL)
        {
            outValue = defaultValue;
            m_iIndex++;
            return;
        }

        outValue = (T)0;
        SetTypeError(GetEnumTypeName(outValue));
        m_iIndex++;
    }

    //
    // Read next string as a comma separated list of enums, using default if needed
    //
    template <class T>
    void ReadEnumStringList(std::vector<T>& outValueList, const SString& strDefaultValue)
    {
        outValueList.clear();
        int     iArgument = lua_type(m_luaVM, m_iIndex);
        SString strValue;
        if (iArgument == LUA_TSTRING)
        {
            strValue = lua_tostring(m_luaVM, m_iIndex);
        }
        else if (iArgument == LUA_TNONE || iArgument == LUA_TNIL)
        {
            strValue = strDefaultValue;
        }
        else
        {
            T outValue;
            SetTypeError(GetEnumTypeName(outValue));
            m_iIndex++;
            return;
        }

        // Parse each part of the string
        std::vector<SString> inValueList;
        strValue.Split(",", inValueList);
        for (uint i = 0; i < inValueList.size(); i++)
        {
            T outValue;
            if (StringToEnum(inValueList[i], outValue))
            {
                outValueList.push_back(outValue);
            }
            else
            {
                // Error
                SetTypeError(GetEnumTypeName(outValue));
                m_bResolvedErrorGotArgumentTypeAndValue = true;
                m_strErrorGotArgumentType = EnumToString((eLuaType)lua_type(m_luaVM, m_iErrorIndex));
                m_strErrorGotArgumentValue = inValueList[i];

                if (iArgument == LUA_TSTRING)
                    m_iIndex++;
                return;
            }
        }

        // Success
        if (iArgument == LUA_TSTRING)
            m_iIndex++;
    }

    //
    // Read next string or number as an enum
    //
    template <class T>
    void ReadEnumStringOrNumber(T& outValue)
    {
        int iArgument = lua_type(m_luaVM, m_iIndex);
        if (iArgument == LUA_TSTRING)
        {
            SString strValue = lua_tostring(m_luaVM, m_iIndex);
            if (StringToEnum(strValue, outValue))
            {
                m_iIndex++;
                return;
            }

            // If will be coercing a string to an enum, make sure string contains only digits
            size_t uiPos = strValue.find_first_not_of("0123456789");
            if (uiPos != SString::npos || strValue.empty())
                iArgument = LUA_TNONE;            //  Force error
        }

        if (iArgument == LUA_TSTRING || iArgument == LUA_TNUMBER)
        {
            outValue = static_cast<T>((int)lua_tonumber(m_luaVM, m_iIndex));
            if (EnumValueValid(outValue))
            {
                m_iIndex++;
                return;
            }
        }

        outValue = (T)0;
        SetTypeError(GetEnumTypeName(outValue));
        m_iIndex++;
    }

protected:
    //
    // Read next userdata, using rules and stuff
    //
    template <class T>
    void InternalReadUserData(bool bAllowNilResult, T*& outValue, bool bHasDefaultValue, T* defaultValue = (T*)-2)
    {
        outValue = NULL;
        int iArgument = lua_type(m_luaVM, m_iIndex);

        if (iArgument == LUA_TLIGHTUSERDATA)
        {
            outValue = (T*)UserDataCast((T*)lua_touserdata(m_luaVM, m_iIndex), m_luaVM);
            if (outValue)
            {
                m_iIndex++;
                return;
            }
        }
        else if (iArgument == LUA_TUSERDATA)
        {
            outValue = (T*)UserDataCast(*((T**)lua_touserdata(m_luaVM, m_iIndex)), m_luaVM);
            if (outValue)
            {
                m_iIndex++;
                return;
            }
        }
        else if (iArgument == LUA_TNONE || iArgument == LUA_TNIL)
        {
            if (bHasDefaultValue)
                outValue = defaultValue;
            else
                outValue = NULL;

            if (outValue || bAllowNilResult)
            {
                m_iIndex++;
                return;
            }
        }

        outValue = NULL;
        SetTypeError(GetClassTypeName((T*)0));
        m_iIndex++;
    }

public:
    //
    // Read next userdata
    // Userdata always valid if no error
    //  * value not userdata - error
    //  * nil value          - error
    //  * no arguments left  - error
    //  * result is NULL     - error
    //
    template <class T>
    void ReadUserData(T*& outValue)
    {
        InternalReadUserData(false, outValue, false);
    }

    //
    // Read next userdata, using default if needed
    // Userdata always valid if no error
    //  * value not userdata - error
    //  * nil value          - use defaultValue
    //  * no arguments left  - use defaultValue
    //  * result is NULL     - error
    //
    template <class T>
    void ReadUserData(T*& outValue, T* defaultValue)
    {
        InternalReadUserData(false, outValue, true, defaultValue);
    }

    //
    // Read next userdata, using NULL default if needed, allowing NULL result
    // Userdata might be NULL even when no error
    //  * false              - use NULL (For easier use of function returns as arguments)
    //  * value not userdata - error
    //  * nil value          - use NULL
    //  * no arguments left  - use NULL
    //
    template <class T>
    void ReadUserData(T*& outValue, int*** defaultValue)
    {
        assert(defaultValue == NULL);
        if (NextIsBool() && !lua_toboolean(m_luaVM, m_iIndex))
        {
            outValue = NULL;
            m_iIndex++;
            return;
        }
        InternalReadUserData(true, outValue, true, (T*)NULL);
    }

    //
    // Read next wrapped userdata
    //
    template <class T, class U>
    void ReadUserData(U*& outValue)
    {
        ReadUserData(outValue);
        if (outValue)
        {
            SString strErrorExpectedType;
            if (CheckWrappedUserDataType<T>(outValue, strErrorExpectedType))
                return;
            SetTypeError(strErrorExpectedType, m_iIndex - 1);
        }
    }

    //
    // Read CLuaArguments
    //
    void ReadLuaArguments(CLuaArguments& outValue)
    {
        outValue.ReadArguments(m_luaVM, m_iIndex);
        m_iIndex += outValue.Count();
    }

    //
    // Read one CLuaArgument
    //
    void ReadLuaArgument(CLuaArgument& outValue)
    {
        int iArgument = lua_type(m_luaVM, m_iIndex);
        if (iArgument != LUA_TNONE)
        {
            outValue.Read(m_luaVM, m_iIndex++);
            return;
        }

        SetTypeError("argument");
        m_iIndex++;
    }

    //
    // Read a table of userdatas
    //
    template <class T>
    void ReadUserDataTable(std::vector<T*>& outList)
    {
        if (lua_type(m_luaVM, m_iIndex) != LUA_TTABLE)
        {
            SetTypeError("table");
            m_iIndex++;
            return;
        }

        for (lua_pushnil(m_luaVM); lua_next(m_luaVM, m_iIndex) != 0; lua_pop(m_luaVM, 1))
        {
            // int idx = lua_tonumber ( m_luaVM, -2 );
            int iArgumentType = lua_type(m_luaVM, -1);

            T* value = NULL;
            if (iArgumentType == LUA_TLIGHTUSERDATA)
            {
                value = (T*)UserDataCast((T*)lua_touserdata(m_luaVM, -1), m_luaVM);
            }
            else if (iArgumentType == LUA_TUSERDATA)
            {
                value = (T*)UserDataCast(*((T**)lua_touserdata(m_luaVM, -1)), m_luaVM);
            }

            if (value != NULL)
                outList.push_back(value);
        }
        m_iIndex++;
    }

    //
    // Read a table of CLuaArguments
    //
    void ReadLuaArgumentsTable(CLuaArguments& outLuaArguments)
    {
        int iArgument = lua_type(m_luaVM, m_iIndex);
        if (iArgument == LUA_TTABLE)
        {
            for (lua_pushnil(m_luaVM); lua_next(m_luaVM, m_iIndex) != 0; lua_pop(m_luaVM, 1))
            {
                outLuaArguments.ReadArgument(m_luaVM, -1);
            }
            m_iIndex++;
            return;
        }

        SetTypeError("table");
        m_iIndex++;
    }

    //
    // Read a table of strings
    //
    void ReadStringTable(std::vector<SString>& outList)
    {
        outList.clear();

        int iArgument = lua_type(m_luaVM, m_iIndex);
        if (iArgument == LUA_TTABLE)
        {
            for (lua_pushnil(m_luaVM); lua_next(m_luaVM, m_iIndex) != 0; lua_pop(m_luaVM, 1))
            {
                int iArgument = lua_type(m_luaVM, -1);
                if (iArgument == LUA_TSTRING || iArgument == LUA_TNUMBER)
                {
                    uint uiLength = lua_strlen(m_luaVM, -1);
                    outList.push_back(SStringX(lua_tostring(m_luaVM, -1), uiLength));
                }
            }
            m_iIndex++;
            return;
        }

        SetTypeError("table");
        m_iIndex++;
    }

    //
    // Reads a table of key-value string pairs
    //
    void ReadPairTable(std::vector<std::pair<SString, SString>>& outPairs, bool bKeyValue = true)
    {
        outPairs.clear();

        int argument = lua_type(m_luaVM, m_iIndex);
        if (argument == LUA_TTABLE)
        {
            outPairs.reserve(64);
            // Iterate sequentially first( { key, value } )
            InternalReadPairTable(outPairs, m_iIndex);
            // Now we can take into account flag's pairs( [ key ] = value )
            if (bKeyValue)
                InternalReadPairKeyValue(outPairs, m_iIndex);
            outPairs.shrink_to_fit();
            ++m_iIndex;
            return;
        }

        SetTypeError("table");
        ++m_iIndex;
    }

    //
    // Reads a table as key-value string pair
    //
    void ReadStringMap(CStringMap& outMap)
    {
        outMap.clear();

        int argument = lua_type(m_luaVM, m_iIndex);
        if (argument == LUA_TTABLE)
        {
            InternalReadStringMap(outMap, m_iIndex);
            ++m_iIndex;
            return;
        }

        SetTypeError("table");
        ++m_iIndex;
    }

    //
    // Reads a table of numbers
    //
    template <typename T>
    void ReadNumberTable(std::vector<T>& outList)
    {
        outList.clear();
        int iArgument = lua_type(m_luaVM, m_iIndex);
        if (iArgument == LUA_TTABLE)
        {
            for (lua_pushnil(m_luaVM); lua_next(m_luaVM, m_iIndex) != 0; lua_pop(m_luaVM, 1))
            {
                int iArgument = lua_type(m_luaVM, -1);
                if (iArgument == LUA_TNUMBER)
                {
                    outList.push_back(static_cast<T>(lua_tonumber(m_luaVM, -1)));
                }
            }
            m_iIndex++;
            return;
        }
        SetTypeError("table");
        m_iIndex++;
    }

protected:
    // Takes key-value pair from top of the stack
    bool InternalReadPair(std::pair<SString, SString>& keyValue)
    {
        int keyType = lua_type(m_luaVM, -2);
        int valueType = lua_type(m_luaVM, -1);
        if (keyType == LUA_TSTRING || keyType == LUA_TNUMBER)
        {
            SString value;
            if (valueType == LUA_TSTRING || valueType == LUA_TNUMBER)
            {
                uint uiLength = lua_strlen(m_luaVM, -1);
                value.assign(lua_tostring(m_luaVM, -1), uiLength);
            }
            else if (valueType == LUA_TBOOLEAN)
            {
                value = (lua_toboolean(m_luaVM, -1) ? "1" : "0");
            }

            // Get rid of empty values
            if (value.length() > 0)
            {
                // Dumb number -> string convertion to avoid stack perturbations
                if (keyType == LUA_TNUMBER)
                {
                    char       s[LUAI_MAXNUMBER2STR];
                    lua_Number n = lua_tonumber(m_luaVM, -2);
                    lua_number2str(s, n);

                    keyValue = std::make_pair(SStringX(s), value);
                }
                else
                    keyValue = std::make_pair(SStringX(lua_tostring(m_luaVM, -2)), value);

                return true;
            }
        }

        return false;
    }

    // Reads { key, value } as a pair
    void InternalReadPairTable(std::vector<std::pair<SString, SString>>& outPairs, int iIndex)
    {
        // lua_next has a bug after it calling findindex internally
        // But we have to iterate sequentially right now. So luaL_getn is the solution.
        for (int i = 1; i <= luaL_getn(m_luaVM, iIndex); ++i)
        {
            lua_pushnumber(m_luaVM, i);
            lua_gettable(m_luaVM, iIndex);

            if (lua_type(m_luaVM, -1) == LUA_TTABLE && luaL_getn(m_luaVM, -1) >= 2)
            {
                lua_pushnumber(m_luaVM, 1);
                lua_gettable(m_luaVM, -2);

                lua_pushnumber(m_luaVM, 2);
                lua_gettable(m_luaVM, -3);

                std::pair<SString, SString> keyValue;

                if (InternalReadPair(keyValue))
                    outPairs.push_back(std::move(keyValue));

                lua_pop(m_luaVM, 2);
            }

            lua_pop(m_luaVM, 1);
        }
    }

    // Reads [ key ] = value as a pair
    void InternalReadPairKeyValue(std::vector<std::pair<SString, SString>>& outPairs, int iIndex)
    {
        lua_pushnil(m_luaVM);

        while (lua_next(m_luaVM, iIndex) != 0)
        {
            std::pair<SString, SString> keyValue;

            if (InternalReadPair(keyValue))
                outPairs.push_back(std::move(keyValue));

            lua_pop(m_luaVM, 1);
        }
    }

    void InternalReadStringMap(CStringMap& outMap, int iIndex)
    {
        lua_pushnil(m_luaVM);
        while (lua_next(m_luaVM, iIndex) != 0)
        {
            int keyType = lua_type(m_luaVM, -2);
            int valueType = lua_type(m_luaVM, -1);
            if (keyType == LUA_TSTRING)
            {
                SStringMapValue value;
                if (valueType == LUA_TSTRING || valueType == LUA_TNUMBER)
                {
                    uint uiLength = lua_strlen(m_luaVM, -1);
                    value.assign(lua_tostring(m_luaVM, -1), uiLength);
                }
                else if (valueType == LUA_TBOOLEAN)
                {
                    value = (lua_toboolean(m_luaVM, -1) ? "1" : "0");
                }
                else if (valueType == LUA_TTABLE)
                {
                    // Recurse
                    InternalReadStringMap(value.subMap, lua_gettop(m_luaVM));
                }
                outMap.insert({SStringX(lua_tostring(m_luaVM, -2)), value});
            }
            lua_pop(m_luaVM, 1);
        }
    }

public:
    //
    // Read a function, but don't do it yet due to Lua stack issues
    //
    void ReadFunction(CLuaFunctionRef& outValue, int defaultValue = -2)
    {
        assert(!m_pPendingFunctionOutValue);

        int iArgument = lua_type(m_luaVM, m_iIndex);
        if (iArgument == LUA_TFUNCTION)
        {
            m_pPendingFunctionOutValue = &outValue;
            m_pPendingFunctionIndex = m_iIndex++;
            return;
        }
        else if (iArgument == LUA_TNONE || iArgument == LUA_TNIL)
        {
            // Only valid default value for function is nil
            if (defaultValue == LUA_REFNIL)
            {
                outValue = CLuaFunctionRef();
                m_iIndex++;
                return;
            }
        }

        SetTypeError("function", m_iIndex);
        m_iIndex++;
    }

    //
    // Call after other arguments have been read
    //
    void ReadFunctionComplete()
    {
        if (!m_pPendingFunctionOutValue)
            return;

        // As we are going to change the stack, save any error info already gotten
        ResolveErrorGotArgumentTypeAndValue();

        assert(m_pPendingFunctionIndex != -1);
        *m_pPendingFunctionOutValue = luaM_toref(m_luaVM, m_pPendingFunctionIndex);
        if (VERIFY_FUNCTION(*m_pPendingFunctionOutValue))
        {
            m_pPendingFunctionIndex = -1;
            return;
        }

        SetTypeError("function", m_pPendingFunctionIndex);
        m_pPendingFunctionIndex = -1;
    }

    // Debug check
    bool IsReadFunctionPending() const { return m_pPendingFunctionOutValue && m_pPendingFunctionIndex != -1; }

    //
    // Peek at next type
    //
    bool NextIs(int iArgument, int iOffset = 0) const { return iArgument == lua_type(m_luaVM, m_iIndex + iOffset); }
    bool NextIsNone(int iOffset = 0) const { return NextIs(LUA_TNONE, iOffset); }
    bool NextIsNil(int iOffset = 0) const { return NextIs(LUA_TNIL, iOffset); }
    bool NextIsBool(int iOffset = 0) const { return NextIs(LUA_TBOOLEAN, iOffset); }
    bool NextIsUserData(int iOffset = 0) const { return NextIs(LUA_TUSERDATA, iOffset) || NextIsLightUserData(iOffset); }
    bool NextIsLightUserData(int iOffset = 0) const { return NextIs(LUA_TLIGHTUSERDATA, iOffset); }
    bool NextIsNumber(int iOffset = 0) const { return NextIs(LUA_TNUMBER, iOffset); }
    bool NextIsString(int iOffset = 0) const { return NextIs(LUA_TSTRING, iOffset); }
    bool NextIsTable(int iOffset = 0) const { return NextIs(LUA_TTABLE, iOffset); }
    bool NextIsFunction(int iOffset = 0) const { return NextIs(LUA_TFUNCTION, iOffset); }
    bool NextCouldBeNumber(int iOffset = 0) const { return NextIsNumber(iOffset) || NextIsString(iOffset); }
    bool NextCouldBeString(int iOffset = 0) const { return NextIsNumber(iOffset) || NextIsString(iOffset); }

    template <class T>
    bool NextIsEnumString(T&, int iOffset = 0)
    {
        int iArgument = lua_type(m_luaVM, m_iIndex + iOffset);
        if (iArgument == LUA_TSTRING)
        {
            T       eDummyResult;
            SString strValue = lua_tostring(m_luaVM, m_iIndex + iOffset);
            return StringToEnum(strValue, eDummyResult);
        }
        return false;
    }

    template <class T>
    bool NextIsUserDataOfType(int iOffset = 0) const
    {
        int iArgument = lua_type(m_luaVM, m_iIndex + iOffset);
        if (iArgument == LUA_TLIGHTUSERDATA)
        {
            if (UserDataCast((T*)lua_touserdata(m_luaVM, m_iIndex + iOffset), m_luaVM))
                return true;
        }
        else if (iArgument == LUA_TUSERDATA)
        {
            if (UserDataCast(*((T**)lua_touserdata(m_luaVM, m_iIndex + iOffset)), m_luaVM))
                return true;
        }
        return false;
    }

    bool NextIsVector4D() const
    {
        return (NextCouldBeNumber() && NextCouldBeNumber(1) && NextCouldBeNumber(2) && NextCouldBeNumber(3)) || NextIsUserDataOfType<CLuaVector4D>();
    }

    bool NextIsVector3D() const
    {
        return (NextCouldBeNumber() && NextCouldBeNumber(1) && NextCouldBeNumber(2)) || NextIsUserDataOfType<CLuaVector3D>() ||
               NextIsUserDataOfType<CLuaVector4D>();
    }

    bool NextIsVector2D() const
    {
        return (NextCouldBeNumber() && NextCouldBeNumber(1)) || NextIsUserDataOfType<CLuaVector2D>() || NextIsUserDataOfType<CLuaVector3D>() ||
               NextIsUserDataOfType<CLuaVector4D>();
    }

    //
    // Conditional reads. Default required in case condition is not met.
    //
    void ReadIfNextIsBool(bool& bOutValue, const bool bDefaultValue)
    {
        if (NextIsBool())
            ReadBool(bOutValue, bDefaultValue);
        else
            bOutValue = bDefaultValue;
    }

    template <class T>
    void ReadIfNextIsUserData(T*& outValue, T* defaultValue)
    {
        if (NextIsUserData())
            ReadUserData(outValue, defaultValue);
        else
            outValue = defaultValue;
    }

    template <class T, class U>
    void ReadIfNextIsNumber(T& outValue, const U& defaultValue)
    {
        if (NextIsNumber())
            ReadNumber(outValue, defaultValue);
        else
            outValue = defaultValue;
    }

    void ReadIfNextIsString(SString& outValue, const char* defaultValue)
    {
        if (NextIsString())
            ReadString(outValue, defaultValue);
        else
            outValue = defaultValue;
    }

    template <class T>
    void ReadIfNextIsEnumString(T& outValue, const T& defaultValue)
    {
        if (NextIsEnumString(outValue))
            ReadEnumString(outValue, defaultValue);
        else
            outValue = defaultValue;
    }

    template <class T, class U>
    void ReadIfNextCouldBeNumber(T& outValue, const U& defaultValue)
    {
        if (NextCouldBeNumber())
            ReadNumber(outValue, defaultValue);
        else
            outValue = defaultValue;
    }

    void ReadIfNextCouldBeString(SString& outValue, const char* defaultValue)
    {
        if (NextCouldBeString())
            ReadString(outValue, defaultValue);
        else
            outValue = defaultValue;
    }

    //
    // SetTypeError
    //
    void SetTypeError(const SString& strExpectedType, int iIndex = -1)
    {
        if (iIndex == -1)
            iIndex = m_iIndex;
        if (!m_bError || iIndex <= m_iErrorIndex)
        {
            m_bError = true;
            m_iErrorIndex = iIndex;
            m_strErrorExpectedType = strExpectedType;
            m_bResolvedErrorGotArgumentTypeAndValue = false;
            m_strErrorCategory = "Bad argument";
        }
    }

    //
    // HasErrors - Optional check if there are any unread arguments
    //
    bool HasErrors(bool bCheckUnusedArgs = false)
    {
        assert(!IsReadFunctionPending());
        if (bCheckUnusedArgs && lua_type(m_luaVM, m_iIndex) != LUA_TNONE)
            return true;

        // Output warning here (there's no better way to integrate it without huge code changes
        if (!m_bError && !m_strCustomWarning.empty())
        {
            #ifdef MTA_CLIENT
            CLuaFunctionDefs::m_pScriptDebugging->LogWarning(m_luaVM, m_strCustomWarning);
            #else
            g_pGame->GetScriptDebugging()->LogWarning(m_luaVM, m_strCustomWarning);
            #endif

            m_strCustomWarning.clear();
        }

        return m_bError;
    }

    //
    // GetErrorMessage
    //
    SString GetErrorMessage()
    {
        if (!m_bError)
            return "No error";

        if (m_bHasCustomMessage)
            return m_strCustomMessage;

        ResolveErrorGotArgumentTypeAndValue();

        // Compose error message
        SString strMessage("Expected %s at argument %d", *m_strErrorExpectedType, m_iErrorIndex);

        if (!m_strErrorGotArgumentType.empty())
        {
            strMessage += SString(", got %s", *m_strErrorGotArgumentType);

            // Append value if available
            if (!m_strErrorGotArgumentValue.empty())
                strMessage += SString(" '%s'", *m_strErrorGotArgumentValue);
        }

        return strMessage;
    }

    //
    // Put off getting error type and value until just before we need it
    //
    void ResolveErrorGotArgumentTypeAndValue()
    {
        if (!m_bError || m_bResolvedErrorGotArgumentTypeAndValue)
            return;

        m_bResolvedErrorGotArgumentTypeAndValue = true;

        if (!m_bHasCustomMessage)
        {
            int iArgument = lua_type(m_luaVM, m_iErrorIndex);
            m_strErrorGotArgumentType = EnumToString((eLuaType)iArgument);
            m_strErrorGotArgumentValue = lua_tostring(m_luaVM, m_iErrorIndex);

            if (iArgument == LUA_TLIGHTUSERDATA)
            {
                m_strErrorGotArgumentType = GetUserDataClassName(lua_touserdata(m_luaVM, m_iErrorIndex), m_luaVM);
                m_strErrorGotArgumentValue = "";
            }
            else if (iArgument == LUA_TUSERDATA)
            {
                m_strErrorGotArgumentType = GetUserDataClassName(*((void**)lua_touserdata(m_luaVM, m_iErrorIndex)), m_luaVM);
                m_strErrorGotArgumentValue = "";
            }
        }
    }

    //
    // Set version warning message
    //
    void SetVersionWarning(const char* szMinReq, const char* szSide, const char* szReason)
    {
        SetCustomWarning(
            SString("<min_mta_version> section in the meta.xml is incorrect or missing (expected at least %s %s because %s)", szSide, szMinReq, szReason));
    }

    //
    // Set custom error message
    //
    void SetCustomError(const char* szReason, const char* szCategory = "Bad usage")
    {
        if (!m_bError)
        {
            m_bError = true;
            m_strErrorCategory = szCategory;
            m_bHasCustomMessage = true;
            m_strCustomMessage = szReason;
        }
    }

    //
    // Make full error message
    //
    SString GetFullErrorMessage() { return SString("%s @ '%s' [%s]", *m_strErrorCategory, lua_tostring(m_luaVM, lua_upvalueindex(1)), *GetErrorMessage()); }

    //
    // Set custom warning message
    //
    void SetCustomWarning(const SString& message) { m_strCustomWarning = message; }

    //
    // Skip n arguments
    //
    void Skip(int n) { m_iIndex += n; }

    bool             m_bError;
    int              m_iErrorIndex;
    SString          m_strErrorExpectedType;
    int              m_iIndex;
    lua_State*       m_luaVM;
    CLuaFunctionRef* m_pPendingFunctionOutValue;
    int              m_pPendingFunctionIndex;
    bool             m_bResolvedErrorGotArgumentTypeAndValue;
    SString          m_strErrorGotArgumentType;
    SString          m_strErrorGotArgumentValue;
    SString          m_strErrorCategory;
    bool             m_bHasCustomMessage;
    SString          m_strCustomMessage;
    SString          m_strCustomWarning;
};
