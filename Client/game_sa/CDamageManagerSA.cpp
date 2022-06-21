/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CDamageManagerSA.cpp
 *  PURPOSE:     Vehicle damage manager
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

BYTE CDamageManagerSA::GetEngineStatus()
{
    DEBUG_TRACE("BYTE CDamageManagerSA::GetEngineStatus (  )");
    return internalInterface->bEngineStatus;
}

VOID CDamageManagerSA::SetEngineStatus(BYTE bEngineState)
{
    DEBUG_TRACE("VOID CDamageManagerSA::SetEngineStatus ( BYTE bEngineState )");
    if (bEngineState > 250)
        bEngineState = 250;
    internalInterface->bEngineStatus = bEngineState;
}

BYTE CDamageManagerSA::GetDoorStatus(eDoors bDoor)
{
    DEBUG_TRACE("BYTE CDamageManagerSA::GetDoorStatus ( eDoors bDoor )");
    if (bDoor < MAX_DOORS)
        return internalInterface->Door[bDoor];
    return NULL;
}

VOID CDamageManagerSA::SetDoorStatus(eDoors bDoor, BYTE bDoorStatus, bool spawnFlyingComponent)
{
    DEBUG_TRACE("VOID CDamageManagerSA::SetDoorStatus ( eDoors bDoor, BYTE bDoorStatus, bool spawnFlyingComponent )");

    if (bDoor < MAX_DOORS)
    {
        // Different from before?
        if (internalInterface->Door[bDoor] != bDoorStatus)
        {
            // Set it
            internalInterface->Door[bDoor] = bDoorStatus;

            CAutomobileSAInterface* pAutomobile = (CAutomobileSAInterface*)internalEntityInterface;

            // Are we making it intact?
            if (bDoorStatus == DT_DOOR_INTACT || bDoorStatus == DT_DOOR_SWINGING_FREE)
            {
                // Grab the car node index for the given door id
                static int s_iCarNodeIndexes[6] = {0x10, 0x11, 0x0A, 0x08, 0x0B, 0x09};

                // Call CAutomobile::FixDoor to update the model
                // CAutomobile::FixDoor
                ((void(__thiscall*)(CAutomobileSAInterface*, int, int))(0x6A35A0))(pAutomobile, s_iCarNodeIndexes[bDoor], bDoor);
            }
            else
            {
                // Call CAutomobile::SetDoorDamage to update the model
                // CAutomobile::SetDoorDamage
                ((void(__thiscall*)(CAutomobileSAInterface*, int, bool))(0x6B1600))(pAutomobile, bDoor, !spawnFlyingComponent);
            }
        }
    }
}

BYTE CDamageManagerSA::GetWheelStatus(eWheelPosition bWheel)
{
    DEBUG_TRACE("BYTE CDamageManagerSA::GetWheelStatus ( eWheelPosition bWheel )");
    if (bWheel < MAX_WHEELS)
        return internalInterface->Wheel[bWheel];
    return NULL;
}

VOID CDamageManagerSA::SetWheelStatus(eWheelPosition bWheel, BYTE bTireStatus)
{
    DEBUG_TRACE("VOID CDamageManagerSA::SetWheelStatus ( eWheelPosition bWheel, BYTE bTireStatus )");
    if (bWheel < MAX_WHEELS)
    {
        // Different than before?
        if (internalInterface->Wheel[bWheel] != bTireStatus)
        {
            internalInterface->Wheel[bWheel] = bTireStatus;
        }
    }
}

VOID CDamageManagerSA::SetPanelStatus(BYTE bPanel, BYTE bPanelStatus)
{
    DEBUG_TRACE("BYTE CDamageManagerSA::SetPanelStatus ( BYTE bLight, BYTE bPanelStatus )");

    // Valid index?
    if (bPanel < MAX_PANELS && bPanelStatus <= 3)
    {
        // Different than already?
        if (GetPanelStatus(bPanel) != bPanelStatus)
        {
            // CDamageManager::SetPanelStatus
            ((void(__thiscall*)(CDamageManagerSAInterface*, unsigned char, int))(FUNC_SetPanelStatus))(internalInterface, bPanel, bPanelStatus);

            CAutomobileSAInterface* pAutomobile = (CAutomobileSAInterface*)internalEntityInterface;

            // Intact?
            if (bPanelStatus == DT_PANEL_INTACT)
            {
                // Grab the car node index for the given panel
                static int s_iCarNodeIndexes[7] = {0x0F, 0x0E, 0x00 /*?*/, 0x00 /*?*/, 0x12, 0x0C, 0x0D};

                //  Call CAutomobile::FixPanel to update the vehicle
                // CAutomobile::FixPanel
                ((void(__thiscall*)(CAutomobileSAInterface*, int, int))(0x6A3670))(pAutomobile, s_iCarNodeIndexes[bPanel], bPanel);
            }
            else
            {
                // Call CAutomobile::SetPanelDamage to update the vehicle

                // CAutomobile::SetPanelDamage
                ((void(__thiscall*)(CAutomobileSAInterface*, int, bool))(0x6B1480))(pAutomobile, bPanel, false);
            }
        }
    }
}

void CDamageManagerSA::SetPanelStatus(unsigned long ulStatus)
{
    unsigned int uiIndex;

    for (uiIndex = 0; uiIndex < MAX_PANELS; uiIndex++)
    {
        SetPanelStatus(static_cast<eDoors>(uiIndex), static_cast<unsigned char>(ulStatus));
        ulStatus >>= 4;
    }
}

BYTE CDamageManagerSA::GetPanelStatus(BYTE bPanel)
{
    if (bPanel < MAX_PANELS)
    {
        DEBUG_TRACE("BYTE CDamageManagerSA::GetPanelStatus ( BYTE bPannel )");

        // CDamageManager::GetPanelStatus
        return ((BYTE(__thiscall*)(CDamageManagerSAInterface*, int))(FUNC_GetPanelStatus))(internalInterface, bPanel);
    }

    return 0;
}

unsigned long CDamageManagerSA::GetPanelStatus()
{
    return internalInterface->Panels;
}

VOID CDamageManagerSA::SetLightStatus(BYTE bLight, BYTE bLightStatus)
{
    DEBUG_TRACE("BYTE CDamageManagerSA::SetLightStatus ( BYTE bLight, BYTE bLightStatus )");

    // CDamageManager::SetLightStatus
    ((void(__thiscall*)(CDamageManagerSAInterface*, int, int))(FUNC_SetLightStatus))(internalInterface, bLight, bLightStatus);
}

void CDamageManagerSA::SetLightStatus(unsigned char ucStatus)
{
    internalInterface->Lights = static_cast<unsigned long>(ucStatus);
}

BYTE CDamageManagerSA::GetLightStatus(BYTE bLight)
{
    DEBUG_TRACE("BYTE CDamageManagerSA::GetLightStatus ( BYTE bLight )");

    // CDamageManager::SetLightStatus
    return ((BYTE(__thiscall*)(CDamageManagerSAInterface*, int))(FUNC_GetLightStatus))(internalInterface, bLight);
}

unsigned char CDamageManagerSA::GetLightStatus()
{
    return static_cast<unsigned char>(internalInterface->Lights);
}

VOID CDamageManagerSA::SetAeroplaneCompStatus(BYTE CompID, BYTE Status)
{
    DEBUG_TRACE("VOID CDamageManagerSA::SetAeroplaneCompStatus( BYTE CompID, BYTE Status)");

    // CDamageManager::SetAeroplaneCompStatus
    ((void(__thiscall*)(CDamageManagerSAInterface*, int, uint))(FUNC_SetAeroplaneCompStatus))(internalInterface, CompID, Status);
}

BYTE CDamageManagerSA::GetAeroplaneCompStatus(BYTE CompID)
{
    DEBUG_TRACE("BYTE CDamageManagerSA::GetAeroplaneCompStatus( BYTE CompID )");

    // CDamageManager::GetAeroplaneCompStatus
    return ((BYTE(__thiscall*)(CDamageManagerSAInterface*, int))(FUNC_GetAeroplaneCompStatus))(internalInterface, CompID);
}

VOID CDamageManagerSA::FuckCarCompletely(BOOL bKeepWheels)
{
    DEBUG_TRACE("VOID CDamageManagerSA::FuckCarCompletely ( BOOL bKeepWheels )");
    // CDamageManager::FuckCarCompletely
    ((void(__thiscall*)(CDamageManagerSAInterface*, bool))(FUNC_FuckCarCompletely))(internalInterface, bKeepWheels);
}
