/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CVehicleNames.cpp
*  PURPOSE:     Vehicle names class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Peter <>
*
*****************************************************************************/

#include <StdInc.h>

using namespace std;

const char* szVehicleNameEmpty = "";

struct SVehicleName
{
    const char* szName;
    const char* szName_replaced; //Compatability
};

static const SFixedArray < SVehicleName, 212 > VehicleNames =
{ {
    {"Landstalker"}, {"Bravura"}, {"Buffalo"}, {"Linerunner"}, {"Perennial"}, {"Sentinel"}, {"Dumper"}, {"Fire Truck"}, {"Trashmaster"}, {"Stretch"}, 
    {"Manana"}, {"Infernus"}, {"Voodoo"}, {"Pony"}, {"Mule"}, {"Cheetah"}, {"Ambulance"}, {"Leviathan"}, {"Moonbeam"}, {"Esperanto"}, 
    {"Taxi"}, {"Washington"}, {"Bobcat"}, {"Mr. Whoopee"}, {"BF Injection"}, {"Hunter"}, {"Premier"}, {"Enforcer"}, {"Securicar"}, {"Banshee"}, 
    {"Predator"}, {"Bus"}, {"Rhino"}, {"Barracks"}, {"Hotknife"}, {"Trailer 1"}, {"Previon"}, {"Coach"}, {"Cabbie"}, {"Stallion"}, 
    {"Rumpo"}, {"RC Bandit"}, {"Romero"}, {"Packer"}, {"Monster 1", "Monster"}, {"Admiral"}, {"Squalo"}, {"Seasparrow"}, {"Pizzaboy"}, {"Tram"}, 
    {"Trailer 2"}, {"Turismo"}, {"Speeder"}, {"Reefer"}, {"Tropic"}, {"Flatbed"}, {"Yankee"}, {"Caddy"}, {"Solair"}, {"Berkley's RC Van"}, 
    {"Skimmer"}, {"PCJ-600"}, {"Faggio"}, {"Freeway"}, {"RC Baron"}, {"RC Raider"}, {"Glendale"}, {"Oceanic"}, {"Sanchez"}, {"Sparrow"}, 
    {"Patriot"}, {"Quadbike"}, {"Coastguard"}, {"Dinghy"}, {"Hermes"}, {"Sabre"}, {"Rustler"}, {"ZR-350"}, {"Walton"}, {"Regina"}, 
    {"Comet"}, {"BMX"}, {"Burrito"}, {"Camper"}, {"Marquis"}, {"Baggage"}, {"Dozer"}, {"Maverick"}, {"News Chopper"}, {"Rancher"}, 
    {"FBI Rancher"}, {"Virgo"}, {"Greenwood"}, {"Jetmax"}, {"Hotring Racer"}, {"Sandking"}, {"Blista Compact"}, {"Police Maverick"}, {"Boxville"}, {"Benson"}, 
    {"Mesa"}, {"RC Goblin"}, {"Hotring Racer 3"}, {"Hotring Racer 2"}, {"Bloodring Banger"}, {"Rancher Lure"}, {"Super GT"}, {"Elegant"}, {"Journey"}, {"Bike"}, 
    {"Mountain Bike"}, {"Beagle"}, {"Cropduster"}, {"Stuntplane"}, {"Tanker"}, {"Roadtrain"}, {"Nebula"}, {"Majestic"}, {"Buccaneer"}, {"Shamal"}, 
    {"Hydra"}, {"FCR-900"}, {"NRG-500"}, {"HPV1000"}, {"Cement Truck"}, {"Towtruck"}, {"Fortune"}, {"Cadrona"}, {"FBI Truck"}, {"Willard"}, 
    {"Forklift"}, {"Tractor"}, {"Combine Harvester"}, {"Feltzer"}, {"Remington"}, {"Slamvan"}, {"Blade"}, {"Freight"}, {"Streak"}, {"Vortex"}, 
    {"Vincent"}, {"Bullet"}, {"Clover"}, {"Sadler"}, {"Fire Truck Ladder"}, {"Hustler"}, {"Intruder"}, {"Primo"}, {"Cargobob"}, {"Tampa"}, 
    {"Sunrise"}, {"Merit"}, {"Utility Van"}, {"Nevada"}, {"Yosemite"}, {"Windsor"}, {"Monster 2"}, {"Monster 3"}, {"Uranus"}, {"Jester"}, 
    {"Sultan"}, {"Stratum"}, {"Elegy"}, {"Raindance"}, {"RC Tiger"}, {"Flash"}, {"Tahoma"}, {"Savanna"}, {"Bandito"}, {"Freight Train Flatbed"}, 
    {"Streak Train Trailer"}, {"Kart"}, {"Mower"}, {"Dune"}, {"Sweeper"}, {"Broadway"}, {"Tornado"}, {"AT-400"}, {"DFT-30"}, {"Huntley"}, 
    {"Stafford"}, {"BF-400"}, {"Newsvan"}, {"Tug"}, {"Trailer (Tanker Commando)"}, {"Emperor"}, {"Wayfarer"}, {"Euros"}, {"Hotdog"}, {"Club"}, 
    {"Box Freight"}, {"Trailer 3"}, {"Andromada"}, {"Dodo"}, {"RC Cam"}, {"Launch"}, {"Police LS", "Police"}, {"Police SF"}, {"Police LV"}, {"Police Ranger", "Ranger"}, 
    {"Picador"}, {"S.W.A.T."}, {"Alpha"}, {"Phoenix"}, {"Glendale Damaged"}, {"Sadler", "Sadler Damaged"}, {"Baggage Trailer (covered)"}, {"Baggage Trailer (Uncovered)"}, {"Trailer (Stairs)"}, {"Boxville Mission"}, 
    {"Farm Trailer"}, {"Street Clean Trailer"}
} };

static const SFixedArray < SVehicleName, 10 > VehicleTypes =
{ {
    {"Automobile"}, {"Plane"}, {"Bike"}, {"Helicopter"}, {"Boat"}, {"Train"}, {"Trailer"}, {"BMX"}, {"Monster Truck"}, {"Quad"}
} };

static const SFixedArray < unsigned char, 212 > ucVehicleTypes =
{ 
    0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0,
    0, 0, 4, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 4, 3, 2, 5, 6, 0, 4, 4, 4, 0,
    0, 0, 0, 0, 1, 2, 2, 2, 1, 3, 0, 0, 2, 3, 0, 9, 4, 4, 0, 0, 1, 0, 0, 0, 0, 7, 0, 0,
    4, 0, 0, 3, 3, 0, 0, 0, 0, 4, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1,
    1, 1, 0, 0, 0, 0, 0, 1, 1, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 1, 0, 0, 8, 8, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0,
    0, 5, 5, 0, 0, 8, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 6, 0, 2, 0, 0, 0, 5, 6, 1, 1, 0, 4,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 0, 6, 6
};

bool CVehicleNames::IsValidModel ( unsigned long ulModel )
{
    return ( // Valid range?
             ulModel >= 400 && ulModel < 610 &&
             
             // Exclude trailers
             ulModel != 435 && ulModel != 450 &&
             ulModel != 591 && ulModel != 606 &&
             ulModel != 607 && ulModel != 584 &&
             ulModel != 608 );
}


const char* CVehicleNames::GetVehicleName ( unsigned long ulModel )
{
    // Valid?
    if ( IsValidModel ( ulModel ) )
    {
        // Look it up in the table
        return VehicleNames [ulModel - 400].szName;
    }

    return szVehicleNameEmpty;
}


unsigned int CVehicleNames::GetVehicleModel ( const char* szName )
{
    // If the specified string was empty, return 0
    if ( szName [0] == 0 ) return 0;

    // Look for it in our table
    for ( unsigned int i = 0; i < NUMELMS( VehicleNames ); i++ )
    {
        if ( stricmp ( szName, VehicleNames [i].szName ) == 0 ||
           ( VehicleNames [i].szName_replaced && stricmp ( szName, VehicleNames [i].szName_replaced ) == 0 ) )
        {
            return i + 400;
        }
    }

    return 0;
}

const char* CVehicleNames::GetVehicleTypeName ( unsigned long ulModel )
{
    // Check whether the model is valid
    if ( IsValidModel ( ulModel ) && ( ( ulModel - 400 ) < NUMELMS( ucVehicleTypes ) ) )
    {
        int iVehicleType = ucVehicleTypes [ulModel - 400];
        return VehicleTypes [iVehicleType].szName;
    }
    
    return szVehicleNameEmpty;
}
