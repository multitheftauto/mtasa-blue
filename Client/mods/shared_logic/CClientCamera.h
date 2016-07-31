/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientCamera.h
*  PURPOSE:     Camera entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*
*****************************************************************************/

#ifndef __CCLIENTCAMERA_H
#define __CCLIENTCAMERA_H

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

class CClientCamera : public CClientEntity
{
    DECLARE_CLASS( CClientCamera, CClientEntity )
    friend class CClientManager;

public:
    inline void                 Unlink                      ( void )                        { };
    void                        DoPulse                     ( void );

    inline eClientEntityType    GetType                     ( void ) const                  { return CCLIENTCAMERA; };

    bool                        SetMatrix                   ( const CMatrix& Matrix );
    bool                        GetMatrix                   ( CMatrix& Matrix ) const;
    void                        GetPosition                 ( CVector& vecPosition ) const;
    void                        SetPosition                 ( const CVector& vecPosition );
    void                        GetRotationDegrees          ( CVector& vecRotation ) const;
    void                        SetRotationRadians          ( const CVector& vecRotation );
    void                        GetFixedTarget              ( CVector& vecTarget, float* pfRoll = NULL ) const;
    void                        SetFixedTarget              ( const CVector& vecPosition, float fRoll = 0 );
    float                       GetFOV                      ()                              { return m_fFOV; }
    void                        SetFOV                      ( float fFOV )                  { m_fFOV = fFOV; }
    void                        SetOrbitTarget              ( const CVector& vecPosition );
    void                        AttachTo                    ( CClientEntity* pElement );

    void                        FadeIn                      ( float fTime );
    void                        FadeOut                     ( float fTime, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue );

    inline CClientPlayer*       GetFocusedPlayer            ( void )                        { return m_pFocusedPlayer; };
    void                        SetFocus                    ( CClientEntity* pEntity, eCamMode eMode, bool bSmoothTransition = false );
    void                        SetFocus                    ( CClientPlayer* pPlayer, eCamMode eMode, bool bSmoothTransition = false );
    void                        SetFocusToLocalPlayer       ( void );

    void                        SetCameraViewMode           ( eVehicleCamMode eMode );
    eVehicleCamMode             GetCameraViewMode           ( void );
    void                        SetCameraClip               ( bool bObjects, bool bVehicles );
    void                        GetCameraClip               ( bool &bObjects, bool &bVehicles );

    inline bool                 IsInFixedMode               ( void )                        { return m_bFixed; }
    void                        ToggleCameraFixedMode       ( bool bEnabled );

    CClientEntity *             GetTargetEntity             ( void );

    void                        UnreferencePlayer           ( CClientPlayer* pPlayer );

private:
                                CClientCamera               ( CClientManager* pManager );
                                ~CClientCamera              ( void );

    static bool                 StaticProcessFixedCamera    ( CCam* pCam );
    bool                        ProcessFixedCamera          ( CCam* pCam );
    void                        SetFocus                    ( CVector * vecTarget, bool bSmoothTransition );
    CMatrix                     GetGtaMatrix                ( void ) const;
    void                        SetGtaMatrix                ( const CMatrix& matInNew, CCam* pCam = NULL ) const;

    void                        SetFocusToLocalPlayerImpl   ( void );

    void                        UnreferenceEntity           ( CClientEntity* pEntity );    
    void                        InvalidateEntity            ( CClientEntity* pEntity );
    void                        RestoreEntity               ( CClientEntity* pEntity );

    CClientPlayerManager*       m_pPlayerManager;

    CClientPlayerPtr            m_pFocusedPlayer;
    CClientEntityPtr            m_pFocusedEntity;
    CEntity*                    m_pFocusedGameEntity;
    bool                        m_bInvalidated;

    bool                        m_bFixed;
    EFixedCameraModeType        m_FixedCameraMode;
    CVector                     m_vecFixedTarget;
    float                       m_fRoll;
    float                       m_fFOV;
    CMatrix                     m_matFixedMatrix;

    CCamera*                    m_pCamera;
};

#endif
