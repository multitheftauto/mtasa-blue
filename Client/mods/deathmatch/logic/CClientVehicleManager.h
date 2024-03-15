/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientVehicleManager.h
 *  PURPOSE:     Vehicle entity manager class header
 *
 *****************************************************************************/

#pragma once

#include "CClientVehicle.h"

class CClientManager;
class CClientVehicle;

extern const SFixedArray<std::uint8_t, 212> g_ucMaxPassengers;

class CClientVehicleManager
{
public:
    ZERO_ON_NEW
    CClientVehicleManager(CClientManager* pManager);
    ~CClientVehicleManager();

    void DeleteAll();

    std::uint32_t           Count() { return static_cast<std::uint32_t>(m_List.size()); };
    static CClientVehicle* Get(ElementID ID);
    CClientVehicle*        GetClosest(CVector& vecPosition, float fRadius);

    void DoPulse();

    static bool               IsTrainModel(std::uint32_t ulModel);
    static bool               IsValidModel(std::uint32_t ulModel);
    static bool               IsStandardModel(std::uint32_t ulModel);
    static eClientVehicleType GetVehicleType(std::uint32_t ulModel);
    static std::uint8_t      GetMaxPassengerCount(std::uint32_t ulModel);
    static std::uint8_t      ConvertIndexToGameSeat(std::uint32_t ulModel, std::uint8_t ucIndex);
    static void               GetRandomVariation(std::uint16_t usModel, std::uint8_t& ucVariant, std::uint8_t& ucVariant2);

    static bool HasTurret(std::uint32_t ulModel);
    static bool HasSirens(std::uint32_t ulModel);
    static bool HasLandingGears(std::uint32_t ulModel);
    static bool HasAdjustableProperty(std::uint32_t ulModel);
    static bool HasSmokeTrail(std::uint32_t ulModel);
    static bool HasTaxiLight(std::uint32_t ulModel);
    static bool HasSearchLight(std::uint32_t ulModel);
    static bool HasDamageModel(std::uint32_t ulModel);
    static bool HasDamageModel(enum eClientVehicleType Type);
    static bool HasDoors(std::uint32_t ulModel);

    bool Exists(CClientVehicle* pVehicle);

    static bool IsVehicleLimitReached();

    void RestreamVehicles(std::uint16_t usModel);
    void RestreamAllVehicles();
    void RestreamVehicleUpgrades(std::uint16_t usModel);

    std::vector<CClientVehicle*>::const_iterator IterBegin() { return m_List.begin(); };
    std::vector<CClientVehicle*>::const_iterator IterEnd() { return m_List.end(); };
    std::vector<CClientVehicle*>::const_iterator StreamedBegin() { return m_StreamedIn.begin(); };
    std::vector<CClientVehicle*>::const_iterator StreamedEnd() { return m_StreamedIn.end(); };

    void AddToList(CClientVehicle* pVehicle) { m_List.push_back(pVehicle); };
    void RemoveFromLists(CClientVehicle* pVehicle);

    void OnCreation(CClientVehicle* pVehicle);
    void OnDestruction(CClientVehicle* pVehicle);

protected:
    CClientManager*               m_pManager;
    bool                          m_bCanRemoveFromList;
    CMappedArray<CClientVehicle*> m_List;
    CMappedArray<CClientVehicle*> m_StreamedIn;
};
