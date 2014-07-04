/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CEntityAddPacket.cpp
*  PURPOSE:     Entity-add packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*               Jax <>
*               Kevin Whiteside <>
*               Cecill Etheredge <>
*               Chris McArthur <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CEntityAddPacket::Add ( CElement * pElement )
{
    // Only add it if it has a parent.
    if ( pElement->GetParentEntity () )
    {
        // Jax: adding some checks here because map/element loading is all over the fucking place!
        switch ( pElement->GetType () )
        {
            case CElement::COLSHAPE:
            {
                CColShape * pColShape = static_cast < CColShape * > ( pElement );
                // If its a server side element only, dont send it
                if ( pColShape->IsPartnered () )
                {
                    return;
                }
                break;
            }
            default: break;
        }

        m_Entities.push_back ( pElement );
    }
}


bool CEntityAddPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    SPositionSync position ( false );

    // Check that we have any entities
    if ( m_Entities.size () > 0 )
    {
        // Write the number of entities
        unsigned int NumElements = m_Entities.size ();
        BitStream.WriteCompressed ( NumElements );

        // For each entity ...
        CVector vecTemp;
        vector < CElement* > ::const_iterator iter = m_Entities.begin ();
        for ( ; iter != m_Entities.end (); iter++ )
        {
            // Entity id
            CElement* pElement = *iter;
            BitStream.Write ( pElement->GetID () );

            // Entity type id
            unsigned char ucEntityTypeID = static_cast < unsigned char > ( pElement->GetType () );
            BitStream.Write ( ucEntityTypeID );

            // Entity parent
            CElement* pParent = pElement->GetParentEntity ();
            ElementID ParentID = INVALID_ELEMENT_ID;
            if ( pParent )
                ParentID = pParent->GetID ();
            BitStream.Write ( ParentID );

            // Entity interior
            BitStream.Write ( pElement->GetInterior () );

            // Entity dimension
            BitStream.WriteCompressed ( pElement->GetDimension () );

            // Entity attached to
            CElement* pElementAttachedTo = pElement->GetAttachedToElement ();
            if ( pElementAttachedTo )
            {
                BitStream.WriteBit ( true );
                BitStream.Write ( pElementAttachedTo->GetID () );

                // Attached position and rotation
                SPositionSync attachedPosition ( false );
                SRotationDegreesSync attachedRotation ( false );
                pElement->GetAttachedOffsets ( attachedPosition.data.vecPosition,
                                               attachedRotation.data.vecRotation );
                BitStream.Write ( &attachedPosition );
                BitStream.Write ( &attachedRotation );
            }
            else
                BitStream.WriteBit ( false );

            // Entity collisions enabled
            bool bCollisionsEnabled = true;

            switch ( pElement->GetType() )
            {
                case CElement::VEHICLE:
                {
                    CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
                    bCollisionsEnabled = pVehicle->GetCollisionEnabled ( );
                    break;
                }
                case CElement::OBJECT:
                {
                    CObject* pObject = static_cast < CObject* > ( pElement );
                    bCollisionsEnabled = pObject->GetCollisionEnabled ( );
                    break;
                }
                case CElement::PED:
                case CElement::PLAYER:
                {
                    CPed* pPed = static_cast < CPed* > ( pElement );
                    bCollisionsEnabled = pPed->GetCollisionEnabled ( );
                    break;
                }
            }

            BitStream.WriteBit ( bCollisionsEnabled );
            
            // Write custom data
            CCustomData* pCustomData = pElement->GetCustomDataPointer ();
            assert ( pCustomData );
            BitStream.WriteCompressed ( pCustomData->CountOnlySynchronized () );
            map < string, SCustomData > :: const_iterator iter = pCustomData->IterBegin ();
            for ( ; iter != pCustomData->IterEnd (); iter++ )
            {
                const char* szName = iter->first.c_str ();
                const CLuaArgument* pArgument = &iter->second.Variable;
                bool bSynchronized = iter->second.bSynchronized;

                if ( bSynchronized )
                {
                    unsigned char ucNameLength = static_cast < unsigned char > ( strlen ( szName ) );
                    BitStream.Write ( ucNameLength );
                    BitStream.Write ( szName, ucNameLength );
                    pArgument->WriteToBitStream ( BitStream );
                }
            }

            // Grab its name
            char szEmpty [1];
            szEmpty [0] = 0;
            const char* szName = pElement->GetName ().c_str ();
            if ( !szName )
                szName = szEmpty;

            // Write the name. It can be empty.
            unsigned short usNameLength = static_cast < unsigned short > ( strlen ( szName ) );
            BitStream.WriteCompressed ( usNameLength );
            if ( usNameLength > 0 )
            {
                BitStream.Write ( const_cast < char * > ( szName ), usNameLength );    
            }

            // Write the sync time context
            BitStream.Write ( pElement->GetSyncTimeContext () );

            // Write the rest depending on the type
            switch ( ucEntityTypeID )
            {
                case CElement::OBJECT:
                {
                    CObject* pObject = static_cast < CObject* > ( pElement );

                    // Position
                    position.data.vecPosition = pObject->GetPosition ();
                    BitStream.Write ( &position );

                    // Rotation
                    SRotationRadiansSync rotationRadians ( false );
                    pObject->GetRotation ( rotationRadians.data.vecRotation );
                    BitStream.Write ( &rotationRadians );

                    // Object id
                    BitStream.WriteCompressed ( pObject->GetModel () );

                    // Alpha
                    SEntityAlphaSync alpha;
                    alpha.data.ucAlpha = pObject->GetAlpha ();
                    BitStream.Write ( &alpha );

                    // Double sided
                    bool bIsDoubleSided = pObject->IsDoubleSided ();
                    BitStream.WriteBit ( bIsDoubleSided );

                    // Moving
                    const CPositionRotationAnimation* pMoveAnimation = pObject->GetMoveAnimation ();
                    if ( pMoveAnimation )
                    {
                         BitStream.WriteBit ( true );
                         pMoveAnimation->ToBitStream ( BitStream, true );
                    }
                    else
                    {
                        BitStream.WriteBit ( false );
                    }

                    // Scale
                    float fScale = pObject->GetScale ();
                    BitStream.Write ( fScale );

                    // Static
                    bool bStatic = pObject->IsStatic ();
                    BitStream.WriteBit ( bStatic );

                    // Health
                    SObjectHealthSync health;
                    health.data.fValue = pObject->GetHealth ();
                    BitStream.Write ( &health );

                    break;
                }

                case CElement::PICKUP:
                {
                    CPickup* pPickup = static_cast < CPickup* > ( pElement );

                    // Position
                    position.data.vecPosition = pPickup->GetPosition ();
                    BitStream.Write ( &position );

                    // Grab the model and write it
                    unsigned short usModel = pPickup->GetModel ();
                    BitStream.WriteCompressed ( usModel );

                    // Write if it's visible
                    bool bVisible = pPickup->IsVisible ();
                    BitStream.WriteBit ( bVisible );

                    // Write the type
                    SPickupTypeSync pickupType;
                    pickupType.data.ucType = pPickup->GetPickupType ();
                    BitStream.Write ( &pickupType );

                    switch ( pPickup->GetPickupType () )
                    {
                        case CPickup::ARMOR:
                        {
                            SPlayerArmorSync armor;
                            armor.data.fValue = pPickup->GetAmount ();
                            BitStream.Write ( &armor );
                            break;
                        }
                        case CPickup::HEALTH:
                        {
                            SPlayerHealthSync health;
                            health.data.fValue = pPickup->GetAmount ();
                            BitStream.Write ( &health );
                            break;
                        }
                        case CPickup::WEAPON:
                        {
                            SWeaponTypeSync weaponType;
                            weaponType.data.ucWeaponType = pPickup->GetWeaponType ();
                            BitStream.Write ( &weaponType );

                            SWeaponAmmoSync ammo ( weaponType.data.ucWeaponType, true, false );
                            ammo.data.usTotalAmmo = pPickup->GetAmmo ();
                            BitStream.Write ( &ammo );
                            break;
                        }
                        default: break;
                    }

                    break;
                }

                case CElement::VEHICLE:
                {
                    CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

                    // Write the vehicle position and rotation
                    position.data.vecPosition = pVehicle->GetPosition ();
                    SRotationDegreesSync rotationDegrees ( false );
                    pVehicle->GetRotationDegrees ( rotationDegrees.data.vecRotation );

                    // Write it
                    BitStream.Write ( &position );
                    BitStream.Write ( &rotationDegrees );

                    // Vehicle id as a char
                    // I'm assuming the "-400" is for adjustment so that all car values can
                    // fit into a char?  Why doesn't someone document this?
                    //
                    // --slush
                    BitStream.Write ( static_cast < unsigned char > ( pVehicle->GetModel () - 400 ) );

                    // Health
                    SVehicleHealthSync health;
                    health.data.fValue = pVehicle->GetHealth ();
                    BitStream.Write ( &health );

                    // Color
                    CVehicleColor& vehColor = pVehicle->GetColor ();
                    uchar ucNumColors = vehColor.GetNumColorsUsed () - 1;
                    BitStream.WriteBits ( &ucNumColors, 2 );
                    for ( uint i = 0 ; i <= ucNumColors ; i++ )
                    {
                        SColor RGBColor = vehColor.GetRGBColor ( i );
                        BitStream.Write ( RGBColor.R );
                        BitStream.Write ( RGBColor.G );
                        BitStream.Write ( RGBColor.B );
                    }

                    // Paintjob
                    SPaintjobSync paintjob;
                    paintjob.data.ucPaintjob = pVehicle->GetPaintjob ();
                    BitStream.Write ( &paintjob );

                    // Write the damage model
                    SVehicleDamageSync damage ( true, true, true, true, false );
                    memcpy ( damage.data.ucDoorStates,  pVehicle->m_ucDoorStates,  MAX_DOORS );
                    memcpy ( damage.data.ucWheelStates, pVehicle->m_ucWheelStates, MAX_WHEELS );
                    memcpy ( damage.data.ucPanelStates, pVehicle->m_ucPanelStates, MAX_PANELS );
                    memcpy ( damage.data.ucLightStates, pVehicle->m_ucLightStates, MAX_LIGHTS );
                    BitStream.Write ( &damage );
                    
                    unsigned char ucVariant = pVehicle->GetVariant();
                    BitStream.Write ( ucVariant );

                    unsigned char ucVariant2 = pVehicle->GetVariant2();
                    BitStream.Write ( ucVariant2 );

                    // If the vehicle has a turret, send its position too
                    unsigned short usModel = pVehicle->GetModel ();
                    if ( CVehicleManager::HasTurret ( usModel ) )
                    {
                        SVehicleTurretSync specific;
                        specific.data.fTurretX = pVehicle->GetTurretPositionX ();
                        specific.data.fTurretY = pVehicle->GetTurretPositionY ();
                        BitStream.Write ( &specific );
                    }

                    // If the vehicle has an adjustable property send its value
                    if ( CVehicleManager::HasAdjustableProperty ( usModel ) )
                    {
                        BitStream.WriteCompressed ( pVehicle->GetAdjustableProperty () );
                    }

                    // If the vehicle has doors, sync their open angle ratios.
                    if ( CVehicleManager::HasDoors ( usModel ) )
                    {
                        SDoorOpenRatioSync door;
                        for ( unsigned char i = 0; i < 6; ++i )
                        {
                            door.data.fRatio = pVehicle->GetDoorOpenRatio ( i );
                            BitStream.Write ( &door );
                        }
                    }

                    // Write all the upgrades
                    CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades ();
                    unsigned char ucNumUpgrades = pUpgrades->Count ();
                    unsigned short* usSlotStates = pUpgrades->GetSlotStates ();
                    BitStream.Write ( ucNumUpgrades );

                    if ( ucNumUpgrades > 0 )
                    {
                        unsigned char ucSlot = 0;
                        for ( ; ucSlot < VEHICLE_UPGRADE_SLOTS ; ucSlot++ )
                        {
                            unsigned short usUpgrade = usSlotStates [ ucSlot ];

                            /*
                            * This is another retarded modification in an attempt to save
                            * a byte.  We're apparently subtracting 1000 so we can store the
                            * information in a single byte instead of two.  This only gives us
                            * a maximum of 256 vehicle slots.
                            *
                            * --slush
                            * -- ChrML: Ehm, GTA only has 17 upgrade slots... This is a valid optimization.
                            */
                            if ( usUpgrade )
                                BitStream.Write ( static_cast < unsigned char > ( usSlotStates [ ucSlot ] - 1000 ) );
                        }
                    }

                    // Get the vehicle's reg plate as 8 bytes of chars with the not used bytes
                    // nulled.
                    const char* cszRegPlate = pVehicle->GetRegPlate ();
                    BitStream.Write ( cszRegPlate, 8 );

                    // Light override
                    SOverrideLightsSync overrideLights;
                    overrideLights.data.ucOverride = pVehicle->GetOverrideLights ();
                    BitStream.Write ( &overrideLights );

                    // Grab various vehicle flags
                    BitStream.WriteBit ( pVehicle->IsLandingGearDown () );
                    BitStream.WriteBit ( pVehicle->IsSirenActive () );
                    BitStream.WriteBit ( pVehicle->IsFuelTankExplodable () );
                    BitStream.WriteBit ( pVehicle->IsEngineOn () );
                    BitStream.WriteBit ( pVehicle->IsLocked () );
                    BitStream.WriteBit ( pVehicle->AreDoorsUndamageable () );
                    BitStream.WriteBit ( pVehicle->IsDamageProof () );
                    BitStream.WriteBit ( pVehicle->IsFrozen () );
                    BitStream.WriteBit ( pVehicle->IsDerailed () );
                    BitStream.WriteBit ( pVehicle->IsDerailable () );
                    BitStream.WriteBit ( pVehicle->GetTrainDirection () );
                    BitStream.WriteBit ( pVehicle->IsTaxiLightOn () );

                    // Write alpha
                    SEntityAlphaSync alpha;
                    alpha.data.ucAlpha = pVehicle->GetAlpha ();
                    BitStream.Write ( &alpha ); 

                    // Write headlight color
                    SColor color = pVehicle->GetHeadLightColor ();
                    if ( color.R != 255 || color.G != 255 || color.B != 255 )
                    {
                        BitStream.WriteBit ( true );
                        BitStream.Write ( color.R );
                        BitStream.Write ( color.G );
                        BitStream.Write ( color.B );
                    }
                    else
                        BitStream.WriteBit ( false );

                    // Write handling
                    if ( g_pGame->GetHandlingManager()->HasModelHandlingChanged ( static_cast < eVehicleTypes > ( pVehicle->GetModel() ) )
                        || pVehicle->HasHandlingChanged() )
                    {
                        BitStream.WriteBit ( true );
                        SVehicleHandlingSync handling;
                        CHandlingEntry* pEntry = pVehicle->GetHandlingData ();

                        handling.data.fMass                         = pEntry->GetMass ();
                        handling.data.fTurnMass                     = pEntry->GetTurnMass ();
                        handling.data.fDragCoeff                    = pEntry->GetDragCoeff ();
                        handling.data.vecCenterOfMass               = pEntry->GetCenterOfMass ();
                        handling.data.ucPercentSubmerged            = pEntry->GetPercentSubmerged ();
                        handling.data.fTractionMultiplier           = pEntry->GetTractionMultiplier ();
                        handling.data.ucDriveType                   = pEntry->GetCarDriveType ();
                        handling.data.ucEngineType                  = pEntry->GetCarEngineType ();
                        handling.data.ucNumberOfGears               = pEntry->GetNumberOfGears ();
                        handling.data.fEngineAcceleration           = pEntry->GetEngineAcceleration ();
                        handling.data.fEngineInertia                = pEntry->GetEngineInertia ();
                        handling.data.fMaxVelocity                  = pEntry->GetMaxVelocity ();
                        handling.data.fBrakeDeceleration            = pEntry->GetBrakeDeceleration ();
                        handling.data.fBrakeBias                    = pEntry->GetBrakeBias ();
                        handling.data.bABS                          = pEntry->GetABS ();
                        handling.data.fSteeringLock                 = pEntry->GetSteeringLock ();
                        handling.data.fTractionLoss                 = pEntry->GetTractionLoss ();
                        handling.data.fTractionBias                 = pEntry->GetTractionBias ();
                        handling.data.fSuspensionForceLevel         = pEntry->GetSuspensionForceLevel ();
                        handling.data.fSuspensionDamping            = pEntry->GetSuspensionDamping ();
                        handling.data.fSuspensionHighSpdDamping     = pEntry->GetSuspensionHighSpeedDamping ();
                        handling.data.fSuspensionUpperLimit         = pEntry->GetSuspensionUpperLimit ();
                        handling.data.fSuspensionLowerLimit         = pEntry->GetSuspensionLowerLimit ();
                        handling.data.fSuspensionFrontRearBias      = pEntry->GetSuspensionFrontRearBias ();
                        handling.data.fSuspensionAntiDiveMultiplier = pEntry->GetSuspensionAntiDiveMultiplier ();
                        handling.data.fCollisionDamageMultiplier    = pEntry->GetCollisionDamageMultiplier ();
                        handling.data.uiModelFlags                  = pEntry->GetModelFlags ();
                        handling.data.uiHandlingFlags               = pEntry->GetHandlingFlags ();
                        handling.data.fSeatOffsetDistance           = pEntry->GetSeatOffsetDistance ();
                        //handling.data.uiMonetary                    = pEntry->GetMonetary ();
                        //handling.data.ucHeadLight                   = pEntry->GetHeadLight ();
                        //handling.data.ucTailLight                   = pEntry->GetTailLight ();
                        handling.data.ucAnimGroup                   = pEntry->GetAnimGroup ();
                        BitStream.Write ( &handling );
                    }
                    else
                        BitStream.WriteBit ( false );

                    break;
                }                

                case CElement::MARKER:
                {
                    CMarker* pMarker = static_cast < CMarker* > ( pElement );

                    // Position
                    position.data.vecPosition = pMarker->GetPosition ();
                    BitStream.Write ( &position );

                    // Type
                    SMarkerTypeSync markerType;
                    markerType.data.ucType = pMarker->GetMarkerType ();
                    BitStream.Write ( &markerType );

                    // Size
                    float fSize = pMarker->GetSize ();
                    BitStream.Write ( fSize );

                    // Colour
                    SColorSync color;
                    color = pMarker->GetColor ();
                    BitStream.Write ( &color );

                    // Write the target position vector eventually
                    if ( markerType.data.ucType == CMarker::TYPE_CHECKPOINT ||
                         markerType.data.ucType == CMarker::TYPE_RING )
                    {
                        if ( pMarker->HasTarget () )
                        {
                            BitStream.WriteBit ( true );

                            position.data.vecPosition = pMarker->GetTarget ();
                            BitStream.Write ( &position );
                        }
                        else
                            BitStream.WriteBit ( false );
                    }

                    break;
                }

                case CElement::BLIP:
                {
                    CBlip* pBlip = static_cast < CBlip* > ( pElement );

                    // Grab the blip position
                    position.data.vecPosition = pBlip->GetPosition ();
                    BitStream.Write ( &position );

                    // Write the ordering id
                    BitStream.WriteCompressed ( pBlip->m_sOrdering );

                    // Write the visible distance
                    SIntegerSync < unsigned short, 14 > visibleDistance ( pBlip->m_usVisibleDistance );
                    BitStream.Write ( &visibleDistance );

                    // Write the icon
                    SIntegerSync < unsigned char, 6 > icon ( pBlip->m_ucIcon );
                    BitStream.Write ( &icon );
                    if ( pBlip->m_ucIcon == 0 )
                    {
                        // Write the size
                        SIntegerSync < unsigned char, 5 > size ( pBlip->m_ucSize );
                        BitStream.Write ( &size );

                        // Write the color
                        SColorSync color;
                        color = pBlip->GetColor ();
                        BitStream.Write ( &color );
                    }                    

                    break;
                }

                case CElement::RADAR_AREA:
                {
                    CRadarArea* pArea = static_cast < CRadarArea* > ( pElement );

                    // Write the position
                    SPosition2DSync position2D ( false );
                    position2D.data.vecPosition = pArea->GetPosition ();
                    BitStream.Write ( &position2D );

                    // Write the size
                    SPosition2DSync size2D ( false );
                    size2D.data.vecPosition = pArea->GetSize ();
                    BitStream.Write ( &size2D );

                    // And the color
                    SColor color = pArea->GetColor ();
                    BitStream.Write ( color.R );
                    BitStream.Write ( color.G );
                    BitStream.Write ( color.B );
                    BitStream.Write ( color.A );

                    // Write whether it is flashing
                    bool bIsFlashing = pArea->IsFlashing ();
                    BitStream.WriteBit ( bIsFlashing );

                    break;
                }

                case CElement::WORLD_MESH:
                {
                    /*
                    CWorldMesh* pMesh = static_cast < CWorldMesh* > ( pElement );

                    // Write the name
                    char* szName = pMesh->GetName ();
                    unsigned short usNameLength = static_cast < unsigned short > ( strlen ( szName ) );
                    BitStream.Write ( usNameLength );
                    BitStream.Write ( szName, static_cast < int > ( usNameLength ) );

                    // Write the position and rotation
                    CVector vecTemp = pMesh->GetPosition ();
                    BitStream.Write ( vecTemp.fX );
                    BitStream.Write ( vecTemp.fY );
                    BitStream.Write ( vecTemp.fZ );
                    
                    vecTemp = pMesh->GetRotation ();
                    BitStream.Write ( vecTemp.fX );
                    BitStream.Write ( vecTemp.fY );
                    BitStream.Write ( vecTemp.fZ );
                    */

                    break;
                }

                case CElement::TEAM:
                {
                    CTeam* pTeam = static_cast < CTeam* > ( pElement );

                    // Write the name
                    char* szTeamName = pTeam->GetTeamName ();
                    unsigned short usNameLength = static_cast < unsigned short > ( strlen ( szTeamName ) );
                    unsigned char ucRed, ucGreen, ucBlue;
                    pTeam->GetColor ( ucRed, ucGreen, ucBlue );
                    bool bFriendlyFire = pTeam->GetFriendlyFire ();
                    BitStream.WriteCompressed ( usNameLength );
                    BitStream.Write ( szTeamName, usNameLength );                    
                    BitStream.Write ( ucRed );
                    BitStream.Write ( ucGreen );
                    BitStream.Write ( ucBlue );
                    BitStream.WriteBit ( bFriendlyFire );

                    break;
                }

                case CElement::PED:
                {
                    CPed* pPed = static_cast < CPed* > ( pElement );

                    // position
                    position.data.vecPosition = pPed->GetPosition ();
                    BitStream.Write ( &position );

                    // model
                    unsigned short usModel = pPed->GetModel ();
                    BitStream.WriteCompressed ( usModel );

                    // rotation
                    SPedRotationSync pedRotation;
                    pedRotation.data.fRotation = pPed->GetRotation ();
                    BitStream.Write ( &pedRotation );

                    // health
                    SPlayerHealthSync health;
                    health.data.fValue = pPed->GetHealth ();
                    BitStream.Write ( &health );

                    // Armor
                    SPlayerArmorSync armor;
                    armor.data.fValue = pPed->GetArmor ();
                    BitStream.Write ( &armor );
                    
                    // vehicle
                    CVehicle * pVehicle = pPed->GetOccupiedVehicle ();
                    if ( pVehicle )
                    {
                        BitStream.WriteBit ( true );
                        BitStream.Write ( pVehicle->GetID () );

                        SOccupiedSeatSync seat;
                        seat.data.ucSeat = pPed->GetOccupiedVehicleSeat ();
                        BitStream.Write ( &seat );
                    }
                    else
                        BitStream.WriteBit ( false );

                    // flags
                    BitStream.WriteBit ( pPed->HasJetPack () );
                    BitStream.WriteBit ( pPed->IsSyncable () );
                    BitStream.WriteBit ( pPed->IsHeadless () );
                    BitStream.WriteBit ( pPed->IsFrozen () );

                    // alpha
                    SEntityAlphaSync alpha;
                    alpha.data.ucAlpha = pPed->GetAlpha ();
                    BitStream.Write ( &alpha );

                    // clothes
                    unsigned char ucNumClothes = 0;
                    CPlayerClothes* pClothes = pPed->GetClothes ( );
                    for ( unsigned char ucType = 0 ; ucType < PLAYER_CLOTHING_SLOTS ; ucType++ )
                    {
                        SPlayerClothing* pClothing = pClothes->GetClothing ( ucType );
                        if ( pClothing )
                        {
                            ucNumClothes++;
                        }
                    }
                    BitStream.Write ( ucNumClothes );
                    for ( unsigned char ucType = 0 ; ucType < PLAYER_CLOTHING_SLOTS ; ucType++ )
                    {
                        SPlayerClothing* pClothing = pClothes->GetClothing ( ucType );
                        if ( pClothing )
                        {
                            unsigned char ucTextureLength = strlen ( pClothing->szTexture );
                            unsigned char ucModelLength = strlen ( pClothing->szModel );

                            BitStream.Write ( ucTextureLength );
                            BitStream.Write ( pClothing->szTexture, ucTextureLength );
                            BitStream.Write ( ucModelLength );
                            BitStream.Write ( pClothing->szModel, ucModelLength );
                            BitStream.Write ( ucType );
                        }
                    }
                    break;
                }

                case CElement::DUMMY:
                {
                    CDummy* pDummy = static_cast < CDummy* > ( pElement );
                    
                    // Type Name
                    const char* szTypeName = pDummy->GetTypeName ().c_str ();
                    unsigned short usTypeNameLength = static_cast < unsigned short > ( strlen ( szTypeName ) );
                    BitStream.WriteCompressed ( usTypeNameLength );
                    BitStream.Write ( const_cast < char* > ( szTypeName ), usTypeNameLength );                      

                    // Position
                    position.data.vecPosition = pDummy->GetPosition();
                    if ( position.data.vecPosition != CVector ( 0.0f, 0.0f, 0.0f ) )
                    {
                        BitStream.WriteBit ( true );
                        BitStream.Write ( &position );
                    }
                    else
                        BitStream.WriteBit ( false );

                    break;
                }

                case CElement::PLAYER:
                {
                    break;
                }

                case CElement::SCRIPTFILE:
                {
                    // No extra data
                    break;
                }

                case CElement::COLSHAPE:
                {
                    CColShape* pColShape = static_cast < CColShape* > ( pElement );
                    if ( !pColShape->GetParentEntity () )
                    {
                        // Jax: i'm pretty sure this is fucking up our packet somehow..
                        // all valid col-shapes should have a parent!
                        assert ( false );
                    }

                    // Type
                    SColshapeTypeSync colType;
                    colType.data.ucType = static_cast < unsigned char > ( pColShape->GetShapeType () );
                    BitStream.Write ( &colType );

                    // Position
                    position.data.vecPosition = pColShape->GetPosition ();
                    BitStream.Write ( &position );
                    
                    // Enabled
                    BitStream.WriteBit ( pColShape->IsEnabled () );

                    // Auto Call Event
                    BitStream.WriteBit ( pColShape->GetAutoCallEvent () );

                    switch ( pColShape->GetShapeType () )
                    {
                        case COLSHAPE_CIRCLE:
                        {
                            BitStream.Write ( static_cast < CColCircle* > ( pColShape )->GetRadius () );
                            break;
                        }
                        case COLSHAPE_CUBOID:
                        {
                            SPositionSync size ( false );
                            size.data.vecPosition = static_cast < CColCuboid* > ( pColShape )->GetSize ();
                            BitStream.Write ( &size );
                            break;
                        }
                        case COLSHAPE_SPHERE:
                        {
                            BitStream.Write ( static_cast < CColSphere* > ( pColShape )->GetRadius () );
                            break;
                        }
                        case COLSHAPE_RECTANGLE:
                        {
                            SPosition2DSync size ( false );
                            size.data.vecPosition = static_cast < CColRectangle* > ( pColShape )->GetSize ();
                            BitStream.Write ( &size );
                            break;
                        }
                        case COLSHAPE_TUBE:
                        {
                            BitStream.Write ( static_cast < CColTube* > ( pColShape )->GetRadius () );
                            BitStream.Write ( static_cast < CColTube* > ( pColShape )->GetHeight () );
                            break;
                        }
                        case COLSHAPE_POLYGON:
                        {
                            CColPolygon* pPolygon = static_cast < CColPolygon* > ( pColShape );
                            BitStream.WriteCompressed ( pPolygon->CountPoints() );
                            std::vector < CVector2D > ::const_iterator iter = pPolygon->IterBegin();
                            for ( ; iter != pPolygon->IterEnd () ; iter++ )
                            {
                                SPosition2DSync vertex ( false );
                                vertex.data.vecPosition = *iter;
                                BitStream.Write ( &vertex );
                            }
                            break;
                        }
                        default: break;
                    }
                    break;
                }

                case CElement::WATER:
                {
                    CWater* pWater = static_cast < CWater* > ( pElement );
                    unsigned char ucNumVertices = (unsigned char)pWater->GetNumVertices ();
                    BitStream.Write ( ucNumVertices );
                    CVector vecVertex;
                    for ( int i = 0; i < ucNumVertices; i++ )
                    {
                        pWater->GetVertex ( i, vecVertex );
                        BitStream.Write ( (short)vecVertex.fX );
                        BitStream.Write ( (short)vecVertex.fY );
                        BitStream.Write ( vecVertex.fZ );
                    }
                    break;
                }

                default:
                {
                    assert ( 0 );
                    CLogger::LogPrintf ( "not sending this element - id: %i\n", pElement->GetType () );
                }
            }
        }

        // Success
        return true;
    }

    return false;
}

