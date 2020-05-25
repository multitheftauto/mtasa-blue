#include "StdInc.h"

// Warn and truncate if key is too long
static void TruncateStringAndWarn(std::string& key, lua_State* const luaVM, CScriptDebugging* const scriptDebugging)
{
    if (key.length() > MAX_CUSTOMDATA_NAME_LENGTH)
    {
        static const char* formatString = "Truncated argument @ '%s' [string length reduced to " QUOTE_DEFINE(MAX_CUSTOMDATA_NAME_LENGTH) " characters at argument 2. Key: %s]";
        key = key.substr(0, MAX_CUSTOMDATA_NAME_LENGTH);
        scriptDebugging->LogCustom(luaVM, SString(formatString, lua_tostring(luaVM, lua_upvalueindex(1)), key.c_str()).c_str());
    }
}

#ifdef MTA_CLIENT
std::variant<bool, std::reference_wrapper<CLuaArgument>> CLuaElementDefs::GetElementData(lua_State* const luaVM, CClientEntity* const element, std::string key, const std::optional<bool> inherit)
#else
std::variant<bool, std::reference_wrapper<CLuaArgument>> CLuaElementDefs::getElementData(lua_State* const luaVM, CElement* const element, std::string key, const std::optional<bool> inherit)
#endif
{
    TruncateStringAndWarn(key, luaVM, m_pScriptDebugging);

#ifdef MTA_CLIENT
    CLuaArgument* const pVariable = element->GetCustomData(key.c_str(), inherit.value_or(true));
#else
    CLuaArgument* const pVariable = CStaticFunctionDefinitions::GetElementData(element, key.c_str(), inherit.value_or(true));
#endif

    if (pVariable)
        return *pVariable;

    return false;
}

#ifdef MTA_CLIENT
//  bool setElementData ( element theElement, string key, var value, [bool synchronize = true] )
bool CLuaElementDefs::SetElementData(lua_State* const luaVM, CClientEntity* const element, std::string key, CLuaArgument newValue, const std::optional<bool> optionalIsSynced)
#else
//  bool setElementData ( element theElement, string key, var value, [bool synchronize = true / string syncType = "broadcast"] )
bool CLuaElementDefs::setElementData(lua_State* const luaVM, CElement* const element, std::string key, CLuaArgument newValue, const std::optional<std::variant<bool, ESyncType>> optionalNewSyncType)
#endif
{
#ifndef MTA_CLIENT
    ESyncType newSyncType = ESyncType::BROADCAST;
    if (optionalNewSyncType)
    {
        const auto& value = optionalNewSyncType.value();
        if (std::holds_alternative<ESyncType>(value))
            newSyncType = std::get<ESyncType>(value);

        else if (!std::get<bool>(value)) // by default its broadcast, so only set it to local if user passed in 'false'
            newSyncType = ESyncType::LOCAL;
    }

    LogWarningIfPlayerHasNotJoinedYet(luaVM, element);
#endif

    TruncateStringAndWarn(key, luaVM, m_pScriptDebugging);

#ifdef MTA_CLIENT
    return CStaticFunctionDefinitions::SetElementData(*element, key.c_str(), newValue, optionalIsSynced.value_or(true));
#else
    return CStaticFunctionDefinitions::SetElementData(element, key.c_str(), newValue, newSyncType);
#endif
}


#ifdef MTA_CLIENT
bool CLuaElementDefs::HasElementData(lua_State* const luaVM, CClientEntity* const element, std::string key, const std::optional<bool> inherit)
#else
bool CLuaElementDefs::hasElementData(lua_State* const luaVM, CElement* const element, std::string key, const std::optional<bool> inherit)
#endif
{
    TruncateStringAndWarn(key, luaVM, m_pScriptDebugging);

#ifdef MTA_CLIENT
    return element->GetCustomData(key.c_str(), inherit.value_or(true)) != nullptr;
#else
    return CStaticFunctionDefinitions::GetElementData(element, key.c_str(), inherit.value_or(true)) != nullptr;
#endif

}
