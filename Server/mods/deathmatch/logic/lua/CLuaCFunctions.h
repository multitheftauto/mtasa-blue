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

class CLuaCFunction
{
public:
                                CLuaCFunction       ( const char* szName, lua_CFunction f, bool bRestricted );

    lua_CFunction               GetAddress          ( void )                    { return m_Function; }

    const SString&              GetName             ( void )                    { return m_strName; }
    void                        SetName             ( std::string& strName )    { m_strName = strName; }

    bool                        IsRestricted        ( void )                    { return m_bRestricted; }

private:
    lua_CFunction               m_Function;
    SString                     m_strName;
    bool                        m_bRestricted;

};

class CLuaCFunctions
{
public:
                                CLuaCFunctions              ();
                                ~CLuaCFunctions             ();

    static CLuaCFunction*       AddFunction                 ( const char* szName, lua_CFunction f, bool bRestricted = false );
    static void                 RemoveFunction              ( const SString& strName );
    static CLuaCFunction*       GetFunction                 ( lua_CFunction f );
    static CLuaCFunction*       GetFunction                 ( const char* szName );
    static bool                 IsNotFunction               ( lua_CFunction f );

    static void                 RegisterFunctionsWithVM     ( lua_State* luaVM );

    static void                 RemoveAllFunctions          ( void );

private:
    static CFastHashMap < lua_CFunction, CLuaCFunction* >               ms_Functions;
    static CFastHashMap < SString, CLuaCFunction* >                     ms_FunctionsByName;
    static void*                                                        ms_pFunctionPtrLow;
    static void*                                                        ms_pFunctionPtrHigh;
};

#endif
