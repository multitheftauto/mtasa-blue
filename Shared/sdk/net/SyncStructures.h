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
#include <ieee754.h>

// Used to make sure that any position values we receive are at least half sane
#define SYNC_POSITION_LIMIT 100000.0f

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
        struct
        {
            int iMin : integerBits;
            int iMax : integerBits;
        } limits;
        limits.iMax = ( 1 << ( integerBits - 1 ) ) - 1;
        limits.iMin = limits.iMax + 1;

        IEEE754_DP dValue ( data.fValue );
        assert ( !dValue.isnan () );

        if ( dValue > limits.iMax ) dValue = (double)limits.iMax;
        else if ( dValue < limits.iMin ) dValue = (double)limits.iMin;

        SFixedPointNumber num;
        num.iValue = (int)( dValue * (double)( 1 << fractionalBits ));

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
    SPlayerHealthSync () : SFloatAsBitsSync ( 0.f, 200.0f, true ) {}
};

struct SPlayerArmorSync : public SFloatAsBitsSync < 8 >
{
    SPlayerArmorSync () : SFloatAsBitsSync ( 0.f, 100.0f, true ) {}
};

struct SVehicleHealthSync : public SFloatAsBitsSync < 12 >
{
    SVehicleHealthSync () : SFloatAsBitsSync ( 0.f, 2000.0f, true ) {}
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
            bitStream.Write ( data.vecPosition.fX );
            bitStream.Write ( data.vecPosition.fY );
            bitStream.Write ( data.vecPosition.fZ );
        }
        else
        {
            SFloatSync < 14, 10 > x, y;
            x.data.fValue = data.vecPosition.fX;
            y.data.fValue = data.vecPosition.fY;

            bitStream.Write ( &x );
            bitStream.Write ( &y );
            bitStream.Write ( data.vecPosition.fZ );
        }
    }

    struct
    {
        CVector vecPosition;
    } data;

private:
    bool m_bUseFloats;
};



//////////////////////////////////////////
//                                      //
//        Rotation Degrees              //
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
    enum { BITCOUNT = 11 };

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
    } data;
};


struct SPedRotationSync : public ISyncStructure
{
    bool Read ( NetBitStreamInterface& bitStream )
    {
        unsigned short sValue;
        if ( bitStream.Read ( sValue ) )
        {
            data.fRotation = static_cast < float > ( sValue ) * 3.14159265f / 16200.0f;
            return true;
        }
        return false;
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        float fRotation = data.fRotation;
        short sValue = static_cast < short > ( fRotation * 16200.0f / 3.14159265f );
        bitStream.Write ( sValue );
    }

    struct
    {
        float fRotation;
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
        if ( bitStream.ReadCompressed ( data.vehicleID ) &&
             bitStream.Read ( data.ucTimeContext ) &&
             bitStream.ReadBits ( (char *)&data, 8 ) )
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
                bitStream.ReadCompressed ( data.trailer );
            }

            return true;
        }
        return false;
    }

    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.WriteCompressed ( data.vehicleID );
        bitStream.Write ( data.ucTimeContext );
        bitStream.WriteBits ( (const char* )&data, 8 );

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
            bitStream.WriteCompressed ( data.trailer );
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
    enum { BITCOUNT = 3 };

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
        bool bState;
        char cLeftStickX;
        char cLeftStickY;

        if ( ( bState = bitStream.ReadBits ( (char *)&data, 10 ) ) )
        {
            if ( data.bLeftStickXChanged && ( bState = bitStream.Read ( cLeftStickX ) ) )
                data.sLeftStickX = static_cast < short > ( (float)cLeftStickX * 128.0f/127.0f );
            if ( bState && data.bLeftStickYChanged && ( bState = bitStream.Read ( cLeftStickY ) ) )
                data.sLeftStickY = static_cast < short > ( (float)cLeftStickY * 128.0f/127.0f );
        }

        return bState;
    }
    void Write ( NetBitStreamInterface& bitStream ) const
    {
        bitStream.WriteBits ( (const char* )&data, 10 );
        if ( data.bLeftStickXChanged )
        {
            char cLeftStickX = static_cast < char > ( (float)data.sLeftStickX * 127.0f/128.0f );
            bitStream.Write ( cLeftStickX );
        }
        if ( data.bLeftStickYChanged )
        {
            char cLeftStickY = static_cast < char > ( (float)data.sLeftStickY * 127.0f/128.0f );
            bitStream.Write ( cLeftStickY );
        }
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
        bool bLeftStickXChanged : 1;
        bool bLeftStickYChanged : 1;
        short sLeftStickX;
        short sLeftStickY;
    } data;
};

struct SVehicleSpecific : public ISyncStructure
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
//                Others                //
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


#pragma pack(pop)
