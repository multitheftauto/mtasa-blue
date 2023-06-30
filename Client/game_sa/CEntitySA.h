/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CEntitySA.h
 *  PURPOSE:     Header file for base entity class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CEntity.h>
#include <CMatrix.h>
#include <CMatrix_Pad.h>
#include <CVector2D.h>
#include <CVector.h>

#define FUNC_SetRwObjectAlpha                               0x5332C0 // CMultiplayerSA.cpp
#define FUNC_CMatrix__ConvertToEulerAngles                  0x59A840 // CMultiplayerSA.cpp
#define FUNC_CMatrix__ConvertFromEulerAngles                0x59AA40 // CMultiplayerSA.cpp

class CRect;
class CEntitySAInterfaceVTBL
{
public:
    DWORD SCALAR_DELETING_DESTRUCTOR;                 // +0h
    DWORD Add_CRect;                                  // +4h
    DWORD Add;                                        // +8h
    DWORD Remove;                                     // +Ch
    DWORD SetIsStatic;                                // +10h
    DWORD SetModelIndex;                              // +14h
    DWORD SetModelIndexNoCreate;                      // +18h
    DWORD CreateRwObject;                             // +1Ch
    DWORD DeleteRwObject;                             // +20h
    DWORD GetBoundRect;                               // +24h
    DWORD ProcessControl;                             // +28h
    DWORD ProcessCollision;                           // +2Ch
    DWORD ProcessShift;                               // +30h
    DWORD TestCollision;                              // +34h
    DWORD Teleport;                                   // +38h
    DWORD SpecialEntityPreCollisionStuff;             // +3Ch
    DWORD SpecialEntityCalcCollisionSteps;            // +40h
    DWORD PreRender;                                  // +44h
    DWORD Render;                                     // +48h
    DWORD SetupLighting;                              // +4Ch
    DWORD RemoveLighting;                             // +50h
    DWORD FlagToDestroyWhenNextProcessed;             // +54h
};

/**
 * \todo Move CReferences (and others below?) into it's own file
 */
class CReference
{
public:
    CReference*                pNext;
    class CEntitySAInterface** ppEntity;
};

class CReferences
{
public:
    CReference  m_refs[3000];
    CReference* m_pFreeList;
};

class CMatrixEx
{
public:
    CMatrix_Padded matrix;
    CMatrix*       pMatrix;                 // usually not initialized
    void*          haveRwMatrix;            // unknown pointer
};

class XYZ
{
public:
    CMatrixEx                    matrix;
    class CPlaceableSAInterface* pRef;
    XYZ*                         pPrev;
    XYZ*                         pNext;
};
static_assert(sizeof(XYZ) == 0x54, "Invalid size for XYZ");

class XYZStore
{
public:
    XYZ  head;
    XYZ  tail;
    XYZ  allocatedListHead;
    XYZ  allocatedListTail;
    XYZ  freeListHead;
    XYZ  freeListTail;
    XYZ* pPool;
};
static_assert(sizeof(XYZStore) == 0x1FC, "Invalid size for XYZStore");

class CSimpleTransformSAInterface            // 16 bytes
{
public:
    CVector m_translate;
    float   m_heading;
};

class CPlaceableSAInterface            // 20 bytes
{
public:
    CSimpleTransformSAInterface m_transform;
    CMatrix_Padded*             matrix;            // This is actually XYZ*, change later
};

class CEntitySAInterface
{
public:
    CEntitySAInterfaceVTBL* vtbl;            // the virtual table

    CPlaceableSAInterface Placeable;            // 4

    RpClump* m_pRwObject;            // 24
    /********** BEGIN CFLAGS **************/
    ulong bUsesCollision : 1;               // Does entity use collision
    ulong bCollisionProcessed : 1;          // Has object been processed by a ProcessEntityCollision function
    ulong bIsStatic : 1;                    // Is entity static
    ulong bHasContacted : 1;                // Has entity processed some contact forces
    ulong bIsStuck : 1;                     // Is entity stuck
    ulong bIsInSafePosition : 1;            // Is entity in a collision free safe position
    ulong bWasPostponed : 1;                // Was entity control processing postponed
    ulong bIsVisible : 1;                   // Is the entity visible
    ulong bIsBIGBuilding : 1;               // Set if this entity is a big building
    ulong bRenderDamaged : 1;               // Use damaged LOD models for objects with applicable damage
    ulong bStreamingDontDelete : 1;         // Dont let the streaming remove this
    ulong bRemoveFromWorld : 1;             // Remove this entity next time it should be processed
    ulong bHasHitWall : 1;                  // Has collided with a building (changes subsequent collisions)
    ulong bImBeingRendered : 1;             // Don't delete me because I'm being rendered
    ulong bDrawLast : 1;                    // Draw object last
    ulong bDistanceFade : 1;                // Fade entity because it is far away
    ulong bDontCastShadowsOn : 1;           // Dont cast shadows on this object
    ulong bOffscreen : 1;                   // Offscreen flag. This can only be trusted when it is set to true
    ulong bIsStaticWaitingForCollision : 1; // This is used by script created entities - they are static until the collision is loaded below them
    ulong bDontStream : 1;                  // Tell the streaming not to stream me
    ulong bUnderwater : 1;                  // This object is underwater change drawing order
    ulong bHasPreRenderEffects : 1;         // Object has a prerender effects attached to it
    ulong bIsTempBuilding : 1;              // Whether or not the building is temporary (i.e. can be created and deleted more than once)
    ulong bDontUpdateHierarchy : 1;         // Don't update the aniamtion hierarchy this frame
    ulong bHasRoadsignText : 1;             // Entity is roadsign and has some 2deffect text stuff to be rendered
    ulong bDisplayedSuperLowLOD : 1;
    ulong bIsProcObject : 1;                // Set object has been generate by procedural object generator
    ulong bBackfaceCulled : 1;              // Gas backface culling on
    ulong bLightObject : 1;                 // Light object with directional lights
    ulong bUnimportantStream : 1;           // Set that this object is unimportant, if streaming is having problems
    ulong bTunnel : 1;                      // Is this model part of a tunnel
    ulong bTunnelTransition : 1;            // This model should be rendered from within and outside of the tunnel
    /********** END CFLAGS **************/

    WORD         RandomSeed;               // 32
    WORD         m_nModelIndex;            // 34
    CReferences* pReferences;              // 36

    DWORD* m_pLastRenderedLink;            //   CLink<CEntity*>* m_pLastRenderedLink; +40

    WORD m_nScanCode;            // 44
    BYTE m_iplIndex;             // used to define which IPL file object is in +46
    BYTE m_areaCode;             // used to define what objects are visible at this point +47

    // LOD shit
    CEntitySAInterface* m_pLod;            // 48
    // num child higher level LODs
    BYTE numLodChildren;            // 52
    // num child higher level LODs that have been rendered
    signed char numLodChildrenRendered;            // 53

    //********* BEGIN CEntityInfo **********//
    BYTE nType : 3;              // what type is the entity              // 54 (2 == Vehicle)
    BYTE nStatus : 5;            // control status       // 54
    //********* END CEntityInfo **********//

    uint8 m_pad0;            // 55

    CRect*      GetBoundRect_(CRect* pRect);
    void        TransformFromObjectSpace(CVector& outPosn, CVector const& offset);
    CVector*    GetBoundCentre(CVector* pOutCentre);
    void        UpdateRW();
    void        UpdateRpHAnim();
    static void StaticSetHooks();

    //
    // Functions to hide member variable misuse
    //

    // Sets
    void SetIsLowLodEntity() { numLodChildrenRendered = 0x40; }

    void SetIsHighLodEntity() { numLodChildrenRendered = 0x60; }

    void SetEntityVisibilityResult(int result)
    {
        if (numLodChildrenRendered & 0x60)
            numLodChildrenRendered = (numLodChildrenRendered & 0x60) | (result & 0x1f);
    }

    // Gets
    bool IsLowLodEntity() const { return (numLodChildrenRendered & 0x60) == 0x40; }

    bool IsHighLodEntity() const { return (numLodChildrenRendered & 0x60) == 0x60; }

    int GetEntityVisibilityResult() const
    {
        if (numLodChildrenRendered & 0x60)
            return numLodChildrenRendered & 0x1f;
        return -1;
    }
};
static_assert(sizeof(CEntitySAInterface) == 0x38, "Invalid size for CEntitySAInterface");

class CEntitySA : public virtual CEntity
{
    friend class COffsets;

public:
    CEntitySAInterface* m_pInterface;

    CEntitySA();
    CEntitySAInterface* GetInterface() { return m_pInterface; };
    void                SetInterface(CEntitySAInterface* intInterface) { m_pInterface = intInterface; };

    bool IsPed() { return GetEntityType() == ENTITY_TYPE_PED; }
    void UpdateRpHAnim();
    bool SetScaleInternal(const CVector& scale);
    void SetPosition(float fX, float fY, float fZ);
    void Teleport(float fX, float fY, float fZ);
    void ProcessControl();
    void SetupLighting();
    void Render();
    void SetOrientation(float fX, float fY, float fZ);
    void FixBoatOrientation();            // eAi you might want to rename this
    void SetPosition(CVector* vecPosition);

    void SetUnderwater(bool bUnderwater);
    bool GetUnderwater();

    virtual void RestoreLastGoodPhysicsState();
    CVector*     GetPosition();
    CVector*     GetPositionInternal();
    CMatrix*     GetMatrix(CMatrix* matrix);
    CMatrix*     GetMatrixInternal(CMatrix* matrix);
    void         SetMatrix(CMatrix* matrix);
    WORD         GetModelIndex();
    eEntityType  GetEntityType();
    bool         IsOnScreen();
    bool         IsFullyVisible();

    bool IsVisible();
    void SetVisible(bool bVisible);

    BYTE GetAreaCode();
    void SetAreaCode(BYTE areaCode);

    float GetDistanceFromCentreOfMassToBaseOfModel();

    void          SetEntityStatus(eEntityStatus bStatus);
    eEntityStatus GetEntityStatus();

    RwFrame*  GetFrameFromId(int id);
    RwMatrix* GetLTMFromId(int id);

    RpClump* GetRpClump();

    bool BeingDeleted;                   // to prevent it trying to delete twice
    bool DoNotRemoveFromGame;            // when deleted, if this is true, it won't be removed from the game

    void SetDoNotRemoveFromGameWhenDeleted(bool bDoNotRemoveFromGame) { DoNotRemoveFromGame = bDoNotRemoveFromGame; };
    bool IsStatic() { return m_pInterface->bIsStatic; }
    void SetStatic(bool bStatic) { m_pInterface->bIsStatic = bStatic; };
    void SetUsesCollision(bool bUsesCollision) { m_pInterface->bUsesCollision = bUsesCollision; };
    bool IsBackfaceCulled() { return m_pInterface->bBackfaceCulled; };
    void SetBackfaceCulled(bool bBackfaceCulled) { m_pInterface->bBackfaceCulled = bBackfaceCulled; };
    void SetAlpha(DWORD dwAlpha);

    void MatrixConvertFromEulerAngles(float fX, float fY, float fZ, int iUnknown);
    void MatrixConvertToEulerAngles(float* fX, float* fY, float* fZ, int iUnknown);

    bool IsPlayingAnimation(char* szAnimName);

    void* GetStoredPointer() { return m_pStoredPointer; };
    void  SetStoredPointer(void* pPointer) { m_pStoredPointer = pPointer; };

    bool IsStaticWaitingForCollision() { return m_pInterface->bIsStaticWaitingForCollision; }
    void SetStaticWaitingForCollision(bool bStatic) { m_pInterface->bIsStaticWaitingForCollision = bStatic; }

    RwMatrix* GetBoneRwMatrix(eBone boneId);
    bool      SetBoneMatrix(eBone boneId, const CMatrix& matrix);

    bool GetBoneRotation(eBone boneId, float& yaw, float& pitch, float& roll);
    bool SetBoneRotation(eBone boneId, float yaw, float pitch, float roll);
    bool GetBonePosition(eBone boneId, CVector& position);
    bool SetBonePosition(eBone boneId, const CVector& position);

    // CEntitySA interface
    virtual void OnChangingPosition(const CVector& vecNewPosition) {}

private:
    static unsigned long FUNC_CClumpModelInfo__GetFrameFromId;
    static unsigned long FUNC_RwFrameGetLTM;

    void*   m_pStoredPointer;
    CVector m_LastGoodPosition;
};

//
// Check for various number problems
//

inline bool IsValidPosition(const CVector& vec)
{
    if (vec.fX < -16000 || vec.fX > 16000 || std::isnan(vec.fX) || vec.fY < -16000 || vec.fY > 16000 || std::isnan(vec.fY) || vec.fZ < -5000 ||
        vec.fZ > 100000 || std::isnan(vec.fZ))
        return false;
    return true;
}

inline bool IsValidMatrix(const CMatrix& mat)
{
    return IsValidPosition(mat.vPos) && IsValidPosition(mat.vFront);
}
