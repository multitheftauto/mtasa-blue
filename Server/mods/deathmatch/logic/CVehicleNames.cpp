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

static const SFixedArray<SVehicleName, 212> VehicleNames = {{{"Admiral"},
                                                             {"Air Tug"},
                                                             {"Alpha"},
                                                             {"Ambulance"},
                                                             {"Andromada"},
                                                             {"Baggage"},
                                                             {"Baggage Trailer (covered)"},
                                                             {"Baggage Trailer (Uncovered)"},
                                                             {"Banshee"},
                                                             {"Barracks"},
                                                             {"BF Injection"},
                                                             {"BF-400"},
                                                             {"Bike"},
                                                             {"Blade"},
                                                             {"Blista Compact"},
                                                             {"Bloodring Banger"},
                                                             {"Bobcat"},
                                                             {"Box Freight"},
                                                             {"Boxville"},
                                                             {"Boxville Mission"},
                                                             {"Broadway"},
                                                             {"Bravura"},
                                                             {"Bucanneer"},
                                                             {"Buffalo"},
                                                             {"Bullet"},
                                                             {"Burrito"},
                                                             {"Bus"},
                                                             {"Cabbie"},
                                                             {"Caddy"},
                                                             {"Cadrona"},
                                                             {"Camper"},
                                                             {"Cargobob"},
                                                             {"Cement Truck"},
                                                             {"Cheetah"},
                                                             {"Clover"},
                                                             {"Club"},
                                                             {"Coach"},
                                                             {"Comet"},
                                                             {"Combine Harvester"},
                                                             {"Cropduster"},
                                                             {"DFT-30"},
                                                             {"Dinghy"},
                                                             {"Dodo"},
                                                             {"Dozer"},
                                                             {"Dumper"},
                                                             {"Dune"},
                                                             {"Elegant"},
                                                             {"Elegy"},
                                                             {"Emperor"},
                                                             {"Enforcer"},
                                                             {"Esperanto"},
                                                             {"Euros"},
                                                             {"Faggio"},
                                                             {"FBI Rancher"},
                                                             {"FBI Truck"},
                                                             {"FCR-900"},
                                                             {"Feltzer"},
                                                             {"Fire Truck"},
                                                             {"Fire Truck Ladder"},
                                                             {"Flash"},
                                                             {"Flatbed"},
                                                             {"Forklift"},
                                                             {"Fortune"},
                                                             {"Freeway"},
                                                             {"Freight"},
                                                             {"Freight Train Flatbed"},
                                                             {"Glendale"},
                                                             {"Glendale Damaged"},
                                                             {"Greenwood"},
                                                             {"Hermes"},
                                                             {"Hotdog"},
                                                             {"Hotknife"},
                                                             {"Hotring Racer"},
                                                             {"Hotring Racer 2"},
                                                             {"Hotring Racer 3"},
                                                             {"Hunter"},
                                                             {"Huntley"},
                                                             {"Hustler"},
                                                             {"Hydra"},
                                                             {"Infernus"},
                                                             {"Intruder"},
                                                             {"Jester"},
                                                             {"Jetmax"},
                                                             {"Journey"},
                                                             {"Kart"},
                                                             {"Landstalker"},
                                                             {"Launch"},
                                                             {"Leviathan"},
                                                             {"Linerunner"},
                                                             {"Majestic"},
                                                             {"Manana"},
                                                             {"Marquis"},
                                                             {"Maverick"},
                                                             {"Mesa"},
                                                             {"Monster 1", "Monster"},
                                                             {"Monster 2"},
                                                             {"Monster 3"},
                                                             {"Moonbeam"},
                                                             {"Mower"},
                                                             {"Mr. Whoopee"},
                                                             {"Mule"},
                                                             {"Nebula"},
                                                             {"News Chopper"},
                                                             {"Newsvan"},
                                                             {"NRG-500"},
                                                             {"Oceanic"},
                                                             {"Packer"},
                                                             {"Patriot"},
                                                             {"PCJ-600"},
                                                             {"Perennial"},
                                                             {"Phoenix"},
                                                             {"Picador"},
                                                             {"Police LS", "Police"},
                                                             {"Police LV"},
                                                             {"Police Maverick"},
                                                             {"Police Ranger", "Ranger"},
                                                             {"Police SF"},
                                                             {"Pony"},
                                                             {"Premier"},
                                                             {"Previon"},
                                                             {"Primo"},
                                                             {"Quadbike"},
                                                             {"Raindance"},
                                                             {"Rancher"},
                                                             {"Rancher Lure"},
                                                             {"RC Baron"},
                                                             {"RC Bandit"},
                                                             {"RC Cam"},
                                                             {"RC Goblin"},
                                                             {"RC Raider"},
                                                             {"RC Tiger"},
                                                             {"Reefer"},
                                                             {"Regina"},
                                                             {"Remington"},
                                                             {"Rhino"},
                                                             {"Roadtrain"},
                                                             {"Romero"},
                                                             {"Rumpo"},
                                                             {"Rustler"},
                                                             {"S.W.A.T."},
                                                             {"Sabre"},
                                                             {"Sadler"},
                                                             {"Sadler Damaged", "Sadler"},
                                                             {"Sanchez"},
                                                             {"Sandking"},
                                                             {"Savanna"},
                                                             {"Seasparrow"},
                                                             {"Securicar"},
                                                             {"Sentinel"},
                                                             {"Shamal"},
                                                             {"Skimmer"},
                                                             {"Slamvan"},
                                                             {"Solair"},
                                                             {"Stallion"},
                                                             {"Stratum"},
                                                             {"Streak"},
                                                             {"Streak Train Trailer"},
                                                             {"Stretch"},
                                                             {"Stuntplane"},
                                                             {"Sultan"},
                                                             {"Sunrise"},
                                                             {"Super GT"},
                                                             {"Sweeper"},
                                                             {"Tampa"},
                                                             {"Tanker"},
                                                             {"Tanker Trailer"},
                                                             {"Tahoma"},
                                                             {"Taxi"},
                                                             {"Topfun Van (Berkley's RC)"},
                                                             {"Tornado"},
                                                             {"Towtruck"},
                                                             {"Trailer 1"},
                                                             {"Trailer 2"},
                                                             {"Trailer 3"},
                                                             {"Trailer (Stairs)"},
                                                             {"Trailer (Tanker Commando)"},
                                                             {"Tractor"},
                                                             {"Tram"},
                                                             {"Trashmaster"},
                                                             {"Tropic"},
                                                             {"Tug"},
                                                             {"Turismo"},
                                                             {"Utility Van"},
                                                             {"Vincent"},
                                                             {"Virgo"},
                                                             {"Voodoo"},
                                                             {"Vortex"},
                                                             {"Walton"},
                                                             {"Washington"},
                                                             {"Wayfarer"},
                                                             {"Willard"},
                                                             {"Windsor"},
                                                             {"Yankee"},
                                                             {"Yosemite"},
                                                             {"ZR-350"}}};


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
