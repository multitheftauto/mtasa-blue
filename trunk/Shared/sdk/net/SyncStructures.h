/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/sdk/net/SyncStructures.h
*  PURPOSE:     Structures used for syncing stuff through the network.
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#include <CVector.h>
#include <net/bitstream.h>

#pragma pack(push)
#pragma pack(1)

// Interface for all sync structures
struct ISyncStructure
{
    virtual         ~ISyncStructure () {}
    virtual bool    Read            ( NetBitStreamInterface& bitStream ) = 0;
    virtual void    Write           ( NetBitStreamInterface& bitStream ) = 0;
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
        return bitStream.ReadBits ( reinterpret_cast < char* > ( &data ), BITCOUNT );
    }
    void Write ( NetBitStreamInterface& bitStream )
    {
        bitStream.WriteBits ( reinterpret_cast < const char* > ( &data ), BITCOUNT );
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
        return bitStream.ReadBits ( reinterpret_cast < char* > ( &data ), BITCOUNT );
    }
    void Write ( NetBitStreamInterface& bitStream )
    {
        bitStream.WriteBits ( reinterpret_cast < const char* > ( &data ), BITCOUNT );
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

        if ( ( bState = bitStream.ReadBits ( reinterpret_cast < char* > ( &data ), 8 ) ) )
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
    void Write ( NetBitStreamInterface& bitStream )
    {
        bitStream.WriteBits ( reinterpret_cast < const char* > ( &data ), 8 );
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

        if ( ( bState = bitStream.ReadBits ( reinterpret_cast < char* > ( &data ), 10 ) ) )
        {
            if ( data.bLeftStickXChanged && ( bState = bitStream.Read ( cLeftStickX ) ) )
                data.sLeftStickX = static_cast < short > ( (float)cLeftStickX * 128.0f/127.0f );
            if ( bState && data.bLeftStickYChanged && ( bState = bitStream.Read ( cLeftStickY ) ) )
                data.sLeftStickY = static_cast < short > ( (float)cLeftStickY * 128.0f/127.0f );
        }

        return bState;
    }
    void Write ( NetBitStreamInterface& bitStream )
    {
        bitStream.WriteBits ( reinterpret_cast < const char* > ( &data ), 10 );
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
        return bitStream.ReadBits ( reinterpret_cast < char* > ( &data ), BITCOUNT );
    }
    void Write ( NetBitStreamInterface& bitStream )
    {
        bitStream.WriteBits ( reinterpret_cast < const char* > ( &data ), BITCOUNT );
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
        return bitStream.ReadBits ( reinterpret_cast < char* > ( &data ), BITCOUNT );
    }
    void Write ( NetBitStreamInterface& bitStream )
    {
        bitStream.WriteBits ( reinterpret_cast < const char* > ( &data ), BITCOUNT );
    }

    struct
    {
        unsigned int uiWeaponType : 6;
    } data;
};

struct SAmmoInClipSync : public virtual ISyncStructure
{
private:
    unsigned char m_usBitCount;

public:	
    SAmmoInClipSync ( unsigned short usAmmoInClip, unsigned char usBitCount = 16 )
    {
        data.usAmmoInClip = usAmmoInClip;
        m_usBitCount = usBitCount;
    }

    bool Read ( NetBitStreamInterface& bitStream )
    {
        return bitStream.ReadBits ( reinterpret_cast < char* > ( &data ), m_usBitCount );
    }

    void Write ( NetBitStreamInterface& bitStream )
    {
        bitStream.WriteBits ( reinterpret_cast < const char* > ( &data ), m_usBitCount );
    }
    
    unsigned short GetAmmoInClip () const
    {
        return data.usAmmoInClip;
    }

    struct
    {
        unsigned short usAmmoInClip;
    } data;
};

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
            unsigned int usBitCount = GetAmmoInClipSyncBitCount ();
            if ( usBitCount != 0 )
            {
                SAmmoInClipSync ammoInClipSync = SAmmoInClipSync( data.usAmmoInClip, usBitCount );
                bStatus = bitStream.Read ( &ammoInClipSync );
                if ( bStatus )
                    data.usAmmoInClip = ammoInClipSync.GetAmmoInClip ();
                else
                    data.usAmmoInClip = 0;
            }
            else
                bStatus = false;
        }

        return bStatus;
    }

    void Write ( NetBitStreamInterface& bitStream )
    {
        if ( m_bSyncTotalAmmo )
            bitStream.WriteCompressed ( data.usTotalAmmo );
        if ( m_bSyncAmmoInClip )
        {
            unsigned int usBitCount = GetAmmoInClipSyncBitCount ();
            if ( usBitCount != 0 )
            {
                SAmmoInClipSync ammoInClipSync = SAmmoInClipSync( data.usAmmoInClip, usBitCount );
                bitStream.Write ( &ammoInClipSync );
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

    unsigned int GetAmmoInClipSyncBitCount ( )
    {
        switch ( m_ucWeaponType )
        {
            case 22: return 6; // Pistol
            case 23: return 5; // Silenced
            case 24: return 3; // Deagle
            case 25: return 1; // Shotgun
            case 26: return 3; // Sawnoff
            case 27: return 3; // Spas12
            case 28: return 7; // Uzi
            case 29: return 5; // Mp5
            case 32: return 7; // Tec9
            case 30: return 5; // Ak47
            case 31: return 6; // M4
            case 33: return 1; // Rifle
            case 34: return 1; // Sniper
            case 35: return 1; // RLauncher
            case 36: return 1; // RPG
            case 37: return 6; // FThrower
            case 38: return 9; // Minigun
            case 16: return 1; // Grenade
            case 17: return 1; // TearGas
            case 18: return 1; // Molotov
            case 39: return 1; // Satchel
            case 41: return 9; // Spraycan
            case 42: return 9; // FireExt
            case 43: return 6; // Camera
            default: return 0; // Melee
        }
    }

};

struct SWeaponAimSync : public ISyncStructure
{
    SWeaponAimSync ( float fWeaponRange = 0.0f ) : m_fWeaponRange ( fWeaponRange ) {}

    bool Read ( NetBitStreamInterface& bitStream )
    {
        bool bStatus = true;
        CVector vecDirection;

        if (( bStatus = bitStream.Read ( data.vecOrigin.fX ) ))
            if (( bStatus = bitStream.Read ( data.vecOrigin.fY ) ))
                if (( bStatus = bitStream.Read ( data.vecOrigin.fZ ) ))
                    bStatus = bitStream.ReadNormVector ( vecDirection.fX, vecDirection.fZ, vecDirection.fY );

        if ( bStatus )
            data.vecTarget = data.vecOrigin + ( vecDirection * m_fWeaponRange );
        else
            data.vecOrigin = data.vecTarget = CVector ( );

        return bStatus;
    }

    void Write ( NetBitStreamInterface& bitStream )
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

    struct
    {
        CVector vecOrigin;
        CVector vecTarget;
    } data;

private:
    float   m_fWeaponRange;
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
        bool bStatus = bitStream.ReadBits ( reinterpret_cast < char* > ( &privateData ), BITCOUNT );
        if ( bStatus )
            data.uiBodypart = privateData.uiBodypart + 3;
        else
            data.uiBodypart = 0;
        return bStatus;
    }
    void Write ( NetBitStreamInterface& bitStream )
    {
        // Bodyparts go from 3 to 9, so substracting 3 from the value
        // and then restoring it will save 1 bit.
        privateData.uiBodypart = data.uiBodypart - 3;
        bitStream.WriteBits ( reinterpret_cast < const char* > ( &privateData ), BITCOUNT );
    }

    struct
    {
        unsigned int uiBodypart;
    } data;

private:
    struct
    {
        unsigned int uiBodypart : 3;
    } privateData;
};


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

    void Write ( NetBitStreamInterface& bitStream )
    {
        float fModulus = data.vecVelocity.Normalize ();
        if ( fModulus == 0.0f )
            bitStream.WriteBit ( false );
        else
        {
            bitStream.WriteBit ( true );
            bitStream.Write ( fModulus );
            bitStream.WriteNormVector ( data.vecVelocity.fX, data.vecVelocity.fY, data.vecVelocity.fZ );
        }
    }

    struct
    {
        CVector vecVelocity;
    } data;
};


#pragma pack(pop)
