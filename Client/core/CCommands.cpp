/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CCommands.cpp
 *  PURPOSE:     Management for dynamically added commands
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

using namespace std;

void TrimWhiteSpace(std::string& str)
{
    unsigned long k = str.find_first_not_of(" \t");
    unsigned long l = str.find_last_not_of(" \t");

    if ((k == std::string::npos) || (l == std::string::npos))
        str = "";
    else
        str = str.substr(k, l - k + 1);
}

template <>
CCommands* CSingleton<CCommands>::m_pSingleton = NULL;

CCommands::CCommands()
{
    WriteDebugEvent("CCommands::CCommands");
    m_pfnExecuteHandler = NULL;
}

CCommands::~CCommands()
{
    WriteDebugEvent("CCommands::~CCommands");
    DeleteAll();
}

void CCommands::Add(const char* szCommand, const char* szDescription, PFNCOMMANDHANDLER pfnHandler, bool bModCommand, bool bAllowScriptedBind)
{
    COMMANDENTRY* pCommand = new COMMANDENTRY;

    // Copy the command name and description to the new command entry.
    STRNCPY(pCommand->szCommandName, szCommand, MAX_COMMAND_NAME_LENGTH);
    STRNCPY(pCommand->szDescription, szDescription, MAX_COMMAND_DESCRIPTION_LENGTH);

    // Set the command.
    pCommand->pfnCmdFunc = pfnHandler;

    // Set the mod bool
    pCommand->bModCommand = bModCommand;
    pCommand->bAllowScriptedBind = bAllowScriptedBind;

    // Set the enabled bool
    pCommand->bEnabled = true;

    // Insert it into the list.
    m_CommandList.push_back(pCommand);
}

unsigned int CCommands::Count()
{
    return static_cast<unsigned int>(m_CommandList.size());
}

bool CCommands::Exists(const char* szCommand)
{
    return Get(szCommand) != NULL;
}

bool CCommands::Execute(const char* szCommandLine)
{
    // Parse out the command name and command line
    string strCmd;
    string strCmdLine;
    CLocalGUI::GetSingleton().GetConsole()->GetCommandInfo(szCommandLine, strCmd, strCmdLine);

    return Execute(strCmd.c_str(), strCmdLine.c_str());
}

bool CCommands::Execute(const char* command, const char* parametersIn, bool handleRemotely, bool isScriptedBind)
{
    // Copy szParametersIn so the contents can be changed
    std::string parameters(parametersIn ? parametersIn : "");

    // HACK: if its a 'chatboxsay' command, use the next parameter
    // Is the command "say" and the arguments start with /? (command comes from the chatbox)
    if (!handleRemotely && !stricmp(command, "chatboxsay"))
    {
        if (!parameters.empty())
        {
            // His line starts with '/'?
            if (parameters[0] == '/')
            {
                // Copy the characters after the slash to the 0 terminator to a seperate buffer
                std::array<char, 256> buffer = {};
                std::strncpy(buffer.data(), parameters.c_str() + 1, buffer.size() - 1);
                buffer.back() = '\0';

                // Split it into command and arguments
                command = std::strtok(buffer.data(), " ");
                if (!command)
                    return false;

                if (char* newParameters = std::strtok(nullptr, "\0"))
                {
                    parameters = std::string(newParameters);
                }
                else
                {
                    parameters.clear();
                }
            }
        }
        else
            return false;
    }

    // Grab the command
    const tagCOMMANDENTRY* entry = Get(command);
    bool                   wasHandled = false;

    // If its a core command, or if its enabled
    if (entry && (!entry->bModCommand || entry->bEnabled))
    {
        // Execute it
        if (!isScriptedBind || entry->bAllowScriptedBind)
            ExecuteHandler(entry->pfnCmdFunc, parameters.c_str());

        wasHandled = true;
    }

    // Recompose the original command text
    std::string val = command;
    val += " ";
    val += parameters;

    // Is it a cvar? (syntax: cvar[ = value])
    if (!wasHandled)
    {
        // Check to see if '=' exists
        std::size_t nOpIndex = val.find('=');
        std::string key = val.substr(0, nOpIndex);

        // Check to see if ' =' exists
        if (val.find(" =") != std::string::npos)
        {
            key = val.substr(0, nOpIndex - 1);
        }

        TrimWhiteSpace(key);

        // Handle the cvar if it exists
        if (CClientVariables::GetSingleton().Exists(key) && !isScriptedBind)
        {
            std::stringstream ss;

            // Determine whether this is an atomic get or set query
            if (nOpIndex != std::string::npos)
            {
                // (set) some_cvar=value
                val = val.substr(nOpIndex + 1);
                TrimWhiteSpace(val);
                CVARS_SET(key, val);

                // HACK: recalculate frame rate limit on cvar change
                if (key == "fps_limit" && m_FpsLimitTimer.Get() >= 500 && CCore::GetSingleton().IsConnected())
                {
                    CCore::GetSingleton().RecalculateFrameRateLimit(-1, true);
                    m_FpsLimitTimer.Reset();
                }
            }
            else
            {
                // (get) some_cvar
                CVARS_GET(key, val);
            }
            ss << key << " = " << val;
            val = ss.str();
            CCore::GetSingleton().GetConsole()->Print(val);
            return true;
        }
    }

    // HACK: if its a 'nick' command, save it here
    bool isNickCommand = !stricmp(command, "nick");
    if (!wasHandled && isNickCommand && !parameters.empty() && !isScriptedBind)
    {
        if (CCore::GetSingleton().IsValidNick(parameters.c_str()))
        {
            CVARS_SET("nick", parameters);

            if (!CCore::GetSingleton().IsConnected())
            {
                CCore::GetSingleton().GetConsole()->Print(std::format("nick: You are now known as {}", parameters));
            }
        }
        else if (!CCore::GetSingleton().IsConnected())
        {
            CCore::GetSingleton().GetConsole()->Print("nick: Chosen nickname contains illegal characters");
        }
    }

    // Try to execute the handler
    if (m_pfnExecuteHandler)
    {
        bool bAllowScriptedBind = (!entry || entry->bAllowScriptedBind);
        if (m_pfnExecuteHandler(command, parameters.c_str(), handleRemotely, wasHandled, isScriptedBind, bAllowScriptedBind))
            return true;
    }

    if (wasHandled)
        return true;

    // Unknown command
    val = _("Unknown command or cvar: ") + command;
    if (!isScriptedBind && !isNickCommand && !entry)
        CCore::GetSingleton().GetConsole()->Print(val);

    return false;
}

void CCommands::Delete(const char* szCommand)
{
    // Find the entry we're looking for
    list<COMMANDENTRY*>::iterator iter = m_CommandList.begin();
    while (iter != m_CommandList.end())
    {
        // Check to see if this is the variable
        if (stricmp(szCommand, (*iter)->szCommandName) == 0)
        {
            // It is, so remove it
            delete *iter;
            iter = m_CommandList.erase(iter);
        }
        else
            ++iter;
    }
}

void CCommands::DeleteAll()
{
    // Delete all the commands
    list<COMMANDENTRY*>::iterator iter = m_CommandList.begin();
    for (; iter != m_CommandList.end(); iter++)
    {
        delete *iter;
    }
    m_CommandList.clear();

    // Re-register our commands
    CCore::GetSingleton().RegisterCommands();
}

tagCOMMANDENTRY* CCommands::Get(const char* szCommand, bool bCheckIfMod, bool bModCommand)
{
    // Find the entry we're looking for
    list<COMMANDENTRY*>::iterator iter = m_CommandList.begin();
    for (; iter != m_CommandList.end(); iter++)
    {
        // Check to see if this is the variable
        if (stricmp(szCommand, (*iter)->szCommandName) == 0)
        {
            if (!bCheckIfMod || (bModCommand == (*iter)->bModCommand))
            {
                return *iter;
            }
        }
    }

    // Couldn't find it
    return NULL;
}

void CCommands::ExecuteHandler(PFNCOMMAND pfnHandler, const char* szParameters)
{
    pfnHandler(szParameters);
}
