#include "StdInc.h"

// Warn and truncate if key is too long(luaVM must be in a var luaVM)
// is undef-ed at the end of the element data func defs
#define EDATA_KEY_WARN_TRUNCATE(key) \
if ((key).length() > MAX_CUSTOMDATA_NAME_LENGTH) \
{ \
    m_pScriptDebugging->LogCustom(luaVM, SString("Truncated argument @ '%s' [string length reduced to " QUOTE_DEFINE(MAX_CUSTOMDATA_NAME_LENGTH) " characters at argument 2]", lua_tostring(luaVM, lua_upvalueindex(1))).c_str()); \
    (key) = (key).substr(0, MAX_CUSTOMDATA_NAME_LENGTH); \
} \


#ifdef MTA_CLIENT
std::variant<bool, CLuaArgument*> CLuaElementDefs::GetElementData(lua_State* const luaVM, CClientEntity* const element, std::string key, const std::optional<bool> inherit)
#else
std::variant<bool, CLuaArgument*> CLuaElementDefs::getElementData(lua_State* const luaVM, CElement* const element, std::string key, const std::optional<bool> inherit)
#endif
{
    EDATA_KEY_WARN_TRUNCATE(key);

#ifdef MTA_CLIENT
    CLuaArgument* const pVariable = element->GetCustomData(key.c_str(), inherit.value_or(true));
#else
    CLuaArgument* const pVariable = CStaticFunctionDefinitions::GetElementData(element, key.c_str(), inherit.value_or(true));
#endif

    return { pVariable ? pVariable : false };
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
    if (optionalNewSyncType.has_value())
    {
        const auto& value = optionalNewSyncType.value();
        if (std::holds_alternative<bool>(value) && !std::get<bool>(value))
            newSyncType = ESyncType::LOCAL;
        else
            newSyncType = std::get<ESyncType>(value);
    }

    LogWarningIfPlayerHasNotJoinedYet(luaVM, element);
#endif
    EDATA_KEY_WARN_TRUNCATE(key);


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
    EDATA_KEY_WARN_TRUNCATE(key);

#ifdef MTA_CLIENT
    return element->GetCustomData(key.c_str(), inherit.value_or(true)) != nullptr;
#else
    return CStaticFunctionDefinitions::GetElementData(element, key.c_str(), inherit.value_or(true)) != nullptr;
#endif

}

#undef EDATA_KEY_WARN_TRUNCATE
