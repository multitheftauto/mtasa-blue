/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CVehicleManager.h
 *  PURPOSE:     Vehicle entity manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

class CVehicleManager;

#pragma once

#include "CVehicle.h"
#include "CVehicleColorManager.h"
#include <list>

// Undefined number of passengers (to disable custom passenger seats overriding in CVehicle)
#define VEHICLE_PASSENGERS_UNDEFINED    255

class CVehicleManager
{
    friend class CVehicle;

public:
    using Container = std::list<CVehicle*>;

    CVehicleManager();
    ~CVehicleManager();

    CVehicle* Create(CElement* pParent, unsigned short usModel, unsigned char ucVariant, unsigned char ucVariant2);
    CVehicle* CreateFromXML(CElement* pParent, CXMLNode& Node, CEvents* pEvents);
    void      DeleteAll();

    bool Exists(CVehicle* pVehicle);

    static bool         IsValidModel(unsigned int uiVehicleModel);
    static eVehicleType GetVehicleType(unsigned short usModel);
    static bool         IsValidUpgrade(unsigned short usUpgrade);
    static unsigned int GetMaxPassengers(unsigned int uiVehicleModel);
    static bool         HasTurret(unsigned int uiVehicleModel);
    static bool         HasSirens(unsigned int uiVehicleModel);
    static bool         HasTaxiLight(unsigned int uiVehicleModel);
    static bool         HasLandingGears(unsigned int uiVehicleModel);
    static bool         HasAdjustableProperty(unsigned int uiVehicleModel);
    static bool         HasSmokeTrail(unsigned int uiVehicleModel);
    static bool         IsTrailer(unsigned int uiVehicleModel);
    static bool         HasDamageModel(unsigned short usModel);
    static bool         HasDamageModel(eVehicleType Type);
    static bool         HasDoors(unsigned short usModel);
    static void         GetRandomVariation(unsigned short usModel, unsigned char& ucVariant, unsigned char& ucVariant2);

    CVehicleColorManager* GetColorManager() { return &m_ColorManager; }
    CVehicleColor         GetRandomColor(unsigned short usModel);

    void GetVehiclesOfType(unsigned int uiModel, lua_State* luaVM);

    Container::size_type GetVehicleCount() { return m_List.size(); }

    Container&       GetVehicles() noexcept { return m_List; }
    const Container& GetVehicles() const noexcept { return m_List; }

    Container&       GetRespawnEnabledVehicles() noexcept { return m_RespawnEnabledVehicles; }
    const Container& GetRespawnEnabledVehicles() const noexcept { return m_RespawnEnabledVehicles; }

private:
    void AddToList(CVehicle* pVehicle) { m_List.push_back(pVehicle); }
    void RemoveFromList(CVehicle* pVehicle);

    CVehicleColorManager m_ColorManager;

    Container m_List;
    Container m_RespawnEnabledVehicles;
};
