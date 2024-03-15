/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CVehicleManager.h
 *  PURPOSE:     Vehicle entity manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
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

    CVehicle* Create(CElement* pParent, std::uint16_t usModel, std::uint8_t ucVariant, std::uint8_t ucVariant2);
    CVehicle* CreateFromXML(CElement* pParent, CXMLNode& Node, CEvents* pEvents);
    void      DeleteAll();

    bool Exists(CVehicle* pVehicle);

    static bool         IsValidModel(std::uint32_t uiVehicleModel);
    static eVehicleType GetVehicleType(std::uint16_t usModel);
    static bool         IsValidUpgrade(std::uint16_t usUpgrade);
    static std::uint32_t GetMaxPassengers(std::uint32_t uiVehicleModel);
    static bool         HasTurret(std::uint32_t uiVehicleModel);
    static bool         HasSirens(std::uint32_t uiVehicleModel);
    static bool         HasTaxiLight(std::uint32_t uiVehicleModel);
    static bool         HasLandingGears(std::uint32_t uiVehicleModel);
    static bool         HasAdjustableProperty(std::uint32_t uiVehicleModel);
    static bool         HasSmokeTrail(std::uint32_t uiVehicleModel);
    static bool         IsTrailer(std::uint32_t uiVehicleModel);
    static bool         HasDamageModel(std::uint16_t usModel);
    static bool         HasDamageModel(eVehicleType Type);
    static bool         HasDoors(std::uint16_t usModel);
    static void         GetRandomVariation(std::uint16_t usModel, std::uint8_t& ucVariant, std::uint8_t& ucVariant2);

    CVehicleColorManager* GetColorManager() { return &m_ColorManager; }
    CVehicleColor         GetRandomColor(std::uint16_t usModel);

    void GetVehiclesOfType(std::uint32_t uiModel, lua_State* luaVM);

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
