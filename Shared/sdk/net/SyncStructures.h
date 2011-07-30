/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/sdk/net/SyncStructures.h
*  PURPOSE:     Structures used for syncing stuff through the network.
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#include <CVector.h>
#include <net/bitstream.h>

// Used to make sure that any position values we receive are at least half sane
#define SYNC_POSITION_LIMIT 100000.0f
// Note: Using SFloatSync < 14, 10 > also limits the range from -8191 to 8192

#pragma pack(push)
#pragma pack(1)


//////////////////////////////////////////
//                                      //
//              Data types              //
//                                      //
//////////////////////////////////////////
template < unsigned int integerBits, unsigned int fractionalBits >
struct SFloatSync : public ISyncStructure
{
    bool Read ( NetBitStreamInterface& bitStream )
    {
        SFixedPointNumber num;
        if ( bitStream.ReadBits ( (char *)&num, integerBits + fractionalBits ) )
        {
            data.fValue = (float)( (double)num.iValue / ( 1 << fractionalBits ) );
            return true;
        }
        return false;
    }

    void Write ( NetBitStreamInterface& bitStream ) const
    {
        double limitsMax = ( 1 << ( integerBits - 1 ) ) - 1;
        double limitsMin = 0 - ( 1 << ( integerBits - 1 ) );
        double scale     = 1 << fractionalBits;

        double dValue = data.fValue;
#ifdef WIN32
#ifdef MTA_DEBUG
        assert ( !_isnan ( dValue ) );
#endif
#endif
        dValue = Clamp < double > ( limitsMin, dValue, limitsMax );

        SFixedPointNumber num;
        num.iValue = static_cast < int > ( dValue * scale );
        bitStream.WriteBits ( (const char* )&num, integerBits + fractionalBits );
    }

    struct
    {
        float fValue;
    } data;

private:
    struct SFixedPointNumber
    {
        int iValue : integerBits + fractionalBits;
    };
};

// Useful when we don't need all bits of integer type
template < typename type, unsigned int bits >
struct SIntegerSync : public ISyncStructure
{
    bool Read ( NetBitStreamInterface& bitStream )
    {
        return bitStream.ReadBits ( reinterpret_cast < char* > ( &data ), bits );
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.WriteBits ( reinterpret_cast < const char* > ( &data ), bits );
    }

    SIntegerSync ( void )
    {
        assert ( bits <= sizeof ( type ) * 8 );
    }
    SIntegerSync ( type value )
    {
        SIntegerSync ();
        data.value = value;
    }

    operator type ( void ) const
    {
        return data.value;
    }

    struct
    {
        type value : bits;
    } data;
};

//////////////////////////////////////////
//                                      //
//              FloatAsBits             //
//                                      //
// Map a float range into bits          //
//                                      //
//////////////////////////////////////////
template < unsigned int bits >
struct SFloatAsBitsSync : public ISyncStructure
{
    SFloatAsBitsSync ( float fMin, float fMax, bool bPreserveGreaterThanMin )
        : m_fMin ( fMin )
        , m_fMax ( fMax )
        , m_bPreserveGreaterThanMin ( bPreserveGreaterThanMin )
    {
    }

    bool Read ( NetBitStreamInterface& bitStream )
    {
        unsigned long ulValue = 0;
        if ( bitStream.ReadBits ( &ulValue, bits ) )
        {
            // Convert bits to position in range
            float fAlpha = ulValue / (float)ulValueMax;
            // Find value in range
            data.fValue = Lerp ( m_fMin, fAlpha, m_fMax );
            return true;
        }
        return false;
    }

    void Write ( NetBitStreamInterface& bitStream ) const
    {
        // Find position in range
        float fAlpha = UnlerpClamped ( m_fMin, data.fValue, m_fMax );
        // Convert to bits
        unsigned long ulValue = Round ( ulValueMax * fAlpha );

        // If required, ensure ( fValue > m_fMin ) is preserved.
        if ( m_bPreserveGreaterThanMin )
            if ( ulValue == 0 && fAlpha > 0.0f )
                ulValue = 1;

        bitStream.WriteBits ( &ulValue, bits );
    }

    struct
    {
        float fValue;
    } data;
private:
    const float m_fMin;
    const float m_fMax;
    const bool  m_bPreserveGreaterThanMin;
    const static unsigned long ulValueMax = ( 1 << bits ) - 1;
};


// Declare specific health and armor sync structures
struct SPlayerHealthSync : public SFloatAsBitsSync < 8 >
{
    SPlayerHealthSync () : SFloatAsBitsSync<8> ( 0.f, 200.0f, true ) {}
};

struct SPlayerArmorSync : public SFloatAsBitsSync < 8 >
{
    SPlayerArmorSync () : SFloatAsBitsSync<8> ( 0.f, 100.0f, true ) {}
};

struct SVehicleHealthSync : public SFloatAsBitsSync < 12 >
{
    SVehicleHealthSync () : SFloatAsBitsSync<12> ( 0.f, 2000.0f, true ) {}
};

struct SLowPrecisionVehicleHealthSync : public SFloatAsBitsSync < 8 >
{
    SLowPrecisionVehicleHealthSync () : SFloatAsBitsSync<8> ( 0.0f, 2000.0f, true ) {}
};

struct SObjectHealthSync : public SFloatAsBitsSync < 11 >
{
    SObjectHealthSync () : SFloatAsBitsSync<11> ( 0.f, 1000.0f, true ) {}
};


//////////////////////////////////////////
//                                      //
//               Position               //
//                                      //
//////////////////////////////////////////
struct SPositionSync : public ISyncStructure
{
    SPositionSync ( bool bUseFloats = false ) : m_bUseFloats ( bUseFloats ) {}

    bool Read ( NetBitStreamInterface& bitStream )
    {
        if ( m_bUseFloats )
        {
            return bitStream.Read ( data.vecPosition.fX ) && data.vecPosition.fX > -SYNC_POSITION_LIMIT && data.vecPosition.fX < SYNC_POSITION_LIMIT &&
                   bitStream.Read ( data.vecPosition.fY ) && data.vecPosition.fY > -SYNC_POSITION_LIMIT && data.vecPosition.fY < SYNC_POSITION_LIMIT &&
                   bitStream.Read ( data.vecPosition.fZ ) && data.vecPosition.fZ > -SYNC_POSITION_LIMIT && data.vecPosition.fZ < SYNC_POSITION_LIMIT;
        }
        else
        {
            SFloatSync < 14, 10 > x, y;

            if ( bitStream.Read ( &x ) && x.data.fValue > -SYNC_POSITION_LIMIT && x.data.fValue < SYNC_POSITION_LIMIT &&
                 bitStream.Read ( &y ) && y.data.fValue > -SYNC_POSITION_LIMIT && y.data.fValue < SYNC_POSITION_LIMIT &&
                 bitStream.Read ( data.vecPosition.fZ ) && data.vecPosition.fZ > -SYNC_POSITION_LIMIT && data.vecPosition.fZ < SYNC_POSITION_LIMIT )
            {
                data.vecPosition.fX = x.data.fValue;
                data.vecPosition.fY = y.data.fValue;
                return true;
            }
        }

        return false;
    }

    void Write ( NetBitStreamInterface& bitStream ) const
    {
        if ( m_bUseFloats )
        {
            bitStream.Write ( Clamp ( -SYNC_POSITION_LIMIT + 1, data.vecPosition.fX, SYNC_POSITION_LIMIT - 1 ) );
            bitStream.Write ( Clamp ( -SYNC_POSITION_LIMIT + 1, data.vecPosition.fY, SYNC_POSITION_LIMIT - 1 ) );
            bitStream.Write ( Clamp ( -SYNC_POSITION_LIMIT + 1, data.vecPosition.fZ, SYNC_POSITION_LIMIT - 1 ) );
        }
        else
        {
            SFloatSync < 14, 10 > x, y;
            x.data.fValue = data.vecPosition.fX;
            y.data.fValue = data.vecPosition.fY;

            bitStream.Write ( &x );
            bitStream.Write ( &y );
            bitStream.Write ( Clamp ( -SYNC_POSITION_LIMIT + 1, data.vecPosition.fZ, SYNC_POSITION_LIMIT - 1 ) );
        }
    }

    struct
    {
        CVector vecPosition;
    } data;

private:
    bool m_bUseFloats;
};

struct SPosition2DSync : public ISyncStructure
{
    SPosition2DSync ( bool bUseFloats = false ) : m_bUseFloats ( bUseFloats ) {}

    bool Read ( NetBitStreamInterface& bitStream )
    {
        if ( m_bUseFloats )
        {
            return bitStream.Read ( data.vecPosition.fX ) && data.vecPosition.fX > -SYNC_POSITION_LIMIT && data.vecPosition.fX < SYNC_POSITION_LIMIT &&
                   bitStream.Read ( data.vecPosition.fY ) && data.vecPosition.fY > -SYNC_POSITION_LIMIT && data.vecPosition.fY < SYNC_POSITION_LIMIT;
        }
        else
        {
            SFloatSync < 14, 10 > x, y;

            if ( bitStream.Read ( &x ) && x.data.fValue > -SYNC_POSITION_LIMIT && x.data.fValue < SYNC_POSITION_LIMIT &&
                 bitStream.Read ( &y ) && y.data.fValue > -SYNC_POSITION_LIMIT && y.data.fValue < SYNC_POSITION_LIMIT )
            {
                data.vecPosition.fX = x.data.fValue;
                data.vecPosition.fY = y.data.fValue;
                return true;
            }
        }

        return false;
    }

    void Write ( NetBitStreamInterface& bitStream ) const
    {
        if ( m_bUseFloats )
        {
            bitStream.Write ( Clamp ( -SYNC_POSITION_LIMIT + 1, data.vecPosition.fX, SYNC_POSITION_LIMIT - 1 ) );
            bitStream.Write ( Clamp ( -SYNC_POSITION_LIMIT + 1, data.vecPosition.fY, SYNC_POSITION_LIMIT - 1 ) );
        }
        else
        {
            SFloatSync < 14, 10 > x, y;
            x.data.fValue = data.vecPosition.fX;
            y.data.fValue = data.vecPosition.fY;

            bitStream.Write ( &x );
            bitStream.Write ( &y );
        }
    }

    struct
    {
        CVector2D vecPosition;
    } data;

private:
    bool m_bUseFloats;
};

// Low precision positions:
// - Write X and Y components bound to [-8192, 8192], with a max error of 0.25 units.
// - Write Z bound to [-110, 1938], with a max error of 1 unit.
struct SLowPrecisionPositionSync : public ISyncStructure
{
    bool Read ( NetBitStreamInterface& bitStream )
    {
        unsigned short usX;
        unsigned short usY;
        unsigned short usZ;

        if ( !bitStream.Read ( usX ) || !bitStream.Read ( usY ) || !bitStream.ReadBits ( reinterpret_cast<char *>(&usZ), 11 ) )
            return false;
        data.vecPosition.fX = 16384.0f * (usX / 65535.0f) - 8192.0f;
        data.vecPosition.fY = 16384.0f * (usY / 65535.0f) - 8192.0f;
        data.vecPosition.fZ = static_cast < float > ( usZ ) - 110.0f;
        return true;
    }

    void Write ( NetBitStreamInterface& bitStream ) const
    {
        float fX = SharedUtil::Clamp ( -8192.0f, data.vecPosition.fX, 8192.0f );
        float fY = SharedUtil::Clamp ( -8192.0f, data.vecPosition.fY, 8192.0f );
        float fZ = SharedUtil::Clamp ( -110.0f, data.vecPosition.fZ, 2048.0f - 110.0f );

        unsigned short usX = static_cast < unsigned short > ( ((fX + 8192.0f) / 16384.0f) * 65535.0f );
        unsigned short usY = static_cast < unsigned short > ( ((fY + 8192.0f) / 16384.0f) * 65535.0f );
        unsigned short usZ = static_cast < unsigned short > ( fZ + 110.0f );

        bitStream.Write ( usX );
        bitStream.Write ( usY );
        bitStream.WriteBits ( reinterpret_cast<const char*>(&usZ), 11 );
    }

    struct
    {
        CVector vecPosition;
    } data;
};

//////////////////////////////////////////
//                                      //
//        Rotation Degrees and radians  //
//                                      //
// 2 bytes for each component           //
//                                      //
//////////////////////////////////////////
struct SRotationDegreesSync : public ISyncStructure
{
    SRotationDegreesSync ( bool bUseFloats = false ) : m_bUseFloats ( bUseFloats ) {}

    bool Read ( NetBitStreamInterface& bitStream )
    {
        if ( m_bUseFloats )
        {
            return bitStream.Read ( data.vecRotation.fX ) &&
                   bitStream.Read ( data.vecRotation.fY ) &&
                   bitStream.Read ( data.vecRotation.fZ );
        }
        else
        {
            unsigned short usRx;
            unsigned short usRy;
            unsigned short usRz;

            if ( bitStream.Read ( usRx ) && bitStream.Read ( usRy ) && bitStream.Read ( usRz ) )
            {
                data.vecRotation.fX = usRx * ( 360.f / 65536.f );
                data.vecRotation.fY = usRy * ( 360.f / 65536.f );
                data.vecRotation.fZ = usRz * ( 360.f / 65536.f );
                return true;
            }
        }

        return false;
    }

    void Write ( NetBitStreamInterface& bitStream ) const
    {
        if ( m_bUseFloats )
        {
            bitStream.Write ( data.vecRotation.fX );
            bitStream.Write ( data.vecRotation.fY );
            bitStream.Write ( data.vecRotation.fZ );
        }
        else
        {
            unsigned short usRx = static_cast < unsigned short > ( data.vecRotation.fX * ( 65536 / 360.f ) );
            unsigned short usRy = static_cast < unsigned short > ( data.vecRotation.fY * ( 65536 / 360.f ) );
            unsigned short usRz = static_cast < unsigned short > ( data.vecRotation.fZ * ( 65536 / 360.f ) );
            bitStream.Write ( usRx );
            bitStream.Write ( usRy );
            bitStream.Write ( usRz );
        }
    }

    struct
    {
        CVector vecRotation;
    } data;

private:
    bool m_bUseFloats;
};


struct SRotationRadiansSync : public ISyncStructure
{
    SRotationRadiansSync ( bool bUseFloats = false ) : m_bUseFloats ( bUseFloats ) {}

    bool Read ( NetBitStreamInterface& bitStream )
    {
        if ( m_bUseFloats )
        {
            return bitStream.Read ( data.vecRotation.fX ) &&
                   bitStream.Read ( data.vecRotation.fY ) &&
                   bitStream.Read ( data.vecRotation.fZ );
        }
        else
        {
            unsigned short usRx;
            unsigned short usRy;
            unsigned short usRz;

            if ( bitStream.Read ( usRx ) && bitStream.Read ( usRy ) && bitStream.Read ( usRz ) )
            {
                data.vecRotation.fX = usRx * ( 6.283185307f / 65536.f );
                data.vecRotation.fY = usRy * ( 6.283185307f / 65536.f );
                data.vecRotation.fZ = usRz * ( 6.283185307f / 65536.f );
                return true;
            }
        }

        return false;
    }

    void Write ( NetBitStreamInterface& bitStream ) const
    {
        if ( m_bUseFloats )
        {
            bitStream.Write ( data.vecRotation.fX );
            bitStream.Write ( data.vecRotation.fY );
            bitStream.Write ( data.vecRotation.fZ );
        }
        else
        {
            unsigned short usRx = static_cast < unsigned short > ( data.vecRotation.fX * ( 65536 / 6.283185307f ) );
            unsigned short usRy = static_cast < unsigned short > ( data.vecRotation.fY * ( 65536 / 6.283185307f ) );
            unsigned short usRz = static_cast < unsigned short > ( data.vecRotation.fZ * ( 65536 / 6.283185307f ) );
            bitStream.Write ( usRx );
            bitStream.Write ( usRy );
            bitStream.Write ( usRz );
        }
    }

    struct
    {
        CVector vecRotation;
    } data;

private:
    bool m_bUseFloats;
};



//////////////////////////////////////////
//                                      //
//               Velocity               //
//                                      //
//////////////////////////////////////////
struct SVelocitySync : public ISyncStructure
{
    bool Read ( NetBitStreamInterface& bitStream )
    {
        if ( !bitStream.ReadBit () )
        {
            data.vecVelocity.fX = data.vecVelocity.fY = data.vecVelocity.fZ = 0.0f;
            return true;
        }
        else
        {
            float fModule;
            if ( bitStream.Read ( fModule ) )
            {
                if ( bitStream.ReadNormVector ( data.vecVelocity.fX, data.vecVelocity.fY, data.vecVelocity.fZ ) )
                {
                    data.vecVelocity = data.vecVelocity * fModule;
                    return true;
                }
            }
        }
        return false;
    }

    void Write ( NetBitStreamInterface& bitStream ) const
    {
        CVector vecVelocity = data.vecVelocity;

        float fModulus = vecVelocity.Normalize ();
        if ( fModulus == 0.0f )
            bitStream.WriteBit ( false );
        else
        {
            bitStream.WriteBit ( true );
            bitStream.Write ( fModulus );
            bitStream.WriteNormVector ( vecVelocity.fX, vecVelocity.fY, vecVelocity.fZ );
        }
    }

    struct
    {
        CVector vecVelocity;
    } data;
};



//////////////////////////////////////////
//                                      //
//           Player pure-sync           //
//                                      //
//////////////////////////////////////////
struct SPlayerPuresyncFlags : public ISyncStructure
{
    enum { BITCOUNT = 12 };

    bool Read ( NetBitStreamInterface& bitStream )
    {
        return bitStream.ReadBits ( (char *)&data, BITCOUNT );
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.WriteBits ( (const char* )&data, BITCOUNT );
    }

    struct
    {
        bool bIsInWater : 1;
        bool bIsOnGround : 1;
        bool bHasJetPack : 1;
        bool bIsDucked : 1;
        bool bWearsGoogles : 1;
        bool bHasContact : 1;
        bool bIsChoking : 1;
        bool bAkimboTargetUp : 1;
        bool bIsOnFire : 1;
        bool bHasAWeapon : 1;
        bool bSyncingVelocity : 1;
        bool bStealthAiming : 1;
    } data;
};


struct SPedRotationSync : public ISyncStructure
{
    bool Read ( NetBitStreamInterface& bitStream )
    {
        const static float fPI = 3.14159265f;
        SFloatAsBitsSync < 16 > rotation ( -fPI, fPI, false );
        if ( bitStream.Read ( &rotation ) )
        {
            data.fRotation = rotation.data.fValue;
            return true;
        }
        return false;
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        const static float fPI = 3.14159265f;
        float fRotation = data.fRotation;
        if ( fRotation < -fPI )
            fRotation += fPI * 2.0f;
        else if ( fRotation > fPI )
            fRotation -= fPI * 2.0f;

        SFloatAsBitsSync < 16 > rotation ( -fPI, fPI, false );
        rotation.data.fValue = fRotation;
        bitStream.Write ( &rotation );
    }

    struct
    {
        float fRotation;
    } data;
};



//////////////////////////////////////////
//                                      //
//           Vehicle pure-sync          //
//                                      //
//////////////////////////////////////////
struct SVehiclePuresyncFlags : public ISyncStructure
{
    enum { BITCOUNT = 11 };

    bool Read ( NetBitStreamInterface& bitStream )
    {
        return bitStream.ReadBits ( reinterpret_cast < char* > ( &data ), BITCOUNT );
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.WriteBits ( reinterpret_cast < const char* > ( &data ), BITCOUNT );
    }

    struct
    {
        bool bIsWearingGoggles : 1;
        bool bIsDoingGangDriveby : 1;
        bool bIsSirenOrAlarmActive : 1;
        bool bIsSmokeTrailEnabled : 1;
        bool bIsLandingGearDown : 1;
        bool bIsOnGround : 1;
        bool bIsInWater : 1;
        bool bIsDerailed : 1;
        bool bIsAircraft : 1;
        bool bHasAWeapon : 1;
        bool bIsHeliSearchLightVisible : 1;
    } data;
};


struct SDrivebyDirectionSync : public ISyncStructure
{
    enum { BITCOUNT = 2 };

    bool Read ( NetBitStreamInterface& bitStream )
    {
        return bitStream.ReadBits ( reinterpret_cast < char* > ( &data ), BITCOUNT );
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.WriteBits ( reinterpret_cast < const char* > ( &data ), BITCOUNT );
    }

    struct
    {
        unsigned char ucDirection : 2;
    } data;
};


//////////////////////////////////////////
//                                      //
//       Unoccupied vehicle sync        //
//                                      //
//////////////////////////////////////////
struct SUnoccupiedVehicleSync : public ISyncStructure
{
    SUnoccupiedVehicleSync () { *((char *)&data) = 0; }

    bool Read ( NetBitStreamInterface& bitStream )
    {
        if ( bitStream.Read ( data.vehicleID ) &&
             bitStream.Read ( data.ucTimeContext ) &&
             bitStream.ReadBits ( (char *)&data, 9 ) )
        {
            if ( data.bSyncPosition )
            {
                SPositionSync pos;
                bitStream.Read ( &pos );
                data.vecPosition = pos.data.vecPosition;
            }

            if ( data.bSyncRotation )
            {
                SRotationDegreesSync rot;
                bitStream.Read ( &rot );
                data.vecRotation = rot.data.vecRotation;
            }

            if ( data.bSyncVelocity )
            {
                SVelocitySync velocity;
                bitStream.Read ( &velocity );
                data.vecVelocity = velocity.data.vecVelocity;
            }

            if ( data.bSyncTurnVelocity )
            {
                SVelocitySync turnVelocity;
                bitStream.Read ( &turnVelocity );
                data.vecTurnVelocity = turnVelocity.data.vecVelocity;
            }

            if ( data.bSyncHealth )
            {
                SVehicleHealthSync health;
                bitStream.Read ( &health );
                data.fHealth = health.data.fValue;
            }

            if ( data.bSyncTrailer )
            {
                bitStream.Read ( data.trailer );
            }

            return true;
        }
        return false;
    }

    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.Write ( data.vehicleID );
        bitStream.Write ( data.ucTimeContext );
        bitStream.WriteBits ( (const char* )&data, 9 );

        if ( data.bSyncPosition )
        {
            SPositionSync pos;
            pos.data.vecPosition = data.vecPosition;
            bitStream.Write ( &pos );
        }

        if ( data.bSyncRotation )
        {
            SRotationDegreesSync rot;
            rot.data.vecRotation = data.vecRotation;
            bitStream.Write ( &rot );
        }

        if ( data.bSyncVelocity )
        {
            SVelocitySync velocity;
            velocity.data.vecVelocity = data.vecVelocity;
            bitStream.Write ( &velocity );
        }

        if ( data.bSyncTurnVelocity )
        {
            SVelocitySync turnVelocity;
            turnVelocity.data.vecVelocity = data.vecTurnVelocity;
            bitStream.Write ( &turnVelocity );
        }

        if ( data.bSyncHealth )
        {
            SVehicleHealthSync health;
            health.data.fValue = data.fHealth;
            bitStream.Write ( &health );
        }

        if ( data.bSyncTrailer )
        {
            bitStream.Write ( data.trailer );
        }
    }

    struct
    {
        bool bSyncPosition : 1;
        bool bSyncRotation : 1;
        bool bSyncVelocity : 1;
        bool bSyncTurnVelocity : 1;
        bool bSyncHealth : 1;
        bool bSyncTrailer : 1;
        bool bEngineOn : 1;
        bool bDerailed : 1;
        bool bIsInWater : 1;
        CVector vecPosition;
        CVector vecRotation;
        CVector vecVelocity;
        CVector vecTurnVelocity;
        float fHealth;
        ElementID trailer;

        ElementID vehicleID;
        unsigned char ucTimeContext;
    } data;
};



//////////////////////////////////////////
//                                      //
//               Keysync                //
//                                      //
//////////////////////////////////////////
struct SKeysyncFlags : public ISyncStructure
{
    enum { BITCOUNT = 4 };

    bool Read ( NetBitStreamInterface& bitStream )
    {
        return bitStream.ReadBits ( (char *)&data, BITCOUNT );
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.WriteBits ( (const char* )&data, BITCOUNT );
    }

    struct
    {
        bool bIsDucked : 1;
        bool bIsChoking : 1;
        bool bAkimboTargetUp : 1;
        bool bSyncingVehicle : 1;
    } data;
};

struct SFullKeysyncSync : public ISyncStructure
{
    // Stick values vary from -128 to 128, but char range is from -128 to 127, so we stretch
    // the stick value to the range from -127 to 127 to make it fit in a char (byte) and save
    // one byte of bandwidth per stick.
    bool Read ( NetBitStreamInterface& bitStream )
    {
        bool bState;
        char cLeftStickX;
        char cLeftStickY;

        if ( ( bState = bitStream.ReadBits ( (char *)&data, 8 ) ) )
        {
            if ( ( bState = bitStream.Read ( cLeftStickX ) ) )
            {
                data.sLeftStickX = static_cast < short > ( (float)cLeftStickX * 128.0f/127.0f );
                if ( ( bState = bitStream.Read ( cLeftStickY ) ) )
                    data.sLeftStickY = static_cast < short > ( (float)cLeftStickY * 128.0f/127.0f );
            }
        }

        return bState;
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.WriteBits ( (const char* )&data, 8 );
        char cLeftStickX = static_cast < char > ( (float)data.sLeftStickX * 127.0f/128.0f );
        bitStream.Write ( cLeftStickX );
        char cLeftStickY = static_cast < char > ( (float)data.sLeftStickY * 127.0f/128.0f );
        bitStream.Write ( cLeftStickY );
    }

    struct
    {
        bool bLeftShoulder1 : 1;
        bool bRightShoulder1 : 1;
        bool bButtonSquare : 1;
        bool bButtonCross : 1;
        bool bButtonCircle : 1;
        bool bButtonTriangle : 1;
        bool bShockButtonL : 1;
        bool bPedWalk : 1;
        short sLeftStickX;
        short sLeftStickY;
    } data;
};

struct SSmallKeysyncSync : public ISyncStructure
{
    // Stick values vary from -128 to 128, but char range is from -128 to 127, so we stretch
    // the stick value to the range from -127 to 127 to make it fit in a char (byte) and save
    // one byte of bandwidth per stick.
    bool Read ( NetBitStreamInterface& bitStream )
    {
        return bitStream.ReadBits ( (char *)&data, 8 );
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.WriteBits ( (const char* )&data, 8 );
    }

    struct
    {
        bool bLeftShoulder1 : 1;
        bool bRightShoulder1 : 1;
        bool bButtonSquare : 1;
        bool bButtonCross : 1;
        bool bButtonCircle : 1;
        bool bButtonTriangle : 1;
        bool bShockButtonL : 1;
        bool bPedWalk : 1;
    } data;
};

struct SVehicleTurretSync : public ISyncStructure
{
    bool Read ( NetBitStreamInterface& bitStream )
    {
        short sHorizontal, sVertical;
        if ( bitStream.Read ( sHorizontal ) && bitStream.Read ( sVertical ) )
        {
            data.fTurretX = static_cast < float > ( sHorizontal ) / ( 32767.0f / PI );
            data.fTurretY = static_cast < float > ( sVertical )   / ( 32767.0f / PI );

            return true;
        }
        return false;
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        // Convert to shorts to save 4 bytes (multiply for precision on how many rounds can fit in a ushort)
        short sHorizontal = static_cast < short > ( data.fTurretX * ( 32767.0f / PI ) ),
              sVertical   = static_cast < short > ( data.fTurretY * ( 32767.0f / PI ) );

        bitStream.Write ( sHorizontal );
        bitStream.Write ( sVertical );
    }

    struct
    {
        float fTurretX;
        float fTurretY;
    } data;
};

struct SDoorOpenRatioSync : public ISyncStructure
{
    bool Read ( NetBitStreamInterface& bitStream )
    {
        bool bUncompressed;
        bool bNotZero;

        if ( !bitStream.ReadBit ( bUncompressed ) ) 
            return false;
        if ( bUncompressed == false )
        {
            if ( !bitStream.ReadBit ( bNotZero ) )
                return false;
            if ( bNotZero )
                data.fRatio = 1.0f;
            else
                data.fRatio = 0.0f;
        }
        else
        {
            SFloatAsBitsSync<10> fl ( 0.0f, 1.0f, true );
            if ( !fl.Read ( bitStream ) )
                return false;
            data.fRatio = fl.data.fValue;
        }

        return true;
    }

    void Write ( NetBitStreamInterface& bitStream ) const
    {
        if ( data.fRatio == 0.0f || data.fRatio == 1.0f )
        {
            bitStream.WriteBit ( false );
            bitStream.WriteBit ( data.fRatio == 1.0f );
        }
        else
        {
            bitStream.WriteBit ( true );
            SFloatAsBitsSync<10> fl ( 0.0f, 1.0f, true );
            fl.data.fValue = data.fRatio;
            bitStream.Write ( &fl );
        }
    }

    struct
    {
        float fRatio;
    } data;
};

//////////////////////////////////////////
//                                      //
//             Weapon sync              //
//                                      //
//////////////////////////////////////////
struct SWeaponSlotSync : public ISyncStructure
{
    enum { BITCOUNT = 4 };

    bool Read ( NetBitStreamInterface& bitStream )
    {
        return bitStream.ReadBits ( (char *)&data, BITCOUNT );
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.WriteBits ( (const char* )&data, BITCOUNT );
    }

    struct
    {
        unsigned int uiSlot : 4;
    } data;
};

struct SWeaponTypeSync : public ISyncStructure
{
    enum { BITCOUNT = 6 };

    bool Read ( NetBitStreamInterface& bitStream )
    {
        return bitStream.ReadBits ( (char *)&data, BITCOUNT );
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.WriteBits ( (const char* )&data, BITCOUNT );
    }

    struct
    {
        unsigned char ucWeaponType : 6;
    } data;
};

struct IAmmoInClipSync : public virtual ISyncStructure
{
#ifndef WIN32
    void* operator new ( size_t size, void* ptr ) { return ptr; }
    void operator delete ( void* ) { }
#endif

    virtual unsigned short GetAmmoInClip () const = 0;
};

template < unsigned int bitCount >
struct SAmmoInClipSync : public IAmmoInClipSync
{
    SAmmoInClipSync ( unsigned short usAmmoInClip )
    {
        data.usAmmoInClip = usAmmoInClip;
    }

    bool Read ( NetBitStreamInterface& bitStream )
    {
        return bitStream.ReadBits ( (char *)&data, bitCount );
    }

    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.WriteBits ( (const char* )&data, bitCount );
    }

    unsigned short GetAmmoInClip () const
    {
        return data.usAmmoInClip;
    }

    struct
    {
        unsigned short usAmmoInClip : bitCount;
    } data;
};


// Declare specific weapon ammo in clip sync structures
typedef SAmmoInClipSync < 6 >  SPistolAmmoInClipSync;
typedef SAmmoInClipSync < 5 >  SSilencedAmmoInClipSync;
typedef SAmmoInClipSync < 3 >  SDeagleAmmoInClipSync;
typedef SAmmoInClipSync < 1 >  SShotgunAmmoInClipSync;
typedef SAmmoInClipSync < 3 >  SSawnoffAmmoInClipSync;
typedef SAmmoInClipSync < 3 >  SSpas12AmmoInClipSync;
typedef SAmmoInClipSync < 7 >  SUziAmmoInClipSync;
typedef SAmmoInClipSync < 5 >  SMp5AmmoInClipSync;
typedef SAmmoInClipSync < 7 >  STec9AmmoInClipSync;
typedef SAmmoInClipSync < 5 >  SAk47AmmoInClipSync;
typedef SAmmoInClipSync < 6 >  SM4AmmoInClipSync;
typedef SAmmoInClipSync < 1 >  SRifleAmmoInClipSync;
typedef SAmmoInClipSync < 1 >  SSniperAmmoInClipSync;
typedef SAmmoInClipSync < 1 >  SRLauncherAmmoInClipSync;
typedef SAmmoInClipSync < 1 >  SRPGAmmoInClipSync;
typedef SAmmoInClipSync < 6 >  SFThrowerAmmoInClipSync;
typedef SAmmoInClipSync < 9 >  SMinigunAmmoInClipSync;
typedef SAmmoInClipSync < 1 >  SGrenadeAmmoInClipSync;
typedef SAmmoInClipSync < 1 >  STearGasAmmoInClipSync;
typedef SAmmoInClipSync < 1 >  SMolotovAmmoInClipSync;
typedef SAmmoInClipSync < 1 >  SSatchelAmmoInClipSync;
typedef SAmmoInClipSync < 9 >  SSpraycanAmmoInClipSync;
typedef SAmmoInClipSync < 9 >  SFireExtAmmoInClipSync;
typedef SAmmoInClipSync < 6 >  SCameraAmmoInClipSync;


struct SWeaponAmmoSync : public ISyncStructure
{
    SWeaponAmmoSync ( unsigned char ucWeaponType, bool bSyncTotalAmmo = true, bool bSyncAmmoInClip = true )
    : m_ucWeaponType ( ucWeaponType ), m_bSyncTotalAmmo ( bSyncTotalAmmo ), m_bSyncAmmoInClip ( bSyncAmmoInClip )
    {
    }

    bool Read ( NetBitStreamInterface& bitStream )
    {
        bool bStatus = true;
        if ( m_bSyncTotalAmmo )
            bStatus = bitStream.ReadCompressed ( data.usTotalAmmo );

        if ( m_bSyncAmmoInClip && bStatus == true )
        {
            char tmp [ 32 ];
            IAmmoInClipSync* pAmmoInClipSync = GetBestAmmoInClipSyncForWeapon ( tmp );
            if ( pAmmoInClipSync )
            {
                bStatus = bitStream.Read ( pAmmoInClipSync );
                if ( bStatus )
                    data.usAmmoInClip = pAmmoInClipSync->GetAmmoInClip ();
                else
                    data.usAmmoInClip = 0;
            }
            else
                bStatus = false;
        }

        return bStatus;
    }

    void Write ( NetBitStreamInterface& bitStream ) const
    {
        if ( m_bSyncTotalAmmo )
            bitStream.WriteCompressed ( data.usTotalAmmo );
        if ( m_bSyncAmmoInClip )
        {
            char tmp [ 32 ];
            IAmmoInClipSync* pAmmoInClipSync = GetBestAmmoInClipSyncForWeapon ( tmp );
            if ( pAmmoInClipSync )
            {
                bitStream.Write ( pAmmoInClipSync );
            }
        }
    }

    struct
    {
        unsigned short usTotalAmmo;
        unsigned short usAmmoInClip;
    } data;

private:
    unsigned char   m_ucWeaponType;
    bool            m_bSyncTotalAmmo;
    bool            m_bSyncAmmoInClip;

    IAmmoInClipSync* GetBestAmmoInClipSyncForWeapon ( void* ptr ) const
    {
        switch ( m_ucWeaponType )
        {

            case 22: return new(ptr) SPistolAmmoInClipSync ( data.usAmmoInClip );
            case 23: return new(ptr) SSilencedAmmoInClipSync ( data.usAmmoInClip );
            case 24: return new(ptr) SDeagleAmmoInClipSync ( data.usAmmoInClip );
            case 25: return new(ptr) SShotgunAmmoInClipSync ( data.usAmmoInClip );
            case 26: return new(ptr) SSawnoffAmmoInClipSync ( data.usAmmoInClip );
            case 27: return new(ptr) SSpas12AmmoInClipSync ( data.usAmmoInClip );
            case 28: return new(ptr) SUziAmmoInClipSync ( data.usAmmoInClip );
            case 29: return new(ptr) SMp5AmmoInClipSync ( data.usAmmoInClip );
            case 32: return new(ptr) STec9AmmoInClipSync ( data.usAmmoInClip );
            case 30: return new(ptr) SAk47AmmoInClipSync ( data.usAmmoInClip );
            case 31: return new(ptr) SM4AmmoInClipSync ( data.usAmmoInClip );
            case 33: return new(ptr) SRifleAmmoInClipSync ( data.usAmmoInClip );
            case 34: return new(ptr) SSniperAmmoInClipSync ( data.usAmmoInClip );
            case 35: return new(ptr) SRLauncherAmmoInClipSync ( data.usAmmoInClip );
            case 36: return new(ptr) SRPGAmmoInClipSync ( data.usAmmoInClip );
            case 37: return new(ptr) SFThrowerAmmoInClipSync ( data.usAmmoInClip );
            case 38: return new(ptr) SMinigunAmmoInClipSync ( data.usAmmoInClip );
            case 16: return new(ptr) SGrenadeAmmoInClipSync ( data.usAmmoInClip );
            case 17: return new(ptr) STearGasAmmoInClipSync ( data.usAmmoInClip );
            case 18: return new(ptr) SMolotovAmmoInClipSync ( data.usAmmoInClip );
            case 39: return new(ptr) SSatchelAmmoInClipSync ( data.usAmmoInClip );
            case 41: return new(ptr) SSpraycanAmmoInClipSync ( data.usAmmoInClip );
            case 42: return new(ptr) SFireExtAmmoInClipSync ( data.usAmmoInClip );
            case 43: return new(ptr) SCameraAmmoInClipSync ( data.usAmmoInClip );
            default:
                // Melee
                return NULL;
                break;
        }
    }
};

struct SWeaponAimSync : public ISyncStructure
{
    SWeaponAimSync ( float fWeaponRange = 0.0f, bool bFull = true ) : m_fWeaponRange ( fWeaponRange ), m_bFull ( bFull ) {}

    bool Read ( NetBitStreamInterface& bitStream )
    {
        bool bStatus = true;
        CVector vecDirection;

        short sArmY;
        if ( (bStatus = bitStream.Read ( sArmY )) )
        {
            data.fArm = ( static_cast < float > ( sArmY ) * 3.14159265f / 180 ) / 90.0f;
        }

        if ( m_bFull && bStatus )
        {
            if (( bStatus = bitStream.Read ( data.vecOrigin.fX ) && data.vecOrigin.fX > -SYNC_POSITION_LIMIT && data.vecOrigin.fX < SYNC_POSITION_LIMIT ))
                if (( bStatus = bitStream.Read ( data.vecOrigin.fY ) && data.vecOrigin.fY > -SYNC_POSITION_LIMIT && data.vecOrigin.fY < SYNC_POSITION_LIMIT ))
                    if (( bStatus = bitStream.Read ( data.vecOrigin.fZ ) && data.vecOrigin.fZ > -SYNC_POSITION_LIMIT && data.vecOrigin.fZ < SYNC_POSITION_LIMIT ))
                        bStatus = bitStream.ReadNormVector ( vecDirection.fX, vecDirection.fZ, vecDirection.fY );

            if ( bStatus )
                data.vecTarget = data.vecOrigin + ( vecDirection * m_fWeaponRange );
            else
                data.vecOrigin = data.vecTarget = CVector ( );
        }

        return bStatus;
    }

    void Write ( NetBitStreamInterface& bitStream ) const
    {
        // Write arm direction (We only sync one arm, Y axis for on foot sync and X axis for driveby)
        short sArm = static_cast < short > ( data.fArm * 90.0f * 180.0f / 3.14159265f );
        bitStream.Write ( sArm );

        if ( m_bFull )
        {
            // Write the origin of the bullets
            bitStream.Write ( data.vecOrigin.fX );
            bitStream.Write ( data.vecOrigin.fY );
            bitStream.Write ( data.vecOrigin.fZ );

            // Get the direction of the bullets
            CVector vecDirection = data.vecTarget - data.vecOrigin;
            vecDirection.Normalize ();

            // Write the normalized vector
            bitStream.WriteNormVector ( vecDirection.fX, vecDirection.fZ, vecDirection.fY );
        }
    }

    bool isFull ()
    {
        return m_bFull;
    }

    struct
    {
        float fArm;
        CVector vecOrigin;
        CVector vecTarget;
    } data;

private:
    float   m_fWeaponRange;
    bool    m_bFull;
};



//////////////////////////////////////////
//                                      //
//              Bodyparts               //
//                                      //
//////////////////////////////////////////
struct SBodypartSync : public ISyncStructure
{
    enum { BITCOUNT = 3 };

    bool Read ( NetBitStreamInterface& bitStream )
    {
        bool bStatus = bitStream.ReadBits ( (char *)&data, BITCOUNT );
        if ( bStatus )
            data.uiBodypart += 3;
        else
            data.uiBodypart = 0;
        return bStatus;
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        struct
        {
            unsigned int uiBodypart : 3;
        } privateData;

        // Bodyparts go from 3 to 9, so substracting 3 from the value
        // and then restoring it will save 1 bit.
        privateData.uiBodypart = data.uiBodypart - 3;
        bitStream.WriteBits ( (const char* )&privateData, BITCOUNT );
    }

    struct
    {
        unsigned int uiBodypart;
    } data;
};



//////////////////////////////////////////
//                                      //
//           Vehicle damage             //
//                                      //
//////////////////////////////////////////
template < unsigned int maxElements, unsigned int numBits >
struct SVehiclePartStateSync : public ISyncStructure
{
    SVehiclePartStateSync ( bool bDeltaSync = true ) : m_bDeltaSync ( bDeltaSync ) {}

    void Set ( const bool bChanged [ maxElements ], const unsigned char ucStates [ maxElements ] )
    {
        memcpy ( data.bChanged, bChanged, sizeof ( bool ) * maxElements );
        memcpy ( data.ucStates, ucStates, sizeof ( char ) * maxElements );
    }

    void Get ( bool* bChanged, unsigned char* ucStates )
    {
        memcpy ( bChanged, data.bChanged, sizeof ( bool ) * maxElements );
        memcpy ( ucStates, data.ucStates, sizeof ( char ) * maxElements );
    }

    bool Read ( NetBitStreamInterface& bitStream )
    {
        for ( unsigned int i = 0; i < maxElements; ++i )
        {
            if ( !m_bDeltaSync || ( data.bChanged [ i ] = bitStream.ReadBit () ) == true )
            {
                struct
                {
                    unsigned int uiState : numBits;
                } privateData;
                if ( !bitStream.ReadBits ( reinterpret_cast < char* > ( &privateData ), numBits ) )
                    return false;

                if ( !m_bDeltaSync )
                    data.bChanged [ i ] = true;
                data.ucStates [ i ] = privateData.uiState;
            }
        }

        return true;
    }

    void Write ( NetBitStreamInterface& bitStream ) const
    {
        for ( unsigned int i = 0; i < maxElements; ++i )
        {
            if ( !m_bDeltaSync || data.bChanged [ i ] )
            {
                struct
                {
                    unsigned int uiState : numBits;
                } privateData;
                privateData.uiState = data.ucStates [ i ];

                if ( m_bDeltaSync )
                    bitStream.WriteBit ( true );
                bitStream.WriteBits ( reinterpret_cast < const char* > ( &privateData ), numBits );
            }
            else
                bitStream.WriteBit ( false );
        }
    }

    struct
    {
        bool bChanged [ maxElements ];
        unsigned char ucStates [ maxElements ];
    } data;

private:
    bool m_bDeltaSync;
};

struct SVehicleDamageSync : public ISyncStructure
{
    SVehicleDamageSync ( bool bSyncDoors = true,
                         bool bSyncWheels = true,
                         bool bSyncPanels = true,
                         bool bSyncLights = true,
                         bool bDeltaSync = true )
    : m_bSyncDoors ( bSyncDoors ),
      m_bSyncWheels ( bSyncWheels ),
      m_bSyncPanels ( bSyncPanels ),
      m_bSyncLights ( bSyncLights ),
      m_bDeltaSync ( bDeltaSync )
    {
    }

    bool Read ( NetBitStreamInterface& bitStream )
    {
        if ( m_bSyncDoors )
        {
            // Read door states
            SVehiclePartStateSync < MAX_DOORS, 3 > doors ( m_bDeltaSync );
            if ( bitStream.Read ( &doors ) )
                doors.Get ( data.bDoorStatesChanged, data.ucDoorStates );
            else
                return false;
        }

        if ( m_bSyncWheels )
        {
            // Read wheel states
            SVehiclePartStateSync < MAX_WHEELS, 2 > wheels ( m_bDeltaSync );
            if ( bitStream.Read ( &wheels ) )
                wheels.Get ( data.bWheelStatesChanged, data.ucWheelStates );
            else
                return false;
        }

        if ( m_bSyncPanels )
        {
            // Read panel states
            SVehiclePartStateSync < MAX_PANELS, 2 > panels ( m_bDeltaSync );
            if ( bitStream.Read ( &panels ) )
                panels.Get ( data.bPanelStatesChanged, data.ucPanelStates );
            else
                return false;
        }

        if ( m_bSyncLights )
        {
            // Read light states
            SVehiclePartStateSync < MAX_LIGHTS, 2 > lights ( m_bDeltaSync );
            if ( bitStream.Read ( &lights ) )
                lights.Get ( data.bLightStatesChanged, data.ucLightStates );
            else
                return false;
        }

        return true;
    }

    void Write ( NetBitStreamInterface& bitStream ) const
    {
        if ( m_bSyncDoors )
        {
            // Write door states
            SVehiclePartStateSync < MAX_DOORS, 3 > doors ( m_bDeltaSync );
            doors.Set ( data.bDoorStatesChanged, data.ucDoorStates );
            bitStream.Write ( &doors );
        }

        if ( m_bSyncWheels )
        {
            // Write wheels states
            SVehiclePartStateSync < MAX_WHEELS, 2 > wheels ( m_bDeltaSync );
            wheels.Set ( data.bWheelStatesChanged, data.ucWheelStates );
            bitStream.Write ( &wheels );
        }

        if ( m_bSyncPanels )
        {
            // Write panel states
            SVehiclePartStateSync < MAX_PANELS, 2 > panels ( m_bDeltaSync );
            panels.Set ( data.bPanelStatesChanged, data.ucPanelStates );
            bitStream.Write ( &panels );
        }

        if ( m_bSyncLights )
        {
            // Write light states
            SVehiclePartStateSync < MAX_LIGHTS, 2 > lights ( m_bDeltaSync );
            lights.Set ( data.bLightStatesChanged, data.ucLightStates );
            bitStream.Write ( &lights );
        }
    }

    struct
    {
        bool bDoorStatesChanged  [ MAX_DOORS ];
        bool bWheelStatesChanged [ MAX_WHEELS ];
        bool bPanelStatesChanged [ MAX_PANELS ];
        bool bLightStatesChanged [ MAX_LIGHTS ];

        unsigned char ucDoorStates  [ MAX_DOORS ];
        unsigned char ucWheelStates [ MAX_WHEELS ];
        unsigned char ucPanelStates [ MAX_PANELS ];
        unsigned char ucLightStates [ MAX_LIGHTS ];
    } data;

private:
    bool m_bSyncDoors;
    bool m_bSyncWheels;
    bool m_bSyncPanels;
    bool m_bSyncLights;
    bool m_bDeltaSync;
};



//////////////////////////////////////////
//                                      //
//           Vehicle handling           //
//                                      //
//////////////////////////////////////////
struct SVehicleHandlingSync : public ISyncStructure
{
    bool Read ( NetBitStreamInterface& bitStream )
    {
        if (
            bitStream.Read    ( data.fMass ) &&
            bitStream.Read    ( data.fTurnMass ) &&
            bitStream.Read    ( data.fDragCoeff ) &&
            bitStream.Read    ( data.vecCenterOfMass.fX ) &&
            bitStream.Read    ( data.vecCenterOfMass.fY ) &&
            bitStream.Read    ( data.vecCenterOfMass.fZ ) &&
            bitStream.Read    ( data.ucPercentSubmerged ) &&
            bitStream.Read    ( data.fTractionMultiplier ) &&
            bitStream.Read    ( data.ucDriveType ) &&
            bitStream.Read    ( data.ucEngineType ) &&
            bitStream.Read    ( data.ucNumberOfGears ) &&
            bitStream.Read    ( data.fEngineAcceleration ) &&
            bitStream.Read    ( data.fEngineInertia ) &&
            bitStream.Read    ( data.fMaxVelocity ) &&
            bitStream.Read    ( data.fBrakeDeceleration ) &&
            bitStream.Read    ( data.fBrakeBias ) &&
            bitStream.ReadBit ( data.bABS ) &&
            bitStream.Read    ( data.fSteeringLock ) &&
            bitStream.Read    ( data.fTractionLoss ) &&
            bitStream.Read    ( data.fTractionBias ) &&
            bitStream.Read    ( data.fSuspensionForceLevel ) &&
            bitStream.Read    ( data.fSuspensionDamping ) &&
            bitStream.Read    ( data.fSuspensionHighSpdDamping ) &&
            bitStream.Read    ( data.fSuspensionUpperLimit ) &&
            bitStream.Read    ( data.fSuspensionLowerLimit ) &&
            bitStream.Read    ( data.fSuspensionFrontRearBias ) &&
            bitStream.Read    ( data.fSuspensionAntiDiveMultiplier ) &&
            bitStream.Read    ( data.fCollisionDamageMultiplier ) &&
            bitStream.Read    ( data.uiModelFlags ) &&
            bitStream.Read    ( data.uiHandlingFlags ) &&
            bitStream.Read    ( data.fSeatOffsetDistance ) &&
            //bitStream.Read  ( data.uiMonetary ) &&
            //bitStream.Read  ( data.ucHeadLight ) &&
            //bitStream.Read  ( data.ucTailLight ) &&
            bitStream.Read    ( data.ucAnimGroup )
            )
            return true;

        return false;
    }

    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.Write    ( data.fMass );
        bitStream.Write    ( data.fTurnMass );
        bitStream.Write    ( data.fDragCoeff );
        bitStream.Write    ( data.vecCenterOfMass.fX );
        bitStream.Write    ( data.vecCenterOfMass.fY );
        bitStream.Write    ( data.vecCenterOfMass.fZ );
        bitStream.Write    ( data.ucPercentSubmerged );
        bitStream.Write    ( data.fTractionMultiplier );
        bitStream.Write    ( data.ucDriveType );
        bitStream.Write    ( data.ucEngineType );
        bitStream.Write    ( data.ucNumberOfGears );
        bitStream.Write    ( data.fEngineAcceleration );
        bitStream.Write    ( data.fEngineInertia );
        bitStream.Write    ( data.fMaxVelocity );
        bitStream.Write    ( data.fBrakeDeceleration );
        bitStream.Write    ( data.fBrakeBias );
        bitStream.WriteBit ( data.bABS );
        bitStream.Write    ( data.fSteeringLock );
        bitStream.Write    ( data.fTractionLoss );
        bitStream.Write    ( data.fTractionBias );
        bitStream.Write    ( data.fSuspensionForceLevel );
        bitStream.Write    ( data.fSuspensionDamping );
        bitStream.Write    ( data.fSuspensionHighSpdDamping );
        bitStream.Write    ( data.fSuspensionUpperLimit );
        bitStream.Write    ( data.fSuspensionLowerLimit );
        bitStream.Write    ( data.fSuspensionFrontRearBias );
        bitStream.Write    ( data.fSuspensionAntiDiveMultiplier );
        bitStream.Write    ( data.fCollisionDamageMultiplier );
        bitStream.Write    ( data.uiModelFlags );
        bitStream.Write    ( data.uiHandlingFlags );
        bitStream.Write    ( data.fSeatOffsetDistance );
        //bitStream.Write  ( data.uiMonetary );
        //bitStream.Write  ( data.ucHeadLight );
		//bitStream.Write  ( data.ucTailLight );
        bitStream.Write    ( data.ucAnimGroup );
    }

    struct
    {
        float           fMass;                          // +4

        float           fTurnMass;                      // +12
        float           fDragCoeff;                     // +16
        CVector         vecCenterOfMass;                // +20
        unsigned char   ucPercentSubmerged;             // +32     (unsigned int - sync changes)

        float           fTractionMultiplier;            // +40

        unsigned char   ucDriveType;                    // +112
        unsigned char   ucEngineType;                   // +113
        unsigned char   ucNumberOfGears;                // +114

        float           fEngineAcceleration;           // +120     (value in handling.cfg * 0x86A950)
        float           fEngineInertia;                 // +124
        float           fMaxVelocity;                   // +128

        float           fBrakeDeceleration;            // +148
        float           fBrakeBias;                     // +152
        bool            bABS;                           // +156

        float           fSteeringLock;                  // +160
        float           fTractionLoss;                  // +164
        float           fTractionBias;                  // +168

        float           fSuspensionForceLevel;          // +172
        float           fSuspensionDamping;             // +176
        float           fSuspensionHighSpdDamping;      // +180
        float           fSuspensionUpperLimit;          // +184
        float           fSuspensionLowerLimit;          // +188
        float           fSuspensionFrontRearBias;       // +192
        float           fSuspensionAntiDiveMultiplier;  // +196

        float           fCollisionDamageMultiplier;     // +200

        unsigned int    uiModelFlags;                   // +204
        unsigned int    uiHandlingFlags;                // +208
        float           fSeatOffsetDistance;            // +212
        //unsigned int    uiMonetary;                     // +216

        //unsigned char   ucHeadLight;                    // +220
        //unsigned char   ucTailLight;                    // +221
        unsigned char   ucAnimGroup;                    // +222
    } data;
};



//////////////////////////////////////////
//                                      //
//           Explosion type             //
//                                      //
//////////////////////////////////////////
struct SExplosionTypeSync : public ISyncStructure
{
    enum { BITCOUNT = 4 };

    bool Read ( NetBitStreamInterface& bitStream )
    {
        return bitStream.ReadBits ( reinterpret_cast < char* > ( &data ), BITCOUNT );
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.WriteBits ( reinterpret_cast < const char* > ( &data ), BITCOUNT );
    }

    struct
    {
        unsigned int uiType : 4;
    } data;
};


//////////////////////////////////////////
//                                      //
//           Map info flags             //
//                                      //
//////////////////////////////////////////
struct SMapInfoFlagsSync : public ISyncStructure
{
    enum { BITCOUNT = 3 };

    bool Read ( NetBitStreamInterface& bitStream )
    {
        return bitStream.ReadBits ( reinterpret_cast < char* > ( &data ), BITCOUNT );
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.WriteBits ( reinterpret_cast < const char* > ( &data ), BITCOUNT );
    }

    struct
    {
        bool bShowNametags : 1;
        bool bShowRadar : 1;
        bool bCloudsEnabled : 1;
    } data;
};


//////////////////////////////////////////
//                                      //
//           Fun bugs state             //
//                                      //
//////////////////////////////////////////
struct SFunBugsStateSync : public ISyncStructure
{
    enum { BITCOUNT = 4 };

    bool Read ( NetBitStreamInterface& bitStream )
    {
        return bitStream.ReadBits ( reinterpret_cast < char* > ( &data ), BITCOUNT );
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.WriteBits ( reinterpret_cast < const char* > ( &data ), BITCOUNT );
    }

    struct
    {
        bool bQuickReload : 1;
        bool bFastFire : 1;
        bool bFastMove : 1;
        bool bCrouchBug : 1;
    } data;
};



//////////////////////////////////////////
//                                      //
//             Quit reasons             //
//                                      //
//////////////////////////////////////////
struct SQuitReasonSync : public ISyncStructure
{
    enum { BITCOUNT = 3 };

    bool Read ( NetBitStreamInterface& bitStream )
    {
        return bitStream.ReadBits ( reinterpret_cast < char* > ( &data ), BITCOUNT );
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.WriteBits ( reinterpret_cast < const char* > ( &data ), BITCOUNT );
    }

    struct
    {
        unsigned int uiQuitReason : 3;
    } data;
};


//////////////////////////////////////////
//                                      //
//         Entity alpha (transparency)  //
//                                      //
// As in most times the alpha value     //
// will be 100% (opaque), we write      //
// 255-alpha compressed, as 255-alpha   //
// will be then 0 in the most common    //
// case. A value of 0 is written using  //
// 1 bit, though for all other values   //
// it will use 1 extra bit.             //
//                                      //
//////////////////////////////////////////
struct SEntityAlphaSync : public ISyncStructure
{
    bool Read ( NetBitStreamInterface& bitStream )
    {
        if ( bitStream.ReadCompressed ( data.ucAlpha ) )
        {
            data.ucAlpha = 255 - data.ucAlpha;
            return true;
        }
        return false;
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        unsigned char ucAlpha = 255 - data.ucAlpha;
        bitStream.WriteCompressed ( ucAlpha );
    }

    struct
    {
        unsigned char ucAlpha;
    } data;
};


//////////////////////////////////////////
//                                      //
//             Marker types             //
//                                      //
//////////////////////////////////////////
struct SMarkerTypeSync : public ISyncStructure
{
    enum { BITCOUNT = 3 };

    bool Read ( NetBitStreamInterface& bitStream )
    {
        return bitStream.ReadBits ( reinterpret_cast < char* > ( &data ), BITCOUNT );
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.WriteBits ( reinterpret_cast < const char* > ( &data ), BITCOUNT );
    }

    struct
    {
        unsigned char ucType : 3;
    } data;
};


//////////////////////////////////////////
//                                      //
//             Pickup types             //
//                                      //
//////////////////////////////////////////
struct SPickupTypeSync : public ISyncStructure
{
    enum { BITCOUNT = 3 };

    bool Read ( NetBitStreamInterface& bitStream )
    {
        return bitStream.ReadBits ( reinterpret_cast < char* > ( &data ), BITCOUNT );
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.WriteBits ( reinterpret_cast < const char* > ( &data ), BITCOUNT );
    }

    struct
    {
        unsigned char ucType : 3;
    } data;
};


//////////////////////////////////////////
//                                      //
//           Colshape types             //
//                                      //
//////////////////////////////////////////
struct SColshapeTypeSync : public ISyncStructure
{
    enum { BITCOUNT = 3 };

    bool Read ( NetBitStreamInterface& bitStream )
    {
        return bitStream.ReadBits ( reinterpret_cast < char* > ( &data ), BITCOUNT );
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.WriteBits ( reinterpret_cast < const char* > ( &data ), BITCOUNT );
    }

    struct
    {
        unsigned char ucType : 3;
    } data;
};


//////////////////////////////////////////
//                                      //
//                Colors                //
//                                      //
//////////////////////////////////////////
struct SColorSync : public ISyncStructure
{
    bool Read ( NetBitStreamInterface& bitStream )
    {
        return bitStream.ReadBits ( reinterpret_cast < char* > ( &data ), 32 );
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.WriteBits ( reinterpret_cast < const char* > ( &data ), 32 );
    }

    // From SColor
    SColorSync( void ) {}
    SColorSync( SColor color )
    {
        data.ucR = color.R;
        data.ucG = color.G;
        data.ucB = color.B;
        data.ucA = color.A;
    }

    // To SColor
    operator SColor( void ) const
    {
        return SColorRGBA ( data.ucR, data.ucG, data.ucB, data.ucA );
    }

    struct
    {
        unsigned char ucR;
        unsigned char ucG;
        unsigned char ucB;
        unsigned char ucA;
    } data;
};


//////////////////////////////////////////
//                                      //
//        Occupied vehicle seat         //
//                                      //
//////////////////////////////////////////
struct SOccupiedSeatSync : public ISyncStructure
{
    enum { BITCOUNT = 3 };

    bool Read ( NetBitStreamInterface& bitStream )
    {
        return bitStream.ReadBits ( reinterpret_cast < char* > ( &data ), BITCOUNT );
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.WriteBits ( reinterpret_cast < const char* > ( &data ), BITCOUNT );
    }

    struct
    {
        unsigned char ucSeat : 3;
    } data;
};


//////////////////////////////////////////
//                                      //
//          Vehicle paintjob            //
//                                      //
//////////////////////////////////////////
struct SPaintjobSync : public ISyncStructure
{
    enum { BITCOUNT = 2 };

    bool Read ( NetBitStreamInterface& bitStream )
    {
        return bitStream.ReadBits ( reinterpret_cast < char* > ( &data ), BITCOUNT );
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.WriteBits ( reinterpret_cast < const char* > ( &data ), BITCOUNT );
    }

    struct
    {
        unsigned char ucPaintjob : 2;
    } data;
};


//////////////////////////////////////////
//                                      //
//    Vehicle override lights state     //
//                                      //
//////////////////////////////////////////
struct SOverrideLightsSync : public ISyncStructure
{
    enum { BITCOUNT = 2 };

    bool Read ( NetBitStreamInterface& bitStream )
    {
        return bitStream.ReadBits ( reinterpret_cast < char* > ( &data ), BITCOUNT );
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.WriteBits ( reinterpret_cast < const char* > ( &data ), BITCOUNT );
    }

    struct
    {
        unsigned char ucOverride : 2;
    } data;
};


//////////////////////////////////////////
//                                      //
//              Lua types               //
//                                      //
//////////////////////////////////////////
struct SLuaTypeSync : public ISyncStructure
{
    enum { BITCOUNT = 4 };

    bool Read ( NetBitStreamInterface& bitStream )
    {
        return bitStream.ReadBits ( reinterpret_cast < char* > ( &data ), BITCOUNT );
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.WriteBits ( reinterpret_cast < const char* > ( &data ), BITCOUNT );
    }

    struct
    {
        unsigned char ucType : 4;
    } data;
};


//////////////////////////////////////////
//                                      //
//            Mouse button              //
//                                      //
//////////////////////////////////////////
struct SMouseButtonSync : public ISyncStructure
{
    enum { BITCOUNT = 3 };

    bool Read ( NetBitStreamInterface& bitStream )
    {
        return bitStream.ReadBits ( reinterpret_cast < char* > ( &data ), BITCOUNT );
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.WriteBits ( reinterpret_cast < const char* > ( &data ), BITCOUNT );
    }

    struct
    {
        unsigned char ucButton : 3;
    } data;
};


//////////////////////////////////////////
//                                      //
//              Heat haze               //
//                                      //
//////////////////////////////////////////
struct SHeatHazeSync : public ISyncStructure
{
    SHeatHazeSync( void ) {}
    SHeatHazeSync( const SHeatHazeSettings& settings )
    {
        data.settings = settings;
    }

    // To SHeatHazeSettings
    operator SHeatHazeSettings( void ) const
    {
        return data.settings;
    }

    template < unsigned int bits, typename T >
    bool ReadRange ( NetBitStreamInterface& bitStream, T& outvalue, const T low, const T hi )
    {
        T temp;
        if ( !bitStream.ReadBits ( &temp, bits ) )
            return false;
        outvalue = Clamp < T > ( low, temp + low, hi );
        return true;
    }

    template < unsigned int bits, typename T >
    void WriteRange ( NetBitStreamInterface& bitStream, const T value, const T low, const T hi ) const
    {
        T temp = Clamp < T > ( low, value, hi ) - low;
        bitStream.WriteBits ( &temp, bits );
    }

    bool Read ( NetBitStreamInterface& bitStream )
    {
        return  bitStream.Read ( data.settings.ucIntensity ) &&
                bitStream.Read ( data.settings.ucRandomShift ) &&
                ReadRange < 10, ushort > ( bitStream, data.settings.usSpeedMin, 0, 1000 ) &&
                ReadRange < 10, ushort > ( bitStream, data.settings.usSpeedMax, 0, 1000 ) &&
                ReadRange < 11, short > ( bitStream, data.settings.sScanSizeX, -1000, 1000 ) &&
                ReadRange < 11, short > ( bitStream, data.settings.sScanSizeY, -1000, 1000 ) &&
                ReadRange < 10, ushort > ( bitStream, data.settings.usRenderSizeX, 0, 1000 ) &&
                ReadRange < 10, ushort > ( bitStream, data.settings.usRenderSizeY, 0, 1000 ) &&
                bitStream.ReadBit ( data.settings.bInsideBuilding );
    }

    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.Write ( data.settings.ucIntensity );
        bitStream.Write ( data.settings.ucRandomShift );
        WriteRange < 10, ushort > ( bitStream, data.settings.usSpeedMin, 0, 1000 );
        WriteRange < 10, ushort > ( bitStream, data.settings.usSpeedMax, 0, 1000 );
        WriteRange < 11, short > ( bitStream, data.settings.sScanSizeX, -1000, 1000 );
        WriteRange < 11, short > ( bitStream, data.settings.sScanSizeY, -1000, 1000 );
        WriteRange < 10, ushort > ( bitStream, data.settings.usRenderSizeX, 0, 1000 );
        WriteRange < 10, ushort > ( bitStream, data.settings.usRenderSizeY, 0, 1000 );
        bitStream.WriteBit ( data.settings.bInsideBuilding );
    }

    struct
    {
        SHeatHazeSettings settings;
    } data;
};

#pragma pack(pop)
