/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CDamageManagerSA.cpp
*  PURPOSE:		Vehicle damage manager
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

BYTE CDamageManagerSA::GetEngineStatus (  )
{
	DEBUG_TRACE("BYTE CDamageManagerSA::GetEngineStatus (  )");
	return internalInterface->bEngineStatus;
}

VOID CDamageManagerSA::SetEngineStatus ( BYTE bEngineState )
{
	DEBUG_TRACE("VOID CDamageManagerSA::SetEngineStatus ( BYTE bEngineState )");
	if(bEngineState > 250) bEngineState = 250;
	internalInterface->bEngineStatus = bEngineState;
}

BYTE CDamageManagerSA::GetDoorStatus ( eDoors bDoor )
{
	DEBUG_TRACE("BYTE CDamageManagerSA::GetDoorStatus ( eDoors bDoor )");
	if(bDoor > MAX_DOORS - 1)
		return NULL;
	else
		return internalInterface->Door[bDoor];
}

VOID CDamageManagerSA::SetDoorStatus ( eDoors bDoor, BYTE bDoorStatus )
{
	DEBUG_TRACE("VOID CDamageManagerSA::SetDoorStatus ( eDoors bDoor, BYTE bDoorStatus )");

	if ( bDoor < MAX_DOORS )
    {
        // Different from before?
        if ( internalInterface->Door [bDoor] != bDoorStatus )
        {
            // Set it
            internalInterface->Door [bDoor] = bDoorStatus;

            // Are we making it intact?
            if ( bDoorStatus == DT_DOOR_INTACT || bDoorStatus == DT_DOOR_SWINGING_FREE )
            {
                // Grab the car node index for the given door id
                static int s_iCarNodeIndexes [6] = { 0x10, 0x11, 0x0A, 0x08, 0x0B, 0x09 };

                // Call CAutomobile::FixDoor to update the model
                DWORD dwFunc = 0x6A35A0;
                DWORD dwThis = (DWORD) internalEntityInterface;
                int iCarNodeIndex = s_iCarNodeIndexes [bDoor];
                DWORD dwDoor = (DWORD) bDoor;
                _asm
                {
                    mov     ecx, dwThis
                    push    dwDoor
                    push    iCarNodeIndex
                    call    dwFunc
                }
            }
            else
            {
                // Call CAutomobile::SetDoorDamage to update the model
                DWORD dwFunc = 0x6B1600;
                DWORD dwThis = (DWORD) internalEntityInterface;
                DWORD dwDoor = (DWORD) bDoor;
                bool bUnknown = false;
                _asm
                {
                    mov     ecx, dwThis
                    push    bUnknown
                    push    dwDoor
                    call    dwFunc
                }
            }
        }
    }
}

BYTE CDamageManagerSA::GetWheelStatus ( eWheels bWheel )
{
	DEBUG_TRACE("BYTE CDamageManagerSA::GetWheelStatus ( eWheels bWheel )");
	if(bWheel > MAX_WHEELS - 1)
		return NULL;
	else
		return internalInterface->Wheel[bWheel];
}

VOID CDamageManagerSA::SetWheelStatus ( eWheels bWheel, BYTE bTireStatus )
{
	DEBUG_TRACE("VOID CDamageManagerSA::SetWheelStatus ( eWheels bWheel, BYTE bTireStatus )");
	if(bWheel <= MAX_WHEELS - 1)
    {	
        // Different than before?
        if ( internalInterface->Wheel[bWheel] != bTireStatus )
        {            
            internalInterface->Wheel[bWheel] = bTireStatus;
        }        
    }
}

VOID CDamageManagerSA::SetPanelStatus ( BYTE bPanel, BYTE bPanelStatus )
{
	DEBUG_TRACE("BYTE CDamageManagerSA::SetPanelStatus ( BYTE bLight, BYTE bPanelStatus )");

    // Valid index?
    if ( bPanel < MAX_PANELS && bPanelStatus <= 3 )
    {
        // Different than already?
        if ( GetPanelStatus ( bPanel ) != bPanelStatus )
        {
            // Call the function to set it
	        DWORD dwFunction = FUNC_SetPanelStatus;
	        DWORD dwThis = (DWORD)internalInterface;
	        DWORD dwPanel = bPanel;
            DWORD dwStatus = bPanelStatus;
	        _asm
	        {
		        mov		ecx, dwThis
                push    dwStatus
		        push	dwPanel
		        call	dwFunction
	        }

            // Intact?
            if ( bPanelStatus == DT_PANEL_INTACT )
            {
                // Grab the car node index for the given panel
                static int s_iCarNodeIndexes [7] = { 0x0F, 0x0E, 0x00 /*?*/, 0x00 /*?*/, 0x12, 0x0C, 0x0D };

                //  Call CAutomobile::FixPanel to update the vehicle
                dwFunction = 0x6A3670;
                dwThis = (DWORD) internalEntityInterface;
                int iCarNodeIndex = s_iCarNodeIndexes [bPanel];
                _asm
                {
                    mov     ecx, dwThis
                    push    dwPanel
                    push    iCarNodeIndex
                    call    dwFunction
                }
            }
            else
            {
                // Call CAutomobile::SetPanelDamage to update the vehicle
                dwFunction = 0x6B1480;
                dwThis = (DWORD) internalEntityInterface;
                bool bUnknown = false;
                _asm
                {
                    mov     ecx, dwThis
                    push    bUnknown
                    push    dwPanel
                    call    dwFunction
                }
            }
        }
    }
}

void CDamageManagerSA::SetPanelStatus ( unsigned long ulStatus )
{
    unsigned int uiIndex;

    for ( uiIndex = 0; uiIndex < MAX_PANELS; uiIndex++ )
    {
        SetPanelStatus ( static_cast < eDoors > ( uiIndex ), static_cast < unsigned char > ( ulStatus ) );
        ulStatus >>= 4;
    }
}

BYTE CDamageManagerSA::GetPanelStatus ( BYTE bPanel )
{
    if ( bPanel < MAX_PANELS )
    {
	    DEBUG_TRACE("BYTE CDamageManagerSA::GetPanelStatus ( BYTE bPannel )");
	    DWORD dwFunction = FUNC_GetPanelStatus;
	    DWORD dwPointer = (DWORD)internalInterface;
	    BYTE bReturn = 0;
	    DWORD dwPanel = bPanel;
	    _asm
	    {
		    mov		ecx, dwPointer
		    push	dwPanel
		    call	dwFunction
		    mov		bReturn, al
	    }

        return bReturn;
    }

	return 0;
}

unsigned long CDamageManagerSA::GetPanelStatus ( void )
{
    return internalInterface->Panels;
}

VOID CDamageManagerSA::SetLightStatus ( BYTE bLight, BYTE bLightStatus )
{
	DEBUG_TRACE("BYTE CDamageManagerSA::SetLightStatus ( BYTE bLight, BYTE bLightStatus )");
	DWORD dwFunction = FUNC_SetLightStatus;
	DWORD dwPointer = (DWORD)internalInterface;
    DWORD dwLight = bLight;
    DWORD dwStatus = bLightStatus;
	_asm
	{
		mov		ecx, dwPointer
        push    dwStatus
		push	dwLight
		call	dwFunction
	}
}

void CDamageManagerSA::SetLightStatus ( unsigned char ucStatus )
{
    internalInterface->Lights = static_cast < unsigned long > ( ucStatus );
}

BYTE CDamageManagerSA::GetLightStatus ( BYTE bLight )
{
	DEBUG_TRACE("BYTE CDamageManagerSA::GetLightStatus ( BYTE bLight )");
	DWORD dwFunction = FUNC_GetLightStatus;
	DWORD dwPointer = (DWORD)internalInterface;
	BYTE bReturn = 0;
	DWORD dwLight = bLight;
	_asm
	{
		mov		ecx, dwPointer
		push	dwLight
		call	dwFunction
		mov		bReturn, al
	}
	return bReturn;
}

unsigned char CDamageManagerSA::GetLightStatus ( void )
{
    return static_cast < unsigned char > ( internalInterface->Lights );
}

VOID CDamageManagerSA::SetAeroplaneCompStatus( BYTE CompID, BYTE Status)
{
	DEBUG_TRACE("VOID CDamageManagerSA::SetAeroplaneCompStatus( BYTE CompID, BYTE Status)");
	DWORD dwFunction = FUNC_SetAeroplaneCompStatus;
	DWORD dwPointer = (DWORD)internalInterface;
	DWORD dwPannel = CompID;
	_asm
	{
		mov		ecx, dwPointer
        push    Status
		push	dwPannel
		call	dwFunction
	}
}

BYTE CDamageManagerSA::GetAeroplaneCompStatus( BYTE CompID )
{
	DEBUG_TRACE("BYTE CDamageManagerSA::GetAeroplaneCompStatus( BYTE CompID )");
	DWORD dwFunction = FUNC_GetAeroplaneCompStatus;
	DWORD dwPointer = (DWORD)internalInterface;
	BYTE bReturn = 0;
	DWORD dwPannel = CompID;
	_asm
	{
		mov		ecx, dwPointer
		push	dwPannel
		call	dwFunction
		mov		bReturn, al
	}
	return bReturn;
}


VOID CDamageManagerSA::FuckCarCompletely ( BOOL bKeepWheels )
{
	DEBUG_TRACE("VOID CDamageManagerSA::FuckCarCompletely ( BOOL bKeepWheels )");
	DWORD dwFunc = FUNC_FuckCarCompletely;
	DWORD dwPointer = (DWORD)internalInterface;
	_asm
	{
		mov		ecx, dwPointer
		push	bKeepWheels
		call	dwFunc
	}	
}