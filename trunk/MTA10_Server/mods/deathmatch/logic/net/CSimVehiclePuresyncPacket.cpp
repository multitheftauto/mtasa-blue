/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*****************************************************************************/

#include "StdInc.h"
#include "SimHeaders.h"


 CSimVehiclePuresyncPacket::CSimVehiclePuresyncPacket ( ElementID PlayerID,
                                                        ushort usPlayerLatency,
                                                        uchar ucPlayerSyncTimeContext,
                                                        bool bPlayerHasOccupiedVehicle,
                                                        ushort usVehicleGotModel,
                                                        uchar ucPlayerGotOccupiedVehicleSeat,
                                                        uchar ucPlayerGotWeaponType,
                                                        float fPlayerGotWeaponRange,
                                                        CControllerState& sharedControllerState )
    : m_PlayerID ( PlayerID )
    , m_usPlayerLatency ( usPlayerLatency )
    , m_ucPlayerSyncTimeContext ( ucPlayerSyncTimeContext )
    , m_bPlayerHasOccupiedVehicle ( bPlayerHasOccupiedVehicle )
    , m_usVehicleGotModel ( usVehicleGotModel )
    , m_ucPlayerGotOccupiedVehicleSeat ( ucPlayerGotOccupiedVehicleSeat )
    , m_ucPlayerGotWeaponType ( ucPlayerGotWeaponType )
    , m_fPlayerGotWeaponRange ( fPlayerGotWeaponRange )
    , m_sharedControllerState ( sharedControllerState )
{
}


//
// Should do the same this as what CVehiclePuresyncPacket::Read() does
//
bool CSimVehiclePuresyncPacket::Read ( NetBitStreamInterface& BitStream )
{
    // Player is in a vehicle?
    if ( m_bPlayerHasOccupiedVehicle )
    {
        // Read out the time context
        if ( !BitStream.Read ( m_Cache.ucTimeContext ) )
            return false;

        // If incoming time context is zero, use the one here
        if ( m_Cache.ucTimeContext == 0 )
            m_Cache.ucTimeContext = m_ucPlayerSyncTimeContext;

        // Only read this packet if it matches the current time context that
        // player is in.
        if ( !CanUpdateSync ( m_Cache.ucTimeContext ) )
        {
            return false;
        }

        // Read out the keysync data
        if ( !ReadFullKeysync ( m_sharedControllerState, BitStream ) )
            return false;

        // Read out its position
        SPositionSync position ( false );
        if ( !BitStream.Read ( &position ) )
            return false;
        m_Cache.PlrPosition = position.data.vecPosition;

        if ( m_usVehicleGotModel == 449 ||
            m_usVehicleGotModel == 537 ||
            m_usVehicleGotModel == 538 ||
            m_usVehicleGotModel == 570 ||
            m_usVehicleGotModel == 569 ||
            m_usVehicleGotModel == 590 )
        {
            // Train specific data
            float fRailPosition = 0.0f;
            uchar ucRailTrack = 0;
            bool bRailDirection = false;
            float fRailSpeed = 0.0f;
            if ( !BitStream.Read ( fRailPosition ) || !BitStream.ReadBit ( bRailDirection ) || !BitStream.Read ( ucRailTrack ) || !BitStream.Read ( fRailSpeed ) )
            {
                return false;
            }
            m_Cache.fRailPosition = fRailPosition;
            m_Cache.bRailDirection = bRailDirection;
            m_Cache.ucRailTrack = ucRailTrack;
            m_Cache.fRailSpeed = fRailSpeed;
        }

        // Read the camera orientation
        ReadCameraOrientation ( position.data.vecPosition, BitStream, m_Cache.vecCamPosition, m_Cache.vecCamFwd );

        // Jax: don't allow any outdated packets through
        SOccupiedSeatSync seat;
        if ( !BitStream.Read ( &seat ) )
            return false;
        if ( seat.data.ucSeat != m_ucPlayerGotOccupiedVehicleSeat )
        {
            // Mis-matching seats can happen when we warp into a different one,
            // which will screw up the whole packet
            return false;
        }

        // Read out the vehicle matrix only if he's the driver
        const unsigned int uiSeat = m_ucPlayerGotOccupiedVehicleSeat;
        if ( uiSeat == 0 )
        {
            // Read out the vehicle rotation in degrees
            SRotationDegreesSync rotation;
            if( !BitStream.Read ( &rotation ) )
                return false;

            // Set it
            m_Cache.VehPosition = position.data.vecPosition;
            m_Cache.VehRotationDeg = rotation.data.vecRotation;

            // Move speed vector
            SVelocitySync velocity;
            if ( !BitStream.Read ( &velocity ) )
                return false;

            m_Cache.BothVelocity = velocity.data.vecVelocity;

            // Turn speed vector
            SVelocitySync turnSpeed;
            if ( !BitStream.Read ( &turnSpeed ) )
                return false;

            m_Cache.VehTurnSpeed = turnSpeed.data.vecVelocity;

            // Health
            SVehicleHealthSync health;
            if ( !BitStream.Read ( &health ) )
                return false;
            m_Cache.fVehHealth = health.data.fValue;

            // Trailer chain
            bool bHasTrailer;
            if ( !BitStream.ReadBit ( bHasTrailer ) )
                return false;

            while ( bHasTrailer )
            {
                STrailerInfo info;
                BitStream.Read ( info.m_TrailerID );
                
                // Read out the trailer position and rotation
                SPositionSync trailerPosition ( false );
                if ( !BitStream.Read ( &trailerPosition ) )
                    return false;

                SRotationDegreesSync trailerRotation;
                if ( !BitStream.Read ( &trailerRotation ) )
                    return false;

                // If we found the trailer
                if ( true )
                {
                    // Set its position and rotation
                    info.m_TrailerPosition = trailerPosition.data.vecPosition;
                    info.m_TrailerRotationDeg = trailerRotation.data.vecRotation;

                    m_Cache.TrailerList.push_back ( info );
                }
                else
                    break;

                if ( BitStream.ReadBit ( bHasTrailer ) == false )
                    return false;
            }
        }

        // Player health
        SPlayerHealthSync health;
        if ( !BitStream.Read ( &health ) )
            return false;
        m_Cache.fPlrHealth = health.data.fValue;

        // Armor
        SPlayerArmorSync armor;
        if ( !BitStream.Read ( &armor ) )
            return false;
        m_Cache.fArmor = armor.data.fValue;

        // Flags
        if ( !BitStream.Read ( &m_Cache.flags ) )
            return false;

        // Weapon sync
        if ( m_Cache.flags.data.bHasAWeapon )
        {
            SWeaponSlotSync slot;
            if ( !BitStream.Read ( &slot ) )
                return false;

            m_Cache.ucWeaponSlot = slot.data.uiSlot;

            if ( m_Cache.flags.data.bIsDoingGangDriveby && CWeaponNames::DoesSlotHaveAmmo ( slot.data.uiSlot ) )
            {
                // Read the ammo states
                SWeaponAmmoSync ammo ( m_ucPlayerGotWeaponType, BitStream.Version () >= 0x44, true );
                if ( !BitStream.Read ( &ammo ) )
                    return false;
                m_Cache.usAmmoInClip = ammo.data.usAmmoInClip;
                m_Cache.usTotalAmmo = ammo.data.usTotalAmmo;

                // Read aim data
                SWeaponAimSync aim ( m_fPlayerGotWeaponRange, true );
                if ( !BitStream.Read ( &aim ) )
                    return false;
                m_Cache.fAimDirection = aim.data.fArm;
                m_Cache.vecSniperSource = aim.data.vecOrigin;
                m_Cache.vecTargetting = aim.data.vecTarget;

                // Read the driveby direction
                SDrivebyDirectionSync driveby;
                if ( !BitStream.Read ( &driveby ) )
                    return false;
                m_Cache.ucDriveByDirection = driveby.data.ucDirection;
            }
        }
        else
            m_Cache.ucWeaponSlot = 0;


        // Vehicle specific data if he's the driver
        if ( uiSeat == 0 )
        {
            ReadVehicleSpecific ( BitStream );
        }

        // Read the vehicle_look_left and vehicle_look_right control states
        // if it's an aircraft.
        if ( m_Cache.flags.data.bIsAircraft )
        {
            m_sharedControllerState.LeftShoulder2 = BitStream.ReadBit () * 255;
            m_sharedControllerState.RightShoulder2 = BitStream.ReadBit () * 255;
        }

        // Success
        return true;
    }

    return false;
}


//
// Should do the same this as what CVehiclePuresyncPacket::Write() does
//
bool CSimVehiclePuresyncPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    // Player is in a vehicle and is the driver?
    if ( m_bPlayerHasOccupiedVehicle )
    {
        // Player ID
        BitStream.Write ( m_PlayerID );

        // Write the time context of that player
        BitStream.Write ( m_Cache.ucTimeContext );

        // Write his ping divided with 2 plus a small number so the client can find out when this packet was sent
        BitStream.WriteCompressed ( m_usPlayerLatency );

        // Write the keysync data
        WriteFullKeysync ( m_sharedControllerState, BitStream );

        // Write the vehicle matrix only if he's the driver
        CVector vecTemp;
        unsigned int uiSeat = m_ucPlayerGotOccupiedVehicleSeat;
        if ( uiSeat == 0 )
        {
            // Vehicle position
            SPositionSync position ( false );
            position.data.vecPosition = m_Cache.VehPosition;
            BitStream.Write ( &position );

            if ( m_usVehicleGotModel == 449 ||
                m_usVehicleGotModel == 537 ||
                m_usVehicleGotModel == 538 ||
                m_usVehicleGotModel == 570 ||
                m_usVehicleGotModel == 569 ||
                m_usVehicleGotModel == 590 )
            {
                BitStream.Write ( m_Cache.fRailPosition );
                BitStream.WriteBit ( m_Cache.bRailDirection );
                BitStream.Write ( m_Cache.ucRailTrack );
                BitStream.Write ( m_Cache.fRailSpeed );
            }

            // Vehicle rotation
            SRotationDegreesSync rotation;
            rotation.data.vecRotation = m_Cache.VehRotationDeg;
            BitStream.Write ( &rotation );

            // Move speed vector
            SVelocitySync velocity;
            velocity.data.vecVelocity = m_Cache.BothVelocity;
            BitStream.Write ( &velocity );

            // Turn speed vector
            SVelocitySync turnSpeed;
            turnSpeed.data.vecVelocity = m_Cache.VehTurnSpeed;
            BitStream.Write ( &turnSpeed );

            // Health
            SVehicleHealthSync health;
            health.data.fValue = m_Cache.fVehHealth;
            BitStream.Write ( &health );

            // Write trailer chain
            if ( BitStream.Version () >= 0x42 )
            {
                for ( std::vector< STrailerInfo >::const_iterator it = m_Cache.TrailerList.begin (); it != m_Cache.TrailerList.end (); it++ )
                {
                    BitStream.WriteBit ( true );

                    BitStream.Write ( it->m_TrailerID );

                    SPositionSync trailerPosition ( false );
                    trailerPosition.data.vecPosition = it->m_TrailerPosition;
                    BitStream.Write ( &trailerPosition );

                    SRotationDegreesSync trailerRotation;
                    trailerRotation.data.vecRotation = it->m_TrailerRotationDeg;
                    BitStream.Write ( &trailerRotation );
                }

                BitStream.WriteBit ( false );
            }
        }

        // Player health and armor
        SPlayerHealthSync health;
        health.data.fValue = m_Cache.fPlrHealth;
        BitStream.Write ( &health );

        SPlayerArmorSync armor;
        armor.data.fValue = m_Cache.fArmor;
        BitStream.Write ( &armor );

        // Weapon
        unsigned char ucWeaponType = m_ucPlayerGotWeaponType;

        BitStream.Write ( &m_Cache.flags );

        // Write the weapon stuff
        if ( m_Cache.flags.data.bHasAWeapon )
        {
            // Write the weapon slot
            SWeaponSlotSync slot;
            slot.data.uiSlot = m_Cache.ucWeaponSlot;
            BitStream.Write ( &slot );

            if ( m_Cache.flags.data.bIsDoingGangDriveby && CWeaponNames::DoesSlotHaveAmmo ( slot.data.uiSlot ) )
            {
                // Write the ammo states
                SWeaponAmmoSync ammo ( ucWeaponType, BitStream.Version () >= 0x44, true );
                ammo.data.usAmmoInClip = m_Cache.usAmmoInClip;
                ammo.data.usTotalAmmo = m_Cache.usTotalAmmo;
                BitStream.Write ( &ammo );

                // Sync aim data
                SWeaponAimSync aim ( 0.0f, true );
                aim.data.vecOrigin = m_Cache.vecSniperSource;
                aim.data.vecTarget = m_Cache.vecTargetting;
                aim.data.fArm = m_Cache.fAimDirection;
                BitStream.Write ( &aim );

                // Sync driveby direction
                SDrivebyDirectionSync driveby;
                driveby.data.ucDirection = m_Cache.ucDriveByDirection;
                BitStream.Write ( &driveby );
            }
        }

        // Vehicle specific data only if he's the driver
        if ( uiSeat == 0 )
        {
            WriteVehicleSpecific ( BitStream );
        }

        // Write vehicle_look_left and vehicle_look_right control states when
        // it's an aircraft.
        if ( m_Cache.flags.data.bIsAircraft )
        {
            BitStream.WriteBit ( m_sharedControllerState.LeftShoulder2 != 0 );
            BitStream.WriteBit ( m_sharedControllerState.RightShoulder2 != 0 );
        }

        // Success
        return true;
    }

    return false;
}


void CSimVehiclePuresyncPacket::ReadVehicleSpecific ( NetBitStreamInterface& BitStream )
{
    // Turret data
    unsigned short usModel = m_usVehicleGotModel;
    if ( CVehicleManager::HasTurret ( usModel ) ) 
    {
        // Read out the turret position
        SVehicleTurretSync vehicle;
        if ( !BitStream.Read ( &vehicle ) )
            return;

        // Set the data
        m_Cache.fTurretX = vehicle.data.fTurretX;
        m_Cache.fTurretY = vehicle.data.fTurretY;
    }

    // Adjustable property value
    if ( CVehicleManager::HasAdjustableProperty ( usModel ) )
    {
        unsigned short usAdjustableProperty;
        if ( BitStream.Read ( usAdjustableProperty ) )
        {
            m_Cache.usAdjustableProperty = usAdjustableProperty;
        }
    }

    // Door angles.
    if ( CVehicleManager::HasDoors ( usModel ) )
    {
        SDoorOpenRatioSync door;

        for ( unsigned int i = 2; i < 6; ++i )
        {
            if ( !BitStream.Read ( &door ) )
                return;
            m_Cache.fDoorOpenRatio[ i-2 ] = door.data.fRatio;
        }
    }
}


void CSimVehiclePuresyncPacket::WriteVehicleSpecific ( NetBitStreamInterface& BitStream ) const
{
    // Turret states
    unsigned short usModel = m_usVehicleGotModel;
    if ( CVehicleManager::HasTurret ( usModel ) )
    {
        SVehicleTurretSync vehicle;
        vehicle.data.fTurretX = m_Cache.fTurretX;
        vehicle.data.fTurretY = m_Cache.fTurretY;
        BitStream.Write ( &vehicle );
    }

    // Adjustable property value
    if ( CVehicleManager::HasAdjustableProperty ( usModel ) )
    {
        BitStream.Write ( m_Cache.usAdjustableProperty );
    }

    // Door angles.
    if ( CVehicleManager::HasDoors ( usModel ) )
    {
        SDoorOpenRatioSync door;
        for ( unsigned int i = 2; i < 6; ++i )
        {
            door.data.fRatio = m_Cache.fDoorOpenRatio[i-2];
            BitStream.Write ( &door );
        }
    }
}
