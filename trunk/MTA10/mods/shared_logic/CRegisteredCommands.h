/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CRegisteredCommands.h
*  PURPOSE:     Registered commands class header
*  DEVELOPERS:  Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Alberto Alonso <rydencillo@gmail.com>
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
        int iLuaFunction;
        bool bCaseSensitive;
    };

public:
                        CRegisteredCommands             ( void );
                        ~CRegisteredCommands            ( void );

    bool                AddCommand                      ( class CLuaMain* pLuaMain, const char* szKey, int iLuaFunction, bool bCaseSensitive );
    bool                RemoveCommand                   ( class CLuaMain* pLuaMain, const char* szKey );
    void                ClearCommands                   ( void );
    void                CleanUpForVM                    ( class CLuaMain* pLuaMain );

    bool                CommandExists                   ( const char* szKey, class CLuaMain* pLuaMain = NULL );

    bool                ProcessCommand                  ( const char* szKey, const char* szArguments );

private:
    SCommand*           GetCommand                      ( const char* szKey, class CLuaMain* pLuaMain = NULL );
    void                CallCommandHandler              ( class CLuaMain* pLuaMain, int iLuaFunction, const char* szKey, const char* szArguments );

    void                TakeOutTheTrash                 ( void );

    list < SCommand* >  m_Commands;
    list < SCommand* >  m_TrashCan;
    bool                m_bIteratingList;
};

#endif
