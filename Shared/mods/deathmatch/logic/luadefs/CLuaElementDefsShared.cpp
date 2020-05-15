#include "StdInc.h"


int CLuaElementDefs::GetElementData(lua_State* luaVM)
{
    //  var getElementData ( element theElement, string key [, inherit = true] )

#ifdef MTA_CLIENT
    CClientEntity* pElement;
#else
    CElement* pElement;
#endif
    SString strKey;
    bool    bInherit;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadString(strKey);
    argStream.ReadBool(bInherit, true);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            if (strKey.length() > MAX_CUSTOMDATA_NAME_LENGTH)
            {
                // Warn and truncate if key is too long
                m_pScriptDebugging->LogCustom(luaVM, SString("Truncated argument @ '%s' [%s]", lua_tostring(luaVM, lua_upvalueindex(1)),
                    "string length reduced to " QUOTE_DEFINE(MAX_CUSTOMDATA_NAME_LENGTH) " characters at argument 2"));
                strKey = strKey.Left(MAX_CUSTOMDATA_NAME_LENGTH);
            }

#ifdef MTA_CLIENT
            CLuaArgument* pVariable = pElement->GetCustomData(strKey, bInherit);
            if (pVariable)
            {
                pVariable->Push(luaVM);
                return 1;
            }
#else
            auto customData = CStaticFunctionDefinitions::GetElementData(pElement, strKey, bInherit);
            if (customData)
            {
                customData->variable.Push(luaVM);
                return 1;
            }
#endif
            
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::HasElementData(lua_State* luaVM)
{
    //  bool hasElementData ( element theElement, string key [, bool inherit = true ] )

#ifdef MTA_CLIENT
    CClientEntity* pElement;
#else
    CElement* pElement;
#endif
    SString   strKey;
    bool      bInherit;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadString(strKey);
    argStream.ReadBool(bInherit, true);

    if (argStream.HasErrors())
    {
        return luaL_error(luaVM, argStream.GetFullErrorMessage());
    }

    if (strKey.length() > MAX_CUSTOMDATA_NAME_LENGTH)
    {
        // Warn and truncate if key is too long
        m_pScriptDebugging->LogCustom(luaVM, SString("Truncated argument @ '%s' [%s]", lua_tostring(luaVM, lua_upvalueindex(1)),
            "string length reduced to " QUOTE_DEFINE(MAX_CUSTOMDATA_NAME_LENGTH) " characters at argument 2"));
        strKey = strKey.Left(MAX_CUSTOMDATA_NAME_LENGTH);
    }

    // Check if data exists with the given key
    const bool exists = pElement->GetCustomData(strKey, bInherit) != nullptr;
    lua_pushboolean(luaVM, exists);

    return 1;
}
