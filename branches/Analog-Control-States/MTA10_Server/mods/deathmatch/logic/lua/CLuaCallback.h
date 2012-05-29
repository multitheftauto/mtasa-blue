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
        if ( m_pLuaMain )
            m_Arguments.Call ( m_pLuaMain, m_iLuaFunction );
    }

    void OnLuaMainDestroy ( CLuaMain* pLuaMain )
    {
        if ( pLuaMain == m_pLuaMain )
        {
            m_pLuaMain = NULL;
            m_iLuaFunction = CLuaFunctionRef ();
            m_Arguments = CLuaArguments ();
        }
    }

protected:
    CLuaMain*       m_pLuaMain;
    CLuaFunctionRef m_iLuaFunction;
    CLuaArguments   m_Arguments;
};


//
// For managing Lua callbacks
//
class CLuaCallbackManager
{
public:

    CLuaCallback* CreateCallback ( CLuaMain* pLuaMain, CLuaFunctionRef iLuaFunction, const CLuaArguments& Args )
    {
        m_CallbackList.push_back ( new CLuaCallback ( pLuaMain, iLuaFunction, Args ) );
        return m_CallbackList.back ();
    }

    void DestroyCallback ( CLuaCallback* pCallback )
    {
        ListRemove ( m_CallbackList, pCallback );
        delete pCallback;
    }

    void OnLuaMainDestroy ( CLuaMain* pLuaMain )
    {
        for ( uint i = 0 ; i < m_CallbackList.size () ; i++ )
            m_CallbackList[i]->OnLuaMainDestroy ( pLuaMain );
    }

protected:
    std::vector < CLuaCallback* >   m_CallbackList;
};
