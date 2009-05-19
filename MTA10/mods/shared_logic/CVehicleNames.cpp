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

char szVehicleNameEmpty [] = "";

struct SVehicleName
{
    char szName [32];
};

SVehicleName VehicleNames [611] = { {"Landstalker"}, {"Bravura"}, {"Buffalo"}, {"Linerunner"}, {"Perennial"}, {"Sentinel"}, {"Dumper"}, {"Fire Truck"}, {"Trashmaster"}, {"Stretch"}, 
{"Manana"}, {"Infernus"}, {"Voodoo"}, {"Pony"}, {"Mule"}, {"Cheetah"}, {"Ambulance"}, {"Leviathan"}, {"Moonbeam"}, {"Esperanto"}, 
{"Taxi"}, {"Washington"}, {"Bobcat"}, {"Mr. Whoopee"}, {"BF Injection"}, {"Hunter"}, {"Premier"}, {"Enforcer"}, {"Securicar"}, {"Banshee"}, 
{"Predator"}, {"Bus"}, {"Rhino"}, {"Barracks"}, {"Hotknife"}, {""}, {"Previon"}, {"Coach"}, {"Cabbie"}, {"Stallion"}, 
{"Rumpo"}, {"RC Bandit"}, {"Romero"}, {"Packer"}, {"Monster"}, {"Admiral"}, {"Squalo"}, {"Seasparrow"}, {"Pizzaboy"}, {""}, 
{""}, {"Turismo"}, {"Speeder"}, {"Reefer"}, {"Tropic"}, {"Flatbed"}, {"Yankee"}, {"Caddy"}, {"Solair"}, {"Berkley's RC Van"}, 
{"Skimmer"}, {"PCJ-600"}, {"Faggio"}, {"Freeway"}, {"RC Baron"}, {"RC Raider"}, {"Glendale"}, {"Oceanic"}, {"Sanchez"}, {"Sparrow"}, 
{"Patriot"}, {"Quadbike"}, {"Coastguard"}, {"Dinghy"}, {"Hermes"}, {"Sabre"}, {"Rustler"}, {"ZR-350"}, {"Walton"}, {"Regina"}, 
{"Comet"}, {"BMX"}, {"Burrito"}, {"Camper"}, {"Marquis"}, {"Baggage"}, {"Dozer"}, {"Maverick"}, {"News Chopper"}, {"Rancher"}, 
{"FBI Rancher"}, {"Virgo"}, {"Greenwood"}, {"Jetmax"}, {"Hotring Racer"}, {"Sandking"}, {"Blista Compact"}, {"Police Maverick"}, {"Boxville"}, {"Benson"}, 
{"Mesa"}, {"RC Goblin"}, {"Hotring Racer"}, {"Hotring Racer"}, {"Bloodring Banger"}, {"Rancher"}, {"Super GT"}, {"Elegant"}, {"Journey"}, {"Bike"}, 
{"Mountain Bike"}, {"Beagle"}, {"Cropduster"}, {"Stuntplane"}, {"Tanker"}, {"Roadtrain"}, {"Nebula"}, {"Majestic"}, {"Buccaneer"}, {"Shamal"}, 
{"Hydra"}, {"FCR-900"}, {"NRG-500"}, {"HPV1000"}, {"Cement Truck"}, {"Towtruck"}, {"Fortune"}, {"Cadrona"}, {"FBI Truck"}, {"Willard"}, 
{"Forklift"}, {"Tractor"}, {"Combine Harvester"}, {"Feltzer"}, {"Remington"}, {"Slamvan"}, {"Blade"}, {"Freight"}, {"Streak"}, {"Vortex"}, 
{"Vincent"}, {"Bullet"}, {"Clover"}, {"Sadler"}, {"Fire Truck"}, {"Hustler"}, {"Intruder"}, {"Primo"}, {"Cargobob"}, {"Tampa"}, 
{"Sunrise"}, {"Merit"}, {"Utility Van"}, {"Nevada"}, {"Yosemite"}, {"Windsor"}, {"Monster"}, {"Monster"}, {"Uranus"}, {"Jester"}, 
{"Sultan"}, {"Stratum"}, {"Elegy"}, {"Raindance"}, {"RC Tiger"}, {"Flash"}, {"Tahoma"}, {"Savanna"}, {"Bandito"}, {""}, 
{""}, {"Kart"}, {"Mower"}, {"Dune"}, {"Sweeper"}, {"Broadway"}, {"Tornado"}, {"AT-400"}, {"DFT-30"}, {"Huntley"}, 
{"Stafford"}, {"BF-400"}, {"Newsvan"}, {"Tug"}, {""}, {"Emperor"}, {"Wayfarer"}, {"Euros"}, {"Hotdog"}, {"Club"}, 
{""}, {""}, {"Andromada"}, {"Dodo"}, {"RC Cam"}, {"Launch"}, {"Police"}, {"Police"}, {"Police"}, {"Ranger"}, 
{"Picador"}, {"S.W.A.T."}, {"Alpha"}, {"Phoenix"}, {"Glendale"}, {"Sadler"}, {""}, {""}, {""}, {"Boxville"}, 
{""} };

SVehicleName VehicleTypes [] = { {"Automobile"}, {"Plane"}, {"Bike"}, {"Helicopter"}, {"Boat"}, {"Train"}, {"Trailer"}, {"BMX"}, {"Monster Truck"}, {"Quad"} };

unsigned char ucVehicleTypes [] = { 
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
    for ( unsigned int i = 0; i <= 211; i++ )
    {
        if ( stricmp ( szName, VehicleNames [i].szName ) == 0 )
        {
            return i + 400;
        }
    }

    return 0;
}

const char* CVehicleNames::GetVehicleTypeName ( unsigned long ulModel )
{
	// Check whether the model is valid
	if ( IsValidModel ( ulModel ) && ((ulModel-400) < sizeof(ucVehicleTypes)) )
	{
		int iVehicleType = ucVehicleTypes [ulModel - 400];
		return VehicleTypes [iVehicleType].szName;
	}
	
	return szVehicleNameEmpty;
}
