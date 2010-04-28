/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CCamera.h
*  PURPOSE:     Camera scene rendering interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_CAMERA
#define __CGAME_CAMERA

#include "CEntity.h"
#include "CCam.h"


enum eCamMode 
{ 
    MODE_NONE = 0, 
    MODE_TOPDOWN, //=1, 
    MODE_GTACLASSIC,// =2, 
    MODE_BEHINDCAR, //=3, 
    MODE_FOLLOWPED, //=4, 
    MODE_AIMING, //=5, 
    MODE_DEBUG, //=6, 
    MODE_SNIPER, //=7, 
    MODE_ROCKETLAUNCHER, //=8,   
    MODE_MODELVIEW, //=9, 
    MODE_BILL, //=10, 
    MODE_SYPHON, //=11, 
    MODE_CIRCLE, //=12, 
    MODE_CHEESYZOOM, //=13, 
    MODE_WHEELCAM, //=14, 
    MODE_FIXED, //=15, 
    MODE_1STPERSON, //=16, 
    MODE_FLYBY, //=17, 
    MODE_CAM_ON_A_STRING, //=18,  
    MODE_REACTION, //=19, 
    MODE_FOLLOW_PED_WITH_BIND, //=20, 
    MODE_CHRIS, //21
    MODE_BEHINDBOAT, //=22, 
    MODE_PLAYER_FALLEN_WATER, //=23, 
    MODE_CAM_ON_TRAIN_ROOF, //=24, 
    MODE_CAM_RUNNING_SIDE_TRAIN, //=25,  
    MODE_BLOOD_ON_THE_TRACKS, //=26, 
    MODE_IM_THE_PASSENGER_WOOWOO, //=27, 
    MODE_SYPHON_CRIM_IN_FRONT,// =28, 
    MODE_PED_DEAD_BABY, //=29, 
    MODE_PILLOWS_PAPS, //=30, 
    MODE_LOOK_AT_CARS, //=31, 
    MODE_ARRESTCAM_ONE, //=32, 
    MODE_ARRESTCAM_TWO, //=33, 
    MODE_M16_1STPERSON, //=34, 
    MODE_SPECIAL_FIXED_FOR_SYPHON, //=35, 
    MODE_FIGHT_CAM, //=36, 
    MODE_TOP_DOWN_PED, //=37,
    MODE_LIGHTHOUSE, //=38
    ///all 1rst person run ablout run about modes now
    MODE_SNIPER_RUNABOUT, //=39, 
    MODE_ROCKETLAUNCHER_RUNABOUT, //=40,  
    MODE_1STPERSON_RUNABOUT, //=41,
    MODE_M16_1STPERSON_RUNABOUT, //=42,
    MODE_FIGHT_CAM_RUNABOUT, //=43,
    MODE_EDITOR, //=44,
    MODE_HELICANNON_1STPERSON, //= 45
    MODE_CAMERA, //46
    MODE_ATTACHCAM,  //47
    MODE_TWOPLAYER,
    MODE_TWOPLAYER_IN_CAR_AND_SHOOTING,
    MODE_TWOPLAYER_SEPARATE_CARS,   //50
    MODE_ROCKETLAUNCHER_HS, 
    MODE_ROCKETLAUNCHER_RUNABOUT_HS,
    MODE_AIMWEAPON,
    MODE_TWOPLAYER_SEPARATE_CARS_TOPDOWN,
    MODE_AIMWEAPON_FROMCAR, //55
    MODE_DW_HELI_CHASE,
    MODE_DW_CAM_MAN,
    MODE_DW_BIRDY,
    MODE_DW_PLANE_SPOTTER,
    MODE_DW_DOG_FIGHT, //60
    MODE_DW_FISH,
    MODE_DW_PLANECAM1,
    MODE_DW_PLANECAM2,
    MODE_DW_PLANECAM3,
    MODE_AIMWEAPON_ATTACHED //65
};

enum {FADE_OUT=0, FADE_IN};

class CCamera
{
public:
    // cammode 56 = heli chase, 14 = wheel cam
    // switchstyle 1 = smooth, 2 = cut
    virtual VOID                        TakeControl(CEntity * entity, eCamMode CamMode, int CamSwitchStyle)=0;
    virtual VOID                        TakeControl(CVector * position, int CamSwitchStyle)=0;
    virtual VOID                        TakeControlAttachToEntity(CEntity * TargetEntity, CEntity * AttachEntity, 
                                                                  CVector * vecOffset, CVector * vecLookAt, 
                                                                  float fTilt, int CamSwitchStyle)=0;
    virtual VOID                        Restore()=0;
    virtual VOID                        RestoreWithJumpCut()=0;
    virtual CMatrix                     * GetMatrix ( CMatrix * matrix )=0;
    virtual VOID                        SetMatrix ( CMatrix * matrix )=0;
    virtual VOID                        SetCamPositionForFixedMode ( CVector * vecPosition, CVector * vecUpOffset )=0;
    virtual VOID                        Find3rdPersonCamTargetVector ( FLOAT fDistance, CVector * vecGunMuzzle, CVector * vecSource, CVector * vecTarget )=0;
    virtual float                       Find3rdPersonQuickAimPitch ( void ) = 0;
    virtual BYTE                        GetActiveCam()=0;
    virtual CCam                        * GetCam(BYTE bCameraID)=0;
    virtual VOID                        SetWidescreen(BOOL bWidescreen)=0;
    virtual BOOL                        GetWidescreen()=0;
    virtual float                       GetCarZoom()=0;
    virtual VOID                        SetCarZoom(float fCarZoom)=0;
    virtual bool                        TryToStartNewCamMode(DWORD dwCamMode)=0;
    virtual bool                        ConeCastCollisionResolve(CVector *pPos, CVector *pLookAt, CVector *pDest, float rad, float minDist, float *pDist)=0;
    virtual void                        VectorTrackLinear ( CVector * pTo, CVector * pFrom, float time, bool bSmoothEnds )=0;
    virtual bool                        IsFading ( void )=0;
    virtual int                         GetFadingDirection ( void )=0;
    virtual void                        Fade ( float fFadeOutTime, int iOutOrIn )=0;
    virtual void                        SetFadeColor ( unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )=0;
    virtual float                       GetCameraRotation ( void )=0;
    virtual RwMatrix *                  GetLTM ( void )=0;
    virtual CEntity *                   GetTargetEntity ( void )=0;
    virtual void                        SetCameraClip ( bool bObjects, bool bVehicles )=0;
};



#endif
