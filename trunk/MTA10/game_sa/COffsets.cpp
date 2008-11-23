/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/COffsets.cpp
*  PURPOSE:		EXE-dependant game variable offsets
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void COffsets::Initialize10EU ( void )
{
    InitializeCommon10 ();

    CSettingsSA::FUNC_GetCurrentVideoMode = 0x7F2D60;
    CSettingsSA::FUNC_GetNumVideoModes = 0x7F2D00;
    CSettingsSA::FUNC_GetVideoModeInfo = 0x7F2D30;

    CEntitySA::FUNC_RwFrameGetLTM = 0x7F09D0;
    CCameraSA::FUNC_RwFrameGetLTM = 0x7F09D0;
}


void COffsets::Initialize10US ( void )
{
    InitializeCommon10 ();

    CSettingsSA::FUNC_GetCurrentVideoMode = 0x7F2D20;
    CSettingsSA::FUNC_GetNumVideoModes = 0x7F2CC0;
    CSettingsSA::FUNC_GetVideoModeInfo = 0x7F2CF0;

    CEntitySA::FUNC_RwFrameGetLTM = 0x7F0990;
    CCameraSA::FUNC_RwFrameGetLTM = 0x7F0990;
}


void COffsets::Initialize11 ( void )
{
    CGameSA::VAR_SystemTime =               (unsigned long*)    0xB7CB84;
    CGameSA::VAR_IsAtMenu =                 (unsigned long*)    0xBA677B;
    CGameSA::VAR_IsGameLoaded =             (unsigned long*)    0x86969C;
    CGameSA::VAR_GamePaused =               (bool*)             0xB7CB49;
    CGameSA::VAR_IsForegroundWindow =       (bool*)             0x8D621C;
    CGameSA::VAR_SystemState =              (unsigned long*)    0xC8FC80;   // 1.01
    CGameSA::VAR_StartGame =                (void*)             0x869641;
    CGameSA::VAR_IsNastyGame =              (bool*)             0x68DD68;
    CGameSA::VAR_TimeScale =                (float*)            0xB7CB64;
    CGameSA::VAR_FPS =                      (float*)            0xB7CB50;
    CGameSA::VAR_OldTimeStep =              (float*)            0xB7CB54;
    CGameSA::VAR_TimeStep =                 (float*)            0xB7CB5C;
    CGameSA::VAR_Framelimiter =             (unsigned long*)    0xC1704C;

    CWeatherSA::VAR_CurrentWeather =        (unsigned char*)    0xC81318;
    CWeatherSA::VAR_CurrentWeather_b =      (unsigned char*)    0xC81320;
    CWeatherSA::VAR_CurrentWeather_c =      (unsigned char*)    0xC8131C;
    CWeatherSA::VAR_AmountOfRain =          (float*)            0x0C81324;
    CWeatherSA::FUNC_IsRaining =                                0x4ABF50;
}


void COffsets::Initialize20 ( void )
{
    // TODO
}


void COffsets::InitializeCommon10 ( void )
{
    CGameSA::VAR_SystemTime =               (unsigned long*)    0xB7CB84;
    CGameSA::VAR_IsAtMenu =                 (unsigned long*)    0xBA677B;
    CGameSA::VAR_IsGameLoaded =             (unsigned long*)    0x86969C;
    CGameSA::VAR_GamePaused =               (bool*)             0xB7CB49;
    CGameSA::VAR_IsForegroundWindow =       (bool*)             0x8D621C;
    CGameSA::VAR_SystemState =              (unsigned long*)    0xC8D4C0;
    CGameSA::VAR_StartGame =                (void*)             0x869641;
    CGameSA::VAR_IsNastyGame =              (bool*)             0x68DD68;
    CGameSA::VAR_TimeScale =                (float*)            0xB7CB64;
    CGameSA::VAR_FPS =                      (float*)            0xB7CB50;
    CGameSA::VAR_OldTimeStep =              (float*)            0xB7CB54;
    CGameSA::VAR_TimeStep =                 (float*)            0xB7CB5C;
    CGameSA::VAR_Framelimiter =             (unsigned long*)    0xC1704C;

    CWeatherSA::VAR_CurrentWeather =        (unsigned char*)    0xC81318;
    CWeatherSA::VAR_CurrentWeather_b =      (unsigned char*)    0xC81320;
    CWeatherSA::VAR_CurrentWeather_c =      (unsigned char*)    0xC8131C;
    CWeatherSA::VAR_AmountOfRain =          (float*)            0x0C81324;
    CWeatherSA::FUNC_IsRaining =                                0x4ABF50;

    CSettingsSA::FUNC_SetCurrentVideoMode =                     0x745C70; // may not be common?

    CEntitySA::FUNC_CClumpModelInfo__GetFrameFromId = 0x4C53C0;
}
