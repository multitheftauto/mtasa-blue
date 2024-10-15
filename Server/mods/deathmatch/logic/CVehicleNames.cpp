/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CVehicleNames.cpp
 *  PURPOSE:     GTA vehicle name definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CVehicleNames.h"
#include "CVehicleManager.h"

const char* szVehicleNameEmpty = "";

struct SVehicleName
{
    const char* szName;
    const char* szName_replaced;            // Compatability
};

static const SFixedArray<SVehicleName, 212> VehicleNames = {{{"Landstalker"},
                                                             {"Bravura"},
                                                             {"Buffalo"},
                                                             {"Linerunner"},
                                                             {"Perennial"},
                                                             {"Sentinel"},
                                                             {"Dumper"},
                                                             {"Fire Truck"},
                                                             {"Trashmaster"},
                                                             {"Stretch"},
                                                             {"Manana"},
                                                             {"Infernus"},
                                                             {"Voodoo"},
                                                             {"Pony"},
                                                             {"Mule"},
                                                             {"Cheetah"},
                                                             {"Ambulance"},
                                                             {"Leviathan"},
                                                             {"Moonbeam"},
                                                             {"Esperanto"},
                                                             {"Taxi"},
                                                             {"Washington"},
                                                             {"Bobcat"},
                                                             {"Mr. Whoopee"},
                                                             {"BF Injection"},
                                                             {"Hunter"},
                                                             {"Premier"},
                                                             {"Enforcer"},
                                                             {"Securicar"},
                                                             {"Banshee"},
                                                             {"Predator"},
                                                             {"Bus"},
                                                             {"Rhino"},
                                                             {"Barracks"},
                                                             {"Hotknife"},
                                                             {"Trailer 1"},
                                                             {"Previon"},
                                                             {"Coach"},
                                                             {"Cabbie"},
                                                             {"Stallion"},
                                                             {"Rumpo"},
                                                             {"RC Bandit"},
                                                             {"Romero"},
                                                             {"Packer"},
                                                             {"Monster 1", "Monster"},
                                                             {"Admiral"},
                                                             {"Squalo"},
                                                             {"Seasparrow"},
                                                             {"Pizzaboy"},
                                                             {"Tram"},
                                                             {"Trailer 2"},
                                                             {"Turismo"},
                                                             {"Speeder"},
                                                             {"Reefer"},
                                                             {"Tropic"},
                                                             {"Flatbed"},
                                                             {"Yankee"},
                                                             {"Caddy"},
                                                             {"Solair"},
                                                             {"Berkley's RC Van"},
                                                             {"Skimmer"},
                                                             {"PCJ-600"},
                                                             {"Faggio"},
                                                             {"Freeway"},
                                                             {"RC Baron"},
                                                             {"RC Raider"},
                                                             {"Glendale"},
                                                             {"Oceanic"},
                                                             {"Sanchez"},
                                                             {"Sparrow"},
                                                             {"Patriot"},
                                                             {"Quadbike"},
                                                             {"Coastguard"},
                                                             {"Dinghy"},
                                                             {"Hermes"},
                                                             {"Sabre"},
                                                             {"Rustler"},
                                                             {"ZR-350"},
                                                             {"Walton"},
                                                             {"Regina"},
                                                             {"Comet"},
                                                             {"BMX"},
                                                             {"Burrito"},
                                                             {"Camper"},
                                                             {"Marquis"},
                                                             {"Baggage"},
                                                             {"Dozer"},
                                                             {"Maverick"},
                                                             {"News Chopper"},
                                                             {"Rancher"},
                                                             {"FBI Rancher"},
                                                             {"Virgo"},
                                                             {"Greenwood"},
                                                             {"Jetmax"},
                                                             {"Hotring Racer"},
                                                             {"Sandking"},
                                                             {"Blista Compact"},
                                                             {"Police Maverick"},
                                                             {"Boxville"},
                                                             {"Benson"},
                                                             {"Mesa"},
                                                             {"RC Goblin"},
                                                             {"Hotring Racer 2"},
                                                             {"Hotring Racer 3"},
                                                             {"Bloodring Banger"},
                                                             {"Rancher Lure"},
                                                             {"Super GT"},
                                                             {"Elegant"},
                                                             {"Journey"},
                                                             {"Bike"},
                                                             {"Mountain Bike"},
                                                             {"Beagle"},
                                                             {"Cropduster"},
                                                             {"Stuntplane"},
                                                             {"Tanker"},
                                                             {"Roadtrain"},
                                                             {"Nebula"},
                                                             {"Majestic"},
                                                             {"Buccaneer"},
                                                             {"Shamal"},
                                                             {"Hydra"},
                                                             {"FCR-900"},
                                                             {"NRG-500"},
                                                             {"HPV1000"},
                                                             {"Cement Truck"},
                                                             {"Towtruck"},
                                                             {"Fortune"},
                                                             {"Cadrona"},
                                                             {"FBI Truck"},
                                                             {"Willard"},
                                                             {"Forklift"},
                                                             {"Tractor"},
                                                             {"Combine Harvester"},
                                                             {"Feltzer"},
                                                             {"Remington"},
                                                             {"Slamvan"},
                                                             {"Blade"},
                                                             {"Freight"},
                                                             {"Streak"},
                                                             {"Vortex"},
                                                             {"Vincent"},
                                                             {"Bullet"},
                                                             {"Clover"},
                                                             {"Sadler"},
                                                             {"Fire Truck Ladder"},
                                                             {"Hustler"},
                                                             {"Intruder"},
                                                             {"Primo"},
                                                             {"Cargobob"},
                                                             {"Tampa"},
                                                             {"Sunrise"},
                                                             {"Merit"},
                                                             {"Utility Van"},
                                                             {"Nevada"},
                                                             {"Yosemite"},
                                                             {"Windsor"},
                                                             {"Monster 2"},
                                                             {"Monster 3"},
                                                             {"Uranus"},
                                                             {"Jester"},
                                                             {"Sultan"},
                                                             {"Stratum"},
                                                             {"Elegy"},
                                                             {"Raindance"},
                                                             {"RC Tiger"},
                                                             {"Flash"},
                                                             {"Tahoma"},
                                                             {"Savanna"},
                                                             {"Bandito"},
                                                             {"Freight Train Flatbed"},
                                                             {"Streak Train Trailer"},
                                                             {"Kart"},
                                                             {"Mower"},
                                                             {"Dune"},
                                                             {"Sweeper"},
                                                             {"Broadway"},
                                                             {"Tornado"},
                                                             {"AT-400"},
                                                             {"DFT-30"},
                                                             {"Huntley"},
                                                             {"Stafford"},
                                                             {"BF-400"},
                                                             {"Newsvan"},
                                                             {"Tug"},
                                                             {"Trailer (Tanker Commando)"},
                                                             {"Emperor"},
                                                             {"Wayfarer"},
                                                             {"Euros"},
                                                             {"Hotdog"},
                                                             {"Club"},
                                                             {"Box Freight"},
                                                             {"Trailer 3"},
                                                             {"Andromada"},
                                                             {"Dodo"},
                                                             {"RC Cam"},
                                                             {"Launch"},
                                                             {"Police LS", "Police"},
                                                             {"Police SF"},
                                                             {"Police LV"},
                                                             {"Police Ranger", "Ranger"},
                                                             {"Picador"},
                                                             {"S.W.A.T."},
                                                             {"Alpha"},
                                                             {"Phoenix"},
                                                             {"Glendale Damaged"},
                                                             {"Sadler Damaged", "Sadler"},
                                                             {"Baggage Trailer (covered)"},
                                                             {"Baggage Trailer (Uncovered)"},
                                                             {"Trailer (Stairs)"},
                                                             {"Boxville Mission"},
                                                             {"Farm Trailer"},
                                                             {"Street Clean Trailer"}}};

bool CVehicleNames::IsValidModel(unsigned long ulModel)
{
    return ulModel >= 400 && ulModel <= 611;
}

const char* CVehicleNames::GetVehicleName(unsigned long ulModel)
{
    // Valid?
    if (IsValidModel(ulModel) && ((ulModel - 400) < NUMELMS(VehicleNames)))
    {
        // Look it up in the table
        return VehicleNames[ulModel - 400].szName;
    }

    return szVehicleNameEmpty;
}

unsigned int CVehicleNames::GetVehicleModel(const char* szName)
{
    // If the specified string was empty, return 0
    if (szName[0] == 0)
        return 0;

    assert(NUMELMS(VehicleNames) == 212);

    // Look for it in our table
    for (unsigned int i = 0; i <= 211; i++)
    {
        if (stricmp(szName, VehicleNames[i].szName) == 0 || (VehicleNames[i].szName_replaced && stricmp(szName, VehicleNames[i].szName_replaced) == 0))
        {
            return i + 400;
        }
    }
    return 0;
}

const char* CVehicleNames::GetVehicleTypeName(unsigned long ulModel)
{
    const char* pVehicleName = "";
    switch (CVehicleManager::GetVehicleType(static_cast<unsigned short>(ulModel)))
    {
        case VEHICLE_NONE:
            pVehicleName = "Unknown";
            break;
        case VEHICLE_CAR:
            pVehicleName = "Automobile";
            break;
        case VEHICLE_BOAT:
            pVehicleName = "Boat";
            break;
        case VEHICLE_TRAIN:
            pVehicleName = "Train";
            break;
        case VEHICLE_HELI:
            pVehicleName = "Helicopter";
            break;
        case VEHICLE_PLANE:
            pVehicleName = "Plane";
            break;
        case VEHICLE_BIKE:
            pVehicleName = "Bike";
            break;
        case VEHICLE_MONSTERTRUCK:
            pVehicleName = "Monster Truck";
            break;
        case VEHICLE_QUADBIKE:
            pVehicleName = "Quad";
            break;
        case VEHICLE_BMX:
            pVehicleName = "BMX";
            break;
        case VEHICLE_TRAILER:
            pVehicleName = "Trailer";
            break;
    }

    return pVehicleName;
}
