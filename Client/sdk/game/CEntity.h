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
#include <CMatrix.h>
#include <CVector.h>
#include "RenderWare.h"

#include <windows.h>

enum eBone;

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
    virtual VOID                      SetPosition(float fX, float fY, float fZ) = 0;
    virtual VOID                      SetPosition(CVector* vecPosition) = 0;
    virtual VOID                      Teleport(float fX, float fY, float fZ) = 0;
    virtual VOID                      ProcessControl() = 0;
    virtual VOID                      SetupLighting() = 0;
    virtual VOID                      Render() = 0;
    virtual VOID                      SetOrientation(float fX, float fY, float fZ) = 0;
    virtual VOID                      FixBoatOrientation() = 0;
    virtual CVector*                  GetPosition() = 0;
    virtual CMatrix*                  GetMatrix(CMatrix* matrix) = 0;
    virtual VOID                      SetMatrix(CMatrix* matrix) = 0;
    virtual WORD                      GetModelIndex() = 0;
    virtual eEntityType               GetEntityType() = 0;
    virtual FLOAT                     GetDistanceFromCentreOfMassToBaseOfModel() = 0;
    virtual VOID                      SetEntityStatus(eEntityStatus bStatus) = 0;
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

    virtual VOID SetDoNotRemoveFromGameWhenDeleted(bool bDoNotRemoveFromGame) = 0;
    virtual VOID SetUsesCollision(BOOL bUsesCollision) = 0;
    virtual BOOL IsBackfaceCulled() = 0;
    virtual VOID SetBackfaceCulled(BOOL bBackfaceCulled) = 0;
    virtual BOOL IsStatic() = 0;
    virtual VOID SetStatic(BOOL bStatic) = 0;
    virtual VOID SetAlpha(DWORD dwAlpha) = 0;

    virtual VOID MatrixConvertFromEulerAngles(float fX, float fY, float fZ, int iUnknown) = 0;
    virtual VOID MatrixConvertToEulerAngles(float* fX, float* fY, float* fZ, int iUnknown) = 0;
    virtual bool IsPlayingAnimation(char* szAnimName) = 0;

    virtual void* GetStoredPointer() = 0;
    virtual void  SetStoredPointer(void* pPointer) = 0;

    virtual BYTE GetAreaCode() = 0;
    virtual void SetAreaCode(BYTE areaCode) = 0;

    virtual bool IsStaticWaitingForCollision() = 0;
    virtual void SetStaticWaitingForCollision(bool bStatic) = 0;

    virtual unsigned long GetArrayID() = 0;

    virtual RwMatrixTag* GetBoneRwMatrix(eBone boneId) = 0;
    virtual bool         SetBoneMatrix(eBone boneId, const CMatrix& matrix) = 0;

    virtual bool GetBoneRotation(eBone boneId, float& yaw, float& pitch, float& roll) = 0;
    virtual bool SetBoneRotation(eBone boneId, float yaw, float pitch, float roll) = 0;
    virtual bool GetBonePosition(eBone boneId, CVector& position) = 0;
    virtual bool SetBonePosition(eBone boneId, const CVector& position) = 0;
};
