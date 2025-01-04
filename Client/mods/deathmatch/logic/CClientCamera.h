/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientCamera.h
 *  PURPOSE:     Camera entity class
 *
 *****************************************************************************/

#pragma once

#include <CMatrix.h>
#include <game/CCamera.h>
#include "CClientCommon.h"
#include "CClientEntity.h"

class CClientEntity;
class CClientManager;
class CClientPlayer;
class CClientPlayerManager;

namespace EFixedCameraMode
{
    enum EFixedCameraModeType
    {
        ROTATION,
        TARGET,
        MATRIX,
    };
}
using EFixedCameraMode::EFixedCameraModeType;

class CClientCamera final : public CClientEntity
{
    DECLARE_CLASS(CClientCamera, CClientEntity)
    friend class CClientManager;

public:
    void Unlink(){};
    void DoPulse();

    eClientEntityType GetType() const { return CCLIENTCAMERA; };

    bool  SetMatrix(const CMatrix& Matrix);
    bool  GetMatrix(CMatrix& Matrix) const;
    void  GetPosition(CVector& vecPosition) const;
    void  SetPosition(const CVector& vecPosition);
    void  GetRotationDegrees(CVector& vecRotation) const;
    void  SetRotationRadians(const CVector& vecRotation);
    void  GetFixedTarget(CVector& vecTarget, float* pfRoll = NULL) const;
    void  SetFixedTarget(const CVector& vecPosition, float fRoll = 0);
    float GetFOV() { return m_fFOV; }
    void  SetFOV(float fFOV) { m_fFOV = fFOV; }
    void  SetOrbitTarget(const CVector& vecPosition);
    void  AttachTo(CClientEntity* pElement);

    void FadeIn(float fTime);
    void FadeOut(float fTime, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue);

    CClientPlayer* GetFocusedPlayer() { return m_pFocusedPlayer; };
    void           SetFocus(CClientEntity* pEntity, eCamMode eMode, bool bSmoothTransition = false);
    void           SetFocus(CClientPlayer* pPlayer, eCamMode eMode, bool bSmoothTransition = false);
    void           SetFocusToLocalPlayer();
    void           Reset();

    void ShakeCamera(float radius, float x, float y, float z) noexcept;
    void ResetShakeCamera() noexcept;

    void            SetCameraVehicleViewMode(eVehicleCamMode eMode);
    void            SetCameraPedViewMode(ePedCamMode eMode);
    eVehicleCamMode GetCameraVehicleViewMode();
    ePedCamMode     GetCameraPedViewMode();
    void            SetCameraClip(bool bObjects, bool bVehicles);
    void            GetCameraClip(bool& bObjects, bool& bVehicles);

    bool IsInFixedMode() { return m_bFixed; }
    void ToggleCameraFixedMode(bool bEnabled);

    void           SetTargetEntity(CClientEntity* pEntity);
    CClientEntity* GetTargetEntity();

    void UnreferencePlayer(CClientPlayer* pPlayer);

private:
    CClientCamera(CClientManager* pManager);
    ~CClientCamera();

    static bool StaticProcessFixedCamera(CCam* pCam);
    bool        ProcessFixedCamera(CCam* pCam);
    void        SetFocus(CVector* vecTarget, bool bSmoothTransition);
    CMatrix     GetGtaMatrix() const;
    void        SetGtaMatrix(const CMatrix& matInNew, CCam* pCam = NULL) const;

    void SetFocusToLocalPlayerImpl();

    void UnreferenceEntity(CClientEntity* pEntity);
    void InvalidateEntity(CClientEntity* pEntity);
    void RestoreEntity(CClientEntity* pEntity);

    CClientPlayerManager* m_pPlayerManager;

    CClientPlayerPtr m_pFocusedPlayer;
    CClientEntityPtr m_pFocusedEntity;
    CEntity*         m_pFocusedGameEntity;
    bool             m_bInvalidated;

    bool                 m_bFixed;
    EFixedCameraModeType m_FixedCameraMode;
    CVector              m_vecFixedTarget;
    float                m_fRoll;
    float                m_fFOV;
    CMatrix              m_matFixedMatrix;

    CCamera* m_pCamera;
};
