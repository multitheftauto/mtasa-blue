/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaModuleManager.h
*  PURPOSE:     Lua module extension manager class
*  DEVELOPERS:  Cecill Etheredge <>
*               Kevin Whiteside <>
*               Alberto Alonso <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// IMPLEMENTATION of Lua dynamic modules

class CLuaModuleManager;

#ifndef __CLUAMODULEMANAGER_H
#define __CLUAMODULEMANAGER_H

#include "CLuaMain.h"
#include "ILuaModuleManager.h"
#include "CLuaManager.h"
#include "../CLogger.h"
#include "../CResource.h"

class CScriptDebugging;

typedef bool (*DefaultModuleFunc)	( void );
typedef void (*RegisterModuleFunc)	( lua_State * );
typedef bool (*InitModuleFunc)		( ILuaModuleManager *, char *, char *, float * );

struct FunctionInfo
{
	// module information
	char				szModuleName[MAX_INFO_LENGTH];
	char				szAuthor[MAX_INFO_LENGTH];
	float				fVersion;

	// module function pointers
	DefaultModuleFunc	ShutdownModule;
	DefaultModuleFunc	DoPulse;
	RegisterModuleFunc	RegisterFunctions;

	RegisterModuleFunc	ResourceStopping;
	RegisterModuleFunc	ResourceStopped;
};

class CLuaModuleManager : public ILuaModuleManager10
{
public:
							CLuaModuleManager		( CLuaManager* pLuaManager );
							~CLuaModuleManager		( void );

	// functions for external modules until DP2.3
	void					Printf					( const char * szFormat, ... );
	void					ErrorPrintf				( const char * szFormat, ... );
	void					DebugPrintf				( lua_State * luaVM, const char * szFormat, ... );
	bool					RegisterFunction		( lua_State * luaVM, const char *szFunctionName, lua_CFunction Func );
    bool                    GetResourceName         ( lua_State * luaVM, std::string &strName );
	unsigned long			GetResourceMetaCRC		( lua_State * luaVM );
	unsigned long			GetResourceFileCRC		( lua_State * luaVM, const char* szFile );

    // functions for external modules until 1.0
    unsigned long           GetVersion              ( );
    const char*             GetVersionString        ( );
    const char*             GetVersionName          ( );
    unsigned long           GetNetcodeVersion       ( );
    const char*             GetOperatingSystemName  ( );
    lua_State*              GetResourceFromName     ( const char* szResourceName );

	// functions for deathmatch
	void					_DoPulse				( void );
	bool					_LoadModule				( const char *szShortFileName, const char *szFileName, bool bLateLoad );
	void					_SetScriptDebugging		( CScriptDebugging* pScriptDebugging );
	void					_RegisterFunctions		( lua_State * luaVM );
	bool					_UnloadModule			( const char *szShortFileName );
	void					_ResourceStopping		( lua_State * luaVM );
	void					_ResourceStopped		( lua_State * luaVM );

private:
	vector < FunctionInfo > m_Functions;
	CScriptDebugging*		m_pScriptDebugging;
	CLuaManager*			m_pLuaManager;
};

class CLuaModule
{
public:
									CLuaModule			( const char* szName );
									~CLuaModule			( void );

	void							RegisterFunction	( char* szFunction );

	inline list<char *>::iterator	GetFunctionsBegin	( void )	{ return m_Functions.begin(); };
	inline list<char *>::iterator	GetFunctionsEnd		( void )	{ return m_Functions.end(); };

private:
	char*			m_szName;
	list < char* >	m_Functions;
};

#endif
