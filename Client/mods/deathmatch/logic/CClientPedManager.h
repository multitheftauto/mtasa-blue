/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPedManager.h
 *  PURPOSE:     Ped entity manager class
 *
 *****************************************************************************/

#pragma once

#include <list>

class CClientManager;
class CClientPed;
class CPlayerPed;
class CEntity;

class CClientPedManager
{
    friend class CClientManager;
    friend class CClientPed;

public:
    void DeleteAll();

    void        DoPulse(bool bDoStandardPulses);
    CClientPed* Get(ElementID ID, bool bCheckPlayers = false);

    bool Exists(CClientPed* pPed);

    std::vector<CClientPed*>::const_iterator IterBegin() { return m_List.begin(); };
    std::vector<CClientPed*>::const_iterator IterEnd() { return m_List.end(); };
    std::vector<CClientPed*>::const_iterator StreamedBegin() { return m_StreamedIn.begin(); };
    std::vector<CClientPed*>::const_iterator StreamedEnd() { return m_StreamedIn.end(); };

    static unsigned short GetWeaponSlotFromModel(DWORD dwModelID);
    static bool           IsValidWeaponModel(DWORD dwModelID);

    void RestreamPeds(unsigned short usModel);
    void RestreamAllPeds();
    void RestreamWeapon(unsigned short usModel);

protected:
    CClientPedManager(class CClientManager* pManager);
    ~CClientPedManager();

    void AddToList(CClientPed* pPed) { m_List.push_back(pPed); }
    void RemoveFromList(CClientPed* pPed);

    void OnCreation(CClientPed* pPed);
    void OnDestruction(CClientPed* pPed);

    CClientManager*          m_pManager;
    std::vector<CClientPed*> m_List;
    std::vector<CClientPed*> m_StreamedIn;
    bool                     m_bRemoveFromList;
};
