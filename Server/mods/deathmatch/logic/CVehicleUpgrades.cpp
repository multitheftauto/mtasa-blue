/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CVehicleUpgrades.cpp
 *  PURPOSE:     GTA vehicle upgrade definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CVehicleUpgrades.h"

char szUpgradeNameEmpty[] = "";

struct SUpgradeName
{
    const char* szName;
};

static const SFixedArray<SUpgradeName, VEHICLE_UPGRADE_SLOTS> UpgradeNames = {{{"Hood"},
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

CVehicleUpgrades::CVehicleUpgrades(CVehicle* pVehicle)
{
    m_pVehicle = pVehicle;
    memset(&m_SlotStates[0], 0, sizeof(m_SlotStates));
}

CVehicleUpgrades::CVehicleUpgrades(CVehicle* pVehicle, CVehicleUpgrades* pUpgrades)
{
    m_pVehicle = pVehicle;
    memset(&m_SlotStates[0], 0, sizeof(m_SlotStates));
    if (pUpgrades)
    {
        m_SlotStates = pUpgrades->GetSlotStates();
    }
}

// Used for CLuaVehicleDefs::GetVehicleCompatibleUpgrades
bool CVehicleUpgrades::IsStaticUpgradeCompatible(unsigned short usUpgrade)
{

    // Vehicle
    eVehicleType   vehicleType = m_pVehicle->GetVehicleType();
    unsigned short usModel = m_pVehicle->GetModel();

    // No upgrades for trains/boats
    if (vehicleType == VEHICLE_TRAIN || vehicleType == VEHICLE_BOAT)
        return false;

    // In MTA every vehicle has a radio...
    if (usUpgrade == 1086)
        return true;

    // Check vehicle model range
    if (usModel < 400 || usModel > 611)
        return false;

    // Check upgrade id range
    if (usUpgrade < 1000 || usUpgrade > 1193)
        return false;

    size_t vehicleIndex = (usModel - 400);
    size_t upgradeIndex = (usUpgrade - 1000);

    const VehicleUpgradeSlots& slots = vehicleUpgradeSlots[vehicleIndex];
    size_t                     slotIndex = upgradeIndex / 32;
    uint32_t                   slot = slots[slotIndex];
    size_t                     upgradeBit = (upgradeIndex - (slotIndex * 32));
    return slot & (1 << upgradeBit);

}

bool CVehicleUpgrades::IsUpgradeCompatible(unsigned short usUpgrade)
{
    unsigned short us = usUpgrade;
    eVehicleType   vehicleType = m_pVehicle->GetVehicleType();

    // No upgrades for trains/boats
    if (vehicleType == VEHICLE_TRAIN || vehicleType == VEHICLE_BOAT)
        return false;

    // In MTA every vehicle has a radio...
    if (us == 1086)
        return true;

    if (vehicleType == VEHICLE_BIKE || vehicleType == VEHICLE_BMX || vehicleType == VEHICLE_HELI)
        return false;

    unsigned short usModel = m_pVehicle->GetModel();
    // Wheels should be compatible with any vehicle which have wheels, except
    // bike/bmx (they're buggy). Vortex is technically a car, but it has no
    // wheels.
    if ((us == 1025 || us == 1073 || us == 1074 || us == 1075 || us == 1076 || us == 1077 || us == 1078 || us == 1079 || us == 1080 || us == 1081 ||
         us == 1082 || us == 1083 || us == 1084 || us == 1085 || us == 1096 || us == 1097 || us == 1098) &&
        usModel != 539)
        return true;

    // No nitro or other upgrades for planes/helicopters
    if (vehicleType == VEHICLE_PLANE)
        return false;

    if (us == VEHICLEUPGRADE_NITRO_5X || us == VEHICLEUPGRADE_NITRO_2X || us == VEHICLEUPGRADE_NITRO_10X || us == VEHICLEUPGRADE_HYDRAULICS)
        return true;

    if (us == 1000 || us == 1001 || us == 1002 || us == 1003 || us == 1014 || /* spoiler */
        us == 1015 || us == 1016 || us == 1023 || us == 1049 || us == 1050 || us == 1058 || us == 1060 || us == 1138 || us == 1139 || us == 1146 ||
        us == 1147 || us == 1158 || us == 1162 || us == 1163 || us == 1164)
    {
        return true;
    }

    switch (usModel)
    {
        case 400:
            return (us == 1020 || us == 1021 || us == 1019 || us == 1018 || us == 1013 || us == 1024 || us == 1008 || us == 1009 || us == 1010);
        case 401:
            return (us == 1005 || us == 1004 || us == 1142 || us == 1143 || us == 1144 || us == 114 || us == 1020 || us == 1019 || us == 1013 || us == 1008 ||
                    us == 1009 || us == 1010 || us == 1006 || us == 1001 || us == 1003 || us == 1017 || us == 1007);
        case 402:
            return (us == 1009 || us == 1010);
        case 404:
            return (us == 1020 || us == 1021 || us == 1019 || us == 1013 || us == 1008 || us == 1009 || us == 1010 || us == 1002 || us == 1016 || us == 1000 ||
                    us == 1017 || us == 1007);
        case 405:
            return (us == 1020 || us == 1021 || us == 1019 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1001 || us == 1014 || us == 1023 ||
                    us == 1000);
        case 409:
            return (us == 1009);
        case 410:
            return (us == 1019 || us == 1021 || us == 1020 || us == 1013 || us == 1024 || us == 1008 || us == 1009 || us == 1010 || us == 1001 || us == 1023 ||
                    us == 1003 || us == 1017 || us == 1007);
        case 411:
            return (us == 1008 || us == 1009 || us == 1010);
        case 412:
            return (us == 1008 || us == 1009 || us == 1010);
        case 415:
            return (us == 1019 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1001 || us == 1023 || us == 1003 || us == 1017 || us == 1007);
        case 418:
            return (us == 1020 || us == 1021 || us == 1008 || us == 1009 || us == 1010 || us == 1006 || us == 1002 || us == 1016);
        case 419:
            return (us == 1008 || us == 1009 || us == 1010);
        case 420:
            return (us == 1005 || us == 1004 || us == 1021 || us == 1019 || us == 1008 || us == 1009 || us == 1010 || us == 1001 || us == 1003);
        case 421:
            return (us == 1020 || us == 1021 || us == 1019 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1014 || us == 1023 || us == 1016 ||
                    us == 1000);
        case 422:
            return (us == 1020 || us == 1021 || us == 1019 || us == 1013 || us == 1008 || us == 1009 || us == 1010 || us == 1017 || us == 1007);
        case 426:
            return (us == 1005 || us == 1004 || us == 1021 || us == 1019 || us == 1008 || us == 1009 || us == 1010 || us == 1006 || us == 1001 || us == 1003);
        case 429:
            return (us == 1008 || us == 1009 || us == 1010);
        case 436:
            return (us == 1020 || us == 1021 || us == 1022 || us == 1019 || us == 1013 || us == 1008 || us == 1009 || us == 1010 || us == 1006 || us == 1001 ||
                    us == 1003 || us == 1017 || us == 1007);
        case 438:
            return (us == 1008 || us == 1009 || us == 1010);
        case 439:
            return (us == 1003 || us == 1023 || us == 1001 || us == 1008 || us == 1009 || us == 1010 || us == 1017 || us == 1007 || us == 1142 || us == 1143 ||
                    us == 1144 || us == 1145 || us == 1013);
        case 442:
            return (us == 1008 || us == 1009 || us == 1010);
        case 445:
            return (us == 1008 || us == 1009 || us == 1010);
        case 451:
            return (us == 1008 || us == 1009 || us == 1010);
        case 458:
            return (us == 1008 || us == 1009 || us == 1010);
        case 466:
            return (us == 1008 || us == 1009 || us == 1010);
        case 467:
            return (us == 1008 || us == 1009 || us == 1010);
        case 474:
            return (us == 1008 || us == 1009 || us == 1010);
        case 475:
            return (us == 1008 || us == 1009 || us == 1010);
        case 477:
            return (us == 1020 || us == 1021 || us == 1019 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1006 || us == 1017 || us == 1007);
        case 478:
            return (us == 1005 || us == 1004 || us == 1012 || us == 1020 || us == 1021 || us == 1022 || us == 1013 || us == 1024 || us == 1008 || us == 1009 ||
                    us == 1010);
        case 479:
            return (us == 1008 || us == 1009 || us == 1010);
        case 480:
            return (us == 1008 || us == 1009 || us == 1010);
        case 489:
            return (us == 1005 || us == 1004 || us == 1020 || us == 1019 || us == 1018 || us == 1013 || us == 1024 || us == 1008 || us == 1009 || us == 1010 ||
                    us == 1006 || us == 1002 || us == 1016 || us == 1000);
        case 491:
            return (us == 1142 || us == 1143 || us == 1144 || us == 1145 || us == 1020 || us == 1021 || us == 1019 || us == 1018 || us == 1008 || us == 1009 ||
                    us == 1010 || us == 1014 || us == 1023 || us == 1003 || us == 1017 || us == 1007);
        case 492:
            return (us == 1005 || us == 1004 || us == 1008 || us == 1009 || us == 1010 || us == 1006 || us == 1016 || us == 1000);
        case 496:
            return (us == 1006 || us == 1017 || us == 1007 || us == 1011 || us == 1019 || us == 1023 || us == 1001 || us == 1008 || us == 1009 || us == 1010 ||
                    us == 1003 || us == 1002 || us == 1142 || us == 1143 || us == 1020);
        case 500:
            return (us == 1020 || us == 1021 || us == 1019 || us == 1013 || us == 1024 || us == 1008 || us == 1009 || us == 1010);
        case 506:
            return (us == 1009);
        case 507:
            return (us == 1008 || us == 1009 || us == 1010);
        case 516:
            return (us == 1004 || us == 1020 || us == 1021 || us == 1019 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1002 || us == 1015 ||
                    us == 1016 || us == 1000 || us == 1017 || us == 1007);
        case 517:
            return (us == 1142 || us == 1143 || us == 1144 || us == 1145 || us == 1020 || us == 1019 || us == 1018 || us == 1008 || us == 1009 || us == 1010 ||
                    us == 1002 || us == 1023 || us == 1016 || us == 1003 || us == 1017 || us == 1007);
        case 518:
            return (us == 1005 || us == 1142 || us == 1143 || us == 1144 || us == 1145 || us == 1020 || us == 1018 || us == 1013 || us == 1008 || us == 1009 ||
                    us == 1010 || us == 1006 || us == 1001 || us == 1023 || us == 1003 || us == 1017 || us == 1007);
        case 526:
            return (us == 1008 || us == 1009 || us == 1010);
        case 527:
            return (us == 1021 || us == 1020 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1001 || us == 1014 || us == 1015 || us == 1017 ||
                    us == 1007);
        case 529:
            return (us == 1012 || us == 1011 || us == 1020 || us == 1019 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1006 || us == 1001 ||
                    us == 1023 || us == 1003 || us == 1017 || us == 1007);
        case 533:
            return (us == 1008 || us == 1009 || us == 1010);
        case 534:
            return (us == 1126 || us == 1127 || us == 1179 || us == 1185 || us == 1100 || us == 1123 || us == 1125 || us == 1008 || us == 1009 || us == 1010 ||
                    us == 1180 || us == 1178 || us == 1101 || us == 1122 || us == 1124 || us == 1106);
        case 535:
            return (us == 1109 || us == 1110 || us == 1113 || us == 1114 || us == 1115 || us == 1116 || us == 1117 || us == 1008 || us == 1009 || us == 1010 ||
                    us == 1120 || us == 1118 || us == 1121 || us == 1119);
        case 536:
            return (us == 1104 || us == 1105 || us == 1182 || us == 1181 || us == 1008 || us == 1009 || us == 1010 || us == 1184 || us == 1183 || us == 1128 ||
                    us == 1103 || us == 1107 || us == 1108);
        case 540:
            return (us == 1004 || us == 1142 || us == 1143 || us == 1144 || us == 1145 || us == 1020 || us == 1019 || us == 1018 || us == 1024 || us == 1008 ||
                    us == 1009 || us == 1010 || us == 1006 || us == 1001 || us == 1023 || us == 1017 || us == 1007);
        case 541:
            return (us == 1008 || us == 1009 || us == 1010);
        case 542:
            return (us == 1144 || us == 1145 || us == 1020 || us == 1021 || us == 1019 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1014 ||
                    us == 1015);
        case 545:
            return (us == 1008 || us == 1009 || us == 1010);
        case 546:
            return (us == 1004 || us == 1142 || us == 1143 || us == 1144 || us == 1145 || us == 1019 || us == 1018 || us == 1024 || us == 1008 || us == 1009 ||
                    us == 1010 || us == 1006 || us == 1002 || us == 1001 || us == 1023 || us == 1017 || us == 1007);
        case 547:
            return (us == 1142 || us == 1143 || us == 1020 || us == 1021 || us == 1019 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1016 ||
                    us == 1003 || us == 1000);
        case 549:
            return (us == 1012 || us == 1011 || us == 1142 || us == 1143 || us == 1144 || us == 1145 || us == 1020 || us == 1019 || us == 1018 || us == 1008 ||
                    us == 1009 || us == 1010 || us == 1001 || us == 1023 || us == 1003 || us == 1017 || us == 1007);
        case 550:
            return (us == 1005 || us == 1004 || us == 1142 || us == 1143 || us == 1144 || us == 1145 || us == 1020 || us == 1019 || us == 1018 || us == 1008 ||
                    us == 1009 || us == 1010 || us == 1006 || us == 1001 || us == 1023 || us == 1003);
        case 551:
            return (us == 1005 || us == 1020 || us == 1021 || us == 1019 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1006 || us == 1002 ||
                    us == 1023 || us == 1016 || us == 1003);
        case 555:
            return (us == 1008 || us == 1009 || us == 1010);
        case 558:
            return (us == 1092 || us == 1089 || us == 1166 || us == 1165 || us == 1008 || us == 1009 || us == 1010 || us == 1168 || us == 1167 || us == 1088 ||
                    us == 1091 || us == 1164 || us == 1163 || us == 1094 || us == 1090 || us == 1095 || us == 1093);
        case 559:
            return (us == 1065 || us == 1066 || us == 1160 || us == 1173 || us == 1008 || us == 1009 || us == 1010 || us == 1159 || us == 1161 || us == 1162 ||
                    us == 1158 || us == 1067 || us == 1068 || us == 1071 || us == 1069 || us == 1072 || us == 1070);
        case 560:
            return (us == 1028 || us == 1029 || us == 1169 || us == 1170 || us == 1008 || us == 1009 || us == 1010 || us == 1141 || us == 1140 || us == 1032 ||
                    us == 1033 || us == 1138 || us == 1139 || us == 1027 || us == 1026 || us == 1030 || us == 1031);
        case 561:
            return (us == 1064 || us == 1059 || us == 1155 || us == 1157 || us == 1008 || us == 1009 || us == 1010 || us == 1154 || us == 1156 || us == 1055 ||
                    us == 1061 || us == 1058 || us == 1060 || us == 1062 || us == 1056 || us == 1063 || us == 1057);
        case 562:
            return (us == 1034 || us == 1037 || us == 1171 || us == 1172 || us == 1008 || us == 1009 || us == 1010 || us == 1149 || us == 1148 || us == 1038 ||
                    us == 1035 || us == 1147 || us == 1146 || us == 1040 || us == 1036 || us == 1041 || us == 1039);
        case 565:
            return (us == 1046 || us == 1045 || us == 1153 || us == 1152 || us == 1008 || us == 1009 || us == 1010 || us == 1150 || us == 1151 || us == 1054 ||
                    us == 1053 || us == 1049 || us == 1050 || us == 1051 || us == 1047 || us == 1052 || us == 1048);
        case 566:
            return (us == 1008 || us == 1009 || us == 1010);
        case 567:
            return (us == 1129 || us == 1132 || us == 1189 || us == 1188 || us == 1008 || us == 1009 || us == 1010 || us == 1187 || us == 1186 || us == 1130 ||
                    us == 1131 || us == 1102 || us == 1133);
        case 575:
            return (us == 1044 || us == 1043 || us == 1174 || us == 1175 || us == 1008 || us == 1009 || us == 1010 || us == 1176 || us == 1177 || us == 1099 ||
                    us == 1042);
        case 576:
            return (us == 1136 || us == 1135 || us == 1191 || us == 1190 || us == 1008 || us == 1009 || us == 1010 || us == 1192 || us == 1193 || us == 1137 ||
                    us == 1134);
        case 579:
            return (us == 1008 || us == 1009 || us == 1010);
        case 580:
            return (us == 1020 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1006 || us == 1001 || us == 1023 || us == 1017 || us == 1007);
        case 585:
            return (us == 1142 || us == 1143 || us == 1144 || us == 1145 || us == 1020 || us == 1019 || us == 1018 || us == 1013 || us == 1008 || us == 1009 ||
                    us == 1010 || us == 1006 || us == 1001 || us == 1023 || us == 1003 || us == 1017 || us == 1007);
        case 587:
            return (us == 1008 || us == 1009 || us == 1010);
        case 589:
            return (us == 1005 || us == 1004 || us == 1144 || us == 1145 || us == 1020 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1024 ||
                    us == 1013 || us == 1006 || us == 1016 || us == 1000 || us == 1017 || us == 1007);
        case 600:
            return (us == 1005 || us == 1004 || us == 1020 || us == 1022 || us == 1018 || us == 1013 || us == 1008 || us == 1009 || us == 1010 || us == 1006 ||
                    us == 1017 || us == 1007);
        case 602:
            return (us == 1008 || us == 1009 || us == 1010);
        case 603:
            return (us == 1144 || us == 1145 || us == 1142 || us == 1143 || us == 1020 || us == 1019 || us == 1018 || us == 1024 || us == 1008 || us == 1009 ||
                    us == 1010 || us == 1006 || us == 1001 || us == 1023 || us == 1017 || us == 1007);
    }
    return false;
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

unsigned short CVehicleUpgrades::GetSlotState(unsigned char ucSlot)
{
    if (ucSlot < VEHICLE_UPGRADE_SLOTS)
        return m_SlotStates[ucSlot];

    return 0;
}

void CVehicleUpgrades::SetSlotState(unsigned char ucSlot, unsigned short usUpgrade)
{
    if (ucSlot < VEHICLE_UPGRADE_SLOTS)
        m_SlotStates[ucSlot] = usUpgrade;
}

bool CVehicleUpgrades::AddUpgrade(unsigned short usUpgrade)
{
    if (IsUpgradeCompatible(usUpgrade))
    {
        unsigned char ucSlot;
        if (GetSlotFromUpgrade(usUpgrade, ucSlot))
        {
            m_SlotStates[ucSlot] = usUpgrade;

            return true;
        }
    }
    return false;
}

void CVehicleUpgrades::AddAllUpgrades()
{
    unsigned short usUpgrade = 1000;
    for (; usUpgrade <= 1193; usUpgrade++)
    {
        AddUpgrade(usUpgrade);
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

unsigned char CVehicleUpgrades::Count()
{
    unsigned char ucSlot = 0, ucCount = 0;
    for (; ucSlot < VEHICLE_UPGRADE_SLOTS; ucSlot++)
    {
        if (m_SlotStates[ucSlot])
            ucCount++;
    }

    return ucCount;
}

const char* CVehicleUpgrades::GetSlotName(unsigned char ucSlot)
{
    if (ucSlot < VEHICLE_UPGRADE_SLOTS)
        return UpgradeNames[ucSlot].szName;

    return szUpgradeNameEmpty;
}

bool CVehicleUpgrades::IsValidUpgrade(unsigned short usUpgrade)
{
    return (usUpgrade >= 1000 && usUpgrade <= 1193);
}
