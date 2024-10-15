/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CVehicleNames.cpp
 *  PURPOSE:     Vehicle names class
 *
 *****************************************************************************/

#include <StdInc.h>

using namespace std;

const char* const szVehicleNameEmpty = "";

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
                                                             {"Sadler", "Sadler Damaged"},
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

static const SFixedArray<SVehicleName, 10> VehicleTypes = {
    {{"Automobile"}, {"Plane"}, {"Bike"}, {"Helicopter"}, {"Boat"}, {"Train"}, {"Trailer"}, {"BMX"}, {"Monster Truck"}, {"Quad"}}};

static const SFixedArray<unsigned char, 212> ucVehicleTypes = {
    0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 4, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0,
    0, 8, 0, 4, 3, 2, 5, 6, 0, 4, 4, 4, 0, 0, 0, 0, 0, 1, 2, 2, 2, 1, 3, 0, 0, 2, 3, 0, 9, 4, 4, 0, 0, 1, 0, 0, 0, 0, 7, 0, 0, 4, 0,
    0, 3, 3, 0, 0, 0, 0, 4, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 2, 2, 2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 1, 0, 0, 8, 8, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 5, 5, 0,
    0, 8, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 6, 0, 2, 0, 0, 0, 5, 6, 1, 1, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 0, 6, 6};

bool CVehicleNames::IsValidModel(unsigned long ulModel)
{
    return ulModel >= 400 && ulModel <= 611;
}

bool CVehicleNames::IsModelTrailer(unsigned long ulModel)
{
    // IsValidModel excludes trailers, so we need the ability to check separately if it is a trailer
    return (ulModel == 435 || ulModel == 450 || ulModel == 591 || ulModel == 606 || ulModel == 607 || ulModel == 584 || ulModel == 608 || ulModel == 610 ||
            ulModel == 611);
}

const char* CVehicleNames::GetVehicleName(unsigned long ulModel)
{
    // Valid?
    if (IsValidModel(ulModel))
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

    // Look for it in our table
    for (unsigned int i = 0; i < NUMELMS(VehicleNames); i++)
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
    // Use parent model ID for non-standard vehicle model IDs.
    if ((ulModel < 400 || ulModel > 611) && CClientVehicleManager::IsValidModel(ulModel))
        ulModel = g_pGame->GetModelInfo(ulModel)->GetParentID();

    // Check whether the model is valid
    if ((IsValidModel(ulModel) || IsModelTrailer(ulModel)) && ((ulModel - 400) < NUMELMS(ucVehicleTypes)))
    {
        int iVehicleType = ucVehicleTypes[ulModel - 400];
        return VehicleTypes[iVehicleType].szName;
    }

    return szVehicleNameEmpty;
}
