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


bool CEntityAddPacket::Write ( NetServerBitStreamInterface& BitStream ) const
{    
    // Check that we have any entities
    if ( m_Entities.size () > 0 )
    {
        // Write the number of entities
        BitStream.Write ( ( ElementID ) m_Entities.size () );

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
            BitStream.Write ( pElement->GetDimension () );

            // Entity attached to
            CElement* pElementAttachedTo = pElement->GetAttachedToElement ();
            ElementID ElementAttachedToID = INVALID_ELEMENT_ID;
            if ( pElementAttachedTo )
                ElementAttachedToID = pElementAttachedTo->GetID ();
            BitStream.Write ( ElementAttachedToID );
            if ( ElementAttachedToID != INVALID_ELEMENT_ID )
            {
                // Attached position
                CVector vecAttachedPos, vecAttachedRot;
                pElement->GetAttachedOffsets ( vecAttachedPos, vecAttachedRot );
                BitStream.Write ( vecAttachedPos.fX );
                BitStream.Write ( vecAttachedPos.fY );
                BitStream.Write ( vecAttachedPos.fZ );

                // Attached rotation
                BitStream.Write ( vecAttachedRot.fX );
                BitStream.Write ( vecAttachedRot.fY );
                BitStream.Write ( vecAttachedRot.fZ );
            }
            
            // Write custom data
            CCustomData* pCustomData = pElement->GetCustomDataPointer ();
			assert ( pCustomData );
            BitStream.Write ( static_cast < unsigned short > ( pCustomData->Count () ) );
            map < string, SCustomData > :: const_iterator iter = pCustomData->IterBegin ();
            for ( ; iter != pCustomData->IterEnd (); iter++ )
            {
                const char* szName = iter->first.c_str ();
                const CLuaArgument* pArgument = &iter->second.Variable;

                unsigned char ucNameLength = static_cast < unsigned char > ( strlen ( szName ) );
                BitStream.Write ( ucNameLength );
                BitStream.Write ( szName, ucNameLength );
                pArgument->WriteToBitStream ( BitStream );
            }

            // Grab its name
            char szEmpty [1];
            szEmpty [0] = 0;
            const char* szName = pElement->GetName ().c_str ();
            if ( !szName )
                szName = szEmpty;

            // Write the name. It can be empty.
            unsigned short usNameLength = static_cast < unsigned short > ( strlen ( szName ) );
            BitStream.Write ( usNameLength );
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
                    vecTemp = pObject->GetPosition ();
                    BitStream.Write ( vecTemp.fX );
                    BitStream.Write ( vecTemp.fY );
                    BitStream.Write ( vecTemp.fZ );

                    // Rotation
                    vecTemp = pObject->GetRotation ();
                    BitStream.Write ( vecTemp.fX );
                    BitStream.Write ( vecTemp.fY );
                    BitStream.Write ( vecTemp.fZ );

                    // Object id
                    BitStream.Write ( pObject->GetModel () );

                    break;
                }

                case CElement::PICKUP:
                {
                    CPickup* pPickup = static_cast < CPickup* > ( pElement );

                    // Position
                    vecTemp = pPickup->GetPosition ();
                    BitStream.Write ( vecTemp.fX );
                    BitStream.Write ( vecTemp.fY );
                    BitStream.Write ( vecTemp.fZ );

                    // Grab the model and write it
                    unsigned short usModel = pPickup->GetModel ();
                    BitStream.Write ( usModel );

                    // Gather the flags
                    bool bVisible = ( bool ) ( pPickup->IsVisible () );

                    // Write the flags
                    unsigned char ucFlags;
                    ucFlags |= bVisible ? 1:0;
                    BitStream.Write ( ucFlags );

                    BitStream.Write ( pPickup->GetPickupType () );
                    switch ( pPickup->GetPickupType () )
                    {
                        case CPickup::ARMOR:
                        case CPickup::HEALTH:
                        {
                            BitStream.Write ( pPickup->GetAmount () );          // float
                            break;
                        }
                        case CPickup::WEAPON:
                        {
                            BitStream.Write ( pPickup->GetWeaponType () );      // unsigned char
                            BitStream.Write ( pPickup->GetAmmo () );            // unsigned short
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
                    const CVector& vecPosition = pVehicle->GetPosition ();
                    CVector vecRotationDegrees;
                    pVehicle->GetRotationDegrees ( vecRotationDegrees );

                    // Write it
                    BitStream.Write ( vecPosition.fX );
                    BitStream.Write ( vecPosition.fY );
                    BitStream.Write ( vecPosition.fZ );

                    BitStream.Write ( vecRotationDegrees.fX );
                    BitStream.Write ( vecRotationDegrees.fY );
                    BitStream.Write ( vecRotationDegrees.fZ );

                    // Vehicle id as a char
                    // I'm assuming the "-400" is for adjustment so that all car values can
                    // fit into a char?  Why doesn't someone document this?
                    //
                    // --slush
                    BitStream.Write ( static_cast < unsigned char > ( pVehicle->GetModel () - 400 ) );

                    // Health as float
                    BitStream.Write ( pVehicle->GetHealth () );

                    // Color as an unsigned long
                    BitStream.Write ( pVehicle->GetColor ().GetColor () );

                    // Paintjob
                    BitStream.Write ( pVehicle->GetPaintjob () );

                    // Write the damage model
                    BitStream.Write ( (char*) pVehicle->m_ucDoorStates, MAX_DOORS );
                    BitStream.Write ( (char*) pVehicle->m_ucWheelStates, MAX_WHEELS );
                    BitStream.Write ( (char*) pVehicle->m_ucPanelStates, MAX_PANELS );
                    BitStream.Write ( (char*) pVehicle->m_ucLightStates, MAX_LIGHTS );

                    // If the vehicle has a turret, send its position too
                    unsigned short usModel = pVehicle->GetModel ();
                    if ( CVehicleManager::HasTurret ( usModel ) )
                    {
                        BitStream.Write ( pVehicle->GetTurretPositionX () );
                        BitStream.Write ( pVehicle->GetTurretPositionY () );
                    }

                    // If the vehicle has an adjustable property send its value
                    if ( CVehicleManager::HasAdjustableProperty ( usModel ) )
                    {
                        BitStream.Write ( pVehicle->GetAdjustableProperty () );
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
                    BitStream.Write ( pVehicle->GetOverrideLights () );

                    // Grab various vehicle flags
                    bool bIsLandingGearDown = pVehicle->IsLandingGearDown ();
                    bool bSireneActive = pVehicle->IsSirenActive ();
                    bool bFuelTankExplodable = pVehicle->IsFuelTankExplodable ();
                    bool bIsEngineOn = pVehicle->IsEngineOn ();
                    bool bIsLocked = pVehicle->IsLocked ();
                    bool bAreDoorsUndamageable = pVehicle->AreDoorsUndamageable ();
                    bool bDamageProof = pVehicle->IsDamageProof ();
                    bool bFrozen = pVehicle->GetFrozen ();
                    bool bDerailed = pVehicle->IsDerailed ();
                    bool bIsDerailable = pVehicle->IsDerailable ();
                    bool bTrainDirection = pVehicle->GetTrainDirection ();

                    // Put them in a byte
                    unsigned short usFlags = 0;
                    if ( bIsLandingGearDown )   usFlags |= 0x0001;
                    if ( bSireneActive )        usFlags |= 0x0002;
                    if ( bFuelTankExplodable )  usFlags |= 0x0004;
                    if ( bIsEngineOn )          usFlags |= 0x0008;
                    if ( bIsLocked )            usFlags |= 0x0010;
                    if ( bAreDoorsUndamageable )usFlags |= 0x0020;
                    if ( bDamageProof )         usFlags |= 0x0040;
                    if ( bFrozen )              usFlags |= 0x0080;
                    if ( bDerailed )            usFlags |= 0x0100;
                    if ( bIsDerailable )        usFlags |= 0x0200;
                    if ( bTrainDirection )      usFlags |= 0x0400;

                    // Write the flags
                    BitStream.Write ( usFlags );

                    // Write alpha
                    BitStream.Write ( pVehicle->GetAlpha () );	

                    break;
                }                

                case CElement::MARKER:
                {
                    CMarker* pMarker = static_cast < CMarker* > ( pElement );

                    // Position
                    vecTemp = pMarker->GetPosition ();
                    BitStream.Write ( vecTemp.fX );
                    BitStream.Write ( vecTemp.fY );
                    BitStream.Write ( vecTemp.fZ );

                    // Type
                    unsigned char ucType = pMarker->GetMarkerType ();
                    BitStream.Write ( ucType );

                    // Size
                    float fSize = pMarker->GetSize ();
                    BitStream.Write ( fSize );

                    // Colour
                    unsigned char ucRed = pMarker->GetColorRed ();
                    unsigned char ucGreen = pMarker->GetColorGreen ();
                    unsigned char ucBlue = pMarker->GetColorBlue ();
                    unsigned char ucAlpha = pMarker->GetColorAlpha ();
                    BitStream.Write ( ucRed );
                    BitStream.Write ( ucGreen );
                    BitStream.Write ( ucBlue );
                    BitStream.Write ( ucAlpha );

                    // Write the target position vector eventually
                    if ( ucType == CMarker::TYPE_CHECKPOINT || ucType == CMarker::TYPE_RING )
                    {
                        if ( pMarker->HasTarget () )
                        {
                            // Flags: We have a target
                            BitStream.Write ( static_cast < unsigned char > ( 1 ) );

                            const CVector& vecTarget = pMarker->GetTarget ();
                            BitStream.Write ( vecTarget.fX );
                            BitStream.Write ( vecTarget.fY );
                            BitStream.Write ( vecTarget.fZ );
                        }
                        else
                        {
                            // Flags: We have no target
                            BitStream.Write ( static_cast < unsigned char > ( 0 ) );
                        }
                    }
                    else
                    {
                        // Flags: We have no target
                        BitStream.Write ( static_cast < unsigned char > ( 0 ) );
                    }

                    break;
                }

                case CElement::BLIP:
                {
                    CBlip* pBlip = static_cast < CBlip* > ( pElement );

                    // Grab the blip position
                    CVector vecPosition = pBlip->GetPosition ();
                    BitStream.Write ( vecPosition.fX );
                    BitStream.Write ( vecPosition.fY );
                    BitStream.Write ( vecPosition.fZ );

                    // Write the ordering id
                    BitStream.Write ( pBlip->m_sOrdering );

                    // Write the icon
                    unsigned char ucIcon = pBlip->m_ucIcon;
                    BitStream.Write ( ucIcon );
                    if ( ucIcon == 0 )
                    {
                        // Write the size and the color
                        BitStream.Write ( pBlip->m_ucSize );
                        BitStream.Write ( pBlip->GetColor () );
                    }                    

                    break;
                }

                case CElement::RADAR_AREA:
                {
                    CRadarArea* pArea = static_cast < CRadarArea* > ( pElement );

                    // Write the position
                    CVector vecTemp = pArea->GetPosition ();
                    BitStream.Write ( vecTemp.fX );
                    BitStream.Write ( vecTemp.fY );

                    // Write the size
                    CVector2D vecTemp2D = pArea->GetSize ();
                    BitStream.Write ( vecTemp2D.fX );
                    BitStream.Write ( vecTemp2D.fY );

                    // And the color
                    BitStream.Write ( pArea->GetColor () );

                    // Populate the flags
                    bool bIsFlashing = pArea->IsFlashing ();

                    // Make a flag byte
                    unsigned char ucFlags = 0;
                    ucFlags |= bIsFlashing ? 1:0;

                    // Write the flags
                    BitStream.Write ( ucFlags );

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
                    unsigned char ucFriendlyFire = ( pTeam->GetFriendlyFire () ) ? 1 : 0;
                    BitStream.Write ( usNameLength );
                    BitStream.Write ( szTeamName, usNameLength );                    
                    BitStream.Write ( ucRed );
                    BitStream.Write ( ucGreen );
                    BitStream.Write ( ucBlue );
                    BitStream.Write ( ucFriendlyFire );

                    break;
                }

                case CElement::PED:
                {
                    CPed* pPed = static_cast < CPed* > ( pElement );

                    // position
                    CVector vecPosition = pPed->GetPosition ();
                    BitStream.Write ( vecPosition.fX );
                    BitStream.Write ( vecPosition.fY );
                    BitStream.Write ( vecPosition.fZ );

                    // model
                    unsigned short usModel = pPed->GetModel ();
                    BitStream.Write ( usModel );

                    // rotation
                    BitStream.Write ( pPed->GetRotation () );

                    // health
                    float fHealth = pPed->GetHealth ();
                    BitStream.Write ( fHealth );

                    float fArmour = pPed->GetArmor ();
                    BitStream.Write ( fArmour );
                    
                    // vehicle
                    CVehicle * pVehicle = pPed->GetOccupiedVehicle ();
                    ElementID VehicleID = ( pVehicle ) ? pVehicle->GetID () : INVALID_ELEMENT_ID;
                    BitStream.Write ( VehicleID );
                    if ( pVehicle )
                    {
                        BitStream.Write ( ( unsigned char ) pPed->GetOccupiedVehicleSeat () );
                    }

                    // flags
                    unsigned char ucFlags = 0;
                    if ( pPed->HasJetPack () ) ucFlags |= 0x1;
                    if ( pPed->IsSyncable () ) ucFlags |= 0x2;
                    BitStream.Write ( ucFlags );

                    break;
                }

                case CElement::DUMMY:
                {
                    CDummy* pDummy = static_cast < CDummy* > ( pElement );
                    
                    // Type Name
                    const char* szTypeName = pDummy->GetTypeName ().c_str ();
                    unsigned short usTypeNameLength = static_cast < unsigned short > ( strlen ( szTypeName ) );
                    BitStream.Write ( usTypeNameLength );
                    BitStream.Write ( const_cast < char* > ( szTypeName ), usTypeNameLength );                      

                    // Position
                    CVector pVector = pDummy->GetPosition();
                    BitStream.Write ( pVector.fX );
                    BitStream.Write ( pVector.fY );
                    BitStream.Write ( pVector.fZ );

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
                    unsigned char ucShapeType = static_cast < unsigned char > ( pColShape->GetShapeType () );
                    BitStream.Write ( ucShapeType );

                    // Position
                    CVector vecPosition;
                    vecPosition = pColShape->GetPosition ();
                    BitStream.Write ( vecPosition.fX );
                    BitStream.Write ( vecPosition.fY );
                    BitStream.Write ( vecPosition.fZ );
                    
                    // Enabled
                    BitStream.Write ( ( unsigned char ) ( ( pColShape->IsEnabled () ) ? 1 : 0 ) );

                    // Auto Call Event
                    BitStream.Write ( ( unsigned char ) ( ( pColShape->GetAutoCallEvent () ) ? 1 : 0 ) );          

                    switch ( pColShape->GetShapeType () )
                    {
                        case COLSHAPE_CIRCLE:
                        {
                            BitStream.Write ( static_cast < CColCircle* > ( pColShape )->GetRadius () );
                            break;
                        }
                        case COLSHAPE_CUBOID:
                        {
                            CVector vecSize = static_cast < CColCuboid* > ( pColShape )->GetSize ();
                            BitStream.Write ( vecSize.fX );
                            BitStream.Write ( vecSize.fY );
                            BitStream.Write ( vecSize.fZ );
                            break;
                        }
                        case COLSHAPE_SPHERE:
                        {
                            BitStream.Write ( static_cast < CColSphere* > ( pColShape )->GetRadius () );
                            break;
                        }
                        case COLSHAPE_RECTANGLE:
                        {
                            CVector2D vecSize = static_cast < CColRectangle* > ( pColShape )->GetSize ();
                            BitStream.Write ( vecSize.fX );
                            BitStream.Write ( vecSize.fY );
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
                            BitStream.Write ( pPolygon->CountPoints() );
                            std::vector < CVector2D > ::const_iterator iter = pPolygon->IterBegin();
                            for ( ; iter != pPolygon->IterEnd () ; iter++ )
                            {
                                BitStream.Write ( (*iter).fX );
                                BitStream.Write ( (*iter).fY );
                            }
                            break;
                        }
                        default: break;
                    }
                    break;
                }

                case CElement::HANDLING:
                {
                    // Cast to handling class
                    CHandling* pHandling = static_cast < CHandling* > ( pElement );

                    bool bMass, bTurnMass, bDragCoeff, bCenterOfMass, bPercentSubmerged;
                    bool bTractionMultiplier, bDriveType, bEngineType, bNumberOfGears;
                    bool bEngineAccelleration, bEngineInertia, bMaxVelocity, bBrakeDecelleration;
                    bool bBrakeBias, bABS, bSteeringLock, bTractionLoss, bTractionBias;
                    bool bSuspensionForceLevel, bSuspensionDamping, bSuspensionHighSpeedDamping;
                    bool bSuspensionUpperLimit, bSuspensionLowerLimit, bSuspensionFrontRearBias;
                    bool bSuspensionAntidiveMultiplier, bCollisionDamageMultiplier;
                    bool bSeatOffsetDistance, bHandlingFlags, bModelFlags, bHeadLight;
                    bool bTailLight, bAnimGroup;

                    // Write all the details about the handling
                    float fMass = pHandling->GetMass ( bMass );
                    float fTurnMass = pHandling->GetTurnMass ( bTurnMass );
                    float fDragCoeff = pHandling->GetDragCoeff ( bDragCoeff );
                    const CVector& vecCenterOfMass = pHandling->GetCenterOfMass ( bCenterOfMass );
                    unsigned int uiPercentSubmerged = pHandling->GetPercentSubmerged ( bPercentSubmerged );
                    float fTractionMultiplier = pHandling->GetTractionMultiplier ( bTractionMultiplier );
                    unsigned char ucDriveType = pHandling->GetDriveType ( bDriveType );
                    unsigned char ucEngineType = pHandling->GetEngineType ( bEngineType );
                    unsigned char ucNumberOfGears = pHandling->GetNumberOfGears ( bNumberOfGears );
                    float fEngineAccelleration = pHandling->GetEngineAccelleration ( bEngineAccelleration );
                    float fEngineInertia = pHandling->GetEngineInertia ( bEngineInertia );
                    float fMaxVelocity = pHandling->GetMaxVelocity ( bMaxVelocity );
                    float fBrakeDecelleration = pHandling->GetBrakeDecelleration ( bBrakeDecelleration );
                    float fBrakeBias = pHandling->GetBrakeBias ( bBrakeBias );
                    unsigned char ucABS = pHandling->GetABS ( bABS ) ? 1 : 0;
                    float fSteeringLock = pHandling->GetSteeringLock ( bSteeringLock );
                    float fTractionLoss = pHandling->GetTractionLoss ( bTractionLoss );
                    float fTractionBias = pHandling->GetTractionBias ( bTractionBias );
                    float fSuspensionForceLevel = pHandling->GetSuspensionForceLevel ( bSuspensionForceLevel );
                    float fSuspensionDamping = pHandling->GetSuspensionDamping ( bSuspensionDamping );
                    float fSuspensionHighSpeedDamping = pHandling->GetSuspensionHighSpeedDamping ( bSuspensionHighSpeedDamping );
                    float fSuspensionUpperLimit = pHandling->GetSuspensionUpperLimit ( bSuspensionUpperLimit );
                    float fSuspensionLowerLimit = pHandling->GetSuspensionLowerLimit ( bSuspensionLowerLimit );
                    float fSuspensionFrontRearBias = pHandling->GetSuspensionFrontRearBias ( bSuspensionFrontRearBias );
                    float fSuspensionAntidiveMultiplier = pHandling->GetSuspensionAntidiveMultiplier ( bSuspensionAntidiveMultiplier );
                    float fCollisionDamageMultiplier = pHandling->GetCollisionDamageMultiplier ( bCollisionDamageMultiplier );
                    float fSeatOffsetDistance = pHandling->GetSeatOffsetDistance ( bSeatOffsetDistance );
                    unsigned int uiHandlingFlags = pHandling->GetHandlingFlags ( bHandlingFlags );
                    unsigned int uiModelFlags = pHandling->GetModelFlags ( bModelFlags );
                    unsigned char ucHeadLight = pHandling->GetHeadLight ( bHeadLight );
                    unsigned char ucTailLight = pHandling->GetTailLight ( bTailLight );
                    unsigned char ucAnimGroup = pHandling->GetAnimGroup ( bAnimGroup );

                    // Put all the bools in a long so the client knows what it's going to
                    // receive
                    unsigned long ulChanged =   static_cast < unsigned long > ( bMass ) |
                                                static_cast < unsigned long > ( bTurnMass ) << 1 |
                                                static_cast < unsigned long > ( bDragCoeff ) << 2 |
                                                static_cast < unsigned long > ( bCenterOfMass ) << 3 |
                                                static_cast < unsigned long > ( bPercentSubmerged ) << 4 |
                                                static_cast < unsigned long > ( bTractionMultiplier ) << 5 |
                                                static_cast < unsigned long > ( bDriveType ) << 6 |
                                                static_cast < unsigned long > ( bEngineType ) << 7 |
                                                static_cast < unsigned long > ( bNumberOfGears ) << 8 |
                                                static_cast < unsigned long > ( bEngineAccelleration ) << 9 |
                                                static_cast < unsigned long > ( bEngineInertia ) << 10 |
                                                static_cast < unsigned long > ( bMaxVelocity ) << 11 |
                                                static_cast < unsigned long > ( bBrakeDecelleration ) << 12 |
                                                static_cast < unsigned long > ( bBrakeBias ) << 13 |
                                                static_cast < unsigned long > ( bABS ) << 14 |
                                                static_cast < unsigned long > ( bSteeringLock ) << 15 |
                                                static_cast < unsigned long > ( bTractionLoss ) << 16 |
                                                static_cast < unsigned long > ( bTractionBias ) << 17 |
                                                static_cast < unsigned long > ( bSuspensionForceLevel ) << 18 |
                                                static_cast < unsigned long > ( bSuspensionDamping ) << 19 |
                                                static_cast < unsigned long > ( bSuspensionHighSpeedDamping ) << 20 |
                                                static_cast < unsigned long > ( bSuspensionUpperLimit ) << 21 |
                                                static_cast < unsigned long > ( bSuspensionLowerLimit ) << 22 |
                                                static_cast < unsigned long > ( bSuspensionFrontRearBias ) << 23 |
                                                static_cast < unsigned long > ( bSuspensionAntidiveMultiplier ) << 24 |
                                                static_cast < unsigned long > ( bCollisionDamageMultiplier ) << 25 |
                                                static_cast < unsigned long > ( bSeatOffsetDistance ) << 26 |
                                                static_cast < unsigned long > ( bHandlingFlags ) << 27 |
                                                static_cast < unsigned long > ( bModelFlags ) << 28 |
                                                static_cast < unsigned long > ( bHeadLight ) << 29 |
                                                static_cast < unsigned long > ( bTailLight ) << 30 |
                                                static_cast < unsigned long > ( bAnimGroup ) << 31;
                    BitStream.Write ( ulChanged );

                    if ( bMass )
                        BitStream.Write ( fMass );

                    if ( bTurnMass )
                        BitStream.Write ( fTurnMass );

                    if ( bDragCoeff )
                        BitStream.Write ( fDragCoeff );

                    if ( bCenterOfMass )
                    {
                        BitStream.Write ( vecCenterOfMass.fX );
                        BitStream.Write ( vecCenterOfMass.fY );
                        BitStream.Write ( vecCenterOfMass.fZ );
                    }

                    if ( bPercentSubmerged )
                        BitStream.Write ( uiPercentSubmerged );

                    if ( bTractionMultiplier )
                        BitStream.Write ( fTractionMultiplier );

                    if ( bDriveType )
                        BitStream.Write ( ucDriveType );

                    if ( bEngineType )
                        BitStream.Write ( ucEngineType );

                    if ( bNumberOfGears )
                        BitStream.Write ( ucNumberOfGears );

                    if ( bEngineAccelleration )
                        BitStream.Write ( fEngineAccelleration );

                    if ( bEngineInertia )
                        BitStream.Write ( fEngineInertia );

                    if ( bMaxVelocity )
                        BitStream.Write ( fMaxVelocity );

                    if ( bBrakeDecelleration )
                        BitStream.Write ( fBrakeDecelleration );

                    if ( bBrakeBias )
                        BitStream.Write ( fBrakeBias );

                    if ( bABS )
                        BitStream.Write ( ucABS );

                    if ( bSteeringLock )
                        BitStream.Write ( fSteeringLock );

                    if ( bTractionLoss )
                        BitStream.Write ( fTractionLoss );

                    if ( bTractionBias )
                        BitStream.Write ( fTractionBias );

                    if ( bSuspensionForceLevel )
                        BitStream.Write ( fSuspensionForceLevel );

                    if ( bSuspensionDamping )
                        BitStream.Write ( fSuspensionDamping );

                    if ( bSuspensionHighSpeedDamping )
                        BitStream.Write ( fSuspensionHighSpeedDamping );

                    if ( bSuspensionUpperLimit )
                        BitStream.Write ( fSuspensionUpperLimit );
                    
                    if ( bSuspensionLowerLimit )
                        BitStream.Write ( fSuspensionLowerLimit );

                    if ( bSuspensionFrontRearBias )
                        BitStream.Write ( fSuspensionFrontRearBias );

                    if ( bSuspensionAntidiveMultiplier )
                        BitStream.Write ( fSuspensionAntidiveMultiplier );

                    if ( bCollisionDamageMultiplier )
                        BitStream.Write ( fCollisionDamageMultiplier );

                    if ( bSeatOffsetDistance )
                        BitStream.Write ( fSeatOffsetDistance );

                    if ( bHandlingFlags )
                        BitStream.Write ( uiHandlingFlags );
                    
                    if ( bModelFlags )
                        BitStream.Write ( uiModelFlags );

                    if ( bHeadLight )
                        BitStream.Write ( ucHeadLight );

                    if ( bTailLight )
                        BitStream.Write ( ucTailLight );

                    if ( bAnimGroup )
                        BitStream.Write ( ucAnimGroup );

                    // Write number of vehicles that have this handling applied as default
                    unsigned char ucCount = static_cast < unsigned char > ( pHandling->CountDefaultTo () );
                    BitStream.Write ( ucCount );

                    // Write their ID's as bytes (real id - 400)
                    std::list < unsigned short > ::const_iterator iter = pHandling->IterDefaultToBegin ();
                    for ( ; iter != pHandling->IterDefaultToEnd (); iter++ )
                    {
                        // Grab the ID
                        unsigned char ucID = (*iter) - 400;
                        BitStream.Write ( ucID );
                    }

                    // Done
                    break;
                }

                case CElement::WATER:
                {
                    CWater* pWater = static_cast < CWater* > ( pElement );
                    BYTE ucNumVertices = pWater->GetNumVertices ();
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
