/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CElement.h
 *  PURPOSE:     Base entity (element) class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <core/CServerInterface.h>
#include <CVector.h>
#include "CMapEventManager.h"
#include "CCustomData.h"
#include "CEvents.h"
#include <list>
#include <cstring>
#include "Enums.h"
#include "CElementGroup.h"

// Used to check fast version of getElementsByType
// #define CHECK_ENTITIES_FROM_ROOT  MTA_DEBUG

#define IS_BLIP(element)     ((element)->GetType()==CElement::BLIP)
#define IS_COLSHAPE(element) ((element)->GetType()==CElement::COLSHAPE)
#define IS_DUMMY(element)    ((element)->GetType()==CElement::DUMMY)
#define IS_FILE(element)     ((element)->GetType()==CElement::SCRIPTFILE)
#define IS_MARKER(element)   ((element)->GetType()==CElement::MARKER)
#define IS_OBJECT(element)   ((element)->GetType()==CElement::OBJECT)
#define IS_PERPLAYER_ENTITY(element) ((element)->IsPerPlayerEntity())
#define IS_PICKUP(element)   ((element)->GetType()==CElement::PICKUP)
#define IS_PED(element)      ((element)->GetType()==CElement::PLAYER||(element)->GetType()==CElement::PED)
#define IS_PLAYER(element)   ((element)->GetType()==CElement::PLAYER)
#define IS_RADAR_AREA(element) ((element)->GetType()==CElement::RADAR_AREA)
#define IS_VEHICLE(element)  ((element)->GetType()==CElement::VEHICLE)
#define IS_CONSOLE(element)  ((element)->GetType()==CElement::CONSOLE)
#define IS_TEAM(element)     ((element)->GetType()==CElement::TEAM)
#define IS_WATER(element)    ((element)->GetType()==CElement::WATER)
#define IS_WEAPON(element)    ((element)->GetType()==CElement::WEAPON)

class CLuaMain;

typedef CFastList<CElement*> CChildListType;
typedef CFastList<CElement*> CElementListType;

typedef std::vector<CElement*>                CElementListSnapshot;
typedef std::shared_ptr<CElementListSnapshot> CElementListSnapshotRef;

class CElement
{
    friend class CPerPlayerEntity;
    friend class CBlip;
    friend class CPlayer;
    friend class CPlayerCamera;

public:
    enum EElementType
    {
        DUMMY,
        PLAYER,
        VEHICLE,
        OBJECT,
        MARKER,
        BLIP,
        PICKUP,
        RADAR_AREA,
        SPAWNPOINT_DEPRECATED,
        REMOTECLIENT_DEPRECATED,
        CONSOLE,
        PATH_NODE_UNUSED,
        WORLD_MESH_UNUSED,
        TEAM,
        PED,
        COLSHAPE,
        SCRIPTFILE,
        WATER,
        WEAPON,
        DATABASE_CONNECTION,
        TRAIN_TRACK,
        ROOT,
        UNKNOWN,
    };

public:
    CElement(CElement* pParent);
    virtual ~CElement();

    virtual CElement* Clone(bool* bAddEntity, CResource* pResource) { return nullptr; }
    bool              IsCloneable();

    bool         IsBeingDeleted() { return m_bIsBeingDeleted; };
    void         SetIsBeingDeleted(bool bBeingDeleted) { m_bIsBeingDeleted = bBeingDeleted; };
    virtual void Unlink() = 0;

    ElementID GetID() { return m_ID; };

    virtual const CVector& GetPosition();
    virtual void           SetPosition(const CVector& vecPosition);

    virtual void GetRotation(CVector& vecRotation) { vecRotation = CVector(); }
    virtual void GetMatrix(CMatrix& matrix);
    virtual void SetMatrix(const CMatrix& matrix);

    virtual bool IsPerPlayerEntity() { return false; };

    CElement* FindChild(const char* szName, unsigned int uiIndex, bool bRecursive);
    CElement* FindChildByType(const char* szType, unsigned int uiIndex, bool bRecursive);
    void      FindAllChildrenByType(const char* szType, lua_State* pLua);
    void      GetChildren(lua_State* pLua);
    void      GetChildrenByType(const char* szType, lua_State* pLua);
    bool      IsMyChild(CElement* pElement, bool bRecursive);
    bool      IsMyParent(CElement* pElement, bool bRecursive);
    void      ClearChildren();
    void      GetDescendantsByType(std::vector<CElement*>& outResult, EElementType elementType);
    void      GetDescendantsByTypeSlow(std::vector<CElement*>& outResult, uint uiTypeHash);
    template <class T>
    void GetDescendantsByType(std::vector<T>& outResult, EElementType elementType)
    {
        GetDescendantsByType((std::vector<CElement*>&)outResult, elementType);
    }

    CMapEventManager* GetEventManager() { return m_pEventManager; };
    CElement*         GetParentEntity() { return m_pParent; };

    CElement* SetParentObject(CElement* pParent, bool bUpdatePerPlayerEntities = true);

    bool AddEvent(CLuaMain* pLuaMain, const char* szName, const CLuaFunctionRef& iLuaFunction, bool bPropagated, EEventPriorityType eventPriority,
                  float fPriorityMod);
    bool CallEvent(const char* szName, const CLuaArguments& Arguments, CPlayer* pCaller = NULL);
    bool DeleteEvent(CLuaMain* pLuaMain, const char* szName, const CLuaFunctionRef& iLuaFunction = CLuaFunctionRef());
    void DeleteEvents(CLuaMain* pLuaMain, bool bRecursive);
    void DeleteAllEvents();

    void           ReadCustomData(CEvents* pEvents, CXMLNode& Node);
    CCustomData&   GetCustomDataManager() { return m_CustomData; }
    CLuaArgument*  GetCustomData(const char* szName, bool bInheritData, ESyncType* pSyncType = nullptr, eCustomDataClientTrust* clientChangesMode = nullptr);
    CLuaArguments* GetAllCustomData(CLuaArguments* table);
    bool           GetCustomDataString(const char* szName, char* pOut, size_t sizeBuffer, bool bInheritData);
    bool           GetCustomDataInt(const char* szName, int& iOut, bool bInheritData);
    bool           GetCustomDataFloat(const char* szName, float& fOut, bool bInheritData);
    bool           GetCustomDataBool(const char* szName, bool& bOut, bool bInheritData);
    void           SetCustomData(const char* szName, const CLuaArgument& Variable, ESyncType syncType = ESyncType::BROADCAST, CPlayer* pClient = NULL,
                                 bool bTriggerEvent = true);
    void           DeleteCustomData(const char* szName);
    void           SendAllCustomData(CPlayer* pPlayer);

    CXMLNode* OutputToXML(CXMLNode* pNode);

    void CleanUpForVM(CLuaMain* pLuaMain, bool bRecursive);

    unsigned int                            CountChildren() { return static_cast<unsigned int>(m_Children.size()); };
    CChildListType ::const_iterator         IterBegin() { return m_Children.begin(); };
    CChildListType ::const_iterator         IterEnd() { return m_Children.end(); };
    CChildListType ::const_reverse_iterator IterReverseBegin() { return m_Children.rbegin(); };
    CChildListType ::const_reverse_iterator IterReverseEnd() { return m_Children.rend(); };
    CElementListSnapshotRef                 GetChildrenListSnapshot();

    static uint        GetTypeHashFromString(const SString& strTypeName);
    EElementType       GetType() { return m_iType; };
    virtual bool       IsEntity() { return false; };
    unsigned int       GetTypeHash() { return m_uiTypeHash; };
    const std::string& GetTypeName() { return m_strTypeName; };
    void               SetTypeName(const std::string& strTypeName);

    const std::string& GetName() { return m_strName; };
    void               SetName(const std::string& strName) { m_strName = strName; };

    bool LoadFromCustomData(CEvents* pEvents, CXMLNode& Node);

    void OnSubtreeAdd(CElement* pElement);
    void OnSubtreeRemove(CElement* pElement);

    virtual void UpdatePerPlayer(){};
    void         UpdatePerPlayerEntities();

    void                                  AddCollision(class CColShape* pShape) { m_Collisions.push_back(pShape); }
    void                                  RemoveCollision(class CColShape* pShape) { m_Collisions.remove(pShape); }
    bool                                  CollisionExists(class CColShape* pShape);
    void                                  RemoveAllCollisions();
    std::list<class CColShape*>::iterator CollisionsBegin() { return m_Collisions.begin(); }
    std::list<class CColShape*>::iterator CollisionsEnd() { return m_Collisions.end(); }

    unsigned short GetDimension() { return m_usDimension; }
    virtual void   SetDimension(unsigned short usDimension);

    class CClient* GetClient();

    CElement*                            GetAttachedToElement() { return m_pAttachedTo; }
    virtual void                         AttachTo(CElement* pElement);
    virtual void                         GetAttachedOffsets(CVector& vecPosition, CVector& vecRotation);
    virtual void                         SetAttachedOffsets(CVector& vecPosition, CVector& vecRotation);
    void                                 AddAttachedElement(CElement* pElement);
    void                                 RemoveAttachedElement(CElement* pElement);
    std::list<CElement*>::const_iterator AttachedElementsBegin() { return m_AttachedElements.begin(); }
    std::list<CElement*>::const_iterator AttachedElementsEnd() { return m_AttachedElements.end(); }
    const char*                          GetAttachToID() { return m_strAttachToID; }
    bool                                 IsElementAttached(CElement* pElement);
    bool                                 IsAttachedToElement(CElement* pElement, bool bRecursive = true);
    virtual bool                         IsAttachable();
    virtual bool                         IsAttachToable();
    void                                 GetAttachedPosition(CVector& vecPosition);
    void                                 GetAttachedRotation(CVector& vecRotation);

    CElementGroup* GetElementGroup() { return m_pElementGroup; }
    void           SetElementGroup(CElementGroup* elementGroup) { m_pElementGroup = elementGroup; }

    // This is used for realtime synced elements. Whenever a position/rotation change is
    // forced from the server either in form of spawn or setElementPosition/rotation a new
    // value is assigned to this from the server. This value also comes with the sync packets.
    // If this value doesn't match the value from the sync packet, the packet should be
    // ignored. Note that if this value is 0, all sync packets should be accepted. This is
    // so we don't need this byte when the element is created first.
    unsigned char GetSyncTimeContext() { return m_ucSyncTimeContext; };
    unsigned char GenerateSyncTimeContext();
    bool          CanUpdateSync(unsigned char ucRemote);

    void AddOriginSourceUser(class CPed* pPed) { m_OriginSourceUsers.push_back(pPed); }
    void RemoveOriginSourceUser(class CPed* pPed) { m_OriginSourceUsers.remove(pPed); }

    unsigned char GetInterior() { return m_ucInterior; }
    void          SetInterior(unsigned char ucInterior);

    bool IsDoubleSided() { return m_bDoubleSided; }
    void SetDoubleSided(bool bDoubleSided) { m_bDoubleSided = bDoubleSided; }

    virtual bool IsOnFire() const noexcept { return false; }
    virtual void SetOnFire(bool onFire) noexcept {}

    // Spatial database
    virtual CSphere GetWorldBoundingSphere();
    virtual void    UpdateSpatialData();

    bool IsCallPropagationEnabled() { return m_bCallPropagationEnabled; }
    void SetCallPropagationEnabled(bool bEnabled) { m_bCallPropagationEnabled = bEnabled; }

    bool CanBeDestroyedByScript() { return m_canBeDestroyedByScript; }
    void SetCanBeDestroyedByScript(bool canBeDestroyedByScript) { m_canBeDestroyedByScript = canBeDestroyedByScript; }

protected:
    CElement*    GetRootElement();
    virtual bool ReadSpecialData(const int iLine) = 0;

    CElement* FindChildIndex(const char* szName, unsigned int uiIndex, unsigned int& uiCurrentIndex, bool bRecursive);
    CElement* FindChildByTypeIndex(unsigned int uiTypeHash, unsigned int uiIndex, unsigned int& uiCurrentIndex, bool bRecursive);
    void      FindAllChildrenByTypeIndex(unsigned int uiTypeHash, lua_State* pLua, unsigned int& uiIndex);

    void CallEventNoParent(const char* szName, const CLuaArguments& Arguments, CElement* pSource, CPlayer* pCaller = NULL);
    void CallParentEvent(const char* szName, const CLuaArguments& Arguments, CElement* pSource, CPlayer* pCaller = NULL);

    CMapEventManager* m_pEventManager;
    CCustomData       m_CustomData;

    EElementType m_iType;
    ElementID    m_ID;
    CElement*    m_pParent;
    bool         m_bIsBeingDeleted;

    CVector m_vecPosition;

    unsigned int            m_uiTypeHash;
    std::string             m_strTypeName;
    std::string             m_strName;
    CChildListType          m_Children;
    CElementListSnapshotRef m_pChildrenListSnapshot;
    uint                    m_uiChildrenListSnapshotRevision;

    std::list<class CPerPlayerEntity*> m_ElementReferenced;
    std::list<class CColShape*>        m_Collisions;
    std::list<class CPlayerCamera*>    m_FollowingCameras;

    CElement*            m_pAttachedTo;
    CVector              m_vecAttachedPosition;
    CVector              m_vecAttachedRotation;
    std::list<CElement*> m_AttachedElements;
    SString              m_strAttachToID;

    CElementGroup* m_pElementGroup;

    unsigned short m_usDimension;
    unsigned char  m_ucSyncTimeContext;

    std::list<class CPed*> m_OriginSourceUsers;
    unsigned char          m_ucInterior;
    bool                   m_bDoubleSided;
    bool                   m_bUpdatingSpatialData;
    bool                   m_bCallPropagationEnabled;
    bool                   m_canBeDestroyedByScript = true;            // If true, destroyElement function will
                                                                       // have no effect on this element
    // Optimization for getElementsByType starting at root
public:
    static void StartupEntitiesFromRoot();

private:
    static bool IsFromRoot(CElement* pEntity);
    static void AddEntityFromRoot(unsigned int uiTypeHash, CElement* pEntity, bool bDebugCheck = true);
    static void RemoveEntityFromRoot(unsigned int uiTypeHash, CElement* pEntity);
    static void GetEntitiesFromRoot(unsigned int uiTypeHash, lua_State* pLua);
    static void GetEntitiesFromRoot(unsigned int uiTypeHash, std::vector<CElement*>& outResult);

#if CHECK_ENTITIES_FROM_ROOT
    static void _CheckEntitiesFromRoot(unsigned int uiTypeHash);
    void        _FindAllChildrenByTypeIndex(unsigned int uiTypeHash, std::map<CElement*, int>& mapResults);
    static void _GetEntitiesFromRoot(unsigned int uiTypeHash, std::map<CElement*, int>& mapResults);
#endif
};

typedef CElement::EElementType EElementType;
