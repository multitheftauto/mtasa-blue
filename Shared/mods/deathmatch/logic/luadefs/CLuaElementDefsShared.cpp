/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/luadefs/CLuaElementDefsShared.cpp
 *  PURPOSE:     Lua element definitions class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "luadefs/CLuaElementDefs.h"
#include "CScriptArgReader.h"
#include "CStaticFunctionDefinitions.h"

int CLuaElementDefs::GetElementData(lua_State* luaVM)
{
    //  var getElementData ( element theElement, string key [, inherit = true] )

    CElement* pElement;
    SString   strKey;
    bool      bInherit;

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
                                                             *SString("string length reduced to %d characters at argument 2", MAX_CUSTOMDATA_NAME_LENGTH)));
                strKey = strKey.Left(MAX_CUSTOMDATA_NAME_LENGTH);
            }

#ifdef MTA_CLIENT
            CLuaArgument* pVariable = pElement->GetCustomData(strKey, bInherit);
#else
            CLuaArgument* pVariable = CStaticFunctionDefinitions::GetElementData(pElement, strKey, bInherit);
#endif
            if (pVariable)
            {
                pVariable->Push(luaVM);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

CLuaArguments CLuaElementDefs::GetAllElementData(CElement* pElement)
{
    CLuaArguments customData;
    pElement->GetAllCustomData(&customData);
    return customData;
}

int CLuaElementDefs::HasElementData(lua_State* luaVM)
{
    //  bool hasElementData ( element theElement, string key [, bool inherit = true ] )

    CElement* pElement;
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
                                                     *SString("string length reduced to %d characters at argument 2", MAX_CUSTOMDATA_NAME_LENGTH)));
        strKey = strKey.Left(MAX_CUSTOMDATA_NAME_LENGTH);
    }

    // Check if data exists with the given key
    bool exists = pElement->GetCustomData(strKey, bInherit) != nullptr;
    lua_pushboolean(luaVM, exists);
    return 1;
}
