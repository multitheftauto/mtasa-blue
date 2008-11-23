/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/ClientCommands.cpp
*  PURPOSE:     Client commands handler class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

/** Warning! This file contains debug code based on EA/Criterion software's
    RenderWare 3.x engine that is proprietary and may not be released
    to anyone. **/

#include <StdInc.h>

#ifdef MTA_DEBUG
#include <Tlhelp32.h>
#include <Psapi.h>
#include <shlwapi.h>
#include <CClientCRC32Hasher.h>
#include <Utils.h>
#endif

// Hide the "conversion from 'unsigned long' to 'DWORD*' of greater size" warning
#pragma warning(disable:4312)

extern CClientGame* g_pClientGame;

bool COMMAND_Executed ( const char* szCommand, const char* szArguments, bool bHandleRemotely, bool bHandled )
{
    // Has the core already handled this command?
    if ( !bHandled )
    {
        char szBuffer [256];
        CLuaArguments Arguments;

        const char* szCommandBufferPointer = szCommand;
        if ( !bHandleRemotely )
        {
            // Is the command "say" and the arguments start with '/' ? (command comes from the chatbox)
            if ( stricmp ( szCommand, "chatboxsay" ) == 0 )
            {
                // His line starts with '/'?
                if ( *szArguments == '/' )
                {
                    // Copy the characters after the slash to the 0 terminator to a seperate buffer
                    strncpy ( szBuffer, &szArguments [ 1 ], 256 );
                    szBuffer [ 255 ] = 0;

                    // Split it into command and arguments
                    char* szNewCommand = strtok ( szBuffer, " " );
                    char* szNewArguments = strtok ( NULL, "\0" );
                    if ( szNewCommand )
                    {
                        // Execute it as another command
                        if ( szNewArguments )
                        {
                            g_pCore->GetCommands ()->Execute ( szNewCommand, szNewArguments );
                        }
                        else
                        {
                            g_pCore->GetCommands ()->Execute ( szNewCommand, "" );
                        }

                        return true;
                    }
                }
                szCommandBufferPointer = "say";
            }
        }

        // Toss them together so we can send it to the server
        if ( szArguments && szArguments [ 0 ] )
            _snprintf ( szBuffer, 256, "%s %s", szCommandBufferPointer, szArguments );
        else
            strncpy ( szBuffer, szCommandBufferPointer, 256 );

        szBuffer [255] = 0;
        g_pClientGame->GetRegisteredCommands ()->ProcessCommand ( szCommandBufferPointer, szArguments );

        // Call the onClientConsole event
        Arguments.PushString ( szBuffer );

	    // Call the event on the local player's onClientConsole first
	    if ( g_pClientGame->GetLocalPlayer () )
		    g_pClientGame->GetLocalPlayer ()->CallEvent ( "onClientConsole", Arguments, true );

	    // Write the chatlength and the content
        NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream ();
        if ( !pBitStream ) 
            return false;

        // Write it to the bitstream
        pBitStream->Write ( szBuffer, static_cast < int > ( strlen ( szBuffer ) ) );

        // Send the packet to the server and free it
        g_pNet->SendPacket ( PACKET_ID_COMMAND, pBitStream, PACKET_PRIORITY_MEDIUM, PACKET_RELIABILITY_RELIABLE, PACKET_ORDERING_CHAT );
        g_pNet->DeallocateNetBitStream ( pBitStream );

        return true;
    }
    else
    {
        // Call our comand-handlers for core-executed commands too
        g_pClientGame->GetRegisteredCommands ()->ProcessCommand ( szCommand, szArguments );
    }
    return false;
}


void COMMAND_Help ( const char *szCmdLine )
{
    // This isnt used
}


void COMMAND_Disconnect ( const char *szCmdLine )
{
    g_pCore->GetModManager ()->RequestUnload ();
}

void COMMAND_ShowNametags ( const char* szCmdLine )
{
    int iCmd = ( szCmdLine && szCmdLine [ 0 ] ) ? atoi ( szCmdLine ) : -1;
    bool bShow = ( iCmd == 1 ) ? true : ( iCmd == 0 ) ? false : !g_pCore->IsChatVisible ();
    g_pClientGame->GetNametags ()->SetVisible ( bShow );
}

void COMMAND_ShowChat ( const char* szCmdLine )
{
    int iCmd = ( szCmdLine && szCmdLine [ 0 ] ) ? atoi ( szCmdLine ) : -1;
    bool bShow = ( iCmd == 1 ) ? true : ( iCmd == 0 ) ? false : !g_pCore->IsChatVisible ();
    g_pCore->SetChatVisible ( bShow );
}

void COMMAND_ChatBox ( const char* szCmdLine )
{
    if ( !(szCmdLine && szCmdLine[0]) )
        return;

    // Split it up into command and rgb
    char* szCommand = strtok ( const_cast < char* > ( szCmdLine ), " " );
    char* szRed = strtok ( NULL, " " );
    char* szGreen = strtok ( NULL, " " );
    char* szBlue = strtok ( NULL, " " );
    unsigned char ucRed = 0, ucGreen = 0, ucBlue = 0;

    if ( !szCommand )
        return;

    if ( szRed && szGreen && szBlue )
    {     
        ucRed = static_cast < unsigned char > ( atoi ( szRed ) );
        ucGreen = static_cast < unsigned char > ( atoi ( szGreen ) );
        ucBlue = static_cast < unsigned char > ( atoi ( szBlue ) );
    }

    // Open the chatbox input with command and color
    g_pCore->EnableChatInput ( szCommand, COLOR_ARGB ( 255, ucRed, ucGreen, ucBlue ) );
}

void COMMAND_ChatFont ( const char* szCmdLine )
{
    if ( !(szCmdLine && szCmdLine [ 0 ]) )
        return;

    int iFont = atoi ( szCmdLine );
    if ( iFont >= 0 && iFont <= ( int ) CHAT_FONT_MAX )
    {
        g_pCore->SetChatFont ( ( eChatFont ) iFont );
    }
}

void COMMAND_ChatLines ( const char* szCmdLine )
{
    if ( !(szCmdLine && szCmdLine [ 0 ]) )
        return;

    int iLines = atoi ( szCmdLine );
    if ( iLines >= 1 && iLines <= 30 )
    {
        g_pCore->SetChatLines ( iLines );
    }
}

void COMMAND_ChatScale ( const char* szCmdLine )
{
    if ( !(szCmdLine && szCmdLine [ 0 ]) )
        return;

    char* szX = strtok ( const_cast < char* > ( szCmdLine ), " " );
    char* szY = strtok ( NULL, " " );        
    if ( szX && szY )
    {
        CVector2D vecScale ( ( float ) atof ( szX ), ( float ) atof ( szY ) );            
        g_pCore->SetChatScale ( vecScale );
    }
}

void COMMAND_ChatWidth ( const char* szCmdLine )
{
    if ( szCmdLine && szCmdLine [ 0 ] )
    {   
        g_pCore->SetChatWidth ( ( float ) atof ( szCmdLine ) );
    }
}

void COMMAND_ChatColor ( const char* szCmdLine )
{
    if ( szCmdLine && szCmdLine [ 0 ] )
    {
        char* szR = strtok ( const_cast < char* > ( szCmdLine ), " " );
        char* szG = strtok ( NULL, " " );
        char* szB = strtok ( NULL, " " );
        char* szA = strtok ( NULL, " " );
        if ( szR && szG && szB && szA )
        {
            g_pCore->SetChatColor ( ( unsigned char ) atoi ( szR ),
                                    ( unsigned char ) atoi ( szG ),
                                    ( unsigned char ) atoi ( szB ),
                                    ( unsigned char ) atoi ( szA ) );
        }
    }
}

void COMMAND_ChatInputColor ( const char* szCmdLine )
{
    if ( szCmdLine && szCmdLine [ 0 ] )
    {
        char* szR = strtok ( const_cast < char* > ( szCmdLine ), " " );
        char* szG = strtok ( NULL, " " );
        char* szB = strtok ( NULL, " " );
        char* szA = strtok ( NULL, " " );
        if ( szR && szG && szB && szA )
        {
            g_pCore->SetChatInputColor ( ( unsigned char ) atoi ( szR ),
                                         ( unsigned char ) atoi ( szG ),
                                         ( unsigned char ) atoi ( szB ),
                                         ( unsigned char ) atoi ( szA ) );
        }
    }
}

void COMMAND_ChatInputPrefixColor ( const char* szCmdLine )
{
    if ( szCmdLine && szCmdLine [ 0 ] )
    {
        char* szR = strtok ( const_cast < char* > ( szCmdLine ), " " );
        char* szG = strtok ( NULL, " " );
        char* szB = strtok ( NULL, " " );
        char* szA = strtok ( NULL, " " );
        if ( szR && szG && szB && szA )
        {
            g_pCore->SetChatInputPrefixColor ( ( unsigned char ) atoi ( szR ),
                                               ( unsigned char ) atoi ( szG ),
                                               ( unsigned char ) atoi ( szB ),
                                               ( unsigned char ) atoi ( szA ) );
        }
    }
}

void COMMAND_ChatInputTextColor ( const char* szCmdLine )
{
    if ( szCmdLine && szCmdLine [ 0 ] )
    {
        char* szR = strtok ( const_cast < char* > ( szCmdLine ), " " );
        char* szG = strtok ( NULL, " " );
        char* szB = strtok ( NULL, " " );
        char* szA = strtok ( NULL, " " );
        if ( szR && szG && szB && szA )
        {
            g_pCore->SetChatInputTextColor ( ( unsigned char ) atoi ( szR ),
                                             ( unsigned char ) atoi ( szG ),
                                             ( unsigned char ) atoi ( szB ),
                                             ( unsigned char ) atoi ( szA ) );
        }
    }
}

void COMMAND_ChatTextCss ( const char* szCmdLine )
{
    if ( szCmdLine && szCmdLine [ 0 ] )
    {
        bool bEnabled = ( atoi ( szCmdLine ) == 1 );
        g_pCore->SetChatCssStyleText ( bEnabled );
    }
}

void COMMAND_ChatBackgroundCss ( const char* szCmdLine )
{
    if ( szCmdLine && szCmdLine [ 0 ] )
    {
        bool bEnabled = ( atoi ( szCmdLine ) == 1 );
        g_pCore->SetChatCssStyleBackground ( bEnabled );
    }
}

void COMMAND_ChatLineLife ( const char* szCmdLine )
{
    if ( szCmdLine && szCmdLine [ 0 ] )
    {
        unsigned long ulTime = ( unsigned long ) atoi ( szCmdLine );
        g_pCore->SetChatLineLife ( ulTime );
    }
}

void COMMAND_ChatLineFadeOut ( const char* szCmdLine )
{
    if ( szCmdLine && szCmdLine [ 0 ] )
    {
        unsigned long ulTime = ( unsigned long ) atoi ( szCmdLine );
        g_pCore->SetChatLineFadeOut ( ulTime );
    }
}

void COMMAND_ChatUseCEGUI ( const char* szCmdLine )
{
    if ( szCmdLine && szCmdLine [ 0 ] )
    {
        bool bUseCEGUI = ( atoi ( szCmdLine ) == 1 );
        g_pCore->SetChatUseCEGUI ( bUseCEGUI );
    }
}

void COMMAND_ShowFPS ( const char* szCmdLine )
{
    if ( szCmdLine && szCmdLine [ 0 ] )
	{
		
	}
}

void COMMAND_LimitFPS ( const char* szCmdLine )
{
    if ( szCmdLine && szCmdLine [ 0 ] )
	{
		int iFPS = atoi ( szCmdLine );
		if ( iFPS >= 20 && iFPS <= 100 )
		{
			g_pCore->GetConfig()->SetFPSLimit ( ( unsigned short ) iFPS );
			g_pCore->GetConsole ()->Printf ( "Your prefered FPS Limit set to %d", iFPS );
		}
		else
		{
			g_pCore->GetConsole ()->Printf ( "Specify a value between 20 and 100" );
		}
	}
}

void COMMAND_ShowNetstat ( const char* szCmdLine )
{
    if ( szCmdLine && szCmdLine [ 0 ] )
	{
		g_pClientGame->ShowNetstat ( atoi ( szCmdLine ) == 1 );
	}
}


void COMMAND_EnterPassenger ( const char* szCmdLine )
{
    // HACK: we don't want them to enter a vehicle if they're in cursor mode
    if ( !g_pClientGame->AreCursorEventsEnabled () )
    {
        g_pClientGame->ProcessVehicleInOutKey ( true );
    }
}


void COMMAND_RadioNext ( const char* szCmdLine )
{
    if ( g_pClientGame )
    {
        CClientPlayer* pPlayer = g_pClientGame->GetPlayerManager ()->GetLocalPlayer ();
        if ( pPlayer )
        {
            pPlayer->NextRadioChannel ();
        }
    }
}


void COMMAND_RadioPrevious ( const char* szCmdLine )
{
    if ( g_pClientGame )
    {
        CClientPlayer* pPlayer = g_pClientGame->GetPlayerManager ()->GetLocalPlayer ();
        if ( pPlayer )
        {
            pPlayer->PreviousRadioChannel ();
        }
    }
}

/*
void COMMAND_Screenshot ( const char* szCmdLine )
{
    g_pClientGame->GetScreenshot ()->SetScreenshotKey ( true );
}
*/

void COMMAND_RadarMap ( const char* szCmdLine )
{
    int iCmd = ( szCmdLine && szCmdLine [ 0 ] ) ? atoi ( szCmdLine ) : -1;
    bool bShow = ( iCmd == 1 ) ? true : ( iCmd == 0 ) ? false : !g_pClientGame->GetRadarMap ()->GetRadarEnabled ();
    g_pClientGame->GetRadarMap ()->SetRadarEnabled ( bShow );
}

void COMMAND_RadarZoomIn ( const char* szCmdLine )
{
    CRadarMap* pRadarMap = g_pClientGame->GetRadarMap ();

    if ( pRadarMap->IsRadarShowing () )
    {
        pRadarMap->ZoomIn ();
    }
}

void COMMAND_RadarZoomOut ( const char* szCmdLine )
{
    CRadarMap* pRadarMap = g_pClientGame->GetRadarMap ();

    if ( pRadarMap->IsRadarShowing () )
    {
        pRadarMap->ZoomOut ();
    }
}

void COMMAND_RadarMoveNorth ( const char* szCmdLine )
{
    CRadarMap* pRadarMap = g_pClientGame->GetRadarMap ();

    if ( pRadarMap->IsRadarShowing () )
    {
		// Toggle on/off
		if ( pRadarMap->IsMovingNorth () )
			pRadarMap->SetMovingNorth ( false );
        else if ( pRadarMap->IsMovingSouth () )
            pRadarMap->SetMovingSouth ( false );
		else
        {
            pRadarMap->SetMovingNorth ( true );
            pRadarMap->SetMovingSouth ( false );
            pRadarMap->SetMovingEast ( false );
            pRadarMap->SetMovingWest ( false );
        }
    }
}

void COMMAND_RadarMoveSouth ( const char* szCmdLine )
{
    CRadarMap* pRadarMap = g_pClientGame->GetRadarMap ();

    if ( pRadarMap->IsRadarShowing () )
    {
		// Toggle on/off
		if ( pRadarMap->IsMovingSouth () )
			pRadarMap->SetMovingSouth ( false );
        else if ( pRadarMap->IsMovingNorth () )
            pRadarMap->SetMovingNorth ( false );
		else
        {
            pRadarMap->SetMovingNorth ( false );
            pRadarMap->SetMovingSouth ( true );
            pRadarMap->SetMovingEast ( false );
            pRadarMap->SetMovingWest ( false );
        }
    }
}

void COMMAND_RadarMoveEast ( const char* szCmdLine )
{
    CRadarMap* pRadarMap = g_pClientGame->GetRadarMap ();

    if ( pRadarMap->IsRadarShowing () )
    {
		// Toggle on/off
		if ( pRadarMap->IsMovingEast () )
			pRadarMap->SetMovingEast ( false );
        else if ( pRadarMap->IsMovingWest () )
            pRadarMap->SetMovingWest ( false );
		else
        {
            pRadarMap->SetMovingNorth ( false );
            pRadarMap->SetMovingSouth ( false );
            pRadarMap->SetMovingEast ( true );
            pRadarMap->SetMovingWest ( false );
        }
    }
}

void COMMAND_RadarMoveWest ( const char* szCmdLine )
{
    CRadarMap* pRadarMap = g_pClientGame->GetRadarMap ();

    if ( pRadarMap->IsRadarShowing () )
    {
		// Toggle on/off
		if ( pRadarMap->IsMovingWest () )
			pRadarMap->SetMovingWest ( false );
        else if ( pRadarMap->IsMovingEast () )
            pRadarMap->SetMovingEast ( false );
		else
        {
            pRadarMap->SetMovingNorth ( false );
            pRadarMap->SetMovingSouth ( false );
            pRadarMap->SetMovingEast ( false );
            pRadarMap->SetMovingWest ( true );
        }
    }
}

void COMMAND_RadarAttach ( const char* szCmdLine )
{
    CRadarMap* pRadarMap = g_pClientGame->GetRadarMap ();

    if ( pRadarMap->IsRadarShowing () )
    {
        pRadarMap->SetAttachedToLocalPlayer ( !g_pClientGame->GetRadarMap ()->IsAttachedToLocalPlayer () );
    }
}

void COMMAND_RadarTransparency ( const char* szCmdLine )
{
}

void COMMAND_MessageTarget ( const char* szCmdLine )
{
    if ( !(szCmdLine || szCmdLine[0]) )
        return;

    CClientPlayer* pTarget = g_pClientGame->GetTargetedPlayer ();
    if ( pTarget )
    {
        const char * szNick = pTarget->GetNickPointer();
        if ( !szNick )
            return;
        char* szParameters = new char [ strlen ( szCmdLine ) + strlen(szNick) + 2 ];
        sprintf ( szParameters, "%s %s", pTarget->GetNickPointer (), szCmdLine );
        g_pCore->GetCommands ()->Execute ( "msg", szParameters );
        delete [] szParameters;
    }
    else
        g_pCore->ChatEchoColor ( "Error: no player target found", 255, 168, 0 );
}

void COMMAND_VehicleNextWeapon ( const char* szCmdLine )
{
    g_pClientGame->ChangeVehicleWeapon ( true );
}

void COMMAND_VehiclePreviousWeapon ( const char* szCmdLine )
{
    g_pClientGame->ChangeVehicleWeapon ( false );
}

void COMMAND_TextScale ( const char* szCmdLine )
{
    if ( !(szCmdLine && szCmdLine [ 0 ]) )
        return;

    float fScale = ( float ) atof ( szCmdLine );
    if ( fScale >= 0.8f && fScale <= 3.0f )
    {
        CClientTextDisplay::SetGlobalScale ( fScale );
        // Save this in our config too
        g_pCore->GetConfig ()->SetTextScale ( fScale );
    }
}


void DumpPlayer ( CClientPlayer* pPlayer, FILE* pFile )
{
    unsigned int uiIndex = 0;

    // Player
    fprintf ( pFile, "%s\n", "*** START OF PLAYER ***" );

    // Write the data
    fprintf ( pFile, "Nick: %s\n", pPlayer->GetNickPointer () );
    
    CVector vecTemp;
    pPlayer->GetPosition ( vecTemp );
    fprintf ( pFile, "Position: %f %f %f\n", vecTemp.fX, vecTemp.fY, vecTemp.fZ );

    fprintf ( pFile, "Nametag text: %s\n", pPlayer->GetNametagText () );

    unsigned char ucR, ucG, ucB;
    pPlayer->GetNametagColor ( ucR, ucG, ucB );
    fprintf ( pFile, "Nametag color: %u %u %u\n", ucR, ucG, ucB );

    fprintf ( pFile, "Nametag show: %u\n", pPlayer->IsNametagShowing () );

    fprintf ( pFile, "Local player: %u\n", pPlayer->IsLocalPlayer () );
    fprintf ( pFile, "Dead: %u\n", pPlayer->IsDead () );

    fprintf ( pFile, "Exp aim: %u\n", pPlayer->IsExtrapolatingAim () );
    fprintf ( pFile, "Latency: %u\n", pPlayer->GetLatency () );

    fprintf ( pFile, "Last psync time: %u\n", pPlayer->GetLastPuresyncTime () );
    fprintf ( pFile, "Has con trouble: %u\n\n", pPlayer->HasConnectionTrouble () );

    CClientTeam* pTeam = pPlayer->GetTeam ();
    if ( pTeam )
        fprintf ( pFile, "Team: %s\n", pTeam->GetTeamName () );

    // Get the matrix
    CMatrix matPlayer;
    pPlayer->GetMatrix ( matPlayer );

    fprintf ( pFile, "Matrix: vecRoll: %f %f %f\n", matPlayer.vRoll.fX, matPlayer.vRoll.fY, matPlayer.vRoll.fZ );
    fprintf ( pFile, "        vecDir:  %f %f %f\n", matPlayer.vDirection.fX, matPlayer.vDirection.fY, matPlayer.vDirection.fZ );
    fprintf ( pFile, "        vecWas:  %f %f %f\n", matPlayer.vWas.fX, matPlayer.vWas.fY, matPlayer.vWas.fZ );
    fprintf ( pFile, "        vecPos:  %f %f %f\n\n", matPlayer.vPos.fX, matPlayer.vPos.fY, matPlayer.vPos.fZ );

    fprintf ( pFile, "Euler rot: %f\n", pPlayer->GetCurrentRotation () );
    fprintf ( pFile, "Cam rot: %f\n", pPlayer->GetCameraRotation () );

    pPlayer->GetMoveSpeed ( vecTemp );
    fprintf ( pFile, "Move speed: %f %f %f\n", vecTemp.fX, vecTemp.fY, vecTemp.fZ );

    pPlayer->GetTurnSpeed ( vecTemp );
    fprintf ( pFile, "Turn speed: %f %f %f\n", vecTemp.fX, vecTemp.fY, vecTemp.fZ );

    CControllerState State;
    pPlayer->GetControllerState ( State );

    signed short* pController = reinterpret_cast < signed short* > ( &State );
    fprintf ( pFile, "CContr state: \n" );
    for ( uiIndex = 0; uiIndex < 36; uiIndex++ )
    {
        fprintf ( pFile, "State [%u] = %i\n", uiIndex, pController [uiIndex] );
    }

    pPlayer->GetLastControllerState ( State );

    pController = reinterpret_cast < signed short* > ( &State );
    fprintf ( pFile, "LContr state: \n" );
    for ( uiIndex = 0; uiIndex < 36; uiIndex++ )
    {
        fprintf ( pFile, "State [%u] = %i\n", uiIndex, pController [uiIndex] );
    }

    fprintf ( pFile, "\nVeh IO state: %i\n", pPlayer->GetVehicleInOutState () );
    fprintf ( pFile, "Visible: %u\n", pPlayer->IsVisible () );
    fprintf ( pFile, "Health: %f\n", pPlayer->GetHealth () );
    fprintf ( pFile, "Armor: %f\n", pPlayer->GetArmor () );
    fprintf ( pFile, "On screen: %u\n", pPlayer->IsOnScreen () );
    fprintf ( pFile, "Frozen: %u\n", pPlayer->IsFrozen () );
    fprintf ( pFile, "Respawn state: %i\n", pPlayer->GetRespawnState () );

    fprintf ( pFile, "Cur weapon slot: %i\n", static_cast < int > ( pPlayer->GetCurrentWeaponSlot () ) );
    fprintf ( pFile, "Cur weapon type: %i\n", static_cast < int > ( pPlayer->GetCurrentWeaponType () ) );

    CWeapon* pWeapon = pPlayer->GetWeapon ();
    if ( pWeapon )
    {
        fprintf ( pFile, "Cur weapon state: %i\n", static_cast < int > ( pWeapon->GetState () ) );
        fprintf ( pFile, "Cur weapon ammo clip: %u\n", pWeapon->GetAmmoInClip () );
        fprintf ( pFile, "Cur weapon ammo total: %u\n", pWeapon->GetAmmoTotal () );
    }

    CTaskManager* pTaskMgr = pPlayer->GetTaskManager ();
    if ( pTaskMgr )
    {
        // Primary task
        CTask* pTask = pTaskMgr->GetTask ( TASK_PRIORITY_PRIMARY );
        if ( pTask )
        {
            fprintf ( pFile, "Primary task name: %s\n", pTask->GetTaskName () );
            fprintf ( pFile, "Primary task type: %i\n", pTask->GetTaskType () );
        }
        else
            fprintf ( pFile, "Primary task: none\n" );

        // Physical response task
        pTask = pTaskMgr->GetTask ( TASK_PRIORITY_PHYSICAL_RESPONSE );
        if ( pTask )
        {
            fprintf ( pFile, "Physical response task name: %s\n", pTask->GetTaskName () );
            fprintf ( pFile, "Physical response task type: %i\n", pTask->GetTaskType () );
        }
        else
            fprintf ( pFile, "Physical response task: none\n" );

        // Event response task temp
        pTask = pTaskMgr->GetTask ( TASK_PRIORITY_EVENT_RESPONSE_TEMP );
        if ( pTask )
        {
            fprintf ( pFile, "Event rsp tmp task name: %s\n", pTask->GetTaskName () );
            fprintf ( pFile, "Event rsp tmp task type: %i\n", pTask->GetTaskType () );
        }
        else
            fprintf ( pFile, "Event rsp tmp task: none\n" );

        // Event response task nontemp
        pTask = pTaskMgr->GetTask ( TASK_PRIORITY_EVENT_RESPONSE_NONTEMP );
        if ( pTask )
        {
            fprintf ( pFile, "Event rsp nontmp task name: %s\n", pTask->GetTaskName () );
            fprintf ( pFile, "Event rsp nontmp task type: %i\n", pTask->GetTaskType () );
        }
        else
            fprintf ( pFile, "Event rsp nontmp task: none\n" );

        // Event response task nontemp
        pTask = pTaskMgr->GetTask ( TASK_PRIORITY_DEFAULT );
        if ( pTask )
        {
            fprintf ( pFile, "Default task name: %s\n", pTask->GetTaskName () );
            fprintf ( pFile, "Default task type: %i\n", pTask->GetTaskType () );
        }
        else
            fprintf ( pFile, "Default task: none\n" );

        // Secondary attack
        pTask = pTaskMgr->GetTaskSecondary ( TASK_SECONDARY_ATTACK );
        if ( pTask )
        {
            fprintf ( pFile, "Secondary attack task name: %s\n", pTask->GetTaskName () );
            fprintf ( pFile, "Secondary attack task type: %i\n", pTask->GetTaskType () );
        }
        else
            fprintf ( pFile, "Secondary attack task task: none\n" );

        // Secondary duck
        pTask = pTaskMgr->GetTaskSecondary ( TASK_SECONDARY_DUCK );
        if ( pTask )
        {
            fprintf ( pFile, "Secondary duck task name: %s\n", pTask->GetTaskName () );
            fprintf ( pFile, "Secondary duck task type: %i\n", pTask->GetTaskType () );
        }
        else
            fprintf ( pFile, "Secondary duck task task: none\n" );

        // Secondary say
        pTask = pTaskMgr->GetTaskSecondary ( TASK_SECONDARY_SAY );
        if ( pTask )
        {
            fprintf ( pFile, "Secondary say task name: %s\n", pTask->GetTaskName () );
            fprintf ( pFile, "Secondary say task type: %i\n", pTask->GetTaskType () );
        }
        else
            fprintf ( pFile, "Secondary say task task: none\n" );

        // Secondary facial complex
        pTask = pTaskMgr->GetTaskSecondary ( TASK_SECONDARY_FACIAL_COMPLEX );
        if ( pTask )
        {
            fprintf ( pFile, "Secondary facial task name: %s\n", pTask->GetTaskName () );
            fprintf ( pFile, "Secondary facial task type: %i\n", pTask->GetTaskType () );
        }
        else
            fprintf ( pFile, "Secondary facial task task: none\n" );

        // Secondary partial anim
        pTask = pTaskMgr->GetTaskSecondary ( TASK_SECONDARY_PARTIAL_ANIM );
        if ( pTask )
        {
            fprintf ( pFile, "Secondary partial anim task name: %s\n", pTask->GetTaskName () );
            fprintf ( pFile, "Secondary partial anim task type: %i\n", pTask->GetTaskType () );
        }
        else
            fprintf ( pFile, "Secondary partial anim task task: none\n" );

        // Secondary IK
        pTask = pTaskMgr->GetTaskSecondary ( TASK_SECONDARY_IK );
        if ( pTask )
        {
            fprintf ( pFile, "Secondary IK task name: %s\n", pTask->GetTaskName () );
            fprintf ( pFile, "Secondary IK task type: %i\n", pTask->GetTaskType () );
        }
        else
            fprintf ( pFile, "Secondary IK task task: none\n" );
    }

    float fX, fY;
    pPlayer->GetAim ( fX, fY );
    fprintf ( pFile, "Aim: %f %f\n", fX, fY );

    vecTemp = pPlayer->GetAimSource ();
    fprintf ( pFile, "Aim source: %f %f %f\n", vecTemp.fX, vecTemp.fY, vecTemp.fZ );

    vecTemp = pPlayer->GetAimTarget ();
    fprintf ( pFile, "Aim target: %f %f %f\n", vecTemp.fX, vecTemp.fY, vecTemp.fZ );

    fprintf ( pFile, "Veh aim anim: %u\n", pPlayer->GetVehicleAimAnim () );
    fprintf ( pFile, "Ducked: %u\n", pPlayer->IsDucked () );
    fprintf ( pFile, "Wearing googles: %u\n", pPlayer->IsWearingGoggles () );
    fprintf ( pFile, "Has jetpack: %u\n", pPlayer->HasJetPack () );
    fprintf ( pFile, "In water: %u\n", pPlayer->IsInWater () );
    fprintf ( pFile, "On ground: %u\n", pPlayer->IsOnGround () );
    fprintf ( pFile, "Is climbing: %u\n", pPlayer->IsClimbing () );
    fprintf ( pFile, "Interiour: %u\n", pPlayer->GetInterior () );
    fprintf ( pFile, "Fight style: %u\n", static_cast < int > ( pPlayer->GetFightingStyle () ) );
    fprintf ( pFile, "Satchel count: %u\n", pPlayer->CountProjectiles ( WEAPONTYPE_REMOTE_SATCHEL_CHARGE ) );

    CRemoteDataStorage* pRemote = pPlayer->GetRemoteData ();
    if ( pRemote )
    {
        vecTemp = pRemote->GetAkimboTarget ();
        fprintf ( pFile, "Akimbo target: %f %f %f\n", vecTemp.fX, vecTemp.fY, vecTemp.fZ );
        fprintf ( pFile, "Akimbo aim up: %u\n", pRemote->GetAkimboTargetUp () );
    }
    else
        fprintf ( pFile, "Remote: no\n" );

    fprintf ( pFile, "Using gun: %u\n", pPlayer->IsUsingGun () );
    fprintf ( pFile, "Entering veh: %u\n", pPlayer->IsEnteringVehicle () );
    fprintf ( pFile, "Getting jacked: %u\n", pPlayer->IsGettingJacked () );
    fprintf ( pFile, "Alpha: %u\n", pPlayer->GetAlpha () );
    

    fprintf ( pFile, "Stats:\n" );

    for ( uiIndex = 0; uiIndex < NUM_PLAYER_STATS; uiIndex++ )
    {
        fprintf ( pFile, "Stat [%u] = %f\n", uiIndex, pPlayer->GetStat ( uiIndex ) );
    }

    fprintf ( pFile, "Streamed in: %u\n", pPlayer->IsStreamedIn () );


    fprintf ( pFile, "Model: %u\n", pPlayer->GetModel () );

    // Write model data
    CModelInfo* pInfo = g_pGame->GetModelInfo ( pPlayer->GetModel () );
    fprintf ( pFile, "Model ref count: %i\n", pInfo->GetRefCount () );
    fprintf ( pFile, "Model loaded: %u\n", pInfo->IsLoaded () );
    fprintf ( pFile, "Model valid: %u\n", pInfo->IsValid () );

    // End of player
    fprintf ( pFile, "%s", "\n*** END OF PLAYER ***\n\n\n\n" );
}


void COMMAND_DumpPlayers ( const char* szCmdLine )
{
    // Create a file to dump to
    char szBuffer [256];
    snprintf ( szBuffer, 256, "%s/dump_%i.txt", g_pClientGame->GetModRoot (), GetTickCount () );
    FILE* pFile = fopen ( szBuffer, "w+" );
    if ( pFile )
    {
        // Write time now
        fprintf ( pFile, "Comments: %s\n", szCmdLine );
        fprintf ( pFile, "Time now: %u\n\n", CClientTime::GetTime () );
        fprintf ( pFile, "Objectcount: %u\n", g_pClientGame->GetObjectManager ()->Count () );
        fprintf ( pFile, "Playercount: %u\n", g_pClientGame->GetPlayerManager ()->Count () );
        fprintf ( pFile, "Vehiclecount: %u\n\n", g_pClientGame->GetVehicleManager ()->Count () );

        fprintf ( pFile, "Used building pool: %u\n", g_pGame->GetPools ()->GetNumberOfUsedSpaces ( BUILDING_POOL ) );
        fprintf ( pFile, "Used ped pool: %u\n", g_pGame->GetPools ()->GetNumberOfUsedSpaces ( PED_POOL ) );
        fprintf ( pFile, "Used object pool: %u\n", g_pGame->GetPools ()->GetNumberOfUsedSpaces ( OBJECT_POOL ) );
        fprintf ( pFile, "Used dummy pool: %u\n", g_pGame->GetPools ()->GetNumberOfUsedSpaces ( DUMMY_POOL ) );
        fprintf ( pFile, "Used vehicle pool: %u\n", g_pGame->GetPools ()->GetNumberOfUsedSpaces ( VEHICLE_POOL ) );
        fprintf ( pFile, "Used col model pool: %u\n", g_pGame->GetPools ()->GetNumberOfUsedSpaces ( COL_MODEL_POOL ) );
        fprintf ( pFile, "Used task pool: %u\n", g_pGame->GetPools ()->GetNumberOfUsedSpaces ( TASK_POOL ) );
        fprintf ( pFile, "Used event pool: %u\n", g_pGame->GetPools ()->GetNumberOfUsedSpaces ( EVENT_POOL ) );
        fprintf ( pFile, "Used task alloc pool: %u\n", g_pGame->GetPools ()->GetNumberOfUsedSpaces ( TASK_ALLOCATOR_POOL ) );
        fprintf ( pFile, "Used ped intelli pool: %u\n", g_pGame->GetPools ()->GetNumberOfUsedSpaces ( PED_INTELLIGENCE_POOL ) );
        fprintf ( pFile, "Used ped attractor pool: %u\n", g_pGame->GetPools ()->GetNumberOfUsedSpaces ( PED_ATTRACTOR_POOL ) );
        fprintf ( pFile, "Used entry info node pool: %u\n", g_pGame->GetPools ()->GetNumberOfUsedSpaces ( ENTRY_INFO_NODE_POOL ) );
        fprintf ( pFile, "Used node route pool: %u\n", g_pGame->GetPools ()->GetNumberOfUsedSpaces ( NODE_ROUTE_POOL ) );
        fprintf ( pFile, "Used patrol route pool: %u\n", g_pGame->GetPools ()->GetNumberOfUsedSpaces ( PATROL_ROUTE_POOL ) );
        fprintf ( pFile, "Used point route pool: %u\n", g_pGame->GetPools ()->GetNumberOfUsedSpaces ( POINT_ROUTE_POOL ) );
        fprintf ( pFile, "Used point double link pool: %u\n", g_pGame->GetPools ()->GetNumberOfUsedSpaces ( POINTER_DOUBLE_LINK_POOL ) );
        fprintf ( pFile, "Used point single link pool: %u\n\n\n", g_pGame->GetPools ()->GetNumberOfUsedSpaces ( POINTER_SINGLE_LINK_POOL ) );

        // Loop through all players
        list < CClientPlayer* > ::const_iterator iter = g_pClientGame->GetPlayerManager ()->IterBegin ();
        for ( ; iter != g_pClientGame->GetPlayerManager ()->IterEnd (); iter++ )
        {
            // Write the player dump
            DumpPlayer ( *iter, pFile );
        }

        // End of the dump. Close it
        fclose ( pFile );
        g_pCore->GetConsole ()->Print ( "dumpplayers: Dumping successfull" );
    }
    else
        g_pCore->GetConsole ()->Print ( "dumpplayers: Unable to create file" );
}


void COMMAND_ShowSyncData ( const char* szCmdLine )
{
    // Grab the player
    CClientPlayer* pPlayer = g_pClientGame->GetPlayerManager ()->Get ( szCmdLine );
    if ( pPlayer )
    {
        g_pClientGame->GetSyncDebug ()->Attach ( *pPlayer );
    }
    else
    {
        g_pClientGame->GetSyncDebug ()->Detach ();
    }
}

#if defined (MTA_DEBUG) || defined (MTA_BETA)

void COMMAND_ShowSyncing ( const char *szCmdLine )
{
    g_pClientGame->ShowSyncingInfo ( atoi ( szCmdLine ) == 1 );
}

void COMMAND_FakeLag ( const char *szCmdLine )
{
    char szBuffer [256];
    char* szExtraPing = NULL;
    char* szExtraPingVary = NULL;

    if ( !(szCmdLine && szCmdLine[0]) )
        return;

    strncpy ( szBuffer, szCmdLine, 256 );
    szBuffer[255] = 0;
    szExtraPing = strtok ( szBuffer, " " );
    szExtraPingVary = strtok ( NULL, " " );
    
    if ( !(szExtraPing && szExtraPingVary) )
    {
        g_pCore->GetConsole ()->Echo ( "Bad syntax" );
        return;
    }

    g_pNet->SetFakeLag ( 0, atoi ( szExtraPing ), atoi ( szExtraPingVary ) );
    g_pCore->GetConsole ()->Printf ( "Fake lag set to %s extra ping with %s extra ping variance", szExtraPing, szExtraPingVary );
}


#endif

#ifdef MTA_DEBUG

void COMMAND_Foo ( const char* szCmdLine )
{
    g_pClientGame->m_Foo.Test ( szCmdLine );
}

#endif


#if defined(MTA_DEBUG) || defined(MTA_DEBUG_COMMANDS)

void COMMAND_ShowTasks ( const char* szCmdLine )
{
    if ( !(szCmdLine && szCmdLine[0]) )
        return;
    g_pClientGame->ShowTasks ( szCmdLine );
}

void COMMAND_ShowPlayer ( const char* szCmdLine )
{
    if ( !(szCmdLine && szCmdLine[0]) )
        return;
    g_pClientGame->ShowPlayer ( szCmdLine );
}

void COMMAND_SetMimic ( const char* szCmdLine )
{
    if ( !(szCmdLine && szCmdLine[0]) )
        return;
    g_pClientGame->SetMimic ( atoi ( szCmdLine ) );
}

void COMMAND_SetMimicLag ( const char* szCmdLine )
{
    if ( !(szCmdLine && szCmdLine[0]) )
        return;
    g_pClientGame->SetMimicLag ( atoi ( szCmdLine ) == 1 );
}

void COMMAND_Paintballs ( const char* szCmdLine )
{
    if ( !(szCmdLine && szCmdLine[0]) )
        return;
    g_pClientGame->SetDoPaintballs ( atoi ( szCmdLine ) == 1 );
}

void COMMAND_Breakpoint ( const char* szCmdLine )
{
    if ( !(szCmdLine && szCmdLine[0]) )
        return;
    _asm int 3
    // Make our main pointer easily accessable
    // Added by slush:  You're a lazy ass if you use this.
    g_pClientGame;
}


void COMMAND_GiveWeapon ( const char *szCmdLine )
{
    if ( !(szCmdLine && szCmdLine[0]) )
        return;

    int nWeaponID = atoi(szCmdLine);
    /*
     * Check validity of the command line weapon id.
     */

    if ( !CClientPickupManager::IsValidWeaponID ( nWeaponID ) )
        return;

    CClientPed * pPed = g_pClientGame->GetManager()->GetPlayerManager()->GetLocalPlayer();
    if ( pPed )
    {
        CWeapon * pPlayerWeapon = pPed->GiveWeapon ( (eWeaponType)nWeaponID, 9999 );
        if ( pPlayerWeapon )
        {
            pPlayerWeapon->SetAsCurrentWeapon();
        }
    }
}

void COMMAND_ShowRPCs ( const char* szCmdLine )
{
    if ( !(szCmdLine && szCmdLine[0]) )
        return;
    g_pClientGame->GetRPCFunctions ()->m_bShowRPCs = ( atoi ( szCmdLine ) == 1 );
}


void COMMAND_Watch ( const char *szCmdLine )
{
    // Note: This code might be a little unsafe if the detouring done by the DLL happens to be done
    //       exactly on a call to WriteProcessMemory even though the chance is small.
    // adds a hook to a process and watches for WPMs to this one
    DWORD dwProcessIDs[250];
    DWORD pBytesReturned = 0;
    unsigned int uiListSize = 50;
    if ( EnumProcesses ( dwProcessIDs, 250 * sizeof(DWORD), &pBytesReturned ) )
    {
        for ( unsigned int i = 0; i < pBytesReturned / sizeof ( DWORD ); i++ )
        {
            // Open the process
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, dwProcessIDs[i]);
            if ( hProcess )
            {
                HMODULE pModule;
                DWORD cbNeeded;
                if ( EnumProcessModules ( hProcess, &pModule, sizeof ( HMODULE ), &cbNeeded ) )
                {
                    char szModuleName[500];
                    if ( GetModuleFileNameEx( hProcess, pModule, szModuleName, 500 ) )
                    {
                        if ( strcmpi ( szModuleName + strlen(szModuleName) - strlen(szCmdLine), szCmdLine ) == 0 )
                        {
                            g_pCore->GetConsole()->Printf ( "Attaching to %s with process id %d...", szModuleName, hProcess );
                            RemoteLoadLibrary ( hProcess, "C:/Program Files/Rockstar Games/GTA San Andreas/mta/wpmhookdll.dll" );
                            CloseHandle ( hProcess );
                            return;
                        } 
                    }
                }

                // Close the process
                CloseHandle ( hProcess );
            }
        }
    }
}


void COMMAND_Hash ( const char *szCmdLine )
{
    // Copy the cmd line
    char* szTemp = new char [ strlen ( szCmdLine ) + 1 ];
    strcpy ( szTemp, szCmdLine );

    // Split up the arguments
    char* szOffset = strtok ( szTemp, " " );
    char* szLength = strtok ( NULL, " " );
    if ( !szOffset || !szLength )
    {
        delete [] szTemp;
        return;
    }

    // Convert to integer
    unsigned long ulOffset = atol ( szOffset );
    unsigned int uiLength = atoi ( szLength );

    // Delete the temp buffer
    delete [] szTemp;

    // Create a hasher
    CClientCRC32Hasher Hasher;
    CRC32 Result;

    // Try hashing the specified area
    try
    {
        if ( Hasher.Calculate ( reinterpret_cast < const char* > ( ulOffset ), uiLength, Result ) )
        {
            // Print it
            g_pCore->GetConsole ()->Printf ( "Hash at 0x%08X, size %u:\n0x%08X", ulOffset, uiLength, Result );
            return;
        }
    }
    catch ( ... )
    {}

    // Failed
    g_pCore->GetConsole ()->Printf ( "Hashing 0x%08X, size %u failed!", ulOffset, uiLength );
}


void COMMAND_HashArray ( const char *szCmdLine )
{
    // Copy the cmd line
    char* szTemp = new char [ strlen ( szCmdLine ) + 1 ];
    strcpy ( szTemp, szCmdLine );

    // Split up the arguments
    char* szOffset = strtok ( szTemp, " " );
    char* szSize = strtok ( NULL, " " );
    char* szArrayLength = strtok ( NULL, " " );
    char* szArrayPad = strtok ( NULL, " " );
    if ( !szOffset || !szSize || !szArrayLength )
    {
        delete [] szTemp;
        return;
    }

    // Convert to integer
    unsigned long ulOffset = atol ( szOffset );
    unsigned int uiSize = atoi ( szSize );
    unsigned int uiArrayLength = atoi ( szArrayLength );

    unsigned int uiArrayPad = 0;
    if ( szArrayPad )
    {
        uiArrayPad = atoi ( szArrayPad );
    }

    // Delete the temp buffer
    delete [] szTemp;

    // Create a hasher
    CClientCRC32Hasher Hasher;
    Hasher.Start ();

    // Try hashing the specified area
    bool bSuccess = false;
    try
    {
        // Hash each segment in the array
        for ( unsigned int i = 0; i < uiArrayLength; i++ )
        {
            Hasher.Append ( reinterpret_cast < const char* > ( ulOffset + ( ( uiSize + uiArrayPad ) * i ) ), uiSize );
        }

        // Success
        bSuccess = true;
    }
    catch ( ... )
    {}

    // Success?
    if ( bSuccess )
    {
        // Finish the hashing and grab the hash
        CRC32 crcResult;
        Hasher.Finish ( crcResult );

        // Print the hash in the console
        g_pCore->GetConsole ()->Printf ( "Hashing array at 0x%08X [length: %u, size: %u, pad: %u]:\n0x%08X", ulOffset, uiArrayLength, uiSize, uiArrayPad, crcResult );
    }
    else
    {
        // Failed
        g_pCore->GetConsole ()->Printf ( "Hashing array at 0x%08X [length: %u, size: %u, pad: %u]: failed!", ulOffset, uiArrayLength, uiSize, uiArrayPad );
    }
}


void COMMAND_Modules ( const char *szCmdLine )
{
    // Get the base address of the requested module
    // Take a snapshot of all modules in the specified process. 
    HANDLE hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, GetCurrentProcessId () ); 
    if ( hModuleSnap != INVALID_HANDLE_VALUE ) 
    {
        //  Set the size of the structure before using it. 
        MODULEENTRY32 ModuleEntry; 
        ModuleEntry.dwSize = sizeof ( MODULEENTRY32 ); 

        // Retrieve information about the first module, 
        // and exit if unsuccessful 
        if ( Module32First ( hModuleSnap, &ModuleEntry ) ) 
        {
            // Create a file
            FILE* pFile = fopen ( "modules.txt", "w+" );
            if ( pFile )
            {
                // Now walk the module list of the process, 
                // and display information about each module 
                do
                {
                    // Print it
                    fprintf ( pFile, "** MODULE **\n"
                                     "Name: %s\n"
                                     "Base: 0x%P\n"
                                     "Size: 0x%P\n"
                                     "\n",
                                     ModuleEntry.szModule,
                                     ModuleEntry.modBaseAddr,
                                     ModuleEntry.modBaseSize );
                }
                while ( Module32Next ( hModuleSnap, &ModuleEntry ) );

                // Close it
                fclose ( pFile );
            }
        }

        // Close the snapshot object
        CloseHandle( hModuleSnap ); 
    }
}

#include <CClientCorona.h>
CClientPickup* pPickupTest = NULL;
CClientCorona* pCoronaTest = NULL;
CVehicle * debugTrain = NULL;
CClientPlayer* pRonkert = NULL;
CObject * obj = NULL;

void COMMAND_Debug ( const char *szCmdLine )
{
    __debugbreak();

    CClientPed* pPlayer = g_pClientGame->GetPlayerManager ()->GetLocalPlayer ();

    pPlayer->GetModelInfo ()->SetPedAudioType ( "PED_TYPE_GEN" );
    pPlayer->GetModelInfo ()->SetPedVoice ( VOICE_GEN, "VOICE_GEN_BBDYG1", "VOICE_GEN_BBDYG2" );

    return;

    CPools* pPools = g_pGame->GetPools ();
    int iEntryInfoNodeEntries = pPools->GetEntryInfoNodePool ()->GetNumberOfUsedSpaces ();
    int iPointerNodeSingleLinkEntries = pPools->GetPointerNodeSingleLinkPool ()->GetNumberOfUsedSpaces ();
    int iPointerNodeDoubleLinkEntries = pPools->GetPointerNodeDoubleLinkPool ()->GetNumberOfUsedSpaces ();

    g_pCore->GetConsole ()->Printf ( "entry info: %i", iEntryInfoNodeEntries );
    g_pCore->GetConsole ()->Printf ( "single node: %i", iPointerNodeSingleLinkEntries );
    g_pCore->GetConsole ()->Printf ( "dbl node: %i", iPointerNodeDoubleLinkEntries );
    /*
    CClientPed* pModel = g_pClientGame->GetPlayerManager ()->GetLocalPlayer ()->GetModel ();
    pModel->SetChoking ( atoi ( szCmdLine ) ? true : false );
    */


    //g_pGame->GetAudio ()->StartRadio ( atoi ( szCmdLine ) );
    

    /*
	CVector vecAimSource, vecAimTarget;
	g_pClientGame->GetLocalPlayerModel()->GetPosition (vecAimSource);
	g_pClientGame->GetLocalPlayerModel()->GetPosition (vecAimTarget);

	vecAimTarget.fY += 10.0f;
	vecAimTarget.fZ += 10.0f;
    */

	/*
	RwIm3DVertex vertices[4];
	RwImVertexIndex incides[4] = {0, 1, 2, 3};

	RwIm3DVertexSetPos(&vertices[0], vecAimSource.fX, vecAimSource.fY, vecAimSource.fZ);
	RwIm3DVertexSetPos(&vertices[1], vecAimSource.fX + 1.0f, vecAimSource.fY + 1.0f, vecAimSource.fZ + 1.0f);
	RwIm3DVertexSetPos(&vertices[2], vecAimTarget.fX, vecAimTarget.fY, vecAimTarget.fZ);
	RwIm3DVertexSetPos(&vertices[3], vecAimTarget.fX + 1.0f, vecAimTarget.fY + 1.0f, vecAimTarget.fZ + 1.0f);

	RwIm3DVertexSetRGBA(&vertices[0], 255, 0, 0, 255);
	RwIm3DVertexSetRGBA(&vertices[1], 255, 0, 0, 255);
	RwIm3DVertexSetRGBA(&vertices[2], 255, 0, 0, 255);
	RwIm3DVertexSetRGBA(&vertices[3], 255, 0, 0, 255);

    RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRISTRIP, indices, 4);
    RwIm3DEnd();
	*/

    /*
	RwV3d * vlist, *nlist;
	RpTriangle * tlist;

	// create the geometry
	RpGeometry * geo = RpGeometryCreate ( 180, 116, rpGEOMETRYTRISTRIP | rpGEOMETRYNORMALS | rpGEOMETRYPOSITIONS | rpGEOMETRYLIGHT | rpGEOMETRYPRELIT | rpGEOMETRYMODULATEMATERIALCOLOR ); //rpGEOMETRYTRISTRIP | rpGEOMETRYPRELIT

	// create the material
	RwSurfaceProperties surfProp;
    surfProp.ambient = 1.0f;
    surfProp.diffuse = 0.7f;
    surfProp.specular = 0.3f;
	RpMaterial *pentagonMaterial = RpMaterialCreate ();
	RpMaterialSetSurfaceProperties ( pentagonMaterial, &surfProp );

	// get the morph target and its vertices/triangles list
	RpMorphTarget * morph = RpGeometryGetMorphTarget(geo, 0);
	vlist = RpMorphTargetGetVertices(morph);
	tlist = RpGeometryGetTriangles(geo);
	nlist = RpMorphTargetGetVertexNormals(morph);

	RwInt32 i, j;
	for(i=0;i<60;i++) {
		BuckyBallVertexList[i].x /= 1000.0f;
		BuckyBallVertexList[i].y /= 1000.0f;
		BuckyBallVertexList[i].z /= 1000.0f;
	}
    for(i=0, j=0; i<5*12; i+=5, j+=5)
    {
		RwV3d a, b, normal;
        RwV3dSub(&a, &BuckyBallVertexList[BuckyBallPentagonList[i + 2]], &BuckyBallVertexList[BuckyBallPentagonList[i]]);
        RwV3dSub(&b, &BuckyBallVertexList[BuckyBallPentagonList[i + 1]], &BuckyBallVertexList[BuckyBallPentagonList[i]]);
        RwV3dCrossProduct(&normal, &a, &b);
        RwV3dNormalize(&normal, &normal);

        *vlist++ = BuckyBallVertexList[BuckyBallPentagonList[i]];
		*nlist++ = normal;
        *vlist++ = BuckyBallVertexList[BuckyBallPentagonList[i + 1]];
		*nlist++ = normal;
        *vlist++ = BuckyBallVertexList[BuckyBallPentagonList[i + 2]];
		*nlist++ = normal;
        *vlist++ = BuckyBallVertexList[BuckyBallPentagonList[i + 3]];
		*nlist++ = normal;
        *vlist++ = BuckyBallVertexList[BuckyBallPentagonList[i + 4]];
		*nlist++ = normal;

        RpGeometryTriangleSetVertexIndices(geo, tlist, (RwUInt16)j, (RwUInt16)(j + 2), (RwUInt16)(j + 1));
		RpGeometryTriangleSetMaterial(geo, tlist++, pentagonMaterial);
        RpGeometryTriangleSetVertexIndices(geo, tlist, (RwUInt16)j, (RwUInt16)(j + 3), (RwUInt16)(j + 2));
		RpGeometryTriangleSetMaterial(geo, tlist++, pentagonMaterial);
        RpGeometryTriangleSetVertexIndices(geo, tlist, (RwUInt16)j, (RwUInt16)(j + 4), (RwUInt16)(j + 3));
		RpGeometryTriangleSetMaterial(geo, tlist++, pentagonMaterial);
    }
    for(i=0; i<6*20; i+=6, j+=6)
    {
        RwV3d a, b, normal;
        RwV3dSub(&a, &BuckyBallVertexList[BuckyBallHexagonList[i+2]], &BuckyBallVertexList[BuckyBallHexagonList[i]]);
        RwV3dSub(&b, &BuckyBallVertexList[BuckyBallHexagonList[i+1]], &BuckyBallVertexList[BuckyBallHexagonList[i]]);
        RwV3dCrossProduct(&normal, &a, &b);
        RwV3dNormalize(&normal, &normal);

        *vlist++ = BuckyBallVertexList[BuckyBallHexagonList[i]];
		*nlist++ = normal;
        *vlist++ = BuckyBallVertexList[BuckyBallHexagonList[i + 1]];
		*nlist++ = normal;
        *vlist++ = BuckyBallVertexList[BuckyBallHexagonList[i + 2]];
		*nlist++ = normal;
        *vlist++ = BuckyBallVertexList[BuckyBallHexagonList[i + 3]];
		*nlist++ = normal;
        *vlist++ = BuckyBallVertexList[BuckyBallHexagonList[i + 4]];
		*nlist++ = normal;
        *vlist++ = BuckyBallVertexList[BuckyBallHexagonList[i + 5]];
		*nlist++ = normal;

        RpGeometryTriangleSetVertexIndices(geo, tlist, (RwUInt16)j, (RwUInt16)(j + 2), (RwUInt16)(j + 1));
        RpGeometryTriangleSetMaterial(geo, tlist++, pentagonMaterial);
        RpGeometryTriangleSetVertexIndices(geo, tlist, (RwUInt16)j, (RwUInt16)(j + 3), (RwUInt16)(j + 2));
        RpGeometryTriangleSetMaterial(geo, tlist++, pentagonMaterial);
        RpGeometryTriangleSetVertexIndices(geo, tlist, (RwUInt16)j, (RwUInt16)(j + 4), (RwUInt16)(j + 3));
        RpGeometryTriangleSetMaterial(geo, tlist++, pentagonMaterial);
        RpGeometryTriangleSetVertexIndices(geo, tlist, (RwUInt16)j, (RwUInt16)(j + 5), (RwUInt16)(j + 4));
        RpGeometryTriangleSetMaterial(geo, tlist++, pentagonMaterial);
    }

	// transform the geometry, so the geometry's bounding sphere gets calculated and we unlock the geometry

	// calculate the bounding sphere and adjust the geometry
	RwSphere boundingSphere;
    RwMatrix *matrix;
    RwV3d temp;

    RpMorphTargetCalcBoundingSphere(morph, &boundingSphere);
    matrix = RwMatrixCreate();

    RwV3dScale(&temp, &boundingSphere.center, -1.0f);
	//temp.z += 1.0f;
    RwMatrixTranslate(matrix, &temp, rwCOMBINEREPLACE);

    temp.x = temp.y = temp.z = 1.0f / boundingSphere.radius;
    RwMatrixScale(matrix, &temp, rwCOMBINEPOSTCONCAT);

	// re-calculate and unlock the geometry
    //RpGeometryTransform(geo, matrix);
	RpGeometryUnlock(geo);
	
	g_pCore->ChatPrintf ( "%.1f %.1f %.1f - %.1f %.1f %.1f", false, vecAimSource.fX, vecAimSource.fY, vecAimSource.fZ, vecAimTarget.fX, vecAimTarget.fY, vecAimTarget.fZ );

	CClientVehicle *v = g_pClientGame->GetLocalPlayerModel()->GetRealOccupiedVehicle();
	CVehicle *vv = v->GetGameVehicle();
	RpClump * clump = vv->GetRpClump ();
	//RpClump * clump = g_pClientGame->GetLocalPlayerModel()->GetGamePlayer()->GetRpClump ();

	// create the atomic and assign the frame
	atomic = RpAtomicCreate ();
	frame = RwFrameCreate ();

	// add the atomic's frame to the vehicle frame
	RwFrameAddChild ( RpGetFrame(clump), frame );
	RpAtomicSetFrame ( atomic, frame );

	//
	RpLight * light = RpLightCreate ( rpLIGHTPOINT );
	RpLightSetFrame ( light, frame );
	RpLightSetRadius ( light, 10.0f );

	RwRGBAReal color = {1.0f, 0.1f, 0.1f, 1.0f};
	RpLightSetColor ( light, &color );

	RpClumpAddLight ( clump, light );

	// set the geometry for the atomic
	RpAtomicSetGeometry ( atomic, geo, 0 );

	// remove all atomics and add the atomic to the vehicle
	//RpClumpForAllAtomics(clump,Test,(void*)clump);
	//RpClumpAddAtomic ( clump,atomic );

	// clean up some stuff
	RpMaterialDestroy(pentagonMaterial);
	RpGeometryDestroy(geo);

	// add the atomic to the world
	RpWorld *w = clump->object.parent;
	g_pCore->ChatPrintf("%x %x",&atomic->worldBoundingSphere,&atomic->boundingSphere);
	//RpWorldAddAtomic ( (RpWorld*)0x00C17038, atomic );
	//RpWorldAddClump ( (RpWorld*)0x00C17038, clump );
	//RpWorldAddAtomic ( w, atomic );
	//RpWorldAddClump ( w, clump );
	//RpWorldAddLight ( w, light );
    */

	/*
	unsigned int id = 0;
	char szName[32] = {0};
	sscanf(szCmdLine,"%s %u", &szName[0], &id);
	g_pCore->ChatPrintf("id %u model %s", false, id, &szName[0]);

	char szFileCOL[128] = {0};
	char szFileDFF[128] = {0};
	_snprintf(&szFileCOL[0],127,"c:\\temp\\imgtool\\mtarv\\%s.col",&szName[0]);
	_snprintf(&szFileDFF[0],127,"c:\\temp\\imgtool\\mtarv\\%s.dff",&szName[0]);

	g_pCore->ChatPrintf("col: %s",&szFileCOL[0]);
	g_pCore->ChatPrintf("dff: %s",&szFileDFF[0]);

	// load txd
	RwTexDictionary *txd = g_pGame->GetRenderWare ()->ReadTXD("c:\\temp\\imgtool\\mtarv\\dm_aztecx.txd");
	if ( txd == NULL ) {
		g_pCore->ChatPrintf("ERROR TXD",false);
		return;
	}
	// import txd
	g_pGame->GetRenderWare ()->ModelInfoImportTXD ( txd, id );

	// load dff
	RpClump *clump = g_pGame->GetRenderWare ()->ReadDFF(&szFileDFF[0],id);
	if ( clump == NULL ) {
		g_pCore->ChatPrintf("ERROR DFF",false);
		return;
	}

	// load col
	CColModel *col = g_pGame->GetRenderWare ()->ReadCOL(&szFileCOL[0],NULL);

	// add col and dff
	g_pGame->GetRenderWare ()->ReplaceCollisions(col, id);
	g_pGame->GetRenderWare ()->ReplaceAllAtomicsInModel(clump, id);
	return;
	*/

	/*
	RpAtomicContainer atomics[MAX_ATOMICS_PER_CLUMP];
	unsigned int cnt = g_pGame->GetRenderWare ()->LoadAtomics (clump, &atomics[0]);

	// replace any objects that are created (e.g. through lua createobject)
    list < CClientObject* > ::const_iterator iter = g_pClientGame->GetObjectManager()->IterBegin ();
    for ( ; iter != g_pClientGame->GetObjectManager()->IterEnd (); iter++ )
    {
		CClientObject *o = *iter;
		if(o && o->GetModel() == id) {		
			g_pCore->ChatPrintf("Found object\n",false);
			//g_pGame->GetRenderWare ()->ReplaceAllAtomicsInClump (o->GetGameObject()->GetRpClump(), &atomics[0], cnt);
			g_pGame->GetRenderWare ()->ReplaceAllAtomicsInModel (clump, id);

			g_pCore->ChatPrintf("Parsed %u atomics.",false, cnt);
		} else {
			g_pCore->ChatPrintf("Incorrect ID.",false);
		}
    }

	// buf, bufsize, ccolmodel&, keyname
	typedef void (__cdecl *LoadCollisionModelVer3_t) (unsigned char*, unsigned int, void*, const char*);
	LoadCollisionModelVer3_t LoadCollisionModelVer3 = (LoadCollisionModelVer3_t) 0x00537CE0;

	typedef bool (__cdecl *LoadCol_t) (int, unsigned char*, int);
	LoadCol_t LoadCol = (LoadCol_t) 0x004106D0;
	unsigned int sz = 0;

	DWORD CColModel_operator_new = 0x0040FC30;
	DWORD CColModel_constructor = 0x0040FB60;
	DWORD SIZEOF_CColModel = 0x30;
	
	// create a new ccolmodel
	DWORD colmodel = 0;
	_asm
	{
		push	SIZEOF_CColModel
		call	CColModel_operator_new
		add		esp, 4
		mov		colmodel, eax
	}
	if ( colmodel )
	{
		_asm
		{
			mov		ecx, colmodel
			call	CColModel_constructor
		}
	} else {
		g_pCore->ChatPrintf("Didn't create col?",false);
		return;
	}

	// load the collision file and parse it
	FILE * f = fopen(&szFileCol[0],"rb");
	if ( f ) {
		fseek(f,0,SEEK_END);
		sz = ftell(f);
		rewind(f);

		unsigned char *buf = new unsigned char[sz];
		if(fread(buf,1,sz,f) != sz) {
			g_pCore->ChatPrintf("Read error.",false);
			return;
		}

		// load the col3 data (skip the 32-byte header)
		LoadCollisionModelVer3(buf+32,sz-32,(void*)colmodel,NULL);
		
		// set some ccolmodel and modelinfo stuff
		DWORD * pPool = (DWORD*)0xA9B0C8;
		DWORD pModelInfoPointer = pPool[id];
		DWORD pOldColModel = *((DWORD*)(pPool[id] + 20));
		*((BYTE*)(pPool[id] + 0x13)) |= 8;
		g_pCore->ChatPrintf("CColModel %x", false, pOldColModel);

//		*((DWORD*)(pPool[id] + 20)) = (DWORD)colmodel;

		// copy the col area
		BYTE* pArea = (BYTE*)(pOldColModel+40);
		g_pCore->ChatPrintf("CColModel area %d", *pArea);
		*((BYTE*)colmodel+40) = *((BYTE*)(pOldColModel+40));

		// apply the ccolmodel to the cmodelinfo
		DWORD CModelInfo_SetColModel = 0x004C4BC0;
		__asm {
			mov		ecx, pModelInfoPointer
			push	1
			push	colmodel
			call	CModelInfo_SetColModel
		}

		// unknown
		DWORD func = 0x005B2C20;
		__asm {
			push	colmodel
			push	id
			call	func
			add		esp, 8
		}

		g_pCore->ChatPrintf("CColModel %x", false, *((DWORD*)(pPool[id] + 20)));

		delete [] buf;
	} else {
		g_pCore->ChatPrintf("Invalid file.",false);
	}

	return;
	*/

	/*
	DWORD *pPool = (DWORD*)0xA9B0C8;

	if ( 1 == 1 ) {
		//g_pCore->ChatPrintf("Valid object.", false);

		// get the rwframe for positioning
		//RwFrame * frame = RpGetFrame ( clump );

		//RpClump *newclump = (RpClump*) TEST ( szCmdLine );
		//g_pCore->ChatPrintf( "New clump has %u atomics.", RpClumpGetNumAtomics ( newclump ) );
		//g_pCore->ChatPrintf ( "%x", (DWORD)newclump );

		//RwStream * texture = RwStreamOpen ( rwSTREAMFILENAME, rwSTREAMREAD, "c:\\temp\\imgtool\\mtarv\\atm.txd" );
		RwStream * stream = RwStreamOpen ( rwSTREAMFILENAME, rwSTREAMREAD, "atm.dff" );
		
		RwChunkHeaderInfo info;
		//RwStreamReadChunkHeaderInfo ( stream, &info );
		//g_pCore->ChatPrintf("DFF: type<%x> version<%u>",info.type,info.version);

		if ( stream == NULL ) {
			g_pCore->ChatPrintf ( "RwStreamOpen error.", false );
			return;
		}

		//RwStreamReadChunkHeaderInfo ( stream, &info );
		//g_pCore->ChatPrintf("TXD: type<%x> version<%u>",info.type,info.version);

		// 0x16
		if ( RwStreamFindChunk ( texture, 0x16, NULL, NULL ) == false ) {
			g_pCore->ChatPrintf("Couldn't find chunk in TXD",false);
			return;
		}

		// 0x10
		if ( RwStreamFindChunk ( stream, 0x10, NULL, NULL ) == false ) {
			g_pCore->ChatPrintf("Couldn't find chunk in DFF",false);
			return;
		}

		//RwTexDictionary * main = (RwTexDictionary*)0x00C88010;
		RwTexDictionary * dict = RwTexDictionaryStreamRead ( texture );

		//unsigned short texdictid = *((unsigned short*)(pPool[veh->GetGameVehicle()->GetModelIndex()] + 0xA));
		unsigned short texdictid = *((unsigned short*)(pPool[atoi(szCmdLine)] + 0xA));
		SetTextureDict(texdictid);

		RwTexDictionary * main = RwTexDictionaryGetCurrent ();
		RwTexDictionaryForAllTextures ( dict, (void*)Test3, (void*)main );

		// R* collision hack?
		//g_pCore->ChatPrintf("ModelInfo pool (%u) points to: %x (%x)", veh->GetGameVehicle()->GetModelIndex(), pPool[veh->GetGameVehicle()->GetModelIndex()], &pPool[veh->GetGameVehicle()->GetModelIndex()]);

		RpPrtStdGlobalDataSetStreamEmbedded ( (void*)pPool[1337] );
		RpClump * newclump = RpClumpStreamRead ( stream );

		//if ( dict == NULL ) { g_pCore->ChatPrintf("No valid TXD",false); return; }

		//RpClumpSetFrame ( newclump, frame );
		//RpClumpForAllAtomics ( newclump, (void*)0x00537290, newclump );
		//RpClumpForAllAtomics ( clump, (void*)0x00537290, newclump );
		//RpClumpForAllAtomics ( clump, (void*)0x00537290, clump );
		//RwTexDictionaryForAllTextures ( main, (void*)Test4, (void*)main );
		//DWORD* cc = (DWORD*)0xC8800C;
		//unsigned short texdictid = ((unsigned short)(pPool[veh->GetGameVehicle()->GetModelIndex()] + 0xA));
		//DWORD dd = texdictid + texdictid*2;

		//RwTexDictionary * main = (RwTexDictionary*)(*cc + dd*4);
		//g_pCore->ChatPrintf ( "Tex dict id %u (%x)", texdictid, main );

		//RwV3d v;
		//v.x = v.y = 0; v.z = 5;
		//RwFrameTranslate ( RpGetFrame ( newclump ), &v, rwCOMBINEREPLACE );

		//RwTexDictionaryForAllTextures ( main, (void*)Test4, NULL );

		// add atomics from the new clump to the existing one
		//RpClumpForAllAtomics ( newclump, (void*)Test2, clump );

		//
		DWORD *x = (DWORD*)0x00B71840;
		*x = 1337;
		RpClumpForAllAtomics ( newclump, (void*)0x00537150, newclump );

		//RwFrame *parent = (RwFrame*)&pPool[593];
		//RwFrame *child = RpGetFrame(newclump);
		//RwFrameAddChild ( parent, child );
		//RpClumpForAllAtomics ( newclump, (void*)0x00537290, &pPool[593] );

		//clump = newclump;
		g_pCore->ChatPrintf ( "%s loaded. <position: %u> <type: %u>", false, szCmdLine, stream->position, stream->type );
		//LoadClumpFile ( stream, veh->GetGameVehicle ()->GetModelIndex () );

		RwStreamClose ( stream, NULL );
		//RwStreamClose ( texture, NULL );
	} else {
		g_pCore->ChatPrintf ( "Invalid object.", false );
	}
		
	//} else {
		//g_pCore->ChatPrintf ( "No vehicle?" );
	//}
	return;
	*/

    /*
    CVector vecPos;
    CClientPlayer * player = g_pClientGame->GetManager()->GetPlayerManager()->GetLocalPlayer();
    player->GetModel()->GetPosition(vecPos);

    DWORD trains[32] = {449 };
    debugTrain = g_pGame->GetPools()->AddTrain ( &vecPos, trains, 1, 1 );
    */
    
    /*
    unsigned char ucHour, ucMin;
    g_pGame->GetClock ()->Get ( &ucHour, &ucMin );
    g_pClientGame->GetBlendedWeather ()->SetWeatherBlended ( atoi ( szCmdLine ), ucHour + 1 );
    */

    /*
    CVector vecPos;
    g_pClientGame->GetLocalPlayerModel()->GetPosition ( vecPos );
    vecPos.fX += 4;

    //pPickupTest = g_pClientGame->GetPickupManager ()->Create ( atoi ( szCmdLine ), 50 );
    if ( g_pClientGame->GetPickupManager ()->IsValidPickupID ( atoi ( szCmdLine ) ) )
    {
        pPickupTest = new CClientPickup ( g_pClientGame->GetManager (), atoi ( szCmdLine ), 50, vecPos );
    }
    */

    //g_pClientGame->GetLocalPlayerModel()->RemoveAllWeapons ();

    /*
    pCoronaTest = new CClientCorona ( g_pClientGame->GetManager (), 50, true );
    CVector vecPosition;
    g_pClientGame->GetLocalPlayerModel ()->GetPosition ( vecPosition );
    pCoronaTest->SetPosition ( vecPosition );
    pCoronaTest->SetColor ( 0x00FF00FF );
    */

    /*
    CClientVehicle* pVehicle = g_pClientGame->GetVehicleManager ()->Get ( atoi ( szCmdLine ) );
    if ( pVehicle )
    {
        g_pCore->GetConsole ()->Printf ( "%s has %f health", pVehicle->GetNamePointer (), pVehicle->GetHealth () );
    }
    */

    /*
    CVector vecPos;
    g_pClientGame->GetManager()->GetPlayerManager()->GetLocalPlayer()->GetModel()->GetPosition ( vecPos );
    vecPos.fX += 4;
    g_pGame->GetPickups()->DisablePickupProcessing ( true );
    pDebugPickup = g_pGame->GetPickups()->CreatePickup ( &vecPos, g_pGame->GetWeaponInfo ( (eWeaponType)atoi(szCmdLine) )->GetModel (), PICKUP_ONCE );
    */



    /*
    CVector vecTemp;
    g_pClientGame->GetLocalPlayer ()->GetModel ()->GetPosition ( vecTemp );       

    for ( int i = 400; i < 415; i++ )
    {
        if ( CClientVehicleManager::IsValidModel ( i ) )
        {
            CClientVehicle* pVehicle = g_pClientGame->GetManager ()->GetVehicleManager ()->Add ( i );
            vecTemp.fX += 5.0f;
            pVehicle->SetPosition ( vecTemp );
        }
    }
    */

    /*
    for ( int i = 0; i < 50; i++ )
    {
        if ( CClientPickupManager::IsValidWeaponID ( i ) )
        {
            CWeaponInfo* pWeaponInfo = g_pGame->GetWeaponInfo ( static_cast < eWeaponType > ( i ) );
            g_pCore->GetConsole ()->Printf ( "%i:%u", i, static_cast < unsigned short > ( pWeaponInfo->GetModel () ) );
        }
    }
    */
/*
    char szBuffer [256];
    strcpy ( szBuffer, szCmdLine );
    char* szType = strtok ( szBuffer, " " );
    char* szForce = strtok ( NULL, " " );
    char* szRadius = strtok ( NULL, " " );
    char* szDistance = strtok ( NULL, " " );
    if ( szType && szForce && szRadius && szDistance )
    {
        CVector vecTemp;
        g_pClientGame->GetLocalPlayerModel ()->GetPosition ( vecTemp );
        vecTemp.fX += (float)atof ( szDistance );
        CExplosion* pExplosion = g_pGame->GetExplosionManager ()->AddExplosion ( (eExplosionType) atoi ( szCmdLine ), &vecTemp, NULL );
        pExplosion->SetExplosionForce ( (float)atof ( szForce ) );
        pExplosion->SetExplosionRadius ( (float)atof ( szRadius ) );

        g_pCore->GetConsole ()->Print ( "Created" );
    }*/

    /*CVector vecPosition;
    g_pClientGame->GetLocalPlayerModel ()->GetPosition ( vecPosition );
    vecPosition.fX += 5.0f;

    pRonkert = new CDeathmatchPlayer ( g_pClientGame->GetManager (), 50, false );
    CClientPed* pPlayerModel = pRonkert->LoadModel ( 50 );
    pPlayerModel->SetPosition ( vecPosition );
    pPlayerModel->SetTargetPosition ( 0, vecPosition );
    pPlayerModel->ResetInterpolation ();
    */

    /*
    CClientPed* pPlayer = g_pClientGame->GetLocalPlayerModel ();
    if ( pPlayer )
    {
        CClientVehicle* pVehicle = pPlayer->GetOccupiedVehicle ();
        if ( pVehicle )
        {
            CModelInfo* pModelInfo = g_pGame->GetModelInfo ( pVehicle->GetModel () );
            if ( pModelInfo )
            {
                if ( pModelInfo->IsUpgradeAvailable ( (eVehicleUpgradePosn)atoi (szCmdLine) ) )
                {
                    g_pCore->ShowMessageBox ( "available", "debug", true );
                }
                else
                    g_pCore->ShowMessageBox ( "unavailable", "debug", true );
            }
        }
    }
    */
}

#include "CVehicleNames.h"

CVehicle* aaa = NULL;
CVehicle* bbb = NULL;

CMatrix *save = NULL;
float fTest = 0;

#include <crtdbg.h>
void COMMAND_Debug2 ( const char *szCmdLine )
{
    g_pGame->GetAudio ()->StopRadio ();

    /*
	g_pGame->GetPools()->DumpPoolsStatus ();
	return;

	CClientVehicle* veh = g_pClientGame->GetLocalPlayerModel ()->GetOccupiedVehicle ();

	veh->SetMoveSpeed ( CVector ( 5.0f, 5.0f, 10.0f ) );
	return;

	if ( save == NULL ) {
		save = new CMatrix;
		veh->GetMatrix ( *save );
	}

	CQuat q_vehicle(save);

	CMatrix m;
	memcpy(&m,save,sizeof(CMatrix));
	m.vDirection.fX = m.vDirection.fY = m.vDirection.fZ = 0;
	m.vDirection.fY = 1.0f;
	CQuat q_changed(&m);

	CQuat q_output;
	CQuat::SLERP(q_vehicle,q_changed,q_output,fTest);
	CQuat::ToMatrix(q_output,*save);

	veh->SetMatrix ( *save );

	g_pCore->ChatPrintf("SLERP: %.2f - %.1f %.1f %.1f", fTest, save->vDirection.fX, save->vDirection.fY, save->vDirection.fZ);

	fTest += 0.01f;

	return;
    */

	/*
	//_ASSERTE(_CrtCheckMemory ());
	char * buf = new char[1024];

	buf[1024] = 10;
	buf[1025] = 10;

	delete [] buf;

	return;

	// get some world-space vectors
	CVector vecA, vecB;
	RwV3d vA, vB;
	g_pClientGame->GetLocalPlayerModel()->GetPosition (vecA);
	g_pClientGame->GetLocalPlayerModel()->GetPosition (vecB);
	vecB.fY += 5.0f;

	vA.x = vecA.fX; vA.y = vecA.fY; vA.z = vecA.fZ;
	vB.x = vecB.fX; vB.y = vecB.fY; vB.z = vecB.fZ;

	// get our ltm (object->world) and invert it (world->object)
	RwMatrix * ltm = &frame->ltm;
	RwMatrix * iltm = RwMatrixCreate ();
	RwMatrixInvert ( iltm, ltm );

	// convert our vectors
	RwV3d tA, tB;
	RwV3dTransformVector ( &tA, &vA, iltm );
	RwV3dNormalize ( &tA, &tA );
	RwV3dTransformVector ( &tB, &vB, iltm );
	RwV3dNormalize ( &tB, &tB );

	RwFrameTranslate ( frame, &tB, rwCOMBINEPOSTCONCAT );
	return;

    float bounce = atof(szCmdLine);
    g_pClientGame->SetMarkerBounce(bounce);
    return;
	obj = g_pGame->GetPools ()->AddObject ( 1337 );
	
	CVector v;
	g_pClientGame->GetLocalPlayer()->GetPosition ( v );
	v.fX += 5;

	obj->SetPosition ( &v );

	return;

	// get the vehicle
	CClientVehicle* veh = g_pClientGame->GetLocalPlayerModel ()->GetOccupiedVehicle ();
	unsigned int vehid = veh->GetGameVehicle()->GetModelIndex();

	if ( veh ) {
		if ( atoi(szCmdLine) == 5 ) {
			veh->FuckCarCompletely(false);
		} else {
			veh->GetGameVehicle()->GetDamageManager()->SetWheelStatus(FRONT_LEFT_WHEEL,atoi(szCmdLine));
			veh->GetGameVehicle()->GetDamageManager()->SetWheelStatus(FRONT_RIGHT_WHEEL,atoi(szCmdLine));
			veh->GetGameVehicle()->GetDamageManager()->SetWheelStatus(REAR_LEFT_WHEEL,atoi(szCmdLine));
			veh->GetGameVehicle()->GetDamageManager()->SetWheelStatus(REAR_RIGHT_WHEEL,atoi(szCmdLine));
		}

		return;
		g_pCore->ChatPrintf("Swapping wheels for vehicle %u", vehid);

		// get the rw clump
		RpClump * pClumpVehicle = (RpClump*) veh->GetGameVehicle ()->GetRpClump ();

		// load our new model stream
		char szDFF[MAX_PATH] = {0};
		char szTXD[MAX_PATH] = {0};

		snprintf ( &szTXD[0], MAX_PATH-1, "C:\\Program Files\\Rockstar Games\\GTA San Andreas\\%s.txd", szCmdLine );
		snprintf ( &szDFF[0], MAX_PATH-1, "C:\\Program Files\\Rockstar Games\\GTA San Andreas\\%s.dff", szCmdLine );

		// read the txd and dff
		RwTexDictionary * pTXD = g_pGame->GetRenderWare ()->ReadTXD ( &szTXD[0] );
		g_pGame->GetRenderWare ()->ModelInfoImportTXD ( pTXD, vehid );

		RpClump * pClumpModel = g_pGame->GetRenderWare ()->ReadDFF ( &szDFF[0], 443 );

		// add atomics
		g_pGame->GetRenderWare ()->AddAllAtomics ( pClumpVehicle, pClumpModel );

		g_pCore->ChatPrintf ( "Succesfully swapped wheels '%s'!", szCmdLine );
	} else {
		g_pCore->ChatPrintf ( "Invalid object.", false );
	}		

	return;

	/*g_pGame->GetPlayerInfo ()->GivePlayerParachute ();
	exit;*/
/*
	int usAudio;
	int iSlot;
	sscanf(szCmdLine, "%u %d", &usAudio, &iSlot);
	g_pGame->GetAudio()->PreloadMissionAudio(usAudio, iSlot);
	g_pCore->GetConsole()->Printf("Preloading sound %u in slot %d", usAudio, iSlot);
	return;

    char aa [256];
    strcpy ( aa, szCmdLine );
    char* a = strtok ( aa, " " );
    char* b = strtok ( NULL, " " );
    char* c = strtok ( NULL, " " );
    if ( !a || !b || !c ) return;
    float x = (float)atof ( a );
    float y = (float)atof ( b );
    float z = (float)atof ( c );

    CVector fff = CVector ( x, y, z );
    g_pClientGame->GetManager ()->GetCamera ()->SetRotation ( fff );
    */

    /*
    if ( aaa || bbb )
    {
        _asm int 3;
        CVehicle* pA = aaa->GetNextTrainCarriage ();
    }
    else
    {
        g_pGame->GetModelInfo ( 590 )->AddRef ();

        CVector vecPos;
        CClientPlayer * player = g_pClientGame->GetManager()->GetPlayerManager()->GetLocalPlayer();
        player->GetModel()->GetPosition(vecPos);

        DWORD a [1];
        a [0] = 590;
        aaa = g_pGame->GetPools()->AddTrain ( &vecPos, a, 1, 1 );
        bbb = g_pGame->GetPools()->AddTrain ( &vecPos, a, 1, 1 );
        aaa->SetNextTrainCarriage ( bbb );

        _asm int 3;
        CVehicle* pA = aaa->GetNextTrainCarriage ();
    }
    */

    /*
    //CClientVehicle* pVehicle = g_pClientGame->GetLocalPlayerModel ()->GetOccupiedVehicle ();
    int i = 0;
    CVehicle * first = debugTrain;
    CVehicle * last = 0;
    
    while ( debugTrain )
    {
        CVehicle * next = debugTrain->GetNextTrainCarriage();
            
        if ( next == 0 )
            last = debugTrain;
        else if ( debugTrain != first )
            g_pGame->GetPools()->RemoveVehicle ( debugTrain );
        
        debugTrain = next;
        i++;
    }

    if ( first && last )
        first->SetNextTrainCarriage ( last );
    /*
    if ( pCoronaTest )
    {
        delete pCoronaTest;
        pCoronaTest = NULL;
    }
    */

    /*
    unsigned long* ulFramelimiter = (unsigned long*)0xC1704C;
    *ulFramelimiter = atoi ( szCmdLine );
    */

    //g_pCore->GetConsole ()->Printf ( "name = %s", CVehicleNames::GetVehicleName ( atoi ( szCmdLine ) ) );
    //g_pCore->GetConsole ()->Printf ( "id = %u", CVehicleNames::GetVehicleModel ( szCmdLine ) );

    /*
    char szBuffer [256];
    strcpy ( szBuffer, szCmdLine );
    char* szA = strtok ( szBuffer, " " );
    char* szB = strtok ( szBuffer, " " );
    if ( szA && szB )
    {
        g_pClientGame->GetBlendedWeather ()->SetWeatherInstantly ( atoi ( szA ), atoi ( szB ) );
    }
    */

    /*
    CClientRadarArea* pRadarArea = new CClientRadarArea ( g_pClientGame->GetManager (), 50 );
    CVector vecPosition;
    g_pClientGame->GetLocalPlayerModel ()->GetPosition ( vecPosition );
    pRadarArea->SetPosition ( CVector2D ( vecPosition.fX, vecPosition.fY ) );
    pRadarArea->SetSize ( CVector2D ( 100.0f, 50.0f ) );
    */

    /*
    if ( pDebugPickup )
    {
        g_pCore->GetConsole()->Printf ( "Pickup State: %d", pDebugPickup->GetState() );
        g_pGame->GetPickups()->DisablePickupProcessing ( false );
    }
    */

    /*
    CVector vecTemp;
    g_pClientGame->GetLocalPlayer ()->GetModel ()->GetPosition ( vecTemp );
    vecTemp.fX += 8.0f;
    pTTTVehicle = g_pClientGame->GetVehicleManager ()->Add ( atoi ( szCmdLine ), 60 );
    pTTTVehicle->SetPosition ( vecTemp );
    */

    /*
    CClientVehicle* pVehicle = g_pClientGame->GetVehicleManager ()->Get ( atoi ( szCmdLine ) );
    if ( pVehicle )
    {
        pVehicle->SetHealth ( atof ( szCmdLine ) );
        g_pCore->GetConsole ()->Printf ( "Set %s", pVehicle->GetNamePointer () );
    }
    */

    /*
    if ( pRonkert )
    {
        pRonkert->GetModel ()->SetModel ( atoi ( szCmdLine ) );
    }
    */

    //g_pClientGame->GetManager ()->GetCamera ()->FadeIn ( 2.0f );
}

CClientPed* pTest = NULL;
CClientVehicle *v, *vnew;

void COMMAND_Debug3 ( const char *szCmdLine )
{
    _asm {
        pushad
        mov     ecx, 0x8CB6F8
        mov     eax, 0x4E7F80
        call    eax
        popad
    }
    g_pGame->GetAudio()->StopRadio();
    g_pGame->GetAudio()->StartRadio(1);
    return;
	vnew = g_pClientGame->GetLocalPlayer()->GetRealOccupiedVehicle();
	g_pCore->ChatPrintf("%x",false,(DWORD)v);
	return;

    /*
	RwV3d temp;
	temp.x = 1.0f;
	temp.y = 2.0f;
	temp.z = 1.0f;
	RwFrameScale ( frame, &temp, rwCOMBINEPRECONCAT );
	return;
    */

	// get the vehicle
	CClientVehicle* veh = g_pClientGame->GetLocalPlayer ()->GetOccupiedVehicle ();
	unsigned int vehid = veh->GetGameVehicle()->GetModelIndex();

	if ( veh ) {
		g_pCore->ChatPrintf("Swapping wheels for vehicle %u", vehid);

		// get the rw clump
		RpClump * pClumpVehicle = (RpClump*) veh->GetGameVehicle ()->GetRpClump ();

		// load our new model stream
		char szDFF[MAX_PATH] = {0};
		char szTXD[MAX_PATH] = {0};

		snprintf ( &szTXD[0], MAX_PATH-1, "C:\\Program Files\\Rockstar Games\\GTA San Andreas\\mtawheels.txd", szCmdLine );
		snprintf ( &szDFF[0], MAX_PATH-1, "C:\\Program Files\\Rockstar Games\\GTA San Andreas\\mtawheels.dff", szCmdLine );

		// read the txd and dff
		RwTexDictionary * pTXD = g_pGame->GetRenderWare ()->ReadTXD ( &szTXD[0] );
		g_pGame->GetRenderWare ()->ModelInfoImportTXD ( pTXD, vehid );

		RpClump * pClumpModel = g_pGame->GetRenderWare ()->ReadDFF ( &szDFF[0], 443 );

		// read all the new atomics
		RpAtomicContainer Atomics[128];
		unsigned int uiAtomics = g_pGame->GetRenderWare ()->LoadAtomics ( pClumpModel, &Atomics[0] );

		// replace wheels
		g_pGame->GetRenderWare ()->ReplaceWheels ( pClumpVehicle, &Atomics[0], uiAtomics, szCmdLine );

		g_pCore->ChatPrintf ( "Succesfully swapped wheels '%s'!", szCmdLine );
	} else {
		g_pCore->ChatPrintf ( "Invalid object.", false );
	}		

	/*
	RwError x;

	DWORD* pPool=(DWORD*)0x00A9B0C8;

	// get the vehicle
	CClientVehicle* veh = g_pClientGame->GetLocalPlayerModel ()->GetOccupiedVehicle ();
	unsigned int vehid = veh->GetGameVehicle()->GetModelIndex();

	if ( veh ) {
		g_pCore->ChatPrintf("Replacing vehicle %u", vehid);

		// get the rw clump
		RpClump * clump = (RpClump*) veh->GetGameVehicle ()->GetRpClump ();

		// load our new model stream
		char szPath[MAX_PATH] = {0};
		snprintf ( &szPath[0], MAX_PATH-1, "C:\\Program Files\\Rockstar Games\\GTA San Andreas\\%s.txd", szCmdLine );
		RwStream * texture = RwStreamOpen ( rwSTREAMFILENAME, rwSTREAMREAD, &szPath[0] );
		snprintf ( &szPath[0], MAX_PATH-1, "C:\\Program Files\\Rockstar Games\\GTA San Andreas\\%s.dff", szCmdLine );
		RwStream * stream = RwStreamOpen ( rwSTREAMFILENAME, rwSTREAMREAD, &szPath[0] );

		// check for errors
		if ( stream == NULL || texture == NULL ) {
			RwErrorGet ( &x );
			g_pCore->ChatPrintf ( "RwStreamOpen error: <%x> <%x>", x.pluginID, x.errorCode );
			return;
		}

		// find our txd chunk (dff loads textures, so correct loading order is: txd, dff)
		if ( RwStreamFindChunk ( texture, 0x16, NULL, NULL ) == false ) {
			g_pCore->ChatPrintf("Couldn't find chunk in TXD");
			return;
		}

		// find our dff chunk
		if ( RwStreamFindChunk ( stream, 0x10, NULL, NULL ) == false ) {
			g_pCore->ChatPrintf("Couldn't find chunk in DFF");
			return;
		}

		// ACHTUNG: crashy with small vehicles (kart)? same vehicles? packer (443) works ok

		// read the texture dictionary from our model (txd)
		RwTexDictionary * dict = RwTexDictionaryStreamRead ( texture );

		// get the texture dictionary (txd) ID from the modelinfo struct
		unsigned short texdictid = *((unsigned short*)(pPool[vehid] + 0xA));

		// call the GTA function that sets it to the currently selected txd (in rw)
		SetTextureDict(texdictid);

		// get the pointer to the currently selected txd
		RwTexDictionary * currenttxd = RwTexDictionaryGetCurrent ();

		// add all the textures from our model into the currently selected txd
		RwTexDictionaryForAllTextures ( dict, (void*)Test3, (void*)currenttxd );

		// rockstar's collision hack: set the global particle emitter to the modelinfo pointer of this vehicle
		RpPrtStdGlobalDataSetStreamEmbedded ( (void*)pPool[443] );

		// read the clump with all its extensions
		RpClump * newclump = RpClumpStreamRead ( stream );

		// clear the entire array
		clumpatomicsnum = 0;
		memset(&clumpatomics[0],NULL,sizeof(clumpatomics)*sizeof(RpAtomicContainer));

		// get the model stuff (seats)
		DWORD pModelStuff = *((DWORD*)(pPool[vehid] + 92));

		// get our dummies
		RwFrame *frame = RpGetFrame ( clump );
		RwFrame *newframe = RpGetFrame ( newclump );
		RwFrame *ped_frontseat = RwFrameFindFrame ( newframe, "ped_frontseat" );
		if ( !ped_frontseat ) {
			g_pCore->ChatPrintf ( "This model doesn't contain a 'ped_frontseat' entry!" );
			return;
		}

		// add our new atomics to the container array
		RpClumpForAllAtomics ( newclump, (void*)Test2, clump );

		// then replace the atomics from the vehicle with the container array ones
		RpClumpForAllAtomics ( clump, (void*)ReplaceAll, clump );
		
		// change the positions of the dummies and other misc visuals
		counter = 0;
		//RwFrameForAllObjects ( RwFrameFindFrame ( frame, "door_lf_dummy" ), ReplaceParts, "door_lf" );
		counter = 0;
		//RwFrameForAllObjects ( RwFrameFindFrame ( frame, "door_rf_dummy" ), ReplaceParts, "door_rf" );

		COPY_DUMMY ( frame, newframe, "door_lf_dummy" );
		COPY_DUMMY ( frame, newframe, "door_rf_dummy" );
		COPY_DUMMY ( frame, newframe, "windscreen_dummy" );
		COPY_DUMMY ( frame, newframe, "boot_dummy" );
		COPY_DUMMY ( frame, newframe, "bonnet_dummy" );
		COPY_DUMMY ( frame, newframe, "bump_front_dummy" );
		COPY_DUMMY ( frame, newframe, "bump_rear_dummy" );
		COPY_DUMMY ( frame, newframe, "engine" );
		COPY_DUMMY ( frame, newframe, "petrolcap" );

		// set the new front seat position for cars (*(modelinfo+92)+48 front seat, +60 back seat)
		CVector *vecFrontSeat = (CVector*)(pModelStuff + 48);
		vecFrontSeat->fX = ped_frontseat->modelling.pos.x; vecFrontSeat->fY = ped_frontseat->modelling.pos.y; vecFrontSeat->fZ = ped_frontseat->modelling.pos.z;

		g_pCore->ChatPrintf ( "Succesfully replaced vehicle %u!", vehid );

		// and close the streams
		RwStreamClose ( stream, NULL );
		RwStreamClose ( texture, NULL );
	} else {
		g_pCore->ChatPrintf ( "Invalid object." );
	}		
	return;

    CClientPlayer* pTarget = g_pClientGame->GetTargetedPlayer ();
    if ( pTarget )
    {
		pTarget->GetModel()->GetGamePlayer()->GetSprintEnergy ();		// REBUILD HACK
	} else {
		g_pCore->ChatEchoColor ( "Error: no player target found", 255, 168, 0 );
	}
	return;


	return;

	if ( g_pGame->GetAudio()->GetMissionAudioLoadingStatus(atoi(szCmdLine) == 1 ) )
		g_pGame->GetAudio()->PlayLoadedMissionAudio(atoi(szCmdLine));
	else
		g_pCore->GetConsole()->Printf("Audio not loaded yet!");
	return;

    CVector fff;
    g_pClientGame->GetManager ()->GetCamera ()->GetRotation ( fff );

    g_pCore->GetConsole ()->Printf ( "rot = %f, %f, %f", fff.fX, fff.fY, fff.fZ );

	*/

    /*CClientVehicle* pVehicle = new CDeathmatchVehicle ( g_pClientGame->GetManager (), 1, atoi(szCmdLine) );
    
    //CVector vecTemp;
    //g_pClientGame->GetPlayerManager ()->GetLocalPlayer ()->GetModel ()->GetPosition ( vecTemp );
    CClientVehicle* pLocalVehicle = g_pClientGame->GetPlayerManager ()->GetLocalPlayer ()->GetModel ()->GetOccupiedVehicle ();
    
    CClientVehicle * pTowedVehicle = pLocalVehicle->GetTowedVehicle();

    pTowedVehicle->BreakTowLink();
    
    return;*/
    /*
    if ( pTest )
        pTest->SetHealth ( atof ( szCmdLine ) );
        */

    /*
    float fStatValue = g_pGame->GetStats()->GetStatValue ( atoi ( szCmdLine ) );
    g_pCore->GetConsole()->Printf ( "Stat %d value: %f", atoi ( szCmdLine ), fStatValue );
    */

    /*
    CClientVehicle* pVehicle = g_pClientGame->GetLocalPlayerModel ()->GetOccupiedVehicle ();
    if ( pVehicle )
    {
        char szBuffer [256];
        strcpy ( szBuffer, szCmdLine );
        char* szDoor = strtok ( szBuffer, " " );
        char* szState = strtok ( NULL, " " );
        if ( szDoor && szState )
        {
            pVehicle->GetGameVehicle ()->GetDamageManager ()->SetLightStatus ( atoi ( szState ) );
            g_pCore->GetConsole ()->Print ( "done" );
        }
    }
*/

    //_asm int 3;
    /*
    CClientPlayer* pTestPlaya = new CDeathmatchPlayer ( g_pClientGame->GetManager (), 50 );
    pTestPlaya->SetNick ( "Ima1337" );
    pTestPlaya->SetIsDead ( false );
    CClientPed* pPlayerModel = pTestPlaya->LoadModel ( 50 );
    CVector vecTemp;
    g_pClientGame->GetLocalPlayerModel ()->GetPosition ( vecTemp );
    vecTemp.fX += 5.0f;
    pPlayerModel->SetPosition ( vecTemp );
    */
    //g_pClientGame->GetSpectator ()->Start ();
    //g_pClientGame->GetSpectator ()->Spectate ( pTestPlaya );

    /*
    CClientVehicle* pVehicle = g_pClientGame->GetLocalPlayerModel ()->GetClosestVehicleInRange ();
    if ( pVehicle )
    {
        
    }
    */
    

    /*
    FILE* pFile = fopen ( "ids.txt", "a+" );

    unsigned int i = 400;
    for ( ; i < 611; i++ )
    {
        if ( CClientVehicleManager::IsValidModel ( i ) )
        {
            CClientVehicle* pVehicle = new CDeathmatchVehicle ( g_pClientGame->GetManager (), i, 50 );
            fprintf ( pFile, "{\"%s\"}, ", pVehicle->GetNamePointer () );
            if ( ( i + 1 ) % 10 == 0 ) fprintf ( pFile, "\n" );
            delete pVehicle;
        }
        else
        {
            fprintf ( pFile, "{\"\"}, " );
            if ( ( i + 1 ) % 10 == 0 ) fprintf ( pFile, "\n" );
        }
    }
    */

    //fclose ( pFile );

    //g_pGame->GetAudio ()->PlayFrontEndSound ( atoi ( szCmdLine ) );

    //g_pClientGame->GetLocalPlayerModel ()->GetOccupiedVehicle ()->SetEngineOn ( false );

    //g_pClientGame->GetLocalPlayerModel ()->GetOccupiedVehicle ()->FuckCarCompletely ( false );

    /*
    CClientVehicle* pVehicle = g_pClientGame->GetVehicleManager ()->Add ( atoi(szCmdLine) );
    CVector vecTemp;
    g_pClientGame->GetPlayerManager ()->GetLocalPlayer ()->GetModel ()->GetPosition ( vecTemp );
    vecTemp.fX += 20.0f;
    pVehicle->SetPosition ( vecTemp );

    pVehicle->SetDoorsLocked ( false );
    */

    /*
    CClientPlayer* pPlayer = g_pClientGame->GetPlayerManager ()->Get ( 50 );
    if ( pPlayer )
    {
        delete pPlayer;
    }


    CVector vecPosition;
    g_pClientGame->GetLocalPlayerModel ()->GetPosition ( vecPosition );


    CClientRadarArea* pRadarArea = new CClientRadarArea ( g_pClientGame->GetManager (), 0xFEFE );
    pRadarArea->SetPosition ( CVector2D ( vecPosition.fX, vecPosition.fY ) );
    pRadarArea->SetColor ( 0x800000FF );
    pRadarArea->SetFlashing ( true );
    pRadarArea->SetSize ( CVector2D ( 100.0f, 200.0f ) );


    /*
    CClientVehicle* pVehicle = g_pClientGame->GetVehicleManager ()->Get ( atoi ( szCmdLine ) );
    if ( pVehicle )
    {
        pVehicle->Fix ();
        g_pCore->GetConsole ()->Printf ( "Fixed %s", pVehicle->GetNamePointer () );
    }
    */

    /*
    CClientVehicle* pVehicle = g_pClientGame->GetLocalPlayer ()->GetModel ()->GetOccupiedVehicle ();
    if ( pVehicle )
    {
        char szBuffer [256];
        strcpy ( szBuffer, szCmdLine );

        char* szX = strtok ( szBuffer, " " );
        char* szY = strtok ( NULL, " " );
        char* szZ = strtok ( NULL, " " );

        pVehicle->SetRotationRadians ( CVector ( atof ( szX ), atof ( szY ), atof ( szZ ) ) );

        CVector vecTemp;
        pVehicle->GetRotationRadians ( vecTemp );
        
        g_pCore->GetConsole ()->Printf ( "%f %f %f", vecTemp.fX, vecTemp.fY, vecTemp.fZ );
    }
    */


    /*
    CClientVehicle* pVehicle = g_pClientGame->GetVehicleManager ()->Add ( 522 );
    
    pVehicle->SetPosition ( vecTemp );
    */
    /*
    CVector vecTemp;
    g_pClientGame->GetPlayerManager ()->GetLocalPlayer ()->GetModel ()->GetPosition ( vecTemp );
    vecTemp.fX += 5.0f;
    CClientPlayer* pPlayer = new CDeathmatchPlayer ( g_pClientGame->GetManager (), 50 );
    CClientPed* pModel = pPlayer->LoadModel ( 0 );
    pModel->UnlockArmor ();
    CWeapon* pWeapon = pModel->GiveWeapon ( (eWeaponType)31, 5000 );
    CControllerState State;
    memset ( &State, 0, sizeof ( CControllerState ) );
    State.RightShoulder1 = 255;
    pModel->SetControllerState ( State );
    pWeapon->SetAsCurrentWeapon ();
    pModel->SetPosition ( vecTemp );

    //g_pClientGame->GetLocalPlayer ()->GetModel ()->GetIntoVehicle ( pTTTVehicle );

    // 

    //
    /*
    FILE* pFile = fopen ( "test.txt", "w+" );
    if ( pFile )
    {
        fprintf ( pFile, "%s", "unsigned char ucMaxPassengers [] = { " );

        bool bFirst = true;
        for ( int i = 400; i < 611; i++ )
        {
            

            if ( CClientVehicleManager::IsValidModel ( i ) )
            {
                CClientVehicle* pVehicle = g_pClientGame->GetManager ()->GetVehicleManager ()->Add ( i );
                if ( pVehicle )
                {
                    pVehicle->Create ();

                    if ( bFirst )
                    {
                        bFirst = false;
                        fprintf ( pFile, "%u", pVehicle->m_pVehicle->GetMaxPassengerCount () );
                    }
                    else
                    {
                        fprintf ( pFile, ", %u", pVehicle->m_pVehicle->GetMaxPassengerCount () );
                    }

                    delete pVehicle;
                }
            }
            else
            {
                if ( bFirst )
                {
                    bFirst = false;
                    fprintf ( pFile, "255" );
                }
                else
                {
                    fprintf ( pFile, ", 255" );
                }
            }
        }


        fprintf ( pFile, "%s", "};" );

        fclose ( pFile );
    }
    */

/*
        CClientVehicle* pVehicle = g_pClientGame->GetLocalPlayerModel ()->GetOccupiedVehicle ();
    if ( pVehicle )
    {
        try
        {
            pVehicle->AddUpgrade ( (eClientVehicleUpgrade)atoi ( szCmdLine ) );
        }
        catch ( ... )
        {
            g_pCore->ChatEcho ( "except" );
        }
    }
    */

    //g_pClientGame->GetPlayerList ()->ClearColumns ();

/*
    if ( pRonkert )
    {
        pRonkert->GetModel ()->SetHealth ( (float)atof ( szCmdLine ) );
    }
    */
}

CVector origin22;
CObject *o;

void COMMAND_Debug4 ( const char* szCmdLine )
{
	g_pCore->GetConsole ()->Printf ( "debug4" );
	g_pClientGame->StartPlayback ();
	return;

    if ( v )
    {
        CVehicle * pGameVehicle = v->GetGameVehicle ();
        //g_pClientGame->GetLocalPlayerModel ()->GetGamePlayer ()->AttachPedToEntity ( pGameVehicle, &CVector ( 0.0f, 0.0f, 3.0f ), 1, 1.0f, (eWeaponType)0 );
		
		//g_pClientGame->GetLocalPlayerModel ()->GetGamePlayer ()->AttachEntityToEntity ( pGameVehicle, &CVector ( 0.0f, 0.0f, 3.0f ), &CVector ( 0.0f, 0.0f, 3.0f ) );
		o->AttachEntityToEntity ( *g_pClientGame->GetLocalPlayer ()->GetGamePlayer (), CVector ( 0.0f, 0.0f, 3.0f ), CVector ( 0.0f, 0.0f, 3.0f ) );

		//pGameVehicle->AttachEntityToEntity ( vnew->GetGameVehicle(), &CVector ( 0.0f, 0.0f, 3.0f ), &CVector ( 0.0f, 0.0f, 0.0f ) );
    }
    else
    {
		o = g_pGame->GetPools ()->AddObject ( 1337 );

        v = new CDeathmatchVehicle ( g_pClientGame->GetManager (), g_pClientGame->GetUnoccupiedVehicleSync (), 1337, 400 );
        CVector vecPos;
        g_pClientGame->GetLocalPlayer ()->GetPosition ( vecPos );
        v->SetPosition ( CVector ( vecPos.fX + 5.0f, vecPos.fY, vecPos.fZ + 2.0f ) );

		CVector v;
		g_pClientGame->GetLocalPlayer()->GetPosition ( v );
		v.fY += 5;

		o->SetPosition ( &v );
	}

	return;

    /*
	CVehicle *vv = v->GetGameVehicle();
	RpClump * vclump = vv->GetRpClump ();
	//RpClump * pclump = g_pClientGame->GetLocalPlayerModel()->GetGamePlayer()->GetRpClump ();
	CClientVehicle *vnew = g_pClientGame->GetLocalPlayerModel()->GetRealOccupiedVehicle();
	RpClump * pclump = vnew->GetGameVehicle()->GetRpClump();

	RwFrame * vframe = RpGetFrame(vclump);
	RwFrame * pframe = RpGetFrame(pclump);
	//RwFrame * pframe = g_pClientGame->GetLocalPlayerModel()->GetGamePlayer()->GetFrameFromId(0);
	
	//RwFrame * test = RwFrameCreate ();
	//RpClumpSetFrame ( vclump, test );
	//RwFrameAddChild ( pframe, test );

//	RwFrameRemoveChild ( vframe->root );
	
	RwV3d temp;
	temp.x = temp.y = temp.z = 0;
	temp.x = 10.0f;

	//RwFrameTranslate ( vframe, &temp, rwCOMBINEPOSTCONCAT );
	RwFrameSetIdentity ( vframe );
	RwFrameSetIdentity ( pframe );
	//RwFrameAddChild ( pframe, vframe );

	return;
    */

	// get the vehicle
	CClientVehicle* veh = g_pClientGame->GetLocalPlayer ()->GetOccupiedVehicle ();
	unsigned int vehid = veh->GetGameVehicle()->GetModelIndex();

	if ( veh ) {
		g_pCore->ChatPrintf("Replacing vehicle %u", vehid);

		// get the rw clump
		RpClump * pClumpVehicle = (RpClump*) veh->GetGameVehicle ()->GetRpClump ();

		// load our new model stream
		char szDFF[MAX_PATH] = {0};
		char szTXD[MAX_PATH] = {0};

		snprintf ( &szTXD[0], MAX_PATH-1, "C:\\Program Files\\Rockstar Games\\GTA San Andreas\\%s.txd", szCmdLine );
		snprintf ( &szDFF[0], MAX_PATH-1, "C:\\Program Files\\Rockstar Games\\GTA San Andreas\\%s.dff", szCmdLine );

		// read the txd and dff
		RwTexDictionary * pTXD = g_pGame->GetRenderWare ()->ReadTXD ( &szTXD[0] );
		g_pGame->GetRenderWare ()->ModelInfoImportTXD ( pTXD, vehid );

		RpClump * pClumpModel = g_pGame->GetRenderWare ()->ReadDFF ( &szDFF[0], 443 );

		// read all the new atomics
		RpAtomicContainer Atomics[128];
		unsigned int uiAtomics = g_pGame->GetRenderWare ()->LoadAtomics ( pClumpModel, &Atomics[0] );

		// replace wheels (some custom mods use "wheel2")
		g_pGame->GetRenderWare ()->ReplaceWheels ( pClumpVehicle, &Atomics[0], uiAtomics, "wheel" );
		g_pGame->GetRenderWare ()->ReplaceWheels ( pClumpVehicle, &Atomics[0], uiAtomics, "wheel2" );

		// replace all atomics
		g_pGame->GetRenderWare ()->ReplaceAllAtomicsInClump ( pClumpVehicle, &Atomics[0], uiAtomics );

		// replace parts
		g_pCore->ChatPrintf ( "Loaded %u atomics.", uiAtomics );
		g_pGame->GetRenderWare ()->ReplacePartModels ( pClumpVehicle, &Atomics[0], uiAtomics, "door_lf" );
		g_pGame->GetRenderWare ()->ReplacePartModels ( pClumpVehicle, &Atomics[0], uiAtomics, "door_rf" );
		g_pGame->GetRenderWare ()->ReplacePartModels ( pClumpVehicle, &Atomics[0], uiAtomics, "door_lr" );
		g_pGame->GetRenderWare ()->ReplacePartModels ( pClumpVehicle, &Atomics[0], uiAtomics, "door_rr" );
		g_pGame->GetRenderWare ()->ReplacePartModels ( pClumpVehicle, &Atomics[0], uiAtomics, "bump_rear" );
		g_pGame->GetRenderWare ()->ReplacePartModels ( pClumpVehicle, &Atomics[0], uiAtomics, "bump_front" );
		g_pGame->GetRenderWare ()->ReplacePartModels ( pClumpVehicle, &Atomics[0], uiAtomics, "bonnet" );
		g_pGame->GetRenderWare ()->ReplacePartModels ( pClumpVehicle, &Atomics[0], uiAtomics, "boot" );
		g_pGame->GetRenderWare ()->ReplacePartModels ( pClumpVehicle, &Atomics[0], uiAtomics, "windscreen" );

		// reposition parts
		g_pGame->GetRenderWare ()->RepositionAtomic ( pClumpVehicle, pClumpModel, "door_lf_dummy" );
		g_pGame->GetRenderWare ()->RepositionAtomic ( pClumpVehicle, pClumpModel, "door_rf_dummy" );
		g_pGame->GetRenderWare ()->RepositionAtomic ( pClumpVehicle, pClumpModel, "door_lr_dummy" );
		g_pGame->GetRenderWare ()->RepositionAtomic ( pClumpVehicle, pClumpModel, "door_rr_dummy" );
		g_pGame->GetRenderWare ()->RepositionAtomic ( pClumpVehicle, pClumpModel, "bump_rear_dummy" );
		g_pGame->GetRenderWare ()->RepositionAtomic ( pClumpVehicle, pClumpModel, "bump_front_dummy" );
		g_pGame->GetRenderWare ()->RepositionAtomic ( pClumpVehicle, pClumpModel, "bonnet_dummy" );
		g_pGame->GetRenderWare ()->RepositionAtomic ( pClumpVehicle, pClumpModel, "boot_dummy" );
		g_pGame->GetRenderWare ()->RepositionAtomic ( pClumpVehicle, pClumpModel, "windscreen_dummy" );

		// reposition wheels
		g_pGame->GetRenderWare ()->RepositionAtomic ( pClumpVehicle, pClumpModel, "wheel_lf_dummy" );
		g_pGame->GetRenderWare ()->RepositionAtomic ( pClumpVehicle, pClumpModel, "wheel_rf_dummy" );
		g_pGame->GetRenderWare ()->RepositionAtomic ( pClumpVehicle, pClumpModel, "wheel_lb_dummy" );
		g_pGame->GetRenderWare ()->RepositionAtomic ( pClumpVehicle, pClumpModel, "wheel_rb_dummy" );

		// reposition the front seat
		g_pGame->GetRenderWare ()->PositionFrontSeat ( pClumpModel, vehid );

		g_pCore->ChatPrintf ( "Succesfully replaced vehicle %u!", vehid );
	} else {
		g_pCore->ChatPrintf ( "Invalid object.", false );
	}		

	return;

	/*
	RwError x;

	DWORD* pPool=(DWORD*)0x00A9B0C8;

	// get the vehicle
	CClientVehicle* veh = g_pClientGame->GetLocalPlayerModel ()->GetOccupiedVehicle ();
	unsigned int vehid = veh->GetGameVehicle()->GetModelIndex();

	if ( veh ) {
		g_pCore->ChatPrintf("Replacing %s in vehicle %u", szCmdLine, vehid);

		// get the rw clump
		RpClump * clump = (RpClump*) veh->GetGameVehicle ()->GetRpClump ();

		// load our new model stream
		RwStream * texture = RwStreamOpen ( rwSTREAMFILENAME, rwSTREAMREAD, "C:\\Program Files\\Rockstar Games\\GTA San Andreas\\mtawheels.txd" );
		RwStream * stream = RwStreamOpen ( rwSTREAMFILENAME, rwSTREAMREAD, "C:\\Program Files\\Rockstar Games\\GTA San Andreas\\mtawheels.dff" );

		// check for errors
		if ( stream == NULL || texture == NULL ) {
			RwErrorGet ( &x );
			g_pCore->ChatPrintf ( "RwStreamOpen error: <%x> <%x>", x.pluginID, x.errorCode );
			return;
		}

		// find our txd chunk (dff loads textures, so correct loading order is: txd, dff)
		if ( RwStreamFindChunk ( texture, 0x16, NULL, NULL ) == false ) {
			g_pCore->ChatPrintf("Couldn't find chunk in TXD");
			return;
		}

		// find our dff chunk
		if ( RwStreamFindChunk ( stream, 0x10, NULL, NULL ) == false ) {
			g_pCore->ChatPrintf("Couldn't find chunk in DFF");
			return;
		}

		// ACHTUNG: crashy with small vehicles (kart)? same vehicles? packer (443) works ok

		// read the texture dictionary from our model (txd)
		RwTexDictionary * dict = RwTexDictionaryStreamRead ( texture );

		// get the texture dictionary (txd) ID from the modelinfo struct
		unsigned short texdictid = *((unsigned short*)(pPool[vehid] + 0xA));

		// call the GTA function that sets it to the currently selected txd (in rw)
		SetTextureDict(texdictid);

		// get the pointer to the currently selected txd
		RwTexDictionary * currenttxd = RwTexDictionaryGetCurrent ();

		// add all the textures from our model into the currently selected txd
		RwTexDictionaryForAllTextures ( dict, (void*)Test3, (void*)currenttxd );

		// rockstar's collision hack: set the global particle emitter to the modelinfo pointer of this vehicle
		RpPrtStdGlobalDataSetStreamEmbedded ( (void*)pPool[443] );

		// read the clump with all its extensions
		RpClump * newclump = RpClumpStreamRead ( stream );

		// clear the entire array
		clumpatomicsnum = 0;
		memset(&clumpatomics[0],NULL,sizeof(clumpatomics)*sizeof(RpAtomicContainer));

		// add our new atomics to the container array
		RpClumpForAllAtomics ( newclump, (void*)Test2, clump );

		// then replace the atomics from the vehicle with the container array ones
		TestStruct s;
		memset(&s,0,sizeof(TestStruct));
		s.clump = clump;
		strncpy(&s.name[0],szCmdLine,16);
		RpClumpForAllAtomics ( clump, (void*)ReplaceWheels, &s );

		g_pCore->ChatPrintf ( "Replaced %s in vehicle %u.", szCmdLine, vehid );

		// and close the streams
		RwStreamClose ( stream, NULL );
		RwStreamClose ( texture, NULL );
	} else {
		g_pCore->ChatPrintf ( "Invalid object." );
	}		
	return;

	*/

	/*g_pGame->GetPlayerInfo ()->StreamParachuteWeapon ( true );
	return;*/

	CVehicle* pVehicle = g_pClientGame->GetLocalPlayer ()->GetOccupiedVehicle()->GetGameVehicle ();
	pVehicle->SetStatic ( true );
	//pVehicle->SetEntityStatus ( STATUS_TRAIN_NOT_MOVING );
	return;
    //g_pCore->ForceCursorVisible ( atoi ( szCmdLine ) == 1 );
    //return;
    /*
    static bool bPlay = false;
    if ( bPlay )
    {
        g_pClientGame->StartPlayback ();
    }
    else
    {
        g_pClientGame->EnablePacketRecorder ( "mta\\mta.rec" );
        bPlay = true;
    }
    return;
    */

    /*
    CMatrix fff;
    g_pClientGame->GetManager ()->GetCamera ()->GetMatrix ( fff );

    g_pCore->GetConsole ()->Printf ( "Pos: %f, %f, %f", fff.vPos.fX, fff.vPos.fY, fff.vPos.fZ );
    g_pCore->GetConsole ()->Printf ( "Dir: %f, %f, %f", fff.vDirection.fX, fff.vDirection.fY, fff.vDirection.fZ );
    g_pCore->GetConsole ()->Printf ( "Was: %f, %f, %f", fff.vWas.fX, fff.vWas.fY, fff.vWas.fZ );
    g_pCore->GetConsole ()->Printf ( "Rll: %f, %f, %f", fff.vRoll.fX, fff.vRoll.fY, fff.vRoll.fZ );
    */

    /*
    static CDeathmatchVehicle* pVehicle = NULL;
    if ( pVehicle )
    {
        CVehicle * pGameVehicle = pVehicle->GetGameVehicle ();
        g_pClientGame->GetLocalPlayerModel ()->GetGamePlayer ()->AttachPedToEntity ( pGameVehicle, &CVector ( 0.0f, 0.0f, 3.0f ), 1, 1.0f, (eWeaponType)28 );
    }
    else
    {
        pVehicle = new CDeathmatchVehicle ( g_pClientGame->GetManager (), g_pClientGame->GetUnoccupiedVehicleSync (), 1337, 400 );
        CVector vecPos;
        g_pClientGame->GetLocalPlayerModel ()->GetPosition ( vecPos );
        pVehicle->SetPosition ( CVector ( vecPos.fX + 5.0f, vecPos.fY, vecPos.fZ + 2.0f ) );
    }
    */

    //return;
    /*static CDeathmatchVehicle* pVehicle = NULL;
    if ( pVehicle )
    {
        CVehicle * pGameVehicle = pVehicle->GetGameVehicle ();
        //pGameVehicle->SetTowLink ( g_pClientGame->GetLocalPlayerModel ()->GetOccupiedVehicle ()->GetGameVehicle () );
        g_pClientGame->GetLocalPlayerModel ()->GetOccupiedVehicle ()->GetGameVehicle ()->SetTowLink ( pGameVehicle );
    }
    else
    {
        pVehicle = new CDeathmatchVehicle ( g_pClientGame->GetManager (), g_pClientGame->GetUnoccupiedVehicleSync (), 1337, 435 );
        CVector vecPos;
        g_pClientGame->GetLocalPlayerModel ()->GetPosition ( vecPos );
        pVehicle->SetPosition ( CVector ( vecPos.fX + 5.0f, vecPos.fY, vecPos.fZ + 2.0f ) );
    }*/

    //return;

    //g_pClientGame->GetManager ()->GetVehicleStreamer ()->SetDimension ( atoi ( szCmdLine ) );
    //return;
    /*
    {
        g_pGame->GetWorld()->SetCurrentArea ( atoi(szCmdLine) );
        g_pClientGame->GetLocalPlayerModel ()->SetAreaCode ( atoi(szCmdLine) );
        g_pClientGame->GetLocalPlayerModel ()->SetPosition ( CVector ( 2233.91f, 1714.73f, 1011.38f ) );
    }*/

    //return;
    
    /*
    if ( atoi(szCmdLine) == 1 )
        g_pGame->GetHud()->DisableRadar ( true );
    else
        g_pGame->GetHud()->DisableRadar ( false );
    return;
    if ( strcmp ( szCmdLine, "" ) == 0 )
    {
        g_pCore->SetDebugVisible ( false );
    }
    else
    {
        g_pCore->SetDebugVisible ( true );
        g_pCore->DebugEcho ( szCmdLine );
    }
    return;
    */

        /*
    static CDeathmatchVehicle* pVehicle = NULL;
    if ( pVehicle )
    {
        CVehicle * pGameVehicle = pVehicle->GetGameVehicle ();
        g_pClientGame->GetLocalPlayerModel ()->GetGamePlayer ()->AttachPedToEntity ( pGameVehicle, &CVector ( 0.0f, 0.0f, 3.0f ), 1, 1.0f, (eWeaponType)28 );
    }
    else
    {
        pVehicle = new CDeathmatchVehicle ( g_pClientGame->GetManager (), g_pClientGame->GetUnoccupiedVehicleSync (), 1337, 400 );
        CVector vecPos;
        g_pClientGame->GetLocalPlayerModel ()->GetPosition ( vecPos );
        pVehicle->SetPosition ( CVector ( vecPos.fX + 5.0f, vecPos.fY, vecPos.fZ + 2.0f ) );
    }
    */

    //return;

    /*if ( atoi(szCmdLine) == 1 )
        g_pMultiplayer->HideRadar ( true );
    else
        g_pMultiplayer->HideRadar ( false );

    g_pCore->ChatEcho ("radar messed with!");
    return;

    CClientPed * pLocalPlayer = g_pClientGame->GetLocalPlayerModel ();
    if ( pLocalPlayer )
    {
        CClientVehicle* pVehicle = pLocalPlayer->GetOccupiedVehicle ();
        if ( pVehicle )
        {
            if ( pVehicle->GetWinchType () != WINCH_BIG_MAGNET )
            {
                if ( pVehicle->SetWinchType ( WINCH_BIG_MAGNET ) )
                {
                    CClientEntity* pAttachedEntity = static_cast < CClientEntity* > (
                    new CDeathmatchVehicle ( g_pClientGame->GetManager (), g_pClientGame->GetUnoccupiedVehicleSync (),
                                             1337, 522 ) );

                    pVehicle->PickupEntityWithWinch ( pAttachedEntity );
                }
            }
        }
    }
    return;*/
    
    /*
    CClientPed * pLocalPlayer = g_pClientGame->GetLocalPlayerModel ();
    pLocalPlayer->SetHasJetPack ( !pLocalPlayer->HasJetPack () );
    return;

    for ( int i = FIRE; i <= GROUP_CONTROL_BACK; i++ )
    {
        int iKey = g_pGame->GetControllerConfigManager()->GetControllerKeyAssociatedWithAction ( (eControllerAction)i, KEYBOARD );
        g_pCore->GetConsole()->Printf ( "Action: %d   Key: %d", i, iKey );
    }
    return;

    CClientVehicle* pVehicle = g_pClientGame->GetLocalPlayerModel ()->GetOccupiedVehicle ();
    if ( pVehicle )
    {
        g_pGame->GetModelInfo ( 1031 )->RequestVehicleUpgrade();
        g_pGame->GetModelInfo ( 1031 )->LoadAllRequestedModels();
        pVehicle->GetUpgrades ()->ForceAddUpgrade ( 1031 );
    }
    return;*/


    /*CClientVehicle* pVehicle = g_pClientGame->GetPlayerManager ()->GetLocalPlayer ()->GetModel ()->GetOccupiedVehicle ();
    if ( pVehicle )
    {
        pVehicle->SetPaintjob ( atoi ( szCmdLine ) );
    }
    
    return;
    */

    /*
    CVector vecPos;
    DWORD dwFunc = 0x40ED80;
    g_pClientGame->GetLocalPlayerModel ()->GetPosition ( vecPos );
    _asm
    {
        lea     eax, vecPos
        push    eax
        call    dwFunc
        add     esp, 4 
    }
    return;
    */

	/*
    FILE * file = fopen ( "c:\\output.csv", "w" );
    for ( int i = 400; i < 589; i++ )
    {
        CModelInfo * modelInfo = g_pGame->GetModelInfo(i);
        if ( modelInfo->IsCar() || modelInfo->IsHeli() || modelInfo->IsTrailer() || modelInfo->IsMonsterTruck() || modelInfo->IsQuadBike() ) 
        {
            fprintf ( file, "%d,%s,", i, modelInfo->GetNameIfVehicle() );
            for ( int j = 0; j <= 16; j++ )
            {
                if ( modelInfo->IsUpgradeAvailable ( (eVehicleUpgradePosn)j ) )
                    fprintf ( file, "1," );
                else
                    fprintf ( file, "0," );
            }
            fprintf ( file, "\n" );
        }
    }
    fclose ( file );
	*/

    /*
    CVector vec;
    g_pClientGame->GetLocalPlayerModel ()->GetPosition ( vec );
    vec.fX += 10.0f;

    CClientVehicle* pVehicle = new CDeathmatchVehicle ( g_pClientGame->GetManager (), 538, 50 );
    CClientVehicle* pFirst = pVehicle;
    pVehicle->SetPosition ( vec );
    pVehicle->SetStreamingEnabled ( false );
    */

    //for ( int i = 0; i < 31; i++ )
    {
        //CClientVehicle* pNew = new CDeathmatchVehicle ( g_pClientGame->GetManager (), 590, /*i +*/ 51 );
        /*
        pNew->SetPosition ( vec );
        pNew->SetStreamingEnabled ( false );
        pVehicle->SetNextTrainCarriage ( pNew );
        pVehicle = pNew;
        */
    }


    //g_pClientGame->GetLocalPlayerModel ()->WarpIntoVehicle ( pFirst );
   

    /*
    CClientVehicle* pVehicle = g_pClientGame->GetLocalPlayerModel ()->GetOccupiedVehicle ();
    if ( pVehicle )
    {
        try
        {
            pVehicle->AddUpgrade ( (eClientVehicleUpgrade)atoi ( szCmdLine ) );
        }
        catch ( ... )
        {
            g_pCore->ChatEcho ( "except" );
        }
    }
    */

    /*
    FILE* pFile = fopen ( "C:/compatible.txt", "w+" );

    for ( int i = 404; i <= 611; i++ )
    {
        if ( CClientVehicleManager::IsValidModel ( i ) )
        {
            CClientVehicle* pVehicle = new CDeathmatchVehicle ( g_pClientGame->GetManager (), i, 50 );
            if ( pVehicle )
            {
                pVehicle->SetStreamingEnabled ( false );
                // Try all upgrades
                for ( int iUp = 1000; iUp <= 1193; iUp++ )
                {
                    try
                    {
                        /*
                        pVehicle->AddUpgrade ( (eClientVehicleUpgrade)iUp );

                        fprintf ( pFile, "%i:%i;%s\n", i, iUp, pVehicle->GetNamePointer () );
                        fflush ( pFile );
                        */
    /*
                    }
                    catch ( ... )
                    {}
                }

                delete pVehicle;
            }
        }
    }

    fclose ( pFile );
    */

    /*
    CClientVehicle* pVehicle = g_pClientGame->GetLocalPlayerModel ()->GetOccupiedVehicle ();
    if ( pVehicle )
    {
        try
        {
            pVehicle->AddUpgrade ( (eClientVehicleUpgrade)atoi ( szCmdLine ) );
        }
        catch ( ... )
        {
            g_pCore->ChatEcho ( "except" );
        }
    }
    */

    /*
    static int test = 0;
    CClientRadarMarker* pMarker = g_pClientGame->GetManager ()->GetRadarMarkerManager ()->AddMarker ( test++ );
    CVector vecTemp;
    g_pClientGame->GetLocalPlayerModel ()->GetPosition ( vecTemp );
    pMarker->SetSprite ( atoi ( szCmdLine ) );
    pMarker->SetScale ( 25 );
    pMarker->SetPosition ( vecTemp );
    */


    /*
    float fHealth = g_pClientGame->GetLocalPlayerModel ()->GetHealth ();
    
    */


    //g_pGame->GetAudio ()->PlayBeatTrack ( atoi ( szCmdLine ) );

    //g_pClientGame->GetLocalPlayerModel ()->m_pPlayerPed->SetGogglesState ( atoi ( szCmdLine ) == 1 );
    //pPlayerModel->Duck ( atoi(szCmdLine) );
    //return;
    //*/

    /*
    CClientVehicle* pVehicle = pPlayerModel->GetClosestVehicleInRange ();
    if ( pPlayerModel->IsInVehicle() )
    {
        pPlayerModel->GetOutOfVehicle ();
    }

    if ( pVehicle )
    {
        if ( pPlayerModel->IsInVehicle() )
            pPlayerModel->GetOutOfVehicle ();
        else
            pPlayerModel->GetIntoVehicleTest ( pVehicle, atoi(szCmdLine) ); 
    }
    else
    {
        g_pCore->GetConsole()->Echo ( "No vehicle in range!");
    }
    /*
    *(int *)0x8a5a84 = 127;

*/
    //CVector vecTemp;
    //g_pClientGame->GetLocalPlayer ()->GetModel ()->GetPosition ( origin22 );       

    /*
    unsigned int uiCount = 0;
    for ( int i = 400; i < 400 + 120; i++ )
    {
        if ( CClientVehicleManager::IsValidModel ( i ) )
        {
            CClientVehicle* pVehicle = g_pClientGame->GetManager ()->GetVehicleManager ()->Add ( i );
            vecTemp.fX += 5.0f;
            pVehicle->SetPosition ( vecTemp );
            ++uiCount;
        }
    }*/

    /*
    CClientVehicle* pVehicle = g_pClientGame->GetVehicleManager ()->Get ( atoi ( szCmdLine ) );
    if ( pVehicle )
    {
        g_pCore->GetConsole ()->Printf ( "%s visible = %u", pVehicle->GetNamePointer (), pVehicle->IsEngineBroken () );
    }
    */


    CVector vecLocalPos;
    g_pClientGame->GetPlayerManager ()->GetLocalPlayer()->GetPosition ( vecLocalPos );
    //static int a = 50;

    CClientVehicle* pV = new CDeathmatchVehicle ( g_pClientGame->GetManager (), NULL, 1, 587 );
    vecLocalPos.fZ += 5.0f;
    pV->SetPosition ( vecLocalPos );

	CClientPlayer* pPlayer = new CClientPlayer ( g_pClientGame->GetManager (), 55, false );
    pPlayer->SetModel ( 0 );
    pPlayer->SetPosition ( vecLocalPos );

    CWeapon * pWeapon = pPlayer->GiveWeapon ( (eWeaponType)29, 1000 );
    if ( pWeapon )
     pWeapon->SetAsCurrentWeapon();

    CControllerState A;
    memset ( &A, 0, sizeof ( CControllerState ) );
    A.ButtonCircle = 255;
    pPlayer->SetControllerState ( A );

	//pPlayer->WarpIntoVehicle ( pV );
    pPlayer->GetIntoVehicle ( pV, 1 );

	return;
    
    /*
    //CClientVehicle* pVehicle = g_pClientGame->GetVehicleManager ()->Get ( 65533 );
    //CVector vecTemp;
    //pVehicle->GetPosition ( vecTemp );


    CWeapon * pWeapon = pModel->GiveWeapon ( (eWeaponType)30, 9999 );
    if ( pWeapon )
     pWeapon->SetAsCurrentWeapon();

    CControllerState cs;
    //pModel->GetControllerState ( cs );
    cs.ButtonSquare = 128;

    static bool i = true;
    pModel->SetControllerState ( cs );

//005FBB70   C2 0400          RETN 4
//005FB9C0   C2 0400          RETN 4



    //pModel->WarpIntoVehicle ( pVehicle );
    //pModel->GetIntoVehicle ( pVehicle, 0 );

    /*
    float* f = (float*)g_pClientGame->GetLocalPlayer ()->GetModel ()->GetGameBaseAddress ();
    f = (float*)((unsigned int)f + 1352);
    *f = 50.0f;
    */

    //g_pGame->GetPlayerInfo ()->SetPlayerMoney ( atoi ( szCmdLine ) );

/*
    char szBuffer [256];
    strncpy ( szBuffer, szCmdLine, 256 );
    char* szWidth = strtok ( szBuffer, " " );
    char* szText = strtok ( NULL, "\0" );
    if ( !szWidth || !szText ) return;

    g_pClientGame->GetPlayerList ()->AddColumn ( szText, (float)atof ( szWidth ) );
    */

/*
    CClientPlayer* pHehe = new CDeathmatchPlayer ( g_pClientGame->GetManager (), 50, false );
    pTest = pHehe->LoadModel ( 120 );
    pHehe->SetNick ( "TestPlayer" );
    */


/*
    CVector vecTemp;
    g_pClientGame->GetLocalPlayerModel ()->GetPosition ( vecTemp );
    vecTemp.fX += 2.0f;
    pTest->SetPosition ( vecTemp );
    pTest->SetTargetPosition ( 0, vecTemp );
    */
    
/*
    CVector vecTemp;
    g_pClientGame->GetLocalPlayerModel ()->GetPosition ( vecTemp );
    vecTemp.fX += 2.0f;
    pTestVehicle = new CDeathmatchVehicle ( g_pClientGame->GetManager (), 420, 55 );
    pTestVehicle->SetPosition ( vecTemp );
    */
}

#endif

/*

#define COPY_DUMMY(dst,src,name) RwFrameCopyMatrix(RwFrameFindFrame(dst,name),RwFrameFindFrame(src,name))
RpAtomicContainer clumpatomics[255];
unsigned int clumpatomicsnum = 0;
unsigned int counter = 0;

struct TestStruct {
	RpClump * clump;
	char name[16];
};

static void Test5(RwFrame *f) {
	RwFrame * ret = f->child;
	while ( ret != NULL ) {
		// recurse into the child
		if ( ret->child != NULL )
			Test5 ( ret );

		if(strncmp(&ret->szName[0],"door_lf_dummy",16)==0) {
			g_pCore->ChatPrintf("%f %f %f",ret->modelling.pos.x,ret->modelling.pos.y,ret->modelling.pos.z);
			ret->modelling.pos.x += 4.0f;
			g_pCore->ChatPrintf("%f %f %f",ret->modelling.pos.x,ret->modelling.pos.y,ret->modelling.pos.z);
		}
		g_pCore->ChatPrintf("'%s' (object '%u')(root: '%s')",&ret->szName[0],ret->object.subType,&ret->root->szName[0]);
		ret = ret->next;
	}
}

static RpAtomic* ReplaceSpecific (RpAtomic * atomic, void * data) {
	TestStruct * s = (TestStruct*)data;
	RpClump * d = s->clump;
	RwFrame * f = RpGetFrame ( atomic );

	// replace atomic
	if ( strncmp ( &f->szName[0], &s->name[0], 16 ) == 0 ) {
		// find a replacement atomic
		for ( int i = 0; i < clumpatomicsnum; i++ ) {
			if ( strncmp ( &clumpatomics[i].szName[0], &f->szName[0], 16 ) == 0 ) {
				RpAtomicSetFrame ( clumpatomics[i].atomic, f );
				clumpatomics[i].atomic->renderCallBack = atomic->renderCallBack;
				clumpatomics[i].atomic->renderFrame = atomic->renderFrame;
				clumpatomics[i].atomic->pipeline = atomic->pipeline;
				clumpatomics[i].atomic->interpolator = atomic->interpolator;
				clumpatomics[i].atomic->repEntry = atomic->repEntry;
				RpClumpAddAtomic ( d, clumpatomics[i].atomic );
				g_pCore->ChatPrintf("Replaced %s",&clumpatomics[i].szName[0]);
			}
		}
		// delete the current atomic
		RpClumpRemoveAtomic ( d, atomic );
	}
	return atomic;
}

static RpAtomic* Test1 (RpAtomic * atomic, void * data) {
	RpClump * d = ( RpClump* ) data;
	RwFrame * f = RpGetFrame ( atomic );

	// replace all wheels
	if ( strncmp ( &f->szName[0], "wheel_lf_dummy", 16 ) == 0 || strncmp ( &f->szName[0], "wheel_rf_dummy", 16 ) == 0 || 
		 strncmp ( &f->szName[0], "wheel_lm_dummy", 16 ) == 0 || strncmp ( &f->szName[0], "wheel_rm_dummy", 16 ) == 0 || 
		 strncmp ( &f->szName[0], "wheel_lb_dummy", 16 ) == 0 || strncmp ( &f->szName[0], "wheel_rb_dummy", 16 ) == 0 ) {
		// find a replacement atomic
		for ( int i = 0; i < clumpatomicsnum; i++ ) {
			// see if it's name is equal to "wheel"
			if ( strncmp ( &clumpatomics[i].szName[0], "wheel", 16 ) == 0 ) {
				// clone our wheel atomic and add it to the clump
				RpAtomic * newatomic = RpAtomicClone ( clumpatomics[i].atomic );
				RpAtomicSetFrame ( newatomic, f );
				RpClumpAddAtomic ( d, newatomic );
			}
		}
		// delete the current atomic
		RpClumpRemoveAtomic ( d, atomic );
	// REPLACE CHASSIS
	} else if ( strncmp ( &f->szName[0], "chassis", 16 ) == 0 || strncmp ( &f->szName[0], "chassis_vlo", 16 ) == 0 ||
		strncmp ( &f->szName[0], "boat_vlo", 16 ) == 0 || strncmp ( &f->szName[0], "boat_hi", 16 ) == 0 ) {
		// find a new suitable atomic
		for ( int i = 0; i < clumpatomicsnum; i++ ) {
			if ( strncmp(&clumpatomics[i].szName[0],&f->szName[0], 16) == 0 ) {
				RpAtomicSetFrame ( clumpatomics[i].atomic, f );
				clumpatomics[i].atomic->renderCallBack = atomic->renderCallBack;
				clumpatomics[i].atomic->renderFrame = atomic->renderFrame;
				clumpatomics[i].atomic->pipeline = atomic->pipeline;
				clumpatomics[i].atomic->interpolator = atomic->interpolator;
				clumpatomics[i].atomic->repEntry = atomic->repEntry;
				RpClumpAddAtomic ( d, clumpatomics[i].atomic );
				//g_pCore->ChatPrintf ( "Replaced '%s'", &f->szName[0] );
			}
		}
		// delete the current atomic
		RpClumpRemoveAtomic ( d, atomic );
	}

	return atomic;
}

static RpAtomic* ReplaceWheels (RpAtomic * atomic, void * data) {
	TestStruct * s = (TestStruct*)data;
	RpClump * d = s->clump;
	RwFrame * f = RpGetFrame ( atomic );

	// replace all wheels
	if ( strncmp ( &f->szName[0], "wheel_lf_dummy", 16 ) == 0 || strncmp ( &f->szName[0], "wheel_rf_dummy", 16 ) == 0 || 
		 strncmp ( &f->szName[0], "wheel_lm_dummy", 16 ) == 0 || strncmp ( &f->szName[0], "wheel_rm_dummy", 16 ) == 0 || 
		 strncmp ( &f->szName[0], "wheel_lb_dummy", 16 ) == 0 || strncmp ( &f->szName[0], "wheel_rb_dummy", 16 ) == 0 ) {
		// find a replacement atomic
		for ( int i = 0; i < clumpatomicsnum; i++ ) {
			// see if it's name is equal to the specified wheel
			if ( strncmp ( &clumpatomics[i].szName[0], &s->name[0], 16 ) == 0 ) {
				// clone our wheel atomic and add it to the clump
				RpAtomic * newatomic = RpAtomicClone ( clumpatomics[i].atomic );
				RpAtomicSetFrame ( newatomic, f );
				RpClumpAddAtomic ( d, newatomic );
			}
		}
		// delete the current atomic
		RpClumpRemoveAtomic ( d, atomic );
	}

	return atomic;
}

static RpAtomic* ReplaceAll (RpAtomic * atomic, void * data) {
	RpClump * d = ( RpClump* ) data;
	RwFrame * f = RpGetFrame ( atomic );

	if ( strncmp ( &f->szName[0], "wheel_lf_dummy", 16 ) == 0 || strncmp ( &f->szName[0], "wheel_rf_dummy", 16 ) == 0 || 
		 strncmp ( &f->szName[0], "wheel_lm_dummy", 16 ) == 0 || strncmp ( &f->szName[0], "wheel_rm_dummy", 16 ) == 0 || 
		 strncmp ( &f->szName[0], "wheel_lb_dummy", 16 ) == 0 || strncmp ( &f->szName[0], "wheel_rb_dummy", 16 ) == 0 ) {
		// see if we can replace any wheels
		for ( int i = 0; i < clumpatomicsnum; i++ ) {
			// see if it's name is equal to "wheel"
			if ( strncmp ( &clumpatomics[i].szName[0], "wheel", 16 ) == 0 ) {
				// clone our wheel atomic and add it to the clump
				RpAtomic * newatomic = RpAtomicClone ( clumpatomics[i].atomic );
				RpAtomicSetFrame ( newatomic, f );
				RpClumpAddAtomic ( d, newatomic );
			}
		}
		// delete the current atomic
		RpClumpRemoveAtomic ( d, atomic );
	} else {
		// find a replacement atomic for everything else
		//g_pCore->ChatPrintf("Searching for '%s'", &f->szName[0]);
		for ( int i = 0; i < clumpatomicsnum; i++ ) {
			if ( strncmp ( &clumpatomics[i].szName[0], &f->szName[0], 16 ) == 0 ) {
				RpAtomicSetFrame ( clumpatomics[i].atomic, f );
				clumpatomics[i].atomic->renderCallBack = atomic->renderCallBack;
				clumpatomics[i].atomic->renderFrame = atomic->renderFrame;
				clumpatomics[i].atomic->pipeline = atomic->pipeline;
				clumpatomics[i].atomic->interpolator = atomic->interpolator;
				clumpatomics[i].atomic->repEntry = atomic->repEntry;
				RpClumpAddAtomic ( d, clumpatomics[i].atomic );
			}
		}
	}
	return atomic;
}

static RpAtomic* Test2 (RpAtomic * atomic, void * data) {
	// adds all the atomics from the loaded clump into our container
	//RpClump * d = (RpClump*)data;
	//RwFrame * f = RpGetFrame(d);
	RwFrame * g = RpGetFrame(atomic);

	clumpatomics[clumpatomicsnum].atomic = atomic;
	strncpy(&clumpatomics[clumpatomicsnum].szName[0],&g->szName[0], 16);
	clumpatomicsnum++;

	//g_pCore->ChatPrintf("Adding '%s' (%u)", &g->szName[0], clumpatomicsnum);

	//RpAtomicSetFrame(atomic, f);		// seems to crash with 1-atomic objects?
	//RpClumpAddAtomic(d, atomic);

	return atomic;
}

static RwTexture* Test3 (RwTexture * texture, void * data) {
	RwTexDictionary * t = (RwTexDictionary*)data;

	// only add the texture if it isn't already inside the txd
	if ( RwTexDictionaryFindNamedTexture ( t, texture->name ) == NULL )
		RwTexDictionaryAddTexture ( t, texture );

	//g_pCore->ChatPrintf("Adding texture %s to %x.", treal->name, t);

	return texture;
}

static RwTexture* Test4 (RwTexture * texture, void * data) {
	return texture;
}
*/
