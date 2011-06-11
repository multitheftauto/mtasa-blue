/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/sdk/net/DeltaSync.h
*  PURPOSE:     Header for network delta synchronisation structures
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>s
*               
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#include "SyncStructures.h"

struct SPlayerDeltaSyncData
{
    SPlayerDeltaSyncData ()
    {
        memset ( this, 0, sizeof(SPlayerDeltaSyncData) );
    }

    unsigned long           lastFullPacket;
    CControllerState        lastControllerState;
    SPlayerPuresyncFlags    lastFlags;
    ElementID               lastContact;
    CVector                 lastPosition;
    unsigned char           lastSyncTimeContext;
    float                   lastHealth;
    float                   lastArmor;
    unsigned short          lastLatency;

    struct
    {
        unsigned char       lastWeaponType;
        unsigned int        lastSlot;
        unsigned int        lastAmmoInClip;
        unsigned int        lastAmmoTotal;
    } weapon;

    struct
    {
        bool                lastWasVehicleSync;
    } vehicle;
};

struct SLightweightSyncData
{
    SLightweightSyncData ()
    {
        health.uiContext = 0;
        health.bSync = false;
        vehicleHealth.uiContext = 0;
        vehicleHealth.bSync = false;
    }

    struct
    {
        float           fLastHealth;
        float           fLastArmor;
        bool            bSync;
        unsigned int    uiContext;
    } health;

    struct
    {
        CVehicle*       lastVehicle;
        float           fLastHealth;
        bool            bSync;
        unsigned int    uiContext;
    } vehicleHealth;
};
