/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CNetAPI.h
*  PURPOSE:     Header for net API class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*               Kent Simon <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CNetAPI;

#ifndef __CNETAPI__
#define __CNETAPI__

#include "CClientCommon.h"
#include "CClientManager.h"
#include "CInterpolator.h"
#include "CBitStream.h"
#include <ctime>

// SYNC SETTINGS
#define TICK_RATE 100
#define CAM_SYNC_RATE 500

enum eServerRPCFunctions;

class CNetAPI
{
public:
                            CNetAPI                         ( CClientManager * pManager);

    void                    DoPulse                         ( void );
    bool                    ProcessPacket                   ( unsigned char bytePacketID, NetBitStreamInterface &bitStream );

    void                    ResetReturnPosition             ( void );

    void                    AddInterpolation                ( const CVector& vecPosition );
    bool                    GetInterpolation                ( CVector& vecPosition, unsigned short usLatency );

	static bool             IsWeaponIDAkimbo				( unsigned char ucWeaponID );
    static bool             IsDriveByWeapon                 ( unsigned char ucWeaponID );

private:
    bool                    IsSmallKeySyncNeeded            ( CClientPed* pPed );
    bool                    IsPureSyncNeeded                ( void );

    void                    ReadKeysync                     ( CClientPlayer* pPlayer, NetBitStreamInterface& BitStream );
    void                    WriteKeysync                    ( CClientPed* pPed, NetBitStreamInterface& BitStream );

    void                    ReadPlayerPuresync              ( CClientPlayer* pPlayer, NetBitStreamInterface& BitStream );
    void                    WritePlayerPuresync             ( CClientPlayer* pPed, NetBitStreamInterface& BitStream );

    void                    ReadVehiclePuresync             ( CClientPlayer* pPlayer, CClientVehicle* pVehicle, NetBitStreamInterface& BitStream );
    void                    WriteVehiclePuresync            ( CClientPed* pPed, CClientVehicle* pVehicle, NetBitStreamInterface& BitStream );

    bool                    ReadSmallKeysync                ( CControllerState& ControllerState, const CControllerState& LastControllerState, NetBitStreamInterface& BitStream );
    void                    WriteSmallKeysync               ( const CControllerState& ControllerState, const CControllerState& LastControllerState, NetBitStreamInterface& BitStream );

    bool                    ReadFullKeysync                 ( CControllerState& ControllerState, NetBitStreamInterface& BitStream );
    void                    WriteFullKeysync                ( const CControllerState& ControllerState, NetBitStreamInterface& BitStream );

    void                    ReadSmallVehicleSpecific        ( CClientVehicle* pVehicle, NetBitStreamInterface& BitStream );
    void                    WriteSmallVehicleSpecific       ( CClientVehicle* pVehicle, NetBitStreamInterface& BitStream );

    void                    ReadFullVehicleSpecific         ( CClientVehicle* pVehicle, NetBitStreamInterface& BitStream );
    void                    WriteFullVehicleSpecific        ( CClientVehicle* pVehicle, NetBitStreamInterface& BitStream );

public:
    bool                    IsCameraSyncNeeded              ( void );
    void                    WriteCameraSync                 ( NetBitStreamInterface& BitStream );

    void                    RPC                             ( eServerRPCFunctions ID, NetBitStreamInterface * pBitStream = NULL, NetPacketOrdering packetOrdering = PACKET_ORDERING_GAME );

private:
    CClientManager*         m_pManager;
    CClientPlayerManager*   m_pPlayerManager;
    CClientVehicleManager*  m_pVehicleManager;
    unsigned long           m_ulLastPuresyncTime;
    unsigned long           m_ulLastSyncReturnTime;    

    bool                    m_bStoredReturnSync;
    bool                    m_bVehicleLastReturn;
    CVector                 m_vecLastReturnPosition;
    CVector                 m_vecLastReturnRotation;

    unsigned long           m_ulLastCameraSyncTime;
    bool                    m_bLastSentCameraMode;
    CClientEntity*          m_pLastSentCameraTarget;
    CVector                 m_vecLastSentCameraPosition;
    CVector                 m_vecLastSentCameraLookAt;

    CInterpolator<CVector>  m_Interpolator;
};

enum eServerRPCFunctions
{
    PLAYER_INGAME_NOTICE,
    PLAYER_TARGET,
    PLAYER_WEAPON,
    KEY_BIND,
    CURSOR_EVENT,
    REQUEST_STEALTH_KILL,
};

#endif