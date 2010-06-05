/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CDamageManagerSA.h
*  PURPOSE:     Header file for vehicle damage manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_DAMAGEMANAGER
#define __CGAMESA_DAMAGEMANAGER

#include <game/CDamageManager.h>
#include "Common.h"

#define FUNC_GetEngineStatus        0x6c22c0 
#define FUNC_SetEngineStatus        0x6c22a0
#define FUNC_GetDoorStatus          0x6c2230
#define FUNC_SetDoorStatus          0x6c21c0
#define FUNC_GetTireStatus          0x6c21b0
#define FUNC_SetTireStatus          0x6c21a0
#define FUNC_GetPanelStatus         0x6c2180
#define FUNC_SetPanelStatus         0x6c2150
#define FUNC_SetLightStatus         0x6c2100
#define FUNC_GetLightStatus         0x6c2130
#define FUNC_SetAeroplaneCompStatus 0x6C22D0
#define FUNC_GetAeroplaneCompStatus 0x6C2300

//006c25d0      public: void __thiscall CDamageManager::FuckCarCompletely(bool)
#define FUNC_FuckCarCompletely      0x6c25d0

#define MAX_DOORS                   6   // also in CAutomobile
#define MAX_WHEELS                  4

class CDamageManagerSAInterface // 28 bytes due to the way its packed (24 containing actual data)
{
public:
    FLOAT           fWheelDamageEffect;
    BYTE            bEngineStatus;      // old - wont be used
    BYTE            Wheel[MAX_WHEELS];
    BYTE            Door[MAX_DOORS];
    DWORD           Lights;             // 2 bits per light
    DWORD           Panels;             // 4 bits per panel
};

class CDamageManagerSA : public CDamageManager
{
private:
    CDamageManagerSAInterface           * internalInterface;
    class CEntitySAInterface*           internalEntityInterface;
public: 
    BYTE            GetEngineStatus         (  );
    VOID            SetEngineStatus         ( BYTE bEngineState );
    BYTE            GetDoorStatus           ( eDoors bDoor );
    VOID            SetDoorStatus           ( eDoors bDoor, BYTE bDoorStatus );
    BYTE            GetWheelStatus          ( eWheels bWheel );
    VOID            SetWheelStatus          ( eWheels bWheel, BYTE bTireStatus );
    BYTE            GetPanelStatus          ( BYTE bPanel );
    unsigned long   GetPanelStatus          ( void );
    VOID            SetPanelStatus          ( BYTE bPanel, BYTE bPanelStatus );
    void            SetPanelStatus          ( unsigned long ulStatus );
    BYTE            GetLightStatus          ( BYTE bLight );
    unsigned char   GetLightStatus          ( void );
    VOID            SetLightStatus          ( BYTE bLight, BYTE bLightStatus );
    void            SetLightStatus          ( unsigned char ucStatus );
    BYTE            GetAeroplaneCompStatus  ( BYTE CompID );
    VOID            SetAeroplaneCompStatus  ( BYTE CompID, BYTE Status);

    VOID            FuckCarCompletely       ( BOOL bKeepWheels );

    CDamageManagerSA ( class CEntitySAInterface* intEntityInterface, CDamageManagerSAInterface * intInterface) { internalEntityInterface = intEntityInterface; internalInterface = intInterface; };
};

#endif
