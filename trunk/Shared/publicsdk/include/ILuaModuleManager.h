/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        publicsdk/include/ILuaModuleManager.h
*  PURPOSE:     Lua dynamic module interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// INTERFACE for Lua dynamic modules

#ifndef __ILUAMODULEMANAGER_H
#define __ILUAMODULEMANAGER_H

#define MAX_INFO_LENGTH 128

extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

class ILuaModuleManager
{
public:
	virtual void             ErrorPrintf          ( const char* szFormat, ... ) = 0;
	virtual void             DebugPrintf          ( lua_State * luaVM, const char* szFormat, ... ) = 0;
	virtual void             Printf               ( const char* szFormat, ... ) = 0;

	virtual bool             RegisterFunction     ( lua_State * luaVM, const char *szFunctionName, lua_CFunction Func ) = 0;
    virtual bool             GetResourceName      ( lua_State * luaVM, std::string &strName ) = 0;
	virtual lua_State*       GetResourceFromName  ( const char* szResourceName ) = 0;
	virtual unsigned long    GetResourceMetaCRC   ( lua_State * luaVM ) = 0;
	virtual unsigned long    GetResourceFileCRC   ( lua_State * luaVM, const char* szFile ) = 0;
};

#endif
