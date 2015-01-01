/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/oopdefs/CLuaOOPFunctionDefs.ACL.cpp
*  PURPOSE:     Lua OOP specific function definitions class
*  DEVELOPERS:  Qais Patankar <qaisjp@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"

int CLuaOOPDefs::DoesACLGroupContainObject ( lua_State* luaVM )
{
    //  bool isObjectInACLGroup ( aclgroup theGroup, string theObject )
    SString strObject; CAccessControlListGroup* pGroup; CAccessControlListGroupObject::EObjectType GroupObjectType;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pGroup );
    argStream.ReadString ( strObject );

    if ( !argStream.HasErrors () )
    {
        // Figure out what type of object this is
        const char* szObjectAfterDot = strObject;
        if ( StringBeginsWith ( strObject, "resource." ) ) {
            szObjectAfterDot += 9;
            GroupObjectType = CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE;
        }
        else if ( StringBeginsWith ( strObject, "user." ) )
        {
            szObjectAfterDot += 5;
            GroupObjectType = CAccessControlListGroupObject::OBJECT_TYPE_USER;
        }
        else
        {
            // Invalid group type
            lua_pushboolean ( luaVM, false );
            return 1;
        }
        if ( pGroup->FindObjectMatch ( szObjectAfterDot, GroupObjectType ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}