/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CEntityAddPacket.cpp
 *  PURPOSE:     Entity-add packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CEntityAddPacket.h"
#include "CColShape.h"
#include "CColCuboid.h"
#include "CColCircle.h"
#include "CColPolygon.h"
#include "CColRectangle.h"
#include "CColTube.h"
#include "CDummy.h"
#include "CPickup.h"
#include "CMarker.h"
#include "CBlip.h"
#include "CRadarArea.h"
#include "CWater.h"
#include "CVehicleManager.h"
#include "CHandlingManager.h"
#include "CGame.h"

//
// Temporary helper functions for fixing crashes on pre r6459 clients.
// Cause of #IND numbers should be handled before it gets here. (To avoid desync)
//
bool IsIndeterminate(float fValue)
{
    return fValue - fValue != 0;
}

void SilentlyFixIndeterminate(float& fValue)
{
    if (IsIndeterminate(fValue))
        fValue = 0;
}

void SilentlyFixIndeterminate(CVector& vecValue)
{
    SilentlyFixIndeterminate(vecValue.fX);
    SilentlyFixIndeterminate(vecValue.fY);
    SilentlyFixIndeterminate(vecValue.fZ);
}

void SilentlyFixIndeterminate(CVector2D& vecValue)
{
    SilentlyFixIndeterminate(vecValue.fX);
    SilentlyFixIndeterminate(vecValue.fY);
}

void CEntityAddPacket::Add(CElement* pElement)
{
    // Only add it if it has a parent.
    if (pElement->GetParentEntity())
    {
        // Jax: adding some checks here because map/element loading is all over the fucking place!
        switch (pElement->GetType())
        {
            case CElement::COLSHAPE:
            {
                CColShape* pColShape = static_cast<CColShape*>(pElement);
                // If its a server side element only, dont send it
                if (pColShape->IsPartnered())
                {
                    return;
                }
                break;
            }
            default:
                break;
        }

        m_Entities.push_back(pElement);
    }
}

bool CEntityAddPacket::Write(NetBitStreamInterface& BitStream) const
{
    SPositionSync position(false);

    // Check that we have any entities
    if (m_Entities.size() > 0)
    {
        // Write the number of entities
        unsigned int NumElements = m_Entities.size();
        BitStream.WriteCompressed(NumElements);

        // For each entity ...
        CVector                           vecTemp;
        vector<CElement*>::const_iterator iter = m_Entities.begin();
        for (; iter != m_Entities.end(); ++iter)
        {
            // Entity id
            CElement* pElement = *iter;
            BitStream.Write(pElement->GetID());

            // Entity type id
            unsigned char ucEntityTypeID = static_cast<unsigned char>(pElement->GetType());
            BitStream.Write(ucEntityTypeID);

            // Entity parent
            CElement* pParent = pElement->GetParentEntity();
            ElementID ParentID = INVALID_ELEMENT_ID;
            if (pParent)
                ParentID = pParent->GetID();
            BitStream.Write(ParentID);

            // Entity interior
            BitStream.Write(pElement->GetInterior());

            // Entity dimension
            BitStream.WriteCompressed(pElement->GetDimension());

            // Entity attached to
            CElement* pElementAttachedTo = pElement->GetAttachedToElement();
            if (pElementAttachedTo)
            {
                BitStream.WriteBit(true);
                BitStream.Write(pElementAttachedTo->GetID());

                // Attached position and rotation
                SPositionSync        attachedPosition(false);
                SRotationDegreesSync attachedRotation(false);
                pElement->GetAttachedOffsets(attachedPosition.data.vecPosition, attachedRotation.data.vecRotation);
                BitStream.Write(&attachedPosition);
                BitStream.Write(&attachedRotation);
            }
            else
                BitStream.WriteBit(false);

            // Entity collisions enabled
            bool bCollisionsEnabled = true;

            switch (pElement->GetType())
            {
                case CElement::VEHICLE:
                {
                    CVehicle* pVehicle = static_cast<CVehicle*>(pElement);
                    bCollisionsEnabled = pVehicle->GetCollisionEnabled();
                    break;
                }
                case CElement::OBJECT:
                case CElement::WEAPON:
                {
                    CObject* pObject = static_cast<CObject*>(pElement);
                    bCollisionsEnabled = pObject->GetCollisionEnabled();
                    break;
                }
                case CElement::PED:
                case CElement::PLAYER:
                {
                    CPed* pPed = static_cast<CPed*>(pElement);
                    bCollisionsEnabled = pPed->GetCollisionEnabled();
                    break;
                }
                default:
                    break;
            }

            BitStream.WriteBit(bCollisionsEnabled);

            if (BitStream.Version() >= 0x56)
                BitStream.WriteBit(pElement->IsCallPropagationEnabled());

            // Write custom data
            CCustomData& pCustomData = pElement->GetCustomDataManager();
            BitStream.WriteCompressed(pCustomData.CountOnlySynchronized());
            map<string, SCustomData>::const_iterator iter = pCustomData.SyncedIterBegin();
            for (; iter != pCustomData.SyncedIterEnd(); ++iter)
            {
                const char*         szName = iter->first.c_str();
                const CLuaArgument* pArgument = &iter->second.Variable;

                unsigned char ucNameLength = static_cast<unsigned char>(strlen(szName));
                BitStream.Write(ucNameLength);
                BitStream.Write(szName, ucNameLength);
                pArgument->WriteToBitStream(BitStream);
            }

            // Grab its name
            char szEmpty[1];
            szEmpty[0] = 0;
            const char* szName = pElement->GetName().c_str();
            if (!szName)
                szName = szEmpty;

            // Write the name. It can be empty.
            unsigned short usNameLength = static_cast<unsigned short>(strlen(szName));
            BitStream.WriteCompressed(usNameLength);
            if (usNameLength > 0)
            {
                BitStream.Write(szName, usNameLength);
            }

            // Write the sync time context
            BitStream.Write(pElement->GetSyncTimeContext());

            // Write the rest depending on the type
            switch (ucEntityTypeID)
            {
                case CElement::OBJECT:
                case CElement::WEAPON:
                {
                    CObject* pObject = static_cast<CObject*>(pElement);

                    // Position
                    position.data.vecPosition = pObject->GetPosition();
                    SilentlyFixIndeterminate(position.data.vecPosition);            // Crash fix for pre r6459 clients
                    BitStream.Write(&position);

                    // Rotation
                    SRotationRadiansSync rotationRadians(false);
                    pObject->GetRotation(rotationRadians.data.vecRotation);
                    BitStream.Write(&rotationRadians);

                    // Object id
                    BitStream.WriteCompressed(pObject->GetModel());

                    // Alpha
                    SEntityAlphaSync alpha;
                    alpha.data.ucAlpha = pObject->GetAlpha();
                    BitStream.Write(&alpha);

                    // Low LOD stuff
                    bool bIsLowLod = pObject->IsLowLod();
                    BitStream.WriteBit(bIsLowLod);

                    CObject*  pLowLodObject = pObject->GetLowLodObject();
                    ElementID LowLodObjectID = pLowLodObject ? pLowLodObject->GetID() : INVALID_ELEMENT_ID;
                    BitStream.Write(LowLodObjectID);

                    // Double sided
                    bool bIsDoubleSided = pObject->IsDoubleSided();
                    BitStream.WriteBit(bIsDoubleSided);

                    // Breakable
                    if (BitStream.Can(eBitStreamVersion::CEntityAddPacket_ObjectBreakable))
                        BitStream.WriteBit(pObject->IsBreakable());

                    // Visible in all dimensions
                    if (BitStream.Can(eBitStreamVersion::DimensionOmnipresence))
                    {
                        bool bIsVisibleInAllDimensions = pObject->IsVisibleInAllDimensions();
                        BitStream.WriteBit(bIsVisibleInAllDimensions);
                    }

                    // Moving
                    const CPositionRotationAnimation* pMoveAnimation = pObject->GetMoveAnimation();
                    if (pMoveAnimation)
                    {
                        BitStream.WriteBit(true);
                        pMoveAnimation->ToBitStream(BitStream, true);
                    }
                    else
                    {
                        BitStream.WriteBit(false);
                    }

                    // Scale
                    const CVector& vecScale = pObject->GetScale();
                    if (BitStream.Version() >= 0x41)
                    {
                        bool bIsUniform = (vecScale.fX == vecScale.fY && vecScale.fX == vecScale.fZ);
                        BitStream.WriteBit(bIsUniform);
                        if (bIsUniform)
                        {
                            bool bIsUnitSize = (vecScale.fX == 1.0f);
                            BitStream.WriteBit(bIsUnitSize);
                            if (!bIsUnitSize)
                                BitStream.Write(vecScale.fX);
                        }
                        else
                        {
                            BitStream.Write(vecScale.fX);
                            BitStream.Write(vecScale.fY);
                            BitStream.Write(vecScale.fZ);
                        }
                    }
                    else
                    {
                        BitStream.Write(vecScale.fX);
                    }

                    // Frozen
                    bool bFrozen = pObject->IsFrozen();
                    BitStream.WriteBit(bFrozen);

                    // Health
                    SObjectHealthSync health;
                    health.data.fValue = pObject->GetHealth();
                    BitStream.Write(&health);

                    // is object break?
                    if (BitStream.Can(eBitStreamVersion::BreakObject_Serverside))
                        BitStream.WriteBit(pObject->GetHealth() <= 0);

                    // Respawnable
                    if (BitStream.Can(eBitStreamVersion::RespawnObject_Serverside))
                        BitStream.WriteBit(pObject->IsRespawnEnabled());

                    if (ucEntityTypeID == CElement::WEAPON)
                    {
                        CCustomWeapon* pWeapon = static_cast<CCustomWeapon*>(pElement);
                        unsigned char  targetType = pWeapon->GetTargetType();
                        BitStream.WriteBits(&targetType, 3);            // 3 bits = 4 possible values.

                        switch (targetType)
                        {
                            case TARGET_TYPE_FIXED:
                            {
                                break;
                            }
                            case TARGET_TYPE_ENTITY:
                            {
                                CElement* pTarget = pWeapon->GetElementTarget();
                                ElementID targetID = pTarget->GetID();

                                BitStream.Write(targetID);
                                if (IS_PED(pTarget))
                                {
                                    // Send full unsigned char... bone documentation looks scarce.
                                    unsigned char ucSubTarget = pWeapon->GetTargetBone();
                                    BitStream.Write(ucSubTarget);            // Send the entire unsigned char as there are a lot of bones.
                                }
                                else if (IS_VEHICLE(pTarget))
                                {
                                    unsigned char ucSubTarget = pWeapon->GetTargetWheel();
                                    BitStream.WriteBits(&ucSubTarget, 4);            // 4 bits = 8 possible values.
                                }
                                break;
                            }
                            case TARGET_TYPE_VECTOR:
                            {
                                CVector vecTarget = pWeapon->GetVectorTarget();
                                BitStream.WriteVector(vecTarget.fX, vecTarget.fY, vecTarget.fZ);
                                break;
                            }
                        }
                        bool bChanged = false;
                        BitStream.WriteBit(bChanged);
                        if (bChanged)
                        {
                            CWeaponStat*   pWeaponStat = pWeapon->GetWeaponStat();
                            unsigned short usDamage = pWeaponStat->GetDamagePerHit();
                            float          fAccuracy = pWeaponStat->GetAccuracy();
                            float          fTargetRange = pWeaponStat->GetTargetRange();
                            float          fWeaponRange = pWeaponStat->GetWeaponRange();
                            BitStream.WriteBits(&usDamage, 12);            // 12 bits = 2048 values... plenty.
                            BitStream.Write(fAccuracy);
                            BitStream.Write(fTargetRange);
                            BitStream.Write(fWeaponRange);
                        }
                        SWeaponConfiguration weaponConfig = pWeapon->GetFlags();

                        BitStream.WriteBit(weaponConfig.bDisableWeaponModel);
                        BitStream.WriteBit(weaponConfig.bInstantReload);
                        BitStream.WriteBit(weaponConfig.bShootIfTargetBlocked);
                        BitStream.WriteBit(weaponConfig.bShootIfTargetOutOfRange);
                        BitStream.WriteBit(weaponConfig.flags.bCheckBuildings);
                        BitStream.WriteBit(weaponConfig.flags.bCheckCarTires);
                        BitStream.WriteBit(weaponConfig.flags.bCheckDummies);
                        BitStream.WriteBit(weaponConfig.flags.bCheckObjects);
                        BitStream.WriteBit(weaponConfig.flags.bCheckPeds);
                        BitStream.WriteBit(weaponConfig.flags.bCheckVehicles);
                        BitStream.WriteBit(weaponConfig.flags.bIgnoreSomeObjectsForCamera);
                        BitStream.WriteBit(weaponConfig.flags.bSeeThroughStuff);
                        BitStream.WriteBit(weaponConfig.flags.bShootThroughStuff);

                        unsigned short usAmmo = pWeapon->GetAmmo();
                        unsigned short usClipAmmo = pWeapon->GetAmmo();
                        ElementID      OwnerID = pWeapon->GetOwner() == NULL ? INVALID_ELEMENT_ID : pWeapon->GetOwner()->GetID();
                        unsigned char  ucWeaponState = pWeapon->GetWeaponState();
                        BitStream.WriteBits(&ucWeaponState, 4);            // 4 bits = 8 possible values for weapon state
                        BitStream.Write(usAmmo);
                        BitStream.Write(usClipAmmo);
                        BitStream.Write(OwnerID);
                    }

                    break;
                }

                case CElement::PICKUP:
                {
                    CPickup* pPickup = static_cast<CPickup*>(pElement);

                    // Position
                    position.data.vecPosition = pPickup->GetPosition();
                    SilentlyFixIndeterminate(position.data.vecPosition);            // Crash fix for pre r6459 clients
                    BitStream.Write(&position);

                    // Grab the model and write it
                    unsigned short usModel = pPickup->GetModel();
                    BitStream.WriteCompressed(usModel);

                    // Write if it's visible
                    bool bVisible = pPickup->IsVisible();
                    BitStream.WriteBit(bVisible);

                    // Write the type
                    SPickupTypeSync pickupType;
                    pickupType.data.ucType = pPickup->GetPickupType();
                    BitStream.Write(&pickupType);

                    switch (pPickup->GetPickupType())
                    {
                        case CPickup::ARMOR:
                        {
                            SPlayerArmorSync armor;
                            armor.data.fValue = pPickup->GetAmount();
                            BitStream.Write(&armor);
                            break;
                        }
                        case CPickup::HEALTH:
                        {
                            SPlayerHealthSync health;
                            health.data.fValue = pPickup->GetAmount();
                            BitStream.Write(&health);
                            break;
                        }
                        case CPickup::WEAPON:
                        {
                            SWeaponTypeSync weaponType;
                            weaponType.data.ucWeaponType = pPickup->GetWeaponType();
                            BitStream.Write(&weaponType);

                            SWeaponAmmoSync ammo(weaponType.data.ucWeaponType, true, false);
                            ammo.data.usTotalAmmo = pPickup->GetAmmo();
                            BitStream.Write(&ammo);
                            break;
                        }
                        default:
                            break;
                    }

                    break;
                }

                case CElement::VEHICLE:
                {
                    CVehicle* pVehicle = static_cast<CVehicle*>(pElement);

                    // Write the vehicle position and rotation
                    position.data.vecPosition = pVehicle->GetPosition();
                    SRotationDegreesSync rotationDegrees(false);
                    pVehicle->GetRotationDegrees(rotationDegrees.data.vecRotation);

                    // Write it
                    BitStream.Write(&position);
                    BitStream.Write(&rotationDegrees);

                    // Vehicle id as a char
                    // I'm assuming the "-400" is for adjustment so that all car values can
                    // fit into a char?  Why doesn't someone document this?
                    //
                    // --slush
                    BitStream.Write(static_cast<unsigned char>(pVehicle->GetModel() - 400));

                    // Health
                    SVehicleHealthSync health;
                    health.data.fValue = pVehicle->GetHealth();
                    BitStream.Write(&health);

                    // Blow state
                    if (BitStream.Can(eBitStreamVersion::VehicleBlowStateSupport))
                    {
                        unsigned char blowState = 0;

                        switch (pVehicle->GetBlowState())
                        {
                            case VehicleBlowState::AWAITING_EXPLOSION_SYNC:
                                blowState = 1;
                                break;
                            case VehicleBlowState::BLOWN:
                                blowState = 2;
                                break;
                        }

                        BitStream.WriteBits(&blowState, 2);
                    }

                    // Color
                    CVehicleColor& vehColor = pVehicle->GetColor();
                    uchar          ucNumColors = vehColor.GetNumColorsUsed() - 1;
                    BitStream.WriteBits(&ucNumColors, 2);
                    for (uint i = 0; i <= ucNumColors; i++)
                    {
                        SColor RGBColor = vehColor.GetRGBColor(i);
                        BitStream.Write(RGBColor.R);
                        BitStream.Write(RGBColor.G);
                        BitStream.Write(RGBColor.B);
                    }

                    // Paintjob
                    SPaintjobSync paintjob;
                    paintjob.data.ucPaintjob = pVehicle->GetPaintjob();
                    BitStream.Write(&paintjob);

                    // Write the damage model
                    SVehicleDamageSync damage(true, true, true, true, false);
                    damage.data.ucDoorStates = pVehicle->m_ucDoorStates;
                    damage.data.ucWheelStates = pVehicle->m_ucWheelStates;
                    damage.data.ucPanelStates = pVehicle->m_ucPanelStates;
                    damage.data.ucLightStates = pVehicle->m_ucLightStates;
                    BitStream.Write(&damage);

                    unsigned char ucVariant = pVehicle->GetVariant();
                    BitStream.Write(ucVariant);

                    unsigned char ucVariant2 = pVehicle->GetVariant2();
                    BitStream.Write(ucVariant2);

                    // If the vehicle has a turret, send its position too
                    unsigned short usModel = pVehicle->GetModel();
                    if (CVehicleManager::HasTurret(usModel))
                    {
                        SVehicleTurretSync specific;
                        specific.data.fTurretX = pVehicle->GetTurretPositionX();
                        specific.data.fTurretY = pVehicle->GetTurretPositionY();
                        BitStream.Write(&specific);
                    }

                    // If the vehicle has an adjustable property send its value
                    if (CVehicleManager::HasAdjustableProperty(usModel))
                    {
                        BitStream.WriteCompressed(pVehicle->GetAdjustableProperty());
                    }

                    // If the vehicle has doors, sync their open angle ratios.
                    if (CVehicleManager::HasDoors(usModel))
                    {
                        SDoorOpenRatioSync door;
                        for (unsigned char i = 0; i < 6; ++i)
                        {
                            door.data.fRatio = pVehicle->GetDoorOpenRatio(i);
                            BitStream.Write(&door);
                        }
                    }

                    // Write all the upgrades
                    CVehicleUpgrades*  pUpgrades = pVehicle->GetUpgrades();
                    unsigned char      ucNumUpgrades = pUpgrades->Count();
                    const SSlotStates& usSlotStates = pUpgrades->GetSlotStates();
                    BitStream.Write(ucNumUpgrades);

                    if (ucNumUpgrades > 0)
                    {
                        unsigned char ucSlot = 0;
                        for (; ucSlot < VEHICLE_UPGRADE_SLOTS; ucSlot++)
                        {
                            unsigned short usUpgrade = usSlotStates[ucSlot];

                            /*
                             * This is another retarded modification in an attempt to save
                             * a byte.  We're apparently subtracting 1000 so we can store the
                             * information in a single byte instead of two.  This only gives us
                             * a maximum of 256 vehicle slots.
                             *
                             * --slush
                             * -- ChrML: Ehm, GTA only has 17 upgrade slots... This is a valid optimization.
                             */
                            if (usUpgrade)
                                BitStream.Write(static_cast<unsigned char>(usSlotStates[ucSlot] - 1000));
                        }
                    }

                    // Get the vehicle's reg plate as 8 bytes of chars with the not used bytes
                    // nulled.
                    const char* cszRegPlate = pVehicle->GetRegPlate();
                    BitStream.Write(cszRegPlate, 8);

                    // Light override
                    SOverrideLightsSync overrideLights;
                    overrideLights.data.ucOverride = pVehicle->GetOverrideLights();
                    BitStream.Write(&overrideLights);

                    // Grab various vehicle flags
                    BitStream.WriteBit(pVehicle->IsLandingGearDown());
                    BitStream.WriteBit(pVehicle->IsSirenActive());
                    BitStream.WriteBit(pVehicle->IsFuelTankExplodable());
                    BitStream.WriteBit(pVehicle->IsEngineOn());
                    BitStream.WriteBit(pVehicle->IsLocked());
                    BitStream.WriteBit(pVehicle->AreDoorsUndamageable());
                    BitStream.WriteBit(pVehicle->IsDamageProof());
                    BitStream.WriteBit(pVehicle->IsFrozen());
                    BitStream.WriteBit(pVehicle->IsDerailed());
                    BitStream.WriteBit(pVehicle->IsDerailable());
                    BitStream.WriteBit(pVehicle->GetTrainDirection());
                    BitStream.WriteBit(pVehicle->IsTaxiLightOn());

                    // Write alpha
                    SEntityAlphaSync alpha;
                    alpha.data.ucAlpha = pVehicle->GetAlpha();
                    BitStream.Write(&alpha);

                    // Write headlight color
                    SColor color = pVehicle->GetHeadLightColor();
                    if (color.R != 255 || color.G != 255 || color.B != 255)
                    {
                        BitStream.WriteBit(true);
                        BitStream.Write(color.R);
                        BitStream.Write(color.G);
                        BitStream.Write(color.B);
                    }
                    else
                        BitStream.WriteBit(false);

                    // Write handling
                    if (g_pGame->GetHandlingManager()->HasModelHandlingChanged(static_cast<eVehicleTypes>(pVehicle->GetModel())) ||
                        pVehicle->HasHandlingChanged())
                    {
                        BitStream.WriteBit(true);
                        SVehicleHandlingSync handling;
                        CHandlingEntry*      pEntry = pVehicle->GetHandlingData();

                        handling.data.fMass = pEntry->GetMass();
                        handling.data.fTurnMass = pEntry->GetTurnMass();
                        handling.data.fDragCoeff = pEntry->GetDragCoeff();
                        handling.data.vecCenterOfMass = pEntry->GetCenterOfMass();
                        handling.data.ucPercentSubmerged = pEntry->GetPercentSubmerged();
                        handling.data.fTractionMultiplier = pEntry->GetTractionMultiplier();
                        handling.data.ucDriveType = pEntry->GetCarDriveType();
                        handling.data.ucEngineType = pEntry->GetCarEngineType();
                        handling.data.ucNumberOfGears = pEntry->GetNumberOfGears();
                        handling.data.fEngineAcceleration = pEntry->GetEngineAcceleration();
                        handling.data.fEngineInertia = pEntry->GetEngineInertia();
                        handling.data.fMaxVelocity = pEntry->GetMaxVelocity();
                        handling.data.fBrakeDeceleration = pEntry->GetBrakeDeceleration();
                        handling.data.fBrakeBias = pEntry->GetBrakeBias();
                        handling.data.bABS = pEntry->GetABS();
                        handling.data.fSteeringLock = pEntry->GetSteeringLock();
                        handling.data.fTractionLoss = pEntry->GetTractionLoss();
                        handling.data.fTractionBias = pEntry->GetTractionBias();
                        handling.data.fSuspensionForceLevel = pEntry->GetSuspensionForceLevel();
                        handling.data.fSuspensionDamping = pEntry->GetSuspensionDamping();
                        handling.data.fSuspensionHighSpdDamping = pEntry->GetSuspensionHighSpeedDamping();
                        handling.data.fSuspensionUpperLimit = pEntry->GetSuspensionUpperLimit();
                        handling.data.fSuspensionLowerLimit = pEntry->GetSuspensionLowerLimit();
                        handling.data.fSuspensionFrontRearBias = pEntry->GetSuspensionFrontRearBias();
                        handling.data.fSuspensionAntiDiveMultiplier = pEntry->GetSuspensionAntiDiveMultiplier();
                        handling.data.fCollisionDamageMultiplier = pEntry->GetCollisionDamageMultiplier();
                        handling.data.uiModelFlags = pEntry->GetModelFlags();
                        handling.data.uiHandlingFlags = pEntry->GetHandlingFlags();
                        handling.data.fSeatOffsetDistance = pEntry->GetSeatOffsetDistance();
                        // handling.data.uiMonetary                    = pEntry->GetMonetary ();
                        // handling.data.ucHeadLight                   = pEntry->GetHeadLight ();
                        // handling.data.ucTailLight                   = pEntry->GetTailLight ();
                        handling.data.ucAnimGroup = pEntry->GetAnimGroup();

                        // Lower and Upper limits cannot match or LSOD (unless boat)
                        // if ( pVehicle->GetModel() != VEHICLE_BOAT )     // Commented until fully tested
                        {
                            float fSuspensionLimitSize = handling.data.fSuspensionUpperLimit - handling.data.fSuspensionLowerLimit;
                            if (fSuspensionLimitSize > -0.1f && fSuspensionLimitSize < 0.1f)
                            {
                                if (fSuspensionLimitSize >= 0.f)
                                    handling.data.fSuspensionUpperLimit = handling.data.fSuspensionLowerLimit + 0.1f;
                                else
                                    handling.data.fSuspensionUpperLimit = handling.data.fSuspensionLowerLimit - 0.1f;
                            }
                        }
                        BitStream.Write(&handling);
                    }
                    else
                        BitStream.WriteBit(false);

                    if (BitStream.Version() >= 0x02B)
                    {
                        unsigned char ucSirenCount = pVehicle->m_tSirenBeaconInfo.m_ucSirenCount;
                        unsigned char ucSirenType = pVehicle->m_tSirenBeaconInfo.m_ucSirenType;
                        bool          bSync = pVehicle->m_tSirenBeaconInfo.m_bOverrideSirens;
                        BitStream.WriteBit(bSync);
                        if (bSync)
                        {
                            BitStream.Write(ucSirenCount);
                            BitStream.Write(ucSirenType);

                            for (int i = 0; i < ucSirenCount; i++)
                            {
                                SVehicleSirenSync syncData;
                                syncData.data.m_bOverrideSirens = true;
                                syncData.data.m_b360Flag = pVehicle->m_tSirenBeaconInfo.m_b360Flag;
                                syncData.data.m_bDoLOSCheck = pVehicle->m_tSirenBeaconInfo.m_bDoLOSCheck;
                                syncData.data.m_bUseRandomiser = pVehicle->m_tSirenBeaconInfo.m_bUseRandomiser;
                                syncData.data.m_bEnableSilent = pVehicle->m_tSirenBeaconInfo.m_bSirenSilent;
                                syncData.data.m_ucSirenID = i;
                                syncData.data.m_vecSirenPositions = pVehicle->m_tSirenBeaconInfo.m_tSirenInfo[i].m_vecSirenPositions;
                                syncData.data.m_colSirenColour = pVehicle->m_tSirenBeaconInfo.m_tSirenInfo[i].m_RGBBeaconColour;
                                syncData.data.m_dwSirenMinAlpha = pVehicle->m_tSirenBeaconInfo.m_tSirenInfo[i].m_dwMinSirenAlpha;
                                BitStream.Write(&syncData);
                            }
                        }
                    }
                    break;
                }

                case CElement::MARKER:
                {
                    CMarker* pMarker = static_cast<CMarker*>(pElement);

                    // Position
                    position.data.vecPosition = pMarker->GetPosition();
                    SilentlyFixIndeterminate(position.data.vecPosition);            // Crash fix for pre r6459 clients
                    BitStream.Write(&position);

                    // Type
                    SMarkerTypeSync markerType;
                    markerType.data.ucType = pMarker->GetMarkerType();
                    BitStream.Write(&markerType);

                    // Size
                    float fSize = pMarker->GetSize();
                    BitStream.Write(fSize);

                    // Colour
                    SColorSync color;
                    color = pMarker->GetColor();
                    BitStream.Write(&color);

                    // Write the target position vector eventually
                    if (markerType.data.ucType == CMarker::TYPE_CHECKPOINT || markerType.data.ucType == CMarker::TYPE_RING)
                    {
                        if (pMarker->HasTarget())
                        {
                            BitStream.WriteBit(true);

                            position.data.vecPosition = pMarker->GetTarget();
                            BitStream.Write(&position);

                            if (markerType.data.ucType == CMarker::TYPE_CHECKPOINT && BitStream.Can(eBitStreamVersion::SetMarkerTargetArrowProperties))
                            {
                                SColor color = pMarker->GetTargetArrowColor();

                                BitStream.Write(color.R);
                                BitStream.Write(color.G);
                                BitStream.Write(color.B);
                                BitStream.Write(color.A);
                                BitStream.Write(pMarker->GetTargetArrowSize());
                            }
                        }
                        else
                            BitStream.WriteBit(false);
                    }

                    // Alpha limit
                    if (BitStream.Can(eBitStreamVersion::Marker_IgnoreAlphaLimits))
                        BitStream.WriteBit(pMarker->AreAlphaLimitsIgnored());

                    break;
                }

                case CElement::BLIP:
                {
                    CBlip* pBlip = static_cast<CBlip*>(pElement);

                    // Grab the blip position
                    position.data.vecPosition = pBlip->GetPosition();
                    BitStream.Write(&position);

                    // Write the ordering id
                    BitStream.WriteCompressed(pBlip->m_sOrdering);

                    // Write the visible distance - 14 bits allows 16383.
                    SIntegerSync<unsigned short, 14> visibleDistance(std::min(pBlip->m_usVisibleDistance, (unsigned short)16383));
                    BitStream.Write(&visibleDistance);

                    // Write the icon
                    SIntegerSync<unsigned char, 6> icon(pBlip->m_ucIcon);
                    BitStream.Write(&icon);

                    // Write the size
                    SIntegerSync<unsigned char, 5> size(pBlip->m_ucSize);
                    BitStream.Write(&size);

                    // Write the color
                    SColorSync color;
                    color = pBlip->GetColor();
                    BitStream.Write(&color);

                    break;
                }

                case CElement::RADAR_AREA:
                {
                    CRadarArea* pArea = static_cast<CRadarArea*>(pElement);

                    // Write the position
                    SPosition2DSync position2D(false);
                    position2D.data.vecPosition = pArea->GetPosition();
                    SilentlyFixIndeterminate(position2D.data.vecPosition);            // Crash fix for pre r6459 clients
                    BitStream.Write(&position2D);

                    // Write the size
                    SPosition2DSync size2D(false);
                    size2D.data.vecPosition = pArea->GetSize();
                    SilentlyFixIndeterminate(size2D.data.vecPosition);            // Crash fix for pre r6459 clients
                    BitStream.Write(&size2D);

                    // And the color
                    SColor color = pArea->GetColor();
                    BitStream.Write(color.R);
                    BitStream.Write(color.G);
                    BitStream.Write(color.B);
                    BitStream.Write(color.A);

                    // Write whether it is flashing
                    bool bIsFlashing = pArea->IsFlashing();
                    BitStream.WriteBit(bIsFlashing);

                    break;
                }

                case CElement::WORLD_MESH_UNUSED:
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
                    CTeam* pTeam = static_cast<CTeam*>(pElement);

                    // Write the name
                    const char*    szTeamName = pTeam->GetTeamName();
                    unsigned short usNameLength = static_cast<unsigned short>(strlen(szTeamName));
                    unsigned char  ucRed, ucGreen, ucBlue;
                    pTeam->GetColor(ucRed, ucGreen, ucBlue);
                    bool bFriendlyFire = pTeam->GetFriendlyFire();
                    BitStream.WriteCompressed(usNameLength);
                    BitStream.Write(szTeamName, usNameLength);
                    BitStream.Write(ucRed);
                    BitStream.Write(ucGreen);
                    BitStream.Write(ucBlue);
                    BitStream.WriteBit(bFriendlyFire);
                    BitStream.Write(pTeam->CountPlayers());
                    for (list<CPlayer*>::const_iterator iter = pTeam->PlayersBegin(); iter != pTeam->PlayersEnd(); ++iter)
                        BitStream.Write((*iter)->GetID());

                    break;
                }

                case CElement::PED:
                {
                    CPed* pPed = static_cast<CPed*>(pElement);

                    // position
                    position.data.vecPosition = pPed->GetPosition();
                    BitStream.Write(&position);

                    // model
                    unsigned short usModel = pPed->GetModel();
                    BitStream.WriteCompressed(usModel);

                    // rotation
                    SPedRotationSync pedRotation;
                    pedRotation.data.fRotation = pPed->GetRotation();
                    BitStream.Write(&pedRotation);

                    // health
                    SPlayerHealthSync health;
                    health.data.fValue = pPed->GetHealth();
                    BitStream.Write(&health);

                    // Armor
                    SPlayerArmorSync armor;
                    armor.data.fValue = pPed->GetArmor();
                    BitStream.Write(&armor);

                    // vehicle
                    CVehicle* pVehicle = pPed->GetOccupiedVehicle();
                    if (pVehicle)
                    {
                        BitStream.WriteBit(true);
                        BitStream.Write(pVehicle->GetID());

                        SOccupiedSeatSync seat;
                        seat.data.ucSeat = pPed->GetOccupiedVehicleSeat();
                        BitStream.Write(&seat);
                    }
                    else
                        BitStream.WriteBit(false);

                    // flags
                    BitStream.WriteBit(pPed->HasJetPack());
                    BitStream.WriteBit(pPed->IsSyncable());
                    BitStream.WriteBit(pPed->IsHeadless());
                    BitStream.WriteBit(pPed->IsFrozen());

                    // alpha
                    SEntityAlphaSync alpha;
                    alpha.data.ucAlpha = pPed->GetAlpha();
                    BitStream.Write(&alpha);

                    // Move anim
                    if (BitStream.Version() > 0x4B)
                    {
                        uchar ucMoveAnim = pPed->GetMoveAnim();
                        BitStream.Write(ucMoveAnim);
                    }

                    // clothes
                    unsigned char   ucNumClothes = 0;
                    CPlayerClothes* pClothes = pPed->GetClothes();
                    for (unsigned char ucType = 0; ucType < PLAYER_CLOTHING_SLOTS; ucType++)
                    {
                        const SPlayerClothing* pClothing = pClothes->GetClothing(ucType);
                        if (pClothing)
                        {
                            ucNumClothes++;
                        }
                    }
                    BitStream.Write(ucNumClothes);
                    for (unsigned char ucType = 0; ucType < PLAYER_CLOTHING_SLOTS; ucType++)
                    {
                        const SPlayerClothing* pClothing = pClothes->GetClothing(ucType);
                        if (pClothing)
                        {
                            unsigned char ucTextureLength = static_cast<uchar>(strlen(pClothing->szTexture));
                            unsigned char ucModelLength = static_cast<uchar>(strlen(pClothing->szModel));

                            BitStream.Write(ucTextureLength);
                            BitStream.Write(pClothing->szTexture, ucTextureLength);
                            BitStream.Write(ucModelLength);
                            BitStream.Write(pClothing->szModel, ucModelLength);
                            BitStream.Write(ucType);
                        }
                    }

                    // weapons
                    if (BitStream.Version() >= 0x61)
                    {
                        // Get a list of weapons
                        for (unsigned char slot = 0; slot < WEAPONSLOT_MAX; ++slot)
                        {
                            CWeapon* pWeapon = pPed->GetWeapon(slot);
                            if (pWeapon->ucType != 0)
                            {
                                BitStream.Write(slot);
                                BitStream.Write(pWeapon->ucType);
                                BitStream.Write(pWeapon->usAmmo);

                                // ammoInClip is not implemented generally
                                // BitStream.Write ( pWeapon->usAmmoInClip );
                            }
                        }

                        // Write end marker (slot)
                        BitStream.Write((unsigned char)0xFF);

                        // Send the current weapon spot
                        unsigned char currentWeaponSlot = pPed->GetWeaponSlot();
                        BitStream.Write(currentWeaponSlot);
                    }

                    break;
                }

                case CElement::DUMMY:
                {
                    CDummy* pDummy = static_cast<CDummy*>(pElement);

                    // Type Name
                    const char*    szTypeName = pDummy->GetTypeName().c_str();
                    unsigned short usTypeNameLength = static_cast<unsigned short>(strlen(szTypeName));
                    BitStream.WriteCompressed(usTypeNameLength);
                    BitStream.Write(szTypeName, usTypeNameLength);

                    // Position
                    position.data.vecPosition = pDummy->GetPosition();
                    if (position.data.vecPosition != CVector(0.0f, 0.0f, 0.0f))
                    {
                        BitStream.WriteBit(true);
                        BitStream.Write(&position);
                    }
                    else
                        BitStream.WriteBit(false);

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
                    CColShape* pColShape = static_cast<CColShape*>(pElement);
                    if (!pColShape->GetParentEntity())
                    {
                        // Jax: i'm pretty sure this is fucking up our packet somehow..
                        // all valid col-shapes should have a parent!
                        assert(false);
                    }

                    // Type
                    SColshapeTypeSync colType;
                    colType.data.ucType = static_cast<unsigned char>(pColShape->GetShapeType());
                    BitStream.Write(&colType);

                    // Position
                    position.data.vecPosition = pColShape->GetPosition();
                    BitStream.Write(&position);

                    // Enabled
                    BitStream.WriteBit(pColShape->IsEnabled());

                    // Auto Call Event
                    BitStream.WriteBit(pColShape->GetAutoCallEvent());

                    switch (pColShape->GetShapeType())
                    {
                        case COLSHAPE_CIRCLE:
                        {
                            BitStream.Write(static_cast<CColCircle*>(pColShape)->GetRadius());
                            break;
                        }
                        case COLSHAPE_CUBOID:
                        {
                            SPositionSync size(false);
                            size.data.vecPosition = static_cast<CColCuboid*>(pColShape)->GetSize();
                            BitStream.Write(&size);
                            break;
                        }
                        case COLSHAPE_SPHERE:
                        {
                            BitStream.Write(static_cast<CColSphere*>(pColShape)->GetRadius());
                            break;
                        }
                        case COLSHAPE_RECTANGLE:
                        {
                            SPosition2DSync size(false);
                            size.data.vecPosition = static_cast<CColRectangle*>(pColShape)->GetSize();
                            BitStream.Write(&size);
                            break;
                        }
                        case COLSHAPE_TUBE:
                        {
                            BitStream.Write(static_cast<CColTube*>(pColShape)->GetRadius());
                            BitStream.Write(static_cast<CColTube*>(pColShape)->GetHeight());
                            break;
                        }
                        case COLSHAPE_POLYGON:
                        {
                            CColPolygon* pPolygon = static_cast<CColPolygon*>(pColShape);
                            BitStream.WriteCompressed(pPolygon->CountPoints());
                            std::vector<CVector2D>::const_iterator iter = pPolygon->IterBegin();
                            for (; iter != pPolygon->IterEnd(); ++iter)
                            {
                                SPosition2DSync vertex(false);
                                vertex.data.vecPosition = *iter;
                                BitStream.Write(&vertex);
                            }

                            if (BitStream.Can(eBitStreamVersion::SetColPolygonHeight))
                            {
                                float fFloor, fCeil;
                                pPolygon->GetHeight(fFloor, fCeil);

                                BitStream.Write(fFloor);
                                BitStream.Write(fCeil);
                            }
                            break;
                        }
                        default:
                            break;
                    }
                    break;
                }

                case CElement::WATER:
                {
                    CWater*       pWater = static_cast<CWater*>(pElement);
                    unsigned char ucNumVertices = (unsigned char)pWater->GetNumVertices();
                    BitStream.Write(ucNumVertices);
                    CVector vecVertex;
                    for (int i = 0; i < ucNumVertices; i++)
                    {
                        pWater->GetVertex(i, vecVertex);
                        BitStream.Write((short)vecVertex.fX);
                        BitStream.Write((short)vecVertex.fY);
                        BitStream.Write(vecVertex.fZ);
                    }
                    if (BitStream.Can(eBitStreamVersion::Water_bShallow_ServerSide))
                        BitStream.WriteBit(pWater->IsWaterShallow());
                    break;
                }

                default:
                {
                    assert(0);
                    CLogger::LogPrintf("not sending this element - id: %i\n", pElement->GetType());
                }
            }
        }

        // Success
        return true;
    }

    return false;
}
