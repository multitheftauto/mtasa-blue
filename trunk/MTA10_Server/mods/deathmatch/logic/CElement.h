/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CElement.h
*  PURPOSE:     Base entity (element) class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <>
*               Jax <>
*               Cecill Etheredge <>
*               lil_Toady <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CELEMENT_H
#define __CELEMENT_H

#include <core/CServerInterface.h>
#include <CVector.h>
#include "CMapEventManager.h"
#include "CCustomData.h"
#include "CEvents.h"
#include <assert.h>
#include <list>
#include <cstring>
#include "CElementGroup.h"

// Used to check fast version of getElementsByType
//#define CHECK_ENTITIES_FROM_ROOT  MTA_DEBUG

#define IS_BLIP(element)     ((element)->GetType()==CElement::BLIP)
#define IS_COLSHAPE(element) ((element)->GetType()==CElement::COLSHAPE)
#define IS_DUMMY(element)    ((element)->GetType()==CElement::DUMMY)
#define IS_FILE(element)     ((element)->GetType()==CElement::SCRIPTFILE)
#define IS_MARKER(element)   ((element)->GetType()==CElement::MARKER)
#define IS_OBJECT(element)   ((element)->GetType()==CElement::OBJECT)
#define IS_PATHNODE(element) ((element)->GetType()==CElement::PATH_NODE)
#define IS_PERPLAYER_ENTITY(element) ((element)->IsPerPlayerEntity())
#define IS_PICKUP(element)   ((element)->GetType()==CElement::PICKUP)
#define IS_PED(element)      ((element)->GetType()==CElement::PLAYER||(element)->GetType()==CElement::PED)
#define IS_PLAYER(element)   ((element)->GetType()==CElement::PLAYER)
#define IS_RADAR_AREA(element) ((element)->GetType()==CElement::RADAR_AREA)
#define IS_VEHICLE(element)  ((element)->GetType()==CElement::VEHICLE)
#define IS_CONSOLE(element)  ((element)->GetType()==CElement::CONSOLE)
#define IS_TEAM(element)     ((element)->GetType()==CElement::TEAM)
#define IS_WATER(element)    ((element)->GetType()==CElement::WATER)

typedef CIntrusiveList < class CElement > CChildListType;

class CElement
{
    friend class CPerPlayerEntity;
    friend class CBlip;
    friend class CPlayer;
    friend class CPlayerCamera;

public:
    enum
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
        PATH_NODE,
        WORLD_MESH,
        TEAM,
        PED,
        COLSHAPE,
        SCRIPTFILE,
        WATER,
        UNKNOWN,
    };

public:
                                                CElement                    ( CElement* pParent, CXMLNode* pNode = NULL );
    virtual                                     ~CElement                   ( void );

    inline bool                                 IsBeingDeleted              ( void )                        { return m_bIsBeingDeleted; };
    inline void                                 SetIsBeingDeleted           ( bool bBeingDeleted )          { m_bIsBeingDeleted = bBeingDeleted; };
    virtual void                                Unlink                      ( void ) = 0;

    inline ElementID                            GetID                       ( void )                        { return m_ID; };
protected:
    void                                        SetID                       ( const ElementID& ID )         { m_ID = ID; }
public:

    virtual const CVector&                      GetPosition                 ( void );
    virtual const CVector&                      GetLastPosition             ( void )                        { return m_vecLastPosition; };
    virtual void                                SetPosition                 ( const CVector& vecPosition );

    virtual void                                GetRotation                 ( CVector & vecRotation )       { vecRotation = CVector (); }

    virtual bool                                IsPerPlayerEntity           ( void )                        { return false; };

    CElement*                                   FindChild                   ( const char* szName, unsigned int uiIndex, bool bRecursive );
    CElement*                                   FindChildByType             ( const char* szType, unsigned int uiIndex, bool bRecursive );
    void                                        FindAllChildrenByType       ( const char* szType, lua_State* pLua );
    void                                        GetChildren                 ( lua_State* pLua );
    bool                                        IsMyChild                   ( CElement* pElement, bool bRecursive );
    void                                        ClearChildren               ( void );

    inline CMapEventManager*                    GetEventManager             ( void )                        { return m_pEventManager; };
    inline CElement*                            GetParentEntity             ( void )                        { return m_pParent; };
    inline CXMLNode*                            GetXMLNode                  ( void )                        { return m_pXMLNode; };

    CElement*                                   SetParentObject             ( CElement* pParent );
    void                                        SetXMLNode                  ( CXMLNode* pNode );

    bool                                        AddEvent                    ( CLuaMain* pLuaMain, const char* szName, const CLuaFunctionRef& iLuaFunction, bool bPropagated );
    bool                                        CallEvent                   ( const char* szName, const CLuaArguments& Arguments, CPlayer* pCaller = NULL );
    bool                                        DeleteEvent                 ( CLuaMain* pLuaMain, const char* szName, const CLuaFunctionRef& iLuaFunction = CLuaFunctionRef () );
    void                                        DeleteEvents                ( CLuaMain* pLuaMain, bool bRecursive );
    void                                        DeleteAllEvents             ( void );

    void                                        ReadCustomData              ( CLuaMain* pLuaMain, CEvents* pEvents );
    inline CCustomData*                         GetCustomDataPointer        ( void )                    { return m_pCustomData; }
    CLuaArgument*                               GetCustomData               ( const char* szName, bool bInheritData, bool* pbIsSynced = NULL );
    CLuaArguments*                              GetAllCustomData            ( CLuaArguments * table );
    bool                                        GetCustomDataString         ( const char* szName, char* pOut, size_t sizeBuffer, bool bInheritData );
    bool                                        GetCustomDataInt            ( const char* szName, int& iOut, bool bInheritData );
    bool                                        GetCustomDataFloat          ( const char* szName, float& fOut, bool bInheritData );
    bool                                        GetCustomDataBool           ( const char* szName, bool& bOut, bool bInheritData );
    void                                        SetCustomData               ( const char* szName, const CLuaArgument& Variable, CLuaMain* pLuaMain, bool bSynchronized = true, CPlayer* pClient = NULL );
    bool                                        DeleteCustomData            ( const char* szName, bool bRecursive );
    void                                        DeleteAllCustomData         ( CLuaMain* pLuaMain, bool bRecursive );

    CXMLNode*                                   OutputToXML                 ( CXMLNode* pNode );

    void                                        CleanUpForVM                ( CLuaMain* pLuaMain, bool bRecursive );

    inline unsigned int                         CountChildren               ( void )                        { return static_cast < unsigned int > ( m_Children.size () ); };
    CChildListType ::const_iterator             IterBegin                   ( void )                        { return m_Children.begin (); };
    CChildListType ::const_iterator             IterEnd                     ( void )                        { return m_Children.end (); };
    CChildListType ::const_reverse_iterator     IterReverseBegin            ( void )                        { return m_Children.rbegin (); };
    CChildListType ::const_reverse_iterator     IterReverseEnd              ( void )                        { return m_Children.rend (); };

    inline int                                  GetType                     ( void )                        { return m_iType; };
    virtual bool                                IsEntity                    ( void )                        { return false; };
    inline unsigned int                         GetTypeHash                 ( void )                        { return m_uiTypeHash; };
    inline const std::string&                   GetTypeName                 ( void )                        { return m_strTypeName; };
    void                                        SetTypeName                 ( const std::string& strTypeName );

    inline const std::string&                   GetName                     ( void )                        { return m_strName; };
    inline void                                 SetName                     ( const std::string& strName )  { m_strName = strName; };

    bool                                        LoadFromCustomData          ( CLuaMain* pLuaMain, CEvents* pEvents );

    void                                        OnSubtreeAdd                ( CElement* pElement );
    void                                        OnSubtreeRemove             ( CElement* pElement );

    virtual void                                UpdatePerPlayer             ( void ) {};
    void                                        UpdatePerPlayerEntities     ( void );

    static unsigned int                         GetTypeID                   ( const char * szTypeName );

    void                                        AddCollision                ( class CColShape* pShape )     { m_Collisions.push_back ( pShape ); }
    void                                        RemoveCollision             ( class CColShape* pShape )     { if ( !m_Collisions.empty() ) m_Collisions.remove ( pShape ); }
    bool                                        CollisionExists             ( class CColShape* pShape );
    void                                        RemoveAllCollisions         ( bool bNotify = false );
    std::list < class CColShape* > ::iterator   CollisionsBegin             ( void )                        { return m_Collisions.begin (); }
    std::list < class CColShape* > ::iterator   CollisionsEnd               ( void )                        { return m_Collisions.end (); }

    inline unsigned short                       GetDimension                ( void )                        { return m_usDimension; }
    inline void                                 SetDimension                ( unsigned short usDimension )  { m_usDimension = usDimension; }

    class CClient*                              GetClient                   ( void );

    inline CElement*                            GetAttachedToElement        ( void )                        { return m_pAttachedTo; }
    virtual void                                AttachTo                    ( CElement* pElement );
    virtual void                                GetAttachedOffsets          ( CVector & vecPosition, CVector & vecRotation );
    virtual void                                SetAttachedOffsets          ( CVector & vecPosition, CVector & vecRotation );
    inline void                                 AddAttachedElement          ( CElement* pElement )          { m_AttachedElements.push_back ( pElement ); }
    inline void                                 RemoveAttachedElement       ( CElement* pElement )          { if ( !m_AttachedElements.empty() ) m_AttachedElements.remove ( pElement ); }
    std::list < CElement* > ::iterator          AttachedElementsBegin       ( void )                        { return m_AttachedElements.begin (); }
    std::list < CElement* > ::iterator          AttachedElementsEnd         ( void )                        { return m_AttachedElements.end (); }
    inline char*                                GetAttachToID               ( void )                        { return m_szAttachToID; }
    bool                                        IsElementAttached           ( CElement* pElement );
    virtual bool                                IsAttachable                ( void );
    virtual bool                                IsAttachToable              ( void );
    void                                        GetAttachedPosition         ( CVector & vecPosition );
    void                                        GetAttachedRotation         ( CVector & vecRotation );

    inline CElementGroup*                       GetElementGroup             ( void )                        { return m_pElementGroup; }
    inline void                                 SetElementGroup             ( CElementGroup * elementGroup ){ m_pElementGroup = elementGroup; }

    // This is used for realtime synced elements. Whenever a position/rotation change is
    // forced from the server either in form of spawn or setElementPosition/rotation a new
    // value is assigned to this from the server. This value also comes with the sync packets.
    // If this value doesn't match the value from the sync packet, the packet should be
    // ignored. Note that if this value is 0, all sync packets should be accepted. This is
    // so we don't need this byte when the element is created first.
    inline unsigned char                        GetSyncTimeContext          ( void )                    { return m_ucSyncTimeContext; };
    unsigned char                               GenerateSyncTimeContext     ( void );
    bool                                        CanUpdateSync               ( unsigned char ucRemote );

    inline void                                 AddOriginSourceUser         ( class CPed * pPed )           { m_OriginSourceUsers.push_back ( pPed ); }
    inline void                                 RemoveOriginSourceUser      ( class CPed * pPed )           { m_OriginSourceUsers.remove ( pPed ); }

    inline unsigned char                        GetInterior                 ( void )                        { return m_ucInterior; }
    inline void                                 SetInterior                 ( unsigned char ucInterior )    { m_ucInterior = ucInterior; }

    bool                                        IsDoubleSided               ( void )                        { return m_bDoubleSided; }
    void                                        SetDoubleSided              ( bool bDoubleSided )           { m_bDoubleSided = bDoubleSided; }

    inline bool                                 IsMapCreated                ( void )                        { return m_bMapCreated; }

    // Spatial database
    virtual CSphere                             GetWorldBoundingSphere      ( void );
    virtual void                                UpdateSpatialData           ( void );

protected:
    CElement*                                   GetRootElement              ( void );
    virtual bool                                ReadSpecialData             ( void ) = 0;

    CElement*                                   FindChildIndex              ( const char* szName, unsigned int uiIndex, unsigned int& uiCurrentIndex, bool bRecursive );
    CElement*                                   FindChildByTypeIndex        ( unsigned int uiTypeHash, unsigned int uiIndex, unsigned int& uiCurrentIndex, bool bRecursive );
    void                                        FindAllChildrenByTypeIndex  ( unsigned int uiTypeHash, lua_State* pLua, unsigned int& uiIndex );

    void                                        CallEventNoParent           ( const char* szName, const CLuaArguments& Arguments, CElement* pSource, CPlayer* pCaller = NULL );
    void                                        CallParentEvent             ( const char* szName, const CLuaArguments& Arguments, CElement* pSource, CPlayer* pCaller = NULL );

public:
    CIntrusiveListNode < CElement >             m_FromRootNode;     // Our node entry in the 'EntitiesFromRoot' list
protected:
    CIntrusiveListNode < CElement >             m_ChildrenNode;     // Our node entry in the parent object m_Children list

    CMapEventManager*                           m_pEventManager;
    CCustomData*                                m_pCustomData;

    int                                         m_iType;
    ElementID                                   m_ID;
    CElement*                                   m_pParent;
    CXMLNode*                                   m_pXMLNode;
    unsigned int                                m_uiLine;
    bool                                        m_bIsBeingDeleted;

    CVector                                     m_vecPosition;
    CVector                                     m_vecLastPosition;

    unsigned int                                m_uiTypeHash;
    std::string                                 m_strTypeName;
    std::string                                 m_strName;
    CChildListType                              m_Children;

    std::list < class CPerPlayerEntity* >       m_ElementReferenced;
    std::list < class CColShape* >              m_Collisions;
    std::list < class CPlayerCamera* >          m_FollowingCameras;

    CElement*                                   m_pAttachedTo;
    CVector                                     m_vecAttachedPosition;
    CVector                                     m_vecAttachedRotation;
    std::list < CElement* >                     m_AttachedElements;
    char                                        m_szAttachToID [ MAX_ELEMENT_NAME_LENGTH + 1 ];

    CElementGroup*                              m_pElementGroup;

    unsigned short                              m_usDimension;
    unsigned char                               m_ucSyncTimeContext;

    std::list < class CPed * >                  m_OriginSourceUsers;
    unsigned char                               m_ucInterior;
    bool                                        m_bDoubleSided;
    bool                                        m_bMapCreated;
    bool                                        m_bUpdatingSpatialData;

    // Optimization for getElementsByType starting at root
public:
    static void                     StartupEntitiesFromRoot ( );
private:
    static bool                     IsFromRoot              ( CElement* pEntity );
    static void                     AddEntityFromRoot       ( unsigned int uiTypeHash, CElement* pEntity, bool bDebugCheck = true );
    static void                     RemoveEntityFromRoot    ( unsigned int uiTypeHash, CElement* pEntity );
    static void                     GetEntitiesFromRoot     ( unsigned int uiTypeHash, lua_State* pLua );

#if CHECK_ENTITIES_FROM_ROOT
    static void                     _CheckEntitiesFromRoot      ( unsigned int uiTypeHash );
    void                            _FindAllChildrenByTypeIndex ( unsigned int uiTypeHash, std::map < CElement*, int >& mapResults );
    static void                     _GetEntitiesFromRoot        ( unsigned int uiTypeHash, std::map < CElement*, int >& mapResults );
#endif
};

#endif

