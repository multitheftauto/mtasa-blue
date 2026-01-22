/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once
#include <stdexcept>
/*
 * Simple utility to verify the Lua stack.
 * The general idea is to simply write
 *   LUA_STACK_EXPECT(N);
 * at the beginning of a function, where N
 * is the expected amount of change in the Lua
 * stack size.
 *
 * As an example: A __index metamethod receives
 * two parameters on the stack and returns a
 * single value. Therefor each __index
 * implementation should decrease the Lua stack
 * size by one. Therefore write:
 *   LUA_STACK_EXPECT(-1);
 */
namespace lua
{
    struct CLuaStackChecker
    {
        lua_State* L;
        int        m_iTop = 0;
        int        m_iExpected = 0;
        CLuaStackChecker(lua_State* L, int expected) : L(L)
        {
            m_iTop = lua_gettop(L);
            m_iExpected = expected;
        }

        ~CLuaStackChecker() noexcept(false)
        {
            int iNewtop = lua_gettop(L);
            int iChange = iNewtop - m_iTop;
            if (iChange != m_iExpected)
            {
                printf("Lua Stack Error: top should be %d is %d (change should be %d is %d)\n", m_iTop + m_iExpected, iNewtop, m_iExpected, iChange);
                throw std::runtime_error("Lua Stack Error");
            }
        }
    };
}            // namespace lua

#ifdef MTA_DEBUG
#define LUA_STACK_EXPECT(i) lua::CLuaStackChecker invalidHiddenName(L, i)
#else
#define LUA_STACK_EXPECT(i)
#endif
