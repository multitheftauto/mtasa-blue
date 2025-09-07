/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CCommands.cpp
 *  PURPOSE:     Management for dynamically added commands
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
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
    string strCmd;
    string strCmdLine;
    CLocalGUI::GetSingleton().GetConsole()->GetCommandInfo(szCommandLine, strCmd, strCmdLine);

    return Execute(strCmd.c_str(), strCmdLine.c_str());
}

bool CCommands::Execute(const char* szCommand, const char* szParametersIn, bool bHandleRemotely, bool bIsScriptedBind)
{
    std::string command;
    std::string parameters;
    if (!ParseCommand(szCommand, szParametersIn, bIsScriptedBind, command, parameters))
        return false;

    if (ExecuteBuiltInCommand(command.c_str(), parameters.c_str(), bIsScriptedBind))
        return true;

    if (ExecuteCVarCommand(command + " " + parameters, bIsScriptedBind))
        return true;

    // Handle nick command
    // TODO: Make a proper builtin command entry for this
    HandleNickCommand(command.c_str(), parameters.c_str(), bIsScriptedBind);

    // Try external handler
    if (ExecuteExternalHandler(command.c_str(), parameters.c_str(), bHandleRemotely, bIsScriptedBind))
        return true;

    // Unknown command
    if (!bIsScriptedBind)
    {
        std::string error = _("Unknown command or cvar: ") + command;
        CCore::GetSingleton().GetConsole()->Print(error.c_str());
    }
    return false;
}

bool CCommands::ParseCommand(const char* szCommand, const char* szParametersIn, bool bIsScriptedBind, std::string& outCommand, std::string& outParameters)
{
    outCommand = szCommand;
    outParameters = szParametersIn ? szParametersIn : "";

    // Handle chatboxsay command transformation
    if (!bIsScriptedBind && !stricmp(szCommand, "chatboxsay"))
    {
        if (!szParametersIn || *szParametersIn != '/')
            return false;

        std::string buffer = szParametersIn + 1;            // Skip the '/'
        size_t      spacePos = buffer.find(' ');

        if (spacePos == std::string::npos)
        {
            outCommand = buffer;
            outParameters = "";
        }
        else
        {
            outCommand = buffer.substr(0, spacePos);
            outParameters = buffer.substr(spacePos + 1);
        }

        if (outCommand.empty())
            return false;
    }

    return true;
}

bool CCommands::ExecuteBuiltInCommand(const char* szCommand, const char* szParameters, bool bIsScriptedBind)
{
    tagCOMMANDENTRY* pEntry = Get(szCommand);
    if (!pEntry)
        return false;

    // Check if command is enabled and allowed
    if (pEntry->bModCommand && !pEntry->bEnabled)
        return false;

    if (bIsScriptedBind && !pEntry->bAllowScriptedBind)
        return false;

    ExecuteHandler(pEntry->pfnCmdFunc, szParameters);
    return true;
}

bool CCommands::ExecuteCVarCommand(const std::string& fullCommand, bool bIsScriptedBind)
{
    if (bIsScriptedBind)
        return false;

    size_t equalPos = fullCommand.find('=');

    if (equalPos == std::string::npos)
    {
        return GetCVarValue(fullCommand);
    }
    else
    {
        return SetCVarValue(fullCommand, equalPos);
    }
}

bool CCommands::GetCVarValue(const std::string& key)
{
    std::string trimmedKey = key;
    TrimWhiteSpace(trimmedKey);

    if (!CClientVariables::GetSingleton().Exists(trimmedKey))
        return false;

    std::string value;
    CVARS_GET(trimmedKey, value);

    std::stringstream ss;
    ss << trimmedKey << " = " << value;
    CCore::GetSingleton().GetConsole()->Print(ss.str().c_str());
    return true;
}

bool CCommands::SetCVarValue(const std::string& fullCommand, size_t equalPos)
{
    std::string key = fullCommand.substr(0, equalPos);
    std::string value = fullCommand.substr(equalPos + 1);

    // Handle space before '='
    size_t spaceEqualPos = fullCommand.find(" =");
    if (spaceEqualPos != std::string::npos)
    {
        key = fullCommand.substr(0, spaceEqualPos);
        value = fullCommand.substr(spaceEqualPos + 2);
    }

    TrimWhiteSpace(key);
    TrimWhiteSpace(value);

    if (!CClientVariables::GetSingleton().Exists(key))
        return false;

    CVARS_SET(key, value);
    HandleSpecialCVars(key, value);

    std::stringstream ss;
    ss << key << " = " << value;
    CCore::GetSingleton().GetConsole()->Print(ss.str().c_str());
    return true;
}

void CCommands::HandleSpecialCVars(const std::string& key, const std::string& value)
{
    if (key == "fps_limit")
    {
        int fpsVal = 0;
        CVARS_GET("fps_limit", fpsVal);
        CCore::GetSingleton().GetFPSLimiter()->SetUserDefinedFPS(fpsVal);
    }

    if (key == "vsync")
    {
        bool vsyncVal = false;
        CVARS_GET("vsync", vsyncVal);
        if (vsyncVal)
        {
            // HACK: (pxd) Get refresh rate from Direct3D? Should this go elsewhere?
            D3DDISPLAYMODE DisplayMode;
            IDirect3D9*    pD3D9 = CProxyDirect3D9::StaticGetDirect3D();
            if (pD3D9 && pD3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &DisplayMode) == D3D_OK)            // Query current display mode
            {
                OutputDebugLine(SString("VSync enabled, setting fps_limit to display refresh rate %d Hz", DisplayMode.RefreshRate));
                CCore::GetSingleton().GetFPSLimiter()->SetDisplayRefreshRate(DisplayMode.RefreshRate);
            }
            else
            {
                OutputDebugLine("VSync enabled, but failed to get display refresh rate, defaulting to 60 Hz");
                CCore::GetSingleton().GetFPSLimiter()->SetDisplayRefreshRate(60);
            }
        }
        else
        {
            OutputDebugLine("VSync disabled, removing fps_limit display refresh rate");
            CCore::GetSingleton().GetFPSLimiter()->SetDisplayRefreshRate(0);
        }
    }
}

void CCommands::HandleNickCommand(const char* szCommand, const char* szParameters, bool bIsScriptedBind)
{
    if (stricmp(szCommand, "nick") != 0 || !szParameters || bIsScriptedBind)
        return;

    if (CCore::GetSingleton().IsValidNick(szParameters))
    {
        CVARS_SET("nick", std::string(szParameters));

        if (!CCore::GetSingleton().IsConnected())
        {
            CCore::GetSingleton().GetConsole()->Printf("nick: You are now known as %s", szParameters);
        }
    }
    else if (!CCore::GetSingleton().IsConnected())
    {
        CCore::GetSingleton().GetConsole()->Print("nick: Chosen nickname contains illegal characters");
    }
}

bool CCommands::ExecuteExternalHandler(const char* szCommand, const char* szParameters, bool bHandleRemotely, bool bIsScriptedBind)
{
    if (!m_pfnExecuteHandler)
        return false;

    tagCOMMANDENTRY* pEntry = Get(szCommand);
    bool             bAllowScriptedBind = (!pEntry || pEntry->bAllowScriptedBind);
    bool             wasHandled = (pEntry != nullptr);

    return m_pfnExecuteHandler(szCommand, szParameters, bHandleRemotely, wasHandled, bIsScriptedBind, bAllowScriptedBind);
}

void CCommands::Delete(const char* szCommand)
{
    list<COMMANDENTRY*>::iterator iter = m_CommandList.begin();
    while (iter != m_CommandList.end())
    {
        if (stricmp(szCommand, (*iter)->szCommandName) == 0)
        {
            delete *iter;
            iter = m_CommandList.erase(iter);
        }
        else
            ++iter;
    }
}

void CCommands::DeleteAll()
{
    list<COMMANDENTRY*>::iterator iter = m_CommandList.begin();
    for (; iter != m_CommandList.end(); iter++)
    {
        delete *iter;
    }
    m_CommandList.clear();

    CCore::GetSingleton().RegisterCommands();
}

tagCOMMANDENTRY* CCommands::Get(const char* szCommand, bool bCheckIfMod, bool bModCommand)
{
    list<COMMANDENTRY*>::iterator iter = m_CommandList.begin();
    for (; iter != m_CommandList.end(); iter++)
    {
        if (stricmp(szCommand, (*iter)->szCommandName) == 0)
        {
            if (!bCheckIfMod || (bModCommand == (*iter)->bModCommand))
            {
                return *iter;
            }
        }
    }

    return NULL;
}

void CCommands::ExecuteHandler(PFNCOMMAND pfnHandler, const char* szParameters)
{
    pfnHandler(szParameters);
}
