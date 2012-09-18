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

CHudSA::CHudSA ( void )
{
    InitComponentList ();
}

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

    // Also disable the radar as the above code will not hide it before the local player has spawned
    if ( bDisabled )
        MemPut < BYTE > ( FUNC_DrawRadarPlanB, 0xC3 );
    else
        MemPut < BYTE > ( FUNC_DrawRadarPlanB, 0x83 );
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

//
// CHudSA::InitComponentList
//
void CHudSA::InitComponentList ( void )
{
    SHudComponent componentList[] = {
                { 1, HUD_AMMO, 1, FUNC_DrawAmmo, 1, 0xCC, 0xC3 },
                { 1, HUD_WEAPON, 1, FUNC_DrawWeaponIcon, 1, 0xCC, 0xC3 },
                { 1, HUD_HEALTH, 1, FUNC_PrintHealthForPlayer, 1, 0xCC, 0xC3 },
                { 1, HUD_BREATH, 1, FUNC_PrintBreathForPlayer, 1, 0xCC, 0xC3 },
                { 1, HUD_ARMOUR, 1, FUNC_PrintArmourForPlayer, 1, 0xCC, 0xC3 },
                { 1, HUD_MONEY, 1, CODE_ShowMoney, 2, 0xCCCC, 0xE990 },
                { 1, HUD_VEHICLE_NAME, 1, FUNC_DrawVehicleName, 1, 0xCC, 0xC3 },
                { 1, HUD_AREA_NAME, 1, FUNC_DrawAreaName, 1, 0xCC, 0xC3 },
                { 1, HUD_RADAR, 1, FUNC_DrawRadar, 1, 0xCC, 0xC3 },
                { 1, HUD_CLOCK, 0, VAR_DisableClock, 1, 1, 0 },
                { 1, HUD_RADIO, 1, FUNC_DrawRadioName, 1, 0xCC, 0xC3 },
                { 1, HUD_WANTED, 1, FUNC_DrawWantedLevel, 1, 0xCC, 0xC3 },
                { 1, HUD_CROSSHAIR, 1, FUNC_DrawCrosshair, 1, 0xCC, 0xC3 },
                { 1, HUD_VITAL_STATS, 1, FUNC_DrawVitalStats, 1, 0xCC, 0xC3 },
                { 0, HUD_HELP_TEXT, 1, FUNC_DrawHelpText, 1, 0xCC, 0xC3 },
            };

    for ( uint i = 0 ; i < NUMELMS( componentList ) ; i++ )
    {
        const SHudComponent& component = componentList[i]; 
        MapSet ( m_HudComponentMap, component.type, component );
    }
}

//
// CHudSA::SetComponentVisible
//
void CHudSA::SetComponentVisible ( eHudComponent component, bool bVisible )
{
    // Handle ALL option
    if ( component == HUD_ALL )
    {
        for ( std::map < eHudComponent, SHudComponent >::iterator iter = m_HudComponentMap.begin () ; iter != m_HudComponentMap.end () ; ++iter )
        {
            const SHudComponent& component = iter->second;
            if ( component.bIsPartOfAll )
                SetComponentVisible ( component.type, bVisible );
        }
        return;
    }

    // Set visiblity of one component
    SHudComponent* pComponent = MapFind ( m_HudComponentMap, component );
    if ( pComponent )
    {
        // Save original bytes if requred
        if ( pComponent->bSaveOriginalBytes )
        {
            pComponent->origData = *(DWORD*)pComponent->uiDataAddr;
            pComponent->bSaveOriginalBytes = false;
        }

        // Poke bytes
        uchar* pSrc = (uchar*)( bVisible ? &pComponent->origData : &pComponent->disabledData );
        uchar* pDest = (uchar*)( pComponent->uiDataAddr );
        for ( uint i = 0 ; i < pComponent->uiDataSize ; i++ )
        {
            MemPut < BYTE > ( pDest + i, pSrc[i] );
        }
    }
}

//
// CHudSA::IsComponentVisible
//
bool CHudSA::IsComponentVisible ( eHudComponent component )
{
    SHudComponent* pComponent = MapFind ( m_HudComponentMap, component );
    if ( pComponent )
    {
        // Determine if invisible by matching data with disabled values
        uchar* pSrc = (uchar*)( &pComponent->disabledData );
        uchar* pDest = (uchar*)( pComponent->uiDataAddr );
        if ( memcmp ( pDest, pSrc, pComponent->uiDataSize ) == 0 )
            return false;   // Matches disabled bytes
        return true;
    }
    return false;
}
