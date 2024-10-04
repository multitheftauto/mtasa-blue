/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/ClientCommands.h
 *  PURPOSE:     Header for client commands class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

bool COMMAND_Executed(const char* szCommand, const char* szArguments, bool bHandleRemotely, bool bHandled, bool bIsScriptedBind, bool bAllowScriptedBind);

void COMMAND_Help(const char* szCmdLine);
void COMMAND_Disconnect(const char* szCmdLine);
void COMMAND_ShowNametags(const char* szCmdLine);
void COMMAND_ShowChat(const char* szCmdLine);
void COMMAND_ShowNetstat(const char* szCmdLine);
void COMMAND_EnterPassenger(const char* szCmdLine);
void COMMAND_RadioNext(const char* szCmdLine);
void COMMAND_RadioPrevious(const char* szCmdLine);
void COMMAND_RadarMap(const char* szCmdLine);
void COMMAND_RadarZoomIn(const char* szCmdLine);
void COMMAND_RadarZoomOut(const char* szCmdLine);
void COMMAND_RadarMoveNorth(const char* szCmdLine);
void COMMAND_RadarMoveSouth(const char* szCmdLine);
void COMMAND_RadarMoveEast(const char* szCmdLine);
void COMMAND_RadarMoveWest(const char* szCmdLine);
void COMMAND_RadarAttach(const char* szCmdLine);
void COMMAND_RadarOpacityDown(const char* szCmdLine);
void COMMAND_RadarOpacityUp(const char* szCmdLine);
void COMMAND_RadarHelp(const char* szCmdLine);
void COMMAND_MessageTarget(const char* szCmdLine);
void COMMAND_VehicleNextWeapon(const char* szCmdLine);
void COMMAND_VehiclePreviousWeapon(const char* szCmdLine);
void COMMAND_TextScale(const char* szCmdLine);
void COMMAND_ChatBox(const char* szCmdLine);
void COMMAND_VoicePushToTalk(const char* szCmdLine);
void COMMAND_ServerInfo(const char* szCmdLine);

// DISABLE THESE BEFORE RELEASE
#ifdef MTA_DEBUG
void COMMAND_ShowSyncData(const char* szCmdLine);
// void COMMAND_DumpPlayers            ( const char* szCmdLine );
#endif

#if defined(MTA_DEBUG) || defined(MTA_BETA)
void COMMAND_ShowSyncing(const char* szCmdLine);
#endif

#ifdef MTA_WEPSYNCDBG
void COMMAND_ShowWepdata(const char* szCmdLine);
#endif

#if defined(MTA_DEBUG) || defined(COMMAND_ShowTasks)
void COMMAND_ShowWepdata(const char* szCmdLine);
void COMMAND_ShowTasks(const char* szCmdLine);
void COMMAND_ShowPlayer(const char* szCmdLine);
void COMMAND_SetMimic(const char* szCmdLine);
void COMMAND_SetMimicLag(const char* szCmdLine);
void COMMAND_Paintballs(const char* szCmdLine);
void COMMAND_GiveWeapon(const char* szCmdLine);
void COMMAND_ShowRPCs(const char* szCmdLine);
void COMMAND_ShowInterpolation(const char* szCmdLine);
#endif

// Commands enabled when development mode in on
void COMMAND_ShowCollision(const char* szCmdLine);
void COMMAND_ShowSound(const char* szCmdLine);
