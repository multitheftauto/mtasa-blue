/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

//
// For bouncing a bullet sync packet
//
class CSimBulletsyncPacket : public CSimPacket
{
public:
    ZERO_ON_NEW

                            CSimBulletsyncPacket            ( ElementID PlayerID );


    ePacketID               GetPacketID                     ( void ) const                  { return PACKET_ID_PLAYER_BULLETSYNC; };
    unsigned long           GetFlags                        ( void ) const                  { return PACKET_MEDIUM_PRIORITY | PACKET_RELIABLE; };

    bool                    Read                            ( NetBitStreamInterface& BitStream );
    bool                    Write                           ( NetBitStreamInterface& BitStream ) const;


    // Set in constructor 
    const ElementID m_PlayerID;

    // Set in Read ()
    struct
    {
        eWeaponType weaponType;
        CVector     vecStart;
        CVector     vecEnd;
        uchar       ucOrderCounter;
    } m_Cache;

};
