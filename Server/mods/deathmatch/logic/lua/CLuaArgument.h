/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaArgument.h
 *  PURPOSE:     Lua argument handler class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
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
    CLuaArgument();
    CLuaArgument(const CLuaArgument& Argument, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables = NULL);
    CLuaArgument(lua_State* luaVM, int iArgument, CFastHashMap<const void*, CLuaArguments*>* pKnownTables = NULL);
    ~CLuaArgument();

    const CLuaArgument& operator=(const CLuaArgument& Argument);
    bool                operator==(const CLuaArgument& Argument) const;
    bool                operator!=(const CLuaArgument& Argument) const;

    void Read(lua_State* luaVM, int iArgument, CFastHashMap<const void*, CLuaArguments*>* pKnownTables = NULL);
    void Push(lua_State* luaVM, CFastHashMap<CLuaArguments*, int>* pKnownTables = NULL) const;

    void ReadBool(bool bBool);
    void ReadNumber(double dNumber);
    void ReadString(const std::string& string);
    void ReadString(const std::string_view& string);
    void ReadString(const char* string);
    void ReadElement(CElement* pElement);
    void ReadElementID(ElementID ID);
    void ReadScriptID(uint uiScriptID);
    void ReadTable(class CLuaArguments* table);

    int GetType() const { return m_iType; };

    bool               GetBoolean() const { return m_bBoolean; };
    lua_Number         GetNumber() const { return m_Number; };
    const std::string& GetString() { return m_strString; };
    void*              GetUserData() const { return m_pUserData; };
    CLuaArguments*     GetTable() const { return m_pTableData; }
    CElement*          GetElement() const;
    bool               GetAsString(SString& strBuffer);

    bool         ReadFromBitStream(NetBitStreamInterface& bitStream, std::vector<CLuaArguments*>* pKnownTables = NULL);
    bool         WriteToBitStream(NetBitStreamInterface& bitStream, CFastHashMap<CLuaArguments*, unsigned long>* pKnownTables = NULL) const;
    json_object* WriteToJSONObject(bool bSerialize = false, CFastHashMap<CLuaArguments*, unsigned long>* pKnownTables = NULL);
    bool         ReadFromJSONObject(json_object* object, std::vector<CLuaArguments*>* pKnownTables = NULL);
    char*        WriteToString(char* szBuffer, int length);

    bool IsEqualTo(const CLuaArgument& compareTo, std::set<const CLuaArguments*>* knownTables = nullptr) const;

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
    void LogUnableToPacketize(const char* szMessage) const;

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

    void CopyRecursive(const CLuaArgument& Argument, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables = NULL);
    void DeleteTableData();
};
