#include "VM/src/lstate.h"
#include "luauex.h"

LUA_API int lua_nresults(lua_State* L)
{
	if (L->ci)
		return L->ci->nresults;

	return -1;
}

LUA_API int lua_getstackgap(lua_State* L)
{
	const int gap1 = ( (char *)L->stack_last - (char *)L->top ) / (int)sizeof(TValue);
	const int gap2 = L->ci->top - L->top;

	return gap1 < gap2 ? gap1 : gap2;
}

LUA_API void lua_addtotalbytes(lua_State *L, int n)
{
	// We cannot just add a bytes number. See lstate.cpp line 99.
	// TODO: Find the solution
	//global_State *g = L->global;
    //g->totalbytes += n;
}