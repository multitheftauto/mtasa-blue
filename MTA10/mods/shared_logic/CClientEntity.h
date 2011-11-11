/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientEntity.h
*  PURPOSE:     Base entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Chris McArthur <>
*               Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

class CClientEntity;

#ifndef __CCLIENTENTITY_H
#define __CCLIENTENTITY_H

#include "CElementArray.h"
#include "CClientCommon.h"
#include <assert.h>
#include <list>
#include <google/dense_hash_map>
class CLuaFunctionRef;

// Used to check fast version of getElementsByType
//#define CHECK_ENTITIES_FROM_ROOT  MTA_DEBUG

class CClientManager;

#define IS_PED(entity) ((entity)->GetType()==CCLIENTPLAYER||(entity)->GetType()==CCLIENTPED)
#define IS_PLAYER(entity) ((entity)->GetType()==CCLIENTPLAYER)
#define IS_REMOTE_PLAYER(player) (IS_PLAYER(player)&&!(player)->IsLocalPlayer())
#define IS_RADARMARKER(entity) ((entity)->GetType()==CCLIENTRADARMARKER)
#define IS_VEHICLE(entity) ((entity)->GetType()==CCLIENTVEHICLE)
#define IS_OBJECT(entity) ((entity)->GetType()==CCLIENTOBJECT)
#define IS_MARKER(entity) ((entity)->GetType()==CCLIENTMARKER)
#define IS_PICKUP(entity) ((entity)->GetType()==CCLIENTPICKUP)
#define IS_RADAR_AREA(entity) ((entity)->GetType()==CCLIENTRADARAREA)
#define IS_COLSHAPE(entity) ((entity)->GetType()==CCLIENTCOLSHAPE)
#define IS_PROJECTILE(entity) ((entity)->GetType()==CCLIENTPROJECTILE)
#define IS_GUI(entity) ((entity)->GetType()==CCLIENTGUI)
#define CHECK_CGUI(entity,type) (((CClientGUIElement*)entity)->GetCGUIElement()->GetType()==type)

enum eClientEntityType
{
    CCLIENTCAMERA,
    CCLIENTPLAYER,
    CCLIENTPLAYERMODEL____,
    CCLIENTVEHICLE,
    CCLIENTRADARMARKER,
    CCLIENTOBJECT,
    CCLIENTCIVILIAN,
    CCLIENTPICKUP,
    CCLIENTRADARAREA,
    CCLIENTMARKER,
    CCLIENTPATHNODE,
    CCLIENTWORLDMESH,
    CCLIENTTEAM,
    CCLIENTPED,
    CCLIENTPROJECTILE,
    CCLIENTGUI,
    CCLIENTSPAWNPOINT_DEPRECATED,
    CCLIENTCOLSHAPE,
    CCLIENTDUMMY, // anything user-defined
    SCRIPTFILE,
    CCLIENTDFF,
    CCLIENTCOL,
    CCLIENTTXD,
    CCLIENTSOUND,
    CCLIENTWATER,
    CCLIENTDXFONT,
    CCLIENTGUIFONT,
    CCLIENTTEXTURE,
    CCLIENTSHADER,
    CCLIENTUNKNOWN,
};

class CClientColShape;
class CClientPed;
class CCustomData;
class CElementGroup;
class CLuaArgument;
class CLuaArguments;
class CLuaMain;
class CMapEventManager;
typedef CIntrusiveList < class CClientEntity > CChildListType;

enum eCClientEntityClassTypes
{
    CLASS_CClientEntity,
    CLASS_CClientCamera,
    CLASS_CClientCivilian,
    CLASS_CClientColModel,
    CLASS_CClientDFF,
    CLASS_CClientGUIElement,
    CLASS_CClientStreamElement,
    CLASS_CClientColShape,
    CLASS_CClientMarker,
    CLASS_CClientPathNode,
    CLASS_CClientPickup,
    CLASS_CClientRadarArea,
    CLASS_CClientRadarMarker,
    CLASS_CClientPed,
    CLASS_CClientPlayer,
    CLASS_CClientTeam,
    CLASS_CClientSound,
    CLASS_CClientVehicle,
    CLASS_CClientDummy,
    CLASS_CClientWater,
    CLASS_CClientColCircle,
    CLASS_CClientColCuboid,
    CLASS_CClientColSphere,
    CLASS_CClientColRectangle,
    CLASS_CClientColPolygon,
    CLASS_CClientColTube,
    CLASS_CClientProjectile,
    CLASS_CClientTXD,
    CLASS_CScriptFile,
    CLASS_CClientObject,
    CLASS_CDeathmatchObject,
    CLASS_CDeathmatchVehicle,
    CLASS_CClientRenderElement,
    CLASS_CClientDxFont,
    CLASS_CClientGuiFont,
    CLASS_CClientMaterial,
    CLASS_CClientTexture,
    CLASS_CClientShader,
    CLASS_CClientRenderTarget,
    CLASS_CClientScreenSource,
};


class CClientEntity
{
    DECLARE_BASE_CLASS( CClientEntity )
public:
                                                CClientEntity           ( ElementID ID );
    virtual                                     ~CClientEntity          ( void );

    virtual bool                                CanBeDeleted            ( void )                    { return true; };

    static inline int                           GetInstanceCount        ( void )                    { return iCount; };

    virtual eClientEntityType                   GetType                 ( void ) const = 0;
    inline bool                                 IsLocalEntity           ( void )                    { return m_ID >= MAX_SERVER_ELEMENTS; };

    // System entity? A system entity means it can't be removed by the server
    // or the client scripts.
    inline bool                                 IsSystemEntity          ( void )                    { return m_bSystemEntity; };
    inline void                                 MakeSystemEntity        ( void )                    { m_bSystemEntity = true; };

    virtual void                                Unlink                  ( void ) = 0;

    // This is used for realtime synced elements. Whenever a position/rotation change is
    // forced from the server either in form of spawn or setElementPosition/rotation a new
    // value is assigned to this from the server. This value also comes with the sync packets.
    // If this value doesn't match the value from the sync packet, the packet should be
    // ignored. Note that if this value is 0, all sync packets should be accepted. This is
    // so we don't need this byte when the element is created first.
    inline unsigned char                        GetSyncTimeContext      ( void )                    { return m_ucSyncTimeContext; };
    inline void                                 SetSyncTimeContext      ( unsigned char ucContext ) { m_ucSyncTimeContext = ucContext; };
    bool                                        CanUpdateSync           ( unsigned char ucRemote );

    inline char*                                GetName                 ( void )                    { return m_szName; };
    inline void                                 SetName                 ( const char* szName )      { assert ( szName ); strncpy ( m_szName, szName, MAX_ELEMENT_NAME_LENGTH ); };

    inline const char*                          GetTypeName             ( void )                    { return m_szTypeName; };
    inline unsigned int                         GetTypeHash             ( void )                    { return m_uiTypeHash; };
    void                                        SetTypeName             ( const char* szName );

    inline CClientEntity*                       GetParent               ( void )                    { return m_pParent; };
    CClientEntity*                              SetParent               ( CClientEntity* pParent );
    CClientEntity*                              AddChild                ( CClientEntity* pChild );
    bool                                        IsMyChild               ( CClientEntity* pEntity, bool bRecursive );
    bool                                        IsMyParent              ( CClientEntity* pEntity, bool bRecursive );
    inline bool                                 IsBeingDeleted          ( void )                    { return m_bBeingDeleted; }
    inline void                                 SetBeingDeleted         ( bool bBeingDeleted )      { m_bBeingDeleted = bBeingDeleted; }
    void                                        ClearChildren           ( void );

    CChildListType ::const_iterator             IterBegin               ( void )                    { return m_Children.begin (); }
    CChildListType ::const_iterator             IterEnd                 ( void )                    { return m_Children.end (); }

    inline ElementID                            GetID                   ( void )                    { return m_ID; };
    void                                        SetID                   ( ElementID ID );

    inline CCustomData*                         GetCustomDataPointer    ( void )                    { return m_pCustomData; }
    CLuaArgument*                               GetCustomData           ( const char* szName, bool bInheritData );
    bool                                        GetCustomDataString     ( const char * szKey, SString& strOut, bool bInheritData );
    bool                                        GetCustomDataFloat      ( const char * szKey, float& fOut, bool bInheritData );
    bool                                        GetCustomDataInt        ( const char * szKey, int& iOut, bool bInheritData );
    bool                                        GetCustomDataBool       ( const char * szKey, bool& bOut, bool bInheritData );
    void                                        SetCustomData           ( const char* szName, const CLuaArgument& Variable, CLuaMain* pLuaMain );
    bool                                        DeleteCustomData        ( const char* szName, bool bRecursive );
    void                                        DeleteAllCustomData     ( CLuaMain* pLuaMain, bool bRecursive );

    virtual bool                                GetMatrix               ( CMatrix& matrix ) const;
    virtual bool                                SetMatrix               ( const CMatrix & matrix );

    virtual void                                GetPosition             ( CVector& vecPosition ) const = 0;
    void                                        GetPositionRelative     ( CClientEntity * pOrigin, CVector& vecPosition ) const;
    virtual void                                SetPosition             ( const CVector& vecPosition ) = 0;
    void                                        SetPositionRelative     ( CClientEntity * pOrigin, const CVector& vecPosition );
    virtual void                                Teleport                ( const CVector& vecPosition ) { SetPosition(vecPosition); }

    virtual inline unsigned short               GetDimension            ( void )                        { return m_usDimension; }
    virtual void                                SetDimension            ( unsigned short usDimension ) { m_usDimension = usDimension; }

    virtual void                                ModelRequestCallback    ( CModelInfo* pModelInfo ) {};

    virtual bool                                IsOutOfBounds           ( void );
    inline CModelInfo*                          GetModelInfo            ( void )                        { return m_pModelInfo; };
    
    inline CClientEntity*                       GetAttachedTo           ( void )                        { return m_pAttachedToEntity; }
    virtual void                                AttachTo                ( CClientEntity * pEntity );
    virtual void                                GetAttachedOffsets      ( CVector & vecPosition, CVector & vecRotation );
    virtual void                                SetAttachedOffsets      ( CVector & vecPosition, CVector & vecRotation );
    inline void                                 AddAttachedEntity       ( CClientEntity* pEntity )      { m_AttachedEntities.push_back ( pEntity ); }
    inline void                                 RemoveAttachedEntity    ( CClientEntity* pEntity )      { if ( !m_AttachedEntities.empty() ) m_AttachedEntities.remove ( pEntity ); }
    bool                                        IsEntityAttached        ( CClientEntity* pEntity );
    std::list < CClientEntity* > ::const_iterator AttachedEntitiesBegin ( void )                        { return m_AttachedEntities.begin (); }
    std::list < CClientEntity* > ::const_iterator AttachedEntitiesEnd   ( void )                        { return m_AttachedEntities.end (); }
    void                                        ReattachEntities        ( void );
    virtual bool                                IsAttachable            ( void );
    virtual bool                                IsAttachToable          ( void );
    virtual void                                DoAttaching             ( void );

    bool                                        AddEvent                ( CLuaMain* pLuaMain, const char* szName, const CLuaFunctionRef& iLuaFunction, bool bPropagated );
    bool                                        CallEvent               ( const char* szName, const CLuaArguments& Arguments, bool bCallOnChildren );
    void                                        CallEventNoParent       ( const char* szName, const CLuaArguments& Arguments, CClientEntity* pSource );
    void                                        CallParentEvent         ( const char* szName, const CLuaArguments& Arguments, CClientEntity* pSource );
    bool                                        DeleteEvent             ( CLuaMain* pLuaMain, const char* szName, const CLuaFunctionRef& iLuaFunction );
    void                                        DeleteEvents            ( CLuaMain* pLuaMain, bool bRecursive );
    void                                        DeleteAllEvents         ( void );

    void                                        CleanUpForVM            ( CLuaMain* pLuaMain, bool bRecursive );


    CClientEntity*                              FindChild               ( const char* szName, unsigned int uiIndex, bool bRecursive );
    CClientEntity*                              FindChildIndex          ( const char* szType, unsigned int uiIndex, unsigned int& uiCurrentIndex, bool bRecursive );
    CClientEntity*                              FindChildByType         ( const char* szType, unsigned int uiIndex, bool bRecursive );
    CClientEntity*                              FindChildByTypeIndex    ( unsigned int uiTypeHash, unsigned int uiIndex, unsigned int& uiCurrentIndex, bool bRecursive );
    void                                        FindAllChildrenByType       ( const char* szType, struct lua_State* luaVM, bool bStreamedIn = false );
    void                                        FindAllChildrenByTypeIndex  ( unsigned int uiTypeHash, lua_State* luaVM, unsigned int& uiIndex, bool bStreamedIn = false );

    inline unsigned int                         CountChildren           ( void )                        { return static_cast < unsigned int > ( m_Children.size () ); };

    void                                        GetChildren             ( lua_State* luaVM );

    void                                        AddCollision                ( CClientColShape* pShape )     { m_Collisions.push_back ( pShape ); }
    void                                        RemoveCollision             ( CClientColShape* pShape )     { if ( !m_Collisions.empty() ) m_Collisions.remove ( pShape ); }
    bool                                        CollisionExists             ( CClientColShape* pShape );
    void                                        RemoveAllCollisions         ( bool bNotify = false );
    std::list < CClientColShape* > ::iterator   CollisionsBegin             ( void )                        { return m_Collisions.begin (); }
    std::list < CClientColShape* > ::iterator   CollisionsEnd               ( void )                        { return m_Collisions.end (); }

    inline CElementGroup*                       GetElementGroup             ( void )                        { return m_pElementGroup; }
    inline void                                 SetElementGroup             ( CElementGroup * elementGroup ){ m_pElementGroup = elementGroup; }

    static unsigned int                         GetTypeID                   ( const char* szTypeName );

    CMapEventManager*                           GetEventManager             ( void )                        { return m_pEventManager; };

    void                                        DeleteClientChildren        ( void );

    // Returns true if this class is inherited by CClientStreamElement
    virtual bool                                IsStreamingCompatibleClass  ( void )                        { return false; };

    inline void                                 AddOriginSourceUser         ( CClientPed * pModel )   { m_OriginSourceUsers.push_back ( pModel ); }
    inline void                                 RemoveOriginSourceUser      ( CClientPed * pModel )   { m_OriginSourceUsers.remove ( pModel ); }

    inline void                                 AddContact                  ( CClientPed * pModel )   { m_Contacts.push_back ( pModel ); }
    inline void                                 RemoveContact               ( CClientPed * pModel )   { m_Contacts.remove ( pModel ); }

    virtual CEntity*                            GetGameEntity               ( void )                  { return NULL; }
    virtual const CEntity*                      GetGameEntity               ( void ) const            { return NULL; }

    bool                                        IsCollidableWith            ( CClientEntity * pEntity );
    void                                        SetCollidableWith           ( CClientEntity * pEntity, bool bCanCollide );

    bool                                        IsDoubleSided               ( void );
    void                                        SetDoubleSided              ( bool bEnable );

    // Game layer functions for CEntity/CPhysical
    virtual void                                InternalAttachTo            ( CClientEntity * pEntity );
    bool                                        IsStatic                    ( void );
    void                                        SetStatic                   ( bool bStatic );
    unsigned char                               GetInterior                 ( void );
    virtual void                                SetInterior                 ( unsigned char ucInterior );
    bool                                        IsOnScreen                  ( void );
    virtual RpClump *                           GetClump                    ( void );

    // Spatial database
    virtual CSphere                             GetWorldBoundingSphere      ( void );
    virtual void                                UpdateSpatialData           ( void );

    virtual void                                DebugRender                 ( const CVector& vecPosition, float fDrawRadius ) {}

    float                                       GetDistanceBetweenBoundingSpheres   ( CClientEntity* pOther );

public:
    CIntrusiveListNode < CClientEntity >        m_FromRootNode;     // Our node entry in the 'EntitiesFromRoot' list
protected:
    CIntrusiveListNode < CClientEntity >        m_ChildrenNode;     // Our node entry in the parent object m_Children list

    CClientManager*                             m_pManager;
    CClientEntity*                              m_pParent;
    CChildListType                              m_Children;

    CCustomData*                                m_pCustomData;

    ElementID                                   m_ID;
    CVector                                     m_vecRelativePosition;

    unsigned short                              m_usDimension;

    unsigned int                                m_uiLine;

private:
    unsigned int                                m_uiTypeHash;
    char                                        m_szTypeName [MAX_TYPENAME_LENGTH + 1];
    char                                        m_szName [MAX_ELEMENT_NAME_LENGTH + 1];

protected:
    unsigned char                               m_ucSyncTimeContext;

    CClientEntity*                              m_pAttachedToEntity;
    CVector                                     m_vecAttachedPosition;
    CVector                                     m_vecAttachedRotation;
    std::list < CClientEntity* >                m_AttachedEntities;

    bool                                        m_bBeingDeleted;
    bool                                        m_bSystemEntity;
    CMapEventManager*                           m_pEventManager;
    CModelInfo*                                 m_pModelInfo;
    std::list < class CClientColShape* >        m_Collisions;
    CElementGroup*                              m_pElementGroup;
    std::list < CClientPed * >                  m_OriginSourceUsers;
    std::list < CClientPed * >                  m_Contacts;
    unsigned char                               m_ucInterior;
    std::map < CClientEntity *, bool >          m_DisabledCollisions;
    bool                                        m_bDoubleSided;
    bool                                        m_bDoubleSidedInit;

private:
    static int                                  iCount;

    // Optimization for getElementsByType starting at root
public:
    static void                     StartupEntitiesFromRoot ( );
private:
    static bool                     IsFromRoot              ( CClientEntity* pEntity );
    static void                     AddEntityFromRoot       ( unsigned int uiTypeHash, CClientEntity* pEntity, bool bDebugCheck = true );
    static void                     RemoveEntityFromRoot    ( unsigned int uiTypeHash, CClientEntity* pEntity );
    static void                     GetEntitiesFromRoot     ( unsigned int uiTypeHash, lua_State* luaVM, bool bStreamedIn );

#if CHECK_ENTITIES_FROM_ROOT
    static void                     _CheckEntitiesFromRoot      ( unsigned int uiTypeHash );
    void                            _FindAllChildrenByTypeIndex ( unsigned int uiTypeHash, std::map < CClientEntity*, int >& mapResults );
    static void                     _GetEntitiesFromRoot        ( unsigned int uiTypeHash, std::map < CClientEntity*, int >& mapResults );
#endif

};

#endif
