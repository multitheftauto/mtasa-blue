/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CVehicleManager.cpp
 *  PURPOSE:     Vehicle entity manager class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CVehicleManager.h"
#include "Utils.h"
#include "lua/LuaCommon.h"
#include "CGame.h"
#include "models/CModelManager.h"

// List over all vehicles with their special attributes
#define VEHICLE_HAS_TURRENT             0x001UL //1
#define VEHICLE_HAS_SIRENS              0x002UL //2
#define VEHICLE_HAS_LANDING_GEARS       0x004UL //4
#define VEHICLE_HAS_ADJUSTABLE_PROPERTY 0x008UL //8
#define VEHICLE_HAS_SMOKE_TRAIL         0x010UL //16
#define VEHICLE_HAS_TAXI_LIGHTS         0x020UL //32
#define VEHICLE_HAS_SEARCH_LIGHT        0x040UL //64

CVehicleManager::CVehicleManager()
{
}

CVehicleManager::~CVehicleManager()
{
    DeleteAll();
}

CVehicle* CVehicleManager::Create(CElement* pParent, unsigned short usModel, unsigned char ucVariant, unsigned char ucVariant2)
{
    CVehicle* const pVehicle = new CVehicle(this, pParent, usModel, ucVariant, ucVariant2);

    if (pVehicle->GetID() == INVALID_ELEMENT_ID)
    {
        delete pVehicle;
        return nullptr;
    }

    return pVehicle;
}

CVehicle* CVehicleManager::CreateFromXML(CElement* pParent, CXMLNode& Node, CEvents* pEvents)
{
    RandomizeRandomSeed();
    CVehicle* pVehicle = new CVehicle(this, pParent, 400, 254, 254);

    if (pVehicle->GetID() == INVALID_ELEMENT_ID || !pVehicle->LoadFromCustomData(pEvents, Node))
    {
        delete pVehicle;
        return nullptr;
    }

    return pVehicle;
}

void CVehicleManager::DeleteAll()
{
    // Delete all items
    DeletePointersAndClearList(m_List);
}

void CVehicleManager::RemoveFromList(CVehicle* pVehicle)
{
    m_List.remove(pVehicle);
}

bool CVehicleManager::Exists(CVehicle* pVehicle)
{
    return ListContains(m_List, pVehicle);
}

bool CVehicleManager::IsValidModel(unsigned int uiVehicleModel)
{
    if (g_pGame->GetModelManager()->GetVehicleModel(uiVehicleModel))
        return true;

    return false;
}

eVehicleType CVehicleManager::GetVehicleType(unsigned short usModel)
{
    CModelVehicle* pModel = g_pGame->GetModelManager()->GetVehicleModel(usModel);
    if (pModel)
        return pModel->GetVehicleType();

    return eVehicleType::NONE;
}

bool CVehicleManager::IsValidUpgrade(unsigned short usUpgrade)
{
    return (usUpgrade >= 1000 && usUpgrade <= 1193);
}

unsigned int CVehicleManager::GetMaxPassengers(unsigned int uiVehicleModel)
{
    CModelVehicle* pModel = g_pGame->GetModelManager()->GetVehicleModel(uiVehicleModel);
    if (pModel)
    {
        return pModel->GetPassengesCount();
    }

    return 0xFF;
}

void CVehicleManager::GetRandomVariation(unsigned short usModel, unsigned char& ucVariant, unsigned char& ucVariant2)
{
    RandomizeRandomSeed();
    ucVariant = 255;
    ucVariant2 = 255;
    // Valid model?
    char cVariants = g_pGame->GetModelManager()->GetVehicleModel(usModel)->GetVariantsCount();
    if (IsValidModel(usModel) && cVariants != 255)
    {
        // caddy || cropduster
        if (usModel == 457 || usModel == 512)
        {
            // 255, 0, 1, 2
            ucVariant = (rand() % 4) - 1;

            // 3, 4, 5
            ucVariant2 = (rand() % 3);
            ucVariant2 += 3;
            return;
        }
        // Slamvan
        else if (usModel == 535)
        {
            // Slamvan has steering wheel "extras" we want one of those so default cannot be an option.
            ucVariant = (rand() % (cVariants + 1));
            return;
        }
        // NRG 500 || BF400
        else if (usModel == 522 || usModel == 581)
        {
            // e.g. 581 ( BF400 )
            // first 3 properties are Exhaust
            // last 2 are fairings.

            // 255, 0, 1, 2
            ucVariant = (rand() % 4) - 1;

            // 3, 4
            ucVariant2 = (rand() % 2);
            ucVariant2 += 3;
            return;
        }
        // e.g. ( rand () % ( 5 + 2 ) ) - 1
        // Can generate 6 then minus 1 = 5
        // Can generate 0 then minus 1 = -1 (255) (default model with nothing)
        ucVariant = (rand() % (cVariants + 2)) - 1;
    }
}

bool CVehicleManager::HasTurret(unsigned int uiModel)
{
    CModelVehicle* pModel = g_pGame->GetModelManager()->GetVehicleModel(uiModel);
    if (pModel)
        return pModel->GetAttributes() & VEHICLE_HAS_TURRENT;

    return false;
}

bool CVehicleManager::HasSirens(unsigned int uiModel)
{
    CModelVehicle* pModel = g_pGame->GetModelManager()->GetVehicleModel(uiModel);
    if (pModel)
        return pModel->GetAttributes() & VEHICLE_HAS_SIRENS;

    return false;
}

bool CVehicleManager::HasTaxiLight(unsigned int uiModel)
{
    CModelVehicle* pModel = g_pGame->GetModelManager()->GetVehicleModel(uiModel);
    if (pModel)
        return pModel->GetAttributes() & VEHICLE_HAS_TAXI_LIGHTS;

    return false;
}

bool CVehicleManager::HasLandingGears(unsigned int uiModel)
{
    CModelVehicle* pModel = g_pGame->GetModelManager()->GetVehicleModel(uiModel);
    if (pModel)
        return pModel->GetAttributes() & VEHICLE_HAS_LANDING_GEARS;

    return false;
}

bool CVehicleManager::HasAdjustableProperty(unsigned int uiModel)
{
    CModelVehicle* pModel = g_pGame->GetModelManager()->GetVehicleModel(uiModel);
    if (pModel)
        return pModel->GetAttributes() & VEHICLE_HAS_ADJUSTABLE_PROPERTY;

    return false;
}

bool CVehicleManager::HasSmokeTrail(unsigned int uiModel)
{
    CModelVehicle* pModel = g_pGame->GetModelManager()->GetVehicleModel(uiModel);
    if (pModel)
        return pModel->GetAttributes() & VEHICLE_HAS_SMOKE_TRAIL;

    return false;
}

bool CVehicleManager::HasDoors(unsigned short usModel)
{
    CModelVehicle* pModel = g_pGame->GetModelManager()->GetVehicleModel(usModel);
    if (pModel)
        return pModel->HasDoors();

    return false;
}

void CVehicleManager::GetVehiclesOfType(unsigned int uiModel, lua_State* luaVM)
{
    assert(luaVM);

    // Add all the matching vehicles to the table
    unsigned int                   uiIndex = 0;
    std::list<CVehicle*>::iterator iter = m_List.begin();
    for (; iter != m_List.end(); ++iter)
    {
        if ((*iter)->GetModel() == uiModel)
        {
            // Add it to the table
            lua_pushnumber(luaVM, ++uiIndex);
            lua_pushelement(luaVM, *iter);
            lua_settable(luaVM, -3);
        }
    }
}
