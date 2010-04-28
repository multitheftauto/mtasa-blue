/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CHudSA.h
*  PURPOSE:     Header file for HUD display class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_HUD
#define __CGAMESA_HUD

#include <game/CHud.h>
#include <CVector.h>
#include "Common.h"

#define FUNC_GetRidOfAllHudMessages 0x588a50
#define FUNC_SetHelpMessage         0x588be0
#define FUNC_SetVehicleName         0x588f50
#define FUNC_SetZoneName            0x588bb0
#define FUNC_SetBigMessage          0x588fc0

#define FUNC_DrawBarChart           0x728640
#define FUNC_CalcScreenCoors        0x70CE30
#define FUNC_Draw2DPolygon          0x7285B0
#define FUNC_Draw                   0x58FAE0

#define VAR_DisableHud              0xC8A7C1
#define VAR_DisableClock            0xBAA400

#define VAR_VehicleNamePtr          0xA0FC98
#define VAR_ZoneNamePtr             0x97537C

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

#define CODE_ShowMoney              0x58F47D

class CHudSA : public CHud
{
public:
    VOID                SetHelpMessage( char * szMessage );
    VOID                SetBigMessage( char * szBigMessage );
    VOID                SetVehicleName( char * szName );
    VOID                SetZoneName( char * szName );
    VOID                Disable ( bool bDisabled );
    VOID                DrawBarChart ( float fX, float fY, DWORD dwWidth, DWORD dwHeight, float fPercentage, DWORD dwForeColor, DWORD dwBorderColor );
    bool                CalcScreenCoors ( CVector * vecPosition1, CVector * vecPosition2, float * fX, float * fY, bool bSetting1, bool bSetting2 );
    void                Draw2DPolygon ( float fX1, float fY1, float fX2, float fY2, float fX3, float fY3, float fX4, float fY4, DWORD dwColor );
    void                DisableAmmo ( bool bDisabled );
    void                DisableWeaponIcon ( bool bDisabled );
    void                DisableHealth ( bool bDisabled );
    void                DisableBreath ( bool bDisabled );
    void                DisableArmour ( bool bDisabled );
    void                DisableVitalStats ( bool bDisabled );
    void                DisableMoney ( bool bDisabled );
    void                DisableVehicleName ( bool bDisabled );
    void                DisableHelpText ( bool bDisabled );
    void                DisableAreaName ( bool bDisabled );
    void                DisableRadar ( bool bDisabled );
    void                DisableClock ( bool bDisabled );
    void                DisableAll ( bool bDisabled );
};

#endif
