/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CCamSA.h
*  PURPOSE:		Header file for camera entity class
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
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
#define CAM_NUM_TARGET_HISTORY			4

class CCamSAInterface // 568 bytes?
{
public:

	bool	bBelowMinDist; //used for follow ped mode
	bool	bBehindPlayerDesired; //used for follow ped mode
	bool 	m_bCamLookingAtVector;
	bool 	m_bCollisionChecksOn;
	bool	m_bFixingBeta; //used for camera on a string
	bool 	m_bTheHeightFixerVehicleIsATrain;
	bool 	LookBehindCamWasInFront;
	bool 	LookingBehind;
	bool 	LookingLeft; // 32
	bool 	LookingRight;
	bool 	ResetStatics; //for interpolation type stuff to work
	bool 	Rotating;

	short	Mode;					// CameraMode
	unsigned int  m_uiFinishTime; // 52
	
	int 	m_iDoCollisionChecksOnFrameNum; 
	int 	m_iDoCollisionCheckEveryNumOfFrames;
	int 	m_iFrameNumWereAt;	// 64
	int 	m_iRunningVectorArrayPos;
	int 	m_iRunningVectorCounter;
	int 	DirectionWasLooking;
	
	float 	f_max_role_angle; //=DEGTORAD(5.0f);	
	float 	f_Roll; //used for adding a slight roll to the camera in the
	float 	f_rollSpeed; //camera on a string mode
	float 	m_fSyphonModeTargetZOffSet;
	float 	m_fAmountFractionObscured;
	float 	m_fAlphaSpeedOverOneFrame; // 100
	float 	m_fBetaSpeedOverOneFrame;
	float 	m_fBufferedTargetBeta;
	float 	m_fBufferedTargetOrientation;
	float	m_fBufferedTargetOrientationSpeed;
	float 	m_fCamBufferedHeight;
	float 	m_fCamBufferedHeightSpeed;
	float 	m_fCloseInPedHeightOffset;
	float 	m_fCloseInPedHeightOffsetSpeed; // 132
	float 	m_fCloseInCarHeightOffset;
	float 	m_fCloseInCarHeightOffsetSpeed;
	float 	m_fDimensionOfHighestNearCar;		
	float	m_fDistanceBeforeChanges;
	float 	m_fFovSpeedOverOneFrame;
	float 	m_fMinDistAwayFromCamWhenInterPolating;
	float 	m_fPedBetweenCameraHeightOffset;
	float 	m_fPlayerInFrontSyphonAngleOffSet; // 164
	float	m_fRadiusForDead;
	float	m_fRealGroundDist; //used for follow ped mode
	float 	m_fTargetBeta;
	float 	m_fTimeElapsedFloat;	
	float 	m_fTilt;
	float 	m_fTiltSpeed;

	float   m_fTransitionBeta;
	float 	m_fTrueBeta;
	float 	m_fTrueAlpha; // 200
	float	m_fInitialPlayerOrientation; //used for first person

	float	Alpha;
	float	AlphaSpeed;
	float	FOV;
    float	FOVSpeed;
	float	Beta;
	float	BetaSpeed;
	float	Distance; // 232
	float	DistanceSpeed;
	float 	CA_MIN_DISTANCE;
	float 	CA_MAX_DISTANCE;
	float	SpeedVar;
	float	m_fCameraHeightMultiplier; //used by TwoPlayer_Separate_Cars_TopDown
	
	// ped onfoot zoom distance
	float m_fTargetZoomGroundOne;
	float m_fTargetZoomGroundTwo; // 256
	float m_fTargetZoomGroundThree;
	// ped onfoot alpha angle offset
	float m_fTargetZoomOneZExtra;
	float m_fTargetZoomTwoZExtra;
	float m_fTargetZoomTwoInteriorZExtra; //extra one for interior
	float m_fTargetZoomThreeZExtra;
	
	float m_fTargetZoomZCloseIn;
	float m_fMinRealGroundDist;
	float m_fTargetCloseInDist;

	// For targetting in cooperative mode.
	float	Beta_Targeting; // 292
	float	X_Targetting, Y_Targetting;
	int	CarWeAreFocussingOn; //which car is closer to the camera in 2 player cooperative mode with separate cars.
	float	CarWeAreFocussingOnI; //interpolated version
	
	float m_fCamBumpedHorz; // 312
	float m_fCamBumpedVert;
	int	m_nCamBumpedTime; // 320
	static int CAM_BUMPED_SWING_PERIOD;
	static int CAM_BUMPED_END_TIME;
	static float CAM_BUMPED_DAMP_RATE;
	static float CAM_BUMPED_MOVE_MULT;

	CVector m_cvecSourceSpeedOverOneFrame; // 324
	CVector m_cvecTargetSpeedOverOneFrame; // 336
	CVector m_cvecUpOverOneFrame; // 348
	
	CVector m_cvecTargetCoorsForFudgeInter; // 360
	CVector m_cvecCamFixedModeVector; // 372
	CVector m_cvecCamFixedModeSource; // 384
  	CVector m_cvecCamFixedModeUpOffSet; // 396
	CVector m_vecLastAboveWaterCamPosition; //408  //helper for when the player has gone under the water

	CVector m_vecBufferedPlayerBodyOffset; // 420

	// The three vectors that determine this camera for this frame
	CVector	Front;	// 432												// Direction of looking in
	CVector	Source;													// Coors in world space
	CVector	SourceBeforeLookBehind;
	CVector	Up;														// Just that
	CVector	m_arrPreviousVectors[NUMBER_OF_VECTORS_FOR_AVERAGE];	// used to average stuff

	CVector m_aTargetHistoryPos[CAM_NUM_TARGET_HISTORY];
	DWORD m_nTargetHistoryTime[CAM_NUM_TARGET_HISTORY];
	DWORD m_nCurrentHistoryPoints;

	CEntitySAInterface *CamTargetEntity;
protected:
	float 		m_fCameraDistance;
	float 		m_fIdealAlpha;
	float 		m_fPlayerVelocity;
	//CVector TempRight;
	CAutomobileSAInterface	*m_pLastCarEntered; // So interpolation works
	CPedSAInterface			*m_pLastPedLookedAt;// So interpolation works 
	bool		m_bFirstPersonRunAboutActive;
};

class CCamSA : public CCam
{
private:
	CCamSAInterface			* internalInterface;
public:
	CCamSA(CCamSAInterface	* camInterface)     { this->internalInterface = camInterface; }
	CCamSAInterface			* GetInterface()    { return this->internalInterface;};
	CVector				* GetFront();
	CVector				* GetUp();
	CVector				* GetSource();
    unsigned int        GetMode()               { return this->internalInterface->Mode; }

	CVector				* GetFixedModeSource();
	CVector				* GetFixedModeVector();

    CEntity *           GetTargetEntity();

    void                AdjustToNewGravity ( const CVector* pvecOldGravity, const CVector* pvecNewGravity );
};

#endif