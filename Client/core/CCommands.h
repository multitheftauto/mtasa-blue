/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CCommands.h
 *  PURPOSE:     Header file for dynamic command manager
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <core/CCommandsInterface.h>
#include <string>
#include "CSingleton.h"

class CCommands : public CCommandsInterface, public CSingleton<CCommands>
{
public:
    CCommands();
    ~CCommands();

    void Add(const char* szCommand, const char* szDescription, PFNCOMMANDHANDLER pfnHandler, bool bModCommand = false, bool bAllowScriptedBind = false);

    unsigned int Count();
    bool         Exists(const char* szCommand);

    bool Execute(const char* szCommandLine);
    bool Execute(const char* szCommand, const char* szParameters, bool bHandleRemotely = false, bool bIsScriptedBind = false);

    void Delete(const char* szCommand);
    void DeleteAll();

    void                     SetExecuteHandler(pfnExecuteCommandHandler pfnHandler) { m_pfnExecuteHandler = pfnHandler; }
    pfnExecuteCommandHandler GetExecuteHandler() { return m_pfnExecuteHandler; }

    tagCOMMANDENTRY* Get(const char* szCommand, bool bCheckIfMod = false, bool bModCommand = false);

    std::list<COMMANDENTRY*>::iterator IterBegin() { return m_CommandList.begin(); }
    std::list<COMMANDENTRY*>::iterator IterEnd() { return m_CommandList.end(); }

private:
    void ExecuteHandler(PFNCOMMAND pfnHandler, const char* szParameters);

    std::list<COMMANDENTRY*> m_CommandList;
    CElapsedTime             m_FpsLimitTimer;

    pfnExecuteCommandHandler m_pfnExecuteHandler;
};
