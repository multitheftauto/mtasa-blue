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
#include <string>

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
    static CLuaCFunction*       AddFunction                 ( const char* szName, lua_CFunction f, bool bRestricted = false );
    static CLuaCFunction*       GetFunction                 ( lua_CFunction f );
    static CLuaCFunction*       GetFunction                 ( const char* szName );

    static void                 RegisterFunctionsWithVM     ( lua_State* luaVM );

    static void                 RemoveAllFunctions          ( void );

private:
    static stdext::hash_map < lua_CFunction, CLuaCFunction* > ms_Functions;
    static stdext::hash_map < std::string, CLuaCFunction* > ms_FunctionsByName;
};

#endif
