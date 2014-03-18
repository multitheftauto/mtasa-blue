/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CEntitySA.h
*  PURPOSE:     Header file for base entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Kent Simon <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_ENTITY
#define __CGAMESA_ENTITY

#include "Common.h"
#include "COffsets.h"
#include <game/CEntity.h>
#include <CMatrix.h>
#include <CMatrix_Pad.h>
#include <CVector2D.h>
#include <CVector.h>

#include "CPlaceableSA.h"

// Entity flags
#define ENTITY_COLLISION        0x00000001
#define ENTITY_COLL_PROCESSED   0x00000002
#define ENTITY_STATIC           0x00000004
#define ENTITY_CONTACTED        0x00000008
#define ENTITY_STUCK            0x00000010
#define ENTITY_NOCOLLHIT        0x00000020
#define ENTITY_PROCESSLATER     0x00000040
#define ENTITY_VISIBLE          0x00000080
    
#define ENTITY_BIG              0x00000100
#define ENTITY_USEDAMAGE        0x00000200
#define ENTITY_DISABLESTREAMING 0x00000400
#define ENTITY_INVALID          0x00000800
#define ENTITY_COLL_WORLD       0x00001000
#define ENTITY_RENDERING        0x00002000
#define ENTITY_RENDER_LAST      0x00004000
#define ENTITY_FADE             0x00008000
    
#define ENTITY_NOSHADOWCAST     0x00010000
#define ENTITY_OFFSCREEN        0x00020000
#define ENTITY_WAITFORCOLL      0x00040000
#define ENTITY_NOSTREAM         0x00080000
#define ENTITY_UNDERWATER       0x00100000
#define ENTITY_PRERENDERED      0x00200000
#define ENTITY_DYNAMIC          0x00400000  // for buildings
#define ENTITY_CACHED_SKELETON  0x00800000
    
#define ENTITY_ROADSIGN         0x01000000
#define ENTITY_LOWLOD           0x02000000
#define ENTITY_PROC_OBJECT      0x04000000
#define ENTITY_BACKFACECULL     0x08000000
#define ENTITY_LIGHTING         0x10000000
#define ENTITY_UNIMPORTANT      0x20000000
#define ENTITY_TUNNEL           0x40000000
#define ENTITY_TUNNELTRANSITION 0x80000000

#define FUNC_GetDistanceFromCentreOfMassToBaseOfModel       0x536BE0

#define FUNC_SetRwObjectAlpha                               0x5332C0
#define FUNC_SetOrientation                                 0x439A80

#define FUNC_CMatrix__ConvertToEulerAngles                  0x59A840
#define FUNC_CMatrix__ConvertFromEulerAngles                0x59AA40

#define FUNC_IsOnScreen                                     0x534540
#define FUNC_IsVisible                                      0x536BC0

// not in CEntity really
#define FUNC_RpAnimBlendClumpGetAssociation                 0x4D6870

class CEntitySAInterfaceVTBL
{   
public:
    DWORD SCALAR_DELETING_DESTRUCTOR;       // +0h
    DWORD Add_CRect;                        // +4h
    DWORD Add;                              // +8h
    DWORD Remove;                           // +Ch
    DWORD SetIsStatic;                      // +10h
    DWORD SetModelIndex;                    // +14h
    DWORD SetModelIndexNoCreate;            // +18h
    DWORD CreateRwObject;                   // +1Ch
    DWORD DeleteRwObject;                   // +20h
    DWORD GetBoundRect;                     // +24h
    DWORD ProcessControl;                   // +28h
    DWORD ProcessCollision;                 // +2Ch
    DWORD ProcessShift;                     // +30h
    DWORD TestCollision;                    // +34h
    DWORD Teleport;                         // +38h
    DWORD SpecialEntityPreCollisionStuff;   // +3Ch
    DWORD SpecialEntityCalcCollisionSteps;  // +40h
    DWORD PreRender;                        // +44h
    DWORD Render;                           // +48h
    DWORD SetupLighting;                    // +4Ch
    DWORD RemoveLighting;                   // +50h
    DWORD FlagToDestroyWhenNextProcessed;   // +54h
};


/** 
 * \todo Move CReferences (and others below?) into it's own file
 */
class CReference
{
public:
    CReference * pNext;
    class CEntitySAInterface ** ppEntity;
};

class CReferences
{
public:
    CReference m_refs[3000];
    CReference *m_pFreeList;
};

class CSimpleTransformSAInterface   // 16 bytes
{
public:
    CVector                         m_translate;
    float                           m_heading;
};

struct CRect {
    float fX1, fY1, fX2, fY2;
};

class CEntitySAInterface : public CPlaceableSAInterface
{
public:
    virtual void __thiscall         AddRect( CRect rect ) = 0;                              // 4
    virtual bool __thiscall         AddToWorld() = 0;                                       // 8
    virtual void __thiscall         RemoveFromWorld() = 0;                                  // 12
    virtual void __thiscall         SetStatic( bool enabled ) = 0;                          // 16
    virtual void __thiscall         SetModelIndex( unsigned short id );                     // 20
    virtual void __thiscall         SetModelIndexNoCreate( unsigned short id ) = 0;         // 24
    virtual RwObject* __thiscall    CreateRwObject() = 0;                                   // 28
    virtual void __thiscall         DeleteRwObject() = 0;                                   // 32
    virtual void __thiscall         GetBoundingBox( CBoundingBox box ) = 0;                 // 36
    virtual void __thiscall         ProcessControl() = 0;                                   // 40
    virtual void __thiscall         ProcessCollision() = 0;                                 // 44
    virtual void __thiscall         ProcessShift() = 0;                                     // 48
    virtual bool __thiscall         TestCollision() = 0;                                    // 52
    virtual void __thiscall         Teleport( float x, float y, float z, int unk ) = 0;     // 56
    virtual void __thiscall         PreFrame() = 0;                                         // 60
    virtual bool __thiscall         Frame() = 0;                                            // 64
    virtual void __thiscall         PreRender() = 0;                                        // 68
    virtual void __thiscall         Render() = 0;                                           // 72
    virtual unsigned char __thiscall    SetupLighting() = 0;                                // 76
    virtual void __thiscall         RemoveLighting( unsigned char id ) = 0;                 // 80
    virtual void __thiscall         Invalidate() = 0;                                       // 84

    unsigned char __thiscall        _SetupLighting          ( void );
    void __thiscall                 _RemoveLighting         ( unsigned char id );

    modelId_t                       GetModelIndex           ( void ) const          { return m_nModelIndex; }
    CBaseModelInfoSAInterface*      GetModelInfo            ( void ) const          { return ppModelInfo[GetModelIndex()]; }

    void                            GetPosition             ( CVector& pos ) const;
    const CVector&                  GetLODPosition          ( void ) const
    {
        if ( m_pLod )
            return m_pLod->Placeable.GetPosition();

        return Placeable.GetPosition();
    }

    CEntitySAInterface*             GetFinalLOD             ( void )
    {
        CEntitySAInterface *lod = m_pLod;

        if ( !lod )
            return this;

        while ( CEntitySAInterface *nextLOD = lod->m_pLod )
            lod = nextLOD;

        return lod;
    }

    float __thiscall                GetBasingDistance       ( void ) const;

    void                            SetAlpha                ( unsigned char alpha );
    CColModelSAInterface* __thiscall    GetColModel         ( void ) const;
    const CVector& __thiscall       GetCollisionOffset      ( CVector& out ) const;
    const CBounds2D& __thiscall     _GetBoundingBox         ( CBounds2D& out ) const;
    void __thiscall                 GetCenterPoint          ( CVector& out ) const;

    float __thiscall                GetRadius               ( void ) const          { return GetColModel()->m_bounds.fRadius; }

    float                           GetFadingAlpha          ( void ) const;

    void __thiscall                 SetOrientation          ( float x, float y, float z );

    bool __thiscall                 IsOnScreen              ( void ) const;
    bool __thiscall                 CheckScreenValidity     ( void ) const;

    void __thiscall                 UpdateRwMatrix          ( void );
    void __thiscall                 UpdateRwFrame           ( void );

    bool __thiscall                 IsInStreamingArea       ( void ) const;

    // System functions for communication with the mods.
    bool                            Reference               ( void );
    void                            Dereference             ( void );

    inline RwObject*                GetRwObject             ( void )                { return m_pRwObject; }
    inline const RwObject*          GetRwObject             ( void ) const          { return m_pRwObject; }

    RpClump     * m_pRwObject; // 24

    union
    {
        struct
        {
            /********** BEGIN CFLAGS **************/
            unsigned long bUsesCollision : 1;           // does entity use collision
            unsigned long bCollisionProcessed : 1;  // has object been processed by a ProcessEntityCollision function
            unsigned long bIsStatic : 1;                // is entity static
            unsigned long bHasContacted : 1;            // has entity processed some contact forces
            unsigned long bIsStuck : 1;             // is entity stuck
            unsigned long bIsInSafePosition : 1;        // is entity in a collision free safe position
            unsigned long bWasPostponed : 1;            // was entity control processing postponed
            unsigned long bIsVisible : 1;               //is the entity visible
            
            unsigned long bIsBIGBuilding : 1;           // Set if this entity is a big building
            unsigned long bRenderDamaged : 1;           // use damaged LOD models for objects with applicable damage
            unsigned long bStreamingDontDelete : 1; // Dont let the streaming remove this 
            unsigned long bRemoveFromWorld : 1;     // remove this entity next time it should be processed
            unsigned long bHasHitWall : 1;              // has collided with a building (changes subsequent collisions)
            unsigned long bImBeingRendered : 1;     // don't delete me because I'm being rendered
            unsigned long bDrawLast :1;             // draw object last
            unsigned long bDistanceFade :1;         // Fade entity because it is far away
            
            unsigned long bDontCastShadowsOn : 1;       // Dont cast shadows on this object
            unsigned long bOffscreen : 1;               // offscreen flag. This can only be trusted when it is set to true
            unsigned long bIsStaticWaitingForCollision : 1; // this is used by script created entities - they are static until the collision is loaded below them
            unsigned long bDontStream : 1;              // tell the streaming not to stream me
            unsigned long bUnderwater : 1;              // this object is underwater change drawing order
            unsigned long bHasPreRenderEffects : 1; // Object has a prerender effects attached to it
            unsigned long bIsTempBuilding : 1;          // whether or not the building is temporary (i.e. can be created and deleted more than once)
            unsigned long bDontUpdateHierarchy : 1; // Don't update the aniamtion hierarchy this frame
            
            unsigned long bHasRoadsignText : 1;     // entity is roadsign and has some 2deffect text stuff to be rendered
            unsigned long bDisplayedSuperLowLOD : 1;
            unsigned long bIsProcObject : 1;            // set object has been generate by procedural object generator
            unsigned long bBackfaceCulled : 1;          // has backface culling on
            unsigned long bLightObject : 1;         // light object with directional lights
            unsigned long bUnimportantStream : 1;       // set that this object is unimportant, if streaming is having problems
            unsigned long bTunnel : 1;          // Is this model part of a tunnel
            unsigned long bTunnelTransition : 1;        // This model should be rendered from within and outside of the tunnel
            /********** END CFLAGS **************/
        };
        unsigned long m_entityFlags;
    };

    WORD        RandomSeed; //32
    WORD        m_nModelIndex;//34
    CReferences *pReferences; //36
    
    void *m_streamingRef; //   CLink<CEntity*>* m_pLastRenderedLink; +40
    
    WORD m_nScanCode;           // 44
    BYTE m_iplIndex;            // used to define which IPL file object is in +46
    BYTE m_areaCode;            // used to define what objects are visible at this point +47
    
    // LOD shit
    CEntitySAInterface * m_pLod; // 48
    // num child higher level LODs
    BYTE numLodChildren; // 52
    // num child higher level LODs that have been rendered
    signed char numLodChildrenRendered; // 53

    //********* BEGIN CEntityInfo **********//
    BYTE nType : 3; // what type is the entity              // 54 (2 == Vehicle)
    BYTE nStatus : 5;               // control status       // 54
    //********* END CEntityInfo **********//

	uint8 m_pad0; // 55

    //
    // Functions to hide member variable misuse
    //

    // Sets
    void SetIsLowLodEntity ( void )
    {
        numLodChildrenRendered = 0x40;
    }

    void SetIsHighLodEntity ( void )
    {
        numLodChildrenRendered = 0x60;
    }

    void SetEntityVisibilityResult ( int result )
    {
        if ( numLodChildrenRendered & 0x60 )
            numLodChildrenRendered = ( numLodChildrenRendered & 0x60 ) | ( result & 0x1f );
    }

    // Gets
    bool IsLowLodEntity ( void ) const
    {
        return ( numLodChildrenRendered & 0x60 ) == 0x40;
    }

    bool IsHighLodEntity ( void ) const
    {
        return ( numLodChildrenRendered & 0x60 ) == 0x60;
    }

    int GetEntityVisibilityResult ( void ) const
    {
        if ( numLodChildrenRendered & 0x60 )
            return numLodChildrenRendered & 0x1f;
        return -1;
    }
};
C_ASSERT(sizeof(CEntitySAInterface) == 0x38);

void Entity_Init( void );
void Entity_Shutdown( void );

class CEntitySA : public virtual CEntity
{
    friend class COffsets;
public:
                                CEntitySA           ( void );

    CEntitySAInterface*         m_pInterface;

    DWORD                       internalID;
//  VOID                        SetModelAlpha ( int iAlpha );

    inline CEntitySAInterface * GetInterface() { return m_pInterface; };
    VOID                        SetInterface( CEntitySAInterface * intInterface ) { m_pInterface = intInterface; };

    VOID                        SetPosition ( float fX, float fY, float fZ );
    VOID                        Teleport ( float fX, float fY, float fZ );
    VOID                        ProcessControl ( void );
    VOID                        SetupLighting ( );
    VOID                        Render ( );
    VOID                        SetOrientation ( float fX, float fY, float fZ );
    VOID                        FixBoatOrientation ( void );        // eAi you might want to rename this
    VOID                        SetPosition ( CVector * vecPosition );

    void                        SetUnderwater ( bool bUnderwater );
    bool                        GetUnderwater ( void );

    virtual void                RestoreLastGoodPhysicsState ( void );
    CVector*                    GetPosition                 ( void );
    CVector*                    GetPositionInternal         ( void );
    CMatrix*                    GetMatrix                   ( CMatrix* matrix );
    CMatrix*                    GetMatrixInternal           ( CMatrix* matrix );
    VOID                        SetMatrix                   ( CMatrix* matrix );
    WORD                        GetModelIndex ();
    eEntityType                 GetEntityType ();
    bool                        IsOnScreen ();
    bool                        IsFullyVisible ();

    bool                        IsVisible ( void );
    void                        SetVisible ( bool bVisible );

    BYTE                        GetAreaCode ( void );
    void                        SetAreaCode ( BYTE areaCode );

    FLOAT                       GetDistanceFromCentreOfMassToBaseOfModel();
	
    VOID                        SetEntityStatus( eEntityStatus bStatus );
    eEntityStatus               GetEntityStatus( );

    RwFrame *                   GetFrameFromId ( int id );
    RwMatrix *                  GetLTMFromId ( int id );

    RpClump *                   GetRpClump ();


    BOOL                        BeingDeleted; // to prevent it trying to delete twice
    BOOL                        DoNotRemoveFromGame; // when deleted, if this is true, it won't be removed from the game

    VOID                        SetDoNotRemoveFromGameWhenDeleted ( bool bDoNotRemoveFromGame ) { DoNotRemoveFromGame = bDoNotRemoveFromGame; };
    BOOL                        IsStatic(VOID)                          { return m_pInterface->bIsStatic; }
    VOID                        SetStatic(BOOL bStatic)                 { m_pInterface->bIsStatic       = bStatic; };
    VOID                        SetUsesCollision(BOOL bUsesCollision)   { m_pInterface->bUsesCollision  = bUsesCollision;};
    BOOL                        IsBackfaceCulled(VOID)                  { return m_pInterface->bBackfaceCulled; };
    VOID                        SetBackfaceCulled(BOOL bBackfaceCulled) { m_pInterface->bBackfaceCulled = bBackfaceCulled; };
    VOID                        SetAlpha(DWORD dwAlpha);

    VOID                        MatrixConvertFromEulerAngles ( float fX, float fY, float fZ, int iUnknown );
    VOID                        MatrixConvertToEulerAngles ( float * fX, float * fY, float * fZ, int iUnknown );

    bool                        IsPlayingAnimation ( char * szAnimName );

    void*                       GetStoredPointer    ( void )                { return m_pStoredPointer; };
    void                        SetStoredPointer    ( void* pPointer )      { m_pStoredPointer = pPointer; };

    bool                        IsStaticWaitingForCollision  ( void )        { return m_pInterface->bIsStaticWaitingForCollision; }
    void                        SetStaticWaitingForCollision  ( bool bStatic ) { m_pInterface->bIsStaticWaitingForCollision  = bStatic; }

    // CEntitySA interface
    virtual void                OnChangingPosition      ( const CVector& vecNewPosition ) {}

private:
    static unsigned long        FUNC_CClumpModelInfo__GetFrameFromId;
    static unsigned long        FUNC_RwFrameGetLTM;

    unsigned long               m_ulArrayID;
    void*                       m_pStoredPointer;
    CVector                     m_LastGoodPosition;
};

#include "CEntitySA.render.h"
#include "CEntitySA.renderfeedback.h"
#include "CEntitySA.lod.h"
#include "CEntitySA.rendersetup.h"
#include "CEntitySA.renderutils.h"

namespace Entity
{
    void    SetReferenceCallbacks( entityReferenceCallback_t addRef, entityReferenceCallback_t delRef );
};


//
// Check for various number problems
//

inline bool IsValidPosition ( const CVector& vec )
{
    if ( vec.fX < -16000 || vec.fX > 16000 || _isnan ( vec.fX )
        || vec.fY < -16000 || vec.fY > 16000 || _isnan ( vec.fY )
        || vec.fZ < -5000 || vec.fZ > 100000 || _isnan ( vec.fZ ) )
        return false;          
    return true;
}

inline bool IsValidMatrix ( const CMatrix& mat )
{
    return IsValidPosition ( mat.vPos )
        && IsValidPosition ( mat.vFront );
}


#endif
