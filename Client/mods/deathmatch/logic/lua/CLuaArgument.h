/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/lua/CLuaArgument.h
 *  PURPOSE:     Lua argument class header
 *
 *****************************************************************************/

#pragma once

extern "C"
{
    #include "lua.h"
}
#include <net/bitstream.h>
#include <string>
#include "json.h"

class CClientEntity;
class CLuaArguments;

#define LUA_TTABLEREF 9
#define LUA_TSTRING_LONG 10

class CLuaArgument
{
public:
    CLuaArgument() noexcept;
    CLuaArgument(const CLuaArgument& Argument, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables = nullptr) noexcept;
    CLuaArgument(NetBitStreamInterface& bitStream, std::vector<CLuaArguments*>* pKnownTables = nullptr) noexcept;
    CLuaArgument(lua_State* luaVM, int iArgument, CFastHashMap<const void*, CLuaArguments*>* pKnownTables = nullptr) noexcept;
    ~CLuaArgument() noexcept;

    const CLuaArgument& operator=(const CLuaArgument& Argument) noexcept;
    bool                operator==(const CLuaArgument& Argument) noexcept;
    bool                operator!=(const CLuaArgument& Argument) noexcept;

    void Read(lua_State* luaVM, int iArgument, CFastHashMap<const void*, CLuaArguments*>* pKnownTables = nullptr) noexcept;
    void ReadBool(bool bBool) noexcept;
    void ReadNumber(double dNumber) noexcept;
    void ReadString(const char* string) noexcept;
    void ReadString(const std::string& strString) noexcept;
    void ReadString(const std::string_view& string) noexcept;
    void ReadElement(const CClientEntity* pElement) noexcept;
    void ReadScriptID(std::uint32_t uiScriptID) noexcept;
    void ReadElementID(ElementID ID) noexcept;
    void ReadTable(const CLuaArguments* table) noexcept;

    void Push(lua_State* luaVM, CFastHashMap<CLuaArguments*, int>* pKnownTables = nullptr) const noexcept;

    int GetType() const noexcept { return m_iType; }
    int GetIndex() const noexcept { return m_iIndex; }

    bool           GetBoolean() const noexcept { return m_bBoolean; }
    lua_Number     GetNumber() const noexcept { return m_Number; }
    const SString& GetString() const noexcept { return m_strString; }
    SString& GetString() noexcept { return m_strString; }
    void*          GetUserData() const noexcept { return m_pUserData; }
    CClientEntity* GetElement() const noexcept;
    CLuaArguments* GetTable() const noexcept { return m_pTableData; }

    bool         ReadFromBitStream(NetBitStreamInterface& bitStream, std::vector<CLuaArguments*>* pKnownTables = nullptr) noexcept;
    bool         WriteToBitStream(NetBitStreamInterface& bitStream, CFastHashMap<CLuaArguments*, std::uint32_t>* pKnownTables = nullptr) const noexcept;
    json_object* WriteToJSONObject(bool bSerialize = false, CFastHashMap<CLuaArguments*, std::uint32_t>* pKnownTables = nullptr) noexcept;
    bool         ReadFromJSONObject(json_object* object, std::vector<CLuaArguments*>* pKnownTables = nullptr) noexcept;
    char*        WriteToString(char* szBuffer, int length) noexcept;

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

    [[nodiscard]] bool TryGetTable(CLuaArguments*& table)
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
    int            m_iIndex;
    bool           m_bBoolean;
    lua_Number     m_Number;
    SString        m_strString;
    void*          m_pUserData;
    CLuaArguments* m_pTableData;
    bool           m_bWeakTableRef;

#ifdef MTA_DEBUG
    std::string m_strFilename;
    int         m_iLine;
#endif

    void CopyRecursive(const CLuaArgument& Argument, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables = nullptr) noexcept;
    bool CompareRecursive(const CLuaArgument& Argument, std::set<CLuaArguments*>* pKnownTables = nullptr) noexcept;
    void DeleteTableData() noexcept;
};
