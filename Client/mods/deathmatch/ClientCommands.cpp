/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/ClientCommands.cpp
 *  PURPOSE:     Client commands handler class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/CWeapon.h>
#include <game/CTaskManager.h>
#include <game/Task.h>

using std::list;
using std::vector;

#ifdef MTA_DEBUG
#include <Tlhelp32.h>
#include <Psapi.h>
#include <shlwapi.h>
#include <Utils.h>
#endif

// Hide the "conversion from 'unsigned long' to 'DWORD*' of greater size" warning
#pragma warning(disable:4312)

extern CClientGame* g_pClientGame;

bool COMMAND_Executed(const char* szCommand, const char* szArguments, bool bHandleRemotely, bool bHandled, bool bIsScriptedBind, bool bAllowScriptedBind)
{
    // Has the core already handled this command?
    if (!bHandled)
    {
        const char* szCommandBufferPointer = szCommand;

        if (!bHandleRemotely)
        {
            // Is the command "say" and the arguments start with '/' ? (command comes from the chatbox)
            if (stricmp(szCommand, "chatboxsay") == 0)
            {
                szCommandBufferPointer = "say";
            }
        }

        // Toss them together so we can send it to the server
        SString strClumpedCommand;
        if (szArguments && szArguments[0])
            strClumpedCommand.Format("%s %s", szCommandBufferPointer, szArguments);
        else
            strClumpedCommand = szCommandBufferPointer;

        // Convert to Unicode, and clamp it to a maximum command length
        std::wstring strClumpedCommandUTF = MbUTF8ToUTF16(strClumpedCommand.c_str());
        strClumpedCommandUTF = strClumpedCommandUTF.substr(0, MAX_COMMAND_LENGTH);
        strClumpedCommand = UTF16ToMbUTF8(strClumpedCommandUTF);

        g_pClientGame->GetRegisteredCommands()->ProcessCommand(szCommandBufferPointer, szArguments);

        // Call the onClientConsole event
        auto pLocalPlayer = g_pClientGame->GetLocalPlayer();

        if (pLocalPlayer)
        {
            CLuaArguments Arguments;

            // Censor input for /login command
            if (!stricmp(szCommandBufferPointer, "login"))
            {
                Arguments.PushString(SString("%s ***", szCommandBufferPointer));
            }
            else
            {
                Arguments.PushString(strClumpedCommand);
            }

            pLocalPlayer->CallEvent("onClientConsole", Arguments, true);
        }

        // Write the chatlength and the content
        NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream();
        if (!pBitStream)
            return false;

        // Write it to the bitstream
        pBitStream->Write(strClumpedCommand.c_str(), static_cast<int>(strlen(strClumpedCommand.c_str())));

        // Send the packet to the server and free it
        g_pNet->SendPacket(PACKET_ID_COMMAND, pBitStream, PACKET_PRIORITY_HIGH, PACKET_RELIABILITY_RELIABLE_ORDERED, PACKET_ORDERING_CHAT);
        g_pNet->DeallocateNetBitStream(pBitStream);

        return true;
    }
    else
    {
        // Call the onClientCoreCommand event
        CLuaArguments Arguments;
        Arguments.PushString(szCommand);

        auto pLocalPlayer = g_pClientGame->GetLocalPlayer();
        pLocalPlayer->CallEvent("onClientCoreCommand", Arguments, true);

        // Call our comand-handlers for core-executed commands too, if allowed
        if (bAllowScriptedBind)
            g_pClientGame->GetRegisteredCommands()->ProcessCommand(szCommand, szArguments);
    }
    return false;
}

void COMMAND_ChatBox(const char* szInCmdLine)
{
    if (!(szInCmdLine && szInCmdLine[0]))
        return;

    COPY_CSTR_TO_TEMP_BUFFER(szCmdLine, szInCmdLine, 256);

    // Split it up into command and rgb
    char*         szCommand = strtok(szCmdLine, " ");
    char*         szRed = strtok(NULL, " ");
    char*         szGreen = strtok(NULL, " ");
    char*         szBlue = strtok(NULL, " ");
    unsigned char ucRed = 0, ucGreen = 0, ucBlue = 0;

    if (!szCommand)
        return;

    if (szRed && szGreen && szBlue)
    {
        ucRed = static_cast<unsigned char>(atoi(szRed));
        ucGreen = static_cast<unsigned char>(atoi(szGreen));
        ucBlue = static_cast<unsigned char>(atoi(szBlue));
    }

    // Open the chatbox input with command and color
    g_pCore->EnableChatInput(szCommand, COLOR_ARGB(255, ucRed, ucGreen, ucBlue));
}

void COMMAND_Help(const char* szCmdLine)
{
    // This isnt used
}

void COMMAND_Disconnect(const char* szCmdLine)
{
    g_pCore->RemoveMessageBox();
    AddReportLog(7110, "Game - COMMAND_Disconnect");
    g_pCore->GetModManager()->RequestUnload();
}

void COMMAND_ShowNametags(const char* szCmdLine)
{
    int  iCmd = (szCmdLine && szCmdLine[0]) ? atoi(szCmdLine) : -1;
    bool bShow = (iCmd == 1) ? true : (iCmd == 0) ? false : !g_pClientGame->GetNametags()->IsVisible();
    g_pClientGame->GetNametags()->SetVisible(bShow);
}

void COMMAND_ShowChat(const char* szCmdLine)
{
    int  iCmd = (szCmdLine && szCmdLine[0]) ? atoi(szCmdLine) : -1;
    bool bShow = (iCmd == 1) ? true : (iCmd == 0) ? false : !g_pCore->IsChatVisible();
    g_pCore->SetChatVisible(bShow, !bShow);
}

void COMMAND_ShowNetstat(const char* szCmdLine)
{
    int iCmd = (szCmdLine && szCmdLine[0]) ? atoi(szCmdLine) : -1;
    g_pClientGame->ShowNetstat(iCmd);
}

void COMMAND_EnterPassenger(const char* szCmdLine)
{
    // HACK: we don't want them to enter a vehicle if they're in cursor mode
    if (g_pClientGame && !g_pClientGame->AreCursorEventsEnabled())
    {
        // Disable passenger entry while reloading so it doesen't abort the animation
        CClientPlayer* pPlayer = g_pClientGame->GetPlayerManager()->GetLocalPlayer();
        if (pPlayer)
        {
            CWeapon* pWeapon = pPlayer->GetWeapon();
            if (pWeapon)
            {
                if (pWeapon->GetState() != WEAPONSTATE_RELOADING)
                {
                    // If we are already in a vehicle
                    CClientVehicle* pVehicle = pPlayer->GetOccupiedVehicle();
                    if (pVehicle)
                    {
                        // Make sure we are in a passenger seat, otherwise we must use enter_exit
                        if (pPlayer->GetOccupiedVehicleSeat() != 0)
                        {
                            pPlayer->ExitVehicle();
                        }
                    }
                    else
                    {
                        // Enter nearest vehicle as passenger
                        pPlayer->EnterVehicle(nullptr, true);
                    }
                }
            }
        }
    }
}

void COMMAND_RadioNext(const char* szCmdLine)
{
    if (g_pClientGame)
    {
        CClientPlayer* pPlayer = g_pClientGame->GetPlayerManager()->GetLocalPlayer();
        if (pPlayer)
        {
            pPlayer->NextRadioChannel();
        }
    }
}

void COMMAND_RadioPrevious(const char* szCmdLine)
{
    if (g_pClientGame)
    {
        CClientPlayer* pPlayer = g_pClientGame->GetPlayerManager()->GetLocalPlayer();
        if (pPlayer)
        {
            pPlayer->PreviousRadioChannel();
        }
    }
}

/*
void COMMAND_Screenshot ( const char* szCmdLine )
{
    g_pClientGame->GetScreenshot ()->SetScreenshotKey ( true );
}
*/

void COMMAND_PlayerMap(const char* szCmdLine)
{
    int  cmd = (szCmdLine && szCmdLine[0]) ? atoi(szCmdLine) : -1;
    bool show = (cmd == 1) ? true : (cmd == 0) ? false : !g_pClientGame->GetPlayerMap()->GetPlayerMapEnabled();
    g_pClientGame->GetPlayerMap()->SetPlayerMapEnabled(show);
}

void COMMAND_PlayerMapZoomIn(const char* szCmdLine)
{
    CPlayerMap* playerMap = g_pClientGame->GetPlayerMap();
    if (playerMap->IsPlayerMapShowing())
        playerMap->ZoomIn();
}

void COMMAND_PlayerMapZoomOut(const char* szCmdLine)
{
    CPlayerMap* playerMap = g_pClientGame->GetPlayerMap();
    if (playerMap->IsPlayerMapShowing())
        playerMap->ZoomOut();
}

void COMMAND_PlayerMapMoveNorth(const char* szCmdLine)
{
    CPlayerMap* playerMap = g_pClientGame->GetPlayerMap();
    if (!playerMap->IsPlayerMapShowing())
        return;

    if (playerMap->IsMovingNorth())
        playerMap->SetMovingNorth(false);
    else if (playerMap->IsMovingSouth())
        playerMap->SetMovingSouth(false);
    else
    {
        playerMap->SetMovingNorth(true);
        playerMap->SetMovingSouth(false);
        playerMap->SetMovingEast(false);
        playerMap->SetMovingWest(false);
    }
}

void COMMAND_PlayerMapMoveSouth(const char* szCmdLine)
{
    CPlayerMap* playerMap = g_pClientGame->GetPlayerMap();
    if (!playerMap->IsPlayerMapShowing())
        return;

    if (playerMap->IsMovingSouth())
        playerMap->SetMovingSouth(false);
    else if (playerMap->IsMovingNorth())
        playerMap->SetMovingNorth(false);
    else
    {
        playerMap->SetMovingNorth(false);
        playerMap->SetMovingSouth(true);
        playerMap->SetMovingEast(false);
        playerMap->SetMovingWest(false);
    }
}

void COMMAND_PlayerMapMoveEast(const char* szCmdLine)
{
    CPlayerMap* playerMap = g_pClientGame->GetPlayerMap();
    if (!playerMap->IsPlayerMapShowing())
        return;

    if (playerMap->IsMovingEast())
        playerMap->SetMovingEast(false);
    else if (playerMap->IsMovingWest())
        playerMap->SetMovingWest(false);
    else
    {
        playerMap->SetMovingNorth(false);
        playerMap->SetMovingSouth(false);
        playerMap->SetMovingEast(true);
        playerMap->SetMovingWest(false);
    }
}

void COMMAND_PlayerMapMoveWest(const char* szCmdLine)
{
    CPlayerMap* playerMap = g_pClientGame->GetPlayerMap();
    if (!playerMap->IsPlayerMapShowing())
        return;

    if (playerMap->IsMovingWest())
        playerMap->SetMovingWest(false);
    else if (playerMap->IsMovingEast())
        playerMap->SetMovingEast(false);
    else
    {
        playerMap->SetMovingNorth(false);
        playerMap->SetMovingSouth(false);
        playerMap->SetMovingEast(false);
        playerMap->SetMovingWest(true);
    }
}

void COMMAND_PlayerMapAttach(const char* szCmdLine)
{
    CPlayerMap* playerMap = g_pClientGame->GetPlayerMap();
    if (playerMap->IsPlayerMapShowing())
        playerMap->SetAttachedToLocalPlayer(!g_pClientGame->GetPlayerMap()->IsAttachedToLocalPlayer());
}

void COMMAND_PlayerMapOpacityDown(const char* szCmdLine)
{
    CPlayerMap* playerMap = g_pClientGame->GetPlayerMap();
    if (!playerMap->IsPlayerMapShowing())
        return;

    int mapAlpha;
    g_pCore->GetCVars()->Get("mapalpha", mapAlpha);
    mapAlpha = std::max(0, mapAlpha - 20);
    g_pCore->GetCVars()->Set("mapalpha", mapAlpha);
}

void COMMAND_PlayerMapOpacityUp(const char* szCmdLine)
{
    CPlayerMap* playerMap = g_pClientGame->GetPlayerMap();
    if (!playerMap->IsPlayerMapShowing())
        return;

    int mapAlpha;
    g_pCore->GetCVars()->Get("mapalpha", mapAlpha);
    mapAlpha = std::min(255, mapAlpha + 20);
    g_pCore->GetCVars()->Set("mapalpha", mapAlpha);
}

void COMMAND_PlayerMapHelp(const char* szCmdLine)
{
    g_pClientGame->GetPlayerMap()->ToggleHelpText();
}

void COMMAND_MessageTarget(const char* szCmdLine)
{
    if (!(szCmdLine && szCmdLine[0]))
        return;

    CClientPlayer* pTarget = g_pClientGame->GetTargetedPlayer();
    if (pTarget)
    {
        const char* szNick = pTarget->GetNick();
        if (!szNick)
            return;
        SString strParameters("%s %s", pTarget->GetNick(), szCmdLine);
        g_pCore->GetCommands()->Execute("msg", strParameters);
    }
    else
        g_pCore->ChatEchoColor("Error: no player target found", 255, 168, 0);
}

void COMMAND_VehicleNextWeapon(const char* szCmdLine)
{
    g_pClientGame->ChangeVehicleWeapon(true);
}

void COMMAND_VehiclePreviousWeapon(const char* szCmdLine)
{
    g_pClientGame->ChangeVehicleWeapon(false);
}

void COMMAND_TextScale(const char* szCmdLine)
{
    if (!(szCmdLine && szCmdLine[0]))
        return;

    float fScale = (float)atof(szCmdLine);
    if (fScale >= 0.8f && fScale <= 3.0f)
    {
        CClientTextDisplay::SetGlobalScale(fScale);
        g_pCore->GetCVars()->Set("text_scale", fScale);
    }
}

void DumpPlayer(CClientPlayer* pPlayer, FILE* pFile)
{
    unsigned int uiIndex = 0;

    // Player
    fprintf(pFile, "%s\n", "*** START OF PLAYER ***");

    // Write the data
    fprintf(pFile, "Nick: %s\n", pPlayer->GetNick());

    CVector vecTemp;
    pPlayer->GetPosition(vecTemp);
    fprintf(pFile, "Position: %f %f %f\n", vecTemp.fX, vecTemp.fY, vecTemp.fZ);

    fprintf(pFile, "Nametag text: %s\n", pPlayer->GetNametagText());

    unsigned char ucR, ucG, ucB;
    pPlayer->GetNametagColor(ucR, ucG, ucB);
    fprintf(pFile, "Nametag color: %u %u %u\n", ucR, ucG, ucB);

    fprintf(pFile, "Nametag show: %u\n", pPlayer->IsNametagShowing());

    fprintf(pFile, "Local player: %u\n", pPlayer->IsLocalPlayer());
    fprintf(pFile, "Dead: %u\n", pPlayer->IsDead());

    fprintf(pFile, "Exp aim: %u\n", pPlayer->IsExtrapolatingAim());
    fprintf(pFile, "Latency: %u\n", pPlayer->GetLatency());

    fprintf(pFile, "Last psync time: %u\n", pPlayer->GetLastPuresyncTime());
    fprintf(pFile, "Has con trouble: %u\n\n", pPlayer->HasConnectionTrouble());

    CClientTeam* pTeam = pPlayer->GetTeam();
    if (pTeam)
        fprintf(pFile, "Team: %s\n", pTeam->GetTeamName());

    // Get the matrix
    CMatrix matPlayer;
    pPlayer->GetMatrix(matPlayer);

    fprintf(pFile, "Matrix: vecRoll: %f %f %f\n", matPlayer.vRight.fX, matPlayer.vRight.fY, matPlayer.vRight.fZ);
    fprintf(pFile, "        vecDir:  %f %f %f\n", matPlayer.vFront.fX, matPlayer.vFront.fY, matPlayer.vFront.fZ);
    fprintf(pFile, "        vecWas:  %f %f %f\n", matPlayer.vUp.fX, matPlayer.vUp.fY, matPlayer.vUp.fZ);
    fprintf(pFile, "        vecPos:  %f %f %f\n\n", matPlayer.vPos.fX, matPlayer.vPos.fY, matPlayer.vPos.fZ);

    fprintf(pFile, "Euler rot: %f\n", pPlayer->GetCurrentRotation());
    fprintf(pFile, "Cam rot: %f\n", pPlayer->GetCameraRotation());

    pPlayer->GetMoveSpeed(vecTemp);
    fprintf(pFile, "Move speed: %f %f %f\n", vecTemp.fX, vecTemp.fY, vecTemp.fZ);

    pPlayer->GetTurnSpeed(vecTemp);
    fprintf(pFile, "Turn speed: %f %f %f\n", vecTemp.fX, vecTemp.fY, vecTemp.fZ);

    CControllerState State;
    pPlayer->GetControllerState(State);

    signed short* pController = reinterpret_cast<signed short*>(&State);
    fprintf(pFile, "CContr state: \n");
    for (uiIndex = 0; uiIndex < 36; uiIndex++)
    {
        fprintf(pFile, "State [%u] = %i\n", uiIndex, pController[uiIndex]);
    }

    pPlayer->GetLastControllerState(State);

    pController = reinterpret_cast<signed short*>(&State);
    fprintf(pFile, "LContr state: \n");
    for (uiIndex = 0; uiIndex < 36; uiIndex++)
    {
        fprintf(pFile, "State [%u] = %i\n", uiIndex, pController[uiIndex]);
    }

    fprintf(pFile, "\nVeh IO state: %i\n", pPlayer->GetVehicleInOutState());
    fprintf(pFile, "Visible: %u\n", pPlayer->IsVisible());
    fprintf(pFile, "Health: %f\n", pPlayer->GetHealth());
    fprintf(pFile, "Armor: %f\n", pPlayer->GetArmor());
    fprintf(pFile, "On screen: %u\n", pPlayer->IsOnScreen());
    fprintf(pFile, "Frozen: %u\n", pPlayer->IsFrozen());
    fprintf(pFile, "Respawn state: %i\n", pPlayer->GetRespawnState());

    fprintf(pFile, "Cur weapon slot: %i\n", static_cast<int>(pPlayer->GetCurrentWeaponSlot()));
    fprintf(pFile, "Cur weapon type: %i\n", static_cast<int>(pPlayer->GetCurrentWeaponType()));

    CWeapon* pWeapon = pPlayer->GetWeapon();
    if (pWeapon)
    {
        fprintf(pFile, "Cur weapon state: %i\n", static_cast<int>(pWeapon->GetState()));
        fprintf(pFile, "Cur weapon ammo clip: %u\n", pWeapon->GetAmmoInClip());
        fprintf(pFile, "Cur weapon ammo total: %u\n", pWeapon->GetAmmoTotal());
    }

    CTaskManager* pTaskMgr = pPlayer->GetTaskManager();
    if (pTaskMgr)
    {
        // Primary task
        CTask* pTask = pTaskMgr->GetTask(TASK_PRIORITY_PRIMARY);
        if (pTask)
        {
            fprintf(pFile, "Primary task name: %s\n", pTask->GetTaskName());
            fprintf(pFile, "Primary task type: %i\n", pTask->GetTaskType());
        }
        else
            fprintf(pFile, "Primary task: none\n");

        // Physical response task
        pTask = pTaskMgr->GetTask(TASK_PRIORITY_PHYSICAL_RESPONSE);
        if (pTask)
        {
            fprintf(pFile, "Physical response task name: %s\n", pTask->GetTaskName());
            fprintf(pFile, "Physical response task type: %i\n", pTask->GetTaskType());
        }
        else
            fprintf(pFile, "Physical response task: none\n");

        // Event response task temp
        pTask = pTaskMgr->GetTask(TASK_PRIORITY_EVENT_RESPONSE_TEMP);
        if (pTask)
        {
            fprintf(pFile, "Event rsp tmp task name: %s\n", pTask->GetTaskName());
            fprintf(pFile, "Event rsp tmp task type: %i\n", pTask->GetTaskType());
        }
        else
            fprintf(pFile, "Event rsp tmp task: none\n");

        // Event response task nontemp
        pTask = pTaskMgr->GetTask(TASK_PRIORITY_EVENT_RESPONSE_NONTEMP);
        if (pTask)
        {
            fprintf(pFile, "Event rsp nontmp task name: %s\n", pTask->GetTaskName());
            fprintf(pFile, "Event rsp nontmp task type: %i\n", pTask->GetTaskType());
        }
        else
            fprintf(pFile, "Event rsp nontmp task: none\n");

        // Event response task nontemp
        pTask = pTaskMgr->GetTask(TASK_PRIORITY_DEFAULT);
        if (pTask)
        {
            fprintf(pFile, "Default task name: %s\n", pTask->GetTaskName());
            fprintf(pFile, "Default task type: %i\n", pTask->GetTaskType());
        }
        else
            fprintf(pFile, "Default task: none\n");

        // Secondary attack
        pTask = pTaskMgr->GetTaskSecondary(TASK_SECONDARY_ATTACK);
        if (pTask)
        {
            fprintf(pFile, "Secondary attack task name: %s\n", pTask->GetTaskName());
            fprintf(pFile, "Secondary attack task type: %i\n", pTask->GetTaskType());
        }
        else
            fprintf(pFile, "Secondary attack task task: none\n");

        // Secondary duck
        pTask = pTaskMgr->GetTaskSecondary(TASK_SECONDARY_DUCK);
        if (pTask)
        {
            fprintf(pFile, "Secondary duck task name: %s\n", pTask->GetTaskName());
            fprintf(pFile, "Secondary duck task type: %i\n", pTask->GetTaskType());
        }
        else
            fprintf(pFile, "Secondary duck task task: none\n");

        // Secondary say
        pTask = pTaskMgr->GetTaskSecondary(TASK_SECONDARY_SAY);
        if (pTask)
        {
            fprintf(pFile, "Secondary say task name: %s\n", pTask->GetTaskName());
            fprintf(pFile, "Secondary say task type: %i\n", pTask->GetTaskType());
        }
        else
            fprintf(pFile, "Secondary say task task: none\n");

        // Secondary facial complex
        pTask = pTaskMgr->GetTaskSecondary(TASK_SECONDARY_FACIAL_COMPLEX);
        if (pTask)
        {
            fprintf(pFile, "Secondary facial task name: %s\n", pTask->GetTaskName());
            fprintf(pFile, "Secondary facial task type: %i\n", pTask->GetTaskType());
        }
        else
            fprintf(pFile, "Secondary facial task task: none\n");

        // Secondary partial anim
        pTask = pTaskMgr->GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM);
        if (pTask)
        {
            fprintf(pFile, "Secondary partial anim task name: %s\n", pTask->GetTaskName());
            fprintf(pFile, "Secondary partial anim task type: %i\n", pTask->GetTaskType());
        }
        else
            fprintf(pFile, "Secondary partial anim task task: none\n");

        // Secondary IK
        pTask = pTaskMgr->GetTaskSecondary(TASK_SECONDARY_IK);
        if (pTask)
        {
            fprintf(pFile, "Secondary IK task name: %s\n", pTask->GetTaskName());
            fprintf(pFile, "Secondary IK task type: %i\n", pTask->GetTaskType());
        }
        else
            fprintf(pFile, "Secondary IK task task: none\n");
    }

    float fX, fY;
    pPlayer->GetAim(fX, fY);
    fprintf(pFile, "Aim: %f %f\n", fX, fY);

    vecTemp = pPlayer->GetAimSource();
    fprintf(pFile, "Aim source: %f %f %f\n", vecTemp.fX, vecTemp.fY, vecTemp.fZ);

    vecTemp = pPlayer->GetAimTarget();
    fprintf(pFile, "Aim target: %f %f %f\n", vecTemp.fX, vecTemp.fY, vecTemp.fZ);

    fprintf(pFile, "Veh aim anim: %u\n", pPlayer->GetVehicleAimAnim());
    fprintf(pFile, "Ducked: %u\n", pPlayer->IsDucked());
    fprintf(pFile, "Wearing googles: %u\n", pPlayer->IsWearingGoggles());
    fprintf(pFile, "Has jetpack: %u\n", pPlayer->HasJetPack());
    fprintf(pFile, "In water: %u\n", pPlayer->IsInWater());
    fprintf(pFile, "On ground: %u\n", pPlayer->IsOnGround());
    fprintf(pFile, "Is climbing: %u\n", pPlayer->IsClimbing());
    fprintf(pFile, "Interiour: %u\n", pPlayer->GetInterior());
    fprintf(pFile, "Fight style: %u\n", static_cast<int>(pPlayer->GetFightingStyle()));
    fprintf(pFile, "Satchel count: %u\n", pPlayer->CountProjectiles(WEAPONTYPE_REMOTE_SATCHEL_CHARGE));

    CRemoteDataStorage* pRemote = pPlayer->GetRemoteData();
    if (pRemote)
    {
        vecTemp = pRemote->GetAkimboTarget();
        fprintf(pFile, "Akimbo target: %f %f %f\n", vecTemp.fX, vecTemp.fY, vecTemp.fZ);
        fprintf(pFile, "Akimbo aim up: %u\n", pRemote->GetAkimboTargetUp());
    }
    else
        fprintf(pFile, "Remote: no\n");

    fprintf(pFile, "Using gun: %u\n", pPlayer->IsUsingGun());
    fprintf(pFile, "Entering veh: %u\n", pPlayer->IsEnteringVehicle());
    fprintf(pFile, "Getting jacked: %u\n", pPlayer->IsGettingJacked());
    fprintf(pFile, "Alpha: %u\n", pPlayer->GetAlpha());

    fprintf(pFile, "Stats:\n");

    for (uiIndex = 0; uiIndex < NUM_PLAYER_STATS; uiIndex++)
    {
        fprintf(pFile, "Stat [%u] = %f\n", uiIndex, pPlayer->GetStat(uiIndex));
    }

    fprintf(pFile, "Streamed in: %u\n", pPlayer->IsStreamedIn());

    fprintf(pFile, "Model: %u\n", pPlayer->GetModel());

    // Write model data
    CModelInfo* pInfo = g_pGame->GetModelInfo(pPlayer->GetModel());
    fprintf(pFile, "Model ref count: %i\n", pInfo->GetRefCount());
    fprintf(pFile, "Model loaded: %u\n", pInfo->IsLoaded());
    fprintf(pFile, "Model valid: %u\n", pInfo->IsValid());

    // End of player
    fprintf(pFile, "%s", "\n*** END OF PLAYER ***\n\n\n\n");
}

void COMMAND_DumpPlayers(const char* szCmdLine)
{
    // Create a file to dump to
    SString strBuffer("%s/dump_%i.txt", g_pClientGame->GetModRoot(), GetTickCount32());
    FILE*   pFile = fopen(strBuffer, "w+");
    if (pFile)
    {
        // Write time now
        fprintf(pFile, "Comments: %s\n", szCmdLine);
        fprintf(pFile, "Time now: %u\n\n", CClientTime::GetTime());
        fprintf(pFile, "Objectcount: %u\n", g_pClientGame->GetObjectManager()->Count());
        fprintf(pFile, "Playercount: %u\n", g_pClientGame->GetPlayerManager()->Count());
        fprintf(pFile, "Vehiclecount: %u\n\n", g_pClientGame->GetVehicleManager()->Count());

        fprintf(pFile, "Used building pool: %u\n", g_pGame->GetPools()->GetNumberOfUsedSpaces(BUILDING_POOL));
        fprintf(pFile, "Used ped pool: %u\n", g_pGame->GetPools()->GetNumberOfUsedSpaces(PED_POOL));
        fprintf(pFile, "Used object pool: %u\n", g_pGame->GetPools()->GetNumberOfUsedSpaces(OBJECT_POOL));
        fprintf(pFile, "Used dummy pool: %u\n", g_pGame->GetPools()->GetNumberOfUsedSpaces(DUMMY_POOL));
        fprintf(pFile, "Used vehicle pool: %u\n", g_pGame->GetPools()->GetNumberOfUsedSpaces(VEHICLE_POOL));
        fprintf(pFile, "Used col model pool: %u\n", g_pGame->GetPools()->GetNumberOfUsedSpaces(COL_MODEL_POOL));
        fprintf(pFile, "Used task pool: %u\n", g_pGame->GetPools()->GetNumberOfUsedSpaces(TASK_POOL));
        fprintf(pFile, "Used event pool: %u\n", g_pGame->GetPools()->GetNumberOfUsedSpaces(EVENT_POOL));
        fprintf(pFile, "Used task alloc pool: %u\n", g_pGame->GetPools()->GetNumberOfUsedSpaces(TASK_ALLOCATOR_POOL));
        fprintf(pFile, "Used ped intelli pool: %u\n", g_pGame->GetPools()->GetNumberOfUsedSpaces(PED_INTELLIGENCE_POOL));
        fprintf(pFile, "Used ped attractor pool: %u\n", g_pGame->GetPools()->GetNumberOfUsedSpaces(PED_ATTRACTOR_POOL));
        fprintf(pFile, "Used entry info node pool: %u\n", g_pGame->GetPools()->GetNumberOfUsedSpaces(ENTRY_INFO_NODE_POOL));
        fprintf(pFile, "Used node route pool: %u\n", g_pGame->GetPools()->GetNumberOfUsedSpaces(NODE_ROUTE_POOL));
        fprintf(pFile, "Used patrol route pool: %u\n", g_pGame->GetPools()->GetNumberOfUsedSpaces(PATROL_ROUTE_POOL));
        fprintf(pFile, "Used point route pool: %u\n", g_pGame->GetPools()->GetNumberOfUsedSpaces(POINT_ROUTE_POOL));
        fprintf(pFile, "Used point double link pool: %u\n", g_pGame->GetPools()->GetNumberOfUsedSpaces(POINTER_DOUBLE_LINK_POOL));
        fprintf(pFile, "Used point single link pool: %u\n\n\n", g_pGame->GetPools()->GetNumberOfUsedSpaces(POINTER_SINGLE_LINK_POOL));

        // Loop through all players
        vector<CClientPlayer*>::const_iterator iter = g_pClientGame->GetPlayerManager()->IterBegin();
        for (; iter != g_pClientGame->GetPlayerManager()->IterEnd(); iter++)
        {
            // Write the player dump
            DumpPlayer(*iter, pFile);
        }

        // End of the dump. Close it
        fclose(pFile);
        g_pCore->GetConsole()->Print("dumpplayers: Dumping successfull");
    }
    else
        g_pCore->GetConsole()->Print("dumpplayers: Unable to create file");
}

void COMMAND_ShowSyncData(const char* szCmdLine)
{
    // Grab the player
    CClientPlayer* pPlayer = g_pClientGame->GetPlayerManager()->Get(szCmdLine);
    if (pPlayer)
    {
        g_pClientGame->GetSyncDebug()->Attach(*pPlayer);
    }
    else
    {
        g_pClientGame->GetSyncDebug()->Detach();
    }
}

void COMMAND_VoicePushToTalk(const char* szCmdLine)
{
    CVoiceRecorder* const pVoiceRecorder = g_pClientGame->GetVoiceRecorder();

    if (pVoiceRecorder->IsEnabled())
    {
        if (!szCmdLine)
            pVoiceRecorder->SetPTTState(!pVoiceRecorder->GetPTTState());
        else if (szCmdLine[0] == '0')
            pVoiceRecorder->SetPTTState(false);
        else if (szCmdLine[0] == '1')
            pVoiceRecorder->SetPTTState(true);
    }
    else
    {
        // Show warning only once per server
        static bool bDone = false;
        if (!bDone)
        {
            bDone = true;
            g_pCore->GetConsole()->Print("voiceptt: This server does not have voice enabled");
        }
    }
}

void COMMAND_ServerInfo(const char* szCmdLine)
{
    SString strSpacer;
    strSpacer.insert(0, 80, '-');
    g_pCore->GetConsole()->Print(*strSpacer);
    g_pClientGame->OutputServerInfo();
    g_pCore->GetConsole()->Print(*strSpacer);
}

#if defined (MTA_DEBUG) || defined (MTA_BETA)

void COMMAND_ShowSyncing(const char* szCmdLine)
{
    g_pClientGame->ShowSyncingInfo(atoi(szCmdLine) == 1);
}

#endif

#if defined(MTA_DEBUG) || defined(MTA_DEBUG_COMMANDS)
void COMMAND_ShowWepdata(const char* szCmdLine)
{
    if (!(szCmdLine && szCmdLine[0]))
        return;
    g_pClientGame->ShowWepdata(szCmdLine);
}

void COMMAND_ShowTasks(const char* szCmdLine)
{
    if (!(szCmdLine && szCmdLine[0]))
        return;
    g_pClientGame->ShowTasks(szCmdLine);
}

void COMMAND_ShowPlayer(const char* szCmdLine)
{
    if (!(szCmdLine && szCmdLine[0]))
        return;
    g_pClientGame->ShowPlayer(szCmdLine);
}

void COMMAND_SetMimic(const char* szCmdLine)
{
    if (!(szCmdLine && szCmdLine[0]))
        return;
    g_pClientGame->SetMimic(atoi(szCmdLine));
}

void COMMAND_SetMimicLag(const char* szCmdLine)
{
    if (!(szCmdLine && szCmdLine[0]))
        return;
    g_pClientGame->SetMimicLag(atoi(szCmdLine) == 1);
}

void COMMAND_Paintballs(const char* szCmdLine)
{
    if (!(szCmdLine && szCmdLine[0]))
        return;
    g_pClientGame->SetDoPaintballs(atoi(szCmdLine) == 1);
}

void COMMAND_GiveWeapon(const char* szCmdLine)
{
    if (!(szCmdLine && szCmdLine[0]))
        return;

    int nWeaponID = atoi(szCmdLine);
    /*
     * Check validity of the command line weapon id.
     */

    if (!CClientPickupManager::IsValidWeaponID(nWeaponID))
        return;

    CClientPed* pPed = g_pClientGame->GetManager()->GetPlayerManager()->GetLocalPlayer();
    if (pPed)
    {
        CWeapon* pPlayerWeapon = pPed->GiveWeapon((eWeaponType)nWeaponID, 9999);
        if (pPlayerWeapon)
        {
            pPlayerWeapon->SetAsCurrentWeapon();
        }
    }
}

void COMMAND_ShowRPCs(const char* szCmdLine)
{
    if (!(szCmdLine && szCmdLine[0]))
        return;
    g_pClientGame->GetRPCFunctions()->m_bShowRPCs = (atoi(szCmdLine) == 1);
}

void COMMAND_ShowInterpolation(const char*)
{
    g_pClientGame->ShowInterpolation(!g_pClientGame->IsShowingInterpolation());
}

#endif

void COMMAND_ShowCollision(const char* szCmdLine)
{
    int  iCmd = (szCmdLine && szCmdLine[0]) ? atoi(szCmdLine) : -1;
    bool bShow = (iCmd == 1) ? true : (iCmd == 0) ? false : !g_pClientGame->GetShowCollision();
    g_pClientGame->SetShowCollision(bShow);
    g_pCore->GetConsole()->Printf("showcol is now set to %d", bShow ? 1 : 0);
    if (bShow && !g_pClientGame->GetDevelopmentMode())
        g_pCore->GetConsole()->Print("showcol will have no effect because development mode is off");
}

void COMMAND_ShowSound(const char* szCmdLine)
{
    int  iCmd = (szCmdLine && szCmdLine[0]) ? atoi(szCmdLine) : -1;
    bool bShow = (iCmd == 1) ? true : (iCmd == 0) ? false : !g_pClientGame->GetShowSound();
    g_pClientGame->SetShowSound(bShow);
    g_pCore->GetConsole()->Printf("showsound is now set to %d", bShow ? 1 : 0);
    if (bShow && !g_pClientGame->GetDevelopmentMode())
        g_pCore->GetConsole()->Print("showsound will have no effect because development mode is off");
}
