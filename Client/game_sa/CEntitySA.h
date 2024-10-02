/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CEntitySA.h
 *  PURPOSE:     Header file for base entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CEntity.h>
#include <CMatrix.h>
#include <CMatrix_Pad.h>
#include <CVector2D.h>
#include <CVector.h>

#define FUNC_GetDistanceFromCentreOfMassToBaseOfModel       0x536BE0

#define FUNC_SetRwObjectAlpha                               0x5332C0
#define FUNC_SetOrientation                                 0x439A80

#define FUNC_CMatrix__ConvertToEulerAngles                  0x59A840
#define FUNC_CMatrix__ConvertFromEulerAngles                0x59AA40

#define FUNC_IsVisible                                      0x536BC0

// not in CEntity really
#define FUNC_RpAnimBlendClumpGetAssociation                 0x4D6870

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
    CEntitySAInterfaceVTBL* vtbl;            // the virtual table it should be in the CPlaceableSAInterface

    CPlaceableSAInterface Placeable;            // 4

    RpClump* m_pRwObject;            // 24
    /********** BEGIN CFLAGS **************/
    unsigned long bUsesCollision : 1;                 // does entity use collision
    unsigned long bCollisionProcessed : 1;            // has object been processed by a ProcessEntityCollision function
    unsigned long bIsStatic : 1;                      // is entity static
    unsigned long bHasContacted : 1;                  // has entity processed some contact forces
    unsigned long bIsStuck : 1;                       // is entity stuck
    unsigned long bIsInSafePosition : 1;              // is entity in a collision free safe position
    unsigned long bWasPostponed : 1;                  // was entity control processing postponed
    unsigned long bIsVisible : 1;                     // is the entity visible

    unsigned long bIsBIGBuilding : 1;                  // Set if this entity is a big building
    unsigned long bRenderDamaged : 1;                  // use damaged LOD models for objects with applicable damage
    unsigned long bStreamingDontDelete : 1;            // Dont let the streaming remove this
    unsigned long bRemoveFromWorld : 1;                // remove this entity next time it should be processed
    unsigned long bHasHitWall : 1;                     // has collided with a building (changes subsequent collisions)
    unsigned long bImBeingRendered : 1;                // don't delete me because I'm being rendered
    unsigned long bDrawLast : 1;                       // draw object last
    unsigned long bDistanceFade : 1;                   // Fade entity because it is far away

    unsigned long bDontCastShadowsOn : 1;            // Dont cast shadows on this object
    unsigned long bOffscreen : 1;                    // offscreen flag. This can only be trusted when it is set to true
    unsigned long
        bIsStaticWaitingForCollision : 1;              // this is used by script created entities - they are static until the collision is loaded below them
    unsigned long bDontStream : 1;                     // tell the streaming not to stream me
    unsigned long bUnderwater : 1;                     // this object is underwater change drawing order
    unsigned long bHasPreRenderEffects : 1;            // Object has a prerender effects attached to it
    unsigned long bIsTempBuilding : 1;                 // whether or not the building is temporary (i.e. can be created and deleted more than once)
    unsigned long bDontUpdateHierarchy : 1;            // Don't update the aniamtion hierarchy this frame

    unsigned long bHasRoadsignText : 1;            // entity is roadsign and has some 2deffect text stuff to be rendered
    unsigned long bDisplayedSuperLowLOD : 1;
    unsigned long bIsProcObject : 1;                 // set object has been generate by procedural object generator
    unsigned long bBackfaceCulled : 1;               // has backface culling on
    unsigned long bLightObject : 1;                  // light object with directional lights
    unsigned long bUnimportantStream : 1;            // set that this object is unimportant, if streaming is having problems
    unsigned long bTunnel : 1;                       // Is this model part of a tunnel
    unsigned long bTunnelTransition : 1;             // This model should be rendered from within and outside of the tunnel
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

    void ResolveReferences()
    {
        using CEntity_ResolveReferences = void*(__thiscall*)(CEntitySAInterface*);
        ((CEntity_ResolveReferences)0x571A40)(this);
    };

    void RemoveShadows()
    {
        using CStencilShadow_dtorByOwner = void*(__cdecl*)(CEntitySAInterface * pEntity);
        ((CStencilShadow_dtorByOwner)0x711730)(this);
    };

    void DeleteRwObject()
    {
        using vtbl_DeleteRwObject = void(__thiscall*)(CEntitySAInterface * pEntity);
        ((vtbl_DeleteRwObject)this->vtbl->DeleteRwObject)(this);
    };

    bool HasMatrix() const noexcept { return Placeable.matrix != nullptr; }

    void RemoveMatrix() { ((void(__thiscall*)(void*))0x54F3B0)(this); }
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
    bool GetBoneRotationQuat(eBone boneId, float& x, float& y, float& z, float& w);
    bool SetBoneRotation(eBone boneId, float yaw, float pitch, float roll);
    bool SetBoneRotationQuat(eBone boneId, float x, float y, float z, float w);
    bool GetBonePosition(eBone boneId, CVector& position);
    bool SetBonePosition(eBone boneId, const CVector& position);

    // CEntitySA interface
    virtual void OnChangingPosition(const CVector& vecNewPosition) {}

private:
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
