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

extern const SFixedArray<unsigned char, 212> g_ucMaxPassengers;

class CClientVehicleManager
{
public:
    ZERO_ON_NEW
    CClientVehicleManager(CClientManager* pManager);
    ~CClientVehicleManager();

    void DeleteAll();

    unsigned int           Count() { return static_cast<unsigned int>(m_List.size()); };
    static CClientVehicle* Get(ElementID ID);
    CClientVehicle*        GetClosest(CVector& vecPosition, float fRadius);

    void DoPulse();

    static bool               IsTrainModel(unsigned long ulModel);
    static bool               IsValidModel(unsigned long ulModel);
    static bool               IsStandardModel(unsigned long ulModel);
    static eClientVehicleType GetVehicleType(unsigned long ulModel);
    static unsigned char      GetMaxPassengerCount(unsigned long ulModel);
    static unsigned char      ConvertIndexToGameSeat(unsigned long ulModel, unsigned char ucIndex);
    static void               GetRandomVariation(unsigned short usModel, unsigned char& ucVariant, unsigned char& ucVariant2);

    static bool HasTurret(unsigned long ulModel);
    static bool HasSirens(unsigned long ulModel);
    static bool HasLandingGears(unsigned long ulModel);
    static bool HasAdjustableProperty(unsigned long ulModel);
    static bool HasSmokeTrail(unsigned long ulModel);
    static bool HasTaxiLight(unsigned long ulModel);
    static bool HasSearchLight(unsigned long ulModel);
    static bool HasDamageModel(unsigned long ulModel);
    static bool HasDamageModel(enum eClientVehicleType Type);
    static bool HasDoors(unsigned long ulModel);

    bool Exists(CClientVehicle* pVehicle);

    static bool IsVehicleLimitReached();

    void RestreamVehicles(unsigned short usModel);
    void RestreamAllVehicles();
    void RestreamVehicleUpgrades(unsigned short usModel);

    std::vector<CClientVehicle*>::const_iterator IterBegin() { return m_List.begin(); };
    std::vector<CClientVehicle*>::const_iterator IterEnd() { return m_List.end(); };
    std::vector<CClientVehicle*>::const_iterator StreamedBegin() { return m_StreamedIn.begin(); };
    std::vector<CClientVehicle*>::const_iterator StreamedEnd() { return m_StreamedIn.end(); };

    void AddToList(CClientVehicle* pVehicle) { m_List.push_back(pVehicle); };
    void RemoveFromLists(CClientVehicle* pVehicle);

    void OnCreation(CClientVehicle* pVehicle);
    void OnDestruction(CClientVehicle* pVehicle);

    bool IsSpawnFlyingComponentEnabled() const noexcept { return m_spawnFlyingComponentsDuringRecreate; }
    void SetSpawnFlyingComponentEnabled(bool isEnabled) noexcept { m_spawnFlyingComponentsDuringRecreate = isEnabled; }

protected:
    CClientManager*               m_pManager;
    bool                          m_bCanRemoveFromList;
    CMappedArray<CClientVehicle*> m_List;
    CMappedArray<CClientVehicle*> m_StreamedIn;
    bool                          m_spawnFlyingComponentsDuringRecreate{true};
};
