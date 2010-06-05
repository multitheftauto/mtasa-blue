/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/CCommands.h
*  PURPOSE:		Header file for dynamic command manager
*  DEVELOPERS:	Christian Myhre Lundheim <>
*				Derek Abdine <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCOMMANDS_H
#define __CCOMMANDS_H

#include <core/CCommandsInterface.h>
#include <string>
#include "CSingleton.h"

class CCommands : public CCommandsInterface, public CSingleton < CCommands >
{
public:
                        CCommands                   ( void );
                        ~CCommands                  ( void );

    void                Add                         ( const char* szCommand, const char* szDescription, PFNCOMMANDHANDLER pfnHandler, bool bModCommand = false );

    unsigned int        Count                       ( void );
    bool                Exists                      ( const char* szCommand );

    bool                Execute                     ( const char* szCommandLine );
    bool                Execute                     ( const char* szCommand, const char* szParameters, bool bHandleRemotely = false );

    void                Delete                      ( const char* szCommand );
    void                DeleteAll                   ( void );

    inline void         SetExecuteHandler           ( pfnExecuteCommandHandler pfnHandler )         { m_pfnExecuteHandler = pfnHandler; };

    tagCOMMANDENTRY*    Get                         ( const char* szCommand, bool bCheckIfMod = false, bool bModCommand = false );

    std::list < COMMANDENTRY* > ::iterator IterBegin( void )                                        { return m_CommandList.begin (); }
    std::list < COMMANDENTRY* > ::iterator IterEnd  ( void )                                        { return m_CommandList.end (); }
private:    

    void                ExecuteHandler              ( PFNCOMMAND pfnHandler, const char* szParameters );

    std::list < COMMANDENTRY* >     m_CommandList;

    pfnExecuteCommandHandler        m_pfnExecuteHandler;
};

#endif