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
    CSimKeysyncPacket(ElementID PlayerID, bool bPlayerHasOccupiedVehicle, ushort usVehicleGotModel, uchar ucPlayerGotWeaponType, float fPlayerGotWeaponRange,
                      bool bVehicleHasHydraulics, bool bVehicleIsPlaneOrHeli, CControllerState& sharedControllerState);

    ePacketID     GetPacketID() const { return PACKET_ID_PLAYER_KEYSYNC; };
    unsigned long GetFlags() const { return PACKET_MEDIUM_PRIORITY | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    // Set in constructor
    const ElementID   m_PlayerID;
    const bool        m_bPlayerHasOccupiedVehicle = false;
    const ushort      m_usVehicleGotModel = 0;
    const uchar       m_ucPlayerGotWeaponType = 0;
    const float       m_fPlayerGotWeaponRange = 0.0f;
    const bool        m_bVehicleHasHydraulics = false;
    const bool        m_bVehicleIsPlaneOrHeli = false;
    CControllerState& m_sharedControllerState;

    // Set in Read ()
    struct
    {
        float fPlayerRotation = 0.0f;
        float fCameraRotation = 0.0f;

        SKeysyncFlags flags = {};

        bool   bWeaponCorrect = false;
        uchar  ucWeaponSlot = 0;            // Only valid if bWeaponCorrect
        ushort usAmmoInClip = 0;            // Only valid if bWeaponCorrect

        float   fAimDirection = 0.0f;      // Only valid if bWeaponCorrect
        CVector vecSniperSource;
        CVector vecTargetting;

        uchar ucDriveByDirection = 0;

        SVehicleTurretSync turretSync;
    } m_Cache;
};
