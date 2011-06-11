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
    enum
    {
        NUM_DELTA_CONTEXTS = 4,
        DELTA_CONTEXT_BITCOUNT = NumberOfSignificantBits<(NUM_DELTA_CONTEXTS-1)>::COUNT
    };

    SPlayerDeltaSyncData ()
    {
        memset ( this, 0, sizeof(SPlayerDeltaSyncData) );
    }

    // In delta sync, we are sending full packets followed by a set of delta packets that
    // define changes on this full packet. These delta packets should be linked somehow
    // to their corresponding full packet, to define what packet are they defining a change of.
    // For this purpouse, we will use the delta sync context which will be increased every time
    // that a full packet is sent, so the following delta packets will carry this context.
    //
    // It is safe to accept packets for a context that is higher than the current one, as the full
    // frame could have been lost due to packetloss and we can apply the changes because anyway
    // the full packet might arrive at some point. Packets with a lower context will only introduce
    // corruption, so they must be dropped.
    unsigned char           deltaSyncContext;
    bool IsValidDelta ( unsigned char ucDelta ) const
    {
        unsigned char ucDeltaSyncContext = deltaSyncContext & (NUM_DELTA_CONTEXTS-1);
        if ( ucDelta >= ucDeltaSyncContext || ( ucDelta == 0 && ucDeltaSyncContext == NUM_DELTA_CONTEXTS-1 ) )
            return true;
        return false;
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
