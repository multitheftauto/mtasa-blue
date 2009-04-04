/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaCFunctions.h
*  PURPOSE:     Lua C-function extension class
*  DEVELOPERS:  Oliver Brown <>
*               Christian Myhre Lundheim <>
*               Alberto Alonso <>
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

    inline lua_CFunction        GetFunctionAddress  ( void )                    { return m_Function; };

    inline const std::string&   GetFunctionName     ( void )                    { return m_strName; };
    void                        SetFunctionName     ( std::string strName )     { m_strName = strName; };

    bool                        IsRestricted        ( void )                    { return m_bRestricted; };

private:
    lua_CFunction               m_Function;
    std::string                 m_strName;
    bool                        m_bRestricted;

};

class CLuaCFunctions
{
public:
    static CLuaCFunction*               AddFunction                 ( const char* szName, lua_CFunction f, bool bRestricted = false );

    static CLuaCFunction*               GetFunction                 ( const char* szName, lua_CFunction f );
    static bool                         GetFunctionName             ( lua_CFunction f, bool& bRestricted, std::string &strName );

    static bool                         IsRestricted                ( const char* szName );

    static void                         RegisterFunctionsWithVM     ( lua_State* luaVM );

    static void                         RemoveAllFunctions          ( void );

};

#endif
