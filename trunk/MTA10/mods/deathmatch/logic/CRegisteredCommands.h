/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CRegisteredCommands.h
*  PURPOSE:     Header file for registered commands class
*  DEVELOPERS:  Kevin Whiteside <kevuwk@gmail.com>
*				Derek Abdine <...>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CREGISTEREDCOMMANDS_H
#define __CREGISTEREDCOMMANDS_H

#include <list>

using namespace std;

#define MAX_REGISTERED_COMMAND_LENGTH 64
#define MAX_REGISTERED_COMMANDHANDLER_LENGTH 64

class CRegisteredCommands
{
    struct SCommand
    {
        class CLuaMain* pLuaMain;
        char szKey [MAX_REGISTERED_COMMAND_LENGTH + 1];
        char szHandler [MAX_REGISTERED_COMMANDHANDLER_LENGTH + 1];
    };

public:
    inline              ~CRegisteredCommands            ( void )                { ClearCommands (); };

    bool                AddCommand                      ( class CLuaMain* pLuaMain, const char* szKey, const char* szHandler );
    bool                RemoveCommand                   ( class CLuaMain* pLuaMain, const char* szKey );
    void                ClearCommands                   ( void );
    void                CleanUpForVM                    ( class CLuaMain* pLuaMain );

    bool                CommandExists                   ( const char* szKey, class CLuaMain* pLuaMain = NULL );

    bool                ProcessCommand                  ( const char* szKey, const char* szArguments, class CClient* pClient );

private:
    SCommand*           GetCommand                      ( const char* szKey, class CLuaMain* pLuaMain = NULL );
    void                CallCommandHandler              ( class CLuaMain* pLuaMain, const char* szHandler, const char* szKey, const char* szArguments, class CClient* pClient );

    list < SCommand* >  m_Commands;
};

#endif
