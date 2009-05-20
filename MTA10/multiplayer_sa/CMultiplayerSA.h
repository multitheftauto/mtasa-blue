/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/CMultiplayerSA.h
*  PURPOSE:     Multiplayer module class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CMULTIPLAYERSA
#define __CMULTIPLAYERSA

#include <game/CGame.h>
#include <multiplayer/CMultiplayer.h>

#include "CPopulationSA.h"
#include "multiplayersa_init.h"

#include "CRemoteDataSA.h"
class CRemoteDataSA;

class CMultiplayerSA : public CMultiplayer
{
    friend class COffsetsMP;

private:
	CRemoteDataSA				* RemoteData;
	CPopulationSA				* Population;
public:
	CMultiplayerSA();
	void                        InitHooks();
    CRemoteDataStorage *        CreateRemoteDataStorage     ();
    void                        DestroyRemoteDataStorage    ( CRemoteDataStorage* pData );
    void                        AddRemoteDataStorage        ( CPlayerPed* pPed, CRemoteDataStorage* pData );
    void                        RemoveRemoteDataStorage     ( CPlayerPed* pPed );

    CPed *                      GetContextSwitchedPed       ( void );

	CPopulationMP				* GetPopulationMP           () { return Population; }
	void						PreventLeavingVehicles      ();
	void						HideRadar                   ( bool bHide );
	void						SetCenterOfWorld            ( CEntity * entity, CVector * vecPosition, FLOAT fHeading );
	void						DisablePadHandler           ( bool bDisabled );
    void                        DisableEnterExitVehicleKey  ( bool bDisabled );
	void						DisableHeatHazeEffect       ( bool bDisable );
    void                        DisableAllVehicleWeapons    ( bool bDisable );

    bool                        GetExplosionsDisabled       ();
    void                        DisableExplosions           ( bool bDisabled );
	void						SetExplosionHandler         ( ExplosionHandler * pExplosionHandler );
    void                        SetDamageHandler            ( DamageHandler * pDamageHandler );
    void                        SetProjectileHandler        ( ProjectileHandler * pProjectileHandler );
    void                        SetProjectileStopHandler    ( ProjectileStopHandler * pProjectileHandler );
    void                        SetFireHandler              ( FireHandler * pFireHandler );
    void                        SetBreakTowLinkHandler      ( BreakTowLinkHandler * pBreakTowLinkHandler );
    void                        SetProcessCamHandler        ( ProcessCamHandler* pProcessCamHandler );
    void                        SetGameProcessHandler       ( GameProcessHandler* pProcessHandler );

    void                        AllowMouseMovement          ( bool bAllow );
    void                        DoSoundHacksOnLostFocus     ( bool bLostFocus );
    void                        DisableZoneNames            ( bool bDisabled );
    void                        DisableBirds                ( bool bDisabled );
    void                        SetSkyColor                 ( unsigned char TopRed, unsigned char TopGreen, unsigned char TopBlue, unsigned char BottomRed, unsigned char BottomGreen, unsigned char BottomBlue );
    void                        ResetSky                    ();
    void                        SetWaterColor               ( float fWaterRed, float fWaterGreen, float fWaterBlue, float fWaterAlpha );
    void                        ResetWater                  ();
    void                        DisableClouds               ( bool bDisabled );
    void                        RebuildMultiplayerPlayer    ( CPed * player );

    void                        AllowWindowsCursorShowing   ( bool bAllow );

    CShotSyncData *             GetLocalShotSyncData        ();

    void                        SetPreContextSwitchHandler  ( PreContextSwitchHandler* pHandler );
    void                        SetPostContextSwitchHandler ( PostContextSwitchHandler* pHandler );
    void                        SetPreWeaponFireHandler     ( PreWeaponFireHandler* pHandler );
    void                        SetPostWeaponFireHandler    ( PostWeaponFireHandler* pHandler );
    void                        SetDrawRadarAreasHandler    ( DrawRadarAreasHandler * pRadarAreasHandler );
    void                        SetRender3DStuffHandler     ( Render3DStuffHandler * pHandler );

    void                        Reset                       ();

    void                        ConvertEulerAnglesToMatrix  ( CMatrix& Matrix, float fX, float fY, float fZ );
    void                        ConvertMatrixToEulerAngles  ( const CMatrix& Matrix, float& fX, float& fY, float& fZ );

    float                       GetGlobalGravity            ();
    void                        SetGlobalGravity            ( float fGravity );

    float                       GetLocalPlayerGravity       ();
    void                        SetLocalPlayerGravity       ( float fGravity );

    void                        SetLocalStatValue           ( unsigned short usStat, float fValue );
    void                        SetLocalStatsStatic         ( bool bStatic );

    void                        SetLocalCameraRotation      ( float fRotation );
    bool                        IsCustomCameraRotationEnabled   ();
    void                        SetCustomCameraRotationEnabled  ( bool bEnabled );

    void                        SetDebugVars                ( float f1, float f2, float f3 );

    CVector&                    GetAkimboTarget             () { return m_vecAkimboTarget; };
    bool                        GetAkimboTargetUp           () { return m_bAkimboTargetUp; };

    void                        SetAkimboTarget             ( const CVector& vecTarget ) { m_vecAkimboTarget = vecTarget; };
    void                        SetAkimboTargetUp           ( bool bUp ) { m_bAkimboTargetUp = bUp; };

    void                        AllowCreatedObjectsInVerticalLineTest ( bool bOn );
    
    CVector                     m_vecAkimboTarget;
    bool                        m_bAkimboTargetUp;

/*	VOID						SetPlayerShotVectors(CPlayerPed* player, Vector3D * vecTarget, Vector3D * vecStart);
	VOID						SetPlayerCameraVectors(CPlayerPed* player, Vector3D * vecSource, Vector3D * vecFront);
	Vector3D					* GetLocalShotOriginVector();*/
	//Vector3D					* GetLocalTargetVector();
	//Vector3D					* GetLocalCrossProductVector();
	/*VOID						SetOnFootPlayerKeyStates(CPlayerPed * player, DWORD keys);
	DWORD						GetOnFootLocalPlayerKeyStates();
	VOID						SetInVehiclePlayerKeyStates(CPlayerPed * player, WORD keys);
	WORD						GetInVehicleLocalPlayerKeyStates();*/
//	Vector3D					* GetLocalStartVector();
//	VOID						SetPlayerStartVector(CPlayerPed* player, Vector3D * vecStart);

private:
    static unsigned long        HOOKPOS_FindPlayerCoors;
    static unsigned long        HOOKPOS_FindPlayerCentreOfWorld;
    static unsigned long        HOOKPOS_FindPlayerHeading;
    static unsigned long        HOOKPOS_CStreaming_Update_Caller;
    static unsigned long        HOOKPOS_CHud_Draw_Caller;
    static unsigned long        HOOKPOS_CRunningScript_Process;
    static unsigned long        HOOKPOS_CExplosion_AddExplosion;
    static unsigned long        HOOKPOS_CRealTimeShadowManager__ReturnRealTimeShadow;
    static unsigned long        HOOKPOS_CCustomRoadsignMgr__RenderRoadsignAtomic;
    static unsigned long        HOOKPOS_Trailer_BreakTowLink;
    static unsigned long        HOOKPOS_CRadar__DrawRadarGangOverlay;
    static unsigned long        HOOKPOS_CTaskComplexJump__CreateSubTask;
    static unsigned long        HOOKPOS_CTrain_ProcessControl_Derail;
    static unsigned long        HOOKPOS_CVehicle_SetupRender;
    static unsigned long        HOOKPOS_CVehicle_ResetAfterRender;
    static unsigned long        HOOKPOS_CObject_Render;
    static unsigned long        HOOKPOS_EndWorldColors;
    static unsigned long        HOOKPOS_CWorld_ProcessVerticalLineSectorList;

    static unsigned long        FUNC_CStreaming_Update;
    static unsigned long        FUNC_CAudioEngine__DisplayRadioStationName;
    static unsigned long        FUNC_CHud_Draw;

    static unsigned long        ADDR_CursorHiding;
    static unsigned long        ADDR_GotFocus;

    static unsigned long        FUNC_CPlayerInfoBase;
};

#endif
