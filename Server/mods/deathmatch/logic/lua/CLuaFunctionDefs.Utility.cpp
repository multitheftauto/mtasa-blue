/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefs.Utility.cpp
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Kent Simon <>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*               Ed Lyons <>
*               Oliver Brown <>
*               Jax <>
*               Chris McArthur <>
*               Kevin Whiteside <>
*               lil_Toady <>
*               Alberto Alonso <rydencillo@gmail.com>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

int CLuaFunctionDefs::GetTok ( lua_State* luaVM )
{
    SString strInput = "";
    unsigned int uiToken = 0;
    unsigned int uiDelimiter = 0;
    SString strDelimiter;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strInput );
    argStream.ReadNumber ( uiToken );

    if ( argStream.NextIsNumber () )
    {
        argStream.ReadNumber ( uiDelimiter );
        wchar_t wUNICODE[2] = { uiDelimiter, '\0' };
        strDelimiter = UTF16ToMbUTF8 ( wUNICODE );
    }
    else  // It's already a string
        argStream.ReadString ( strDelimiter );

    if ( !argStream.HasErrors () )
    {
        if ( uiToken > 0 && uiToken < 1024 )
        {
            unsigned int uiCount = 1;
            char* szText = new char[strInput.length () + 1];
            strcpy ( szText, strInput );
            char* szToken = strtok ( szText, strDelimiter );

            // We're looking for the first part?
            if ( uiToken != 1 )
            {
                // strtok count number of times
                do
                {
                    uiCount++;
                    szToken = strtok ( NULL, strDelimiter );
                } while ( uiCount != uiToken );
            }

            // Found it?
            if ( szToken )
            {
                // Return it
                lua_pushstring ( luaVM, szToken );
                delete[] szText;
                return 1;
            }

            // Delete the text
            delete[] szText;
        }
        else
            m_pScriptDebugging->LogWarning ( luaVM, "Token parameter sent to split must be greater than 0 and smaller than 1024" );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}
