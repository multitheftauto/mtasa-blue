/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CHudSA.h
 *  PURPOSE:     Header file for HUD display class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CHud.h>
#include <CVector.h>

#define FUNC_Draw                   0x58FAE0

#define CODE_ShowMoney              0x58F47D

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

protected:
    void InitComponentList();

    std::map<eHudComponent, SHudComponent> m_HudComponentMap;

    float* m_pfAspectRatioMultiplicator;
    float* m_pfCameraCrosshairScale;
    float  m_fSniperCrosshairScale;
};
