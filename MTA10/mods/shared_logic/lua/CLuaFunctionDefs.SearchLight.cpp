/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.SearchLight.cpp
*  PURPOSE:     Lua function definitions class
*
*****************************************************************************/
#include "StdInc.h"

int CLuaFunctionDefs::CreateSearchLight ( lua_State* luaVM )
{
//  searchlight createSearchLight ( float startX, float startY, float startZ, float endX, float endY, float endZ, float startRadius, float endRadius [, bool renderSpot = true ] )
    CVector vecStart, vecEnd; float startRadius, endRadius; bool renderSpot;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( vecStart.fX );
    argStream.ReadNumber ( vecStart.fY );
    argStream.ReadNumber ( vecStart.fZ );
    argStream.ReadNumber ( vecEnd.fX );
    argStream.ReadNumber ( vecEnd.fY );
    argStream.ReadNumber ( vecEnd.fZ );
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
