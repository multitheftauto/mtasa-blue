/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPedSync.h
 *  PURPOSE:     Header for ped sync class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CClientCommon.h>
#include "CClientVehicle.h"

class CPedSync
{
public:
    CPedSync(CClientPedManager* pPedManager);
    ~CPedSync();

    bool ProcessPacket(unsigned char ucPacketID, NetBitStreamInterface& bitStream);
    void DoPulse();

    void AddPed(CClientPed* pPed);
    void RemovePed(CClientPed* pPed);

    std::list<CClientPed*>::const_iterator IterBegin() { return m_List.begin(); };
    std::list<CClientPed*>::const_iterator IterEnd() { return m_List.end(); };
    CMappedList<CClientPed*>               GetList() { return m_List; };

    bool Exists(CClientPed* pPed);

private:
    void Packet_PedStartSync(NetBitStreamInterface& BitStream);
    void Packet_PedStopSync(NetBitStreamInterface& BitStream);
    void Packet_PedSync(NetBitStreamInterface& BitStream);

    void Update();
    void WritePedInformation(NetBitStreamInterface* pBitStream, CClientPed* pPed);

    CClientPedManager*       m_pPedManager;
    CMappedList<CClientPed*> m_List;
    unsigned long            m_ulLastSyncTime;
};
