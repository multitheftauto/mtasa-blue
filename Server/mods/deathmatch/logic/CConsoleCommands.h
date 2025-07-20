/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CConsoleCommands.h
 *  PURPOSE:     Server console command definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CClient.h"

class CConsoleCommands
{
public:
    static bool Update(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool StartResource(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool StopResource(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool StopAllResources(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool RestartResource(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool RefreshResources(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool RefreshAllResources(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool ListResources(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool ResourceInfo(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool UpgradeResources(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool CheckResources(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);

    static bool Say(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool TeamSay(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool Msg(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool Me(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool Nick(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);

    static bool LogIn(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool LogOut(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool ChgMyPass(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);

    static bool AddAccount(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool DelAccount(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool ChgPass(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool Shutdown(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);

    static bool AExec(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);

    static bool WhoIs(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);

    static bool DebugScript(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);

    static bool Help(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);

    static bool LoadModule(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool UnloadModule(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool ReloadModule(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);

    static bool Ver(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool Ase(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool OpenPortsTest(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool SetDbLogLevel(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);

    static bool ReloadBans(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);

    static bool AclRequest(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool AuthorizeSerial(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool ReloadAcl(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool DebugJoinFlood(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool DebugUpTime(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
    static bool FakeLag(class CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient);
};
