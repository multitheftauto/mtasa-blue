/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaDatabaseDefs.h
*  PURPOSE:     Lua function definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaDatabaseDefs : public CLuaDefs
{
public:
    static void     LoadFunctions ( void );
    static void     AddClass ( lua_State* luaVM );

    static void DbQueryCallback ( CDbJobData* pJobData, void* pContext );
    static void DbFreeCallback ( CDbJobData* pJobData, void* pContext );
    static void DbExecCallback ( CDbJobData* pJobData, void* pContext );

    LUA_DECLARE ( DbConnect );
    LUA_DECLARE ( DbExec );
    LUA_DECLARE_OOP ( DbQuery );
    LUA_DECLARE ( DbFree );
    LUA_DECLARE ( DbPoll );
    LUA_DECLARE ( DbPrepareString );

    LUA_DECLARE ( ExecuteSQLCreateTable );
    LUA_DECLARE ( ExecuteSQLDropTable );
    LUA_DECLARE ( ExecuteSQLDelete );
    LUA_DECLARE ( ExecuteSQLInsert );
    LUA_DECLARE ( ExecuteSQLSelect );
    LUA_DECLARE ( ExecuteSQLUpdate );
    LUA_DECLARE ( ExecuteSQLQuery );
};