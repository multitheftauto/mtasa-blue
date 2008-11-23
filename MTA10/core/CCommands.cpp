/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/CCommands.cpp
*  PURPOSE:		Management for dynamically added commands
*  DEVELOPERS:	Christian Myhre Lundheim <>
*				Derek Abdine <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

template<> CCommands * CSingleton< CCommands >::m_pSingleton = NULL;

CCommands::CCommands ( void )
{
    WriteDebugEvent ( "CCommands::CCommands" );
    m_pfnExecuteHandler = NULL;
}


CCommands::~CCommands ( void )
{
    WriteDebugEvent ( "CCommands::~CCommands" );
    DeleteAll ();
}


void CCommands::Add ( const char* szCommand, const char* szDescription, PFNCOMMANDHANDLER pfnHandler, bool bModCommand )
{
    COMMANDENTRY* pCommand = new COMMANDENTRY;

    // Copy the command name and description to the new command entry.
    strncpy ( pCommand->szCommandName, szCommand,
              GET_LENGTH_BOUND ( MAX_COMMAND_NAME_LENGTH, strlen ( szCommand ) + 1 ) );

	strncpy ( pCommand->szDescription, szDescription,
			  GET_LENGTH_BOUND ( MAX_COMMAND_DESCRIPTION_LENGTH, strlen ( szDescription ) + 1 ) );

    // Set the command.
    pCommand->pfnCmdFunc = pfnHandler;

    // Set the mod bool
    pCommand->bModCommand = bModCommand;

    // Set the enabled bool
    pCommand->bEnabled = true;

    // Insert it into the list.
    m_CommandList.push_back ( pCommand );
}


unsigned int CCommands::Count ( void )
{
    return static_cast < unsigned int > ( m_CommandList.size () );
}


bool CCommands::Exists ( const char* szCommand )
{
    return Get ( szCommand ) != NULL;
}


bool CCommands::Execute ( const char* szCommandLine )
{
    // Parse out the command name and command line
    string strCmd;
    string strCmdLine;
    CLocalGUI::GetSingleton ().GetConsole () ->GetCommandInfo ( szCommandLine, strCmd, strCmdLine );

    // Try to execute it
    return Execute ( strCmd.c_str (), strCmdLine.c_str ());
}


bool CCommands::Execute ( const char* szCommand, const char* szParameters, bool bHandleRemotely )
{
    // HACK: if its a 'chatboxsay' command, use the next paramater
    // Is the command "say" and the arguments start with /? (command comes from the chatbox)
    if ( !stricmp ( szCommand, "chatboxsay" ) )
    {
        if ( szParameters )
        {
            // His line starts with '/'?
            if ( *szParameters == '/' )
            {
                // Copy the characters after the slash to the 0 terminator to a seperate buffer
                char szBuffer [256];
                strncpy ( szBuffer, szParameters + 1, 256 );
                szBuffer [ 255 ] = 0;

                // Split it into command and arguments
                szCommand = strtok ( szBuffer, " " );
                szParameters = strtok ( NULL, "\0" );
                if ( szCommand == 0 )
                {
                    return false;
                }
            }
        }
        else
            return false;
    }

    // Grab the command
    tagCOMMANDENTRY* pEntry = Get ( szCommand );
    if ( pEntry )
    {
        // If its a core command, or if its enabled
        if ( !pEntry->bModCommand || pEntry->bEnabled )
        {
            // Execute it
            ExecuteHandler ( pEntry->pfnCmdFunc, szParameters );
        }
    }

    if ( m_pfnExecuteHandler )
    {
        m_pfnExecuteHandler ( szCommand, szParameters, bHandleRemotely, ( pEntry != NULL ) );
    }

    return ( pEntry != NULL );
}


void CCommands::Delete ( const char* szCommand )
{
    // Find the entry we're looking for
    list < COMMANDENTRY* > ::iterator iter = m_CommandList.begin ();
    for ( ; iter != m_CommandList.end () ; iter++ )
    {
        // Check to see if this is the variable
        if ( stricmp ( szCommand, (*iter)->szCommandName ) == 0 ) 
        { 
            // It is, so remove it
            delete *iter;
            m_CommandList.erase ( iter );

			// Restart iterating, unless the list is empty
			if ( m_CommandList.empty() ) return;
			iter = m_CommandList.begin ();
        }    
	}
}


void CCommands::DeleteAll ( void )
{
    // Delete all the commands
    list < COMMANDENTRY* > ::iterator iter = m_CommandList.begin ();
    for ( ; iter != m_CommandList.end () ; iter++ )
    {
        delete *iter;
    }
    m_CommandList.clear ();

    // Re-register our commands
    CCore::GetSingleton ().RegisterCommands ();
}


tagCOMMANDENTRY* CCommands::Get ( const char* szCommand, bool bCheckIfMod, bool bModCommand )
{
    // Find the entry we're looking for
    list < COMMANDENTRY* > ::iterator iter = m_CommandList.begin ();
    for ( ; iter != m_CommandList.end () ; iter++ )
    {
        // Check to see if this is the variable
        if ( stricmp( szCommand, (*iter)->szCommandName ) == 0 ) 
        {
            if ( !bCheckIfMod || ( bModCommand == (*iter)->bModCommand ) )
            {
                return *iter;
            }
        }    
	}

    // Couldn't find it
    return NULL;
}


void CCommands::ExecuteHandler ( PFNCOMMAND pfnHandler, const char* szParameters )
{
    pfnHandler ( szParameters );
}
