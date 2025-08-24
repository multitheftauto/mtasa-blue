/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientObjectManager.h
 *  PURPOSE:     Physical object entity manager class
 *
 *****************************************************************************/

#pragma once

#include <game/CPools.h>

class CClientManager;
class CClientObject;
class CObject;

class CClientObjectManager
{
public:
    ZERO_ON_NEW
    CClientObjectManager(class CClientManager* pManager);
    ~CClientObjectManager();

    void DoPulse();

    void DeleteAll();

    const CMappedArray<CClientObject*>& GetObjects() const { return m_Objects; };
    unsigned int                        Count() { return static_cast<unsigned int>(m_Objects.size()); };
    unsigned int                        CountCreatedObjects() { return static_cast<unsigned int>(g_pGame->GetPools()->GetObjectCount()); };
    static CClientObject*               Get(ElementID ID);

    static bool IsValidModel(unsigned long ulObjectModel);
    static bool IsBreakableModel(unsigned long ulObjectModel);
    bool        Exists(CClientObject* pObject);
    bool        ObjectsAroundPointLoaded(const CVector& vecPosition, float fRadius, unsigned short usDimension, SString* pstrStatus = NULL);

    static bool StaticIsObjectLimitReached();
    static bool StaticIsLowLodObjectLimitReached();
    bool        IsObjectLimitReached();
    bool        IsLowLodObjectLimitReached();
    bool        IsHardObjectLimitReached();

    void RestreamObjects(unsigned short usModel);
    void RestreamAllObjects();

    void AddToList(CClientObject* pObject) { m_Objects.push_back(pObject); }
    void RemoveFromLists(CClientObject* pObject);

    void OnCreation(CClientObject* pObject);
    void OnDestruction(CClientObject* pObject);

protected:
    void UpdateLimitInfo();

    int  m_iEntryInfoNodeEntries;
    int  m_iPointerNodeDoubleLinkEntries;
    uint m_uiMaxStreamedInCount;
    uint m_uiMaxLowLodStreamedInCount;
    uint m_uiStreamedInCount;
    uint m_uiLowLodStreamedInCount;
    bool m_bDoneLimitWarning;

    CClientManager*              m_pManager;
    bool                         m_bCanRemoveFromList;
    CMappedArray<CClientObject*> m_Objects;
    CMappedArray<CClientObject*> m_StreamedIn;
};
