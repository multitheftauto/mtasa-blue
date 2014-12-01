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
    SFixedArray < unsigned char, MAX_DOORS > m_ucDoorStates;
    SFixedArray < unsigned char, MAX_WHEELS > m_ucWheelStates;
    SFixedArray < unsigned char, MAX_PANELS > m_ucPanelStates;
    SFixedArray < unsigned char, MAX_LIGHTS > m_ucLightStates;
};

//
// Copy of enough data from CPlayer to enable autonomous relaying of pure sync packets
//
class CSimPlayer
{
public:
    ZERO_ON_NEW
                                                    CSimPlayer                  ( void )                        { DEBUG_CREATE_COUNT( "CSimPlayer" ); }
                                                    ~CSimPlayer                 ( void )                        { DEBUG_DESTROY_COUNT( "CSimPlayer" ); }

    bool                                            IsJoined                    ( void )                        { return m_iStatus == STATUS_JOINED; };
    const std::multimap < ushort, CSimPlayer* >&    GetPuresyncSendList         ( void );
    unsigned short                                  GetBitStreamVersion         ( void )                        { return m_usBitStreamVersion; };
    NetServerPlayerID&                              GetSocket                   ( void )                        { return m_PlayerSocket; };

    // General synced vars
    int                                     m_iStatus;
    unsigned short                          m_usBitStreamVersion;
    NetServerPlayerID                       m_PlayerSocket;
    std::vector < CSimPlayer* >             m_PuresyncSendListFlat;
    std::multimap < ushort, CSimPlayer* >   m_PuresyncSendListGrouped;      // Send list grouped by bitstream version
    bool                                    m_bSendListChanged;
    bool                                    m_bHasOccupiedVehicle;
    CControllerState                        m_sharedControllerState;        // Updated by CSim*Packet code

    // Used in CSimPlayerPuresyncPacket and CSimVehiclePuresyncPacket
    ElementID                   m_PlayerID;
    ushort                      m_usLatency;
    uchar                       m_ucSyncTimeContext;
    uchar                       m_ucWeaponType;

    // Used in CSimVehiclePuresyncPacket
    ushort                      m_usVehicleModel;
    uchar                       m_ucOccupiedVehicleSeat;
    float                       m_fWeaponRange;
    uint                        m_uiVehicleDamageInfoSendPhase;
    SSimVehicleDamageInfo       m_VehicleDamageInfo;

    // Used in CSimKeysyncPacket
    bool                        m_bVehicleHasHydraulics;
    bool                        m_bVehicleIsPlaneOrHeli;
    float                       m_fCameraRotation;      // Not needed when all clients are >=0x2C bitstream version
    float                       m_fPlayerRotation;      //                          ''

    // Flag
    bool                        m_bDoneFirstUpdate;

    // Interlink to CPlayer object
    CPlayer*                    m_pRealPlayer;
};
