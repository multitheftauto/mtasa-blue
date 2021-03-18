/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaVehicleDefs.cpp
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <lua/CLuaFunctionParser.h>

// Way too complicated with new parser, as there's an unused argument
// which if missing would shift all arguments. It's kinda crap.
// TODO: Do this with new parser if nested parsers will be done
int CreateVehicle(lua_State* luaVM)
{
    //  vehicle createVehicle ( int model, float x, float y, float z, [float rx, float ry, float rz, string numberplate, bool bDirection, int variant1, int
    //  variant2] )
    ushort  usModel;
    CVector vecPosition;
    CVector vecRotation;
    SString strNumberPlate;
    uchar   ucVariant;
    uchar   ucVariant2;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(usModel);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadVector3D(vecRotation, CVector());
    argStream.ReadString(strNumberPlate, "");
    if (argStream.NextIsBool()) // Unused argument
    {
        bool bDirection;
        argStream.ReadBool(bDirection);
    }
    argStream.ReadNumber(ucVariant, 254);
    argStream.ReadNumber(ucVariant2, 254);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                CVehicle* pVehicle =
                    CStaticFunctionDefinitions::CreateVehicle(pResource, usModel, vecPosition, vecRotation, strNumberPlate, ucVariant, ucVariant2);
                if (pVehicle)
                {
                    CElementGroup* pGroup = pResource->GetElementGroup();
                    if (pGroup)
                        pGroup->Add(pVehicle);
                    lua_pushelement(luaVM, pVehicle);
                    return 1;
                }
            }
        }
    }
    else
        g_pGame->GetScriptDebugging()->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

size_t ResolveModel(std::variant<CVehicle*, size_t> vehicleOrModel)
{
    if (std::holds_alternative<size_t>(vehicleOrModel))
    {
        size_t model = std::get<size_t>(vehicleOrModel);
        if (!CVehicleManager::IsValidModel(model))
            throw std::invalid_argument("Invalid vehicle model");
        return model;
    }
    return std::get<CVehicle*>(vehicleOrModel)->GetModel();
}

static std::variant<std::string_view, bool> GetVehicleType(std::variant<CVehicle*, size_t> vehicleOrModel)
{
    std::string_view typeName = CVehicleNames::GetVehicleTypeName(ResolveModel(vehicleOrModel));
    if (typeName.empty())
        return false;
    return typeName;
}

static std::variant<bool, unsigned int> GetVehicleMaxPassengers(std::variant<CVehicle*, size_t> vehicleOrModel)
{
    if (auto max = CVehicleManager::GetMaxPassengers(ResolveModel(vehicleOrModel)); max != 0xff)
        return max;
    return false;
}

static bool SetVehicleVariant(CVehicle* pVehicle, std::optional<unsigned char> variant, std::optional<unsigned char> variant2)
{
    return CStaticFunctionDefinitions::SetVehicleVariant(pVehicle, variant.value_or(0xfe), variant2.value_or(0xfe));
}

static CLuaMultiReturn<unsigned char, unsigned char> GetVehicleVariant(CVehicle* pVehicle)
{
    return { pVehicle->GetVariant(), pVehicle->GetVariant2() };
}

std::variant<bool, unsigned short> CLuaVehicleDefs::GetVehicleModelFromName(CVehicle* pVehicle, std::string_view name)
{
    if (auto model = (unsigned short)CVehicleNames::GetVehicleModel(name.data()))
        return model;
    return false;
}

static bool GetVehicleLandingGearDown(CVehicle* pVehicle)
{
    if (CVehicleManager::HasLandingGears(pVehicle->GetModel()))
        return pVehicle->IsLandingGearDown();
    return false;
}

static bool SetVehicleSirens(CVehicle* pVehicle, unsigned char sirenID, CVector pos, uchar r, uchar g, uchar b, std::optional<uchar> a, std::optional<DWORD> minA)
{
    if (sirenID <= 0 || sirenID >= 9)
        return false;

    // Array indicies start at 0 so compensate here. This way all code works properly and we get nice 1-8 numbers for API
    sirenID--;

    SSirenInfo sirenInfo;
    sirenInfo.m_tSirenInfo[sirenID] = { pos, SColorRGBA{ r, g, b, a.value_or(255) }, minA.value_or(0) };
    return CStaticFunctionDefinitions::SetVehicleSirens(pVehicle, sirenID, sirenInfo);
}

static auto GetVehicleSirenParams(CVehicle* pVehicle)
{
    SSirenInfo info = pVehicle->m_tSirenBeaconInfo;

    using namespace std;
    return unordered_map<string_view, variant<uchar, unordered_map<string_view, bool>>>{
        {"SirenCount", info.m_ucSirenCount},
        {"SirenType", info.m_ucSirenType},
        {"Flags", unordered_map<string_view, bool>{
                {"360", info.m_b360Flag},
                {"DoLOSCheck", info.m_bDoLOSCheck},
                {"UseRandomiser", info.m_bUseRandomiser},
                {"Silent", info.m_bSirenSilent}
            }
        }
    };
}

static auto GetVehicleSirens(CVehicle* pVehicle)
{
    const auto& beaconInfo = pVehicle->m_tSirenBeaconInfo;

    std::vector<std::unordered_map<std::string_view, lua_Number>> out;
    out.reserve(beaconInfo.m_ucSirenCount);

    for (size_t i = 0; i < beaconInfo.m_ucSirenCount; i++)
    {
        const auto& info = beaconInfo.m_tSirenInfo[i];
        out.push_back({
            { "Min_Alpha", info.m_dwMinSirenAlpha },
            { "Red", info.m_RGBBeaconColour.R },
            { "Green", info.m_RGBBeaconColour.G },
            { "Blue", info.m_RGBBeaconColour.B },
            { "Alpha", info.m_RGBBeaconColour.A },
            { "x", info.m_vecSirenPositions.fX },
            { "y", info.m_vecSirenPositions.fY },
            { "z", info.m_vecSirenPositions.fZ }
        });
    }
}

static bool GiveVehicleSirens(CVehicle* pVehicle, unsigned char count, unsigned char type,
    std::optional<bool> b360Flag, std::optional<bool> doLOSCheck, std::optional<bool> useRandomiser, std::optional<bool> sirenSilent)
{
    if (!count)
        return false;

    return CStaticFunctionDefinitions::GiveVehicleSirens(pVehicle, type, count, {
        b360Flag.value_or(false),
        doLOSCheck.value_or(true),
        useRandomiser.value_or(true),
        sirenSilent.value_or(false)
    });
}

static std::variant<std::string_view, bool> GetVehicleName(CVehicle* pVehicle)
{
    std::string_view modelName = CVehicleNames::GetVehicleName(pVehicle->GetModel());
    if (modelName.empty())
        return false;
    return modelName;
}

std::variant<std::string_view, bool> CLuaVehicleDefs::GetVehicleNameFromModel(size_t model)
{
    std::string_view modelName = CVehicleNames::GetVehicleName(model);
    if (modelName.empty())
        return false;
    return modelName;
}

static std::variant<CPed*, bool> GetVehicleOccupant(CVehicle* pVehicle, size_t seat)
{
    if (CPed* occupant = pVehicle->GetOccupant(seat))
        return occupant;
    return false;
}

static std::variant<std::vector<CPed*>, bool> GetVehicleOccupants(CVehicle* pVehicle)
{
    const auto seats = pVehicle->GetMaxPassengers();
    if (seats == 0xff)
        return false; // Vehicle doesn't have seats

    std::vector<CPed*> occupants;
    occupants.reserve(seats);
    for (size_t i = 0u; i < seats; i++)
        if (CPed* ped = pVehicle->GetOccupant(i))
            occupants.emplace_back(ped);
    return occupants;
}

static std::variant<CPed*, bool> GetVehicleController(CVehicle* pVehicle)
{
    if (CPed* controller = pVehicle->GetController())
        return controller;
    return false;
}

static CLuaMultiReturn<float, float, float> GetVehicleRotation(CVehicle* pVehicle)
{
    CVector rot;
    pVehicle->GetRotationDegrees(rot);
    return { rot.fX, rot.fY, rot.fZ };
}

static CLuaMultiReturn<float, float, float> GetVehicleTurnVelocity(CVehicle* pVehicle)
{
    CVector speed = pVehicle->GetTurnSpeed();
    return { speed.fX, speed.fY, speed.fZ };
}

static CVector OOP_GetVehicleTurnVelocity(CVehicle* pVehicle)
{
    return pVehicle->GetTurnSpeed();
}

static bool GetVehicleSirensOn(CVehicle* pVehicle)
{
    // Does the vehicle have sirens?
    if (CVehicleManager::HasSirens(pVehicle->GetModel()) || pVehicle->DoesVehicleHaveSirens())
        return pVehicle->IsSirenActive();
    return false;
}

static bool RemoveVehicleSirens(CVehicle* pVehicle)
{
    pVehicle->m_tSirenBeaconInfo.m_bOverrideSirens = false;
    pVehicle->RemoveVehicleSirens();

    CBitStream BitStream;
    g_pGame->GetPlayerManager()->BroadcastOnlyJoined(CElementRPCPacket(pVehicle, REMOVE_VEHICLE_SIRENS, *BitStream.pBitStream));

    return true;
}

static CLuaMultiReturn<float, float> GetVehicleTurretPosition(CVehicle* pVehicle)
{
    return { pVehicle->GetTurretPositionX(), pVehicle->GetTurretPositionY() };
}

static bool IsVehicleLocked(CVehicle* pVehicle)
{
    return pVehicle->IsLocked();
}

static std::vector<CVehicle*> GetVehiclesOfType(CVehicle* pVehicle, size_t requiredModel)
{
    const auto& list = g_pGame->GetVehicleManager()->GetVehicles();
    std::vector<CVehicle*> matching;
    matching.reserve(list.size()); // Generous, but the vector is short lived, so..
    for (CVehicle* veh : list)
        if (veh->GetModel() == requiredModel)
            matching.push_back(veh);
    return matching;
}

static std::variant<unsigned short, bool> GetVehicleUpgradeOnSlot(CVehicle* pVehicle, unsigned char slot)
{
    if (CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades())
        return pUpgrades->GetSlotState(slot);
    return false;
}

static std::variant<std::string_view, bool> GetVehicleUpgradeSlotName(CVehicle* pVehicle, unsigned char upgradeOrSlot)
{
    if (CVehicleUpgrades::IsValidSlot(upgradeOrSlot))
        return CVehicleUpgrades::GetSlotName(upgradeOrSlot);
    else if (CVehicleUpgrades::IsValidUpgrade(upgradeOrSlot))
    {
        unsigned char slot;
        if (CVehicleUpgrades::GetSlotFromUpgrade(upgradeOrSlot, slot))
            return CVehicleUpgrades::GetSlotName(slot);
    }
    return false;
}

static std::variant<std::vector<unsigned short>, bool> GetVehicleUpgrades(CVehicle* pVehicle)
{
    if (CVehicleUpgrades* upgrades = pVehicle->GetUpgrades())
    {
        std::vector<unsigned short> upgradesOut;
        upgradesOut.reserve(VEHICLE_UPGRADE_SLOTS);
        for (auto state : upgrades->GetSlotStates())
            if (state)
                upgradesOut.push_back(state);
        return upgradesOut;
    }
    return false;
}

static std::variant<std::vector<unsigned short>, bool> GetVehicleCompatibleUpgrades(CVehicle* pVehicle, std::optional<unsigned char> slot)
{
    if (slot && !CVehicleUpgrades::IsValidSlot(*slot))
        return std::vector<unsigned short>{}; // Would return an empty table as per old behaviour

    if (CVehicleUpgrades* upgrades = pVehicle->GetUpgrades())
    {
        std::vector<unsigned short> listOut;
        listOut.reserve(256);
        for (unsigned short upg = 1000; upg <= 1193; upg++)
        {
            if (!upgrades->IsUpgradeCompatible(upg))
                continue;

            if (slot)
            {
                unsigned char upgradeSlot;
                if (!upgrades->GetSlotFromUpgrade(upg, upgradeSlot))
                    continue;

                if (upgradeSlot != *slot)
                    continue;
            }

            listOut.push_back(upg);
        }
    }
    return false;
}

static std::variant<size_t, bool> GetVehicleDoorState(CVehicle* pVehicle, size_t door)
{
    if (auto optState = pVehicle->GetDoorState(door))
        return (size_t)*optState;
    return false;
}

static std::variant<size_t, bool> GetVehicleLightState(CVehicle* pVehicle, size_t light)
{
    if (auto optState = pVehicle->GetLightState(light))
        return (size_t)*optState;
    return false;
}

static std::variant<size_t, bool> GetVehiclePanelState(CVehicle* pVehicle, size_t panel)
{
    if (auto optState = pVehicle->GetPanelState(panel))
        return (size_t)*optState;
    return false;
}


static auto GetVehicleWheelStates(CVehicle* pVehicle)
{
    const auto& states = pVehicle->m_ucWheelStates;
    return CLuaMultiReturn<size_t, size_t, size_t, size_t>{
        states[FRONT_LEFT_WHEEL],
        states[REAR_LEFT_WHEEL],
        states[FRONT_RIGHT_WHEEL],
        states[REAR_RIGHT_WHEEL]
    };
}

static std::variant<CVehicle*, bool> GetVehicleTowedByVehicle(CVehicle* pVehicle)
{
    if (CVehicle* towedVehicle = pVehicle->GetTowedVehicle())
        return towedVehicle;
    return false;
}

static std::variant<CVehicle*, bool> GetVehicleTowingVehicle(CVehicle* pVehicle)
{
    if (CVehicle* towedByVehicle = pVehicle->GetTowedByVehicle())
        return towedByVehicle;
    return false;
}

// TODO: Use the method parser here (ArgumentParserWarn<false, CVehicle::...>)
static size_t GetVehicleOverrideLights(CVehicle* pVehicle)
{
    return pVehicle->GetOverrideLights();
}

static size_t GetVehiclePaintjob(CVehicle* pVehicle)
{
    return pVehicle->GetPaintjob();
}

static std::string_view GetVehiclePlateText(CVehicle* pVehicle)
{
    return pVehicle->GetRegPlate();
}

static bool IsVehicleDamageProof(CVehicle* pVehicle)
{
    return pVehicle->IsDamageProof();
}

static bool IsVehicleFuelTankExplodable(CVehicle* pVehicle)
{
    return pVehicle->IsFuelTankExplodable();
}

static bool IsVehicleFrozen(CVehicle* pVehicle)
{
    return pVehicle->IsFrozen();
}

static bool IsVehicleOnGround(CVehicle* pVehicle)
{
    return pVehicle->IsOnGround();
}

static bool GetVehicleEngineState(CVehicle* pVehicle)
{
    return pVehicle->IsEngineOn();
}

static bool IsVehicleBlown(CVehicle* vehicle)
{
    return vehicle->GetIsBlown();
}

static bool IsVehicleTaxiLightOn(CVehicle* pVehicle)
{
    return pVehicle->IsTaxiLightOn();
}

static bool IsTrainDerailed(CVehicle* pVehicle)
{
    if (pVehicle->GetVehicleType() == VEHICLE_TRAIN)
        return pVehicle->IsDerailed();
    return false;
}

static bool IsTrainDerailable(CVehicle* pVehicle)
{
    if (pVehicle->GetVehicleType() == VEHICLE_TRAIN)
        return pVehicle->IsDerailable();
    return false;
}

static bool GetTrainDirection(CVehicle* pVehicle)
{
    if (pVehicle->GetVehicleType() == VEHICLE_TRAIN)
        return pVehicle->GetTrainDirection();
    return false;
}

static std::variant<float, bool> GetTrainSpeed(CVehicle* pVehicle)
{
    if (pVehicle->GetVehicleType() == VEHICLE_TRAIN)
        return pVehicle->GetVelocity().Length();
    return false;
}

static std::variant<float, bool> GetTrainPosition(CVehicle* pVehicle)
{
    if (pVehicle->GetVehicleType() == VEHICLE_TRAIN && !pVehicle->IsDerailed())
        return pVehicle->GetTrainPosition();
    return false;
}

static std::variant<CTrainTrack*, bool> GetTrainTrack(CVehicle* pVehicle)
{
    if (pVehicle->GetVehicleType() == VEHICLE_TRAIN && !pVehicle->IsDerailed())
        return pVehicle->GetTrainTrack();
    return false;
}

static CLuaMultiReturn<size_t, size_t, size_t> GetVehicleHeadLightColor(CVehicle* pVehicle)
{
    auto color = pVehicle->GetHeadLightColor();
    return { color.R, color.G, color.B };
}

static std::variant<
    CLuaMultiReturn<uchar, uchar, uchar, uchar>,
    CLuaMultiReturn<uchar, uchar, uchar, uchar, uchar, uchar, uchar, uchar, uchar, uchar, uchar, uchar>
>
GetVehicleColor(CVehicle* pVehicle, std::optional<bool> rgb)
{
    auto& colors = pVehicle->GetColor();
    if (rgb.value_or(false))
    {
        // This is worse than Chernobyl
        SColor RGBcolors[] = { colors.GetRGBColor(0), colors.GetRGBColor(1), colors.GetRGBColor(2), colors.GetRGBColor(3) };
        return CLuaMultiReturn<uchar, uchar, uchar, uchar, uchar, uchar, uchar, uchar, uchar, uchar, uchar, uchar>(
            RGBcolors[0].R, RGBcolors[0].G, RGBcolors[0].B,
            RGBcolors[1].R, RGBcolors[1].G, RGBcolors[1].B,
            RGBcolors[2].R, RGBcolors[2].G, RGBcolors[2].B,
            RGBcolors[3].R, RGBcolors[3].G, RGBcolors[3].B
        );
    }
    else
    {
        return CLuaMultiReturn<uchar, uchar, uchar, uchar>(
            colors.GetPaletteColor(0),
            colors.GetPaletteColor(1),
            colors.GetPaletteColor(2),
            colors.GetPaletteColor(3)
        );
    }
}

// Sadly this can't be done in a better way..
// TODO: Refactor this if possible
static bool SetVehicleColor(CElement* pElement,
    uchar c1orr1, uchar c2org1, uchar c3orb1,
    std::optional<uchar> c4orr2, std::optional<uchar> g2, std::optional<uchar> b2,
    std::optional<uchar> r3, std::optional<uchar> g3, std::optional<uchar> b3,
    std::optional<uchar> r4, std::optional<uchar> g4, std::optional<uchar> b4
)
{
    const std::array<std::optional<uchar>, 12> v = {
        c1orr1, c2org1, c3orb1, c4orr2, g2, b2, r3, g3, b3, r4, g4, b4
    };

    // Calculate the number of given optionals, to make our life easier...
    const size_t givenCount = std::distance(v.begin(), std::find_if(v.begin() + 3, v.end(), std::logical_not<>{}));

    if (givenCount == 4) // Palette
        return CStaticFunctionDefinitions::SetVehicleColor(pElement, { *v[0], *v[1], *v[2], *v[3] });
    else if (givenCount % 3 == 0) // RGB colors
    {
        return CStaticFunctionDefinitions::SetVehicleColor(pElement, CVehicleColor{
            SColorRGBA{*v[0], *v[1], *v[2], 0},
            SColorRGBA{v[3].value_or(0), v[4].value_or(0), v[5].value_or(0), 0},
            SColorRGBA{v[6].value_or(0), v[7].value_or(0), v[8].value_or(0), 0},
            SColorRGBA{v[9].value_or(0), v[10].value_or(0), v[11].value_or(0), 0},
        });
    }
    else
        throw std::invalid_argument("Invalid number of color arguments");
}

static bool AddVehicleUpgrade(CElement* pElement, std::variant<unsigned short, std::string_view> upgrade)
{
    if (std::holds_alternative<std::string_view>(upgrade))
    {
        if (std::get<std::string_view>(upgrade) == "all")
            return CStaticFunctionDefinitions::AddAllVehicleUpgrades(pElement);
        else
            throw std::invalid_argument("Invalid upgrade given: must be either a number, or \"all\".");
    }
    else
        return CStaticFunctionDefinitions::AddVehicleUpgrade(pElement, std::get<unsigned short>(upgrade));
}

static bool SetVehicleDoorState(CElement* pElement, unsigned char door, unsigned char state, std::optional<bool> spawnFlying)
{
    return CStaticFunctionDefinitions::SetVehicleDoorState(pElement, door, state, spawnFlying.value_or(true));
}

static bool SetVehicleWheelStates(CElement* pElement, uchar fl, std::optional<uchar> rl, std::optional<uchar> fr, std::optional<uchar> rr)
{
    const auto flopt = fl == -1 ? std::nullopt : std::make_optional(fl);
    return CStaticFunctionDefinitions::SetVehicleWheelStates(pElement, flopt, rl, fr, rr);
}

static CLuaMultiReturn<float, float, float> GetVehicleRespawnRotation(CVehicle* pVehicle)
{
    CVector pos = pVehicle->GetRespawnRotationDegrees();
    return { pos.fX, pos.fY, pos.fZ };
}

static CVector OOP_GetVehicleRespawnRotation(CVehicle* pVehicle)
{
    return pVehicle->GetRespawnRotationDegrees();
}

static CLuaMultiReturn<float, float, float> GetVehicleRespawnPosition(CVehicle* pVehicle)
{
    CVector pos = pVehicle->GetRespawnPosition();
    return { pos.fX, pos.fY, pos.fZ };
}

static CVector OOP_GetVehicleRespawnPosition(CVehicle* pVehicle)
{
    return pVehicle->GetRespawnPosition();
}

static bool SetVehicleRespawnPosition(CElement* pElement, CVector pos, std::optional<CVector> rot)
{
    if (CStaticFunctionDefinitions::SetVehicleRespawnPosition(pElement, pos))
    {
        if (rot)
            return CStaticFunctionDefinitions::SetVehicleRespawnRotation(pElement, *rot);
        return true;
    }
    return false;
}

static bool SpawnVehicle(CElement* pElement, CVector pos, std::optional<CVector> rot)
{
    return CStaticFunctionDefinitions::SpawnVehicle(pElement, pos, rot.value_or(CVector()));
}

static bool DetachTrailerFromVehicle(CVehicle* pVehicle, std::optional<CVehicle*> pTrailer)
{
    return CStaticFunctionDefinitions::DetachTrailerFromVehicle(pVehicle, pTrailer.value_or(nullptr));
}

static bool SetTrainTrack(CVehicle* pVehicle, CTrainTrack* pTrainTrack)
{
    if (pVehicle->GetVehicleType() != VEHICLE_TRAIN || pVehicle->IsDerailed())
        return false;

    // TODO(qaisjp, feature/custom-train-tracks): this needs to support non-default train tracks
    if (!pTrainTrack->IsDefault())
        throw std::invalid_argument("setTrainTrack only supports default train tracks");

    pVehicle->SetTrainTrack(pTrainTrack);

    CBitStream BitStream;
    BitStream.pBitStream->Write(pTrainTrack->GetDefaultTrackId());

    g_pGame->GetPlayerManager()->BroadcastOnlyJoined(CElementRPCPacket(pVehicle, SET_TRAIN_TRACK, *BitStream.pBitStream));

    return true;
}

static bool SetTrainPosition(CVehicle* pVehicle, float fPos)
{
    if (abs(fPos) > LONG_MAX)
        throw std::invalid_argument("Train position supplied is too large");
    return CStaticFunctionDefinitions::SetTrainPosition(pVehicle, fPos);
}

static bool SetVehicleHeadLightColor(CVehicle* pVehicle, unsigned char r, unsigned char g, unsigned char b)
{
    return CStaticFunctionDefinitions::SetVehicleHeadLightColor(pVehicle, SColorRGBA{ r, g, b, 0xFF });
}

static bool SetVehicleDoorOpenRatio(CElement* pElement, unsigned char ucDoor, float fRatio, std::optional<unsigned long> ulTime)
{
    return CStaticFunctionDefinitions::SetVehicleDoorOpenRatio(pElement, ucDoor, fRatio, ulTime.value_or(0));
}

static std::variant<float, bool> GetVehicleDoorOpenRatio(CVehicle* pVehicle, unsigned char door)
{
    if (door <= 5)
        return pVehicle->GetDoorOpenRatio(door);
    return false;
}

static bool SetVehiclePlateText(CElement* pElement, std::string text)
{
    return CStaticFunctionDefinitions::SetVehiclePlateText(pElement, text);
}

void CLuaVehicleDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"createVehicle", CreateVehicle},

        // Vehicle get funcs
        {"getVehicleType", ArgumentParserWarn<false, GetVehicleType>},
        {"getVehicleVariant", ArgumentParserWarn<false, GetVehicleVariant>},
        {"getVehicleColor", ArgumentParserWarn<false, GetVehicleColor>},
        {"getVehicleModelFromName", ArgumentParserWarn<false, GetVehicleModelFromName>},
        {"getVehicleLandingGearDown", ArgumentParserWarn<false, GetVehicleLandingGearDown>},
        {"getVehicleName", ArgumentParserWarn<false, GetVehicleName>},
        {"getVehicleNameFromModel", ArgumentParserWarn<false, GetVehicleNameFromModel>},
        {"getVehicleOccupant", ArgumentParserWarn<false, GetVehicleOccupant>},
        {"getVehicleOccupants", ArgumentParserWarn<false, GetVehicleOccupants>},
        {"getVehicleController", ArgumentParserWarn<false, GetVehicleController>},
        {"getVehicleRotation", ArgumentParserWarn<false, GetVehicleRotation>},
        {"getVehicleSirensOn", ArgumentParserWarn<false, GetVehicleSirensOn>},
        {"getVehicleTurnVelocity", ArgumentParserWarn<false, GetVehicleTurnVelocity>},
        {"getVehicleTurretPosition", ArgumentParserWarn<false, GetVehicleTurretPosition>},
        {"getVehicleMaxPassengers", ArgumentParserWarn<false, GetVehicleMaxPassengers>},
        {"isVehicleLocked", ArgumentParserWarn<false, IsVehicleLocked>},
        {"getVehiclesOfType", ArgumentParserWarn<false, GetVehiclesOfType>},
        {"getVehicleUpgradeOnSlot", ArgumentParserWarn<false, GetVehicleUpgradeOnSlot>},
        {"getVehicleUpgrades", ArgumentParserWarn<false, GetVehicleUpgrades>},
        {"getVehicleUpgradeSlotName", ArgumentParserWarn<false, GetVehicleUpgradeSlotName>},
        {"getVehicleCompatibleUpgrades", ArgumentParserWarn<false, GetVehicleCompatibleUpgrades>},
        {"getVehicleDoorState", ArgumentParserWarn<false, GetVehicleDoorState>},
        {"getVehicleWheelStates", ArgumentParserWarn<false, GetVehicleWheelStates>},
        {"getVehicleLightState", ArgumentParserWarn<false, GetVehicleLightState>},
        {"getVehiclePanelState", ArgumentParserWarn<false, GetVehiclePanelState>},
        {"getVehicleOverrideLights", ArgumentParserWarn<false, GetVehicleOverrideLights>},
        {"getVehicleTowedByVehicle", ArgumentParserWarn<false, GetVehicleTowedByVehicle>},
        {"getVehicleTowingVehicle", ArgumentParserWarn<false, GetVehicleTowingVehicle>},
        {"getVehiclePaintjob", ArgumentParserWarn<false, GetVehiclePaintjob>},
        {"getVehiclePlateText", ArgumentParserWarn<false, GetVehiclePlateText>},
        {"isVehicleDamageProof", ArgumentParserWarn<false, IsVehicleDamageProof>},
        {"isVehicleFuelTankExplodable", ArgumentParserWarn<false, IsVehicleFuelTankExplodable>},
        {"isVehicleFrozen", ArgumentParserWarn<false, IsVehicleFrozen>},
        {"isVehicleOnGround", ArgumentParserWarn<false, IsVehicleOnGround>},
        {"getVehicleEngineState", ArgumentParserWarn<false, GetVehicleEngineState>},
        {"isTrainDerailed", ArgumentParserWarn<false, IsTrainDerailed>},
        {"isTrainDerailable", ArgumentParserWarn<false, IsTrainDerailable>},
        {"getTrainDirection", ArgumentParserWarn<false, GetTrainDirection>},
        {"getTrainSpeed", ArgumentParserWarn<false, GetTrainSpeed>},
        //{"getTrainTrack", ArgumentParserWarn<false, ArgumentParser<GetTrainTrack>>},
        {"getTrainPosition", ArgumentParserWarn<false, GetTrainPosition>},
        {"isVehicleBlown", ArgumentParserWarn<false, IsVehicleBlown>},
        {"getVehicleHeadLightColor", ArgumentParserWarn<false, GetVehicleHeadLightColor>},
        {"getVehicleDoorOpenRatio", ArgumentParserWarn<false, GetVehicleDoorOpenRatio>},
        {"getVehicleRespawnPosition", ArgumentParserWarn<false, GetVehicleRespawnPosition>},
        {"getVehicleRespawnRotation", ArgumentParserWarn<false, GetVehicleRespawnRotation>},

        // Vehicle set funcs
        {"fixVehicle", ArgumentParserWarn<false, CStaticFunctionDefinitions::FixVehicle>},
        {"blowVehicle", ArgumentParserWarn<false, CStaticFunctionDefinitions::BlowVehicle>},
        {"setVehicleRotation", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetVehicleRotation>},
        {"setVehicleTurnVelocity", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetVehicleTurnVelocity>},
        {"setVehicleColor", ArgumentParserWarn<false, SetVehicleColor>},
        {"setVehicleLandingGearDown", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetVehicleLandingGearDown>}, 
        {"setVehicleLocked", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetVehicleLocked>}, 
        {"setVehicleDoorsUndamageable", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetVehicleDoorsUndamageable>}, 
        {"setVehicleSirensOn", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetVehicleSirensOn>}, 
        {"setVehicleTaxiLightOn", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetVehicleTaxiLightOn>},
        {"isVehicleTaxiLightOn", ArgumentParserWarn<false, IsVehicleTaxiLightOn>},
        {"addVehicleUpgrade", ArgumentParserWarn<false, AddVehicleUpgrade>},
        {"removeVehicleUpgrade", ArgumentParserWarn<false, CStaticFunctionDefinitions::RemoveVehicleUpgrade>},
        {"setVehicleDoorState", ArgumentParserWarn<false, SetVehicleDoorState>},
        {"setVehicleWheelStates", ArgumentParserWarn<false, SetVehicleWheelStates>},
        {"setVehicleLightState", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetVehicleLightState>},
        {"setVehiclePanelState", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetVehiclePanelState>},
        {"toggleVehicleRespawn", ArgumentParserWarn<false, CStaticFunctionDefinitions::ToggleVehicleRespawn>},
        {"setVehicleRespawnDelay", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetVehicleRespawnDelay>},
        {"setVehicleIdleRespawnDelay", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetVehicleIdleRespawnDelay>},
        {"setVehicleRespawnPosition", ArgumentParserWarn<false, SetVehicleRespawnPosition>},
        {"setVehicleRespawnRotation", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetVehicleRespawnRotation>},
        {"respawnVehicle", ArgumentParserWarn<false, CStaticFunctionDefinitions::RespawnVehicle>},
        {"resetVehicleExplosionTime", ArgumentParserWarn<false, CStaticFunctionDefinitions::ResetVehicleExplosionTime>},
        {"resetVehicleIdleTime", ArgumentParserWarn<false, CStaticFunctionDefinitions::ResetVehicleIdleTime>},
        {"spawnVehicle", ArgumentParserWarn<false, SpawnVehicle>},
        {"setVehicleOverrideLights", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetVehicleOverrideLights>},
        {"attachTrailerToVehicle", ArgumentParserWarn<false, CStaticFunctionDefinitions::AttachTrailerToVehicle>},
        {"detachTrailerFromVehicle", ArgumentParserWarn<false, DetachTrailerFromVehicle>},
        {"setVehicleEngineState", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetVehicleEngineState>},
        {"setVehicleDirtLevel", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetVehicleDirtLevel>},
        {"setVehicleDamageProof", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetVehicleDamageProof>},
        {"setVehiclePaintjob", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetVehiclePaintjob>},
        {"setVehicleFuelTankExplodable", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetVehicleFuelTankExplodable>},
        {"setVehicleFrozen", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetVehicleFrozen>},
        {"setTrainDerailed", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetTrainDerailed>},
        {"setTrainDerailable", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetTrainDerailable>},
        {"setTrainDirection", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetTrainDirection>},
        {"setTrainSpeed", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetTrainSpeed>},
        //{"setTrainTrack", ArgumentParser<SetTrainTrack>},
        {"setTrainPosition", ArgumentParserWarn<false, SetTrainPosition>},
        {"setVehicleHeadLightColor", ArgumentParserWarn<false, SetVehicleHeadLightColor>},
        {"setVehicleTurretPosition", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetVehicleTurretPosition>},
        {"setVehicleDoorOpenRatio", ArgumentParserWarn<false, SetVehicleDoorOpenRatio>},
        {"setVehicleVariant", ArgumentParserWarn<false, SetVehicleVariant>},
        {"addVehicleSirens", ArgumentParserWarn<false, GiveVehicleSirens>},
        {"removeVehicleSirens", ArgumentParserWarn<false, RemoveVehicleSirens>},
        {"setVehicleSirens", ArgumentParserWarn<false, SetVehicleSirens>},
        {"getVehicleSirens", ArgumentParserWarn<false, GetVehicleSirens>},
        {"getVehicleSirenParams", ArgumentParserWarn<false, GetVehicleSirenParams>},
        {"setVehiclePlateText", ArgumentParserWarn<false, SetVehiclePlateText>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaVehicleDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "getModelFromName", "getVehicleModelFromName");
    lua_classfunction(luaVM, "getNameFromModel", "getVehicleNameFromModel");
    lua_classfunction(luaVM, "getAllOfType", "getVehiclesOfType");
    lua_classfunction(luaVM, "getModelHandling", "getModelHandling");
    lua_classfunction(luaVM, "setModelHandling", "setModelHandling");
    lua_classfunction(luaVM, "getOriginalHandling", "getOriginalHandling");
    lua_classfunction(luaVM, "getUpgradeSlotName", "getVehicleUpgradeSlotName");

    lua_classfunction(luaVM, "create", "createVehicle");
    lua_classfunction(luaVM, "blow", "blowVehicle");
    lua_classfunction(luaVM, "fix", "fixVehicle");
    lua_classfunction(luaVM, "detachTrailer", "detachTrailerFromVehicle");
    lua_classfunction(luaVM, "addUpgrade", "addVehicleUpgrade");
    lua_classfunction(luaVM, "attachTrailer", "attachTrailerToVehicle");
    lua_classfunction(luaVM, "removeUpgrade", "removeVehicleUpgrade");
    lua_classfunction(luaVM, "resetIdleTime", "resetVehicleIdleTime");
    lua_classfunction(luaVM, "resetExplosionTime", "resetVehicleExplosionTime");
    lua_classfunction(luaVM, "respawn", "respawnVehicle");
    lua_classfunction(luaVM, "spawn", "spawnVehicle");
    lua_classfunction(luaVM, "toggleRespawn", "toggleVehicleRespawn");
    lua_classfunction(luaVM, "removeSirens", "removeVehicleSirens");
    lua_classfunction(luaVM, "addSirens", "addVehicleSirens");

    lua_classfunction(luaVM, "isDamageProof", "isVehicleDamageProof");
    lua_classfunction(luaVM, "isFuelTankExplodable", "isVehicleFuelTankExplodable");
    lua_classfunction(luaVM, "isLocked", "isVehicleLocked");
    lua_classfunction(luaVM, "isOnGround", "isVehicleOnGround");
    lua_classfunction(luaVM, "isDerailable", "isTrainDerailable");
    lua_classfunction(luaVM, "isDerailed", "isTrainDerailed");
    lua_classfunction(luaVM, "isBlown", "isVehicleBlown");
    lua_classfunction(luaVM, "isTaxiLightOn", "isVehicleTaxiLightOn");
    lua_classfunction(luaVM, "getSirenParams", "getVehicleSirenParams");
    lua_classfunction(luaVM, "getVariant", "getVehicleVariant");
    lua_classfunction(luaVM, "getSirens", "getVehicleSirens");
    lua_classfunction(luaVM, "getDirection", "getTrainDirection");
    lua_classfunction(luaVM, "getTrainSpeed", "getTrainSpeed");
    //lua_classfunction(luaVM, "getTrack", "getTrainTrack");
    lua_classfunction(luaVM, "getTrainPosition", "getTrainPosition");
    lua_classfunction(luaVM, "getHeadLightColor", "getVehicleHeadLightColor");
    lua_classfunction(luaVM, "getColor", "getVehicleColor");
    lua_classfunction(luaVM, "getCompatibleUpgrades", "getVehicleCompatibleUpgrades");
    lua_classfunction(luaVM, "getController", "getVehicleController");
    lua_classfunction(luaVM, "getDoorState", "getVehicleDoorState");
    lua_classfunction(luaVM, "getEngineState", "getVehicleEngineState");
    lua_classfunction(luaVM, "getLandingGearDown", "getVehicleLandingGearDown");
    lua_classfunction(luaVM, "getLightState", "getVehicleLightState");
    lua_classfunction(luaVM, "getMaxPassengers", "getVehicleMaxPassengers");
    lua_classfunction(luaVM, "getName", "getVehicleName");
    lua_classfunction(luaVM, "getOccupant", "getVehicleOccupant");
    lua_classfunction(luaVM, "getOccupants", "getVehicleOccupants");
    lua_classfunction(luaVM, "getOverrideLights", "getVehicleOverrideLights");
    lua_classfunction(luaVM, "getPlateText", "getVehiclePlateText");
    lua_classfunction(luaVM, "getPaintjob", "getVehiclePaintjob");
    lua_classfunction(luaVM, "getPanelState", "getVehiclePanelState");
    lua_classfunction(luaVM, "areSirensOn", "getVehicleSirensOn");
    lua_classfunction(luaVM, "getTowedByVehicle", "getVehicleTowedByVehicle");
    lua_classfunction(luaVM, "getTowingVehicle", "getVehicleTowingVehicle");
    lua_classfunction(luaVM, "getTurnVelocity", "getVehicleTurnVelocity", ArgumentParserWarn<false, OOP_GetVehicleTurnVelocity>);
    lua_classfunction(luaVM, "getTurretPosition", "getVehicleTurretPosition");
    lua_classfunction(luaVM, "getVehicleType", "getVehicleType");            // This isn't "getType" because it would overwrite Element.getType
    lua_classfunction(luaVM, "getUpgradeOnSlot", "getVehicleUpgradeOnSlot");
    lua_classfunction(luaVM, "getUpgrades", "getVehicleUpgrades");
    lua_classfunction(luaVM, "getWheelStates", "getVehicleWheelStates");
    lua_classfunction(luaVM, "getDoorOpenRatio", "getVehicleDoorOpenRatio");
    lua_classfunction(luaVM, "getHandling", "getVehicleHandling");
    lua_classfunction(luaVM, "getRespawnPosition", "getVehicleRespawnPosition");
    lua_classfunction(luaVM, "getRespawnRotation", "getVehicleRespawnRotation");

    lua_classfunction(luaVM, "setColor", "setVehicleColor");
    lua_classfunction(luaVM, "setDamageProof", "setVehicleDamageProof");
    lua_classfunction(luaVM, "setDoorState", "setVehicleDoorState");
    lua_classfunction(luaVM, "setDoorsUndamageable", "setVehicleDoorsUndamageable");
    lua_classfunction(luaVM, "setEngineState", "setVehicleEngineState");
    lua_classfunction(luaVM, "setFuelTankExplodable", "setVehicleFuelTankExplodable");
    lua_classfunction(luaVM, "setIdleRespawnDelay", "setVehicleIdleRespawnDelay");
    lua_classfunction(luaVM, "setLandingGearDown", "setVehicleLandingGearDown");
    lua_classfunction(luaVM, "setLightState", "setVehicleLightState");
    lua_classfunction(luaVM, "setLocked", "setVehicleLocked");
    lua_classfunction(luaVM, "setOverrideLights", "setVehicleOverrideLights");
    lua_classfunction(luaVM, "setPaintjob", "setVehiclePaintjob");
    lua_classfunction(luaVM, "setPanelState", "setVehiclePanelState");
    lua_classfunction(luaVM, "setRespawnDelay", "setVehicleRespawnDelay");
    lua_classfunction(luaVM, "setRespawnPosition", "setVehicleRespawnPosition");
    lua_classfunction(luaVM, "setRespawnRotation", "setVehicleRespawnRotation");
    lua_classfunction(luaVM, "setSirensOn", "setVehicleSirensOn");
    lua_classfunction(luaVM, "setTurretPosition", "setVehicleTurretPosition");
    lua_classfunction(luaVM, "setDoorOpenRatio", "setVehicleDoorOpenRatio");
    lua_classfunction(luaVM, "setHandling", "setVehicleHandling");
    lua_classfunction(luaVM, "setTurnVelocity", "setVehicleTurnVelocity");
    lua_classfunction(luaVM, "setWheelStates", "setVehicleWheelStates");
    lua_classfunction(luaVM, "setHeadLightColor", "setVehicleHeadLightColor");
    lua_classfunction(luaVM, "setTaxiLightOn", "setVehicleTaxiLightOn");
    lua_classfunction(luaVM, "setVariant", "setVehicleVariant");
    lua_classfunction(luaVM, "setSirens", "setVehicleSirens");
    lua_classfunction(luaVM, "setPlateText", "setVehiclePlateText");
    lua_classfunction(luaVM, "setDerailable", "setTrainDerailable");
    lua_classfunction(luaVM, "setDerailed", "setTrainDerailed");
    lua_classfunction(luaVM, "setDirection", "setTrainDirection");
    //lua_classfunction(luaVM, "setTrack", "setTrainTrack");
    lua_classfunction(luaVM, "setTrainPosition", "setTrainPosition");
    lua_classfunction(luaVM, "setTrainSpeed", "setTrainSpeed");            // Reduce confusion

    lua_classvariable(luaVM, "damageProof", "setVehicleDamageProof", "isVehicleDamageProof");
    lua_classvariable(luaVM, "locked", "setVehicleLocked", "isVehicleLocked");
    lua_classvariable(luaVM, "derailable", "setTrainDerailable", "isTrainDerailable");
    lua_classvariable(luaVM, "derailed", "setTrainDerailed", "isTrainDerailed");
    lua_classvariable(luaVM, "blown", "blowVehicle", "isVehicleBlown");
    lua_classvariable(luaVM, "direction", "setTrainDirection", "getTrainDirection");
    lua_classvariable(luaVM, "trainSpeed", "setTrainSpeed", "getTrainSpeed");
    //lua_classvariable(luaVM, "track", "setTrainTrack", "getTrainTrack");
    lua_classvariable(luaVM, "trainPosition", "setTrainPosition", "getTrainPosition");
    lua_classvariable(luaVM, "taxiLightOn", "setVehicleTaxiLightOn", "isVehicleTaxiLightOn");
    lua_classvariable(luaVM, "fuelTankExplodable", "setVehicleFuelTankExplodable", "isVehicleFuelTankExplodable");
    lua_classvariable(luaVM, "plateText", "setVehiclePlateText", "getVehiclePlateText");
    lua_classvariable(luaVM, "sirensOn", "setVehicleSirensOn", "getVehicleSirensOn");
    lua_classvariable(luaVM, "sirenParams", NULL, "getVehicleSirenParams");
    lua_classvariable(luaVM, "controller", NULL, "getVehicleController");
    lua_classvariable(luaVM, "engineState", "setVehicleEngineState", "getVehicleEngineState");
    lua_classvariable(luaVM, "paintjob", "setVehiclePaintjob", "getVehiclePaintjob");
    lua_classvariable(luaVM, "occupants", NULL, "getVehicleOccupants");
    lua_classvariable(luaVM, "compatibleUpgrades", NULL, "getVehicleCompatibleUpgrades");
    lua_classvariable(luaVM, "doorsUndamageable", "setVehicleDoorsUndamageable", NULL);
    lua_classvariable(luaVM, "towingVehicle", NULL, "getVehicleTowingVehicle");
    lua_classvariable(luaVM, "towedByVehicle", NULL, "getVehicleTowedByVehicle");
    lua_classvariable(luaVM, "landingGearDown", "setVehicleLandingGearDown", "getVehicleLandingGearDown");
    lua_classvariable(luaVM, "maxPassengers", NULL, "getVehicleMaxPassengers");
    lua_classvariable(luaVM, "upgrades", NULL, "getVehicleUpgrades");
    lua_classvariable(luaVM, "turretPosition", "setVehicleTurretPosition", "getVehicleTurretPosition");
    lua_classvariable(luaVM, "turnVelocity", "setVehicleTurnVelocity", "getVehicleTurnVelocity", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetVehicleTurnVelocity>, ArgumentParserWarn<false, OOP_GetVehicleTurnVelocity>);
    lua_classvariable(luaVM, "overrideLights", "setVehicleOverrideLights", "getVehicleOverrideLights");
    lua_classvariable(luaVM, "idleRespawnDelay", "setVehicleIdleRespawnDelay", NULL);
    lua_classvariable(luaVM, "respawnDelay", "setVehicleRespawnDelay", NULL);
    lua_classvariable(luaVM, "respawnPosition", "setVehicleRespawnPosition", "getVehicleRespawnPosition", ArgumentParserWarn<false, SetVehicleRespawnPosition>, ArgumentParserWarn<false, OOP_GetVehicleRespawnPosition>);
    lua_classvariable(luaVM, "respawnRotation", "setVehicleRespawnRotation", "getVehicleRespawnRotation", ArgumentParserWarn<false, CStaticFunctionDefinitions::SetVehicleRespawnRotation>, ArgumentParserWarn<false, OOP_GetVehicleRespawnRotation>);
    lua_classvariable(luaVM, "onGround", NULL, "isVehicleOnGround");
    lua_classvariable(luaVM, "name", NULL, "getVehicleName");
    lua_classvariable(luaVM, "vehicleType", NULL, "getVehicleType");
    lua_classvariable(luaVM, "sirens", NULL, "getVehicleSirens");
    lua_classvariable(luaVM, "handling", nullptr, "getVehicleHandling");
    lua_classvariable(luaVM, "occupant", NULL, "getVehicleOccupant");

    lua_registerclass(luaVM, "Vehicle", "Element");
}
