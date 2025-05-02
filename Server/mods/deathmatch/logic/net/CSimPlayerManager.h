/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

class CSimPlayerManager
{
public:
    ZERO_ON_NEW

    // Main thread methods
    void AddSimPlayer(CPlayer* pPlayer);
    void RemoveSimPlayer(CPlayer* pPlayer);
    void UpdateSimPlayer(CPlayer* pPlayer);

    // Any thread methods
    void LockSimSystem();
    void UnlockSimSystem();

    // Sync thread methods
    bool        HandlePlayerPureSync(const NetServerPlayerID& Socket, NetBitStreamInterface* BitStream);
    bool        HandleVehiclePureSync(const NetServerPlayerID& Socket, NetBitStreamInterface* BitStream);
    bool        HandleKeySync(const NetServerPlayerID& Socket, NetBitStreamInterface* BitStream);
    bool        HandleBulletSync(const NetServerPlayerID& Socket, NetBitStreamInterface* BitStream);
    bool        HandlePedTaskPacket(const NetServerPlayerID& Socket, NetBitStreamInterface* BitStream);
    CSimPlayer* Get(const NetServerPlayerID& PlayerSocket);
    void        Broadcast(const CSimPacket& Packet, const std::multimap<ushort, CSimPlayer*>& sendList);

protected:
    // Shared variables
    bool                                     m_bIsLocked;
    CCriticalSection                         m_CS;
    std::set<CSimPlayer*>                    m_AllSimPlayerMap;
    std::map<NetServerPlayerID, CSimPlayer*> m_SocketSimMap;
};
