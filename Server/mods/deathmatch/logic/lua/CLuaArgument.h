/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaArgument.h
 *  PURPOSE:     Lua argument handler class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <string>

extern "C"
{
    #include "lua.h"
}
#include "../common/CBitStream.h"
#include "json.h"

class CElement;
class CLuaArguments;

#define LUA_TTABLEREF 9
#define LUA_TSTRING_LONG 10

class CLuaArgument
{
public:
    CLuaArgument() noexcept;
    CLuaArgument(const CLuaArgument& argument, CFastHashMap<CLuaArguments*, CLuaArguments*>* knownTables = nullptr) noexcept;
    CLuaArgument(lua_State* luaVM, int argument, CFastHashMap<const void*, CLuaArguments*>* knownTables = nullptr) noexcept;
    ~CLuaArgument() noexcept;

    const CLuaArgument& operator=(const CLuaArgument& Argument) noexcept;
    bool                operator==(const CLuaArgument& Argument) const noexcept;
    bool                operator!=(const CLuaArgument& Argument) const noexcept;

    void Read(lua_State* luaVM, int argument, CFastHashMap<const void*, CLuaArguments*>* knownTables = nullptr) noexcept;
    void Push(lua_State* luaVM, CFastHashMap<CLuaArguments*, int>* knownTables = nullptr) const noexcept;

    void ReadBool(bool bBool) noexcept;
    void ReadNumber(double dNumber) noexcept;
    void ReadString(const char* string) noexcept;
    void ReadString(const std::string& strString) noexcept;
    void ReadString(const std::string_view& string) noexcept;
    void ReadElement(const CElement* pElement) noexcept;
    void ReadElementID(ElementID ID) noexcept;
    void ReadScriptID(std::uint32_t uiScriptID) noexcept;
    void ReadTable(const CLuaArguments* table) noexcept;

    int GetType() const noexcept { return m_iType; }

    bool               GetBoolean() const noexcept { return m_bBoolean; }
    lua_Number         GetNumber() const noexcept { return m_Number; }
    const std::string& GetString() const noexcept { return m_strString; }
    void*              GetUserData() const noexcept { return m_pUserData; }
    CLuaArguments*     GetTable() const noexcept { return m_pTableData; }
    CElement*          GetElement() const noexcept;
    bool               GetAsString(SString& strBuffer) noexcept;

    bool         ReadFromBitStream(NetBitStreamInterface& bitStream, std::vector<CLuaArguments*>* knownTables = nullptr) noexcept;
    bool         WriteToBitStream(NetBitStreamInterface& bitStream, CFastHashMap<CLuaArguments*, std::uint32_t>* knownTables = nullptr) const noexcept;
    json_object* WriteToJSONObject(bool serialize = false, CFastHashMap<CLuaArguments*, std::uint32_t>* knownTables = nullptr) noexcept;
    bool         ReadFromJSONObject(json_object* object, std::vector<CLuaArguments*>* knownTables = nullptr) noexcept;
    char*        WriteToString(char* buffer, int length) noexcept;

    bool IsEqualTo(const CLuaArgument& compareTo, std::set<const CLuaArguments*>* knownTables = nullptr) const noexcept;

    [[nodiscard]] bool IsString() const noexcept { return m_iType == LUA_TSTRING; }

    [[nodiscard]] bool TryGetString(std::string_view& string) const noexcept
    {
        if (IsString())
        {
            string = m_strString;
            return true;
        }

        string = {};
        return false;
    }

    [[nodiscard]] bool IsNumber() const noexcept { return m_iType == LUA_TNUMBER; }

    [[nodiscard]] bool TryGetNumber(lua_Number& number) const noexcept
    {
        if (IsNumber())
        {
            number = m_Number;
            return true;
        }

        number = {};
        return false;
    }

    [[nodiscard]] bool IsTable() const noexcept;

    [[nodiscard]] bool TryGetTable(CLuaArguments*& table) const noexcept
    {
        if (IsTable())
        {
            table = m_pTableData;
            return true;
        }

        table = nullptr;
        return false;
    }

private:
    void LogUnableToPacketize(const char* szMessage) const noexcept;

    int            m_iType;
    bool           m_bBoolean;
    lua_Number     m_Number;
    std::string    m_strString;
    void*          m_pUserData;
    CLuaArguments* m_pTableData;
    bool           m_bWeakTableRef;

#ifdef MTA_DEBUG
    std::string m_strFilename;
    int         m_iLine;
#endif

    void CopyRecursive(const CLuaArgument& Argument, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables = nullptr) noexcept;
    void DeleteTableData() noexcept;
};
