/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/CClient.cpp
 *  PURPOSE:     Main client module class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#define ALLOC_STATS_MODULE_NAME "client"
#include "SharedUtil.hpp"
#include <core/CClientCommands.h>

CCoreInterface*         g_pCore = NULL;
CLocalizationInterface* g_pLocalization = NULL;
CGame*                  g_pGame = NULL;
CMultiplayer*           g_pMultiplayer = NULL;
CNet*                   g_pNet = NULL;
CClientGame*            g_pClientGame = NULL;

int CClient::ClientInitialize(const char* szArguments, CCoreInterface* pCore)
{
    // Supported core version?
    if (pCore->GetVersion() != MTACORE_20)
    {
        return 1;
    }

#if defined(MTA_DM_EXPIRE_DAYS)
    // Make public client test builds expire
    if (GetDaysUntilExpire() < -1)
    {
        MessageBox(NULL, _("This version has expired."), "MTA: San Andreas " MTA_DM_BUILDTAG_LONG + _E("CD64"), MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
        TerminateProcess(GetCurrentProcess(), 1);
    }
#endif

    // Init the global pointers to the interfaces
    g_pCore = pCore;
    g_pLocalization = pCore->GetLocalization();
    g_pGame = pCore->GetGame();
    g_pMultiplayer = pCore->GetMultiplayer();
    g_pNet = pCore->GetNetwork();

    // HACK
    pCore->SetOfflineMod(false);

    // HACK FOR CHATBOX NOT VISIBLE. WILL CAUSE SAVING CHATBOX STATE NOT TO WORK
    g_pCore->SetChatVisible(true, false);

    // Register our local commands
    g_pCore->GetCommands()->SetExecuteHandler(COMMAND_Executed);
    g_pCore->GetCommands()->Add("disconnect", _("disconnect from the game"), COMMAND_Disconnect);
    g_pCore->GetCommands()->Add("shownametags", _("shows the nametags"), COMMAND_ShowNametags);
    g_pCore->GetCommands()->Add("showchat", _("shows the chatbox"), COMMAND_ShowChat);
    g_pCore->GetCommands()->Add("shownetstat", _("shows the network statistics"), COMMAND_ShowNetstat);

    // Key commands (registered as 'mod commands', can be disabled)
    g_pCore->GetCommands()->Add("chatbox", _("open the chat input"), COMMAND_ChatBox, true, true);
    g_pCore->GetCommands()->Add("voiceptt", _("transmits voice to other players"), COMMAND_VoicePushToTalk, true, true);
    g_pCore->GetCommands()->Add("enter_passenger", _("enters a car as passenger"), COMMAND_EnterPassenger, true, true);
    g_pCore->GetCommands()->Add("radio_next", _("next radio channel"), COMMAND_RadioNext, true, true);
    g_pCore->GetCommands()->Add("radio_previous", _("previous radio channel"), COMMAND_RadioPrevious, true, true);
    g_pCore->GetCommands()->Add("radar", _("enables the player-map view"), COMMAND_PlayerMap, true, true);
    g_pCore->GetCommands()->Add("radar_zoom_in", _("zooms the player-map in"), COMMAND_PlayerMapZoomIn, true, true);
    g_pCore->GetCommands()->Add("radar_zoom_out", _("zooms the player-map out"), COMMAND_PlayerMapZoomOut, true, true);
    g_pCore->GetCommands()->Add("radar_move_north", _("moves the player-map north"), COMMAND_PlayerMapMoveNorth, true, true);
    g_pCore->GetCommands()->Add("radar_move_south", _("moves the player-map south"), COMMAND_PlayerMapMoveSouth, true, true);
    g_pCore->GetCommands()->Add("radar_move_east", _("moves the player-map east"), COMMAND_PlayerMapMoveEast, true, true);
    g_pCore->GetCommands()->Add("radar_move_west", _("moves the player-map west"), COMMAND_PlayerMapMoveWest, true, true);
    g_pCore->GetCommands()->Add("radar_attach", _("attaches the player-map"), COMMAND_PlayerMapAttach, true, true);
    g_pCore->GetCommands()->Add("radar_opacity_down", _("reduces player-map opacity"), COMMAND_PlayerMapOpacityDown, true, true);
    g_pCore->GetCommands()->Add("radar_opacity_up", _("increases player-map opacity"), COMMAND_PlayerMapOpacityUp, true, true);
    g_pCore->GetCommands()->Add("radar_help", _("toggles player-map help text"), COMMAND_PlayerMapHelp, true, true);
    g_pCore->GetCommands()->Add("msg_target", _("sends a message to the targetted player"), COMMAND_MessageTarget, true);
    g_pCore->GetCommands()->Add("vehicle_next_weapon", _("changes to the next weapon whilst in a vehicle"), COMMAND_VehicleNextWeapon, true);
    g_pCore->GetCommands()->Add("vehicle_previous_weapon", _("changes to the previous weapon whilst in a vehicle"), COMMAND_VehiclePreviousWeapon, true);
    g_pCore->GetCommands()->Add("sinfo", _("outputs info about the current server"), COMMAND_ServerInfo, true);

    // ACHTUNG" Should this be handled by the atomic cvar setter?
    g_pCore->GetCommands()->Add("textscale", _("defines the scale multiplier of all text-displays"), COMMAND_TextScale, true);

    // Bind our radio commands to the radio keys
    g_pCore->GetKeyBinds()->AddControlFunction("radio_next", CClientGame::HandleRadioNext);
    g_pCore->GetKeyBinds()->AddControlFunction("radio_previous", CClientGame::HandleRadioPrevious);

    // Development mode
    g_pCore->GetCommands()->Add("showcol", _("(Development mode) shows the colshapes"), COMMAND_ShowCollision);
    g_pCore->GetCommands()->Add("showsound", _("(Development mode) prints world sound ids into the debug window"), COMMAND_ShowSound);

#ifdef MTA_DEBUG
    g_pCore->GetCommands()->Add("showsync", "show sync data", COMMAND_ShowSyncData);
    // g_pCore->GetCommands ()->Add ( "dumpall",           "dump internals (comment)",                           COMMAND_DumpPlayers );
#endif

// Debug commands
#if defined(MTA_DEBUG) || defined(MTA_BETA)
    g_pCore->GetCommands()->Add("showsyncing", "shows syncing information", COMMAND_ShowSyncing);
#endif

#ifdef MTA_WEPSYNCDBG
    pCore->GetCommands()->Add("showwepdata", "shows the given player weapon data (nick)", COMMAND_ShowWepdata);
#endif

#if defined(MTA_DEBUG) || defined(MTA_DEBUG_COMMANDS)
    pCore->GetCommands()->Add("showwepdata", "shows the given player weapon data (nick)", COMMAND_ShowWepdata);
    pCore->GetCommands()->Add("showtasks", "shows the local player tasks (nick)", COMMAND_ShowTasks);
    pCore->GetCommands()->Add("showplayer", "shows extended player information (nick)", COMMAND_ShowPlayer);
    pCore->GetCommands()->Add("setmimic", "enables player mimics (amount)", COMMAND_SetMimic);
    pCore->GetCommands()->Add("setmimiclag", "enables player mimic lag (amount)", COMMAND_SetMimicLag);
    pCore->GetCommands()->Add("paintballs", "enables paintball mode", COMMAND_Paintballs);
    pCore->GetCommands()->Add("giveweapon", "gives the player a weapon (id)", COMMAND_GiveWeapon);
    pCore->GetCommands()->Add("showrpcs", "shows the remote prodecure calls", COMMAND_ShowRPCs);
    pCore->GetCommands()->Add("showinterpolation", "shows information about the interpolation", COMMAND_ShowInterpolation);
#endif

    // Got any arguments?
    if (szArguments && szArguments[0] != '\0')
    {
        if (stricmp(szArguments, "playback") == 0)
        {
            // Create new clientgame
            g_pClientGame = new CClientGame(true);

            // Connect
            g_pCore->GetConsole()->Echo("Starting playback...");
            g_pClientGame->StartPlayback();
            g_pClientGame->StartGame("Playback", NULL);
        }
        else
        {
            // Are we supposed to launch the server and play locally?
            if (stricmp(szArguments, "local") == 0)
            {
                // Create clientgame
                g_pClientGame = new CClientGame(true);

                g_pClientGame->SetupLocalGame(CClientGame::SERVER_TYPE_LOCAL);

                // Connect
                // g_pClientGame->StartLocalGame ( "Player" );
            }
            else if (stricmp(szArguments, "editor") == 0)
            {
                // Create clientgame
                g_pClientGame = new CClientGame(true);

                // Connect
                g_pClientGame->StartLocalGame(CClientGame::SERVER_TYPE_EDITOR);
            }
            else
            {
                InitializeArguments arguments = ExtractInitializeArguments(szArguments);

                // Got the nickname?
                if (!arguments.nickname.empty())
                {
                    g_pClientGame = new CClientGame;

                    // Enable the packet recorder
                    // g_pClientGame->EnablePacketRecorder ( "log.rec" );
                    // g_pCore->GetConsole ()->Echo ( "Packetlogger is logging to log.rec" );

                    // Start the game
                    g_pClientGame->StartGame(arguments.nickname.c_str(), arguments.password.c_str());
                }
                else
                {
                    g_pCore->GetConsole()->Print("deathmatch: not enough arguments");
                    g_pCore->GetModManager()->RequestUnload();
                }
            }
        }
    }

    // Success
    return 0;
}

void CClient::ClientShutdown()
{
    // Unbind our radio controls
    g_pCore->GetKeyBinds()->RemoveControlFunction("radio_next", CClientGame::HandleRadioNext);
    g_pCore->GetKeyBinds()->RemoveControlFunction("radio_previous", CClientGame::HandleRadioPrevious);

    // If the client modification is loaded, delete it
    if (g_pClientGame)
    {
        delete g_pClientGame;
        g_pClientGame = NULL;
    }
}

void CClient::PreFrameExecutionHandler()
{
    // If the client modification is loaded, pulse it
    if (g_pClientGame)
    {
        g_pClientGame->DoPulsePreFrame();
    }
}

void CClient::PreHUDRenderExecutionHandler(bool bDidUnminimize, bool bDidRecreateRenderTargets)
{
    if (g_pClientGame)
    {
        g_pClientGame->DoPulsePreHUDRender(bDidUnminimize, bDidRecreateRenderTargets);
    }
}

void CClient::PostFrameExecutionHandler()
{
    // If the client modification is loaded, pulse it
    if (g_pClientGame)
    {
        g_pClientGame->DoPulsePostFrame();
    }
}

void CClient::IdleHandler()
{
    if (g_pClientGame)
    {
        g_pClientGame->IdleHandler();
    }
}

bool CClient::WebsiteRequestResultHandler(const std::unordered_set<SString>& newPages)
{
    if (g_pClientGame)
        return g_pClientGame->TriggerBrowserRequestResultEvent(newPages);

    return false;
}

bool CClient::ProcessCommand(const char* commandName, size_t commandNameLength, const void* userdata, size_t userdataSize)
{
    if (commandName == nullptr || commandNameLength == 0)
        return false;

    std::string_view command{commandName, commandNameLength};

    if (command == mtasa::CMD_ALWAYS_SHOW_TRANSFERBOX)
    {
        if (userdata == nullptr || sizeof(bool) != userdataSize)
            return false;

        auto& alwaysShowTransferBox = *reinterpret_cast<const bool*>(userdata);
        g_pClientGame->GetTransferBox()->SetAlwaysVisible(alwaysShowTransferBox);
        return true;
    }

    return false;
}

void CClient::RestreamModel(unsigned short usModel)
{
    if (g_pClientGame)
    {
        g_pClientGame->RestreamModel(usModel);
    }
}

bool CClient::HandleException(CExceptionInformation* pExceptionInformation)
{
#ifndef MTA_DEBUG
#ifndef MTA_ALLOW_DEBUG
    // Let the clientgame write its dump, then make the core terminate our process
    if (g_pClientGame && pExceptionInformation)
    {
        g_pClientGame->HandleException(pExceptionInformation);
    }

    return false;
#else
    // We want to be able to debug using the debugger in debug-mode
    return true;
#endif
#else
    // We want to be able to debug using the debugger in debug-mode
    return true;
#endif
}

void CClient::GetPlayerNames(std::vector<SString>& vPlayerNames)
{
    if (g_pClientGame)
    {
        vPlayerNames.clear();
        for (std::vector<CClientPlayer*>::const_iterator iter = g_pClientGame->GetPlayerManager()->IterBegin();
             iter != g_pClientGame->GetPlayerManager()->IterEnd(); ++iter)
        {
            CClientPlayer* pClient = *iter;
            SString        strPlayerName = pClient->GetNametagText();
            vPlayerNames.push_back(strPlayerName);
        }
    }
}

void CClient::OnWindowFocusChange(bool state)
{
    g_pClientGame->OnWindowFocusChange(state);
}

CClient::InitializeArguments CClient::ExtractInitializeArguments(const char* arguments)
{
    // Format: "nickname [password]"
    // Examples: "GloriousToaster99 secret", "RandomPainter10"
    std::string_view view(arguments);
    using size_type = std::string_view::size_type;

    InitializeArguments result;

    // Search for the first non-whitespace character
    if (size_t nicknameBegin = view.find_first_not_of(' '); nicknameBegin != std::string_view::npos)
    {
        // Search for the first whitespace delimiter character
        if (size_t nicknameDelimiter = view.find_first_of(' ', nicknameBegin); nicknameDelimiter != std::string_view::npos)
        {
            result.nickname = view.substr(nicknameBegin, nicknameDelimiter - nicknameBegin);

            // Search for the next non-whitespace character
            if (nicknameDelimiter = view.find_first_not_of(' ', nicknameDelimiter); nicknameDelimiter != std::string_view::npos)
            {
                // Extract password from the string remainder
                if (size_t passwordDelimiter = view.find_first_of(' ', nicknameDelimiter); passwordDelimiter != std::string_view::npos)
                {
                    result.password = view.substr(nicknameDelimiter, passwordDelimiter - nicknameDelimiter);
                }
                else
                {
                    result.password = view.substr(nicknameDelimiter);
                }
            }
        }
        else
        {
            result.nickname = view.substr(nicknameBegin);
        }
    }

    return result;
}
