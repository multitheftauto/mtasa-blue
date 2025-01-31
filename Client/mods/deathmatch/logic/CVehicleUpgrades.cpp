/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CVehicleUpgrades.cpp
 *  PURPOSE:     Vehicle upgrades class
 *
 *****************************************************************************/

#include <StdInc.h>
#include <unordered_map>
#include <vector>

#define VEHICLE_UPGRADE_SLOTS 17

#define NUM_VEHICLE_MODELS 212


char szUpgradeNameEmpty[] = "";

struct SUpgradeName
{
    const char* const szName;
};

static const SFixedArray<SUpgradeName, 17> UpgradeNames = {{{"Hood"},
                                                            {"Vent"},
                                                            {"Spoiler"},
                                                            {"Sideskirt"},
                                                            {"Front Bullbars"},
                                                            {"Rear Bullbars"},
                                                            {"Headlights"},
                                                            {"Roof"},
                                                            {"Nitro"},
                                                            {"Hydraulics"},
                                                            {"Stereo"},
                                                            {"Unknown"},
                                                            {"Wheels"},
                                                            {"Exhaust"},
                                                            {"Front Bumper"},
                                                            {"Rear Bumper"},
                                                            {"Misc"}}};

// Define a structure to represent vehicle upgrades

using VehicleUpgradeSlots = std::array<uint32_t, 7>; // 224 bits
static const std::array<VehicleUpgradeSlots, NUM_VEHICLE_MODELS> vehicleUpgradeSlots = {{
    /*400=*/{0x33c2700, 0x0, 0xfffe00, 0x7},
    /*401=*/{0x29be7ff, 0x14060000, 0xfffe00, 0x7, 0x400dcc00, 0x1c},
    /*402=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*403=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*404=*/{0x2bbe78f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
    /*405=*/{0x2bdc70f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
    /*406=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*407=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*408=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*409=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*410=*/{0x3bbe78f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
    /*411=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*412=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*413=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*414=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*415=*/{0x28fc78f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
    /*416=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*417=*/{0x0, 0x0, 0x400000},
    /*418=*/{0x2b1c74f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
    /*419=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*420=*/{0x2a9c73f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
    /*421=*/{0x2bdc70f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
    /*422=*/{0x23a0780, 0x0, 0xfffe00, 0x7},
    /*423=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*424=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*425=*/{0x0, 0x0, 0x400000},
    /*426=*/{0x2a9c77f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
    /*427=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*428=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*429=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*430=*/{},
    /*431=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*432=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*433=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*434=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*435=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*436=*/{0x2fbe7cf, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
    /*437=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*438=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*439=*/{0x283e78f, 0x14060000, 0xfffe00, 0x7, 0x400fcc00, 0x1c},
    /*440=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*441=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*442=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*443=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*444=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*445=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*446=*/{},
    /*447=*/{0x0, 0x0, 0x400000},
    /*448=*/{0x0, 0x0, 0x400000},
    /*449=*/{},
    /*450=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*451=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*452=*/{},
    /*453=*/{},
    /*454=*/{},
    /*455=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*456=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*457=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*458=*/{0x281c70f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
    /*459=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*460=*/{0x2000000, 0x0, 0x7ffe00, 0x7},
    /*461=*/{0x0, 0x0, 0x400000},
    /*462=*/{0x0, 0x0, 0x400000},
    /*463=*/{0x0, 0x0, 0x400000},
    /*464=*/{0x2000000, 0x0, 0x7ffe00, 0x7},
    /*465=*/{0x0, 0x0, 0x400000},
    /*466=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*467=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*468=*/{0x0, 0x0, 0x400000},
    /*469=*/{0x0, 0x0, 0x400000},
    /*470=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*471=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*472=*/{},
    /*473=*/{},
    /*474=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*475=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*476=*/{0x2000000, 0x0, 0x7ffe00, 0x7},
    /*477=*/{0x23e07c0, 0x0, 0xfffe00, 0x7},
    /*478=*/{0x2701730, 0x0, 0xfffe00, 0x7},
    /*479=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*480=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*481=*/{0x0, 0x0, 0x400000},
    /*482=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*483=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*484=*/{},
    /*485=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*486=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*487=*/{0x0, 0x0, 0x400000},
    /*488=*/{0x0, 0x0, 0x400000},
    /*489=*/{0x39de77f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
    /*490=*/{0x281c70f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
    /*491=*/{0x2bfc78f, 0x14060000, 0xfffe00, 0x7, 0x400fcc00, 0x1c},
    /*492=*/{0x281c77f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
    /*493=*/{},
    /*494=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*495=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*496=*/{0x29bcfcf, 0x14060000, 0xfffe00, 0x7, 0x400ccc00, 0x1c},
    /*497=*/{0x0, 0x0, 0x400000},
    /*498=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*499=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*500=*/{0x2380700, 0x0, 0xfffe00, 0x7},
    /*501=*/{0x0, 0x0, 0x400000},
    /*502=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*503=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*504=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*505=*/{0x281c70f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
    /*506=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*507=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*508=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*509=*/{0x0, 0x0, 0x400000},
    /*510=*/{0x0, 0x0, 0x400000},
    /*511=*/{0x2000000, 0x0, 0x7ffe00, 0x7},
    /*512=*/{0x2000000, 0x0, 0x7ffe00, 0x7},
    /*513=*/{0x2000000, 0x0, 0x7ffe00, 0x7},
    /*514=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*515=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*516=*/{0x2bfc79f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
    /*517=*/{0x29fc78f, 0x14060000, 0xfffe00, 0x7, 0x400fcc00, 0x1c},
    /*518=*/{0x297e7ef, 0x14060000, 0xfffe00, 0x7, 0x400fcc00, 0x1c},
    /*519=*/{0x2000000, 0x0, 0x7ffe00, 0x7},
    /*520=*/{0x2000000, 0x0, 0x7ffe00, 0x7},
    /*521=*/{0x0, 0x0, 0x400000},
    /*522=*/{0x0, 0x0, 0x400000},
    /*523=*/{0x0, 0x0, 0x400000},
    /*524=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*525=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*526=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*527=*/{0x2b7c78f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
    /*528=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*529=*/{0x29fdfcf, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
    /*530=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*531=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*532=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*533=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*534=*/{0x2000700, 0x0, 0xfffe00, 0x7fffffff, 0x0, 0x21c0000},
    /*535=*/{0x2000700, 0x0, 0xfffe00, 0x3fe6007},
    /*536=*/{0x2000700, 0x0, 0xfffe00, 0x1b87, 0x1, 0x1e00000},
    /*537=*/{},
    /*538=*/{},
    /*539=*/{0x0, 0x0, 0x400000},
    /*540=*/{0x39fc7df, 0x14060000, 0xfffe00, 0x7, 0x400fcc00, 0x1c},
    /*541=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*542=*/{0x2bdc70f, 0x14060000, 0xfffe00, 0x7, 0x400f0c00, 0x1c},
    /*543=*/{0x281c70f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
    /*544=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*545=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*546=*/{0x38fc7df, 0x14060000, 0xfffe00, 0x7, 0x400fcc00, 0x1c},
    /*547=*/{0x2bdc70f, 0x14060000, 0xfffe00, 0x7, 0x400ccc00, 0x1c},
    /*548=*/{0x0, 0x0, 0x400000},
    /*549=*/{0x29fdf8f, 0x14060000, 0xfffe00, 0x7, 0x400fcc00, 0x1c},
    /*550=*/{0x29dc77f, 0x14060000, 0xfffe00, 0x7, 0x400fcc00, 0x1c},
    /*551=*/{0x2bdc76f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
    /*552=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*553=*/{0x2000000, 0x0, 0x7ffe00, 0x7},
    /*554=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*555=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*556=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*557=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*558=*/{0x281c70f, 0x14060000, 0x7fffffff, 0x7, 0x400c0c00, 0x1fc},
    /*559=*/{0x281c70f, 0x14060000, 0xfffffe, 0x7, 0x7fffffff, 0x201f},
    /*560=*/{0x7fffffff, 0x14060003, 0xfffe00, 0x7, 0x400c3c00, 0x61c},
    /*561=*/{0x281c70f, 0x7fffffff, 0xfffe01, 0x7, 0x7c0c0c00, 0x1c},
    /*562=*/{0x281c70f, 0x140603fc, 0xfffe00, 0x7, 0x403c0c00, 0x181c},
    /*563=*/{0x0, 0x0, 0x400000},
    /*564=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*565=*/{0x281c70f, 0x147fe000, 0xfffe00, 0x7, 0x43cc0c00, 0x1c},
    /*566=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*567=*/{0x2000700, 0x0, 0xfffe00, 0x47, 0x3e, 0x3c000000},
    /*568=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*569=*/{},
    /*570=*/{},
    /*571=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*572=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*573=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*574=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*575=*/{0x2000700, 0x1c00, 0xfffe00, 0xf, 0x0, 0x3c000},
    /*576=*/{0x2000700, 0x0, 0xfffe00, 0x7, 0x3c0, 0x7fffffff, 0x3},
    /*577=*/{0x2000000, 0x0, 0x7ffe00, 0x7},
    /*578=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*579=*/{0x281c70f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
    /*580=*/{0x297c7cf, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
    /*581=*/{0x0, 0x0, 0x400000},
    /*582=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*583=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*584=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*585=*/{0x29fe7cf, 0x14060000, 0xfffe00, 0x7, 0x400fcc00, 0x1c},
    /*586=*/{0x0, 0x0, 0x400000},
    /*587=*/{0x281c70f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
    /*588=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*589=*/{0x297c7ff, 0x14060000, 0xfffe00, 0x7, 0x400f0c00, 0x1c},
    /*590=*/{},
    /*591=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*592=*/{0x2000000, 0x0, 0x7ffe00, 0x7},
    /*593=*/{0x2000000, 0x0, 0x7ffe00, 0x7},
    /*594=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*595=*/{},
    /*596=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*597=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*598=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*599=*/{0x281c70f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
    /*600=*/{0x25627f0, 0x0, 0xfffe00, 0x7},
    /*601=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*602=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*603=*/{0x29fc7cf, 0x14060000, 0xfffe00, 0x7, 0x400fcc00, 0x1c},
    /*604=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*605=*/{0x281c70f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
    /*606=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*607=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*608=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*609=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*610=*/{0x2000700, 0x0, 0xfffe00, 0x7},
    /*611=*/{0x2000700, 0x0, 0xfffe00, 0x7},
}};

CVehicleUpgrades::CVehicleUpgrades(CClientVehicle* pVehicle)
{
    m_pVehicle = pVehicle;
    memset(&m_SlotStates[0], 0, sizeof(m_SlotStates));
    m_usLastLocalAddNitroType = 0;
}

bool CVehicleUpgrades::IsUpgrade(unsigned short usModel)
{
    return (usModel >= 1000 && usModel <= 1193);
}

bool CVehicleUpgrades::IsUpgradeCompatible(unsigned short usUpgrade)
{
    unsigned short     us = usUpgrade;
    eClientVehicleType vehicleType = m_pVehicle->GetVehicleType();

    // No upgrades for trains/boats
    if (vehicleType == CLIENTVEHICLE_TRAIN || vehicleType == CLIENTVEHICLE_BOAT)
        return false;

    // In MTA every vehicle has a radio...
    if (us == 1086)
        return true;

    if (vehicleType == CLIENTVEHICLE_BIKE || vehicleType == CLIENTVEHICLE_BMX || vehicleType == CLIENTVEHICLE_HELI)
        return false;

    unsigned short usModel = m_pVehicle->GetModel();

    // No nitro or other upgrades for planes
    if (vehicleType == CLIENTVEHICLE_PLANE)
        return false;

    bool bReturn = false;
    int modelRequestIDParent = g_pGame->GetModelInfo(usModel)->GetParentID();

   if (auto* pModelInfo = g_pGame->GetModelInfo(usModel); pModelInfo && pModelInfo->GetParentID() != 0)
    usModel = static_cast<uint16_t>(pModelInfo->GetParentID());

    size_t vehicleIndex = (usModel - 400);
    size_t upgradeIndex = (usUpgrade - 1000);

    const VehicleUpgradeSlots& slots = vehicleUpgradeSlots[vehicleIndex];
    size_t                     slotIndex = upgradeIndex / 32;
    uint32_t                   slot = slots[slotIndex];
    size_t                     upgradeBit = (upgradeIndex - (slotIndex * 32));
    return slot & (1 << upgradeBit);
}


bool CVehicleUpgrades::GetSlotFromUpgrade(unsigned short us, unsigned char& ucSlot)
{
    if (us == 1011 || us == 1012 || us == 1111 || us == 1112 || us == 1142 || /* bonet */
        us == 1143 || us == 1144 || us == 1145)
    {
        ucSlot = 0;
        return true;
    }
    if (us == 1004 || us == 1005 || us == 1142 || us == 1143 || us == 1144 || us == 1145) /* vent */
    {
        ucSlot = 1;
        return true;
    }
    if (us == 1000 || us == 1001 || us == 1002 || us == 1003 || us == 1014 || /* spoiler */
        us == 1015 || us == 1016 || us == 1023 || us == 1049 || us == 1050 || us == 1058 || us == 1060 || us == 1138 || us == 1139 || us == 1146 ||
        us == 1147 || us == 1158 || us == 1162 || us == 1163 || us == 1164)
    {
        ucSlot = 2;
        return true;
    }
    if (us == 1007 || us == 1017 || us == 1026 || us == 1027 || us == 1030 || /* sideskirt */
        us == 1031 || us == 1036 || us == 1039 || us == 1040 || us == 1041 || us == 1042 || us == 1047 || us == 1048 || us == 1051 || us == 1052 ||
        us == 1056 || us == 1057 || us == 1062 || us == 1063 || us == 1069 || us == 1070 || us == 1071 || us == 1072 || us == 1090 || us == 1093 ||
        us == 1094 || us == 1095 || us == 1099 || us == 1101 || us == 1102 || us == 1106 || us == 1107 || us == 1108 || us == 1118 || us == 1119 ||
        us == 1120 || us == 1121 || us == 1122 || us == 1124 || us == 1133 || us == 1134 || us == 1137)
    {
        ucSlot = 3;
        return true;
    }
    if (us == 1115 || us == 1116)            // front bullbars
    {
        ucSlot = 4;
        return true;
    }
    if (us == 1109 || us == 1110)            // rear bullbars
    {
        ucSlot = 5;
        return true;
    }
    if (us == 1013 || us == 1024)            // lamps
    {
        ucSlot = 6;
        return true;
    }
    if (us == 1006 || us == 1032 || us == 1033 || us == 1035 || us == 1038 || /* roof */
        us == 1053 || us == 1054 || us == 1055 || us == 1061 || us == 1067 || us == 1068 || us == 1088 || us == 1091 || us == 1103 || us == 1128 ||
        us == 1130 || us == 1131)
    {
        ucSlot = 7;
        return true;
    }
    if (us == 1008 || us == 1009 || us == 1010)            // nitro
    {
        ucSlot = 8;
        return true;
    }
    if (us == 1087)            // hydraulics
    {
        ucSlot = 9;
        return true;
    }
    if (us == 1086)            // stereo
    {
        ucSlot = 10;
        return true;
    }
    if (us == 1025 || us == 1073 || us == 1074 || us == 1075 || us == 1076 || /* wheels */
        us == 1077 || us == 1078 || us == 1079 || us == 1080 || us == 1081 || us == 1082 || us == 1083 || us == 1084 || us == 1085 || us == 1096 ||
        us == 1097 || us == 1098)
    {
        ucSlot = 12;
        return true;
    }
    if (us == 1018 || us == 1019 || us == 1020 || us == 1021 || us == 1022 || /* exhaust */
        us == 1028 || us == 1029 || us == 1034 || us == 1037 || us == 1043 || us == 1044 || us == 1045 || us == 1046 || us == 1059 || us == 1064 ||
        us == 1065 || us == 1066 || us == 1089 || us == 1092 || us == 1104 || us == 1105 || us == 1113 || us == 1114 || us == 1126 || us == 1127 ||
        us == 1129 || us == 1132 || us == 1135 || us == 1136)
    {
        ucSlot = 13;
        return true;
    }
    if (us == 1117 || us == 1152 || us == 1153 || us == 1155 || us == 1157 || /* front bumper */
        us == 1160 || us == 1165 || us == 1166 || us == 1169 || us == 1170 || us == 1171 || us == 1172 || us == 1173 || us == 1174 || us == 1175 ||
        us == 1179 || us == 1181 || us == 1182 || us == 1185 || us == 1188 || us == 1189 || us == 1190 || us == 1191)
    {
        ucSlot = 14;
        return true;
    }
    if (us == 1140 || us == 1141 || us == 1148 || us == 1149 || us == 1150 || /* rear bumper */
        us == 1151 || us == 1154 || us == 1156 || us == 1159 || us == 1161 || us == 1167 || us == 1168 || us == 1176 || us == 1177 || us == 1178 ||
        us == 1180 || us == 1183 || us == 1184 || us == 1186 || us == 1187 || us == 1192 || us == 1193)
    {
        ucSlot = 15;
        return true;
    }

    if (us == 1100 || us == 1123 || us == 1125) /* misc */
    {
        ucSlot = 16;
        return true;
    }

    return false;
}

bool CVehicleUpgrades::AddUpgrade(unsigned short usUpgrade, bool bAddedLocally)
{
    if (m_pVehicle)
    {
        // If its a compatible upgrade
        if (IsUpgradeCompatible(usUpgrade))
        {
            // If upgrade is nitro, and the same one was added locally less than a 2 seconds ago, don't add
            if (usUpgrade == VEHICLEUPGRADE_NITRO_5X || usUpgrade == VEHICLEUPGRADE_NITRO_2X || usUpgrade == VEHICLEUPGRADE_NITRO_10X)
            {
                if (bAddedLocally)
                {
                    m_usLastLocalAddNitroType = usUpgrade;
                    m_lastLocalAddNitroTimer.Reset();
                }
                else
                {
                    if (HasUpgrade(usUpgrade) && usUpgrade == m_usLastLocalAddNitroType)
                    {
                        if (m_lastLocalAddNitroTimer.Get() < 2000)
                            return true;
                    }
                }
            }

            ForceAddUpgrade(usUpgrade);

            return true;
        }
    }
    return false;
}

void CVehicleUpgrades::AddAllUpgrades()
{
    if (m_pVehicle)
    {
        unsigned short usUpgrade = 1000;
        for (; usUpgrade <= 1193; usUpgrade++)
        {
            if (IsUpgradeCompatible(usUpgrade))
            {
                ForceAddUpgrade(usUpgrade);
            }
        }
    }
}

void CVehicleUpgrades::ForceAddUpgrade(unsigned short usUpgrade)
{
    unsigned char ucSlot;
    if (GetSlotFromUpgrade(usUpgrade, ucSlot))
    {
        CVehicle* pVehicle = m_pVehicle->GetGameVehicle();
        if (pVehicle)
        {
            // Grab the upgrade model
            CModelInfo* pModelInfo = g_pGame->GetModelInfo(usUpgrade);
            if (pModelInfo)
            {
                if (!g_pGame->IsASyncLoadingEnabled() || !pModelInfo->IsLoaded())
                {
                    // Request and load now
                    pModelInfo->Request(BLOCKING, "CVehicleUpgrades::ForceAddUpgrade");
                }
                // Add the upgrade
                pVehicle->AddVehicleUpgrade(usUpgrade);
            }
        }

        // Add it to the slot
        m_SlotStates[ucSlot] = usUpgrade;

        // Reset wheel scale if it is a wheel upgrade
        if (ucSlot == 12)
            m_pVehicle->ResetWheelScale();
    }
}

// Not really a restream.
// Call after changing from/to a custom model to see the result immediately
void CVehicleUpgrades::RestreamVehicleUpgrades(unsigned short usUpgrade)
{
    unsigned char ucSlot;
    if (GetSlotFromUpgrade(usUpgrade, ucSlot))
    {
        if (m_SlotStates[ucSlot] == usUpgrade)
            ForceAddUpgrade(usUpgrade);
    }
}

bool CVehicleUpgrades::HasUpgrade(unsigned short usUpgrade)
{
    unsigned char ucSlot = 0;
    for (; ucSlot < VEHICLE_UPGRADE_SLOTS; ucSlot++)
    {
        if (m_SlotStates[ucSlot] == usUpgrade)
            return true;
    }

    return false;
}

bool CVehicleUpgrades::RemoveUpgrade(unsigned short usUpgrade)
{
    if (HasUpgrade(usUpgrade))
    {
        if (m_pVehicle)
        {
            CVehicle* pVehicle = m_pVehicle->GetGameVehicle();
            if (pVehicle)
            {
                pVehicle->RemoveVehicleUpgrade(usUpgrade);
            }

            unsigned char ucSlot;
            if (GetSlotFromUpgrade(usUpgrade, ucSlot))
            {
                m_SlotStates[ucSlot] = 0;
            }

            // Reset wheel scale if it is a wheel upgrade
            if (ucSlot == 12)
                m_pVehicle->ResetWheelScale();

            return true;
        }
    }

    return false;
}

unsigned short CVehicleUpgrades::GetSlotState(unsigned char ucSlot)
{
    if (ucSlot < VEHICLE_UPGRADE_SLOTS)
        return m_SlotStates[ucSlot];

    return 0;
}

void CVehicleUpgrades::ReAddAll()
{
    unsigned char ucSlot = 0;
    for (; ucSlot < VEHICLE_UPGRADE_SLOTS; ucSlot++)
    {
        if (m_SlotStates[ucSlot])
        {
            if (IsUpgradeCompatible(m_SlotStates[ucSlot]))
            {
                ForceAddUpgrade(m_SlotStates[ucSlot]);
            }
            else
            {
                // Not compatible with the mod they are using so just ignore it
                m_SlotStates[ucSlot] = 0;
            }
        }
    }
}

void CVehicleUpgrades::RemoveAll(bool bRipFromVehicle)
{
    unsigned char ucSlot = 0;
    for (; ucSlot < VEHICLE_UPGRADE_SLOTS; ucSlot++)
    {
        if (m_SlotStates[ucSlot])
        {
            if (bRipFromVehicle)
            {
                if (m_pVehicle)
                {
                    CVehicle* pVehicle = m_pVehicle->GetGameVehicle();
                    if (pVehicle)
                    {
                        pVehicle->RemoveVehicleUpgrade(m_SlotStates[ucSlot]);
                    }
                }
            }
            m_SlotStates[ucSlot] = 0;

            // Reset wheel scale for wheel upgrades
            if (ucSlot == 12 && m_pVehicle)
                m_pVehicle->ResetWheelScale();
        }
    }
}

const char* CVehicleUpgrades::GetSlotName(unsigned char ucSlot)
{
    if (ucSlot < VEHICLE_UPGRADE_SLOTS)
        return UpgradeNames[ucSlot].szName;

    return szUpgradeNameEmpty;
}
