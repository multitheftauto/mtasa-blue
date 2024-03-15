/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CVehicleRPCs.cpp
 *  PURPOSE:     Vehicle remote procedure calls
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CVehicleRPCs.h"

void CVehicleRPCs::LoadFunctions()
{
    AddHandler(DESTROY_ALL_VEHICLES, DestroyAllVehicles, "DestroyAllVehicles");
    AddHandler(FIX_VEHICLE, FixVehicle, "FixVehicle");
    AddHandler(BLOW_VEHICLE, BlowVehicle, "BlowVehicle");
    AddHandler(SET_VEHICLE_ROTATION, SetVehicleRotation, "SetVehicleRotation");
    AddHandler(SET_VEHICLE_TURNSPEED, SetVehicleTurnSpeed, "SetVehicleTurnSpeed");
    AddHandler(SET_VEHICLE_COLOR, SetVehicleColor, "SetVehicleColor");
    AddHandler(SET_VEHICLE_LOCKED, SetVehicleLocked, "SetVehicleLocked");
    AddHandler(SET_VEHICLE_DOORS_UNDAMAGEABLE, SetVehicleDoorsUndamageable, "");
    AddHandler(SET_VEHICLE_SIRENE_ON, SetVehicleSireneOn, "SetVehicleSireneOn");
    AddHandler(SET_VEHICLE_LANDING_GEAR_DOWN, SetVehicleLandingGearDown, "SetVehicleLandingGearDown");
    AddHandler(SET_HELICOPTER_ROTOR_SPEED, SetHelicopterRotorSpeed, "SetHelicopterRotorSpeed");
    AddHandler(ADD_VEHICLE_UPGRADE, AddVehicleUpgrade, "AddVehicleUpgrade");
    AddHandler(ADD_ALL_VEHICLE_UPGRADES, AddAllVehicleUpgrades, "AddAllVehicleUpgrades");
    AddHandler(REMOVE_VEHICLE_UPGRADE, RemoveVehicleUpgrade, "RemoveVehicleUpgrade");
    AddHandler(SET_VEHICLE_DAMAGE_STATE, SetVehicleDamageState, "SetVehicleDamageState");
    AddHandler(SET_VEHICLE_OVERRIDE_LIGHTS, SetVehicleOverrideLights, "SetVehicleOverrideLights");
    AddHandler(SET_VEHICLE_ENGINE_STATE, SetVehicleEngineState, "SetVehicleEngineState");
    AddHandler(SET_VEHICLE_DIRT_LEVEL, SetVehicleDirtLevel, "SetVehicleDirtLevel");
    AddHandler(SET_VEHICLE_DAMAGE_PROOF, SetVehicleDamageProof, "SetVehicleDamageProof");
    AddHandler(SET_VEHICLE_PAINTJOB, SetVehiclePaintjob, "SetVehiclePaintjob");
    AddHandler(SET_VEHICLE_FUEL_TANK_EXPLODABLE, SetVehicleFuelTankExplodable, "SetVehicleFuelTankExplodable");
    AddHandler(SET_VEHICLE_WHEEL_STATES, SetVehicleWheelStates, "SetVehicleWheelStates");
    AddHandler(SET_VEHICLE_FROZEN, SetVehicleFrozen, "SetVehicleFrozen");
    AddHandler(SET_TRAIN_DERAILED, SetTrainDerailed, "SetTrainDerailed");
    AddHandler(SET_TRAIN_DERAILABLE, SetTrainDerailable, "SetTrainDerailable");
    AddHandler(SET_TRAIN_DIRECTION, SetTrainDirection, "SetTrainDirection");
    AddHandler(SET_TRAIN_SPEED, SetTrainSpeed, "SetTrainSpeed");
    AddHandler(SET_TRAIN_TRACK, SetTrainTrack, "SetTrainTrack");
    AddHandler(SET_TRAIN_POSITION, SetTrainPosition, "SetTrainPosition");
    AddHandler(SET_TAXI_LIGHT_ON, SetVehicleTaxiLightOn, "SetVehicleTaxiLightOn");
    AddHandler(SET_VEHICLE_HEADLIGHT_COLOR, SetVehicleHeadLightColor, "SetVehicleHeadLightColor");
    AddHandler(SET_VEHICLE_TURRET_POSITION, SetVehicleTurretPosition, "SetVehicleTurretPosition");
    AddHandler(SET_VEHICLE_DOOR_OPEN_RATIO, SetVehicleDoorOpenRatio, "SetVehicleDoorOpenRatio");
    AddHandler(SET_VEHICLE_VARIANT, SetVehicleVariant, "SetVehicleVariant");
    AddHandler(GIVE_VEHICLE_SIRENS, GiveVehicleSirens, "giveVehicleSirens");
    AddHandler(REMOVE_VEHICLE_SIRENS, RemoveVehicleSirens, "removeVehicleSirens");
    AddHandler(SET_VEHICLE_SIRENS, SetVehicleSirens, "setVehicleSirens");
    AddHandler(SET_VEHICLE_PLATE_TEXT, SetVehiclePlateText, "setVehiclePlateText");
}

void CVehicleRPCs::DestroyAllVehicles(NetBitStreamInterface& bitStream)
{
    m_pVehicleManager->DeleteAll();
}

void CVehicleRPCs::FixVehicle(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    // Read out the vehicle id
    std::uint8_t ucTimeContext;
    if (bitStream.Read(ucTimeContext))
    {
        // Grab the vehicle
        CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
        if (pVehicle)
        {
            // Fix it
            pVehicle->Fix();
            pVehicle->SetSyncTimeContext(ucTimeContext);
        }
    }
}

void CVehicleRPCs::BlowVehicle(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    std::uint8_t syncTimeContext;
    bool          withExplosion = true;

    if (bitStream.Read(syncTimeContext))
    {
        if (bitStream.Can(eBitStreamVersion::VehicleBlowStateSupport) && !bitStream.ReadBit(withExplosion))
        {
            return;
        }

        CClientVehicle* vehicle = m_pVehicleManager->Get(pSource->GetID());

        if (vehicle)
        {
            VehicleBlowFlags blow;
            blow.withExplosion = withExplosion;
            vehicle->Blow(blow);

            vehicle->SetSyncTimeContext(syncTimeContext);
        }
    }
}

void CVehicleRPCs::SetVehicleRotation(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    // Grab the vehicle
    CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
    if (pVehicle)
    {
        // Read out the rotation
        CVector       vecRotation;
        std::uint8_t ucTimeContext;
        if (bitStream.Read(vecRotation.fX) && bitStream.Read(vecRotation.fY) && bitStream.Read(vecRotation.fZ) && bitStream.Read(ucTimeContext))
        {
            // Set the new rotation
            pVehicle->SetRotationDegrees(vecRotation);
            pVehicle->SetSyncTimeContext(ucTimeContext);
        }
    }
}

void CVehicleRPCs::SetVehicleTurnSpeed(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    // Grab the vehicle
    CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
    if (pVehicle)
    {
        // Read out the turn speed
        CVector vecTurnSpeed;
        if (bitStream.Read(vecTurnSpeed.fX) && bitStream.Read(vecTurnSpeed.fY) && bitStream.Read(vecTurnSpeed.fZ))
        {
            // Set the new movespeed
            pVehicle->SetTurnSpeed(vecTurnSpeed);
        }
    }
}

void CVehicleRPCs::SetVehicleColor(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
    if (pVehicle)
    {
        CVehicleColor vehColor;
        uchar         ucNumColors = 0;
        bitStream.ReadBits(&ucNumColors, 2);
        for (uint i = 0; i <= ucNumColors; i++)
        {
            SColor RGBColor = 0;
            bitStream.Read(RGBColor.R);
            bitStream.Read(RGBColor.G);
            bitStream.Read(RGBColor.B);
            vehColor.SetRGBColor(i, RGBColor);
        }

        pVehicle->SetColor(vehColor);
    }
}

void CVehicleRPCs::SetVehicleLocked(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    // Read out the stuff
    std::uint8_t ucLocked;
    if (bitStream.Read(ucLocked))
    {
        // Grab the vehicle
        CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
        if (pVehicle)
        {
            // Set its locked state
            pVehicle->SetDoorsLocked(ucLocked != 0);
        }
    }
}

void CVehicleRPCs::SetVehicleDoorsUndamageable(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    // Read out the stuff
    std::uint8_t ucDoorsUndamageable;
    if (bitStream.Read(ucDoorsUndamageable))
    {
        // Grab the vehicle
        CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
        if (pVehicle)
        {
            // Set its doors undamageable state
            pVehicle->SetDoorsUndamageable(ucDoorsUndamageable != 0);
        }
    }
}

void CVehicleRPCs::SetVehicleSireneOn(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
    if (pVehicle)
    {
        std::uint8_t ucSirenesOn;
        if (bitStream.Read(ucSirenesOn))
        {
            bool bSirenesOn = false;
            if (ucSirenesOn > 0)
                bSirenesOn = true;

            pVehicle->SetSirenOrAlarmActive(bSirenesOn);
        }
    }
}

void CVehicleRPCs::SetVehicleTaxiLightOn(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    std::uint8_t ucTaxiLightOn;
    if (bitStream.Read(ucTaxiLightOn))
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
        if (pVehicle)
        {
            pVehicle->SetTaxiLightOn(ucTaxiLightOn != 0);
        }
    }
}

void CVehicleRPCs::SetVehicleLandingGearDown(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
    if (pVehicle)
    {
        std::uint8_t ucLandingGearDown;
        if (bitStream.Read(ucLandingGearDown))
        {
            bool bLandingGearDown = false;
            if (ucLandingGearDown > 0)
                bLandingGearDown = true;

            pVehicle->SetLandingGearDown(bLandingGearDown);
        }
    }
}

void CVehicleRPCs::SetHelicopterRotorSpeed(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
    if (pVehicle)
    {
        std::uint8_t ucRotorSpeed;
        if (bitStream.Read(ucRotorSpeed))
        {
            // Convert the given rotor speed from between 0-100 to 0-0.22
            float fRotorSpeed = (static_cast<float>(ucRotorSpeed) / 100.0f * 0.22f);
            pVehicle->SetHeliRotorSpeed(fRotorSpeed);
        }
    }
}

void CVehicleRPCs::AddVehicleUpgrade(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
    if (pVehicle)
    {
        std::uint16_t usUpgrade;
        if (bitStream.Read(usUpgrade))
        {
            CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades();
            if (pUpgrades)
            {
                pUpgrades->AddUpgrade(usUpgrade, false);
            }
        }
    }
}

void CVehicleRPCs::AddAllVehicleUpgrades(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
    if (pVehicle)
    {
        CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades();
        if (pUpgrades)
        {
            pUpgrades->AddAllUpgrades();
        }
    }
}

void CVehicleRPCs::RemoveVehicleUpgrade(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
    if (pVehicle)
    {
        std::uint8_t ucUpgrade;
        if (bitStream.Read(ucUpgrade))
        {
            // Convert back and add
            std::uint16_t usUpgrade = (ucUpgrade + 1000);

            CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades();
            if (pUpgrades)
            {
                pUpgrades->RemoveUpgrade(usUpgrade);
            }
        }
    }
}

void CVehicleRPCs::SetVehicleDamageState(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
    if (pVehicle)
    {
        std::uint8_t ucObject;
        if (bitStream.Read(ucObject))
        {
            switch (ucObject)
            {
                case 0:            // Door
                {
                    std::uint8_t ucDoor, ucState;
                    if (bitStream.Read(ucDoor) && bitStream.Read(ucState))
                    {
                        bool spawnFlyingComponent = true;

                        if (bitStream.Can(eBitStreamVersion::SetVehicleDoorState_SpawnFlyingComponent))
                        {
                            if (!bitStream.ReadBit(spawnFlyingComponent))
                                break;
                        }

                        pVehicle->SetDoorStatus(ucDoor, ucState, spawnFlyingComponent);
                    }
                    break;
                }
                case 1:            // Wheel
                {
                    std::uint8_t ucWheel, ucState;
                    if (bitStream.Read(ucWheel) && bitStream.Read(ucState))
                    {
                        pVehicle->SetWheelStatus(ucWheel, ucState, false);
                    }
                    break;
                }
                case 2:            // Light
                {
                    std::uint8_t ucLight, ucState;
                    if (bitStream.Read(ucLight) && bitStream.Read(ucState))
                    {
                        pVehicle->SetLightStatus(ucLight, ucState);
                    }
                    break;
                }
                case 3:            // Panel
                {
                    std::uint8_t ucPanel, ucState;
                    if (bitStream.Read(ucPanel) && bitStream.Read(ucState))
                    {
                        pVehicle->SetPanelStatus(ucPanel, ucState);
                    }
                }
                default:
                    break;
            }
        }
    }
}

void CVehicleRPCs::SetVehicleOverrideLights(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    std::uint8_t ucLights;
    if (bitStream.Read(ucLights))
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
        if (pVehicle)
        {
            pVehicle->SetOverrideLights(ucLights);
        }
    }
}

void CVehicleRPCs::SetVehicleEngineState(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    bool bState;
    if (bitStream.ReadBit(bState))
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
        if (pVehicle)
        {
            pVehicle->SetEngineOn(bState);
        }
    }
}

void CVehicleRPCs::SetVehicleDirtLevel(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    float fDirtLevel;
    if (bitStream.Read(fDirtLevel))
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
        if (pVehicle)
        {
            pVehicle->SetDirtLevel(fDirtLevel);
        }
    }
}

void CVehicleRPCs::SetVehicleDamageProof(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    std::uint8_t ucDamageProof;
    if (bitStream.Read(ucDamageProof))
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
        if (pVehicle)
        {
            pVehicle->SetScriptCanBeDamaged((ucDamageProof == 0));
        }
    }
}

void CVehicleRPCs::SetVehiclePaintjob(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    std::uint8_t ucPaintjob;
    if (bitStream.Read(ucPaintjob))
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
        if (pVehicle)
        {
            pVehicle->SetPaintjob(ucPaintjob);
        }
    }
}

void CVehicleRPCs::SetVehicleFuelTankExplodable(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    bool bExplodable;
    if (bitStream.ReadBit(bExplodable))
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
        if (pVehicle)
        {
            pVehicle->SetCanShootPetrolTank(bExplodable);
        }
    }
}

void CVehicleRPCs::SetVehicleWheelStates(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    std::uint8_t ucWheelStates[MAX_WHEELS];
    if (bitStream.Read((char*)ucWheelStates, MAX_WHEELS))
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
        if (pVehicle)
        {
            for (auto i = 0; i < MAX_WHEELS; i++)
                pVehicle->SetWheelStatus(i, ucWheelStates[i], false);
        }
    }
}

void CVehicleRPCs::SetVehicleFrozen(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    bool bFrozen;
    if (bitStream.ReadBit(bFrozen))
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
        if (pVehicle)
        {
            pVehicle->SetScriptFrozen(bFrozen);
            pVehicle->SetFrozen(bFrozen);
        }
    }
}

void CVehicleRPCs::SetTrainDerailed(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    bool bDerailed;
    if (bitStream.ReadBit(bDerailed))
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
        if (pVehicle)
        {
            pVehicle->SetDerailed(bDerailed);
        }
    }
}

void CVehicleRPCs::SetTrainDerailable(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    bool bDerailable;
    if (bitStream.ReadBit(bDerailable))
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
        if (pVehicle)
        {
            pVehicle->SetDerailable(bDerailable);
        }
    }
}

void CVehicleRPCs::SetTrainDirection(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    bool bDirection;
    if (bitStream.ReadBit(bDirection))
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
        if (pVehicle)
        {
            pVehicle->SetTrainDirection(bDirection);
        }
    }
}

void CVehicleRPCs::SetTrainSpeed(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    float fSpeed;
    if (bitStream.Read(fSpeed))
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
        if (pVehicle)
        {
            pVehicle->SetTrainSpeed(fSpeed);
        }
    }
}

void CVehicleRPCs::SetTrainTrack(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    uchar ucTrack;
    if (bitStream.Read(ucTrack))
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
        if (pVehicle)
        {
            pVehicle->SetTrainTrack(ucTrack);
        }
    }
}

void CVehicleRPCs::SetTrainPosition(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    float fPosition;
    if (bitStream.Read(fPosition))
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
        if (pVehicle)
        {
            pVehicle->SetTrainPosition(fPosition, false);
        }
    }
}

void CVehicleRPCs::SetVehicleHeadLightColor(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    SColorRGBA color(255, 255, 255, 255);
    if (bitStream.Read(color.R) && bitStream.Read(color.G) && bitStream.Read(color.B))
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
        if (pVehicle)
        {
            pVehicle->SetHeadLightColor(color);
        }
    }
}

void CVehicleRPCs::SetVehicleTurretPosition(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    float fHorizontal;
    float fVertical;

    if (bitStream.Read(fHorizontal) && bitStream.Read(fVertical))
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
        if (pVehicle)
        {
            pVehicle->SetTurretRotation(fHorizontal, fVertical);
        }
    }
}

void CVehicleRPCs::SetVehicleDoorOpenRatio(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    SIntegerSync<std::uint8_t, 3> ucDoor;
    SDoorOpenRatioSync             angle;
    std::uint32_t                   uiTime;

    if (bitStream.Read(&ucDoor) && bitStream.Read(&angle) && bitStream.ReadCompressed(uiTime))
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
        if (pVehicle)
        {
            pVehicle->SetDoorOpenRatio(ucDoor, angle.data.fRatio, uiTime, true);
        }
    }
}

void CVehicleRPCs::SetVehicleVariant(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    std::uint8_t ucVariant = 0;
    std::uint8_t ucVariant2 = 0;
    if (bitStream.Read(ucVariant) && bitStream.Read(ucVariant2))
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get(pSource->GetID());
        if (pVehicle)
        {
            pVehicle->SetVariant(ucVariant, ucVariant2);
        }
    }
}

void CVehicleRPCs::GiveVehicleSirens(CClientEntity* pSourceEntity, NetBitStreamInterface& bitStream)
{
    SVehicleSirenAddSync sirenData;
    if (bitStream.Read(&sirenData))
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get(pSourceEntity->GetID());
        if (pVehicle)
        {
            pVehicle->GiveVehicleSirens(sirenData.data.m_ucSirenType, sirenData.data.m_ucSirenCount);
            pVehicle->SetVehicleFlags(sirenData.data.m_b360Flag, sirenData.data.m_bUseRandomiser, sirenData.data.m_bDoLOSCheck, sirenData.data.m_bEnableSilent);
        }
    }
}

void CVehicleRPCs::SetVehicleSirens(CClientEntity* pSourceEntity, NetBitStreamInterface& bitStream)
{
    SVehicleSirenSync sirenData;
    if (bitStream.Read(&sirenData))
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get(pSourceEntity->GetID());
        if (pVehicle)
        {
            pVehicle->SetVehicleSirenPosition(sirenData.data.m_ucSirenID, sirenData.data.m_vecSirenPositions);
            pVehicle->SetVehicleSirenMinimumAlpha(sirenData.data.m_ucSirenID, sirenData.data.m_dwSirenMinAlpha);
            pVehicle->SetVehicleSirenColour(sirenData.data.m_ucSirenID, sirenData.data.m_colSirenColour);
            pVehicle->SetVehicleFlags(sirenData.data.m_b360Flag, sirenData.data.m_bUseRandomiser, sirenData.data.m_bDoLOSCheck, sirenData.data.m_bEnableSilent);
        }
    }
}

void CVehicleRPCs::RemoveVehicleSirens(CClientEntity* pSourceEntity, NetBitStreamInterface& bitStream)
{
    CClientVehicle* pVehicle = m_pVehicleManager->Get(pSourceEntity->GetID());
    if (pVehicle)
    {
        pVehicle->RemoveVehicleSirens();
    }
}

void CVehicleRPCs::SetVehiclePlateText(CClientEntity* pSourceEntity, NetBitStreamInterface& bitStream)
{
    SString strText;
    if (bitStream.ReadString(strText))
    {
        CClientVehicle* pVehicle = m_pVehicleManager->Get(pSourceEntity->GetID());
        if (pVehicle)
        {
            pVehicle->SetRegPlate(strText);
        }
    }
}
