/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/CVehicleManager.h
 *  PURPOSE:     Vehicle entity manager class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
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

    CVehicle* Create(CElement* pParent, ushort usModel, uchar ucVariant, uchar ucVariant2);
    CVehicle* CreateFromXML(CElement* pParent, CXMLNode& Node, CEvents* pEvents);
    void      DeleteAll();

    bool Exists(CVehicle* pVehicle);

    static bool         IsValidModel(ushort usModel);
    static eVehicleType GetVehicleType(ushort usModel);
    static bool         IsValidUpgrade(ushort usModel);
    static uchar        GetMaxPassengers(ushort usModel);
    static bool         HasTurret(ushort usModel);
    static bool         HasSirens(ushort usModel);
    static bool         HasTaxiLight(ushort usModel);
    static bool         HasLandingGears(ushort usModel);
    static bool         HasAdjustableProperty(ushort usModel);
    static bool         HasSmokeTrail(ushort usModel);
    static bool         IsTrailer(ushort usModel);
    static bool         HasDamageModel(ushort usModel);
    static bool         HasDamageModel(eVehicleType Type);
    static bool         HasDoors(ushort usModel);
    static void         GetRandomVariation(ushort usModel, uchar& ucVariant, uchar& ucVariant2);

    CVehicleColorManager* GetColorManager() { return &m_ColorManager; }
    CVehicleColor         GetRandomColor(ushort usModel);

    void GetVehiclesOfType(ushort usModel, lua_State* luaVM);

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
