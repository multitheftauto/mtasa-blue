/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CRPCFunctions.h
*  PURPOSE:     Header for RPC functions class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CRPCFunctions_H
#define __CRPCFunctions_H

#include <vector>

class CRPCFunctions;

#define DECLARE_RPC(a) static void a ( class NetBitStreamInterface& bitStream );

class CRPCFunctions
{
protected:
    enum eRPCFunctions;
private:

    typedef void (*pfnRPCHandler) ( class NetBitStreamInterface& bitStream );
    struct SRPCHandler
    {
        unsigned char ID;
        pfnRPCHandler Callback;
        char szName [ 32 ];
    };

public:
                                CRPCFunctions                           ( class CClientGame* pClientGame );
    virtual                     ~CRPCFunctions                          ( void );

    void                        AddHandlers                             ( void );
    static void                 AddHandler                              ( unsigned char ucID, pfnRPCHandler Callback, char * szName = "unknown" );
    void                        ProcessPacket                           ( class NetBitStreamInterface& bitStream );

    bool                        m_bShowRPCs;

protected:
    static class CClientManager*                m_pManager;
    static class CClientCamera*                 m_pCamera;
    static class CClientMarkerManager*          m_pMarkerManager;
    static class CClientObjectManager*          m_pObjectManager;
    static class CClientPickupManager*          m_pPickupManager;
    static class CClientPlayerManager*          m_pPlayerManager;
    static class CClientRadarAreaManager*       m_pRadarAreaManager;
    static class CClientRadarMarkerManager*     m_pRadarMarkerManager;
    static class CClientDisplayManager*         m_pDisplayManager;
    static class CClientVehicleManager*         m_pVehicleManager;
    static class CClientPathManager*            m_pPathManager;
    static class CClientTeamManager*            m_pTeamManager;
    static class CClientPedManager*             m_pPedManager;

    static class CBlendedWeather*               m_pBlendedWeather;
    static class CClientGame*                   m_pClientGame;

private:
    vector < SRPCHandler * >                    m_RPCHandlers;

protected:
    enum eRPCFunctions
    {
        DONT_USE_0 = 0,
        SET_TIME,
        SET_WEATHER,
        SET_WEATHER_BLENDED,
		SET_MINUTE_DURATION,

        SET_ELEMENT_PARENT,
        SET_ELEMENT_DATA,
        SET_ELEMENT_POSITION,
        SET_ELEMENT_VELOCITY,
        SET_ELEMENT_INTERIOR,
        SET_ELEMENT_DIMENSION,
        ATTACH_ELEMENTS,
        DETACH_ELEMENTS,
        SET_ELEMENT_ALPHA,
		SET_ELEMENT_NAME,
        SET_ELEMENT_HEALTH,
        SET_ELEMENT_MODEL,
        
        SET_PLAYER_MONEY,            
        SHOW_PLAYER_HUD_COMPONENT,
        FORCE_PLAYER_MAP,
        SET_PLAYER_NAMETAG_TEXT,
        REMOVE_PLAYER_NAMETAG_COLOR,
        SET_PLAYER_NAMETAG_COLOR,
        SET_PLAYER_NAMETAG_SHOWING,              	

        SET_PED_ARMOR,
        SET_PED_ROTATION,
        GIVE_PED_JETPACK,
        REMOVE_PED_JETPACK,
        REMOVE_PED_CLOTHES,
        SET_PED_GRAVITY,
        SET_PED_CHOKING,
        SET_PED_FIGHTING_STYLE,
        SET_PED_MOVE_ANIM,
        WARP_PED_INTO_VEHICLE,
        REMOVE_PED_FROM_VEHICLE,
        SET_PED_DOING_GANG_DRIVEBY,
        SET_PED_ANIMATION,
        SET_PED_ON_FIRE,
        SET_PED_HEADLESS,

        DESTROY_ALL_VEHICLES,
        FIX_VEHICLE,
        BLOW_VEHICLE,
        SET_VEHICLE_ROTATION,
        SET_VEHICLE_TURNSPEED,
        SET_VEHICLE_COLOR,
        SET_VEHICLE_LOCKED,
        SET_VEHICLE_DOORS_UNDAMAGEABLE,
        SET_VEHICLE_SIRENE_ON,
        SET_VEHICLE_LANDING_GEAR_DOWN,
        SET_HELICOPTER_ROTOR_SPEED,
		ADD_VEHICLE_UPGRADE,
        ADD_ALL_VEHICLE_UPGRADES,
	    REMOVE_VEHICLE_UPGRADE,
	    SET_VEHICLE_DAMAGE_STATE,
        SET_VEHICLE_OVERRIDE_LIGHTS,
        SET_VEHICLE_ENGINE_STATE,
        SET_VEHICLE_DIRT_LEVEL,
        SET_VEHICLE_DAMAGE_PROOF,
        SET_VEHICLE_PAINTJOB,
        SET_VEHICLE_FUEL_TANK_EXPLODABLE,
        SET_VEHICLE_WHEEL_STATES,
        SET_VEHICLE_FROZEN,
        SET_TRAIN_DERAILED,

        GIVE_WEAPON,
        TAKE_WEAPON,
        TAKE_ALL_WEAPONS,
		GIVE_WEAPON_AMMO,
        TAKE_WEAPON_AMMO,
        SET_WEAPON_AMMO,
		SET_WEAPON_SLOT,

        DESTROY_ALL_BLIPS,
        SET_BLIP_ICON,
        SET_BLIP_SIZE,
        SET_BLIP_COLOR,
        SET_BLIP_ORDERING,

        DESTROY_ALL_OBJECTS,
        SET_OBJECT_ROTATION,
        MOVE_OBJECT,
        STOP_OBJECT,

        DESTROY_ALL_RADAR_AREAS,
        SET_RADAR_AREA_SIZE,
        SET_RADAR_AREA_COLOR,
        SET_RADAR_AREA_FLASHING,

        DESTROY_ALL_MARKERS,
        SET_MARKER_TYPE,
        SET_MARKER_COLOR,
        SET_MARKER_SIZE,
        SET_MARKER_TARGET,
        SET_MARKER_ICON,

        DESTROY_ALL_PICKUPS,
        SET_PICKUP_TYPE,
        SET_PICKUP_VISIBLE,

		PLAY_SOUND,

        BIND_KEY,
        UNBIND_KEY,
        TOGGLE_CONTROL_ABILITY,
        TOGGLE_ALL_CONTROL_ABILITY,
        SET_CONTROL_STATE,
        FORCE_RECONNECT,

        SET_TEAM_NAME,
        SET_TEAM_COLOR,
        SET_PLAYER_TEAM,
        SET_TEAM_FRIENDLY_FIRE,

        SET_WANTED_LEVEL,

        SET_CAMERA_MATRIX,
        SET_CAMERA_TARGET,
        SET_CAMERA_INTERIOR,
        FADE_CAMERA,

        SHOW_CURSOR,
		SHOW_CHAT,

        SET_GRAVITY,
        SET_GAME_SPEED,
        SET_WAVE_HEIGHT,
        SET_SKY_GRADIENT,
        RESET_SKY_GRADIENT,
        SET_BLUR_LEVEL,
        SET_FPS_LIMIT,
        SET_GARAGE_OPEN,
        RESET_MAP_INFO,

        REMOVE_ELEMENT_DATA,

        HANDLING_SET_DEFAULT,
        HANDLING_RESTORE_DEFAULT,
        HANDLING_SET_PROPERTY,

        TOGGLE_DEBUGGER,

        NUM_RPC_FUNCS,
    };
};

#endif