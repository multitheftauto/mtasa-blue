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

    unsigned short         Count() { return static_cast<unsigned short>(m_List.size()); };
    static CClientVehicle* Get(ElementID ID);
    CClientVehicle*        GetClosest(CVector& vecPosition, float fRadius);

    void DoPulse();

    static bool               IsTrainModel(unsigned short usModel);
    static bool               IsValidModel(unsigned short usModel);
    static bool               IsStandardModel(unsigned short usModel);
    static eClientVehicleType GetVehicleType(unsigned short usModel);
    static unsigned char      GetMaxPassengerCount(unsigned short usModel);
    static unsigned char      ConvertIndexToGameSeat(unsigned short usModel, unsigned char ucIndex);
    static void               GetRandomVariation(unsigned short usModel, unsigned char& ucVariant, unsigned char& ucVariant2);

    static bool HasTurret(unsigned short usModel);
    static bool HasSirens(unsigned short usModel);
    static bool HasLandingGears(unsigned short usModel);
    static bool HasAdjustableProperty(unsigned short usModel);
    static bool HasSmokeTrail(unsigned short usModel);
    static bool HasTaxiLight(unsigned short usModel);
    static bool HasSearchLight(unsigned short usModel);
    static bool HasDamageModel(unsigned short usModel);
    static bool HasDamageModel(eClientVehicleType Type);
    static bool HasDoors(unsigned short usModel);

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

protected:
    CClientManager*               m_pManager;
    bool                          m_bCanRemoveFromList;
    CMappedArray<CClientVehicle*> m_List;
    CMappedArray<CClientVehicle*> m_StreamedIn;
};
