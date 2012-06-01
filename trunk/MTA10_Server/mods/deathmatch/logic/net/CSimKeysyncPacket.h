/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

//
// For bouncing a key sync packet
//
class CSimKeysyncPacket : public CSimPacket
{
public:
    ZERO_ON_NEW

                            CSimKeysyncPacket               ( ElementID PlayerID,
                                                              bool bPlayerHasOccupiedVehicle,
                                                              ushort usVehicleGotModel,
                                                              uchar ucPlayerGotWeaponType,
                                                              float fPlayerGotWeaponRange,
                                                              bool bVehicleHasHydraulics,
                                                              bool bVehicleIsPlaneOrHeli,
                                                              CControllerState& sharedControllerState,
                                                              float fPlayerGotCameraRotation,
                                                              float fPlayerGotPlayerRotation );


    ePacketID               GetPacketID                     ( void ) const                  { return PACKET_ID_PLAYER_KEYSYNC; };
    unsigned long           GetFlags                        ( void ) const                  { return PACKET_MEDIUM_PRIORITY | PACKET_SEQUENCED; };

    bool                    Read                            ( NetBitStreamInterface& BitStream );
    bool                    Write                           ( NetBitStreamInterface& BitStream ) const;


    // Set in constructor 
    const ElementID m_PlayerID;
    const bool      m_bPlayerHasOccupiedVehicle;
    const ushort    m_usVehicleGotModel;
    const uchar     m_ucPlayerGotWeaponType;
    const float     m_fPlayerGotWeaponRange;
    const bool      m_bVehicleHasHydraulics;
    const bool      m_bVehicleIsPlaneOrHeli;
    CControllerState& m_sharedControllerState;
    const float     m_fPlayerGotCameraRotation;
    const float     m_fPlayerGotPlayerRotation;

    // Set in Read ()
    struct
    {
        float           fPlayerRotation;
        float           fCameraRotation;

        SKeysyncFlags   flags;

        bool            bWeaponCorrect;
        uchar           ucWeaponSlot;       // Only valid if bWeaponCorrect
        ushort          usAmmoInClip;      // Only valid if bWeaponCorrect

        float           fAimDirection;      // Only valid if bWeaponCorrect
        CVector         vecSniperSource;
        CVector         vecTargetting;

        uchar           ucDriveByDirection;

        SVehicleTurretSync turretSync;

        bool            bUseBulletSync;
        bool            bBulletSyncFireButtonDown;

    } m_Cache;

};


