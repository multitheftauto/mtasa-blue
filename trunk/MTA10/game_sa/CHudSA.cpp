/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CHudSA.cpp
*  PURPOSE:		HUD display
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
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
		push	0
		push	1
		lea		eax, szHelp
		push	eax
		call	dwFunction
		add		esp, 0xC
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
		push	10 // what is this param?
		lea		eax, wszBigMessage
		push	eax
		call	dwFunction
		add		esp, 8
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
//			wchar_t szHelp[255] = {'\0'};
	//MultiByteToWideChar(CP_ACP, 0, szMessage, -1, szHelp, 255);
		if(strlen(szName) < 50)
		{
			strcpy(szVehicleName, szName);
			szVehicleNamePtr = szVehicleName;
		}
	}
	else
	{
		*(DWORD *)VAR_VehicleNamePtr = 0;
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
		*(DWORD *)VAR_ZoneNamePtr = 0;
	}
}

VOID CHudSA::Disable ( bool bDisabled )
{
	DEBUG_TRACE("VOID CHudSA::Disable ( bool bDisabled )");
	DWORD oldProt, oldProt2;
	VirtualProtect((LPVOID)FUNC_Draw,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	if ( bDisabled )
		*(BYTE *)FUNC_Draw = 0xC3;
	else
		*(BYTE *)FUNC_Draw = 0x80;
	VirtualProtect((LPVOID)FUNC_Draw,1,oldProt,&oldProt2);

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
		//push	bSetting2
		//push	bSetting1
		push	fY
		push	fX
		push	vecPosition2
		push	vecPosition1
		call	dwFunc
		add		esp, 0x18
		sub		esp, 8
		mov		bReturn, al
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
    DWORD oldProt, oldProt2;
    static BYTE byteOriginal = 0;
    VirtualProtect((LPVOID)FUNC_DrawAmmo,1,PAGE_EXECUTE_READWRITE,&oldProt);		
    if ( bDisabled && !byteOriginal )
    {
        byteOriginal = *(BYTE *)FUNC_DrawAmmo;
	    *(BYTE *)FUNC_DrawAmmo = 0xC3;
    }
    else if ( !bDisabled && byteOriginal )
    {
	    *(BYTE *)FUNC_DrawAmmo = byteOriginal;
        byteOriginal = 0;
    }

	VirtualProtect((LPVOID)FUNC_DrawAmmo,1,oldProt,&oldProt2);
}

void CHudSA::DisableWeaponIcon ( bool bDisabled )
{
    DWORD oldProt, oldProt2;
    static BYTE byteOriginal = 0;
    VirtualProtect((LPVOID)FUNC_DrawWeaponIcon,1,PAGE_EXECUTE_READWRITE,&oldProt);		
    if ( bDisabled && !byteOriginal )
    {
        byteOriginal = *(BYTE *)FUNC_DrawWeaponIcon;
	    *(BYTE *)FUNC_DrawWeaponIcon = 0xC3;
    }
    else if ( !bDisabled && byteOriginal )
    {
	    *(BYTE *)FUNC_DrawWeaponIcon = byteOriginal;
        byteOriginal = 0;
    }

	VirtualProtect((LPVOID)FUNC_DrawWeaponIcon,1,oldProt,&oldProt2);
}

void CHudSA::DisableHealth ( bool bDisabled )
{
    DWORD oldProt, oldProt2;
    static BYTE byteOriginal = 0;
    VirtualProtect((LPVOID)FUNC_PrintHealthForPlayer,1,PAGE_EXECUTE_READWRITE,&oldProt);		
    if ( bDisabled && !byteOriginal )
    {
        byteOriginal = *(BYTE *)FUNC_PrintHealthForPlayer;
	    *(BYTE *)FUNC_PrintHealthForPlayer = 0xC3;
    }
    else if ( !bDisabled && byteOriginal )
    {
	    *(BYTE *)FUNC_PrintHealthForPlayer = byteOriginal;
        byteOriginal = 0;
    }

	VirtualProtect((LPVOID)FUNC_PrintHealthForPlayer,1,oldProt,&oldProt2);
}

void CHudSA::DisableBreath ( bool bDisabled )
{
    DWORD oldProt, oldProt2;
    static BYTE byteOriginal = 0;
    VirtualProtect((LPVOID)FUNC_PrintBreathForPlayer,1,PAGE_EXECUTE_READWRITE,&oldProt);		
    if ( bDisabled && !byteOriginal )
    {
        byteOriginal = *(BYTE *)FUNC_PrintBreathForPlayer;
	    *(BYTE *)FUNC_PrintBreathForPlayer = 0xC3;
    }
    else if ( !bDisabled && byteOriginal )
    {
	    *(BYTE *)FUNC_PrintBreathForPlayer = byteOriginal;
        byteOriginal = 0;
    }

	VirtualProtect((LPVOID)FUNC_PrintBreathForPlayer,1,oldProt,&oldProt2);
}

void CHudSA::DisableArmour ( bool bDisabled )
{
    DWORD oldProt, oldProt2;
    static BYTE byteOriginal = 0;
    VirtualProtect((LPVOID)FUNC_PrintArmourForPlayer,1,PAGE_EXECUTE_READWRITE,&oldProt);		
    if ( bDisabled && !byteOriginal )
    {
        byteOriginal = *(BYTE *)FUNC_PrintArmourForPlayer;
	    *(BYTE *)FUNC_PrintArmourForPlayer = 0xC3;
    }
    else if ( !bDisabled && byteOriginal )
    {
	    *(BYTE *)FUNC_PrintArmourForPlayer = byteOriginal;
        byteOriginal = 0;
    }

	VirtualProtect((LPVOID)FUNC_PrintArmourForPlayer,1,oldProt,&oldProt2);
}

void CHudSA::DisableVitalStats ( bool bDisabled )
{
    DWORD oldProt, oldProt2;
    static BYTE byteOriginal = 0;
    VirtualProtect((LPVOID)FUNC_DrawVitalStats,1,PAGE_EXECUTE_READWRITE,&oldProt);		
    if ( bDisabled && !byteOriginal )
    {
        byteOriginal = *(BYTE *)FUNC_DrawVitalStats;
	    *(BYTE *)FUNC_DrawVitalStats = 0xC3;
    }
    else if ( !bDisabled && byteOriginal )
    {
	    *(BYTE *)FUNC_DrawVitalStats = byteOriginal;
        byteOriginal = 0;
    }

	VirtualProtect((LPVOID)FUNC_DrawVitalStats,1,oldProt,&oldProt2);
}

void CHudSA::DisableMoney ( bool bDisabled )
{
    DWORD oldProt, oldProt2;
    static DWORD dwOriginal = 0;
    VirtualProtect((LPVOID)CODE_ShowMoney,4,PAGE_EXECUTE_READWRITE,&oldProt);		
    if ( bDisabled && !dwOriginal )
    {
        dwOriginal = *(DWORD *)CODE_ShowMoney;
        VirtualProtect((LPVOID)CODE_ShowMoney,2,PAGE_EXECUTE_READWRITE,&oldProt);		
	    *(BYTE *)CODE_ShowMoney       = 0x90;
        *(BYTE *)(CODE_ShowMoney + 1) = 0xE9;
	    VirtualProtect((LPVOID)CODE_ShowMoney,2,oldProt,&oldProt2);
    }
    else if ( !bDisabled && dwOriginal )
    {
	    *(DWORD *)CODE_ShowMoney = dwOriginal;
        dwOriginal = 0;
    }

	VirtualProtect((LPVOID)CODE_ShowMoney,4,oldProt,&oldProt2);
}

void CHudSA::DisableVehicleName ( bool bDisabled )
{
    DWORD oldProt, oldProt2;
    static BYTE byteOriginal = 0;
    VirtualProtect((LPVOID)FUNC_DrawVehicleName,1,PAGE_EXECUTE_READWRITE,&oldProt);		
    if ( bDisabled && !byteOriginal )
    {
        byteOriginal = *(BYTE *)FUNC_DrawVehicleName;
	    *(BYTE *)FUNC_DrawVehicleName = 0xC3;
    }
    else if ( !bDisabled && byteOriginal )
    {
	    *(BYTE *)FUNC_DrawVehicleName = byteOriginal;
        byteOriginal = 0;
    }

	VirtualProtect((LPVOID)FUNC_DrawVehicleName,1,oldProt,&oldProt2);
}

void CHudSA::DisableHelpText ( bool bDisabled )
{
    DWORD oldProt, oldProt2;
    static BYTE byteOriginal = 0;
    VirtualProtect((LPVOID)FUNC_DrawHelpText,1,PAGE_EXECUTE_READWRITE,&oldProt);		
    if ( bDisabled && !byteOriginal )
    {
        byteOriginal = *(BYTE *)FUNC_DrawHelpText;
	    *(BYTE *)FUNC_DrawHelpText = 0xC3;
    }
    else if ( !bDisabled && byteOriginal )
    {
	    *(BYTE *)FUNC_DrawHelpText = byteOriginal;
        byteOriginal = 0;
    }

	VirtualProtect((LPVOID)FUNC_DrawHelpText,1,oldProt,&oldProt2);
}

void CHudSA::DisableAreaName ( bool bDisabled )
{
    DWORD oldProt, oldProt2;
    static BYTE byteOriginal = 0;
    VirtualProtect((LPVOID)FUNC_DrawAreaName,1,PAGE_EXECUTE_READWRITE,&oldProt);		
    if ( bDisabled && !byteOriginal )
    {
        byteOriginal = *(BYTE *)FUNC_DrawAreaName;
	    *(BYTE *)FUNC_DrawAreaName = 0xC3;
    }
    else if ( !bDisabled && byteOriginal )
    {
	    *(BYTE *)FUNC_DrawAreaName = byteOriginal;
        byteOriginal = 0;
    }

	VirtualProtect((LPVOID)FUNC_DrawAreaName,1,oldProt,&oldProt2);
}

void CHudSA::DisableRadar ( bool bDisabled )
{
    DWORD oldProt, oldProt2;
    static BYTE byteOriginal = 0;
    VirtualProtect((LPVOID)FUNC_DrawRadar,1,PAGE_EXECUTE_READWRITE,&oldProt);		
    if ( bDisabled && !byteOriginal )
    {
        byteOriginal = *(BYTE *)FUNC_DrawRadar;
	    *(BYTE *)FUNC_DrawRadar = 0xC3;
    }
    else if ( !bDisabled && byteOriginal )
    {
	    *(BYTE *)FUNC_DrawRadar = byteOriginal;
        byteOriginal = 0;
    }

	VirtualProtect((LPVOID)FUNC_DrawRadar,1,oldProt,&oldProt2);
}

void CHudSA::DisableClock ( bool bDisabled )
{
    DWORD oldProt, oldProt2;
    VirtualProtect((LPVOID)VAR_DisableClock,4,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(int *)VAR_DisableClock = bDisabled ? 0 : 1;
	VirtualProtect((LPVOID)VAR_DisableClock,4,oldProt,&oldProt2);
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
}
