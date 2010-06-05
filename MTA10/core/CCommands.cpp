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

using namespace std;

void TrimWhiteSpace ( std::string & str )  
{  
    unsigned long k = str.find_first_not_of(" \t");
    unsigned long l = str.find_last_not_of (" \t");

    if ( (k == std::string::npos) || (l == std::string::npos) )
        str = "";
    else
        str = str.substr ( k, l-k+1 );
}

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

    return Execute ( strCmd.c_str (), strCmdLine.c_str () );
}


bool CCommands::Execute ( const char* szCommand, const char* szParameters, bool bHandleRemotely )
{
    // HACK: if its a 'chatboxsay' command, use the next parameter
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
            return true;
        }
    }

    // Is it a cvar? (syntax: cvar[ = value])
    std::string val = std::string ( szCommand ) + " " + std::string ( szParameters ? szParameters : "" );
    unsigned int nOpIndex = val.find ( '=' );
    std::string key = val.substr ( 0, nOpIndex );
    if (val.find(" = ") != std::string::npos) {
        key = val.substr ( 0, nOpIndex-1 );
    }
    if ( CClientVariables::GetSingleton ().Exists ( key ) ) {
        std::stringstream ss;

        // Determine whether this is an atomic get or set query
        if ( nOpIndex != std::string::npos ) {
            // (set) some_cvar=value
            val = val.substr ( nOpIndex + 1 );
            TrimWhiteSpace ( val );
            CVARS_SET ( key, val );
        } else {
            // (get) some_cvar
            CVARS_GET ( key, val );
        }
        ss << key << " = " << val;
        val = ss.str ();
        CCore::GetSingleton ().GetConsole ()->Print ( val.c_str () );
        return true;
    }

    // Try to execute the handler
    if ( m_pfnExecuteHandler )
    {
        if ( m_pfnExecuteHandler ( szCommand, szParameters, bHandleRemotely, ( pEntry != NULL ) ) )
            return true;
    }

    // Unknown command
    val = std::string ( "Unknown command or cvar: " ) + szCommand;
    CCore::GetSingleton ().GetConsole ()->Print ( val.c_str () );
    return false;
}


void CCommands::Delete ( const char* szCommand )
{
    // Find the entry we're looking for
    list < COMMANDENTRY* > ::iterator iter = m_CommandList.begin ();
    while ( iter != m_CommandList.end () )
    {
        // Check to see if this is the variable
        if ( stricmp ( szCommand, (*iter)->szCommandName ) == 0 ) 
        { 
            // It is, so remove it
            delete *iter;
            iter = m_CommandList.erase ( iter );
        }
		else
			++iter;
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
