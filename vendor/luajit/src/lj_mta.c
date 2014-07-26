
/*
** MTA Specific stuff written by Oli for pre C Function call hooking
*/	

#include "lua.h"
#include "luajit.h"

typedef struct lua_State lua_State;
lua_PreCallHook pPreCallHook = NULL;
void lua_registerPreCallHook ( lua_PreCallHook f )
{
    pPreCallHook = f;
}

lua_PostCallHook pPostCallHook = NULL;
void lua_registerPostCallHook ( lua_PostCallHook f )
{
    pPostCallHook = f;
}
static int ACLWrapper(lua_State *L, lua_CFunction f)
{
    if ( pPreCallHook != NULL && pPreCallHook ( f, L ) == 1 )
    {
        int iReturn = f(L);
		if ( pPostCallHook != NULL )
		{
			pPostCallHook ( f, L );
        }
		return iReturn;
    }
	return 0;
}

void lua_RegisterHook(lua_State* L)
{
    lua_pushlightuserdata(L, (void *)ACLWrapper);
    luaJIT_setmode(L, -1, LUAJIT_MODE_WRAPCFUNC|LUAJIT_MODE_ON);
    lua_pop(L, 1);
}
