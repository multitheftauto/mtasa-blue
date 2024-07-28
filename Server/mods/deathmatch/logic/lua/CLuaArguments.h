/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaArguments.h
 *  PURPOSE:     Lua argument manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
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
    CLuaArguments() noexcept {}
    CLuaArguments(const CLuaArguments& arguments, CFastHashMap<CLuaArguments*, CLuaArguments*>* knownTables = nullptr) noexcept;

    ~CLuaArguments() noexcept { DeleteArguments(); }

    void CopyRecursive(const CLuaArguments& arguments, CFastHashMap<CLuaArguments*, CLuaArguments*>* knownTables = nullptr) noexcept;

    const CLuaArguments& operator=(const CLuaArguments& arguments) noexcept;
    CLuaArgument*        operator[](const std::size_t position) const noexcept;

    void ReadArgument(lua_State* luaVM, int index) noexcept;
    void ReadArguments(lua_State* luaVM, int indexBegin = 1) noexcept;
    void PushArguments(lua_State* luaVM) const noexcept;
    void PushArguments(const CLuaArguments& arguments) noexcept;
    bool Call(class CLuaMain* luaMain, const CLuaFunctionRef& luaFunction, CLuaArguments* returnValues = nullptr) const noexcept;
    bool CallGlobal(class CLuaMain* luaMain, const char* function, CLuaArguments* returnValues = nullptr) const noexcept;

    void ReadTable(lua_State* luaVM, int indexBegin, CFastHashMap<const void*, CLuaArguments*>* knownTables = nullptr) noexcept;
    void PushAsTable(lua_State* luaVM, CFastHashMap<CLuaArguments*, int>* knownTables = nullptr) const noexcept;

    CLuaArgument* PushNil() noexcept;
    CLuaArgument* PushBoolean(bool bBool) noexcept;
    CLuaArgument* PushNumber(double dNumber) noexcept;
    CLuaArgument* PushString(const char* string) noexcept;
    CLuaArgument* PushString(const std::string& strString) noexcept;
    CLuaArgument* PushString(const std::string_view& string) noexcept;
    CLuaArgument* PushElement(const CElement* pElement) noexcept;
    CLuaArgument* PushBan(const CBan* pBan) noexcept;
    CLuaArgument* PushACL(const CAccessControlList* pACL) noexcept;
    CLuaArgument* PushACLGroup(const CAccessControlListGroup* pACLGroup) noexcept;
    CLuaArgument* PushAccount(const CAccount* pAccount) noexcept;
    CLuaArgument* PushResource(const CResource* pResource) noexcept;
    CLuaArgument* PushTextDisplay(const CTextDisplay* pTextDisplay) noexcept;
    CLuaArgument* PushTextItem(const CTextItem* pTextItem) noexcept;
    CLuaArgument* PushTimer(const CLuaTimer* pLuaTimer) noexcept;
    CLuaArgument* PushDbQuery(const CDbJobData* pJobData) noexcept;

    CLuaArgument* PushArgument(const CLuaArgument& argument) noexcept;
    CLuaArgument* PushTable(const CLuaArguments* table) noexcept;

    void DeleteArguments() noexcept;
    void ValidateTableKeys() noexcept;
    void Pop() noexcept;

    bool         ReadFromBitStream(NetBitStreamInterface& bitStream, std::vector<CLuaArguments*>* knownTables = nullptr) noexcept;
    bool         ReadFromJSONString(const char* json) noexcept;
    bool         WriteToBitStream(NetBitStreamInterface& bitStream, CFastHashMap<CLuaArguments*, std::uint32_t>* knownTables = nullptr) const noexcept;
    bool         WriteToJSONString(std::string& json, bool bSerialize = false, int flags = JSON_C_TO_STRING_PLAIN) noexcept;
    json_object* WriteTableToJSONObject(bool serialize = false, CFastHashMap<CLuaArguments*, std::uint32_t>* knownTables = nullptr) noexcept;
    json_object* WriteToJSONArray(bool serialize) noexcept;
    bool         ReadFromJSONObject(json_object* object, std::vector<CLuaArguments*>* knownTables = nullptr) noexcept;
    bool         ReadFromJSONArray(json_object* object, std::vector<CLuaArguments*>* knownTables = nullptr) noexcept;

    [[nodiscard]] std::vector<CLuaArgument*>::size_type Count() const noexcept { return m_Arguments.size(); }

    [[nodiscard]] bool IsEmpty() const noexcept { return m_Arguments.empty(); }
    [[nodiscard]] bool IsNotEmpty() const noexcept { return !IsEmpty(); }
    
    std::vector<CLuaArgument*>& GetArguments() noexcept { return m_Arguments; }
    const std::vector<CLuaArgument*>& GetArguments() const noexcept { return m_Arguments; }

    [[nodiscard]] std::vector<CLuaArgument*>::iterator begin() noexcept { return m_Arguments.begin(); }
    [[nodiscard]] std::vector<CLuaArgument*>::iterator end() noexcept { return m_Arguments.end(); }
    [[nodiscard]] std::vector<CLuaArgument*>::const_iterator begin() const noexcept { return m_Arguments.cbegin(); }
    [[nodiscard]] std::vector<CLuaArgument*>::const_iterator end() const noexcept { return m_Arguments.cend(); }

    bool IsEqualTo(const CLuaArguments& compareTo, std::set<const CLuaArguments*>* knownTables = nullptr) const noexcept;

private:
    std::vector<CLuaArgument*> m_Arguments;
};
