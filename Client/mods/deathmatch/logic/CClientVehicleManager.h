/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CClientVehicleManager.h
 *  PURPOSE:     Vehicle entity manager class header
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CClientVehicle.h"

class CClientManager;
class CClientVehicle;

extern const SFixedArray<uchar, 212> g_ucMaxPassengers;

class CClientVehicleManager
{
public:
    ZERO_ON_NEW
    CClientVehicleManager(CClientManager* pManager);
    ~CClientVehicleManager();

    void DeleteAll();

    ushort                 Count() { return static_cast<ushort>(m_List.size()); };
    static CClientVehicle* Get(ElementID ID);
    CClientVehicle*        GetClosest(CVector& vecPosition, float fRadius);

    void DoPulse();

    static bool               IsTrainModel(ushort usModel);
    static bool               IsValidModel(ushort usModel);
    static bool               IsStandardModel(ushort usModel);
    static eClientVehicleType GetVehicleType(ushort usModel);
    static uchar              GetMaxPassenger(ushort usModel);
    static void               GetRandomVariation(ushort usModel, uchar& ucVariant, uchar& ucVariant2);
    static uchar              ConvertIndexToGameSeat(ushort usModel, uchar ucIndex);

    static bool HasTurret(ushort usModel);
    static bool HasSirens(ushort usModel);
    static bool HasLandingGears(ushort usModel);
    static bool HasAdjustableProperty(ushort usModel);
    static bool HasSmokeTrail(ushort usModel);
    static bool HasTaxiLight(ushort usModel);
    static bool HasSearchLight(ushort usModel);
    static bool HasDamageModel(ushort usModel);
    static bool HasDamageModel(eClientVehicleType Type);
    static bool HasDoors(ushort usModel);

    bool Exists(CClientVehicle* pVehicle);

    static bool IsVehicleLimitReached();

    void RestreamVehicles(ushort usModel);
    void RestreamAllVehicles();
    void RestreamVehicleUpgrades(ushort usModel);

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
