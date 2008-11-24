/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/ClientCommands.h
*  PURPOSE:     Header for client commands class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLIENTCOMMANDS_H
#define __CLIENTCOMMANDS_H

bool COMMAND_Executed              ( const char* szCommand, const char* szArguments, bool bHandleRemotely, bool bHandled );

void COMMAND_Help                   ( const char* szCmdLine );
void COMMAND_Disconnect             ( const char* szCmdLine );
void COMMAND_FrameSkip              ( const char* szCmdLine );
void COMMAND_ShowNametags           ( const char* szCmdLine );
void COMMAND_ShowChat               ( const char* szCmdLine );
void COMMAND_ShowNetstat            ( const char* szCmdLine );
void COMMAND_EnterPassenger         ( const char* szCmdLine );
void COMMAND_RadioNext              ( const char* szCmdLine );
void COMMAND_RadioPrevious          ( const char* szCmdLine );
void COMMAND_DriveBy			    ( const char* szCmdLine );
void COMMAND_RadarMap               ( const char* szCmdLine );
void COMMAND_RadarZoomIn            ( const char* szCmdLine );
void COMMAND_RadarZoomOut           ( const char* szCmdLine );
void COMMAND_RadarMoveNorth         ( const char* szCmdLine );
void COMMAND_RadarMoveSouth         ( const char* szCmdLine );
void COMMAND_RadarMoveEast          ( const char* szCmdLine );
void COMMAND_RadarMoveWest          ( const char* szCmdLine );
void COMMAND_RadarAttach            ( const char* szCmdLine );
void COMMAND_RadarTransparency      ( const char* szCmdLine );
void COMMAND_MessageTarget          ( const char* szCmdLine );
void COMMAND_VehicleNextWeapon      ( const char* szCmdLine );
void COMMAND_VehiclePreviousWeapon  ( const char* szCmdLine );
void COMMAND_TextScale              ( const char* szCmdLine );
void COMMAND_ChatBox                ( const char* szCmdLine );

// DISABLE THESE BEFORE RELEASE
#ifdef MTA_DEBUG
void COMMAND_ShowSyncData           ( const char* szCmdLine );
//void COMMAND_DumpPlayers            ( const char* szCmdLine );
#endif

#if defined(MTA_DEBUG) || defined(MTA_BETA)
    void COMMAND_ShowSyncing        ( const char* szCmdLine );
    void COMMAND_FakeLag            ( const char* szCmdLine );
#endif

#ifdef MTA_DEBUG
    void COMMAND_Foo                ( const char* szCmdLine );
#endif

#if defined(MTA_DEBUG) || defined(COMMAND_ShowTasks)
    void COMMAND_ShowTasks          ( const char* szCmdLine );
    void COMMAND_ShowPlayer         ( const char* szCmdLine );
    void COMMAND_SetMimic           ( const char* szCmdLine );
    void COMMAND_SetMimicLag        ( const char* szCmdLine );
    void COMMAND_Paintballs         ( const char* szCmdLine );
    void COMMAND_Breakpoint         ( const char* szCmdLine );
    void COMMAND_GiveWeapon         ( const char* szCmdLine );
    void COMMAND_ShowRPCs           ( const char* szCmdLine );

    void COMMAND_Watch              ( const char* szCmdLine );
    void COMMAND_Hash               ( const char* szCmdLine );
    void COMMAND_HashArray          ( const char* szCmdLine );
    void COMMAND_Modules            ( const char* szCmdLine );

    void COMMAND_Debug              ( const char* szCmdLine );
    void COMMAND_Debug2             ( const char* szCmdLine );
    void COMMAND_Debug3             ( const char* szCmdLine );
    void COMMAND_Debug4             ( const char* szCmdLine );
#endif

#endif
