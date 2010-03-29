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

#ifndef __CLUAACLDEFS_H
#define __CLUAACLDEFS_H

#include "CLuaDefs.h"

class CLuaACLDefs: public CLuaDefs
{
public:
    static void     LoadFunctions                       ( void );

    static int      aclReload                           ( lua_State* luaVM );
    static int      aclSave                             ( lua_State* luaVM );

    static int      aclCreate                           ( lua_State* luaVM );
    static int      aclDestroy                          ( lua_State* luaVM );

    static int      aclGet                              ( lua_State* luaVM );
    static int      aclList                             ( lua_State* luaVM );

    static int      aclGetName                          ( lua_State* luaVM );
    static int      aclGetRight                         ( lua_State* luaVM );
    static int      aclSetRight                         ( lua_State* luaVM );
    static int      aclListRights                       ( lua_State* luaVM );
    static int      aclRemoveRight                      ( lua_State* luaVM );

    static int      aclCreateGroup                      ( lua_State* luaVM );
    static int      aclDestroyGroup                     ( lua_State* luaVM );

    static int      aclGetGroup                         ( lua_State* luaVM );
    static int      aclGroupList                        ( lua_State* luaVM );

    static int      aclGroupGetName                     ( lua_State* luaVM );
    static int      aclGroupAddACL                      ( lua_State* luaVM );
    static int      aclGroupListACL                     ( lua_State* luaVM );
    static int      aclGroupRemoveACL                   ( lua_State* luaVM );
    static int      aclGroupAddObject                   ( lua_State* luaVM );
    static int      aclGroupListObjects                 ( lua_State* luaVM );
    static int      aclGroupRemoveObject                ( lua_State* luaVM );

    static int      isObjectInACLGroup                  ( lua_State* luaVM );
    static int      hasObjectPermissionTo               ( lua_State* luaVM );
};

#endif
