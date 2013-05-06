/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

//
// For bouncing a ped task packet
//
class CSimPedTaskPacket : public CSimPacket
{
public:
    ZERO_ON_NEW

                            CSimPedTaskPacket               ( ElementID PlayerID );

    ePacketID               GetPacketID                     ( void ) const                  { return PACKET_ID_PED_TASK; };
    unsigned long           GetFlags                        ( void ) const                  { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE; };

    bool                    Read                            ( NetBitStreamInterface& BitStream );
    bool                    Write                           ( NetBitStreamInterface& BitStream ) const;


    // Set in constructor 
    const ElementID m_PlayerID;

    // Set in Read ()
    struct
    {
        uint    uiNumBitsInPacketBody;
        char    DataBuffer[ 56 ];
    } m_Cache;

};
