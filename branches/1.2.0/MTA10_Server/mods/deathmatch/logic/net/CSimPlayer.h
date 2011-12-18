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
    bool                                    HasOccupiedVehicle          ( void )                        { return m_bHasOccupiedVehicle; };
    const std::vector < CSimPlayer* >&      GetSendList                 ( void )                        { return m_SendList; }
    unsigned short                          GetBitStreamVersion         ( void )                        { return m_usBitStreamVersion; };
    NetServerPlayerID&                      GetSocket                   ( void )                        { return m_PlayerSocket; };

    // General synced vars
    int                         m_iStatus;
    unsigned short              m_usBitStreamVersion;
    NetServerPlayerID           m_PlayerSocket;
    std::vector < CSimPlayer* > m_SendList;
    bool                        m_bHasOccupiedVehicle;

    // Used in CSimPlayerPuresyncPacket and CSimVehiclePuresyncPacket
    ElementID                   m_PlayerID;
    ushort                      m_usLatency;
    uchar                       m_ucSyncTimeContext;
    uchar                       m_ucWeaponType;

    // Used in CSimVehiclePuresyncPacket
    ushort                      m_usVehicleModel;
    uchar                       m_ucOccupiedVehicleSeat;
    float                       m_fWeaponRange;

    // Flag
    bool                        m_bDoneFirstUpdate;

    // Interlink to CPlayer object
    CPlayer*                    m_pRealPlayer;
};
