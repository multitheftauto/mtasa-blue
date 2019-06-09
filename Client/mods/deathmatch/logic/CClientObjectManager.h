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

class CClientManager;
class CClientObject;
class CObject;

class CClientObjectManager
{
public:
    CClientObjectManager(class CClientManager* pManager);
    ~CClientObjectManager();

    void DoPulse();

    void DeleteAll();

    unsigned int          Count() { return static_cast<unsigned int>(m_Objects.size()); };
    unsigned int          CountCreatedObjects() { return static_cast<unsigned int>(g_pGame->GetPools()->GetObjectCount()); };
    static CClientObject* Get(ElementID ID);

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

    void AddToList(CClientObject* pObject) { m_Objects.push_back(pObject); }
    void RemoveFromLists(CClientObject* pObject);

    void OnCreation(CClientObject* pObject);
    void OnDestruction(CClientObject* pObject);

protected:
    void UpdateLimitInfo();

    int  m_iEntryInfoNodeEntries = 0;
    int  m_iPointerNodeSingleLinkEntries = 0;
    int  m_iPointerNodeDoubleLinkEntries = 0;
    uint m_uiMaxStreamedInCount = 0;
    uint m_uiMaxLowLodStreamedInCount = 0;
    uint m_uiStreamedInCount = 0;
    uint m_uiLowLodStreamedInCount = 0;
    bool m_bDoneLimitWarning = false;

    CClientManager*              m_pManager = nullptr;
    bool                         m_bCanRemoveFromList = false;
    CMappedArray<CClientObject*> m_Objects;
    CMappedArray<CClientObject*> m_StreamedIn;
};
