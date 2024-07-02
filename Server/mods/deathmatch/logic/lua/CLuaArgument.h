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
    CLuaArgument(const CLuaArgument& Argument, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables = nullptr) noexcept;
    CLuaArgument(lua_State* luaVM, int iArgument, CFastHashMap<const void*, CLuaArguments*>* pKnownTables = nullptr) noexcept;
    ~CLuaArgument() noexcept;

    const CLuaArgument& operator=(const CLuaArgument& Argument) noexcept;
    bool                operator==(const CLuaArgument& Argument) const noexcept;
    bool                operator!=(const CLuaArgument& Argument) const noexcept;

    void Read(lua_State* luaVM, int iArgument, CFastHashMap<const void*, CLuaArguments*>* pKnownTables = nullptr) noexcept;
    void Push(lua_State* luaVM, CFastHashMap<CLuaArguments*, int>* pKnownTables = nullptr) const noexcept;

    void ReadBool(bool bBool) noexcept;
    void ReadNumber(double dNumber) noexcept;
    void ReadString(const std::string& strString) noexcept;
    void ReadElement(const CElement* pElement) noexcept;
    void ReadElementID(ElementID ID) noexcept;
    void ReadScriptID(std::uint32_t uiScriptID) noexcept;
    void ReadTable(const CLuaArguments* table) noexcept;

    int GetType() const noexcept { return m_iType; }

    bool               GetBoolean() const noexcept { return m_bBoolean; }
    lua_Number         GetNumber() const noexcept { return m_Number; }
    const std::string& GetString() const noexcept { return m_strString; }
    std::string&       GetString() noexcept { return m_strString; }
    void*              GetUserData() const noexcept { return m_pUserData; }
    CElement*          GetElement() const noexcept;
    bool               GetAsString(SString& strBuffer) noexcept;

    bool         ReadFromBitStream(NetBitStreamInterface& bitStream, std::vector<CLuaArguments*>* pKnownTables = nullptr) noexcept;
    bool         WriteToBitStream(NetBitStreamInterface& bitStream, CFastHashMap<CLuaArguments*, std::uint32_t>* pKnownTables = nullptr) const noexcept;
    json_object* WriteToJSONObject(bool bSerialize = false, CFastHashMap<CLuaArguments*, std::uint32_t>* pKnownTables = nullptr) noexcept;
    bool         ReadFromJSONObject(json_object* object, std::vector<CLuaArguments*>* pKnownTables = nullptr) noexcept;
    char*        WriteToString(char* szBuffer, int length) noexcept;

    bool IsEqualTo(const CLuaArgument& compareTo, std::set<const CLuaArguments*>* knownTables = nullptr) const noexcept;

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
