/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

struct SSimVehicleDamageInfo
{
    SFixedArray<unsigned char, MAX_DOORS>  m_ucDoorStates = {};
    SFixedArray<unsigned char, MAX_WHEELS> m_ucWheelStates = {};
    SFixedArray<unsigned char, MAX_PANELS> m_ucPanelStates = {};
    SFixedArray<unsigned char, MAX_LIGHTS> m_ucLightStates = {};
};

//
// Copy of enough data from CPlayer to enable autonomous relaying of pure sync packets
//
class CSimPlayer
{
public:
    CSimPlayer() { DEBUG_CREATE_COUNT("CSimPlayer"); }
    ~CSimPlayer() { DEBUG_DESTROY_COUNT("CSimPlayer"); }

    bool                                      IsJoined() { return m_bIsJoined; };
    const std::multimap<ushort, CSimPlayer*>& GetPuresyncSendList();
    unsigned short                            GetBitStreamVersion() { return m_usBitStreamVersion; };
    NetServerPlayerID&                        GetSocket() { return m_PlayerSocket; };

    // General synced vars
    bool                               m_bIsJoined = false;
    unsigned short                     m_usBitStreamVersion = 0;
    NetServerPlayerID                  m_PlayerSocket ;
    std::vector<CSimPlayer*>           m_PuresyncSendListFlat;
    std::multimap<ushort, CSimPlayer*> m_PuresyncSendListGrouped;            // Send list grouped by bitstream version
    bool                               m_bSendListChanged = false;
    bool                               m_bHasOccupiedVehicle = false;
    CControllerState                   m_sharedControllerState;            // Updated by CSim*Packet code

    // Used in CSimPlayerPuresyncPacket and CSimVehiclePuresyncPacket
    ElementID m_PlayerID;
    ushort    m_usLatency = 0;
    uchar     m_ucSyncTimeContext = 0;
    uchar     m_ucWeaponType = 0;

    // Used in CSimVehiclePuresyncPacket
    ushort                m_usVehicleModel = 0;
    uchar                 m_ucOccupiedVehicleSeat = 0;
    float                 m_fWeaponRange = 0.0f;
    uint                  m_uiVehicleDamageInfoSendPhase = 0;
    SSimVehicleDamageInfo m_VehicleDamageInfo;

    // Used in CSimKeysyncPacket
    bool m_bVehicleHasHydraulics = false;
    bool m_bVehicleIsPlaneOrHeli = false;

    // Flag
    bool m_bDoneFirstUpdate = false;

    // Interlink to CPlayer object
    CPlayer* m_pRealPlayer = nullptr;
};
