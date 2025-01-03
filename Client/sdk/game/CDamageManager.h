/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CDamageManager.h
 *  PURPOSE:     Damage manager interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

// Not sure if R* used these
#define DT_ENGINE_OK                    (0)
#define DT_ENGINE_RADIATOR_BURST        (100)
#define DT_ENGINE_ENGINE_OVERHEAT       (150)
#define DT_ENGINE_ENGINE_PIPES_BURST    (200)
#define DT_ENGINE_ON_FIRE               (225)
#define DT_ENGINE_FUCKED                (250)

/*
const unsigned char DT_ENG_INOPERABLE = 0x01;
const unsigned char DT_ENG_EXHAUST_SMOKE = 0x02;
const unsigned char DT_ENG_RADIATOR_STEAM = 0x04;
const unsigned char DT_ENG_ON_FIRE = 0x08;
*/

enum eWheelStatus
{
    DT_WHEEL_INTACT = 0,
    //  DT_WHEEL_CAP_MISSING,
    //  DT_WHEEL_WARPED,
    DT_WHEEL_BURST,
    DT_WHEEL_MISSING,

    // MTA custom state
    DT_WHEEL_INTACT_COLLISIONLESS,
};

enum eDoorStatus
{
    DT_DOOR_INTACT = 0,
    DT_DOOR_SWINGING_FREE,
    DT_DOOR_BASHED,
    DT_DOOR_BASHED_AND_SWINGING_FREE,
    DT_DOOR_MISSING
};

enum ePlaneComponentStatus
{
    DT_PLANE_INTACT = 0,
    DT_PLANE_BASHED,
    //  DT_PLANE_BASHED2,
    DT_PLANE_MISSING
};

enum eComponentStatus : std::uint8_t
{
    DT_PANEL_INTACT = 0,
    DT_PANEL_OPENED,
    DT_PANEL_DAMAGED,
    DT_PANEL_OPENED_DAMAGED,
    DT_PANEL_MISSING
};

enum eLightStatus
{
    DT_LIGHT_OK = 0,
    DT_LIGHT_SMASHED
};

enum eDoors
{
    BONNET = 0,
    BOOT,
    FRONT_LEFT_DOOR,
    FRONT_RIGHT_DOOR,
    REAR_LEFT_DOOR,
    REAR_RIGHT_DOOR,
    MAX_DOORS
};

enum eWheelPosition
{
    FRONT_LEFT_WHEEL = 0,
    REAR_LEFT_WHEEL,
    FRONT_RIGHT_WHEEL,
    REAR_RIGHT_WHEEL,

    MAX_WHEELS
};

enum ePanels
{
    FRONT_LEFT_PANEL = 0,
    FRONT_RIGHT_PANEL,
    REAR_LEFT_PANEL,
    REAR_RIGHT_PANEL,
    WINDSCREEN_PANEL,            // needs to be in same order as in component.h
    FRONT_BUMPER,
    REAR_BUMPER,

    MAX_PANELS            // MUST BE 8 OR LESS
};

enum eLights
{
    // these have to correspond to their respective panels
    LEFT_HEADLIGHT = 0,
    RIGHT_HEADLIGHT,
    LEFT_TAIL_LIGHT,
    RIGHT_TAIL_LIGHT,
    /*  LEFT_BRAKE_LIGHT,
        RIGHT_BRAKE_LIGHT,
        FRONT_LEFT_INDICATOR,
        FRONT_RIGHT_INDICATOR,
        REAR_LEFT_INDICATOR,
        REAR_RIGHT_INDICATOR,*/

    MAX_LIGHTS            // MUST BE 16 OR LESS
};

enum class eCarNodes
{
    NONE = 0,
    CHASSIS,
    WHEEL_RF,
    WHEEL_RM,
    WHEEL_RB,
    WHEEL_LF,
    WHEEL_LM,
    WHEEL_LB,
    DOOR_RF,
    DOOR_RR,
    DOOR_LF,
    DOOR_LR,
    BUMP_FRONT,
    BUMP_REAR,
    WING_RF,
    WING_LF,
    BONNET,
    BOOT,
    WINDSCREEN,
    EXHAUST,
    MISC_A,
    MISC_B,
    MISC_C,
    MISC_D,
    MISC_E,

    NUM_NODES
};

enum class eCarComponentCollisionTypes
{
    COL_NODE_BUMPER = 0,
    COL_NODE_WHEEL,
    COL_NODE_DOOR,
    COL_NODE_BONNET,
    COL_NODE_BOOT,
    COL_NODE_PANEL,

    COL_NODES_NUM
};

class CDamageManager
{
public:
    virtual BYTE          GetEngineStatus() = 0;
    virtual void          SetEngineStatus(BYTE bEngineState) = 0;
    virtual BYTE          GetDoorStatus(eDoors bDoor) = 0;
    virtual void          SetDoorStatus(eDoors bDoor, BYTE bDoorStatus, bool spawnFlyingComponent) = 0;
    virtual BYTE          GetWheelStatus(eWheelPosition bTire) = 0;
    virtual void          SetWheelStatus(eWheelPosition bTire, BYTE bTireStatus) = 0;
    virtual BYTE          GetPanelStatus(BYTE bPanel) const = 0;
    virtual unsigned long GetPanelStatus() = 0;
    virtual void          SetPanelStatus(BYTE bPanel, BYTE bPanelStatus, bool spawnFlyingComponent = true, bool breakGlass = false) = 0;
    virtual void          SetPanelStatus(unsigned long ulStatus, bool spawnFlyingComponent = true, bool breakGlass = false) = 0;
    virtual BYTE          GetLightStatus(BYTE bLight) = 0;
    virtual unsigned char GetLightStatus() = 0;
    virtual void          SetLightStatus(BYTE bLight, BYTE bLightStatus) = 0;
    virtual void          SetLightStatus(unsigned char ucStatus) = 0;
    virtual BYTE          GetAeroplaneCompStatus(BYTE CompID) = 0;
    virtual void          SetAeroplaneCompStatus(BYTE CompID, BYTE Status) = 0;            // component ids begin at 12 - probably

    virtual void FuckCarCompletely(bool bKeepWheels) = 0;
};
