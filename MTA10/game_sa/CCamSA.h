/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CCamSA.h
*  PURPOSE:     Header file for camera entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_CAM
#define __CGAMESA_CAM

#include <game/CCam.h>
#include "CEntitySA.h"
#include "CAutomobileSA.h"
#include "CPedSA.h"

const int NUMBER_OF_VECTORS_FOR_AVERAGE=2;
#define CAM_NUM_TARGET_HISTORY          4

class CCamSAInterface
{
public:

    bool    bBelowMinDist; //used for follow ped mode
    bool    bBehindPlayerDesired; //used for follow ped mode
    bool    m_bCamLookingAtVector;
    bool    m_bCollisionChecksOn;
    bool    m_bFixingBeta; //used for camera on a string // 4
    bool    m_bTheHeightFixerVehicleIsATrain;
    bool    LookBehindCamWasInFront;
    bool    LookingBehind;
    bool    LookingLeft; // 8
    bool    LookingRight;
    bool    ResetStatics; //for interpolation type stuff to work
    bool    Rotating;

    short   Mode;                   // CameraMode  // 12
    unsigned int  m_uiFinishTime; // 14
    
    int     m_iDoCollisionChecksOnFrameNum; // 18
    int     m_iDoCollisionCheckEveryNumOfFrames;
    int     m_iFrameNumWereAt;
    int     m_iRunningVectorArrayPos; // 32
    int     m_iRunningVectorCounter;
    int     DirectionWasLooking; // 40
    
    float   f_max_role_angle; //=DEGTORAD(5.0f);    // 44
    float   f_Roll; //used for adding a slight roll to the camera in the
    float   f_rollSpeed; //camera on a string mode
    float   m_fSyphonModeTargetZOffSet; // 56
    float   m_fAmountFractionObscured;
    float   m_fAlphaSpeedOverOneFrame; // 64
    float   m_fBetaSpeedOverOneFrame;
    float   m_fBufferedTargetBeta; // 72
    float   m_fBufferedTargetOrientation;
    float   m_fBufferedTargetOrientationSpeed; // 80
    float   m_fCamBufferedHeight;
    float   m_fCamBufferedHeightSpeed;
    float   m_fCloseInPedHeightOffset; // 92
    float   m_fCloseInPedHeightOffsetSpeed;
    float   m_fCloseInCarHeightOffset;
    float   m_fCloseInCarHeightOffsetSpeed; // 104
    float   m_fDimensionOfHighestNearCar;
    float   m_fDistanceBeforeChanges; // 112
    float   m_fFovSpeedOverOneFrame;
    float   m_fMinDistAwayFromCamWhenInterPolating; // 120
    float   m_fPedBetweenCameraHeightOffset;
    float   m_fPlayerInFrontSyphonAngleOffSet; // 128
    float   m_fRadiusForDead;
    float   m_fRealGroundDist; //used for follow ped mode // 136
    float   m_fTargetBeta;
    float   m_fTimeElapsedFloat;
    float   m_fTilt; // 148
    float   m_fTiltSpeed; // 152

    float   m_fTransitionBeta; // 156
    float   m_fTrueBeta; // 160
    float   m_fTrueAlpha; // 164
    float   m_fInitialPlayerOrientation; //used for first person // 168

    float   m_fVerticalAngle; // alpha // 172
    float   AlphaSpeed; // 176
    float   FOV; // 180
    float   FOVSpeed; // 184
    float   m_fHorizontalAngle; // beta // 188
    float   BetaSpeed; // 192
    float   Distance; // 196
    float   DistanceSpeed;
    float   CA_MIN_DISTANCE; // 204
    float   CA_MAX_DISTANCE;
    float   SpeedVar;
    float   m_fCameraHeightMultiplier; //used by TwoPlayer_Separate_Cars_TopDown // 216
    
    // ped onfoot zoom distance
    float m_fTargetZoomGroundOne; // 220
    float m_fTargetZoomGroundTwo;
    float m_fTargetZoomGroundThree; // 228
    // ped onfoot alpha angle offset
    float m_fTargetZoomOneZExtra;
    float m_fTargetZoomTwoZExtra;
    float m_fTargetZoomTwoInteriorZExtra; //extra one for interior
    float m_fTargetZoomThreeZExtra; // 244
    
    float m_fTargetZoomZCloseIn; // 248
    float m_fMinRealGroundDist;
    float m_fTargetCloseInDist;

    // For targetting in cooperative mode.
    float   Beta_Targeting; // 260
    float   X_Targetting, Y_Targetting;
    int CarWeAreFocussingOn; //which car is closer to the camera in 2 player cooperative mode with separate cars. // 272
    float   CarWeAreFocussingOnI; //interpolated version
    
    float m_fCamBumpedHorz; // 280
    float m_fCamBumpedVert;
    int m_nCamBumpedTime; // 288
    static int CAM_BUMPED_SWING_PERIOD;
    static int CAM_BUMPED_END_TIME;
    static float CAM_BUMPED_DAMP_RATE;
    static float CAM_BUMPED_MOVE_MULT;

    CVector m_cvecSourceSpeedOverOneFrame; // 292
    CVector m_cvecTargetSpeedOverOneFrame; // 304
    CVector m_cvecUpOverOneFrame; // 316
    
    CVector m_cvecTargetCoorsForFudgeInter; // 328
    CVector m_cvecCamFixedModeVector; // 340
    CVector m_cvecCamFixedModeSource; // 352
    CVector m_cvecCamFixedModeUpOffSet; // 364
    CVector m_vecLastAboveWaterCamPosition; // 376  //helper for when the player has gone under the water

    CVector m_vecBufferedPlayerBodyOffset; // 388

    // The three vectors that determine this camera for this frame
    CVector Front;  // 400                                          // Direction of looking in
    CVector Source;                                                 // Coors in world space
    CVector SourceBeforeLookBehind; // 424
    CVector Up;                                                     // Just that
    CVector m_arrPreviousVectors[NUMBER_OF_VECTORS_FOR_AVERAGE];    // used to average stuff // 448

    CVector m_aTargetHistoryPos[CAM_NUM_TARGET_HISTORY]; // 472
    DWORD m_nTargetHistoryTime[CAM_NUM_TARGET_HISTORY]; // 520
    DWORD m_nCurrentHistoryPoints; // 536

    CEntitySAInterface *CamTargetEntity;
public:
    float       m_fCameraDistance; // 544
    float       m_fIdealAlpha;
    float       m_fPlayerVelocity; // 552
    //CVector TempRight;
    CAutomobileSAInterface  *m_pLastCarEntered; // So interpolation works
    CPedSAInterface         *m_pLastPedLookedAt;// So interpolation works 
    bool        m_bFirstPersonRunAboutActive; // 564
};
C_ASSERT(sizeof(CCamSAInterface) == 0x238);

class CCamSA : public CCam
{
private:
    CCamSAInterface*    m_pInterface;

public:
                        CCamSA                      ( CCamSAInterface* pInterface )     { m_pInterface = pInterface; }
    CCamSAInterface*    GetInterface                ()              { return m_pInterface; }

    CVector*            GetFront                    () const        { return &m_pInterface->Front; }
    CVector*            GetUp                       () const        { return &m_pInterface->Up; }
    CVector*            GetSource                   () const        { return &m_pInterface->Source; }
    unsigned int        GetMode                     () const        { return m_pInterface->Mode; }
    float               GetFOV                      () const        { return m_pInterface->FOV; }
    void                SetFOV                      ( float fFOV )  { m_pInterface->FOV = fFOV; }
    void                SetDirection                ( float fHorizontal, float fVertical );

    CVector*            GetFixedModeSource          () const        { return &m_pInterface->m_cvecCamFixedModeSource; }
    CVector*            GetFixedModeVector          () const        { return &m_pInterface->m_cvecCamFixedModeVector; }
    CVector*            GetTargetHistoryPos         () const        { return m_pInterface->m_aTargetHistoryPos; }

    CEntity*            GetTargetEntity             () const;
};

#endif
