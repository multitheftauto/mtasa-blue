/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CCameraSA.h
*  PURPOSE:		Header file for camera rendering class
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_CAMERA
#define __CGAMESA_CAMERA

#include <game/CCamera.h>
#include "CCamSA.h"
#include "CGarageSA.h"
#include "CEntitySA.h"
#include "COffsets.h"

#include <CMatrix_Pad.h>

#define FUNC_TakeControl					0x50C7C0 // ##SA##
#define FUNC_TakeControlNoEntity			0x50C8B0
#define FUNC_TakeControlAttachToEntity		0x50C910
#define FUNC_Restore						0x50B930
#define FUNC_SetCamPositionForFixedMode		0x50BEC0
#define FUNC_ConeCastCollisionResolve		0x51A5D0

#define FUNC_Find3rdPersonCamTargetVector	0x514970 //##SA##
#define FUNC_Find3rdPersonQuickAimPitch     0x50AD40
#define FUNC_TryToStartNewCamMode			0x467AAD
#define FUNC_VectorTrackLinear				0x50D1D0
#define FUNC_GetFadingDirection				0x50ADF0
#define FUNC_GetFading						0x50ADE0
#define FUNC_Fade							0x50AC20
#define FUNC_SetFadeColour					0x50BF00
	
#define VAR_ActiveCam						0x7E46FE
#define VAR_CameraRotation                  0xB6F178 // used for controling where the player faces


#define MAX_CAMS							3 

#define MAX_NUM_OF_NODES					800 // for trains

/*** R* Defines ***/
#define NUMBER_OF_VECTORS_FOR_AVERAGE 2
#define MAX_NUM_OF_SPLINETYPES (4)
/*** END R* Defines ***/

/*** BEGIN PURE R* CLASSES ***/

class CCamPathSplines
{
	public: 
	enum {MAXPATHLENGTH=800};
	FLOAT *m_arr_PathData;//	FLOAT m_arr_PathData[MAXPATHLENGTH];	
};

class CQueuedMode
{
public:
	short		Mode;					// CameraMode
	FLOAT		Duration;				// How long for (<0.0f -> indefinately)
	short		MinZoom, MaxZoom;		// How far is player allowed to zoom in & out
};

class CTrainCamNode
{
	public:
	CVector m_cvecCamPosition;
	CVector m_cvecPointToLookAt;
	CVector m_cvecMinPointInRange;//this is the minimum required distance the train has to be to the camera to 
	//allow a switch to the node cam 
	CVector m_cvecMaxPointInRange;//this is the minimum required distance the train has to be to from the camera 
	FLOAT m_fDesiredFOV;
	FLOAT m_fNearClip;
	//to switch from the  the node cam
};

/*** END PURE R* CLASSES ***/

class CCameraSAInterface 
{
public:
	// CPlaceable
	CPlaceableSAInterface	Placeable;
	// End CPlaceable

	//move these out the class, have decided to set up a mirrored enumerated type thingy at the top
	bool 	m_bAboveGroundTrainNodesLoaded;
	bool 	m_bBelowGroundTrainNodesLoaded;
	bool 	m_bCamDirectlyBehind;	
	bool 	m_bCamDirectlyInFront;	
  	bool	m_bCameraJustRestored;
	bool 	m_bcutsceneFinished;
	bool 	m_bCullZoneChecksOn;
	bool 	m_bFirstPersonBeingUsed; // To indicate if the m_bFirstPersonBeingUsed viewer is being used.
	bool	m_bJustJumpedOutOf1stPersonBecauseOfTarget;
	bool 	m_bIdleOn;
	bool  	m_bInATunnelAndABigVehicle;
	bool 	m_bInitialNodeFound;
    bool 	m_bInitialNoNodeStaticsSet;
	bool   	m_bIgnoreFadingStuffForMusic;
	bool  	m_bPlayerIsInGarage;
	bool	m_bPlayerWasOnBike;
	bool  	m_bJustCameOutOfGarage;
	bool 	m_bJustInitalised;//Just so the speed thingy doesn't go mad right at the start
    unsigned char 	m_bJust_Switched;//Variable to indicate that we have jumped somewhere, Raymond needs this for the audio engine
	bool 	m_bLookingAtPlayer;
	bool 	m_bLookingAtVector;
	bool 	m_bMoveCamToAvoidGeom;
	bool 	m_bObbeCinematicPedCamOn;
	bool 	m_bObbeCinematicCarCamOn;
	bool 	m_bRestoreByJumpCut;
	bool   	m_bUseNearClipScript;
	bool 	m_bStartInterScript;
	unsigned char 	m_bStartingSpline;
	bool 	m_bTargetJustBeenOnTrain; //this variable is needed to be able to restore the camera
	bool 	m_bTargetJustCameOffTrain;
    bool 	m_bUseSpecialFovTrain;
	bool    m_bUseTransitionBeta;
	bool 	m_bUseScriptZoomValuePed;
	bool 	m_bUseScriptZoomValueCar;
	bool 	m_bWaitForInterpolToFinish;
	bool	m_bItsOkToLookJustAtThePlayer; //Used when interpolating
	bool 	m_bWantsToSwitchWidescreenOff;	
	bool 	m_WideScreenOn;	
	bool	m_1rstPersonRunCloseToAWall;
	bool	m_bHeadBob;
	bool 	m_bVehicleSuspenHigh;
	bool 	m_bEnable1rstPersonCamCntrlsScript; 
	bool 	m_bAllow1rstPersonWeaponsCamera;
	bool	m_bCooperativeCamMode;
	bool	m_bAllowShootingWith2PlayersInCar;
	bool	m_bDisableFirstPersonInCar;
	static bool	m_bUseMouse3rdPerson;
#ifndef FINALBUILD	
	bool 	bStaticFrustum;
#endif	

	// for debug keyboard stuff
#ifndef MASTER
	unsigned char display_kbd_debug;
	float kbd_fov_value;
#endif // MASTER

		// The following fields allow the level designers to specify the camera for 2 player games.
	short	m_ModeForTwoPlayersSeparateCars;
	short	m_ModeForTwoPlayersSameCarShootingAllowed;
	short	m_ModeForTwoPlayersSameCarShootingNotAllowed;
	short	m_ModeForTwoPlayersNotBothInCar;

	bool 	m_bGarageFixedCamPositionSet;
    bool 	m_vecDoingSpecialInterPolation;
	bool 	m_bScriptParametersSetForInterPol;

	
	bool 	m_bFading;//to indicate that we are fading 
	bool 	m_bMusicFading;
	bool 	m_bMusicFadedOut;

	bool 	m_bFailedCullZoneTestPreviously;
	bool 	m_FadeTargetIsSplashScreen;//used as hack for fading 
	bool 	WorldViewerBeingUsed; // To indicate if the world viewer is being used.										 

	
	unsigned char	m_uiTransitionJUSTStarted;	// This is the first frame of a transition.
	unsigned char	m_uiTransitionState;		// 0:one mode 1:transition
	unsigned char	ActiveCam;				// Which one at the moment (0 or 1)
										// Their is a fudge at the end when the renderware matrix will receive either
										// the active camera or the worldviewer camera
	unsigned int	m_uiCamShakeStart;			// When did the camera shake start.
	unsigned int 	m_uiFirstPersonCamLastInputTime;
 	unsigned int 	m_uiLongestTimeInMill;
  	unsigned int 	m_uiNumberOfTrainCamNodes;
    unsigned int 	m_uiTimeLastChange;
	unsigned int 	m_uiTimeWeLeftIdle_StillNoInput;
	unsigned int  m_uiTimeWeEnteredIdle;
	unsigned int	m_uiTimeTransitionStart;	// When was the transition started ?
	unsigned int	m_uiTransitionDuration;		// How long does the transition take ?
	unsigned int	m_uiTransitionDurationTargetCoors;
	int 	m_BlurBlue;
	int 	m_BlurGreen;
	int 	m_BlurRed;
	int 	m_BlurType;
	int 	m_iWorkOutSpeedThisNumFrames;//duh	
	int 	m_iNumFramesSoFar; //counter
	int 	m_iCurrentTrainCamNode;//variable indicating which camera node we are at for the train
  	int 	m_motionBlur;//to indicate that we are fading

	int 	m_imotionBlurAddAlpha;
	int 	m_iCheckCullZoneThisNumFrames; 
	int 	m_iZoneCullFrameNumWereAt;
	int 	WhoIsInControlOfTheCamera; //to discern between obbe and scripts


//	float	CarZoomIndicator;
//	float 	CarZoomValue;
//	float	CarZoomValueSmooth;
//	float 	m_fCarZoomValueScript;
//	float 	PedZoomIndicator;
//	float	m_fPedZoomValue;
//	float	m_fPedZoomValueSmooth;
//	float 	m_fPedZoomValueScript;
	int	m_nCarZoom; 				// store zoom index
	float 	m_fCarZoomBase;				// store base zoom distance from index
	float	m_fCarZoomTotal;			// store total zoom after modded by camera modes
	float	m_fCarZoomSmoothed;			// buffered version of the var above
	float 	m_fCarZoomValueScript;
	int 	m_nPedZoom;					// store zoom index
	float	m_fPedZoomBase;				// store base zoom distance from index
	float	m_fPedZoomTotal;			// store total zoom after modded by camera modes
	float	m_fPedZoomSmoothed;			// buffered version of the var above
	float 	m_fPedZoomValueScript;


	float	CamFrontXNorm, CamFrontYNorm;
	float	DistanceToWater;
	float	HeightOfNearestWater;
	float 	FOVDuringInter;
	float	LODDistMultiplier;	// This takes into account the FOV and the standard LOD multiplier Smaller aperture->bigger LOD multipliers.
	float 	GenerationDistMultiplier;	// This takes into account the FOV but noy the standard LOD multiplier

	float 	m_fAlphaSpeedAtStartInter;
  	float	m_fAlphaWhenInterPol;
	float 	m_fAlphaDuringInterPol;
	float   m_fBetaDuringInterPol;
	float 	m_fBetaSpeedAtStartInter;
  	float 	m_fBetaWhenInterPol;
	float 	m_fFOVWhenInterPol;
	float 	m_fFOVSpeedAtStartInter;
	float 	m_fStartingBetaForInterPol;
	float 	m_fStartingAlphaForInterPol;
  	float   m_PedOrientForBehindOrInFront;

	float 	m_CameraAverageSpeed; //this is an average depending on how many frames we work it out
	float 	m_CameraSpeedSoFar; //this is a running total
	float	m_fCamShakeForce;			// How severe is the camera shake.
    float 	m_fFovForTrain;
	float 	m_fFOV_Wide_Screen;

	float  	m_fNearClipScript;
	float	m_fOldBetaDiff;			// Needed for interpolation between 2 modes
	float 	m_fPositionAlongSpline;//Variable used to indicate how far along the spline we are 0-1 for started to completed respectively
	float 	m_ScreenReductionPercentage;
	float 	m_ScreenReductionSpeed;
	float 	m_AlphaForPlayerAnim1rstPerson;
	
	float	Orientation;			// The orientation of the camera. Used for peds walking.
	float	PlayerExhaustion;		// How tired is player (inaccurate sniping) 0.0f-1.0f
					// The following things are used by the sound code to
					// play reverb depending on the surroundings. From a point
					// in front of the camera the disance is measured to the
					// nearest obstacle (building)
	float	SoundDistUp; //, SoundDistLeft, SoundDistRight;		// These ones are buffered and should be used by the audio
	float	SoundDistUpAsRead; //, SoundDistLeftAsRead, SoundDistRightAsRead;
	float	SoundDistUpAsReadOld; //, SoundDistLeftAsReadOld, SoundDistRightAsReadOld;
					// Very rough distance to the nearest water for the sound to use
					// Front vector X&Y normalised to 1. Used by loads of stuff.

	
	float	m_fAvoidTheGeometryProbsTimer;
	short	m_nAvoidTheGeometryProbsDirn;
	
	float 	m_fWideScreenReductionAmount;//0 for not reduced 1 for fully reduced (Variable for Les)
	float   m_fStartingFOVForInterPol;
	
		// These ones are static so that they don't get cleared in CCamera::Init()
	static	float m_fMouseAccelHorzntl;// acceleration multiplier for 1st person controls
	static	float m_fMouseAccelVertical;// acceleration multiplier for 1st person controls
	static	float m_f3rdPersonCHairMultX;
	static	float m_f3rdPersonCHairMultY;

	CCamSAInterface	Cams[3];				// The actual cameras (usually only one of the two is active)
							// And to complicate this we have a third camera, this camera is 
							// used for debugging when we want to have a look at the world.
							// We can't change the camera mode because other objects depend on their

	// #########################################//
	// DATA NOT UPDATED FOR SA  BELOW HERE!!!!! //
	// #########################################//

	CGarageSAInterface *pToGarageWeAreIn;
	CGarageSAInterface *pToGarageWeAreInForHackAvoidFirstPerson;
	CQueuedMode m_PlayerMode;
	// The higher priority player camera mode. This one is used
	// for the sniper mode and rocket launcher mode.
	// This one overwrites the m_PlayerMode above.
	CQueuedMode PlayerWeaponMode;
	CVector m_PreviousCameraPosition; //needed to work out speed
	CVector m_RealPreviousCameraPosition; // This cane be used by stuff outside the camera code. The one above is the same as the current coordinates outwidth the camera code.
										// an active camera for range finding etc
	CVector m_cvecAimingTargetCoors;		// Coors to look at with Gordons aiming thing
	// The player camera that is waiting to be used
	// This camera can replace the default camera where this is
	// needed (in tricky situations like tunnels for instance)
	CVector m_vecFixedModeVector;
	CVector m_vecFixedModeSource;
	CVector m_vecFixedModeUpOffSet;
	CVector m_vecCutSceneOffset;
	CVector m_cvecStartingSourceForInterPol;
	CVector m_cvecStartingTargetForInterPol;
	CVector m_cvecStartingUpForInterPol;
	CVector m_cvecSourceSpeedAtStartInter;
	CVector m_cvecTargetSpeedAtStartInter;
	CVector m_cvecUpSpeedAtStartInter;
	CVector m_vecSourceWhenInterPol;
	CVector m_vecTargetWhenInterPol;
	CVector m_vecUpWhenInterPol;
	CVector m_vecClearGeometryVec;
	CVector m_vecGameCamPos;
	CVector SourceDuringInter, TargetDuringInter, UpDuringInter;


	CVector m_vecAttachedCamOffset; // for attaching the camera to a ped or vehicle (set by level designers for use in cutscenes)
	CVector m_vecAttachedCamLookAt;	
	FLOAT m_fAttachedCamAngle; // for giving the attached camera a tilt.

	// RenderWare camera pointer
	DWORD * m_pRwCamera; // was RwCamera *
	///stuff for cut scenes
	CEntitySAInterface *pTargetEntity;
	CEntitySAInterface *pAttachedEntity;
	//CVector CutScene; 
	CCamPathSplines m_arrPathArray[MAX_NUM_OF_SPLINETYPES]; //These only get created when the script calls the load splines function
	// maybe this shouldn't be here depends if GTA_TRAIN is defined (its not)
	//CTrainCamNode 	m_arrTrainCamNode[MAX_NUM_OF_NODES];

	bool m_bMirrorActive;
	bool m_bResetOldMatrix;

//	protected:
	CMatrix_Padded m_cameraMatrix;
	CMatrix_Padded m_cameraMatrixOld;
	CMatrix_Padded m_viewMatrix;
	CMatrix_Padded m_matInverse;
	CMatrix_Padded m_matMirrorInverse;
	CMatrix_Padded m_matMirror;

	CVector m_vecFrustumNormals[4];
	CVector m_vecFrustumWorldNormals[4];
	CVector m_vecFrustumWorldNormals_Mirror[4];

	FLOAT m_fFrustumPlaneOffsets[4];
	FLOAT m_fFrustumPlaneOffsets_Mirror[4];

	CVector m_vecRightFrustumNormal;
	CVector m_vecBottomFrustumNormal;
	CVector m_vecTopFrustumNormal;

	CVector m_vecOldSourceForInter;
	CVector m_vecOldFrontForInter;
	CVector m_vecOldUpForInter;
	FLOAT 	m_vecOldFOVForInter;
	FLOAT 	m_fFLOATingFade;//variable representing the FLOAT version of CDraw::Fade. Necessary to stop loss of precision
	FLOAT 	m_fFLOATingFadeMusic;
	FLOAT 	m_fTimeToFadeOut;
	FLOAT 	m_fTimeToFadeMusic;
	FLOAT	m_fTimeToWaitToFadeMusic;
	FLOAT   m_fFractionInterToStopMoving; 
	FLOAT 	m_fFractionInterToStopCatchUp;
	FLOAT   m_fFractionInterToStopMovingTarget; 
	FLOAT 	m_fFractionInterToStopCatchUpTarget;

	FLOAT 	m_fGaitSwayBuffer;
	FLOAT   m_fScriptPercentageInterToStopMoving;
	FLOAT   m_fScriptPercentageInterToCatchUp;
	DWORD	m_fScriptTimeForInterPolation;


	short 	m_iFadingDirection;
	int 	m_iModeObbeCamIsInForCar;
	short 	m_iModeToGoTo;
	short 	m_iMusicFadingDirection;
	short 	m_iTypeOfSwitch;

	DWORD 	m_uiFadeTimeStarted;
	DWORD 	m_uiFadeTimeStartedMusic;
};


class CCameraSA : public CCamera
{
    friend COffsets;
private:
	CCameraSAInterface			* internalInterface;
	CCamSA						* Cams[MAX_CAMS];
public:
								CCameraSA(CCameraSAInterface * cameraInterface);
                                ~CCameraSA ( void );

	CCameraSAInterface			* GetInterface() { return internalInterface; };
	VOID						TakeControl(CEntity * entity, eCamMode CamMode, int CamSwitchStyle);
	VOID						TakeControl(CVector * position, int CamSwitchStyle);
	VOID						TakeControlAttachToEntity(CEntity * TargetEntity, CEntity * AttachEntity, 
														  CVector * vecOffset, CVector * vecLookAt, 
														  float fTilt, int CamSwitchStyle);
	VOID						Restore();
    VOID						RestoreWithJumpCut();
	CMatrix						* GetMatrix ( CMatrix * matrix );
	VOID						SetMatrix ( CMatrix * matrix );
	VOID						SetCamPositionForFixedMode ( CVector * vecPosition, CVector * vecUpOffset );
	VOID						Find3rdPersonCamTargetVector ( FLOAT fDistance, CVector * vecGunMuzzle, CVector * vecSource, CVector * vecTarget );
	float                       Find3rdPersonQuickAimPitch ( void );
    BYTE						GetActiveCam();

	CCam						* GetCam(BYTE bCameraID);
    virtual CCam                * GetCam ( CCamSAInterface* camInterface );

	VOID						SetWidescreen(BOOL bWidescreen);
	BOOL						GetWidescreen();
	FLOAT						GetCarZoom();
	VOID						SetCarZoom(FLOAT fCarZoom);
	bool						TryToStartNewCamMode(DWORD dwCamMode);
	bool						ConeCastCollisionResolve(CVector *pPos, CVector *pLookAt, CVector *pDest, float rad, float minDist, float *pDist);
	void						VectorTrackLinear ( CVector * pTo, CVector * pFrom, float time, bool bSmoothEnds );
	bool						IsFading ( void );
	int							GetFadingDirection ( void );
	void						Fade ( float fFadeOutTime, int iOutOrIn );
	void						SetFadeColor ( unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue );
    float                       GetCameraRotation ( void );
    RwMatrix *                  GetLTM ( void );
    CEntity *                   GetTargetEntity ( void );

private:
    static unsigned long        FUNC_RwFrameGetLTM;
};

#endif
