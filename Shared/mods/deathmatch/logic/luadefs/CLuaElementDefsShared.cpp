#include "StdInc.h"

#ifdef CElement
#undef CElement
#endif
#ifdef MTA_CLIENT
using CElement = CClientEntity;
#endif

// Warn and truncate if key is too long
void TruncateStringAndWarn(lua_State* L, std::string& key)
{
    if (key.length() > MAX_CUSTOMDATA_NAME_LENGTH)
    {
#ifdef MTA_CLIENT
        const auto scriptDebugging = g_pClientGame->GetScriptDebugging();
#else
        const auto scriptDebugging = g_pGame->GetScriptDebugging();
#endif
        scriptDebugging->LogCustom(L, SString(
            "Truncated argument @ '%s' [string length reduced to " QUOTE_DEFINE(MAX_CUSTOMDATA_NAME_LENGTH) " characters at argument 2]",
            lua_tostring(L, lua_upvalueindex(1))
        ));
        key.resize(MAX_CUSTOMDATA_NAME_LENGTH);
    }
}

std::variant<bool, std::reference_wrapper<CLuaArgument>>
CLuaElementDefs::GetElementData(lua_State* L, CElement* element, std::string key, std::optional<bool> inherit)
{
    TruncateStringAndWarn(L, key);
    if (auto value = element->GetCustomData(key.c_str(), inherit.value_or(true)))
        return *value;
    return false;
}

#ifdef MTA_CLIENT
//  bool setElementData ( element theElement, string key, var value, [bool synchronize = true] )
bool CLuaElementDefs::SetElementData(lua_State* L, CClientEntity* element, std::string key, CLuaArgument newValue, std::optional<bool> optionalIsSynced)
#else
//  bool setElementData ( element theElement, string key, var value, [bool synchronize = true / string syncType = "broadcast"] )
bool CLuaElementDefs::SetElementData(lua_State* L, CElement* element, std::string key, CLuaArgument newValue, std::optional<std::variant<bool, ESyncType>> optionalNewSyncType)
#endif
{
    TruncateStringAndWarn(L, key);

#ifdef MTA_CLIENT
    return CStaticFunctionDefinitions::SetElementData(*element, key.c_str(), newValue, optionalIsSynced.value_or(true));
#else
    ESyncType newSyncType = ESyncType::BROADCAST;
    if (optionalNewSyncType)
    {
        const auto& value = optionalNewSyncType.value();
        if (std::holds_alternative<ESyncType>(value))
            newSyncType = std::get<ESyncType>(value);

        else if (!std::get<bool>(value)) // by default its broadcast, so only set it to local if user passed in 'false'
            newSyncType = ESyncType::LOCAL;
    }

    LogWarningIfPlayerHasNotJoinedYet(L, element);
    return CStaticFunctionDefinitions::SetElementData(element, key.c_str(), newValue, newSyncType);
#endif
}

bool CLuaElementDefs::HasElementData(lua_State* L, CElement* element, std::string key, std::optional<bool> inherit)
{
    TruncateStringAndWarn(L, key);
    return element->GetCustomData(key.c_str(), inherit.value_or(true));
}
