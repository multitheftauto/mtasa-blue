/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaCallback.h
*  PURPOSE:     Lua callback handler
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

//
// For saving a Lua callback function and arguments until needed
//
class CLuaCallback
{
public:
    CLuaCallback ( CLuaMain* pLuaMain, CLuaFunctionRef iLuaFunction, const CLuaArguments& Args )
        : m_pLuaMain ( pLuaMain )
        , m_iLuaFunction ( iLuaFunction )
        , m_Arguments ( Args )
    {
    }

    void Call ( void )
    {
        m_Arguments.Call ( m_pLuaMain, m_iLuaFunction );
    }

protected:
    CLuaMain*       m_pLuaMain;
    CLuaFunctionRef m_iLuaFunction;
    CLuaArguments   m_Arguments;
};
