/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CNetAPI.h
 *  PURPOSE:     Header for net API class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

class CNetAPI;

#pragma once

#include "CClientCommon.h"
#include "CClientManager.h"
#include "CInterpolator.h"
#include "CBitStream.h"
#include <ctime>
#include "CTickRateSettings.h"

// SYNC SETTINGS
#define TICK_RATE       ( g_TickRateSettings.iPureSync )
#define CAM_SYNC_RATE   ( g_TickRateSettings.iCamSync )
#define TICK_RATE_AIM   ( std::min ( TICK_RATE, g_TickRateSettings.iKeySyncRotation ) )  // Keysync or puresync update the aim, so use the shortest interval

enum eServerRPCFunctions
{
    PLAYER_INGAME_NOTICE,
    INITIAL_DATA_STREAM,
    PLAYER_TARGET,
    PLAYER_WEAPON,
    KEY_BIND,
    CURSOR_EVENT,
    REQUEST_STEALTH_KILL,
};

class CNetAPI
{
public:
    ZERO_ON_NEW
    CNetAPI(CClientManager* pManager);

    void DoPulse();
    bool ProcessPacket(unsigned char bytePacketID, NetBitStreamInterface& bitStream);

    void ResetReturnPosition();

    void AddInterpolation(const CVector& vecPosition);
    bool GetInterpolation(CVector& vecPosition, unsigned short usLatency);
    void SendBulletSyncFire(eWeaponType weaponType, const CVector& vecStart, const CVector& vecEnd, float fDamage, uchar ucHitZone,
                            CClientPlayer* pRemoteDamagedPlayer);
    void SendBulletSyncCustomWeaponFire(CClientWeapon* pWeapon, const CVector& vecStart, const CVector& vecEnd);
    bool IsNetworkTrouble() { return m_bIsNetworkTrouble; }

    static bool IsWeaponIDAkimbo(unsigned char ucWeaponID);
    static bool IsDriveByWeapon(unsigned char ucWeaponID);

private:
    bool IsSmallKeySyncNeeded(CClientPed* pPed);
    bool IsPureSyncNeeded();

    void ReadKeysync(CClientPlayer* pPlayer, NetBitStreamInterface& BitStream);
    void WriteKeysync(CClientPed* pPed, NetBitStreamInterface& BitStream);

    void ReadBulletsync(CClientPlayer* pPlayer, NetBitStreamInterface& BitStream);
    void ReadWeaponBulletsync(CClientPlayer* pWeapon, NetBitStreamInterface& BitStream);

    void ReadPlayerPuresync(CClientPlayer* pPlayer, NetBitStreamInterface& BitStream);
    void WritePlayerPuresync(CClientPlayer* pPed, NetBitStreamInterface& BitStream);

    void ReadVehiclePuresync(CClientPlayer* pPlayer, CClientVehicle* pVehicle, NetBitStreamInterface& BitStream);
    void WriteVehiclePuresync(CClientPed* pPed, CClientVehicle* pVehicle, NetBitStreamInterface& BitStream);

    bool ReadSmallKeysync(CControllerState& ControllerState, NetBitStreamInterface& BitStream);
    void WriteSmallKeysync(const CControllerState& ControllerState, NetBitStreamInterface& BitStream);

    bool ReadFullKeysync(CControllerState& ControllerState, NetBitStreamInterface& BitStream);
    void WriteFullKeysync(const CControllerState& ControllerState, NetBitStreamInterface& BitStream);

    void ReadSmallVehicleSpecific(CClientVehicle* pVehicle, NetBitStreamInterface& BitStream, int iRemoteModelID);
    void WriteSmallVehicleSpecific(CClientVehicle* pVehicle, NetBitStreamInterface& BitStream);

    void ReadFullVehicleSpecific(CClientVehicle* pVehicle, NetBitStreamInterface& BitStream, int iRemoteModelID);
    void WriteFullVehicleSpecific(CClientVehicle* pVehicle, NetBitStreamInterface& BitStream);

    void ReadLightweightSync(CClientPlayer* pPlayer, NetBitStreamInterface& BitStream);
    void ReadVehicleResync(CClientVehicle* pVehicle, NetBitStreamInterface& BitStream);

    void GetLastSentControllerState(CControllerState* pControllerState, float* pfCameraRotation, float* pfLastAimY);
    void SetLastSentControllerState(const CControllerState& ControllerState, float fCameraRotation, float fLastAimY);

    void ReadVehiclePartsState(CClientVehicle* pVehicle, NetBitStreamInterface& BitStream);

public:
    bool IsCameraSyncNeeded();
    void WriteCameraSync(NetBitStreamInterface& BitStream);

    void RPC(eServerRPCFunctions ID, NetBitStreamInterface* pBitStream = NULL);

private:
    CClientManager*        m_pManager;
    CClientPlayerManager*  m_pPlayerManager;
    CClientVehicleManager* m_pVehicleManager;
    unsigned long          m_ulLastPuresyncTime;
    unsigned long          m_ulLastSyncReturnTime;

    bool    m_bStoredReturnSync;
    bool    m_bVehicleLastReturn;
    CVector m_vecLastReturnPosition;
    CVector m_vecLastReturnRotation;

    CElapsedTime m_CameraSyncTimer;

    CInterpolator<CVector> m_Interpolator;

    bool         m_bIsNetworkTrouble;
    bool         m_bIncreaseTimeoutTime;
    CElapsedTime m_IncreaseTimeoutTimeTimer;

    CElapsedTime     m_TimeSinceMouseOrAnalogStateSent;
    CControllerState m_LastSentControllerState;
    float            m_fLastSentCameraRotation;
    float            m_fLastSentAimY;
    uchar            m_ucBulletSyncOrderCounter;
    uchar            m_ucCustomWeaponBulletSyncOrderCounter;
};
