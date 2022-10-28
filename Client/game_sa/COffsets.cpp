/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/COffsets.cpp
 *  PURPOSE:     EXE-dependant game variable offsets
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CCameraSA.h"
#include "CEntitySA.h"
#include "COffsets.h"
#include "CSettingsSA.h"
#include "CWeatherSA.h"
#include "CGameSA.h"

void COffsets::Initialize10EU()
{
    InitializeCommon10();

    CSettingsSA::FUNC_GetCurrentVideoMode = 0x7F2D60;
    CSettingsSA::FUNC_GetNumVideoModes = 0x7F2D00;
    CSettingsSA::FUNC_GetVideoModeInfo = 0x7F2D30;
    CSettingsSA::FUNC_SetDrawDistance = 0x572ea0;
    CSettingsSA::FUNC_GetNumSubSystems = 0x07F2C40;
    CSettingsSA::FUNC_GetCurrentSubSystem = 0x07F2CA0;
    CSettingsSA::FUNC_SetSubSystem = 0x07F2CD0;

    CEntitySA::FUNC_RwFrameGetLTM = 0x7F09D0;
    CCameraSA::FUNC_RwFrameGetLTM = 0x7F09D0;
}

void COffsets::Initialize10US()
{
    InitializeCommon10();

    CSettingsSA::FUNC_GetCurrentVideoMode = 0x7F2D20;
    CSettingsSA::FUNC_GetNumVideoModes = 0x7F2CC0;
    CSettingsSA::FUNC_GetVideoModeInfo = 0x7F2CF0;
    CSettingsSA::FUNC_SetDrawDistance = 0x572ea0;
    CSettingsSA::FUNC_GetNumSubSystems = 0x07F2C00;
    CSettingsSA::FUNC_GetCurrentSubSystem = 0x07F2C60;
    CSettingsSA::FUNC_SetSubSystem = 0x07F2C90;

    CEntitySA::FUNC_RwFrameGetLTM = 0x7F0990;
    CCameraSA::FUNC_RwFrameGetLTM = 0x7F0990;
}

void COffsets::Initialize11()
{
    CGameSA::VAR_SystemTime = (unsigned long*)0xB7CB84;
    CGameSA::VAR_IsAtMenu = (unsigned long*)0xBA677B;
    CGameSA::VAR_IsForegroundWindow = (bool*)0x8D621C;
    CGameSA::VAR_SystemState = (unsigned long*)0xC8FC80;            // 1.01
    CGameSA::VAR_TimeScale = (float*)0xB7CB64;
    CGameSA::VAR_FPS = (float*)0xB7CB50;
    CGameSA::VAR_OldTimeStep = (float*)0xB7CB54;
    CGameSA::VAR_TimeStep = (float*)0xB7CB5C;
    CGameSA::VAR_Framelimiter = (unsigned long*)0xC1704C;

    CWeatherSA::VAR_CWeather__ForcedWeatherType = (unsigned char*)0xC81318;
    CWeatherSA::VAR_CWeather__OldWeatherType = (unsigned char*)0xC81320;
    CWeatherSA::VAR_CWeather__NewWeatherType = (unsigned char*)0xC8131C;
    CWeatherSA::VAR_CWeather__Rain = (float*)0xC81324;
}

void COffsets::InitializeCommon10()
{
    CGameSA::VAR_SystemTime = (unsigned long*)0xB7CB84;
    CGameSA::VAR_IsAtMenu = (unsigned long*)0xBA677B;
    CGameSA::VAR_IsForegroundWindow = (bool*)0x8D621C;
    CGameSA::VAR_SystemState = (unsigned long*)0xC8D4C0;
    CGameSA::VAR_TimeScale = (float*)0xB7CB64;
    CGameSA::VAR_FPS = (float*)0xB7CB50;
    CGameSA::VAR_OldTimeStep = (float*)0xB7CB54;
    CGameSA::VAR_TimeStep = (float*)0xB7CB5C;
    CGameSA::VAR_Framelimiter = (unsigned long*)0xC1704C;

    CWeatherSA::VAR_CWeather__ForcedWeatherType = (unsigned char*)0xC81318;
    CWeatherSA::VAR_CWeather__OldWeatherType = (unsigned char*)0xC81320;
    CWeatherSA::VAR_CWeather__NewWeatherType = (unsigned char*)0xC8131C;
    CWeatherSA::VAR_CWeather__Rain = (float*)0xC81324;

    CSettingsSA::FUNC_SetCurrentVideoMode = 0x745C70;            // may not be common?

    CEntitySA::FUNC_CClumpModelInfo__GetFrameFromId = 0x4C53C0;
}
