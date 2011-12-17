/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CHudSA.cpp
*  PURPOSE:     HUD display
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

char szVehicleName[50] = {'\0'};
char szZoneName[50] = {'\0'};

VOID CHudSA::SetHelpMessage( char * szMessage )
{
    DEBUG_TRACE("VOID CHudSA::SetHelpMessage( char * szMessage )");
    wchar_t szHelp[255] = {'\0'};
    MultiByteToWideChar(CP_ACP, 0, szMessage, -1, szHelp, 255);

    DWORD dwFunction = FUNC_SetHelpMessage;
    _asm
    {
        push    0
        push    1
        lea     eax, szHelp
        push    eax
        call    dwFunction
        add     esp, 0xC
    }
}

/**
 * \todo Find out what param 2 is
 */
VOID CHudSA::SetBigMessage( char * szBigMessage )
{
    DEBUG_TRACE("VOID CHudSA::SetBigMessage( char * szBigMessage )");
    wchar_t wszBigMessage[255] = {'\0'};
    MultiByteToWideChar(CP_ACP, 0, szBigMessage, -1, wszBigMessage, 255);

    DWORD dwFunction = FUNC_SetBigMessage;
    _asm
    {
        push    10 // what is this param?
        lea     eax, wszBigMessage
        push    eax
        call    dwFunction
        add     esp, 8
    }
}

/** 
 * \todo Fix: doesn't work
 */
VOID CHudSA::SetVehicleName( char * szName )
{
    DEBUG_TRACE("VOID CHudSA::SetVehicleName( char * szName )");
    char * szVehicleNamePtr = (char *)VAR_VehicleNamePtr;
    if(szName != 0)
    {
//          wchar_t szHelp[255] = {'\0'};
    //MultiByteToWideChar(CP_ACP, 0, szMessage, -1, szHelp, 255);
        if(strlen(szName) < 50)
        {
            strcpy(szVehicleName, szName);
            szVehicleNamePtr = szVehicleName;
        }
    }
    else
    {
        MemPutFast < DWORD > ( VAR_VehicleNamePtr, 0 );
    }
}

/** 
 * \todo Fix: doesn't work
 */
VOID CHudSA::SetZoneName( char * szName )
{
    DEBUG_TRACE("VOID CHudSA::SetZoneName( char * szName )");
    char * szZoneNamePtr = (char *)VAR_ZoneNamePtr;
    if(szName != 0)
    {
        if(strlen(szName) < 50)
        {
            strcpy(szZoneName, szName);
            szZoneNamePtr = szZoneName;
        }
    }
    else
    {
        MemPutFast < DWORD > ( VAR_ZoneNamePtr, 0 );
    }
}

VOID CHudSA::Disable ( bool bDisabled )
{
    DEBUG_TRACE("VOID CHudSA::Disable ( bool bDisabled )");
    if ( bDisabled )
        MemPut < BYTE > ( FUNC_Draw, 0xC3 );
    else
        MemPut < BYTE > ( FUNC_Draw, 0x80 );

}

bool CHudSA::IsDisabled ( void )
{
    return *(BYTE*)FUNC_Draw == 0xC3;
}

VOID CHudSA::DrawBarChart ( float fX, float fY, DWORD dwWidth, DWORD dwHeight, float fPercentage, DWORD dwForeColor, DWORD dwBorderColor )
{
    DWORD dwFunc= FUNC_DrawBarChart;
    _asm
    {
        push    dwBorderColor
        push    dwForeColor
        push    1
        push    0
        push    0
        push    fPercentage
        push    dwHeight
        push    dwWidth
        push    fY
        push    fX
        call    dwFunc
        add     esp, 0x28
    }
}

bool CHudSA::CalcScreenCoors ( CVector * vecPosition1, CVector * vecPosition2, float * fX, float * fY, bool bSetting1, bool bSetting2 )
{
    DWORD dwFunc = 0x71DA00;
    bool bReturn = false;
    _asm
    {
        //push  bSetting2
        //push  bSetting1
        push    fY
        push    fX
        push    vecPosition2
        push    vecPosition1
        call    dwFunc
        add     esp, 0x18
        sub     esp, 8
        mov     bReturn, al
    }
    return bReturn;
}

void CHudSA::Draw2DPolygon ( float fX1, float fY1, float fX2, float fY2, float fX3, float fY3, float fX4, float fY4, DWORD dwColor )
{
    DWORD dwFunc = FUNC_Draw2DPolygon;
    _asm
    {
        lea     eax, dwColor
        push    eax
        push    fY4
        push    fX4
        push    fY3
        push    fX3
        push    fY2
        push    fX2
        push    fY1
        push    fX1
        call    dwFunc
        add     esp, 36
    }
}

void CHudSA::DisableAmmo ( bool bDisabled )
{
    static BYTE byteOriginal = 0;
    if ( bDisabled && !byteOriginal )
    {
        byteOriginal = *(BYTE *)FUNC_DrawAmmo;
        MemPut < BYTE > ( FUNC_DrawAmmo, 0xC3 );
    }
    else if ( !bDisabled && byteOriginal )
    {
        MemPut < BYTE > ( FUNC_DrawAmmo, byteOriginal );
        byteOriginal = 0;
    }
}

void CHudSA::DisableWeaponIcon ( bool bDisabled )
{
    static BYTE byteOriginal = 0;
    if ( bDisabled && !byteOriginal )
    {
        byteOriginal = *(BYTE *)FUNC_DrawWeaponIcon;
        MemPut < BYTE > ( FUNC_DrawWeaponIcon, 0xC3 );
    }
    else if ( !bDisabled && byteOriginal )
    {
        MemPut < BYTE > ( FUNC_DrawWeaponIcon, byteOriginal );
        byteOriginal = 0;
    }
}

void CHudSA::DisableHealth ( bool bDisabled )
{
    static BYTE byteOriginal = 0;
    if ( bDisabled && !byteOriginal )
    {
        byteOriginal = *(BYTE *)FUNC_PrintHealthForPlayer;
        MemPut < BYTE > ( FUNC_PrintHealthForPlayer, 0xC3 );
    }
    else if ( !bDisabled && byteOriginal )
    {
        MemPut < BYTE > ( FUNC_PrintHealthForPlayer, byteOriginal );
        byteOriginal = 0;
    }
}

void CHudSA::DisableBreath ( bool bDisabled )
{
    static BYTE byteOriginal = 0;
    if ( bDisabled && !byteOriginal )
    {
        byteOriginal = *(BYTE *)FUNC_PrintBreathForPlayer;
        MemPut < BYTE > ( FUNC_PrintBreathForPlayer, 0xC3 );
    }
    else if ( !bDisabled && byteOriginal )
    {
        MemPut < BYTE > ( FUNC_PrintBreathForPlayer, byteOriginal );
        byteOriginal = 0;
    }
}

void CHudSA::DisableArmour ( bool bDisabled )
{
    static BYTE byteOriginal = 0;
    if ( bDisabled && !byteOriginal )
    {
        byteOriginal = *(BYTE *)FUNC_PrintArmourForPlayer;
        MemPut < BYTE > ( FUNC_PrintArmourForPlayer, 0xC3 );
    }
    else if ( !bDisabled && byteOriginal )
    {
        MemPut < BYTE > ( FUNC_PrintArmourForPlayer, byteOriginal );
        byteOriginal = 0;
    }
}

void CHudSA::DisableVitalStats ( bool bDisabled )
{
    static BYTE byteOriginal = 0;
    if ( bDisabled && !byteOriginal )
    {
        byteOriginal = *(BYTE *)FUNC_DrawVitalStats;
        MemPut < BYTE > ( FUNC_DrawVitalStats, 0xC3 );
    }
    else if ( !bDisabled && byteOriginal )
    {
        MemPut < BYTE > ( FUNC_DrawVitalStats, byteOriginal );
        byteOriginal = 0;
    }
}

void CHudSA::DisableMoney ( bool bDisabled )
{
    static DWORD dwOriginal = 0;
    if ( bDisabled && !dwOriginal )
    {
        dwOriginal = *(DWORD *)CODE_ShowMoney;
        MemPut < BYTE > ( CODE_ShowMoney, 0x90 );
        MemPut < BYTE > ( CODE_ShowMoney + 1, 0xE9 );
    }
    else if ( !bDisabled && dwOriginal )
    {
        MemPut < DWORD > ( CODE_ShowMoney, dwOriginal );
        dwOriginal = 0;
    }
}

void CHudSA::DisableVehicleName ( bool bDisabled )
{
    static BYTE byteOriginal = 0;
    if ( bDisabled && !byteOriginal )
    {
        byteOriginal = *(BYTE *)FUNC_DrawVehicleName;
        MemPut < BYTE > ( FUNC_DrawVehicleName, 0xC3 );
    }
    else if ( !bDisabled && byteOriginal )
    {
        MemPut < BYTE > ( FUNC_DrawVehicleName, byteOriginal );
        byteOriginal = 0;
    }
}

void CHudSA::DisableHelpText ( bool bDisabled )
{
    static BYTE byteOriginal = 0;
    if ( bDisabled && !byteOriginal )
    {
        byteOriginal = *(BYTE *)FUNC_DrawHelpText;
        MemPut < BYTE > ( FUNC_DrawHelpText, 0xC3 );
    }
    else if ( !bDisabled && byteOriginal )
    {
        MemPut < BYTE > ( FUNC_DrawHelpText, byteOriginal );
        byteOriginal = 0;
    }
}

void CHudSA::DisableAreaName ( bool bDisabled )
{
    static BYTE byteOriginal = 0;
    if ( bDisabled && !byteOriginal )
    {
        byteOriginal = *(BYTE *)FUNC_DrawAreaName;
        MemPut < BYTE > ( FUNC_DrawAreaName, 0xC3 );
    }
    else if ( !bDisabled && byteOriginal )
    {
        MemPut < BYTE > ( FUNC_DrawAreaName, byteOriginal );
        byteOriginal = 0;
    }
}

void CHudSA::DisableRadar ( bool bDisabled )
{
    static BYTE byteOriginal = 0;
    if ( bDisabled && !byteOriginal )
    {
        byteOriginal = *(BYTE *)FUNC_DrawRadar;
        MemPut < BYTE > ( FUNC_DrawRadar, 0xC3 );
    }
    else if ( !bDisabled && byteOriginal )
    {
        MemPut < BYTE > ( FUNC_DrawRadar, byteOriginal );
        byteOriginal = 0;
    }
}

void CHudSA::DisableClock ( bool bDisabled )
{
    MemPutFast < int > ( VAR_DisableClock, bDisabled ? 0 : 1 );
}

void CHudSA::DisableRadioName ( bool bDisabled )
{
    static BYTE byteOriginal = 0;
    if ( bDisabled && !byteOriginal )
    {
        byteOriginal = *(BYTE *)FUNC_DrawRadioName;
        MemPut < BYTE > ( FUNC_DrawRadioName, 0xC3 );
    }
    else if ( !bDisabled && byteOriginal )
    {
        MemPut < BYTE > ( FUNC_DrawRadioName, byteOriginal );
        byteOriginal = 0;
    }
}

void CHudSA::DisableWantedLevel ( bool bDisabled )
{
    static BYTE byteOriginal = 0;
    if ( bDisabled && !byteOriginal )
    {
        byteOriginal = *(BYTE *)FUNC_DrawWantedLevel;
        MemPut < BYTE > ( FUNC_DrawWantedLevel, 0xC3 );
    }
    else if ( !bDisabled && byteOriginal )
    {
        MemPut < BYTE > ( FUNC_DrawWantedLevel, byteOriginal );
        byteOriginal = 0;
    }
}

void CHudSA::DisableCrosshair ( bool bDisabled )
{
    static BYTE byteOriginal = 0;
    if ( bDisabled && !byteOriginal )
    {
        byteOriginal = *(BYTE *)FUNC_DrawCrosshair;
        MemPut < BYTE > ( FUNC_DrawCrosshair, 0xC3 );
    }
    else if ( !bDisabled && byteOriginal )
    {
        MemPut < BYTE > ( FUNC_DrawCrosshair, byteOriginal );
        byteOriginal = 0;
    }
}

void CHudSA::DisableAll ( bool bDisabled )
{
    DisableAmmo ( bDisabled );
    DisableWeaponIcon ( bDisabled );
    DisableHealth ( bDisabled );
    DisableBreath ( bDisabled );
    DisableArmour ( bDisabled );
    DisableVitalStats ( bDisabled );
    DisableMoney ( bDisabled );
    DisableVehicleName ( bDisabled );
    // Keep the help text always disabled
    //DisableHelpText ( bDisabled );
    DisableAreaName ( bDisabled );
    DisableRadar ( bDisabled );
    DisableClock ( bDisabled );
    DisableRadioName ( bDisabled );
    DisableWantedLevel ( bDisabled );
    DisableCrosshair ( bDisabled );
}
