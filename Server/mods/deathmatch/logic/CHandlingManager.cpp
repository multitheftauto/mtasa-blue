/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/CHandlingManager.cpp
 *  PURPOSE:     Vehicle handling manager
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CHandlingManager.h"
#include "CHandlingConfig.h"
#include "CCommon.h"
#include "CVehicleManager.h"
#include "CGame.h"
#include "models/CModelManager.h"

// Original handling data
static tHandlingData                   m_OriginalHandlingData[HT_MAX];
static std::unique_ptr<CHandlingEntry> m_OriginalEntries[HT_MAX];

// Model handling data
static std::unordered_map<std::size_t, std::unique_ptr<CHandlingEntry>> m_ModelEntries;
static std::unordered_map<std::size_t, bool>                            m_bModelHandlingChanged;

static std::map<std::string, eHandlingProperty> m_HandlingNames;

CHandlingManager::CHandlingManager()
{
    // Initialize all default handlings
    InitializeDefaultHandlings();

    // Create a handling entry
    for (std::size_t i = 0; i < HT_MAX; i++)
    {
        // For every original handling data
        m_OriginalEntries[i] = std::make_unique<CHandlingEntry>(&m_OriginalHandlingData[i]);
    }

    // https://www.gtamodding.com/index.php?title=Handling.cfg#GTA_San_Andreas
    // https://projectcerbera.com/gta/sa/tutorials/handling

    m_HandlingNames["mass"] = HANDLING_MASS;                                                   // works (mass > 0)
    m_HandlingNames["turnMass"] = HANDLING_TURNMASS;                                           // works
    m_HandlingNames["dragCoeff"] = HANDLING_DRAGCOEFF;                                         // works
    m_HandlingNames["centerOfMass"] = HANDLING_CENTEROFMASS;                                   // works
    m_HandlingNames["percentSubmerged"] = HANDLING_PERCENTSUBMERGED;                           // works
    m_HandlingNames["tractionMultiplier"] = HANDLING_TRACTIONMULTIPLIER;                       // works
    m_HandlingNames["driveType"] = HANDLING_DRIVETYPE;                                         // works
    m_HandlingNames["engineType"] = HANDLING_ENGINETYPE;                                       // works
    m_HandlingNames["numberOfGears"] = HANDLING_NUMOFGEARS;                                    // works
    m_HandlingNames["engineAcceleration"] = HANDLING_ENGINEACCELERATION;                       // works
    m_HandlingNames["engineInertia"] = HANDLING_ENGINEINERTIA;                                 // works
    m_HandlingNames["maxVelocity"] = HANDLING_MAXVELOCITY;                                     // works
    m_HandlingNames["brakeDeceleration"] = HANDLING_BRAKEDECELERATION;                         // works
    m_HandlingNames["brakeBias"] = HANDLING_BRAKEBIAS;                                         // works
    m_HandlingNames["ABS"] = HANDLING_ABS;                                                     // has no effect in vanilla gta either
    m_HandlingNames["steeringLock"] = HANDLING_STEERINGLOCK;                                   // works
    m_HandlingNames["tractionLoss"] = HANDLING_TRACTIONLOSS;                                   // works
    m_HandlingNames["tractionBias"] = HANDLING_TRACTIONBIAS;                                   // works
    m_HandlingNames["suspensionForceLevel"] = HANDLING_SUSPENSION_FORCELEVEL;                  // works
    m_HandlingNames["suspensionDamping"] = HANDLING_SUSPENSION_DAMPING;                        // works
    m_HandlingNames["suspensionHighSpeedDamping"] = HANDLING_SUSPENSION_HIGHSPEEDDAMPING;      // works
    m_HandlingNames["suspensionUpperLimit"] = HANDLING_SUSPENSION_UPPER_LIMIT;                 // works
    m_HandlingNames["suspensionLowerLimit"] = HANDLING_SUSPENSION_LOWER_LIMIT;                 // works
    m_HandlingNames["suspensionFrontRearBias"] = HANDLING_SUSPENSION_FRONTREARBIAS;            // works
    m_HandlingNames["suspensionAntiDiveMultiplier"] = HANDLING_SUSPENSION_ANTIDIVEMULTIPLIER;  // works
    m_HandlingNames["collisionDamageMultiplier"] = HANDLING_COLLISIONDAMAGEMULTIPLIER;         // works
    m_HandlingNames["seatOffsetDistance"] = HANDLING_SEATOFFSETDISTANCE;                       // works
    m_HandlingNames["monetary"] = HANDLING_MONETARY;            // useless as it only influences SP stats (value of damaged property)
    m_HandlingNames["handlingFlags"] = HANDLING_HANDLINGFLAGS;  // works
    m_HandlingNames["modelFlags"] = HANDLING_MODELFLAGS;        // works
    m_HandlingNames["headLight"] = HANDLING_HEADLIGHT;          // doesn't work
    m_HandlingNames["tailLight"] = HANDLING_TAILLIGHT;          // doesn't seem to work*
    m_HandlingNames["animGroup"] = HANDLING_ANIMGROUP;          // works model based
}
//* needs testing by someone who knows more about handling

CHandlingManager::~CHandlingManager()
{
}

std::unique_ptr<CHandlingEntry> CHandlingManager::CreateHandlingData() const noexcept
{
    return std::make_unique<CHandlingEntry>();
}

bool CHandlingManager::ApplyHandlingData(std::uint32_t model, CHandlingEntry* pEntry) const noexcept
{
    CHandlingEntry* pHandling = GetModelHandlingData(model);
    if (!pHandling)
        return false;

    pHandling->ApplyHandlingData(pEntry);
    return true;
}

const CHandlingEntry* CHandlingManager::GetOriginalHandlingData(std::uint32_t model) const noexcept
{
    // Within range?
    if (!CVehicleManager::IsValidModel(model))
        return nullptr;

    // Get our Handling ID
    const eHandlingTypes eHandling = GetHandlingID(model);

    // Return it
    return m_OriginalEntries[eHandling].get();
}

CHandlingEntry* CHandlingManager::GetModelHandlingData(std::uint32_t model) const noexcept
{
    // Within range?
    if (!CVehicleManager::IsValidModel(model))
        return nullptr;

    auto entries = m_ModelEntries.find(model);
    if (entries == m_ModelEntries.end())
    {
        // Get our Handling ID
        const eHandlingTypes eHandling = GetHandlingID(model);

        m_ModelEntries[model] = std::make_unique<CHandlingEntry>(&m_OriginalHandlingData[eHandling]);
        if (!m_ModelEntries[model])
            return nullptr;

        entries = m_ModelEntries.find(model);
    }

    return entries->second.get();
}

eHandlingProperty CHandlingManager::GetPropertyEnumFromName(const std::string& name) const noexcept
{
    const auto it = m_HandlingNames.find(name);
    return it != m_HandlingNames.end() ? it->second : HANDLING_MAX;
}

bool CHandlingManager::HasModelHandlingChanged(std::uint32_t model) const noexcept
{
    // Within range?
    if (!CVehicleManager::IsValidModel(model))
        return false;

    // Return if we have changed
    return m_bModelHandlingChanged[model];
}

void CHandlingManager::SetModelHandlingHasChanged(std::uint32_t model, bool bChanged) const noexcept
{
    // Within range?
    if (!CVehicleManager::IsValidModel(model))
        return;

    // Return if we have changed.
    m_bModelHandlingChanged[model] = bChanged;
}

// Return the handling manager id
eHandlingTypes CHandlingManager::GetHandlingID(std::uint32_t model) const noexcept
{
    if (g_pGame && g_pGame->GetModelManager())
    {
        auto modelPtr = g_pGame->GetModelManager()->FindModel(model);
        if (modelPtr && modelPtr->IsCustom())
        {
            std::uint32_t baseModel = modelPtr->GetParentModelId();
            if (baseModel != model)
                return GetHandlingID(baseModel);
        }
    }

    switch (model)
    {
        case VT_LANDSTAL:
            return HT_LANDSTAL;
        case VT_BRAVURA:
            return HT_BRAVURA;
        case VT_BUFFALO:
            return HT_BUFFALO;
        case VT_LINERUN:
            return HT_LINERUN;
        case VT_PEREN:
            return HT_PEREN;
        case VT_SENTINEL:
            return HT_SENTINEL;
        case VT_DUMPER:
            return HT_DUMPER;
        case VT_FIRETRUK:
            return HT_FIRETRUK;
        case VT_TRASH:
            return HT_TRASH;
        case VT_STRETCH:
            return HT_STRETCH;
        case VT_MANANA:
            return HT_MANANA;
        case VT_INFERNUS:
            return HT_INFERNUS;
        case VT_VOODOO:
            return HT_VOODOO;
        case VT_PONY:
            return HT_PONY;
        case VT_MULE:
            return HT_MULE;
        case VT_CHEETAH:
            return HT_CHEETAH;
        case VT_AMBULAN:
            return HT_AMBULAN;
        case VT_LEVIATHN:
            return HT_LEVIATHN;
        case VT_MOONBEAM:
            return HT_MOONBEAM;
        case VT_ESPERANT:
            return HT_ESPERANT;
        case VT_TAXI:
            return HT_TAXI;
        case VT_WASHING:
            return HT_WASHING;
        case VT_BOBCAT:
            return HT_BOBCAT;
        case VT_MRWHOOP:
            return HT_MRWHOOP;
        case VT_BFINJECT:
            return HT_BFINJECT;
        case VT_HUNTER:
            return HT_HUNTER;
        case VT_PREMIER:
            return HT_PREMIER;
        case VT_ENFORCER:
            return HT_ENFORCER;
        case VT_SECURICA:
            return HT_SECURICA;
        case VT_BANSHEE:
            return HT_BANSHEE;
        case VT_PREDATOR:
            return HT_PREDATOR;
        case VT_BUS:
            return HT_BUS;
        case VT_RHINO:
            return HT_RHINO;
        case VT_BARRACKS:
            return HT_BARRACKS;
        case VT_HOTKNIFE:
            return HT_HOTKNIFE;
        case VT_ARTICT1:
            return HT_ARTICT1;
        case VT_PREVION:
            return HT_PREVION;
        case VT_COACH:
            return HT_COACH;
        case VT_CABBIE:
            return HT_CABBIE;
        case VT_STALLION:
            return HT_STALLION;
        case VT_RUMPO:
            return HT_RUMPO;
        case VT_RCBANDIT:
            return HT_RCBANDIT;
        case VT_ROMERO:
            return HT_ROMERO;
        case VT_PACKER:
            return HT_PACKER;
        case VT_MONSTER:
            return HT_MONSTER;
        case VT_ADMIRAL:
            return HT_ADMIRAL;
        case VT_SQUALO:
            return HT_SQUALO;
        case VT_SEASPAR:
            return HT_SEASPAR;
        case VT_PIZZABOY:
            return HT_PIZZABOY;
        case VT_TRAM:
            return HT_TRAM;
        case VT_ARTICT2:
            return HT_ARTICT2;
        case VT_TURISMO:
            return HT_TURISMO;
        case VT_SPEEDER:
            return HT_SPEEDER;
        case VT_REEFER:
            return HT_REEFER;
        case VT_TROPIC:
            return HT_TROPIC;
        case VT_FLATBED:
            return HT_FLATBED;
        case VT_YANKEE:
            return HT_YANKEE;
        case VT_CADDY:
            return HT_GOLFCART;
        case VT_SOLAIR:
            return HT_SOLAIR;
        case VT_TOPFUN:
            return HT_TOPFUN;
        case VT_SKIMMER:
            return HT_SEAPLANE;
        case VT_PCJ600:
            return HT_BIKE;
        case VT_FAGGIO:
            return HT_FAGGIO;
        case VT_FREEWAY:
            return HT_FREEWAY;
        case VT_RCBARON:
            return HT_RCBARON;
        case VT_RCRAIDER:
            return HT_RCRAIDER;
        case VT_GLENDALE:
            return HT_GLENDALE;
        case VT_OCEANIC:
            return HT_OCEANIC;
        case VT_SANCHEZ:
            return HT_DIRTBIKE;
        case VT_SPARROW:
            return HT_SPARROW;
        case VT_PATRIOT:
            return HT_PATRIOT;
        case VT_QUAD:
            return HT_QUADBIKE;
        case VT_COASTG:
            return HT_COASTGRD;
        case VT_DINGHY:
            return HT_DINGHY;
        case VT_HERMES:
            return HT_HERMES;
        case VT_SABRE:
            return HT_SABRE;
        case VT_RUSTLER:
            return HT_RUSTLER;
        case VT_ZR350:
            return HT_ZR350;
        case VT_WALTON:
            return HT_WALTON;
        case VT_REGINA:
            return HT_REGINA;
        case VT_COMET:
            return HT_COMET;
        case VT_BMX:
            return HT_BMX;
        case VT_BURRITO:
            return HT_BURRITO;
        case VT_CAMPER:
            return HT_CAMPER;
        case VT_MARQUIS:
            return HT_MARQUIS;
        case VT_BAGGAGE:
            return HT_BAGGAGE;
        case VT_DOZER:
            return HT_DOZER;
        case VT_MAVERICK:
            return HT_MAVERICK;
        case VT_VCNMAV:
            return HT_COASTMAV;
        case VT_RANCHER:
            return HT_RANCHER;
        case VT_FBIRANCH:
            return HT_FBIRANCH;
        case VT_VIRGO:
            return HT_VIRGO;
        case VT_GREENWOO:
            return HT_GREENWOO;
        case VT_JETMAX:
            return HT_CUPBOAT;
        case VT_HOTRING:
            return HT_HOTRING;
        case VT_SANDKING:
            return HT_SANDKING;
        case VT_BLISTAC:
            return HT_BLISTAC;
        case VT_POLMAV:
            return HT_POLMAV;
        case VT_BOXVILLE:
            return HT_BOXVILLE;
        case VT_BENSON:
            return HT_BENSON;
        case VT_MESA:
            return HT_MESA;
        case VT_RCGOBLIN:
            return HT_RCGOBLIN;
        case VT_HOTRINA:
            return HT_HOTRINA;
        case VT_HOTRINB:
            return HT_HOTRINB;
        case VT_BLOODRA:
            return HT_BLOODRA;
        case VT_RNCHLURE:
            return HT_RNCHLURE;
        case VT_SUPERGT:
            return HT_SUPERGT;
        case VT_ELEGANT:
            return HT_ELEGANT;
        case VT_JOURNEY:
            return HT_JOURNEY;
        case VT_BIKE:
            return HT_CHOPPERB;
        case VT_MTBIKE:
            return HT_MTB;
        case VT_BEAGLE:
            return HT_BEAGLE;
        case VT_CROPDUST:
            return HT_CROPDUST;
        case VT_STUNT:
            return HT_STUNT;
        case VT_PETRO:
            return HT_PETROL;
        case VT_RDTRAIN:
            return HT_RDTRAIN;
        case VT_NEBULA:
            return HT_NEBULA;
        case VT_MAJESTIC:
            return HT_MAJESTIC;
        case VT_BUCCANEE:
            return HT_BUCCANEE;
        case VT_SHAMAL:
            return HT_SHAMAL;
        case VT_HYDRA:
            return HT_HYDRA;
        case VT_FCR900:
            return HT_FCR900;
        case VT_NRG500:
            return HT_NRG500;
        case VT_COPBIKE:
            return HT_HPV1000;
        case VT_CEMENT:
            return HT_CEMENT;
        case VT_TOWTRUCK:
            return HT_TOWTRUCK;
        case VT_FORTUNE:
            return HT_FORTUNE;
        case VT_CADRONA:
            return HT_CADRONA;
        case VT_FBITRUCK:
            return HT_FBITRUCK;
        case VT_WILLARD:
            return HT_WILLARD;
        case VT_FORKLIFT:
            return HT_FORKLIFT;
        case VT_TRACTOR:
            return HT_TRACTOR;
        case VT_COMBINE:
            return HT_COMBINE;
        case VT_FELTZER:
            return HT_FELTZER;
        case VT_REMINGTN:
            return HT_REMINGTN;
        case VT_SLAMVAN:
            return HT_SLAMVAN;
        case VT_BLADE:
            return HT_BLADE;
        case VT_FREIGHT:
            return HT_FREIGHT;
        case VT_STREAK:
            return HT_STREAK;
        case VT_VORTEX:
            return HT_VORTEX;
        case VT_VINCENT:
            return HT_VINCENT;
        case VT_BULLET:
            return HT_BULLET;
        case VT_CLOVER:
            return HT_CLOVER;
        case VT_SADLER:
            return HT_SADLER;
        case VT_FIRELA:
            return HT_FIRELA;
        case VT_HUSTLER:
            return HT_HUSTLER;
        case VT_INTRUDER:
            return HT_INTRUDER;
        case VT_PRIMO:
            return HT_PRIMO;
        case VT_CARGOBOB:
            return HT_CARGOBOB;
        case VT_TAMPA:
            return HT_TAMPA;
        case VT_SUNRISE:
            return HT_SUNRISE;
        case VT_MERIT:
            return HT_MERIT;
        case VT_UTILITY:
            return HT_UTILITY;
        case VT_NEVADA:
            return HT_NEVADA;
        case VT_YOSEMITE:
            return HT_YOSEMITE;
        case VT_WINDSOR:
            return HT_WINDSOR;
        case VT_MONSTERA:
            return HT_MTRUCK_A;
        case VT_MONSTERB:
            return HT_MTRUCK_B;
        case VT_URANUS:
            return HT_URANUS;
        case VT_JESTER:
            return HT_JESTER;
        case VT_SULTAN:
            return HT_SULTAN;
        case VT_STRATUM:
            return HT_STRATUM;
        case VT_ELEGY:
            return HT_ELEGY;
        case VT_RAINDANC:
            return HT_RAINDANC;
        case VT_RCTIGER:
            return HT_RCTIGER;
        case VT_FLASH:
            return HT_FLASH;
        case VT_TAHOMA:
            return HT_TAHOMA;
        case VT_SAVANNA:
            return HT_SAVANNA;
        case VT_BANDITO:
            return HT_BANDITO;
        case VT_FREIFLAT:
            return HT_FREIFLAT;
        case VT_STREAKC:
            return HT_CSTREAK;
        case VT_KART:
            return HT_KART;
        case VT_MOWER:
            return HT_MOWER;
        case VT_DUNERIDE:
            return HT_DUNE;
        case VT_SWEEPER:
            return HT_SWEEPER;
        case VT_BROADWAY:
            return HT_BROADWAY;
        case VT_TORNADO:
            return HT_TORNADO;
        case VT_AT400:
            return HT_AT400;
        case VT_DFT30:
            return HT_DFT30;
        case VT_HUNTLEY:
            return HT_HUNTLEY;
        case VT_STAFFORD:
            return HT_STAFFORD;
        case VT_BF400:
            return HT_BF400;
        case VT_NEWSVAN:
            return HT_NEWSVAN;
        case VT_TUG:
            return HT_TUG;
        case VT_PETROTR:
            return HT_PETROTR;
        case VT_EMPEROR:
            return HT_EMPEROR;
        case VT_WAYFARER:
            return HT_WAYFARER;
        case VT_EUROS:
            return HT_EUROS;
        case VT_HOTDOG:
            return HT_HOTDOG;
        case VT_CLUB:
            return HT_CLUB;
        case VT_FREIBOX:
            return HT_FREIBOX;
        case VT_ARTICT3:
            return HT_ARTICT3;
        case VT_ANDROM:
            return HT_ANDROM;
        case VT_DODO:
            return HT_DODO;
        case VT_RCCAM:
            return HT_RCCAM;
        case VT_LAUNCH:
            return HT_LAUNCH;
        case VT_COPCARLA:
            return HT_POLICE_LA;
        case VT_COPCARSF:
            return HT_POLICE_SF;
        case VT_COPCARVG:
            return HT_POLICE_VG;
        case VT_COPCARRU:
            return HT_POLRANGER;
        case VT_PICADOR:
            return HT_PICADOR;
        case VT_SWATVAN:
            return HT_SWATVAN;
        case VT_ALPHA:
            return HT_ALPHA;
        case VT_PHOENIX:
            return HT_PHOENIX;
        case VT_GLENSHIT:
            return HT_GLENSHIT;
        case VT_SADLSHIT:
            return HT_SADLSHIT;
        case VT_BAGBOXA:
            return HT_BAGBOXA;
        case VT_BAGBOXB:
            return HT_BAGBOXB;
        case VT_TUGSTAIR:
            return HT_STAIRS;
        case VT_BOXBURG:
            return HT_BOXBURG;
        case VT_FARMTR1:
            return HT_FARM_TR1;
        case VT_UTILTR1:
            return HT_UTIL_TR1;
        default:
            break;
    }
    return HT_LANDSTAL;
}

void CHandlingManager::InitializeDefaultHandlings() noexcept
{
    // Reset
    memset(&m_OriginalHandlingData[0], 0, sizeof(m_OriginalHandlingData));

    // Load original handling data from handling.conf
    CHandlingConfig config("mods/deathmatch/handling.conf");
    if (!config.Load(this, m_OriginalHandlingData))
    {
        CHandlingConfig fallbackConfig("handling.conf");
        fallbackConfig.Load(this, m_OriginalHandlingData);
    }

    // These vehicles share handling lines with other models in the original game.
    // We give them separate MTA entries so scripts can customize each independently,
    // but we preserve the source entry's iVehicleID because GTA:SA engine code may
    // use it to index into the global handling array (which only has 210 entries).
    // Using out-of-range IDs (210+) would cause out-of-bounds reads and incorrect
    // vehicle physics (e.g. wrong reverse speed).
    m_OriginalHandlingData[210] = m_OriginalHandlingData[69];   // HT_HOTRINA = HT_HOTRING
    m_OriginalHandlingData[211] = m_OriginalHandlingData[69];   // HT_HOTRINB = HT_HOTRING
    m_OriginalHandlingData[212] = m_OriginalHandlingData[103];  // HT_SADLSHIT = HT_SADLER
    m_OriginalHandlingData[213] = m_OriginalHandlingData[52];   // HT_GLENSHIT = HT_GLENDALE
    m_OriginalHandlingData[214] = m_OriginalHandlingData[163];  // HT_FAGGIO = HT_PIZZABOY
    m_OriginalHandlingData[215] = m_OriginalHandlingData[7];    // HT_FIRELA = HT_FIRETRUK
    m_OriginalHandlingData[216] = m_OriginalHandlingData[65];   // HT_RNCHLURE = HT_RANCHER
    m_OriginalHandlingData[217] = m_OriginalHandlingData[126];  // HT_FREIBOX = HT_FREIFLAT
}
