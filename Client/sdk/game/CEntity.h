/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CEntity.h
 *  PURPOSE:     Base entity interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Common.h"

class CEntitySAInterface;
class CMatrix;
class CVector;
enum eBone;
struct RpClump;
struct RwFrame;
struct RwMatrix;

enum eEntityType
{
    ENTITY_TYPE_NOTHING,
    ENTITY_TYPE_BUILDING,
    ENTITY_TYPE_VEHICLE,
    ENTITY_TYPE_PED,
    ENTITY_TYPE_OBJECT,
    ENTITY_TYPE_DUMMY,
    ENTITY_TYPE_NOTINPOOLS
};

enum eEntityStatus
{
    STATUS_PLAYER,
    STATUS_PLAYER_PLAYBACKFROMBUFFER,
    STATUS_SIMPLE,
    STATUS_PHYSICS,
    STATUS_ABANDONED,
    STATUS_WRECKED,
    STATUS_TRAIN_MOVING,
    STATUS_TRAIN_NOT_MOVING,
    STATUS_HELI,
    STATUS_PLANE,
    STATUS_REMOTE_CONTROLLED,
    STATUS_PLAYER_DISABLED,
    STATUS_TRAILER,                  // ToDo: Check this. I can't reproduce that this is set for trailers
    STATUS_SIMPLE_TRAILER            // Same here
};

class CEntity
{
public:
    virtual ~CEntity(){};

    //  virtual VOID                        SetModelAlpha ( int iAlpha )=0;
    virtual class CEntitySAInterface* GetInterface() = 0;
    virtual void                      UpdateRpHAnim() = 0;
    virtual bool                      SetScaleInternal(const CVector& scale) = 0;
    virtual void                      SetPosition(float fX, float fY, float fZ) = 0;
    virtual void                      SetPosition(CVector* vecPosition) = 0;
    virtual void                      Teleport(float fX, float fY, float fZ) = 0;
    virtual void                      ProcessControl() = 0;
    virtual void                      SetupLighting() = 0;
    virtual void                      Render() = 0;
    virtual void                      SetOrientation(float fX, float fY, float fZ) = 0;
    virtual void                      FixBoatOrientation() = 0;
    virtual CVector*                  GetPosition() = 0;
    virtual CMatrix*                  GetMatrix(CMatrix* matrix) = 0;
    virtual void                      SetMatrix(CMatrix* matrix) = 0;
    virtual WORD                      GetModelIndex() = 0;
    virtual eEntityType               GetEntityType() = 0;
    virtual float                     GetDistanceFromCentreOfMassToBaseOfModel() = 0;
    virtual void                      SetEntityStatus(eEntityStatus bStatus) = 0;
    virtual eEntityStatus             GetEntityStatus() = 0;
    virtual bool                      IsOnScreen() = 0;
    virtual bool                      IsFullyVisible() = 0;
    virtual void                      SetUnderwater(bool bUnderwater) = 0;
    virtual bool                      GetUnderwater() = 0;
    virtual RwFrame*                  GetFrameFromId(int id) = 0;
    virtual RwMatrix*                 GetLTMFromId(int id) = 0;

    virtual RpClump* GetRpClump() = 0;

    virtual bool IsVisible() = 0;
    virtual void SetVisible(bool bVisible) = 0;

    virtual void SetDoNotRemoveFromGameWhenDeleted(bool bDoNotRemoveFromGame) = 0;
    virtual void SetUsesCollision(bool bUsesCollision) = 0;
    virtual bool IsBackfaceCulled() = 0;
    virtual void SetBackfaceCulled(bool bBackfaceCulled) = 0;
    virtual bool IsStatic() = 0;
    virtual void SetStatic(bool bStatic) = 0;
    virtual void SetAlpha(DWORD dwAlpha) = 0;

    virtual void MatrixConvertFromEulerAngles(float fX, float fY, float fZ, int iUnknown) = 0;
    virtual void MatrixConvertToEulerAngles(float* fX, float* fY, float* fZ, int iUnknown) = 0;
    virtual bool IsPlayingAnimation(char* szAnimName) = 0;

    virtual void* GetStoredPointer() = 0;
    virtual void  SetStoredPointer(void* pPointer) = 0;

    virtual BYTE GetAreaCode() = 0;
    virtual void SetAreaCode(BYTE areaCode) = 0;

    virtual bool IsStaticWaitingForCollision() = 0;
    virtual void SetStaticWaitingForCollision(bool bStatic) = 0;

    virtual RwMatrix* GetBoneRwMatrix(eBone boneId) = 0;
    virtual bool      SetBoneMatrix(eBone boneId, const CMatrix& matrix) = 0;

    virtual bool GetBoneRotation(eBone boneId, float& yaw, float& pitch, float& roll) = 0;
    virtual bool GetBoneRotationQuat(eBone boneId, float& x, float& y, float& z, float& w) = 0;
    virtual bool SetBoneRotation(eBone boneId, float yaw, float pitch, float roll) = 0;
    virtual bool SetBoneRotationQuat(eBone boneId, float x, float y, float z, float w) = 0;
    virtual bool GetBonePosition(eBone boneId, CVector& position) = 0;
    virtual bool SetBonePosition(eBone boneId, const CVector& position) = 0;

    virtual bool IsOnFire() = 0;
    virtual bool SetOnFire(bool onFire) = 0;
};
