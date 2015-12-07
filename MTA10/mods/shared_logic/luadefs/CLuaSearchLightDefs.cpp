/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CLuaSearchLightDefs.cpp
*  PURPOSE:     Lua searchlight class functions
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaSearchLightDefs::LoadFunctions ( void )
{
    CLuaCFunctions::AddFunction ( "createSearchLight", CreateSearchLight );
}

void CLuaSearchLightDefs::AddClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "create", "createSearchLight" );

    lua_registerclass ( luaVM, "SearchLight", "Element" );
}

int CLuaSearchLightDefs::CreateSearchLight ( lua_State* luaVM )
{
    //  searchlight createSearchLight ( float startX, float startY, float startZ, float endX, float endY, float endZ, float startRadius, float endRadius [, bool renderSpot = true ] )
    CVector vecStart, vecEnd; float startRadius, endRadius; bool renderSpot;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecStart );
    argStream.ReadVector3D ( vecEnd );
    argStream.ReadNumber ( startRadius );
    argStream.ReadNumber ( endRadius );
    argStream.ReadBool ( renderSpot, true );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        CResource* pResource = pLuaMain ? pLuaMain->GetResource () : nullptr;

        if ( pResource )
        {
            auto pLight = CStaticFunctionDefinitions::CreateSearchLight ( *pResource, vecStart, vecEnd, startRadius, endRadius, renderSpot );
            if ( pLight )
            {
                CElementGroup* pGroup = pResource->GetElementGroup ();
                if ( pGroup )
                    pGroup->Add ( pLight );

                lua_pushelement ( luaVM, pLight );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}
