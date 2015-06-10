/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaACLDefs.h
*  PURPOSE:     Lua ACL function definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaACLDefs: public CLuaDefs
{
public:
    static void     LoadFunctions ( void );
    static void     AddClass      ( lua_State* luaVM );

    LUA_DECLARE ( aclReload );
    LUA_DECLARE ( aclSave );

    LUA_DECLARE ( aclCreate );
    LUA_DECLARE ( aclDestroy );

    LUA_DECLARE ( aclGet );
    LUA_DECLARE ( aclList );

    LUA_DECLARE ( aclGetName );
    LUA_DECLARE ( aclGetRight );
    LUA_DECLARE ( aclSetRight );
    LUA_DECLARE ( aclListRights );
    LUA_DECLARE ( aclRemoveRight );

    LUA_DECLARE ( aclCreateGroup );
    LUA_DECLARE ( aclDestroyGroup );

    LUA_DECLARE ( aclGetGroup );
    LUA_DECLARE ( aclGroupList );

    LUA_DECLARE ( aclGroupGetName );
    LUA_DECLARE ( aclGroupAddACL );
    LUA_DECLARE ( aclGroupListACL );
    LUA_DECLARE ( aclGroupRemoveACL );
    LUA_DECLARE ( aclGroupAddObject );
    LUA_DECLARE ( aclGroupListObjects );
    LUA_DECLARE ( aclGroupRemoveObject );
    LUA_DECLARE_OOP ( isObjectInACLGroup );
    LUA_DECLARE ( hasObjectPermissionTo );
};
