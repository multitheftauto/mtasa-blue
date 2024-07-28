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
    CLuaArgument();
    CLuaArgument(const CLuaArgument& Argument, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables = NULL);
    CLuaArgument(NetBitStreamInterface& bitStream, std::vector<CLuaArguments*>* pKnownTables = NULL);
    CLuaArgument(lua_State* luaVM, int iArgument, CFastHashMap<const void*, CLuaArguments*>* pKnownTables = NULL);
    ~CLuaArgument();

    const CLuaArgument& operator=(const CLuaArgument& Argument);
    bool                operator==(const CLuaArgument& Argument);
    bool                operator!=(const CLuaArgument& Argument);

    void Read(lua_State* luaVM, int iArgument, CFastHashMap<const void*, CLuaArguments*>* pKnownTables = NULL);
    void ReadBool(bool bBool);
    void ReadNumber(double dNumber);
    void ReadString(const std::string& string);
    void ReadString(const std::string_view& string);
    void ReadString(const char* string);
    void ReadElement(CClientEntity* pElement);
    void ReadScriptID(uint uiScriptID);
    void ReadElementID(ElementID ID);
    void ReadTable(class CLuaArguments* table);

    void Push(lua_State* luaVM, CFastHashMap<CLuaArguments*, int>* pKnownTables = NULL) const;

    int GetType() const { return m_iType; };
    int GetIndex() const { return m_iIndex; };

    bool           GetBoolean() const { return m_bBoolean; };
    lua_Number     GetNumber() const { return m_Number; };
    const SString& GetString() { return m_strString; };
    void*          GetUserData() const { return m_pUserData; };
    CLuaArguments* GetTable() const { return m_pTableData; }
    CClientEntity* GetElement() const;

    bool         ReadFromBitStream(NetBitStreamInterface& bitStream, std::vector<CLuaArguments*>* pKnownTables = NULL);
    bool         WriteToBitStream(NetBitStreamInterface& bitStream, CFastHashMap<CLuaArguments*, unsigned long>* pKnownTables = NULL) const;
    json_object* WriteToJSONObject(bool bSerialize = false, CFastHashMap<CLuaArguments*, unsigned long>* pKnownTables = NULL);
    bool         ReadFromJSONObject(json_object* object, std::vector<CLuaArguments*>* pKnownTables = NULL);
    char*        WriteToString(char* szBuffer, int length);

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
    void LogUnableToPacketize(const char* szMessage) const;

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

    void CopyRecursive(const CLuaArgument& Argument, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables = NULL);
    bool CompareRecursive(const CLuaArgument& Argument, std::set<CLuaArguments*>* pKnownTables = NULL);
    void DeleteTableData();
};
