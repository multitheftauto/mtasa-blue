/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/lua/CLuaArguments.h
 *  PURPOSE:     Lua arguments manager class header
 *
 *****************************************************************************/

#pragma once

extern "C"
{
    #include "lua.h"
}

#include <net/bitstream.h>
#include "CLuaArgument.h"
#include "json.h"
#include <vector>
#include "CLuaFunctionRef.h"

inline void LUA_CHECKSTACK(lua_State* L, int size)
{
    if (lua_getstackgap(L) < size + 5)
        lua_checkstack(L, (size + 2) * 3 + 4);
}

class CLuaArguments;

class CLuaArguments
{
public:
    CLuaArguments() noexcept {}
    CLuaArguments(const CLuaArguments& Arguments, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables = nullptr) noexcept;
    CLuaArguments(NetBitStreamInterface& bitStream, std::vector<CLuaArguments*>* pKnownTables = nullptr) noexcept;
    ~CLuaArguments() noexcept { DeleteArguments(); };

    void CopyRecursive(const CLuaArguments& Arguments, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables = nullptr) noexcept;

    const CLuaArguments& operator=(const CLuaArguments& Arguments) noexcept;
    CLuaArgument*        operator[](const std::size_t uiPosition) const noexcept;

    void ReadArgument(lua_State* luaVM, int uiIndex) noexcept;
    void ReadArguments(lua_State* luaVM, int uiIndexBegin = 1) noexcept;
    void PushArguments(lua_State* luaVM) const noexcept;
    void PushArguments(const CLuaArguments& Arguments) noexcept;
    bool Call(class CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, CLuaArguments* returnValues = nullptr) const noexcept;
    bool CallGlobal(class CLuaMain* pLuaMain, const char* szFunction, CLuaArguments* returnValues = nullptr) const noexcept;

    void ReadTable(lua_State* luaVM, int iIndexBegin, CFastHashMap<const void*, CLuaArguments*>* pKnownTables = nullptr) noexcept;
    void PushAsTable(lua_State* luaVM, CFastHashMap<CLuaArguments*, int>* pKnownTables = nullptr) const noexcept;

    CLuaArgument* PushNil() noexcept;
    CLuaArgument* PushBoolean(bool bBool) noexcept;
    CLuaArgument* PushNumber(double dNumber) noexcept;
    CLuaArgument* PushString(const std::string& strString) noexcept;
    CLuaArgument* PushElement(const CClientEntity* pElement) noexcept;
    CLuaArgument* PushArgument(const CLuaArgument& argument) noexcept;
    CLuaArgument* PushResource(const CResource* pResource) noexcept;
    CLuaArgument* PushTable(const CLuaArguments* table) noexcept;

    void DeleteArguments() noexcept;
    void Pop() noexcept;

    bool         ReadFromBitStream(NetBitStreamInterface& bitStream, std::vector<CLuaArguments*>* pKnownTables = nullptr) noexcept;
    bool         WriteToBitStream(NetBitStreamInterface& bitStream, CFastHashMap<CLuaArguments*, std::uint32_t>* pKnownTables = nullptr) const noexcept;
    void         ValidateTableKeys() noexcept;
    bool         ReadFromJSONString(const char* szJSON) noexcept;
    bool         WriteToJSONString(std::string& strJSON, bool bSerialize = false, int flags = JSON_C_TO_STRING_PLAIN) noexcept;
    json_object* WriteTableToJSONObject(bool bSerialize = false, CFastHashMap<CLuaArguments*, std::uint32_t>* pKnownTables = nullptr) noexcept;
    json_object* WriteToJSONArray(bool bSerialize) noexcept;
    bool         ReadFromJSONObject(json_object* object, std::vector<CLuaArguments*>* pKnownTables = nullptr) noexcept;
    bool         ReadFromJSONArray(json_object* object, std::vector<CLuaArguments*>* pKnownTables = nullptr) noexcept;

    CLuaArgument*&       at(const std::size_t index) noexcept;
    CLuaArgument* const& at(const std::size_t index) const noexcept;

    std::size_t Count() const noexcept { return m_Arguments.size(); }

    std::vector<CLuaArgument*>::const_iterator begin() const noexcept { return m_Arguments.cbegin(); }
    std::vector<CLuaArgument*>::const_iterator end() const noexcept { return m_Arguments.cend(); }

    std::vector<CLuaArgument*>::iterator begin() noexcept { return m_Arguments.begin(); }
    std::vector<CLuaArgument*>::iterator end() noexcept { return m_Arguments.end(); }

private:
    std::vector<CLuaArgument*> m_Arguments;
};
