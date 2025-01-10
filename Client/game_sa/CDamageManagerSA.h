/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CDamageManagerSA.h
 *  PURPOSE:     Header file for vehicle damage manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CDamageManager.h>

#define FUNC_GetPanelStatus         0x6c2180
#define FUNC_SetPanelStatus         0x6c2150
#define FUNC_SetLightStatus         0x6c2100
#define FUNC_GetLightStatus         0x6c2130
#define FUNC_SetAeroplaneCompStatus 0x6C22D0
#define FUNC_GetAeroplaneCompStatus 0x6C2300
#define FUNC_FuckCarCompletely      0x6c25d0

#define MAX_DOORS                   6   // also in CAutomobile
#define MAX_WHEELS                  4

class CDamageManagerSAInterface            // 28 bytes due to the way its packed (24 containing actual data)
{
public:
    std::uint8_t GetPanelStatus(std::uint8_t panelId)
    {
        if (panelId >= MAX_PANELS)
            return 0;

        return ((std::uint8_t(__thiscall*)(CDamageManagerSAInterface*, std::uint8_t))FUNC_GetPanelStatus)(this, panelId);
    }

    float fWheelDamageEffect;
    BYTE  bEngineStatus;            // old - wont be used
    BYTE  Wheel[MAX_WHEELS];
    BYTE  Door[MAX_DOORS];
    DWORD Lights;            // 2 bits per light
    DWORD Panels;            // 4 bits per panel
};
static_assert(sizeof(CDamageManagerSAInterface) == 0x18, "Invalid size for CDamageManagerSAInterface");

class CDamageManagerSA : public CDamageManager
{
private:
    CDamageManagerSAInterface* internalInterface;
    class CEntitySAInterface*  internalEntityInterface;

public:
    BYTE          GetEngineStatus();
    void          SetEngineStatus(BYTE bEngineState);
    BYTE          GetDoorStatus(eDoors bDoor);
    void          SetDoorStatus(eDoors bDoor, BYTE bDoorStatus, bool spawnFlyingComponent);
    BYTE          GetWheelStatus(eWheelPosition bWheel);
    void          SetWheelStatus(eWheelPosition bWheel, BYTE bTireStatus);
    BYTE          GetPanelStatus(BYTE bPanel) const;
    unsigned long GetPanelStatus();
    void          SetPanelStatus(BYTE bPanel, BYTE bPanelStatus, bool spawnFlyingComponent = true, bool breakGlass = false);
    void          SetPanelStatus(unsigned long ulStatus, bool spawnFlyingComponent = true, bool breakGlass = false);
    BYTE          GetLightStatus(BYTE bLight);
    unsigned char GetLightStatus();
    void          SetLightStatus(BYTE bLight, BYTE bLightStatus);
    void          SetLightStatus(unsigned char ucStatus);
    BYTE          GetAeroplaneCompStatus(BYTE CompID);
    void          SetAeroplaneCompStatus(BYTE CompID, BYTE Status);

    void FuckCarCompletely(bool bKeepWheels);

    static int GetCarNodeIndexFromPanel(std::uint8_t panelId) noexcept;

    CDamageManagerSA(class CEntitySAInterface* intEntityInterface, CDamageManagerSAInterface* intInterface)
    {
        internalEntityInterface = intEntityInterface;
        internalInterface = intInterface;
    };
};
