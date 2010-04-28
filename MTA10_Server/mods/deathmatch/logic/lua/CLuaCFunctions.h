/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaCFunctions.h
*  PURPOSE:     Lua C-function extension class
*  DEVELOPERS:  Oliver Brown <>
*               Christian Myhre Lundheim <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CLuaCFunctions;

#ifndef __CLUACFUNCTIONS_H
#define __CLUACFUNCTIONS_H

#include "LuaCommon.h"

#ifndef WIN32
    // gcc's hash(), used in the google hash maps, does not support pointers by default.
    // Also gcc refuses to implicitly cast function pointers to void*... so we
    // need a special case.
    namespace __gnu_cxx
    {
        template <>
        struct hash < lua_CFunction >
        {
            size_t operator()( lua_CFunction const& pFunc ) const
            {
                return (size_t)pFunc;
            }
        };
    }
#endif

class CLuaCFunction
{
public:
                                CLuaCFunction       ( const char* szName, lua_CFunction f, bool bRestricted );

    lua_CFunction               GetAddress          ( void )                    { return m_Function; }

    const std::string&          GetName             ( void )                    { return m_strName; }
    void                        SetName             ( std::string& strName )    { m_strName = strName; }

    bool                        IsRestricted        ( void )                    { return m_bRestricted; }

private:
    lua_CFunction               m_Function;
    std::string                 m_strName;
    bool                        m_bRestricted;

};

class CLuaCFunctions
{
public:
                                CLuaCFunctions              ();
                                ~CLuaCFunctions             ();

    static CLuaCFunction*       AddFunction                 ( const char* szName, lua_CFunction f, bool bRestricted = false );
    static CLuaCFunction*       GetFunction                 ( lua_CFunction f );
    static CLuaCFunction*       GetFunction                 ( const char* szName );

    static void                 RegisterFunctionsWithVM     ( lua_State* luaVM );

    static void                 RemoveAllFunctions          ( void );

private:
    static google::dense_hash_map < lua_CFunction, CLuaCFunction* >     ms_Functions;
    static google::dense_hash_map < std::string, CLuaCFunction* >       ms_FunctionsByName;
    static bool                                                         ms_bMapsInitialized;
public:
    static void                 InitializeHashMaps          ( );
};

#endif
