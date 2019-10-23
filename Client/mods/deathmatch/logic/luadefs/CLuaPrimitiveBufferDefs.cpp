/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaPrimitiveBufferDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

void CLuaPrimitiveBufferDefs::LoadFunctions()
{
    std::map<const char*, lua_CFunction> functions{
        {"primitiveBufferCreate", PrimitiveBufferCreate},
    };

    // Add functions
    for (const auto& pair : functions)
    {
        CLuaCFunctions::AddFunction(pair.first, pair.second);
    }
}

void CLuaPrimitiveBufferDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "Create", "primitiveBufferCreate");
    //lua_classvariable(luaVM, "someVariable", nullptr, "functionName");

    lua_registerclass(luaVM, "PrimitiveBuffer", "Element");
}

int CLuaPrimitiveBufferDefs::PrimitiveBufferCreate(lua_State* luaVM)
{
    CVector2D        vecPosition;
    float            fRadius = 0.1f;
    CScriptArgReader argStream(luaVM);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                // Create it and return it
                CClientPrimitiveBuffer* pBuffer = CStaticFunctionDefinitions::CreatePrimitiveBuffer(*pResource);
                if (pBuffer)
                {
                    CElementGroup* pGroup = pResource->GetElementGroup();
                    if (pGroup)
                    {
                        pGroup->Add(pBuffer);
                    }
                    lua_pushelement(luaVM, pBuffer);
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}
