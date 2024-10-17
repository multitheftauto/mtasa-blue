/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CHudSA.h
 *  PURPOSE:     Header file for HUD display class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CHud.h>
#include <CVector.h>

#define FUNC_Draw                   0x58FAE0

#define VAR_DisableClock            0xBAA400

#define VAR_AspectRatioMult         0x859524
#define VAR_CameraCrosshairScale    0x866C74

#define FUNC_DrawAmmo               0x5893B0
#define FUNC_DrawWeaponIcon         0x58D7D0
#define FUNC_PrintHealthForPlayer   0x589270
#define FUNC_PrintBreathForPlayer   0x589190
#define FUNC_PrintArmourForPlayer   0x5890A0
#define FUNC_DrawVitalStats         0x589650
#define FUNC_DrawVehicleName        0x58AEA0
#define FUNC_DrawHelpText           0x58B6E0
#define FUNC_DrawAreaName           0x58AA50
#define FUNC_DrawRadar              0x58A330
#define FUNC_DrawRadarPlanB         0x58A335
#define FUNC_DrawRadioName          0x4E9E50
#define FUNC_DrawWantedLevel        0x58D9A0
#define FUNC_DrawCrosshair          0x58E020

#define CODE_ShowMoney              0x58F47D

#define VAR_CTheScripts_bDrawCrossHair 0xA44490

struct SHudComponent
{
    bool          bIsPartOfAll;
    eHudComponent type;
    bool          bSaveOriginalBytes;
    DWORD         uiDataAddr;
    DWORD         uiDataSize;
    DWORD         origData;
    DWORD         disabledData;
};

class CHudSA : public CHud
{
public:
    CHudSA();
    void Disable(bool bDisabled);
    bool IsDisabled();
    void SetComponentVisible(eHudComponent component, bool bVisible);
    bool IsComponentVisible(eHudComponent component);
    void AdjustComponents(float fAspectRatio);
    void ResetComponentAdjustment();
    bool IsCrosshairVisible();

protected:
    void InitComponentList();

    std::map<eHudComponent, SHudComponent> m_HudComponentMap;

    float* m_pfAspectRatioMultiplicator;
    float* m_pfCameraCrosshairScale;
    float  m_fSniperCrosshairScale;
};
