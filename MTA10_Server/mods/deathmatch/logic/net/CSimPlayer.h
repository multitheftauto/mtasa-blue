/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

//
// Copy of enough data from CPlayer to enable autonomous relaying of pure sync packets
//
class CSimPlayer
{
public:
    ZERO_ON_NEW
                                            CSimPlayer                  ( void )                        { DEBUG_CREATE_COUNT( "CSimPlayer" ); }
                                            ~CSimPlayer                 ( void )                        { DEBUG_DESTROY_COUNT( "CSimPlayer" ); }

    bool                                    IsJoined                    ( void )                        { return m_iStatus == STATUS_JOINED; };
    const std::vector < CSimPlayer* >&      GetPuresyncSendList         ( void )                        { return m_PuresyncSendList; }
    unsigned short                          GetBitStreamVersion         ( void )                        { return m_usBitStreamVersion; };
    NetServerPlayerID&                      GetSocket                   ( void )                        { return m_PlayerSocket; };

    // General synced vars
    int                         m_iStatus;
    unsigned short              m_usBitStreamVersion;
    NetServerPlayerID           m_PlayerSocket;
    std::vector < CSimPlayer* > m_PuresyncSendList;
    bool                        m_bHasOccupiedVehicle;
    CControllerState            m_sharedControllerState;        // Updated by CSim*Packet code

    // Used in CSimPlayerPuresyncPacket and CSimVehiclePuresyncPacket
    ElementID                   m_PlayerID;
    ushort                      m_usLatency;
    uchar                       m_ucSyncTimeContext;
    uchar                       m_ucWeaponType;

    // Used in CSimVehiclePuresyncPacket
    ushort                      m_usVehicleModel;
    uchar                       m_ucOccupiedVehicleSeat;
    float                       m_fWeaponRange;

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
