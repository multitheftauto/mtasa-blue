/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaArguments.h
 *  PURPOSE:     Lua argument manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

extern "C"
{
    #include "lua.h"
}

#include "CLuaArgument.h"
#include <vector>
#include "../common/CBitStream.h"
#include "json.h"
#include "CLuaFunctionRef.h"

inline void LUA_CHECKSTACK(lua_State* L, int size)
{
    if (lua_getstackgap(L) < size + 5)
        lua_checkstack(L, (size + 2) * 3 + 4);
}

class CAccessControlList;
class CAccessControlListGroup;
class CAccount;
class CBan;
class CElement;
class CLuaTimer;
class CResource;
class CTextDisplay;
class CTextItem;
class CDbJobData;

class CLuaArguments;

class CLuaArguments
{
public:
    CLuaArguments() {}
    CLuaArguments(const CLuaArguments& Arguments, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables = NULL);

    ~CLuaArguments() { DeleteArguments(); };

    void CopyRecursive(const CLuaArguments& Arguments, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables = NULL);

    const CLuaArguments& operator=(const CLuaArguments& Arguments);
    CLuaArgument*        operator[](const unsigned int uiPosition) const;

    void ReadArgument(lua_State* luaVM, signed int uiIndex);
    void ReadArguments(lua_State* luaVM, signed int uiIndexBegin = 1);
    void PushArguments(lua_State* luaVM) const;
    void PushArguments(const CLuaArguments& Arguments);
    bool Call(class CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, CLuaArguments* returnValues = NULL) const;
    bool CallGlobal(class CLuaMain* pLuaMain, const char* szFunction, CLuaArguments* returnValues = NULL) const;

    void ReadTable(lua_State* luaVM, int iIndexBegin, CFastHashMap<const void*, CLuaArguments*>* pKnownTables = NULL);
    void PushAsTable(lua_State* luaVM, CFastHashMap<CLuaArguments*, int>* pKnownTables = nullptr) const;

    CLuaArgument* PushNil();
    CLuaArgument* PushBoolean(bool bBool);
    CLuaArgument* PushNumber(double dNumber);
    CLuaArgument* PushString(const std::string& string);
    CLuaArgument* PushString(const std::string_view& string);
    CLuaArgument* PushString(const char* string);
    CLuaArgument* PushElement(CElement* pElement);
    CLuaArgument* PushBan(CBan* pBan);
    CLuaArgument* PushACL(CAccessControlList* pACL);
    CLuaArgument* PushACLGroup(CAccessControlListGroup* pACLGroup);
    CLuaArgument* PushAccount(CAccount* pAccount);
    CLuaArgument* PushResource(CResource* pResource);
    CLuaArgument* PushTextDisplay(CTextDisplay* pTextDisplay);
    CLuaArgument* PushTextItem(CTextItem* pTextItem);
    CLuaArgument* PushTimer(CLuaTimer* pLuaTimer);
    CLuaArgument* PushDbQuery(CDbJobData* pJobData);

    CLuaArgument* PushArgument(const CLuaArgument& argument);
    CLuaArgument* PushTable(CLuaArguments* table);

    void DeleteArguments();
    void ValidateTableKeys();
    void Pop();

    bool         ReadFromBitStream(NetBitStreamInterface& bitStream, std::vector<CLuaArguments*>* pKnownTables = NULL);
    bool         ReadFromJSONString(const char* szJSON);
    bool         WriteToBitStream(NetBitStreamInterface& bitStream, CFastHashMap<CLuaArguments*, unsigned long>* pKnownTables = NULL) const;
    bool         WriteToJSONString(std::string& strJSON, bool bSerialize = false, int flags = JSON_C_TO_STRING_PLAIN);
    json_object* WriteTableToJSONObject(bool bSerialize = false, CFastHashMap<CLuaArguments*, unsigned long>* pKnownTables = NULL);
    json_object* WriteToJSONArray(bool bSerialize);
    bool         ReadFromJSONObject(json_object* object, std::vector<CLuaArguments*>* pKnownTables = NULL);
    bool         ReadFromJSONArray(json_object* object, std::vector<CLuaArguments*>* pKnownTables = NULL);

    [[nodiscard]] bool IsNotEmpty() const noexcept { return !m_Arguments.empty(); }
    [[nodiscard]] bool IsEmpty() const noexcept { return m_Arguments.empty(); }

    [[nodiscard]] std::vector<CLuaArgument*>::size_type Count() const noexcept { return m_Arguments.size(); }

    [[nodiscard]] std::vector<CLuaArgument*>::const_iterator begin() const noexcept { return m_Arguments.begin(); }
    [[nodiscard]] std::vector<CLuaArgument*>::const_iterator end() const noexcept { return m_Arguments.end(); }

    bool IsEqualTo(const CLuaArguments& compareTo, std::set<const CLuaArguments*>* knownTables = nullptr) const;

private:
    std::vector<CLuaArgument*> m_Arguments;
};
