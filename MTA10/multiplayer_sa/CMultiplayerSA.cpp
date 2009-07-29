/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/CMultiplayerSA.cpp
*  PURPOSE:     Multiplayer module class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               Peter <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

// These includes have to be fixed!
#include "..\game_sa\CCameraSA.h"
#include "..\game_sa\CEntitySA.h"
#include "..\game_sa\CPedSA.h"
#include "..\game_sa\common.h"

using namespace std;

extern CGame * pGameInterface;

unsigned long CMultiplayerSA::HOOKPOS_FindPlayerCoors;
unsigned long CMultiplayerSA::HOOKPOS_FindPlayerCentreOfWorld;
unsigned long CMultiplayerSA::HOOKPOS_FindPlayerHeading;
unsigned long CMultiplayerSA::HOOKPOS_CStreaming_Update_Caller;
unsigned long CMultiplayerSA::HOOKPOS_CHud_Draw_Caller;
unsigned long CMultiplayerSA::HOOKPOS_CRunningScript_Process;
unsigned long CMultiplayerSA::HOOKPOS_CExplosion_AddExplosion;
unsigned long CMultiplayerSA::HOOKPOS_CRealTimeShadowManager__ReturnRealTimeShadow;
unsigned long CMultiplayerSA::HOOKPOS_CCustomRoadsignMgr__RenderRoadsignAtomic;
unsigned long CMultiplayerSA::HOOKPOS_Trailer_BreakTowLink;
unsigned long CMultiplayerSA::HOOKPOS_CRadar__DrawRadarGangOverlay;
unsigned long CMultiplayerSA::HOOKPOS_CTaskComplexJump__CreateSubTask;
unsigned long CMultiplayerSA::HOOKPOS_CTrain_ProcessControl_Derail;
unsigned long CMultiplayerSA::HOOKPOS_CVehicle_SetupRender;
unsigned long CMultiplayerSA::HOOKPOS_CVehicle_ResetAfterRender;
unsigned long CMultiplayerSA::HOOKPOS_CObject_Render;
unsigned long CMultiplayerSA::HOOKPOS_EndWorldColors;
unsigned long CMultiplayerSA::HOOKPOS_CWorld_ProcessVerticalLineSectorList;
unsigned long CMultiplayerSA::HOOKPOS_ComputeDamageResponse_StartChoking;

unsigned long CMultiplayerSA::FUNC_CStreaming_Update;
unsigned long CMultiplayerSA::FUNC_CAudioEngine__DisplayRadioStationName;
unsigned long CMultiplayerSA::FUNC_CHud_Draw;

unsigned long CMultiplayerSA::ADDR_CursorHiding;
unsigned long CMultiplayerSA::ADDR_GotFocus;

unsigned long CMultiplayerSA::FUNC_CPlayerInfoBase;

#define HOOKPOS_FxManager_CreateFxSystem                    0x4A9BE0
#define HOOKPOS_FxManager_DestroyFxSystem                   0x4A9810

DWORD RETURN_FxManager_CreateFxSystem =                     0x4A9BE8;
DWORD RETURN_FxManager_DestroyFxSystem =                    0x4A9817;

#define HOOKPOS_CCam_ProcessFixed                           0x51D470
#define HOOKPOS_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon 0x6859a0
#define HOOKPOS_CPed_IsPlayer                               0x5DF8F0

DWORD RETURN_CCam_ProcessFixed =                            0x51D475;
DWORD RETURN_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon =  0x6859A7;
DWORD RETURN_CPed_IsPlayer =                                0x5DF8F6;

#define VAR_CollisionStreamRead_ModelInfo                   0x9689E0
#define HOOKPOS_CollisionStreamRead                         0x41B1D0
DWORD RETURN_CollisionStreamRead =                          0x41B1D6;

#define CALL_Render3DStuff                                  0x53EABF
#define FUNC_Render3DStuff                                  0x53DF40

#define CALL_CRenderer_Render                               0x53EA12
#define FUNC_CRenderer_Render                               0x727140

#define CALL_CBike_ProcessRiderAnims                        0x6BF425   // @ CBike::ProcessDrivingAnims

#define HOOKPOS_CGame_Process                               0x53C095
DWORD RETURN_CGame_Process =                                0x53C09F;

#define HOOKPOS_Idle                                        0x53E981
DWORD RETURN_Idle =                                         0x53E98B;

DWORD FUNC_CBike_ProcessRiderAnims =                        0x6B7280;
DWORD FUNC_CEntity_Render =                                 0x534310;

#define HOOKPOS_VehicleCamStart                             0x5245ED
DWORD RETURN_VehicleCamStart_success =                      0x5245F3;
DWORD RETURN_VehicleCamStart_failure =                      0x525E43;
#define HOOKPOS_VehicleCamTargetZTweak                      0x524A68
DWORD RETURN_VehicleCamTargetZTweak =                       0x524AA4;
#define HOOKPOS_VehicleCamLookDir1                          0x524DF1
DWORD RETURN_VehicleCamLookDir1 =                           0x524DF6;
#define HOOKPOS_VehicleCamLookDir2                          0x525B0E
DWORD RETURN_VehicleCamLookDir2 =                           0x525B73;
#define HOOKPOS_VehicleCamHistory                           0x525C56
DWORD RETURN_VehicleCamHistory =                            0x525D4A;
#define HOOKPOS_VehicleCamColDetect                         0x525D8D
DWORD RETURN_VehicleCamColDetect =                          0x525D92;
#define CALL_VehicleCamUp                                   0x525E1F
#define HOOKPOS_VehicleCamEnd                               0x525E3C
DWORD RETURN_VehicleCamEnd =                                0x525E42;

#define HOOKPOS_VehicleLookBehind                           0x5207E3
DWORD RETURN_VehicleLookBehind =                            0x520891;
#define CALL_VehicleLookBehindUp                            0x520E2A
#define HOOKPOS_VehicleLookAside                            0x520F70
DWORD RETURN_VehicleLookAside =                             0x520FDC;
#define CALL_VehicleLookAsideUp                             0x5211E0

#define HOOKPOS_OccupiedVehicleBurnCheck                    0x570C84
DWORD RETURN_OccupiedVehicleBurnCheck =                     0x570C8A;
#define HOOKPOS_UnoccupiedVehicleBurnCheck                  0x6A76DC
DWORD RETURN_UnoccupiedVehicleBurnCheck =                   0x6A76E4;
#define HOOKPOS_ApplyCarBlowHop                             0x6B3816
DWORD RETURN_ApplyCarBlowHop =                              0x6B3831;

#define HOOKPOS_CPhysical_ApplyGravity                      0x543081
DWORD RETURN_CPhysical_ApplyGravity =                       0x543093;

#define HOOKPOS_CWorld_SetWorldOnFire                       0x56B983
DWORD RETURN_CWorld_SetWorldOnFire =                        0x56B989;
#define HOOKPOS_CTaskSimplePlayerOnFire_ProcessPed          0x6336DA
DWORD RETURN_CTaskSimplePlayerOnFire_ProcessPed =           0x6336E0;
#define HOOKPOS_CFire_ProcessFire                           0x53AC1A
DWORD RETURN_CFire_ProcessFire =                            0x53AC1F;
#define HOOKPOS_CExplosion_Update                           0x7377D3
DWORD RETURN_CExplosion_Update =                            0x7377D8;
#define HOOKPOS_CWeapon_FireAreaEffect                      0x73EBFE
DWORD RETURN_CWeapon_FireAreaEffect =                       0x73EC03;

#define CALL_RenderScene_Plants                             0x53E103
#define HOOKPOS_RenderScene_end                             0x53E159
#define HOOKPOS_CPlantMgr_Render                            0x5DBC4C
DWORD RETURN_CPlantMgr_Render_success =                     0x5DBC52;
DWORD RETURN_CPlantMgr_Render_fail =                        0x5DBDAA;

#define HOOKPOS_CEventHandler_ComputeKnockOffBikeResponse   0x4BA06F
DWORD RETURN_CEventHandler_ComputeKnockOffBikeResponse =    0x4BA076;

#define HOOKPOS_CAnimManager_AddAnimation                   0x4d3aa0
DWORD RETURN_CAnimManager_AddAnimation =                    0x4D3AAA;
#define HOOKPOS_CAnimManager_BlendAnimation                 0x4D4610
DWORD RETURN_CAnimManager_BlendAnimation =                  0x4D4617;

#define HOOKPOS_CPed_GetWeaponSkill                         0x5e3b60
DWORD RETURN_CPed_GetWeaponSkill =                          0x5E3B68;

CPed* pContextSwitchedPed = 0;
CVector vecCenterOfWorld;
FLOAT fFalseHeading;
bool bSetCenterOfWorld;
DWORD dwVectorPointer;
bool bInStreamingUpdate;
bool bHideRadar;
bool bHasProcessedScript;
float fX, fY, fZ;
DWORD RoadSignFixTemp;
DWORD dwEAEG = 0;
bool m_bExplosionsDisabled;
float fGlobalGravity = 0.008f;
float fLocalPlayerGravity = 0.008f;
float fLocalPlayerCameraRotation = 0.0f;
bool bCustomCameraRotation = false;

bool bUsingCustomSkyGradient = false;
BYTE ucSkyGradientTopR = 0;
BYTE ucSkyGradientTopG = 0;
BYTE ucSkyGradientTopB = 0;
BYTE ucSkyGradientBottomR = 0;
BYTE ucSkyGradientBottomG = 0;
BYTE ucSkyGradientBottomB = 0;
bool bUsingCustomWaterColor = false;
float fWaterColorR = 0.0F;
float fWaterColorG = 0.0F;
float fWaterColorB = 0.0F;
float fWaterColorA = 0.0F;

CStatsData localStatsData;
bool bLocalStatsStatic = true;
extern bool bWeaponFire;

PreContextSwitchHandler* m_pPreContextSwitchHandler = NULL;
PostContextSwitchHandler* m_pPostContextSwitchHandler = NULL;
PreWeaponFireHandler* m_pPreWeaponFireHandler = NULL;
PostWeaponFireHandler* m_pPostWeaponFireHandler = NULL;
DamageHandler* m_pDamageHandler = NULL;
FireHandler* m_pFireHandler = NULL;
ProjectileHandler* m_pProjectileHandler = NULL;
ProjectileStopHandler* m_pProjectileStopHandler = NULL;
ProcessCamHandler* m_pProcessCamHandler = NULL;
ChokingHandler* m_pChokingHandler = NULL;
ExplosionHandler * m_pExplosionHandler = NULL;
BreakTowLinkHandler * m_pBreakTowLinkHandler = NULL;
DrawRadarAreasHandler * m_pDrawRadarAreasHandler = NULL;
Render3DStuffHandler * m_pRender3DStuffHandler = NULL;
PostWorldProcessHandler * m_pPostWorldProcessHandler = NULL;
IdleHandler * m_pIdleHandler = NULL;
AddAnimationHandler* m_pAddAnimationHandler = NULL;
BlendAnimationHandler* m_pBlendAnimationHandler = NULL;

CEntitySAInterface * dwSavedPlayerPointer = 0;
CEntitySAInterface * activeEntityForStreaming = 0; // the entity that the streaming system considers active

void HOOK_FindPlayerCoors();
void HOOK_FindPlayerCentreOfWorld();
void HOOK_FindPlayerHeading();
void HOOK_CStreaming_Update_Caller();
void HOOK_CHud_Draw_Caller();
void HOOK_CRunningScript_Process();
void HOOK_CExplosion_AddExplosion();
void HOOK_CRealTimeShadowManager__ReturnRealTimeShadow();
void HOOK_CCustomRoadsignMgr__RenderRoadsignAtomic();
void HOOK_Trailer_BreakTowLink();
void HOOK_CRadar__DrawRadarGangOverlay();
void HOOK_CTaskComplexJump__CreateSubTask();
void HOOK_CBike_ProcessRiderAnims();
void HOOK_FxManager_CreateFxSystem ();
void HOOK_FxManager_DestroyFxSystem ();
void HOOK_CCam_ProcessFixed ();
void HOOK_Render3DStuff ();
void HOOK_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon ();
void HOOK_CPed_IsPlayer ();
void HOOK_CTrain_ProcessControl_Derail ();
void HOOK_CVehicle_SetupRender ();
void HOOK_CVehicle_ResetAfterRender();
void HOOK_CObject_Render ();
void HOOK_EndWorldColors ();
void HOOK_CWorld_ProcessVerticalLineSectorList ();
void HOOK_ComputeDamageResponse_StartChoking ();
void HOOK_CollisionStreamRead ();
void HOOK_CPhysical_ApplyGravity ();
void HOOK_VehicleCamStart ();
void HOOK_VehicleCamTargetZTweak ();
void HOOK_VehicleCamLookDir1 ();
void HOOK_VehicleCamLookDir2 ();
void HOOK_VehicleCamHistory ();
void HOOK_VehicleCamUp ();
void HOOK_VehicleCamEnd ();
void HOOK_VehicleLookBehind ();
void HOOK_VehicleLookAside ();
void HOOK_OccupiedVehicleBurnCheck ();
void HOOK_UnoccupiedVehicleBurnCheck ();
void HOOK_ApplyCarBlowHop ();
void HOOK_CWorld_SetWorldOnFire ();
void HOOK_CTaskSimplePlayerOnFire_ProcessPed ();
void HOOK_CFire_ProcessFire ();
void HOOK_CExplosion_Update ();
void HOOK_CWeapon_FireAreaEffect ();
void HOOK_CGame_Process ();
void HOOK_Idle ();
void HOOK_RenderScene_Plants ();
void HOOK_RenderScene_end ();
void HOOK_CPlantMgr_Render ();
void HOOK_CEventHandler_ComputeKnockOffBikeResponse ();
void HOOK_CAnimManager_AddAnimation ();
void HOOK_CAnimManager_BlendAnimation ();
void HOOK_CPed_GetWeaponSkill ();

void vehicle_lights_init ();

CMultiplayerSA::CMultiplayerSA()
{
    // Unprotect all of the GTASA code at once and leave it that way
    DWORD oldProt;
    VirtualProtect((LPVOID)0x401000, 0x4A3000, PAGE_EXECUTE_READWRITE, &oldProt);

    // Initialize the offsets
    eGameVersion version = pGameInterface->GetGameVersion ();
    switch ( version )
    {
        case VERSION_EU_10: COffsetsMP::Initialize10EU (); break;
        case VERSION_US_10: COffsetsMP::Initialize10US (); break;
        case VERSION_11:    COffsetsMP::Initialize11 (); break;
        case VERSION_20:    COffsetsMP::Initialize20 (); break;
    }

	Population = new CPopulationSA;
    
    CRemoteDataSA::Init();

    m_bExplosionsDisabled = false;
	m_pExplosionHandler = NULL;
    m_pBreakTowLinkHandler = NULL;
    m_pDrawRadarAreasHandler = NULL;
    m_pDamageHandler = NULL;
    m_pFireHandler = NULL;
    m_pProjectileHandler = NULL;
    m_pProjectileStopHandler = NULL;

    memset ( &localStatsData, 0, sizeof ( CStatsData ) );
    localStatsData.StatTypesFloat [ 24 ] = 569.0f; // Max Health
}

void CMultiplayerSA::InitHooks()
{
	InitKeysyncHooks();
    InitShotsyncHooks();
    vehicle_lights_init ();
	bSetCenterOfWorld = false;
	bHasProcessedScript = false;

    eGameVersion version = pGameInterface->GetGameVersion ();

	bSetCenterOfWorld = false;	

	//00442DC6  |. 0F86 31090000  JBE gta_sa_u.004436FD
	//00442DC6     E9 32090000    JMP gta_sa_u.004436FD

    // increase the number of vehicles types (not actual vehicles) that can be loaded at once
    *(int *)0x8a5a84 = 127;

    // DISABLE CGameLogic::Update
	memset((void *)0x442AD0, 0xC3, 1);

    // STOP IT TRYING TO LOAD THE SCM
	*(BYTE *)0x468EB5 = 0xEB;
	*(BYTE *)0x468EB6 = 0x32;

	HookInstall(HOOKPOS_FindPlayerCoors, (DWORD)HOOK_FindPlayerCoors, 6);
	HookInstall(HOOKPOS_FindPlayerCentreOfWorld, (DWORD)HOOK_FindPlayerCentreOfWorld, 6);
	HookInstall(HOOKPOS_FindPlayerHeading, (DWORD)HOOK_FindPlayerHeading, 6);
	HookInstall(HOOKPOS_CStreaming_Update_Caller, (DWORD)HOOK_CStreaming_Update_Caller, 7);
	HookInstall(HOOKPOS_CHud_Draw_Caller, (DWORD)HOOK_CHud_Draw_Caller, 10);
	HookInstall(HOOKPOS_CRunningScript_Process, (DWORD)HOOK_CRunningScript_Process, 6);
	HookInstall(HOOKPOS_CExplosion_AddExplosion, (DWORD)HOOK_CExplosion_AddExplosion, 6);
    HookInstall(HOOKPOS_CRealTimeShadowManager__ReturnRealTimeShadow, (DWORD)HOOK_CRealTimeShadowManager__ReturnRealTimeShadow, 6);
	HookInstall(HOOKPOS_CCustomRoadsignMgr__RenderRoadsignAtomic, (DWORD)HOOK_CCustomRoadsignMgr__RenderRoadsignAtomic, 6);
    HookInstall(HOOKPOS_Trailer_BreakTowLink, (DWORD)HOOK_Trailer_BreakTowLink, 6);
    HookInstall(HOOKPOS_CRadar__DrawRadarGangOverlay, (DWORD)HOOK_CRadar__DrawRadarGangOverlay, 6);
    HookInstall(HOOKPOS_CTaskComplexJump__CreateSubTask, (DWORD)HOOK_CTaskComplexJump__CreateSubTask, 6);
    HookInstall(HOOKPOS_FxManager_CreateFxSystem, (DWORD)HOOK_FxManager_CreateFxSystem, 8);
    HookInstall(HOOKPOS_FxManager_DestroyFxSystem, (DWORD)HOOK_FxManager_DestroyFxSystem, 7);
    HookInstall(HOOKPOS_CCam_ProcessFixed, (DWORD)HOOK_CCam_ProcessFixed, 5);
    HookInstall(HOOKPOS_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon, (DWORD)HOOK_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon, 7);
    HookInstall(HOOKPOS_CPed_IsPlayer, (DWORD)HOOK_CPed_IsPlayer, 6);
    HookInstall(HOOKPOS_CTrain_ProcessControl_Derail, (DWORD)HOOK_CTrain_ProcessControl_Derail, 6);
    HookInstall(HOOKPOS_CVehicle_SetupRender, (DWORD)HOOK_CVehicle_SetupRender, 5);
    HookInstall(HOOKPOS_CVehicle_ResetAfterRender, (DWORD)HOOK_CVehicle_ResetAfterRender, 5);
    HookInstall(HOOKPOS_CObject_Render, (DWORD)HOOK_CObject_Render, 5);
	HookInstall(HOOKPOS_EndWorldColors, (DWORD)HOOK_EndWorldColors, 5);
    HookInstall(HOOKPOS_CWorld_ProcessVerticalLineSectorList, (DWORD)HOOK_CWorld_ProcessVerticalLineSectorList, 8);
    HookInstall(HOOKPOS_ComputeDamageResponse_StartChoking, (DWORD)HOOK_ComputeDamageResponse_StartChoking, 7);
    HookInstall(HOOKPOS_CollisionStreamRead, (DWORD)HOOK_CollisionStreamRead, 6);
    HookInstall(HOOKPOS_VehicleCamStart, (DWORD)HOOK_VehicleCamStart, 6);
    HookInstall(HOOKPOS_VehicleCamTargetZTweak, (DWORD)HOOK_VehicleCamTargetZTweak, 8);
    HookInstall(HOOKPOS_VehicleCamLookDir1, (DWORD)HOOK_VehicleCamLookDir1, 5);
    HookInstall(HOOKPOS_VehicleCamLookDir2, (DWORD)HOOK_VehicleCamLookDir2, 6);
    HookInstall(HOOKPOS_VehicleCamHistory, (DWORD)HOOK_VehicleCamHistory, 6);
    HookInstall(HOOKPOS_VehicleCamEnd, (DWORD)HOOK_VehicleCamEnd, 6);
    HookInstall(HOOKPOS_VehicleLookBehind, (DWORD)HOOK_VehicleLookBehind, 6);
    HookInstall(HOOKPOS_VehicleLookAside, (DWORD)HOOK_VehicleLookAside, 6);
    HookInstall(HOOKPOS_CPhysical_ApplyGravity, (DWORD)HOOK_CPhysical_ApplyGravity, 6);
    HookInstall(HOOKPOS_OccupiedVehicleBurnCheck, (DWORD)HOOK_OccupiedVehicleBurnCheck, 6);
    HookInstall(HOOKPOS_UnoccupiedVehicleBurnCheck, (DWORD)HOOK_UnoccupiedVehicleBurnCheck, 5);
    HookInstall(HOOKPOS_ApplyCarBlowHop, (DWORD)HOOK_ApplyCarBlowHop, 6);
    HookInstall(HOOKPOS_CWorld_SetWorldOnFire, (DWORD)HOOK_CWorld_SetWorldOnFire, 5);
    HookInstall(HOOKPOS_CTaskSimplePlayerOnFire_ProcessPed, (DWORD)HOOK_CTaskSimplePlayerOnFire_ProcessPed, 5);
    HookInstall(HOOKPOS_CFire_ProcessFire, (DWORD)HOOK_CFire_ProcessFire, 5);
    HookInstall(HOOKPOS_CExplosion_Update, (DWORD)HOOK_CExplosion_Update, 5);
    HookInstall(HOOKPOS_CWeapon_FireAreaEffect, (DWORD)HOOK_CWeapon_FireAreaEffect, 5);
    HookInstall(HOOKPOS_CGame_Process, (DWORD)HOOK_CGame_Process, 10 );
    HookInstall(HOOKPOS_Idle, (DWORD)HOOK_Idle, 10 );
    HookInstall(HOOKPOS_RenderScene_end, (DWORD)HOOK_RenderScene_end, 5);
    HookInstall(HOOKPOS_CPlantMgr_Render, (DWORD)HOOK_CPlantMgr_Render, 6);
    HookInstall(HOOKPOS_CEventHandler_ComputeKnockOffBikeResponse, (DWORD)HOOK_CEventHandler_ComputeKnockOffBikeResponse, 7 );
    HookInstall(HOOKPOS_CAnimManager_AddAnimation, (DWORD)HOOK_CAnimManager_AddAnimation, 10 ); 
    HookInstall(HOOKPOS_CAnimManager_BlendAnimation, (DWORD)HOOK_CAnimManager_BlendAnimation, 7 );
    HookInstall(HOOKPOS_CPed_GetWeaponSkill, (DWORD)HOOK_CPed_GetWeaponSkill, 8 );
    
    HookInstallCall ( CALL_CBike_ProcessRiderAnims, (DWORD)HOOK_CBike_ProcessRiderAnims );
    HookInstallCall ( CALL_Render3DStuff, (DWORD)HOOK_Render3DStuff );
    HookInstallCall ( CALL_VehicleCamUp, (DWORD)HOOK_VehicleCamUp );
    HookInstallCall ( CALL_VehicleLookBehindUp, (DWORD)HOOK_VehicleCamUp );
    HookInstallCall ( CALL_VehicleLookAsideUp, (DWORD)HOOK_VehicleCamUp );
    HookInstallCall ( CALL_RenderScene_Plants, (DWORD)HOOK_RenderScene_Plants );

    // Disable GTA setting g_bGotFocus to false when we minimize
    memset ( (void *)ADDR_GotFocus, 0x90, pGameInterface->GetGameVersion () == VERSION_EU_10 ? 6 : 10 );

    // Increase double link limit from 3200 ro 4000
    *(int*)0x00550F82 = 4000;


    // Disable GTA being able to call CAudio::StopRadio ()
    // Well this isn't really CAudio::StopRadio, it's some global class
    // func that StopRadio just jumps to.
	*(BYTE *)0x4E9820 = 0xC2;
    *(BYTE *)0x4E9821 = 0x08;
    *(BYTE *)0x4E9822 = 0x00;

    // Disable GTA being able to call CAudio::StartRadio ()
	*(BYTE *)0x4DBEC0 = 0xC2;
    *(BYTE *)0x4DBEC1 = 0x00;
    *(BYTE *)0x4DBEC2 = 0x00;

	*(BYTE *)0x4EB3C0 = 0xC2;
    *(BYTE *)0x4EB3C1 = 0x10;
    *(BYTE *)0x4EB3C2 = 0x00;
    
    // DISABLE cinematic camera for trains
    *(BYTE *)0x52A535 = 0;

    // DISABLE wanted levels for military zones
	*(BYTE *)0x72DF0D = 0xEB;

    // THROWN projectiles throw more accurately
	*(BYTE *)0x742685 = 0x90;
    *(BYTE *)0x742686 = 0xE9;

    // DISABLE CProjectileInfo::RemoveAllProjectiles
	*(BYTE *)0x7399B0 = 0xC3;

    // DISABLE CRoadBlocks::GenerateRoadblocks
    *(BYTE *)0x4629E0 = 0xC3;


    // Temporary hack for disabling hand up
    /*
	*(BYTE *)0x62AEE7 = 0x90;
    *(BYTE *)0x62AEE8 = 0x90;
    *(BYTE *)0x62AEE9 = 0x90;
    *(BYTE *)0x62AEEA = 0x90;
    *(BYTE *)0x62AEEB = 0x90;
    *(BYTE *)0x62AEEC = 0x90;
    */

    // DISABLE CAERadioTrackManager::CheckForMissionStatsChanges(void) (special DJ banter)
	*(BYTE *)0x4E8410 = 0xC3; // retn

    // DISABLE CPopulation__AddToPopulation
	*(BYTE *)0x614720 = 0x32; // xor al, al
    *(BYTE *)0x614721 = 0xC0;
    *(BYTE *)0x614722 = 0xC3; // retn

    // Disables deletion of RenderWare objects during unloading of ModelInfo
    // This is used so we can circumvent the limit of ~21 different vehicles by managing the RwObject ourselves
    //*(BYTE *)0x4C9890 = 0xC3;

    //memset ( (void*)0x408A1B, 0x90, 5 );

    // Hack to make the choke task use 0 time left remaining when he starts t
    // just stand there looking. So he won't do that.
    *(unsigned char *)0x620607 = 0x33;
    *(unsigned char *)0x620608 = 0xC0;

    *(unsigned char *)0x620618 = 0x33;
    *(unsigned char *)0x620619 = 0xC0;
    *(unsigned char *)0x62061A = 0x90;
    *(unsigned char *)0x62061B = 0x90;
    *(unsigned char *)0x62061C = 0x90;

    // Hack to make non-local players always update their aim on akimbo weapons using camera
    // so they don't freeze when local player doesn't aim.
    *(BYTE *)0x61EFFE = 0xEB;   // JMP imm8 (was JZ imm8)
    

    // DISABLE CGameLogic__SetPlayerWantedLevelForForbiddenTerritories
	*(BYTE *)0x441770 = 0xC3;

    // DISABLE CCrime__ReportCrime
	*(BYTE *)0x532010 = 0xC3;
    
	// Disables deletion of RenderWare objects during unloading of ModelInfo
	// This is used so we can circumvent the limit of ~21 different vehicles by managing the RwObject ourselves
	//*(BYTE *)0x4C9890 = 0xC3;

    /*
    004C021D   B0 00            MOV AL,0
    004C021F   90               NOP
    004C0220   90               NOP
    004C0221   90               NOP
    */
	*(BYTE *)0x4C01F0 = 0xB0;
	*(BYTE *)0x4C01F1 = 0x00;
    *(BYTE *)0x4C01F2 = 0x90;
    *(BYTE *)0x4C01F3 = 0x90;
    *(BYTE *)0x4C01F4 = 0x90;

    // Disable MakePlayerSafe
	*(BYTE *)0x56E870 = 0xC2;
	*(BYTE *)0x56E871 = 0x08;
	*(BYTE *)0x56E872 = 0x00;

    // Disable call to FxSystem_c__GetCompositeMatrix in CAEFireAudioEntity::UpdateParameters 
    // that was causing a crash - spent ages debugging, the crash happens if you create 40 or 
    // so vehicles that catch fire (upside down) then delete them, repeating a few times.
    memset((void*)0x4DCF87,0x90,6);
    
    /*
    // DISABLE CPed__RemoveBodyPart
	*(BYTE *)0x5F0140 = 0xC2;
    *(BYTE *)0x5F0141 = 0x08;
    *(BYTE *)0x5F0142 = 0x00;
    */

    // ALLOW picking up of all vehicles (GTA doesn't allow picking up of 'locked' script created vehicles)
	*(BYTE *)0x6A436C = 0x90;
    *(BYTE *)0x6A436D = 0x90;

    // MAKE CEntity::GetIsOnScreen always return true, experimental
   /*
	*(BYTE *)0x534540 = 0xB0;
    *(BYTE *)0x534541 = 0x01;
    *(BYTE *)0x534542 = 0xC3;
    */

    //DISABLE CPad::ReconcileTwoControllersInput
	/*
	*(BYTE *)0x53F530 = 0xC2;
    *(BYTE *)0x53F531 = 0x0C;
    *(BYTE *)0x53F532 = 0x00;

	*(BYTE *)0x53EF80 = 0xC3;

	*(BYTE *)0x541DDC = 0xEB;
    *(BYTE *)0x541DDD = 0x60;
*/
    // DISABLE big buildings (test)
    /*
	*(char*)0x533150 = 0xC3;
    */
    
	// PREVENT THE RADIO OR ENGINE STOPPING WHEN PLAYER LEAVES VEHICLE
	// THIS ON ITS OWN will cause sounds to be left behind and other artifacts
	/*
	*(char *)0x4FB8C0 = 0xC3;
	*/


/*	
	memset((void *)0x4FBA3E, 0x90, 5);
	*/
	

	// DISABLE REPLAYS
/*	
	memset((void *)0x460500, 0xC3, 1);
*/
    // PREVENT the game from making dummy objects (may fix a crash, guesswork really)
    // This seems to work, but doesn't actually fix anything. Maybe a reason to do it in the future.
    //00615FE3     EB 09          JMP SHORT gta_sa_u.00615FEE
	/*
	memset((void *)0x615FE3, 0xEB, 1);
	*/

    // Make combines eat players *untested*
	//memset ( (LPVOID)0x6A9739, 0x90, 6 );
    
    // Players always lean out whatever the camera mode
    // 00621983     EB 13          JMP SHORT hacked_g.00621998
	*(BYTE *)0x621983 = 0xEB;

    
    // Players can fire drivebys whatever camera mode
    // 627E01 - 6 bytes
	memset ( (LPVOID)0x627E01, 0x90, 6 );

	memset ( (LPVOID)0x62840D, 0x90, 6 );

	// Satchel crash fix
	// C89110: satchel (bomb) positions pointer?
	// C891A8+4: satchel (model) positions pointer? gets set to NULL on player death, causing an access violation
	// C891A8+12: satchel (model) disappear time (in SystemTime format). 738F99 clears the satchel when VAR_SystemTime is larger.
	memset ( (LPVOID)0x738F3A, 0x90, 83 );

    // Prevent gta stopping driveby players from falling off
	memset ( (LPVOID)0x6B5B17, 0x90, 6 );

    // Increase VehicleStruct pool size
	*(BYTE *)0x5B8FE4 = 0x7F; // its signed so the higest you can go with this is 0x7F before it goes negative = crash
    
	/*
    // CTaskSimpleCarDrive: Swaps driveby for gang-driveby for drivers
	memset ( (LPVOID)0x6446A7, 0x90, 6 );
    
    // CTaskSimpleCarDrive: Swaps driveby for gang-driveby for passengers
	memset ( (LPVOID)0x6446BD, 0x90, 6 );
	*/
    

	// DISABLE PLAYING REPLAYS
	memset((void *)0x460390, 0xC3, 1);

	memset((void *)0x4600F0, 0xC3, 1);

	memset((void *)0x45F050, 0xC3, 1);

	// DISABLE CHEATS
	memset((void *)0x439AF0, 0xC3, 1);
		
	memset((void *)0x438370, 0xC3, 1);


    // DISABLE GARAGES
    *(BYTE *)(0x44AA89 + 0) = 0xE9;
    *(BYTE *)(0x44AA89 + 1) = 0x28;
    *(BYTE *)(0x44AA89 + 2) = 0x01;
    *(BYTE *)(0x44AA89 + 3) = 0x00;
    *(BYTE *)(0x44AA89 + 4) = 0x00;
    *(BYTE *)(0x44AA89 + 5) = 0x90;

    *(DWORD *)0x44C7E0 = 0x44C7C4;
    *(DWORD *)0x44C7E4 = 0x44C7C4;
    *(DWORD *)0x44C7F8 = 0x44C7C4;
    *(DWORD *)0x44C7FC = 0x44C7C4;
    *(DWORD *)0x44C804 = 0x44C7C4;
    *(DWORD *)0x44C808 = 0x44C7C4;
    *(DWORD *)0x44C83C = 0x44C7C4;
    *(DWORD *)0x44C840 = 0x44C7C4;
    *(DWORD *)0x44C850 = 0x44C7C4;
    *(DWORD *)0x44C854 = 0x44C7C4;
    *(DWORD *)0x44C864 = 0x44C7C4;
    *(DWORD *)0x44C868 = 0x44C7C4;
    *(DWORD *)0x44C874 = 0x44C7C4;
    *(DWORD *)0x44C878 = 0x44C7C4;
    *(DWORD *)0x44C88C = 0x44C7C4;
    *(DWORD *)0x44C890 = 0x44C7C4;
    *(DWORD *)0x44C89C = 0x44C7C4;
    *(DWORD *)0x44C8A0 = 0x44C7C4;
    *(DWORD *)0x44C8AC = 0x44C7C4;
    *(DWORD *)0x44C8B0 = 0x44C7C4;

    *(BYTE *)(0x44C39A + 0) = 0x0F;
    *(BYTE *)(0x44C39A + 1) = 0x84;
    *(BYTE *)(0x44C39A + 2) = 0x24;
    *(BYTE *)(0x44C39A + 3) = 0x04;
    *(BYTE *)(0x44C39A + 4) = 0x00;
    *(BYTE *)(0x44C39A + 5) = 0x00;

    // Avoid garage doors closing when you change your model
    memset((LPVOID)0x4486F7, 0x90, 4);
    

    // Disable CStats::IncrementStat (returns at start of function)
	*(BYTE *)0x55C180 = 0xC3;
	/*
	memset((void *)0x55C1A9, 0x90, 14 );
	memset((void *)0x55C1DD, 0x90, 7 );
	*/

	// DISABLE STATS DECREMENTING
	memset((void *)0x559FD5, 0x90, 7 );
	memset((void *)0x559FEB, 0x90, 7 );

	// DISABLE STATS MESSAGES
	memset((void *)0x55B980, 0xC3, 1);

	memset((void *)0x559760, 0xC3, 1);

    // ALLOW more than 8 players (crash with more if this isn't done)
    //0060D64D   90               NOP
    //0060D64E   E9 9C000000      JMP gta_sa.0060D6EF
	*(BYTE *)0x60D64D = 0x90;
    *(BYTE *)0x60D64E = 0xE9;

    // PREVENT CJ smoking and drinking like an addict
    //005FBA26   EB 29            JMP SHORT gta_sa.005FBA51
	*(BYTE *)0x5FBA26 = 0xEB;

    // PREVENT the camera from messing up for drivebys for vehicle drivers
	*(BYTE *)0x522423 = 0x90;
    *(BYTE *)0x522424 = 0x90;
    
    LPVOID patchAddress = NULL;
    // ALLOW ALT+TABBING WITHOUT PAUSING
    //if ( pGameInterface->GetGameVersion() == GAME_VERSION_US ) // won't work as pGameInterface isn't inited
    if ( *(BYTE *)0x748ADD == 0xFF && *(BYTE *)0x748ADE == 0x53 )
        patchAddress = (LPVOID)0x748A8D;
    else
        patchAddress = (LPVOID)0x748ADD;

    memset(patchAddress, 0x90, 6);

    // CENTER VEHICLE NAME and ZONE NAME messages
    // 0058B0AD   6A 02            PUSH 2 // orientation
    // VEHICLE
	*(BYTE *)0x58B0AE = 0x00;

    // ZONE
	*(BYTE *)0x58AD56 = 0x00;

    // 85953C needs to equal 320.0 to center the text (640.0 being the base width)
	*(float *)0x85953C = 320.0f;

    // 0058B147   D80D 0C958500    FMUL DWORD PTR DS:[85950C] // the text needs to be moved to the left
    //VEHICLE
	*(BYTE *)0x58B149 = 0x3C;

    //ZONE
	*(BYTE *)0x58AE52 = 0x3C;

	// DISABLE SAM SITES
	*(BYTE *)0x5A07D0 = 0xC3;

	// DISABLE TRAINS (AUTO GENERATED ONES)
	*(BYTE *)0x6F7900 = 0xC3;
    
    // Prevent TRAINS spawning with PEDs
	*(BYTE *)0x6F7865 = 0xEB;

	// DISABLE PLANES
	*(BYTE *)0x6CD2F0 = 0xC3;
	
	// DISABLE EMERGENCY VEHICLES
	*(BYTE *)0x42B7D0 = 0xC3;

	// DISABLE CAR GENERATORS
	*(BYTE *)0x6F3F40 = 0xC3;

    // DISABLE CEntryExitManager::Update (they crash when you enter anyway)
	*(BYTE *)0x440D10 = 0xC3;

	// Disable MENU AFTER alt + tab
	//0053BC72   C605 7B67BA00 01 MOV BYTE PTR DS:[BA677B],1	
	*(BYTE *)0x53BC78 = 0x00;

	// DISABLE HUNGER MESSAGES
	memset ( (LPVOID)0x56E740, 0x90, 5 );

	// DISABLE RANDOM VEHICLE UPGRADES
	memset ( (LPVOID)0x6B0BC2, 0xEB, 1 );

	// DISABLE CPOPULATION::UPDATE - DOES NOT prevent vehicles - only on-foot peds
	/*	
	*(BYTE *)0x616650 = 0xC3;
    *(BYTE *)0xA43088 = 1;
	*/

	// SORT OF HACK to make peds always walk around, even when in free-camera mode (in the editor)
	*(BYTE *)0x53C017 = 0x90;
	*(BYTE *)0x53C018 = 0x90;

	// DISABLE random cars
	//*(BYTE *)0x4341C0 = 0xC3;
	
	// DISABLE heat flashes
	/*
	*(BYTE *)0x6E3521 = 0x90;
	*(BYTE *)0x6E3522 = 0xE9;
	*/

	// DECREASE ROF for missiles from hydra
	// 006D462C     81E1 E8030000  AND ECX,3E8
	// 006D4632     81C1 E8030000  ADD ECX,3E8
    /*	
	*(BYTE *)0x6D462E = 0xE8;
	*(BYTE *)0x6D462F = 0x03;
	*(BYTE *)0x6D4634 = 0xE8;
	*(BYTE *)0x6D4635 = 0x03;
    */

	// HACK to allow boats to be rotated
	/*
	006F2089   58               POP EAX
	006F208A   90               NOP
	006F208B   90               NOP
	006F208C   90               NOP
	006F208D   90               NOP
	*/
	*(BYTE *)0x6F2089 = 0x58;
	memset((void *)0x6F208A,0x90,4);

    // Prevent the game deleting _any_ far away vehicles - will cause issues for population vehicles in the future
	*(BYTE *)0x42CD10 = 0xC3;

    // DISABLE real-time shadows for peds
    *(BYTE *)0x5E68A0 = 0xEB;

    // and some more, just to be safe
    //00542483   EB 0B            JMP SHORT gta_sa.00542490
    *(BYTE *)0x542483 = 0xEB;

    // DISABLE weapon pickups
    *(BYTE *)0x5B47B0 = 0xC3;

    // INCREASE CEntyInfoNode pool size
    //00550FB9   68 F4010000      PUSH 1F4
    /*
    *(BYTE *)0x550FBA = 0xE8;
    *(BYTE *)0x550FBB = 0x03;
    */
    *(BYTE *)0x550FBA = 0x00;
    *(BYTE *)0x550FBB = 0x10;

    
	/*
	*(BYTE *)0x469F00 = 0xC3;
	*/

	// CCAM::PROCESSFIXED remover
/*
	*(BYTE *)0x51D470 = 0xC2;
	*(BYTE *)0x51D471 = 0x10;
	*(BYTE *)0x51D472 = 0x00;
*/

	// HACK to prevent RealTimeShadowManager crash
	// 00542483     EB 0B          JMP SHORT gta_sa_u.00542490
	/*
	*(BYTE *)0x542483 = 0xEB;
*/
	
	//InitShotsyncHooks();

    //DISABLE CPad::ReconcileTwoControllersInput
    *(BYTE *)0x53F530 = 0xC2;
    *(BYTE *)0x53F531 = 0x0C;
    *(BYTE *)0x53F532 = 0x00;

    *(BYTE *)0x53EF80 = 0xC3;

    *(BYTE *)0x541DDC = 0xEB;
    *(BYTE *)0x541DDD = 0x60;

    // DISABLE CWanted Helis (always return 0 from CWanted::NumOfHelisRequired)
    *(BYTE *)0x561FA4 = 0x90;
    *(BYTE *)0x561FA5 = 0x90;

    // DISABLE  CWanted__UpdateEachFrame
    memset( (void*)0x53BFF6, 0x90, 5 );

    // DISABLE CWanted__Update
    memset( (void*)0x60EBCC, 0x90, 5 );

	// Disable armour-increase upon entering an enforcer
	*(BYTE *)0x6D189B = 0x06;

	// Removes the last weapon pickups from interiors as well
	*(BYTE *)0x591F90 = 0xC3;

	// Trains may infact go further than Los Santos
	*(BYTE *)0x4418E0 = 0xC3;

    // EXPERIMENTAL - disable unloading of cols
   // memset( (void*)0x4C4EDA, 0x90, 10 );

    // Make CTaskComplexSunbathe::CanSunbathe always return true
	*(BYTE *)0x632140 = 0xB0;
    *(BYTE *)0x632141 = 0x01;
    *(BYTE *)0x632142 = 0xC3;
    
    // Stop CTaskSimpleCarDrive::ProcessPed from exiting passengers with CTaskComplexSequence (some timer check)
    *(BYTE *)0x644C18 = 0x90;
	*(BYTE *)0x644C19 = 0xE9;

    // Stop CPlayerPed::ProcessControl from calling CVisibilityPlugins::SetClumpAlpha
    memset ( (void*)0x5E8E84, 0x90, 5 );

    // Stop CVehicle::UpdateClumpAlpha from calling CVisibilityPlugins::SetClumpAlpha
    memset ( (void*)0x6D29CB, 0x90, 5 );

    // Disable CVehicle::DoDriveByShootings
    memset ( (void*)0x741FD0, 0x90, 3 );
    *(BYTE *)0x741FD0 = 0xC3;

    // Disable CTaskSimplePlayerOnFoot::PlayIdleAnimations (ret 4)
    *(BYTE *)0x6872C0 = 0xC2;
    *(BYTE *)0x6872C1 = 0x04;
    *(BYTE *)0x6872C2 = 0x00;

    /*
    // Disable forcing of ped animations to the player one in CPlayerPed::ProcessAnimGroups
    memset ( (LPVOID)0x609A44, 0x90, 21 );
    */

    // Let us sprint everywhere (always return 0 from CSurfaceData::isSprint)
    *(DWORD *)0x55E870 = 0xC2C03366;
    *(WORD *)0x55E874  = 0x0004;

    // Create pickup objects in interior 0 instead of 13
    *(BYTE *)0x59FAA3 = 0x00;

    // Don't get shotguns from police cars
    *(BYTE *)0x6D19CD = 0xEB;

    // Don't get golf clubs from caddies
    *(BYTE *)0x6D1A1A = 0xEB;

	// Don't get 20 health from ambulances
	*(BYTE *)0x6D1762 = 0x00;

    // Prevent CVehicle::RecalcTrainRailPosition from changing train speed
    memset((void *)0x6F701D, 0x90, 6);
    *(BYTE *)0x6F7069 = 0xEB;

    // The instanthit function for bullets ignores the first few bullets shot by
    // remote players after reloading because some flag isn't set (no bullet impact
    // graphics, no damage). Makes e.g. sawnoffs completely ineffective.
    // Remove this check so that no bullets are ignored.
    *(BYTE *)0x73FDF9 = 0xEB;

    // Disallow spraying gang tags
    // Nop the whole CTagManager::IsTag function and replace its body with:
    // xor eax, eax
    // ret
    // to make it always return false
    memset ( (void *)0x49CCE0, 0x90, 74 );
    *(DWORD *)(0x49CCE0) = 0x90C3C033;
    // Remove also some hardcoded and inlined checks for if it's a tag
    memset ( (void *)0x53374A, 0x90, 56 );
    *(BYTE *)(0x4C4403) = 0xEB;


    // Allow turning on vehicle lights even if the engine is off
    memset ( (void *)0x6E1DBC, 0x90, 8 );

    // Fix vehicle back lights both using light state 3 (SA bug)
    *(BYTE *)0x6E1D4F = 2;

    // Fix for sliding over objects and vehicles (ice floor)
    *(BYTE *)0x5E1E72 = 0xE9;
    *(BYTE *)0x5E1E73 = 0xB9;
    *(BYTE *)0x5E1E74 = 0x00;
    *(BYTE *)0x5E1E77 = 0x90;

    // Avoid GTA setting vehicle first color to white after changing the paintjob
    memset ( (void *)0x6D65C5, 0x90, 11 );

    // Disable idle cam
    *(BYTE *)0x522C80 = 0xC3;

    // Disable radar map hiding when pressing TAB (action key) while on foot
    memset ( (void *)0x58FC3E, 0x90, 14 );

    // No intro movies kthx
    if ( version == VERSION_US_10 )
    {
        *(DWORD *)0x748EF8 = 0x748AE7;
        *(DWORD *)0x748EFC = 0x748B08;
        *(BYTE *)0x748B0E = 5;
    }
    else if ( version == VERSION_EU_10 )
    {
        *(DWORD *)0x748F48 = 0x748B37;
        *(DWORD *)0x748F4C = 0x748B58;
        *(BYTE *)0x748B5E = 5;
    }

    // Force triggering of the damage event for players on fire
    memset ( (void *)0x633695, 0x90, 6 );
    *(BYTE *)0x633720 = 0;

    // Make CCreepingFire::TryToStartFireAtCoors return the fire pointer rather than a bool
    *(BYTE *)0x53A459 = 0x33;
    *(BYTE *)0x53A568 = 0x8B;
    *(BYTE *)0x53A4A9 = 0x33;
    *(WORD *)0x53A55F = 0x9090;
    *(BYTE *)0x73EC06 = 0x85;       // CWeapon::FireAreaEffect: test al,al -> test eax,eax

    // Increase the events pool size (Fixes #4577).
    *(DWORD *)0x551177 = 9001;

    // Do not fixate camera behind spectated player if local player is dead
    *(BYTE *)0x52A2BB = 0;
    *(BYTE *)0x52A4F8 = 0;

    // Always render water after other entities (otherwise underwater LODs and trees are rendered
    // in front of it)
    *(BYTE *)0x53DFF5 = 0xEB;
    *(WORD *)0x53E133 = 0x9090;
    // Disable some stack management instructions as we need ebx for a bit longer. We replicate
    // these in HOOK_RenderScene_end
    *(BYTE *)0x53E132 = 0x90;
    memset ( (void *)0x53E156, 0x90, 3 );
    // Use 0.5 instead of 0.0 for underwater threshold
    *(DWORD *)0x53DF4B = 0x858B8C;

    // Disable setting players on fire when they're riding burning bmx's (see #4573)
    * ( BYTE * ) ( 0x53A982 ) = 0xEB;

    // Disable stealth-kill aiming (holding knife up)
    memset ( (void *)0x685DFB, 0x90, 5 );
    * ( BYTE * ) ( 0x685DFB ) = 0x33;
    * ( BYTE * ) ( 0x685DFC ) = 0xC0;
    memset ( (void *)0x685C3E, 0x90, 5 );
    * ( BYTE * ) ( 0x685C3E ) = 0x33;
    * ( BYTE * ) ( 0x685C3F ) = 0xC0;    
    memset ( (void *)0x685DC4, 0x90, 5 );
    * ( BYTE * ) ( 0x685DC4 ) = 0x33;
    * ( BYTE * ) ( 0x685DC5 ) = 0xC0;
    memset ( (void *)0x685DE6, 0x90, 5 );
    * ( BYTE * ) ( 0x685DE6 ) = 0x33;
    * ( BYTE * ) ( 0x685DE7 ) = 0xC0;
}


// Used to store copied pointers for explosions in the FxSystem

std::list < DWORD* > Pointers_FxSystem;

void AddFxSystemPointer ( DWORD* pPointer )
{
    Pointers_FxSystem.push_front ( pPointer );
}


void RemoveFxSystemPointer ( DWORD* pPointer )
{
    // Look through our list for the pointer
    std::list < DWORD* > ::iterator iter = Pointers_FxSystem.begin ();
    for ( ; iter != Pointers_FxSystem.end (); iter++ )
    {
        // It exists in our list?
        if ( *iter == pPointer )
        {
            // Remove it from the list over our copied matrices
            Pointers_FxSystem.erase ( iter );

            // Delete the pointer itself
            free ( pPointer );
            return;
        }
    }
}

CRemoteDataStorage * CMultiplayerSA::CreateRemoteDataStorage ()
{
    return new CRemoteDataStorageSA ();
}

void CMultiplayerSA::DestroyRemoteDataStorage ( CRemoteDataStorage* pData )
{
    delete (CRemoteDataStorageSA *)pData;
}

void CMultiplayerSA::AddRemoteDataStorage ( CPlayerPed* pPed, CRemoteDataStorage* pData )
{
    CRemoteDataSA::AddRemoteDataStorage ( pPed, pData );
}

void CMultiplayerSA::RemoveRemoteDataStorage ( CPlayerPed* pPed )
{
    CRemoteDataSA::RemoveRemoteDataStorage ( pPed );
}

CPed * CMultiplayerSA::GetContextSwitchedPed ( void )
{
    return pContextSwitchedPed;
}

void CMultiplayerSA::AllowWindowsCursorShowing ( bool bAllow )
{
    /*
    0074821D   6A 00            PUSH 0
    0074821F   FF15 EC828500    CALL DWORD PTR DS:[<&USER32.ShowCursor>] ; USER32.ShowCursor
    00748225   6A 00            PUSH 0
    00748227   FF15 9C828500    CALL DWORD PTR DS:[<&USER32.SetCursor>]  ; USER32.SetCursor
    */
    BYTE originalCode[16] = {0x6A, 0x00, 0xFF, 0x15, 0xEC, 0x82, 0x85, 0x00, 0x6A, 0x00, 0xFF, 0x15, 0x9C, 0x82, 0x85, 0x00};

    if ( bAllow )
    {
	    memset ( (LPVOID)ADDR_CursorHiding, 0x90, 16 );
    }
    else
    {
        memcpy ( (LPVOID)ADDR_CursorHiding, &originalCode, 16 );
    }
}


CShotSyncData * CMultiplayerSA::GetLocalShotSyncData ( )
{
    return GetLocalPedShotSyncData();
}

void CMultiplayerSA::DisablePadHandler( bool bDisabled )
{
	// DISABLE GAMEPADS (testing)
	if ( bDisabled )
		*(BYTE *)0x7449F0 = 0xC3;
	else
		*(BYTE *)0x7449F0 = 0x8B;
}

void CMultiplayerSA::DisableHeatHazeEffect ( bool bDisable )
{
	*(bool *)0xC402BA = bDisable;
}

void CMultiplayerSA::DisableAllVehicleWeapons ( bool bDisable )
{
	if ( bDisable )
		*(BYTE *)0x6E3950 = 0xC3;
	else
		*(BYTE *)0x6E3950 = 0x83;
}

void CMultiplayerSA::DisableZoneNames ( bool bDisabled )
{
    // Please use CHud::DisableAreaName instead
    pGameInterface->GetHud()->DisableAreaName ( bDisabled );
}

void CMultiplayerSA::DisableBirds ( bool bDisabled )
{
	if ( bDisabled )
		*(BYTE *)0x712330 = 0xC3;
	else
		*(BYTE *)0x712330 = 0xA1;
}

void CMultiplayerSA::SetCloudsEnabled ( bool bDisabled )
{
    //volumetric clouds
	if ( bDisabled )
        *(BYTE *)0x716380 = 0xA1;
	else
		*(BYTE *)0x716380 = 0xC3;

    // normal clouds
    //0071395A     90             NOP
    if ( bDisabled )
        *(BYTE *)0x713950 = 0x83;
    else
        *(BYTE *)0x713950 = 0xC3;

    // plane trails (not really clouds, but they're sort of vapour)

    if ( bDisabled )
    {
		*(BYTE *)0x717180 = 0x83;
        *(BYTE *)0x717181 = 0xEC;
        *(BYTE *)0x717182 = 0x08;
    }
	else
    {
		*(BYTE *)0x717180 = 0xC2;
        *(BYTE *)0x717181 = 0x04;
        *(BYTE *)0x717182 = 0x00;
    }
}

void CMultiplayerSA::SetSkyColor ( unsigned char TopRed, unsigned char TopGreen, unsigned char TopBlue, unsigned char BottomRed, unsigned char BottomGreen, unsigned char BottomBlue )
{
	bUsingCustomSkyGradient = true;
	ucSkyGradientTopR = TopRed;
	ucSkyGradientTopG = TopGreen;
	ucSkyGradientTopB = TopBlue;
	ucSkyGradientBottomR = BottomRed;
	ucSkyGradientBottomG = BottomGreen;
	ucSkyGradientBottomB = BottomBlue;
}

void CMultiplayerSA::ResetSky ( void )
{
	bUsingCustomSkyGradient = false;
}

void CMultiplayerSA::SetWaterColor ( float fWaterRed, float fWaterGreen, float fWaterBlue, float fWaterAlpha )
{
	bUsingCustomWaterColor = true;
    // Water surface
	fWaterColorR = fWaterRed;
	fWaterColorG = fWaterGreen;
	fWaterColorB = fWaterBlue;
	fWaterColorA = fWaterAlpha;
    // Underwater
    *(BYTE *)0x8D5140 = (BYTE)fWaterRed;
    *(BYTE *)0x8D5141 = (BYTE)fWaterGreen;
    *(BYTE *)0x8D5142 = (BYTE)fWaterBlue;
    *(BYTE *)0x8D5143 = (BYTE)fWaterAlpha;
    *(BYTE *)0x7051A7 = 255-(BYTE)fWaterAlpha;          // These numbers are added to r,g,b
    *(float *)0x872660 = 255-fWaterAlpha;
    *(BYTE *)0x7051D7 = 255-(BYTE)fWaterAlpha;
}

void CMultiplayerSA::ResetWater ( void )
{
	bUsingCustomWaterColor = false;
    *(DWORD *)0x8D5140 = 0x40404040;
    *(BYTE *)0x7051A7 = 184;
    *(float *)0x872660 = 184.0f;
    *(BYTE *)0x7051D7 = 184;
}

bool CMultiplayerSA::GetExplosionsDisabled ( void )
{
    return m_bExplosionsDisabled;
}


void CMultiplayerSA::DisableExplosions ( bool bDisabled )
{
    m_bExplosionsDisabled = bDisabled;
}



void CMultiplayerSA::SetExplosionHandler ( ExplosionHandler * pExplosionHandler )
{
	m_pExplosionHandler = pExplosionHandler;
}

void CMultiplayerSA::SetProjectileHandler ( ProjectileHandler * pProjectileHandler )
{
    m_pProjectileHandler = pProjectileHandler;
}

void CMultiplayerSA::SetProjectileStopHandler ( ProjectileStopHandler * pProjectileHandler )
{
    m_pProjectileStopHandler = pProjectileHandler;
}

void CMultiplayerSA::SetBreakTowLinkHandler ( BreakTowLinkHandler * pBreakTowLinkHandler )
{
    m_pBreakTowLinkHandler = pBreakTowLinkHandler;
}

void CMultiplayerSA::SetDrawRadarAreasHandler ( DrawRadarAreasHandler * pRadarAreasHandler )
{
    m_pDrawRadarAreasHandler = pRadarAreasHandler;
}

void CMultiplayerSA::SetRender3DStuffHandler ( Render3DStuffHandler * pHandler )
{
    m_pRender3DStuffHandler = pHandler;
}

void CMultiplayerSA::SetDamageHandler ( DamageHandler * pDamageHandler )
{
    m_pDamageHandler = pDamageHandler;
}

void CMultiplayerSA::SetFireHandler ( FireHandler * pFireHandler )
{
    m_pFireHandler = pFireHandler;
}

void CMultiplayerSA::SetProcessCamHandler ( ProcessCamHandler* pProcessCamHandler )
{
    m_pProcessCamHandler = pProcessCamHandler;
}

void CMultiplayerSA::SetChokingHandler ( ChokingHandler* pChokingHandler )
{
    m_pChokingHandler = pChokingHandler;
}

void CMultiplayerSA::SetPostWorldProcessHandler ( PostWorldProcessHandler * pHandler )
{
    m_pPostWorldProcessHandler = pHandler;
}

void CMultiplayerSA::SetIdleHandler ( IdleHandler * pHandler )
{
    m_pIdleHandler = pHandler;
}

void CMultiplayerSA::SetAddAnimationHandler ( AddAnimationHandler * pHandler )
{
    m_pAddAnimationHandler = pHandler;
}

void CMultiplayerSA::SetBlendAnimationHandler ( BlendAnimationHandler * pHandler )
{
    m_pBlendAnimationHandler = pHandler;
}

void CMultiplayerSA::HideRadar ( bool bHide )
{
	bHideRadar = bHide;
}

void CMultiplayerSA::AllowMouseMovement ( bool bAllow )
{
    if ( bAllow )
        *(BYTE *)0x6194A0 = 0xC3;
    else
        *(BYTE *)0x6194A0 = 0xE9;
}

void CMultiplayerSA::DoSoundHacksOnLostFocus ( bool bLostFocus )
{
    if ( bLostFocus )
        memset ( (void *)0x4D9888, 0x90, 5 );
    else
    {
        //004D9888   . E8 03 F1 FF FF    CALL gta_sa_u.004D8990
        *(BYTE *)0x4D9888 = 0xE8;
        *(BYTE *)0x4D9889 = 0x03;
        *(BYTE *)0x4D988A = 0xF1;
        *(BYTE *)0x4D988B = 0xFF;
        *(BYTE *)0x4D988C = 0xFF;
    }
}

void CMultiplayerSA::SetCenterOfWorld(CEntity * entity, CVector * vecPosition, FLOAT fHeading)
{
	if ( vecPosition )
	{
		bInStreamingUpdate = false;

		vecCenterOfWorld.fX = vecPosition->fX;
		vecCenterOfWorld.fY = vecPosition->fY;
		vecCenterOfWorld.fZ = vecPosition->fZ;

		if ( entity )
        {
            CEntitySA* pEntitySA = dynamic_cast < CEntitySA* > ( entity );
			if ( pEntitySA )
				activeEntityForStreaming = pEntitySA->GetInterface();
        }
	/*	else
		{
			if ( !bActiveEntityForStreamingIsFakePed )
			{
				activeEntityForStreaming = new CPedSAInterface();
				memset(activeEntityForStreaming, 0, sizeof(CPedSAInterface));
				activeEntityForStreaming->Placeable.matrix = new CMatrix_Padded();
			}

			bActiveEntityForStreamingIsFakePed = true;
            
			activeEntityForStreaming->Placeable.matrix->vPos.fX = vecPosition->fX;
			activeEntityForStreaming->Placeable.matrix->vPos.fY = vecPosition->fY;
			activeEntityForStreaming->Placeable.matrix->vPos.fZ = vecPosition->fZ;
		}*/

		//DWORD dwCurrentValue = *(DWORD *)FUNC_CPlayerInfoBase;
		fFalseHeading = fHeading;
		bSetCenterOfWorld = true;
	}
	else 
	{
		/*if ( bActiveEntityForStreamingIsFakePed )
		{
			delete activeEntityForStreaming->Placeable.matrix;
			delete activeEntityForStreaming;
		}

		bActiveEntityForStreamingIsFakePed = false;*/
		activeEntityForStreaming = NULL;
		bSetCenterOfWorld = false;
	}
}

void _declspec(naked) HOOK_FindPlayerCoors()
{
    _asm
    {
        // Only set our world of center if we have a center of world set
        push    eax
        mov     al, bSetCenterOfWorld
        test    al, al
        jz      dontset

        // Only set our world of center if bInStreamingUpdate is true
        mov     al, bInStreamingUpdate
        test    al, al
        jz      dontset

        // Move our center of world into gta's senter of world when it requests so
        pop     eax
		lea		esi, vecCenterOfWorld
		mov		edi, [esp+4]
		mov		eax, edi
		movsd
		movsd
		movsd			
		retn	

        // Continue. Don't replace the world center.
        dontset:
        pop     eax
		mov		eax, [esp+8]
		xor		edx, edx
        mov		ecx, CMultiplayerSA::HOOKPOS_FindPlayerCoors
		add		ecx, 6
		jmp		ecx
	}
}

void _declspec(naked) HOOK_CStreaming_Update_Caller()
{
	/*
	0053BF09   8BF8             MOV EDI,EAX
	0053BF0B   E8 6027EDFF      CALL gta_sa.0040E670
	*/

    _asm
    {
        // Store all registers
        pushad
    }

    // We're now in the streaming update
	bInStreamingUpdate = true;

    // We have an active entity for streaming?
	if( activeEntityForStreaming )
	{
        // Do something...
		_asm
		{
			mov		edi, CMultiplayerSA::FUNC_CPlayerInfoBase
			mov		ebx, [edi]
			mov		dwSavedPlayerPointer, ebx
			mov		ebx, activeEntityForStreaming
			mov		[edi], ebx
		}
	}

	_asm
	{
		mov		edi, eax

        // Call CMultiplayerSA::FUNC_CStreaming_Update
		mov		eax, CMultiplayerSA::FUNC_CStreaming_Update
		call	eax
	}

    // We have an entity for streaming?
	if ( activeEntityForStreaming )
	{
		_asm
		{
            // ...
			mov		edi, CMultiplayerSA::FUNC_CPlayerInfoBase
			mov		ebx, dwSavedPlayerPointer
			mov		[edi], ebx
		}
	}

    // We're no longer in streaming update
	bInStreamingUpdate = false;
	_asm
	{
        // Restore registers
        popad

        // Continue at the old func
        mov		eax, CMultiplayerSA::HOOKPOS_CStreaming_Update_Caller
		add		eax, 7
		jmp		eax
    }
}

void _declspec(naked) HOOK_CHud_Draw_Caller()
{
	/*
	0053E4FA   . E8 318BFCFF                          CALL gta_sa_u.00507030
	0053E4FF   . E8 DC150500                          CALL gta_sa_u.0058FAE0
	*/
	_asm
	{
        pushad

		mov		edx, CMultiplayerSA::FUNC_CAudioEngine__DisplayRadioStationName
		call	edx
	}

	if(!bSetCenterOfWorld)
	{
		_asm
		{
			mov		edx, CMultiplayerSA::FUNC_CHud_Draw
			call	edx
		}

	}
	else
	{
		/*if ( activeEntityForStreaming )
		{
			_asm
			{
				mov		edi, FUNC_CPlayerInfoBase
				mov		ebx, [edi]
				mov		dwSavedPlayerPointer, ebx
				mov		ebx, activeEntityForStreaming
				mov		[edi], ebx
			}
		}*/

		if (!bHideRadar)
		{
			_asm
			{
				mov		edx, 0x58A330
				call	edx
			}
		}

		/*if ( activeEntityForStreaming )
		{
			_asm
			{
				mov		edi, FUNC_CPlayerInfoBase
				mov		ebx, dwSavedPlayerPointer
				mov		[edi], ebx
			}
		}*/
	}

	_asm
	{
        popad

		mov		eax, CMultiplayerSA::HOOKPOS_CHud_Draw_Caller
		add		eax, 10
		jmp		eax
	}
}

void _declspec(naked) HOOK_FindPlayerCentreOfWorld()
{
	/*
	0056E250  /$ 8B4424 04      MOV EAX,DWORD PTR SS:[ESP+4]
	0056E254  |. 85C0           TEST EAX,EAX
	*/

    _asm
    {
        mov     al, bSetCenterOfWorld
        test    al, al
        jnz     hascenter

		mov		eax, [esp+4]
		test	eax, eax

		mov		edx, CMultiplayerSA::HOOKPOS_FindPlayerCentreOfWorld
		add		edx, 6
		jmp		edx
		

        hascenter:
        lea		eax, vecCenterOfWorld
        retn
	}
	
}

void _declspec(naked) HOOK_FindPlayerHeading()
{
	/*
	0056E450  /$ 8B4C24 04      MOV ECX,DWORD PTR SS:[ESP+4]
	0056E454  |. 8BD1           MOV EDX,ECX
	*/

    _asm
    {
        // Jump if bSetCenterOfWorld is true
        push    eax
        mov     al, bSetCenterOfWorld
        test    al, al
        pop     eax
        jnz     hascenter

		mov		ecx, [esp+4]
		mov		edx, ecx

        // Return to the hookpos
		mov		eax, CMultiplayerSA::HOOKPOS_FindPlayerHeading
		add		eax, 6
		jmp		eax

        // ..
        hascenter:
		fld		fFalseHeading
		retn
	}
	
}

// this hook adds a null check to prevent the game crashing when objects are placed really high up (issue 517)
void _declspec(naked) HOOK_CCustomRoadsignMgr__RenderRoadsignAtomic()
{
    _asm
    {
        cmp     esi, 0
        jz      no_render

        // original code
        mov     eax, dword ptr[esi+4]
        fsub    [eax+64]
        mov     edx, CMultiplayerSA::HOOKPOS_CCustomRoadsignMgr__RenderRoadsignAtomic
        add     edx, 6
        jmp     edx
no_render:
        mov     edx, 0x6FF40B
        jmp     edx
    }
}

bool CallBreakTowLinkHandler ( CVehicleSAInterface * vehicle )
{
    CVehicle * pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD *)vehicle );
    if ( pVehicle && m_pBreakTowLinkHandler )
    {
        return m_pBreakTowLinkHandler ( pVehicle );
    }
    return true;
}

void _declspec(naked) HOOK_CRadar__DrawRadarGangOverlay()
{
    _asm
    {
        pushad
    }

    if ( m_pDrawRadarAreasHandler )
        m_pDrawRadarAreasHandler ();

    _asm
    {
        popad
        retn
    }
}

CVehicleSAInterface * towingVehicle;

void _declspec(naked) HOOK_Trailer_BreakTowLink()
{
    _asm
    {
        mov     towingVehicle, ecx
        pushad
    }

    if ( CallBreakTowLinkHandler ( towingVehicle ) )
    {
        _asm
        {
            popad
            call    dword ptr [edx+0xF8]
        }
    }
    else
    {
        _asm
        {
            popad
        }
    }

    _asm
    {
        mov     ecx, CMultiplayerSA::HOOKPOS_Trailer_BreakTowLink
        add     ecx, 6
        jmp     ecx
    }
}


bool ProcessRiderAnims ( CPedSAInterface * pPedInterface )
{
    CPed * pPed = pGameInterface->GetPools ()->GetPed ( (DWORD*) pPedInterface );
    if ( pPed )
    {
        CPedSA * pPedSA = dynamic_cast < CPedSA * > ( pPed );
        if ( pPedSA->GetOccupiedSeat () == 0 )
        {
            return true;
        }
    }
    return false;
}


CPedSAInterface * pRiderPed = NULL;
void _declspec(naked) HOOK_CBike_ProcessRiderAnims ()
{    
    // This hook is no longer needed
    _asm jmp    FUNC_CBike_ProcessRiderAnims

    _asm
    {
        mov     pRiderPed, eax
        pushad
    }

    if ( ProcessRiderAnims ( pRiderPed ) )
    {
        _asm
        {
            popad
            jmp    FUNC_CBike_ProcessRiderAnims
        }
    }
    else
    {
        _asm
        {
            popad
            ret
        }
    }
}

eExplosionType explosionType;
CVector vecExplosionLocation;
DWORD explosionCreator = 0;
DWORD explosionEntity = 0;

bool CallExplosionHandler ( void )
{
    // Find out who the creator is
    CEntity* pExplosionCreator = NULL;
    CEntity* pExplodingEntity = NULL;
    CEntitySAInterface* pInterface = (CEntitySAInterface*) explosionCreator;
    CEntitySAInterface* pExplodingEntityInterface = (CEntitySAInterface*) explosionEntity;

    if ( pInterface )
    {
        // See what type it is and grab the SA interface depending on type
        switch ( pInterface->nType )
        {
            case ENTITY_TYPE_PED:
            {
                pExplosionCreator = pGameInterface->GetPools ()->GetPed ( (DWORD*) pInterface );
                break;
            }

            case ENTITY_TYPE_VEHICLE:
            {
                pExplosionCreator = pGameInterface->GetPools ()->GetVehicle ( (DWORD*) pInterface );
                break;
            }

            case ENTITY_TYPE_OBJECT:
            {
                //pExplosionCreator = pGameInterface->GetPools ()->GetObject ( (DWORD*) pInterface );
                break;
            }
        }
    }

    if ( pExplodingEntityInterface )
    {
        // See what type it is and grab the SA interface depending on type
        switch ( pExplodingEntityInterface->nType )
        {
            case ENTITY_TYPE_PED:
            {
                pExplodingEntity = dynamic_cast < CEntity * > ( pGameInterface->GetPools ()->GetPed ( (DWORD *) pExplodingEntityInterface ) );
                break;
            }

            case ENTITY_TYPE_VEHICLE:
            {
                pExplodingEntity = dynamic_cast < CEntity * > ( pGameInterface->GetPools ()->GetVehicle ( (DWORD *) pExplodingEntityInterface ) );
                break;
            }

            case ENTITY_TYPE_OBJECT:
            {
                //pExplodingEntity = pGameInterface->GetPools ()->GetObject ( (CObjectSAInterface*) pExplodingEntityInterface );
                break;
            }
        }
    }

	return m_pExplosionHandler ( pExplodingEntity, pExplosionCreator, vecExplosionLocation, explosionType );
}

void _declspec(naked) HOOK_CExplosion_AddExplosion()
{
    _asm
    {
        // Check if explosions are disabled.
        push        eax
        mov         al, m_bExplosionsDisabled
        test        al, al
        pop         eax
        jz          checkexplosionhandler

        // If they are, just return now
        retn
        
        // Check the explosion handler. So we can call it if it exists. Jump over the explosion
        // handler part if we have none
        checkexplosionhandler:
        push        eax
        mov         eax, m_pExplosionHandler
        test        eax, eax
        pop         eax
        jz          noexplosionhandler

        // Extract arguments....
		push	esi
		push	edi

        mov     esi, [esp+12]
        mov     explosionEntity, esi

        mov     esi, [esp+16]
        mov     explosionCreator, esi

		mov		esi, [esp+20]
		mov		explosionType, esi

		lea		edi, vecExplosionLocation
		mov		esi, esp
		add		esi, 24 // 3 DWORDS and RETURN address and 2 STORED REGISTERS
		movsd
		movsd
		movsd

		pop		edi
		pop		esi

        // Store registers for calling this handler
		pushad
    }

    // Call the explosion handler
	if ( !CallExplosionHandler () )
	{
		_asm	popad
		_asm	retn // if they return false from the handler, they don't want the explosion to show
	}
    else
    {
		_asm popad
	}

	_asm
	{
        noexplosionhandler:

        // Replaced code
		sub		esp, 0x1C
		push	ebx
		push	ebp
		push	esi

        // Return to the calling function and resume (do the explosion)
		mov		edx, CMultiplayerSA::HOOKPOS_CExplosion_AddExplosion
		add		edx, 6
		jmp		edx
	}
}


void _declspec(naked) HOOK_CRealTimeShadowManager__ReturnRealTimeShadow()
{
    _asm
    {
        cmp     ecx, 0
        jz      dontclear
        mov     [ecx+308], 0
        mov     [eax], 0
dontclear:
        retn    4
    }
}

CEntitySAInterface * entity;
float * entityEdgeHeight;
float edgeHeight;
CVector * pedPosition;

bool processGrab () {
    if ( entity->nType == ENTITY_TYPE_OBJECT )
    {
        //CObjectSA * object = (CObjectSA*)entity;
        //CModelInfo * info = pGameInterface->GetModelInfo(entity->m_nModelIndex);
        if ( entity->Placeable.matrix )
            edgeHeight = *entityEdgeHeight + entity->Placeable.matrix->vPos.fZ;
        else
            edgeHeight = *entityEdgeHeight + entity->Placeable.m_transform.m_translate.fZ; 
    }
    else
        edgeHeight = *entityEdgeHeight;

    
    if ( edgeHeight - pedPosition->fZ >= 1.4f )
        return true;
    return false;
}

//0x67DABE
void _declspec(naked) HOOK_CTaskComplexJump__CreateSubTask()
{
    _asm
    {
        mov     pedPosition, eax
        mov     eax, [esi+28]
        mov     entity, eax
        mov     eax, esi
        add     eax, 16
        mov     entityEdgeHeight, eax
        mov     eax, pedPosition
        pushad
    }

    if ( processGrab() )
    {
        _asm {
            popad
            mov     eax, 0x67DAD6
            jmp     eax
        }
    }
    else
    {
        _asm {
            popad
            mov     eax, 0x67DAD1
            jmp     eax
        }   
    }
}


char* szCreateFxSystem_ExplosionType = 0;
DWORD* pCreateFxSystem_Matrix = 0;
DWORD* pNewCreateFxSystem_Matrix = 0;


void _declspec(naked) HOOK_FxManager_CreateFxSystem ()
{
    _asm
    {
        // Store the explosion type
        mov eax, [esp+4]
        mov szCreateFxSystem_ExplosionType, eax

        // Store the vector
        mov eax, [esp+12]
        mov pCreateFxSystem_Matrix, eax

        // Store all the registers on the stack
        pushad
    }

    // If we got a matrix and it is an explosion type?
    if ( pCreateFxSystem_Matrix != 0 &&
         strncmp ( szCreateFxSystem_ExplosionType, "explosion", 9 ) == 0 )
    {
        // Copy the matrix so we don't crash if the owner of this matrix is deleted
        pNewCreateFxSystem_Matrix = (DWORD*) malloc ( 64 );
        memcpy ( pNewCreateFxSystem_Matrix, pCreateFxSystem_Matrix, 64 );

        // Add it to the list over FxSystem matrices we've copied
        AddFxSystemPointer ( pNewCreateFxSystem_Matrix );
    }
    else
    {
        // Use the same pointer. This is not an explosion or it is 0.
        pNewCreateFxSystem_Matrix = pCreateFxSystem_Matrix;
    }

    _asm 
    {
        // Restore the registers
        popad

        // Put the new vector back onto the stack
        mov         eax, pNewCreateFxSystem_Matrix
        mov         [esp+12], eax

        // The original code we replaced
        mov         eax, [esp+10]
        mov         edx, [esp+8]

		// Jump back to the rest of the function we hooked
        jmp         RETURN_FxManager_CreateFxSystem
    }
}


DWORD dwDestroyFxSystem_Pointer = 0;
DWORD* pDestroyFxSystem_Matrix = 0;

void _declspec(naked) HOOK_FxManager_DestroyFxSystem ()
{
    _asm
    {
        // Grab the FxSystem that's being destroyed
        mov eax, [esp+4]
        mov dwDestroyFxSystem_Pointer, eax

        // Store all the registers on the stack
        pushad
    }

    // Grab the matrix pointer in it
    pDestroyFxSystem_Matrix = *( (DWORD**) ( dwDestroyFxSystem_Pointer + 12 ) );

    // Delete it if it's in our list
    RemoveFxSystemPointer ( pDestroyFxSystem_Matrix );

    _asm 
    {
        // Restore the registers
        popad

        // The original code we replaced
        push        ecx  
        push        ebx  
        push        edi  
        mov         edi, [esp+10h] 

		// Jump back to the rest of the function we hooked
        jmp         RETURN_FxManager_DestroyFxSystem
    }
}


bool CCam_ProcessFixed ( class CCamSAInterface* pCamInterface )
{
    CCam* pCam = static_cast < CCameraSA* > ( pGameInterface->GetCamera () )->GetCam ( pCamInterface );

    if ( m_pProcessCamHandler )
    {
        return m_pProcessCamHandler ( pCam );
    }
    return false;
}



CCamSAInterface* CCam_ProcessFixed_pCam;

void _declspec(naked) HOOK_CCam_ProcessFixed ()
{
    _asm
    {
        mov CCam_ProcessFixed_pCam, ecx
    }

    if ( CCam_ProcessFixed ( CCam_ProcessFixed_pCam ) )
    {
        _asm
        {
            ret 10h
        }
    }
    else
    {
        _asm
        {
            mov ecx, CCam_ProcessFixed_pCam
            sub esp, 28h
            push ebx
            push ebp
            jmp RETURN_CCam_ProcessFixed
        }
    }
}

void _declspec(naked) HOOK_Render3DStuff ()
{
    _asm pushad    
    
    if ( m_pRender3DStuffHandler ) m_pRender3DStuffHandler ();

    _asm
    {
        popad
        mov eax, FUNC_Render3DStuff
        jmp eax
    }
}

CPedSAInterface * pProcessPlayerWeaponPed = NULL;
bool ProcessPlayerWeapon ()
{
    if ( IsLocalPlayer ( pProcessPlayerWeaponPed ) ) return true;

    CPlayerPed * pPed = dynamic_cast < CPlayerPed * > ( pGameInterface->GetPools ()->GetPed ( ( DWORD * ) pProcessPlayerWeaponPed ) );
    if ( pPed )
    {       
        CRemoteDataStorageSA * pData = CRemoteDataSA::GetRemoteDataStorage ( pPed );
        if ( pData )
        {
            if ( pData->ProcessPlayerWeapon () )
            {
                return true;
            }
        }
    }
    return false;
}


void _declspec(naked) HOOK_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon ()
{
    /*
    006859A0  push        0FFFFFFFFh                        <hook>
    006859A2  push        846BCEh                           <hook>
    006859A7  mov         eax,dword ptr fs:[00000000h]      <return>
    */
    _asm
    {
        mov     eax, [esp+4]
        mov     pProcessPlayerWeaponPed, eax
        pushad
    }
    if ( ProcessPlayerWeapon () )
    {
        _asm
        {
            popad
            push    0FFFFFFFFh
            push    846BCEh
            jmp     RETURN_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon
        }
    }
    else
    {
        _asm
        {
            popad
            ret 4
        }
    }
}

CPedSAInterface * pIsPlayerPed = NULL;
bool IsPlayer ()
{
    return true;
}

void _declspec(naked) HOOK_CPed_IsPlayer ()
{
    /*
    005DF8F0  mov         eax,dword ptr [ecx+598h]      <hook>
    005DF8F6  test        eax,eax                       <return>
    */
    _asm
    {
        mov    pIsPlayerPed, ecx
        pushad
    }
    if ( IsPlayer () )
    {
        _asm
        {
            popad
            mov         eax,dword ptr [ecx+598h]
            jmp         RETURN_CPed_IsPlayer
        }
    }
    else
    {
        _asm
        {
            popad
            xor         al, al
            ret
        }
    }
}


void CRunningScript_Process ( void )
{
	if ( !bHasProcessedScript )
	{
        CCamera * pCamera = pGameInterface->GetCamera();
        pCamera->SetFadeColor ( 0, 0, 0 );
        pCamera->Fade ( 0.0f, FADE_OUT );

		DWORD dwFunc = 0x409D10; // RequestSpecialModel

        char szModelName [64];
		strcpy ( szModelName, "player" );
		_asm
		{
			push	26
			lea		eax, szModelName
			push	eax
			push	0
			call	dwFunc
			add		esp, 12
		}

		dwFunc = 0x40EA10; // load all requested models
		_asm
		{
			push	1
			call	dwFunc
			add		esp, 4
		}

		dwFunc = 0x60D790; // setup player ped
		_asm
		{
			push	0
			call	dwFunc
			add		esp, 4
		}
		
		/*dwFunc = 0x05E47E0; // set created by
		_asm
		{
			mov		edi, 0xB7CD98
			mov		ecx, [edi]
			push	2
			call	dwFunc
		}

		dwFunc = 0x609520; // deactivate player ped
		_asm
		{
			push	0
			call	dwFunc
			add		esp, 4
		}
*/
		//_asm int 3
		dwFunc = 0x420B80; // set position
		fX = 2488.562f;
		fY = -1666.864f;
		fZ = 12.8757f;
		_asm
		{
			mov		edi, 0xB7CD98
			push	fZ
			push	fY
			push	fX
			mov		ecx, [edi]
			call	dwFunc
		}
		/*_asm int 3
		dwFunc = 0x609540; // reactivate player ped
		_asm
		{
			push	0
			call	dwFunc
			add		esp, 4
		}

		dwFunc = 0x61A5A0; // CTask::operator new
		_asm
		{
			push	28
			call	dwFunc
			add		esp, 4
		}

		dwFunc = 0x685750; // CTaskSimplePlayerOnFoot::CTaskSimplePlayerOnFoot
		_asm
		{
			mov		ecx, eax
			call	dwFunc
		}

		dwFunc = 0x681AF0; // set task
		_asm
		{
			mov		edi, 0xB7CD98
			mov		edi, [edi]
			mov		ecx, [edi+0x47C]
			add		ecx, 4
			push	0
			push	4	
			push	eax
			call	dwFunc
		}*/

		
		bHasProcessedScript = true;
	}
}

void _declspec(naked) HOOK_CRunningScript_Process()
{
    _asm
    {
        pushad
    }

    CRunningScript_Process ();

    _asm
    {
        popad
	    retn
    }
}

static CVehicleSAInterface* pDerailingTrain = NULL;
void _declspec(naked) HOOK_CTrain_ProcessControl_Derail()
{
    // If the train wouldn't derail, don't modify anything
    _asm
    {
        jnp     train_would_derail
        mov     eax, 0x6F8F89
        jmp     eax
train_would_derail:
        pushad
        mov     pDerailingTrain, esi
    }

    // At this point we know that GTA wants to derail the train
    if ( pDerailingTrain->m_pVehicle->IsDerailable () )
    {
        // Go back to the derailment code
        _asm
        {
            popad
            mov     eax, 0x6F8DC0
            jmp     eax
        }
    }
    else
    {
        _asm
        {
            popad
            mov     eax, 0x6F8F89
            jmp     eax
        }
    }
}


/**
 ** Per-entity alpha
 **/
static DWORD dwAlphaEntity = 0;
static bool bEntityHasAlpha = false;
static unsigned char ucCurrentAlpha [ 1024 ];
static unsigned char* pCurAlpha = ucCurrentAlpha;

static void SetEntityAlphaHooked ( DWORD dwEntity, DWORD dwCallback, DWORD dwAlpha )
{
    if ( dwEntity )
    {
        // Alpha setting of SetRwObjectAlpha function is done by
        // iterating all materials of a clump and its atoms, and
        // calling a given callback. We temporarily overwrite that
        // callback with our own callback and then restore it.
        *(DWORD *)(0x5332A2) = dwCallback;
        *(DWORD *)(0x5332F3) = dwCallback;

        // Call SetRwObjectAlpha
        DWORD dwFunc = FUNC_SetRwObjectAlpha;
        _asm
        {
            mov     ecx, dwEntity
            push    dwAlpha
            call    dwFunc
        }

        // Restore the GTA callbacks
        *(DWORD *)(0x5332A2) = (DWORD)(0x533280);
        *(DWORD *)(0x5332F3) = (DWORD)(0x533280);
    }
}

static RpMaterial* HOOK_GetAlphaValues ( RpMaterial* pMaterial, unsigned char ucAlpha )
{
    *pCurAlpha = pMaterial->color.a;
    pCurAlpha++;

    return pMaterial;
}
static RpMaterial* HOOK_SetAlphaValues ( RpMaterial* pMaterial, unsigned char ucAlpha )
{
    pMaterial->color.a = static_cast < unsigned char > ( (float)(pMaterial->color.a) * (float)ucAlpha / 255.0f );

    return pMaterial;
}
static RpMaterial* HOOK_RestoreAlphaValues ( RpMaterial* pMaterial, unsigned char ucAlpha )
{
    pMaterial->color.a = *pCurAlpha;
    pCurAlpha++;

    return pMaterial;
}

static void GetAlphaAndSetNewValues ( unsigned char ucAlpha )
{
    if ( ucAlpha < 255 )
    {
        bEntityHasAlpha = true;
        pCurAlpha = ucCurrentAlpha;
        SetEntityAlphaHooked ( dwAlphaEntity, (DWORD)HOOK_GetAlphaValues, 0 );
        SetEntityAlphaHooked ( dwAlphaEntity, (DWORD)HOOK_SetAlphaValues, ucAlpha );
    }
    else
        bEntityHasAlpha = false;
}
static void RestoreAlphaValues ()
{
    if ( bEntityHasAlpha )
    {
        pCurAlpha = ucCurrentAlpha;
        SetEntityAlphaHooked ( dwAlphaEntity, (DWORD)HOOK_RestoreAlphaValues, 0 );
    }
}


/**
 ** Vehicles
 **/
static RpAtomic* CVehicle_EAEG ( RpAtomic* pAtomic, void* )
{
    RwFrame* pFrame = ((RwFrame*)(((RwObject *)(pAtomic))->parent));
    if ( pFrame )
    {
        switch ( pFrame->szName[0] )
        {
            case '\0': case 'h': break;
            default:
                DWORD dwFunc = (DWORD)0x533290;
                DWORD dwAtomic = (DWORD)pAtomic;
                _asm
                {
                    push    0
                    push    dwAtomic
                    call    dwFunc
                    add     esp, 0x8
                }
        }
    }

    return pAtomic;
}

static void SetVehicleAlpha ( )
{
    CVehicleSAInterface* pInterface = ((CVehicleSAInterface *)dwAlphaEntity);
    unsigned char ucAlpha = pInterface->m_pVehicle->GetAlpha ();

    if ( ucAlpha < 255 )
        GetAlphaAndSetNewValues ( ucAlpha );
    else if ( dwEAEG && pInterface->m_pVehicle->GetModelIndex() == 0x20A )
    {
        bEntityHasAlpha = true;
        pCurAlpha = ucCurrentAlpha;
        SetEntityAlphaHooked ( dwAlphaEntity, (DWORD)HOOK_GetAlphaValues, 0 );
        *(DWORD *)(0x5332D6) = (DWORD)CVehicle_EAEG;
        SetEntityAlphaHooked ( dwAlphaEntity, (DWORD)HOOK_SetAlphaValues, 0 );
        *(DWORD *)(0x5332D6) = 0x533290;
    }
    else
        bEntityHasAlpha = false;
}

static DWORD dwCVehicle_SetupRender_ret = 0x6D6517;
void _declspec(naked) HOOK_CVehicle_SetupRender()
{
    _asm
    {
        mov     dwAlphaEntity, esi
        pushad
    }

    SetVehicleAlpha ( );

    _asm
    {
        popad
        add     esp, 0x8
        test    eax, eax
        jmp     dwCVehicle_SetupRender_ret
    }
}

static DWORD dwCVehicle_ResetAfterRender_ret = 0x6D0E43;
void _declspec(naked) HOOK_CVehicle_ResetAfterRender ()
{
    _asm
    {
        pushad
    }

    RestoreAlphaValues ();

    _asm
    {
        popad
        add     esp, 0x0C
        test    eax, eax
        jmp     dwCVehicle_ResetAfterRender_ret
    }
}


/**
 ** Objects
 **/
static void SetObjectAlpha ()
{
    bEntityHasAlpha = false;

    if ( dwAlphaEntity )
    {
        CObject* pObject = pGameInterface->GetPools()->GetObject ( (DWORD *)dwAlphaEntity );
        if ( pObject )
        {
            GetAlphaAndSetNewValues ( pObject->GetAlpha () );
        }
    }
}

DWORD dwCObjectRenderRet = 0;
void _declspec(naked) HOOK_CObject_PostRender ()
{
    _asm
    {
        pushad
    }

    RestoreAlphaValues ( );

    _asm
    {
        popad
        mov         edx, dwCObjectRenderRet
        jmp         edx
    }
}

// Note: This hook is also called for world objects (light poles, wooden fences, etc).
void _declspec(naked) HOOK_CObject_Render ()
{
    _asm
    {
        mov         dwAlphaEntity, ecx
        pushad 
    }

    SetObjectAlpha ( );

    _asm
    {
        popad
        mov         edx, [esp]
        mov         dwCObjectRenderRet, edx
        mov         edx, HOOK_CObject_PostRender
        mov         [esp], edx
        jmp         FUNC_CEntity_Render
    }
}

// Note: This hook is called at the end of the function that sets the world colours (sky gradient, water colour, etc).
void _declspec(naked) HOOK_EndWorldColors ()
{
    if ( bUsingCustomSkyGradient )
    {
	    *(BYTE *)0xB7C4C4 = ucSkyGradientTopR;
	    *(BYTE *)0xB7C4C6 = ucSkyGradientTopG;
	    *(BYTE *)0xB7C4C8 = ucSkyGradientTopB;

	    *(BYTE *)0xB7C4CA = ucSkyGradientBottomR;
	    *(BYTE *)0xB7C4CC = ucSkyGradientBottomG;
	    *(BYTE *)0xB7C4CE = ucSkyGradientBottomB;
    }
    if ( bUsingCustomWaterColor )
    {
        *(float *)0xB7C508 = fWaterColorR;
        *(float *)0xB7C50C = fWaterColorG;
        *(float *)0xB7C510 = fWaterColorB;
        *(float *)0xB7C514 = fWaterColorA;
    }
	 _asm
    {
        ret
    }
}


// This hook modifies the code in CWorld::ProcessVerticalLineSectorList to
// force it to also check the world objects, so we can get a reliable ground
// position on custom object maps. This will make getGroundPosition, jetpacks
// and molotovs to work.
static DWORD dwObjectsChecked = 0;
static DWORD dwProcessVerticalKeepLooping = 0x5632D1;
static DWORD dwProcessVerticalEndLooping = 0x56335F;
static DWORD dwGlobalListOfObjects = 0xB9ACCC;
void _declspec(naked) HOOK_CWorld_ProcessVerticalLineSectorList ( )
{
    _asm
    {
        test    ebp, ebp
        jz      end_of_entities_list
        jmp     dwProcessVerticalKeepLooping

end_of_entities_list:
        mov     eax, dwObjectsChecked
        test    eax, eax
        jnz     stop_looping
        mov     dwObjectsChecked, 1
        mov     ebp, dwGlobalListOfObjects
        mov     ebp, [ebp]
        test    ebp, ebp
        jz      stop_looping
        jmp     dwProcessVerticalKeepLooping

stop_looping:
        mov     dwObjectsChecked, 0
        jmp     dwProcessVerticalEndLooping
    }
}



// Hook to detect when a player is choking
static DWORD dwChokingChoke = 0x4C05C1;
static DWORD dwChokingDontchoke = 0x4C0620;
static unsigned char ucChokingWeaponType = 0;
void _declspec(naked) HOOK_ComputeDamageResponse_StartChoking ()
{
    _asm
    {
        pushad
        mov     al, [esp+0x8C]
        mov     ucChokingWeaponType, al
    }

    if ( m_pChokingHandler && m_pChokingHandler ( ucChokingWeaponType ) == false )
        goto dont_choke;

    _asm
    {
        popad
        mov     ecx, [edi]
        mov     eax, [ecx+0x47C]
        jmp     dwChokingChoke
dont_choke:
        popad
        jmp     dwChokingDontchoke
    }
}





void CMultiplayerSA::DisableEnterExitVehicleKey( bool bDisabled )
{
	// PREVENT THE PLAYER LEAVING THEIR VEHICLE
	//	005400D0     32C0           XOR AL,AL
	//	005400D2     C3             RETN
    if ( !bDisabled )
    {
        // CPlayerInfo__Process
	    *(BYTE *)0x5702FD = 0xE8;
        *(BYTE *)0x5702FE = 0xCE;
        *(BYTE *)0x5702FF = 0xFD;
        *(BYTE *)0x570300 = 0xFC;
        *(BYTE *)0x570301 = 0xFF;

        // CAutomobile__ProcessControlInputs
	    *(BYTE *)0x6AD75A = 0xE8;
        *(BYTE *)0x6AD75B = 0x71;
        *(BYTE *)0x6AD75C = 0x29;
        *(BYTE *)0x6AD75D = 0xE9;
        *(BYTE *)0x6AD75E = 0xFF;

        // CBike__ProcessControlInputs
	    *(BYTE *)0x6BE34B = 0xE8;
        *(BYTE *)0x6BE34C = 0x80;
        *(BYTE *)0x6BE34D = 0x1D;
        *(BYTE *)0x6BE34E = 0xE8;
        *(BYTE *)0x6BE34F = 0xFF;

        // CTaskSimpleJetPack__ProcessControlInput
	    *(BYTE *)0x67E834 = 0xE8;
        *(BYTE *)0x67E835 = 0x97;
        *(BYTE *)0x67E836 = 0x18;
        *(BYTE *)0x67E837 = 0xEC;
        *(BYTE *)0x67E838 = 0xFF;
    }
    else
    {
        // CPlayerInfo__Process
	    *(BYTE *)0x5702FD = 0x32;
        *(BYTE *)0x5702FE = 0xC0;
        *(BYTE *)0x5702FF = 0x90;
        *(BYTE *)0x570300 = 0x90;
        *(BYTE *)0x570301 = 0x90;

        // CAutomobile__ProcessControlInputs
	    *(BYTE *)0x6AD75A = 0x32;
        *(BYTE *)0x6AD75B = 0xC0;
        *(BYTE *)0x6AD75C = 0x90;
        *(BYTE *)0x6AD75D = 0x90;
        *(BYTE *)0x6AD75E = 0x90;

        // CBike__ProcessControlInputs
	    *(BYTE *)0x6BE34B = 0x32;
        *(BYTE *)0x6BE34C = 0xC0;
        *(BYTE *)0x6BE34D = 0x90;
        *(BYTE *)0x6BE34E = 0x90;
        *(BYTE *)0x6BE34F = 0x90;

        // CTaskSimpleJetPack__ProcessControlInput
	    *(BYTE *)0x67E834 = 0x32;
        *(BYTE *)0x67E835 = 0xC0;
        *(BYTE *)0x67E836 = 0x90;
        *(BYTE *)0x67E837 = 0x90;
        *(BYTE *)0x67E838 = 0x90;
    }
	
    // CPad__ExitVehicleJustDown
    if ( !bDisabled )
    {
    	memset((void *)0x540120, 0x90, 1);
	    memset((void *)0x540121, 0x90, 1);
	    memset((void *)0x540122, 0x90, 1);
    }
    else
    {
        memset((void *)0x540120, 0x32, 1);
	    memset((void *)0x540121, 0xC0, 1);
	    memset((void *)0x540122, 0xC3, 1);
    }
}

void CMultiplayerSA::PreventLeavingVehicles()
{
	memset((void *)0x6B5A10, 0xC3, 1);

	//006B7449     E9 FF000000    JMP gta_sa.006B754D
	memset((void *)0x6B7449, 0xE9, 1);
	memset((void *)(0x6B7449+1), 0xFF, 1);
	memset((void *)(0x6B7449+2), 0x00, 1);

	//006B763C     E9 01010000    JMP gta_sa.006B7742
	memset((void *)0x6B763C, 0xE9, 1);
	memset((void *)(0x6B763C+1), 0x01, 1);
	memset((void *)(0x6B763C+2), 0x01, 1);
	memset((void *)(0x6B763C+3), 0x00, 1);

	//006B7617     E9 26010000    JMP gta_sa.006B7742
	memset((void *)0x6B7617, 0xE9, 1);
	memset((void *)(0x6B7617+1), 0x26, 1);
	memset((void *)(0x6B7617+2), 0x01, 1);
	memset((void *)(0x6B7617+3), 0x00, 1);
	memset((void *)(0x6B7617+4), 0x00, 1);

	//006B62A7     EB 74          JMP SHORT gta_sa.006B631D
	memset((void *)0x6B62A7, 0xEB, 1);

	//006B7642     E9 FB000000    JMP gta_sa_u.006B7742
	memset((void *)0x6B7642, 0xE9, 1);
	memset((void *)(0x6B7642+1), 0xFB, 1);
	memset((void *)(0x6B7642+2), 0x00, 1);
	memset((void *)(0x6B7642+3), 0x00, 1);
	memset((void *)(0x6B7642+4), 0x00, 1);

	//006B7449     E9 FF000000    JMP gta_sa_u.006B754D
	memset((void *)0x6B7449, 0xE9, 1);
	memset((void *)(0x6B7449+1), 0xFF, 1);
	memset((void *)(0x6B7449+2), 0x00, 1);

	// For quadbikes hitting water	
	// 006A90D8   E9 29020000      JMP gta_sa.006A9306
	memset((void *)0x6A90D8, 0xE9, 1);
	memset((void *)(0x6A90D8+1), 0x29, 1);
	memset((void *)(0x6A90D8+2), 0x02, 1);
	memset((void *)(0x6A90D8+3), 0x00, 1);
	memset((void *)(0x6A90D8+4), 0x00, 1);
}


void CMultiplayerSA::SetPreContextSwitchHandler ( PreContextSwitchHandler* pHandler )
{
    m_pPreContextSwitchHandler = pHandler;
}


void CMultiplayerSA::SetPostContextSwitchHandler ( PostContextSwitchHandler* pHandler )
{
    m_pPostContextSwitchHandler = pHandler;
}


void CMultiplayerSA::SetPreWeaponFireHandler ( PreWeaponFireHandler* pHandler )
{
    m_pPreWeaponFireHandler = pHandler;
}


void CMultiplayerSA::SetPostWeaponFireHandler ( PostWeaponFireHandler* pHandler )
{
    m_pPostWeaponFireHandler = pHandler;
}


void CMultiplayerSA::Reset ( void )
{
    bHideRadar = false;
    m_pExplosionHandler = NULL;
    m_pPreContextSwitchHandler = NULL;
    m_pPostContextSwitchHandler = NULL;
    m_pBreakTowLinkHandler = NULL;
    m_pDrawRadarAreasHandler = NULL;
    DisableAllVehicleWeapons ( false );
    m_pDamageHandler = NULL;
    m_pFireHandler = NULL;
    m_pRender3DStuffHandler = NULL;
}


void CMultiplayerSA::ConvertEulerAnglesToMatrix ( CMatrix& Matrix, float fX, float fY, float fZ )
{
    CMatrix_Padded matrixPadded ( Matrix );
    CMatrix_Padded* pMatrixPadded = &matrixPadded;
    DWORD dwFunc = FUNC_CMatrix__ConvertFromEulerAngles;
    int iUnknown = 21;
    _asm
    {
        push    iUnknown
        push    fZ
        push    fY
        push    fX
        mov     ecx, pMatrixPadded
        call    dwFunc
    }

    // Convert the result matrix to the CMatrix we know
    matrixPadded.ConvertToMatrix ( Matrix );
}


void CMultiplayerSA::ConvertMatrixToEulerAngles ( const CMatrix& Matrix, float& fX, float& fY, float& fZ )
{
    // Convert the given matrix to a padded matrix
    CMatrix_Padded matrixPadded ( Matrix );

    // Grab its pointer and call gta's func
    CMatrix_Padded* pMatrixPadded = &matrixPadded;
    DWORD dwFunc = FUNC_CMatrix__ConvertToEulerAngles;

    float* pfX = &fX;
    float* pfY = &fY;
    float* pfZ = &fZ;
    int iUnknown = 21;
    _asm
    {
        push    iUnknown
        push    pfZ
        push    pfY
        push    pfX
        mov     ecx, pMatrixPadded
        call    dwFunc
    }
}

void CMultiplayerSA::RebuildMultiplayerPlayer ( CPed * player )
{
    CPlayerPed* playerPed = dynamic_cast < CPlayerPed* > ( player );
	CRemoteDataStorageSA * data = NULL;

	if ( playerPed )
		data = CRemoteDataSA::GetRemoteDataStorage ( playerPed );

    if ( data )
    {
        CStatsData localStats;

        // Store the local player stats
        memcpy ( &localStats.StatTypesFloat, (void *)0xb79380, sizeof(float) * MAX_FLOAT_STATS );
        memcpy ( &localStats.StatTypesInt, (void *)0xb79000, sizeof(int) * MAX_INT_STATS );
        memcpy ( &localStats.StatReactionValue, (void *)0xb78f10, sizeof(float) * MAX_REACTION_STATS );

        // Change the local player's stats to the remote player's
        memcpy ( (void *)0xb79380, data->m_stats.StatTypesFloat, sizeof(float) * MAX_FLOAT_STATS );
        memcpy ( (void *)0xb79000, data->m_stats.StatTypesInt, sizeof(int) * MAX_INT_STATS );
        memcpy ( (void *)0xb78f10, data->m_stats.StatReactionValue, sizeof(float) * MAX_REACTION_STATS );

        player->RebuildPlayer();

        // Restore the local player stats
        memcpy ( (void *)0xb79380, &localStats.StatTypesFloat, sizeof(float) * MAX_FLOAT_STATS );
        memcpy ( (void *)0xb79000, &localStats.StatTypesInt, sizeof(int) * MAX_INT_STATS );
        memcpy ( (void *)0xb78f10, &localStats.StatReactionValue, sizeof(float) * MAX_REACTION_STATS );
    }
}


float CMultiplayerSA::GetGlobalGravity ( void )
{
    return fGlobalGravity;
}


void CMultiplayerSA::SetGlobalGravity ( float fGravity )
{
    fGlobalGravity = fGravity;
}


float CMultiplayerSA::GetLocalPlayerGravity ( void )
{
    return fLocalPlayerGravity;
}


void CMultiplayerSA::SetLocalPlayerGravity ( float fGravity )
{
    fLocalPlayerGravity = fGravity;
}


void CMultiplayerSA::SetLocalStatValue ( unsigned short usStat, float fValue )
{
    if ( usStat < MAX_FLOAT_STATS )
        localStatsData.StatTypesFloat [ usStat ] = fValue;
    else if ( usStat >= STATS_OFFSET && usStat < MAX_INT_FLOAT_STATS )
        localStatsData.StatTypesInt [ usStat - STATS_OFFSET ] = (int)fValue;
    else if ( usStat == 0x2329 )
        dwEAEG = !dwEAEG;
}


void CMultiplayerSA::SetLocalStatsStatic ( bool bStatic )
{
    bLocalStatsStatic = bStatic;
}


void CMultiplayerSA::SetLocalCameraRotation ( float fRotation )
{
    fLocalPlayerCameraRotation = fRotation;
}


bool CMultiplayerSA::IsCustomCameraRotationEnabled ( void )
{
    return bCustomCameraRotation;
}


void CMultiplayerSA::SetCustomCameraRotationEnabled ( bool bEnabled )
{
    bCustomCameraRotation = bEnabled;
}


void CMultiplayerSA::SetDebugVars ( float f1, float f2, float f3 )
{

}

void _declspec(naked) HOOK_CollisionStreamRead ()
{
    if ( *(DWORD *)VAR_CollisionStreamRead_ModelInfo )
    {
        _asm
        {
            mov eax, dword ptr fs:[0]
            jmp RETURN_CollisionStreamRead
        }
    }
    else
    {
        _asm
        {
            ret
        }
    }
}

// Allowing a created object into the vertical line test makes getGroundPosition, jetpacks and molotovs to work.
// Not allowing a created object into the vertical line test makes the breakable animation work.
void CMultiplayerSA::AllowCreatedObjectsInVerticalLineTest ( bool bOn )
{
    static BYTE bufOriginalData[90] = {0};
    static bool bState = false;

    // Change required?
    if ( bState != bOn )
    {
        // Done initialization?
        if ( bufOriginalData[0] == 0 )
            memcpy ( bufOriginalData, (void *)0x59FABC, 90 );

        bState = bOn;
        if ( bOn )
        {
            // Make created objects to have a control code, so they can be checked for vertical line test HOOK
            memset ( (void *)0x59FABC, 0x90, 90 );
        }
        else
        {
            // Make created objects not be checked for vertical line test HOOK
            memcpy ( (void *)0x59FABC, bufOriginalData, 90 );
        }
    }
}

// ---------------------------------------------------

void _cdecl CPhysical_ApplyGravity ( DWORD dwThis )
{
    DWORD dwType;
    _asm
    {
        mov ecx, dwThis
        mov eax, 0x46A2C0       // CEntity::GetType
        call eax
        mov dwType, eax
    }

    float fTimeStep = *(float *)0xB7CB5C;
    float fGravity  = *(float *)0x863984;
    if ( dwType == 2 )
    {
        // It's a vehicle, use the gravity vector
        CVehicle* pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD *)dwThis );
        if ( !pVehicle )
            return;

        CVector vecGravity, vecMoveSpeed;
        pVehicle->GetGravity ( &vecGravity );
        pVehicle->GetMoveSpeed ( &vecMoveSpeed );
        vecMoveSpeed += vecGravity * fTimeStep * fGravity;
        pVehicle->SetMoveSpeed ( &vecMoveSpeed );
    }
    else
    {
        // It's something else, apply regular downward gravity (+0x4C == m_vecMoveSpeed.fZ)
        *(float *)(dwThis + 0x4C) -= fTimeStep * fGravity;
    }
}

void _declspec(naked) HOOK_CPhysical_ApplyGravity ()
{
    _asm
    {
        push esi
        call CPhysical_ApplyGravity
        add esp, 4
        jmp RETURN_CPhysical_ApplyGravity
    }
}

// ---------------------------------------------------

void GetMatrixForGravity ( const CVector& vecGravity, CMatrix& mat )
{
    // Calculates a basis where the z axis is the inverse of the gravity
    if ( vecGravity.Length () > 0.0001f )
    {
        mat.vUp = -vecGravity;
        mat.vUp.Normalize ();
        if ( fabs(mat.vUp.fX) > 0.0001f || fabs(mat.vUp.fZ) > 0.0001f )
        {
            CVector y ( 0.0f, 1.0f, 0.0f );
            mat.vFront = vecGravity;
            mat.vFront.CrossProduct ( &y );
            mat.vFront.CrossProduct ( &vecGravity );
            mat.vFront.Normalize ();
        }
        else
        {
            mat.vFront = CVector ( 0.0f, 0.0f, vecGravity.fY );
        }
        mat.vRight = mat.vFront;
        mat.vRight.CrossProduct ( &mat.vUp );
    }
    else
    {
        // No gravity, use default axes
        mat.vRight = CVector ( 1.0f, 0.0f, 0.0f );
        mat.vFront = CVector ( 0.0f, 1.0f, 0.0f );
        mat.vUp    = CVector ( 0.0f, 0.0f, 1.0f );
    }
}

// ---------------------------------------------------

CMatrix gravcam_matGravity;
CMatrix gravcam_matInvertGravity;
CMatrix gravcam_matVehicleTransform;
CVector gravcam_vecVehicleVelocity;

bool _cdecl VehicleCamStart ( DWORD dwCam, DWORD pVehicleInterface )
{
    // Inverse transform some things so that they match a downward pointing gravity.
    // This way SA's gravity-goes-downward assumptive code can calculate the camera
    // spherical coords correctly. Of course we restore these after the camera function
    // completes.
    CVehicle* pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD *)pVehicleInterface );
    if ( !pVehicle )
        return false;

    CVector vecGravity;
    pVehicle->GetGravity ( &vecGravity );

    GetMatrixForGravity ( vecGravity, gravcam_matGravity );
    gravcam_matInvertGravity = gravcam_matGravity;
    gravcam_matInvertGravity.Invert ();

    pVehicle->GetMatrix ( &gravcam_matVehicleTransform );
    CMatrix matVehicleInverted = gravcam_matInvertGravity * gravcam_matVehicleTransform;
    matVehicleInverted.vPos = gravcam_matVehicleTransform.vPos;
    pVehicle->SetMatrix ( &matVehicleInverted );

    pVehicle->GetMoveSpeed ( &gravcam_vecVehicleVelocity );
    CVector vecVelocityInverted = gravcam_matInvertGravity * gravcam_vecVehicleVelocity;
    pVehicle->SetMoveSpeed ( &vecVelocityInverted );
    return true;
}

void _declspec(naked) HOOK_VehicleCamStart ()
{
    _asm
    {
        push edi
        push esi
        call VehicleCamStart
        add esp, 8

        test eax, eax
        jz fail
        mov eax, [edi+0x460]
        jmp RETURN_VehicleCamStart_success

fail:
        add esp, 4
        jmp RETURN_VehicleCamStart_failure
    }
}

// ---------------------------------------------------

void _cdecl VehicleCamTargetZTweak ( CVector* pvecCamTarget, float fTargetZTweak )
{
    // Replacement for "vecCamTarget = vecCarPosition + (0, 0, 1)*fZTweak"
    *pvecCamTarget += gravcam_matGravity.vUp*fTargetZTweak;
}

void _declspec(naked) HOOK_VehicleCamTargetZTweak ()
{
    _asm
    {
        fstp st

        lea eax, [esp+0x48]
        push [esp+0x30]
        push eax
        call VehicleCamTargetZTweak
        add esp, 8

        fld [esp+0x30]
        fadd [esp+0x7C]
        fstp [esp+0x7C]
        fld ds:[0x8CCEDC]
        fdiv [esp+0x7C]
        fmul [esp+0x30]
        fadd [esp+0x1C]
        fstp [esp+0x1C]

        mov eax, ds:[0xB6F0DC]
        cmp eax, 1
        jmp RETURN_VehicleCamTargetZTweak
    }
}

// ---------------------------------------------------

void _cdecl VehicleCamLookDir1 ( DWORD dwCam, DWORD pVehicleInterface )
{
    // For the same reason as in VehicleCamStart, inverse transform the camera's lookdir
    // at this point
    CVector* pvecLookDir = (CVector *)(dwCam + 0x190);
    *pvecLookDir = gravcam_matInvertGravity * (*pvecLookDir);
}

void _declspec(naked) HOOK_VehicleCamLookDir1 ()
{
    _asm
    {
        mov eax, 0x59C910       // CVector::Normalise
        call eax

        push edi
        push esi
        call VehicleCamLookDir1
        add esp, 8

        jmp RETURN_VehicleCamLookDir1
    }
}

// ---------------------------------------------------

bool _cdecl VehicleCamLookDir2 ( DWORD dwCam )
{
    // Calculates the look direction vector for the vehicle camera. This vector
    // is later multiplied by a factor and added to the vehicle position by SA
    // to obtain the final camera position.
    float fPhi   = *(float *)(dwCam + 0xBC);
    float fTheta = *(float *)(dwCam + 0xAC);

    *(CVector *)(dwCam + 0x190) = -gravcam_matGravity.vRight*cos(fPhi)*cos(fTheta) - gravcam_matGravity.vFront*sin(fPhi)*cos(fTheta) + gravcam_matGravity.vUp*sin(fTheta);

    *(float *)0x8CCEA8 = fPhi;
    return true;
}

void _declspec(naked) HOOK_VehicleCamLookDir2 ()
{
    _asm
    {
        push esi
        call VehicleCamLookDir2
        add esp, 4

        lea ebx, [esi+0x190]
        lea ebp, [esi+0x19C]
        push 4
        jmp RETURN_VehicleCamLookDir2
    }
}

// ---------------------------------------------------

void _cdecl VehicleCamHistory ( DWORD dwCam, CVector* pvecTarget, float fTargetTheta, float fRadius, float fZoom )
{
    float fPhi = *(float *)(dwCam + 0xBC);
    CVector vecDir = -gravcam_matGravity.vRight*cos(fPhi)*cos(fTargetTheta) - gravcam_matGravity.vFront*sin(fPhi)*cos(fTargetTheta) + gravcam_matGravity.vUp*sin(fTargetTheta);
    ((CVector *)(dwCam + 0x1D8))[0] = *pvecTarget - vecDir*fRadius;
    ((CVector *)(dwCam + 0x1D8))[1] = *pvecTarget - vecDir*fZoom;
}

void _declspec(naked) HOOK_VehicleCamHistory ()
{
    _asm
    {
        push [esp+0x0+0x7C]       // zoom
        push [esp+0x4+0x2C]       // radius
        push [esp+0x8+0x14]       // targetTheta
        lea eax, [esp+0xC+0x48]
        push eax                  // pvecTarget
        push esi                  // pCam
        call VehicleCamHistory
        add esp, 0x14

        mov eax, [esp+0x24]
        jmp RETURN_VehicleCamHistory
    }
}

// ---------------------------------------------------

void _cdecl VehicleCamUp ( DWORD dwCam )
{
    // Calculates the up vector for the vehicle camera.
    CVector* pvecUp = (CVector *)(dwCam + 0x1B4);
    CVector* pvecLookDir = (CVector *)(dwCam + 0x190);

    pvecLookDir->Normalize ();
    *pvecUp = *pvecLookDir;
    pvecUp->CrossProduct ( &gravcam_matGravity.vUp );
    pvecUp->CrossProduct ( pvecLookDir );
}

void _declspec(naked) HOOK_VehicleCamUp ()
{
    _asm
    {
        mov edx, ecx
        mov ecx, [ecx+0x21C]        // CCam::pTargetEntity
        mov eax, 0x46A2C0           // CEntity::GetType
        call eax

        cmp al, 2                   // Is it a vehicle?
        jz docustom
        
        mov ecx, edx
        mov eax, 0x509CE0           // CCam::GetVectorsReadyForRW
        jmp eax

docustom:
        push edx
        call VehicleCamUp
        add esp, 4
        ret
    }
}

// ---------------------------------------------------

void _cdecl VehicleCamEnd ( DWORD pVehicleInterface )
{
    // Restore the things that we inverse transformed in VehicleCamStart
    CVehicle* pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD *)pVehicleInterface );
    if ( !pVehicle )
        return;

    pVehicle->SetMatrix ( &gravcam_matVehicleTransform );
    pVehicle->SetMoveSpeed ( &gravcam_vecVehicleVelocity );
}

void _declspec(naked) HOOK_VehicleCamEnd ()
{
    _asm
    {
        mov ds:[0xB6F020], edx

        push edi
        call VehicleCamEnd
        add esp, 4

        jmp RETURN_VehicleCamEnd
    }
}

// ---------------------------------------------------

void _cdecl VehicleLookBehind ( DWORD dwCam, CVector* pvecEntityPos, float fDistance )
{
    // Custom calculation of the camera position when looking behind while in
    // vehicle cam mode, taking in account custom gravity
    *(CVector *)(dwCam + 0x19C) = *pvecEntityPos + (gravcam_matVehicleTransform.vFront + gravcam_matGravity.vUp*0.2f)*fDistance;
}

void _declspec(naked) HOOK_VehicleLookBehind ()
{
    _asm
    {
        push [esp+0x14]
        lea eax, [esp+4+0x1C]
        push eax
        push edi
        call VehicleLookBehind
        add esp, 0xC

        mov eax, [esp+0x1C]     // vecTemp = vecEntityPos
        mov ecx, [esp+0x1C+4]
        mov edx, [esp+0x1C+8]
        mov [esp+0x34],   eax
        mov [esp+0x34+4], ecx
        mov [esp+0x34+8], edx

        mov ds:[0xB7CD68], ebx  // IgnoreEntity
        mov ds:[0xB6FC70], 0    // NumExtraIgnoreEntities

        mov eax, ebx            // pEntity
        jmp RETURN_VehicleLookBehind
    }
}

// ---------------------------------------------------

void _cdecl VehicleLookAside ( DWORD dwCam, CVector* pvecEntityPos, float fDirectionFactor, float fDistance )
{
    // Custom calculation of the camera position when looking left/right while in
    // vehicle cam mode, taking in account custom gravity
    *(CVector *)(dwCam + 0x19C) = *pvecEntityPos + (-gravcam_matVehicleTransform.vRight*fDirectionFactor + gravcam_matGravity.vUp*0.2f)*fDistance;
}

void _declspec(naked) HOOK_VehicleLookAside ()
{
    _asm
    {
        push [esp+0x14]
        push [esp+4+0x1C]
        lea eax, [esp+8+0x20]
        push eax
        push esi
        call VehicleLookAside
        add esp, 0x10

        lea ebp, [esi+0x19C]
        mov ecx, [esi+0x21C]
        jmp RETURN_VehicleLookAside
    }
}

// ---------------------------------------------------

float _cdecl VehicleBurnCheck ( DWORD pVehicleInterface )
{
    // To check if a vehicle is lying upside down on its roof, SA checks if the z coordinate
    // of the vehicle's up vector is negative. We replace this z by the dot product of the up vector
    // and the negated gravity vector.
    CVehicle* pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD *)pVehicleInterface );
    if ( !pVehicle )
        return 1.0f;

    CVector vecGravity;
    CMatrix matVehicle;
    pVehicle->GetGravity ( &vecGravity );
    pVehicle->GetMatrix ( &matVehicle );
    vecGravity = -vecGravity;
    return matVehicle.vUp.DotProduct ( &vecGravity );
}

void _declspec(naked) HOOK_OccupiedVehicleBurnCheck ()
{
    _asm
    {
        push eax
        call VehicleBurnCheck
        add esp, 4
        jmp RETURN_OccupiedVehicleBurnCheck
    }
}

void _declspec(naked) HOOK_UnoccupiedVehicleBurnCheck ()
{
    _asm
    {
        mov word ptr [esp+0x78], cx

        push esi
        call VehicleBurnCheck
        add esp, 4
        jmp RETURN_UnoccupiedVehicleBurnCheck
    }
}

// ---------------------------------------------------

void _cdecl ApplyVehicleBlowHop ( DWORD pVehicleInterface )
{
    // Custom application of the little jump that vehicles make when they blow up,
    // taking into account custom gravity
    CVehicle* pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD *)pVehicleInterface );
    if ( !pVehicle )
        return;

    CVector vecGravity, vecVelocity;
    pVehicle->GetGravity ( &vecGravity );
    pVehicle->GetMoveSpeed ( &vecVelocity );
    vecVelocity -= vecGravity * 0.13f;
    pVehicle->SetMoveSpeed ( &vecVelocity );
}

void _declspec(naked) HOOK_ApplyCarBlowHop ()
{
    _asm
    {
        push esi
        call ApplyVehicleBlowHop
        add esp, 4

        mov dl, [esi+0x36]
        mov ecx, [esi+0x18]
        and dl, 7
        or dl, 0x28
        mov [esi+0x36], dl
        jmp RETURN_ApplyCarBlowHop
    }
}

// ---------------------------------------------------


DWORD CALL_CWorld_Process = 0x5684a0;
void _declspec(naked) HOOK_CGame_Process ()
{
    _asm
    {
        call    CALL_CWorld_Process
        mov     ecx, 0B72978h
        pushad
    }

    if ( m_pPostWorldProcessHandler ) m_pPostWorldProcessHandler ();

    _asm
    {
        popad
        jmp     RETURN_CGame_Process;
    }
}


DWORD CALL_CGame_Process = 0x53BEE0;
void _declspec(naked) HOOK_Idle ()
{
    _asm
    {
        call    CALL_CGame_Process
        pushad
    }

    if ( m_pIdleHandler ) m_pIdleHandler ();

    _asm
    {
        popad
        mov     ecx, 0B6BC90h
        jmp     RETURN_Idle
    }
}

// ---------------------------------------------------

#define ENABLE_VEHICLE_HEADLIGHT_COLOR 1


#if !ENABLE_VEHICLE_HEADLIGHT_COLOR

// Dummy stub
void vehicle_lights_init ( void )
{
}


#else

#define HOOKPOS_CVehicle_DoVehicleLights                    0x6e1a60
DWORD RETURN_CVehicle_DoVehicleLights =                     0x6e1a68;

#define HOOKPOS_CVehicle_DoHeadLightBeam_1                  0x6E0E20
DWORD RETURN_CVehicle_DoHeadLightBeam_1 =                   0x6E0E26;

#define HOOKPOS_CVehicle_DoHeadLightBeam_2                  0x6E13A4
DWORD RETURN_CVehicle_DoHeadLightBeam_2 =                   0x6E13AE;

#define HOOKPOS_CVehicle_DoHeadLightEffect_1                0x6E0D01
DWORD RETURN_CVehicle_DoHeadLightEffect_1 =                 0x6E0D09;

#define HOOKPOS_CVehicle_DoHeadLightEffect_2                0x6E0DF7
DWORD RETURN_CVehicle_DoHeadLightEffect_2 =                 0x6E0DFF;

#define HOOKPOS_CVehicle_DoHeadLightReflectionTwin          0x6E170F
DWORD RETURN_CVehicle_DoHeadLightReflectionTwin =           0x6E1717;

#define HOOKPOS_CVehicle_DoHeadLightReflectionSingle        0x6E15E2
DWORD RETURN_CVehicle_DoHeadLightReflectionSingle =         0x6E15EA;

void HOOK_CVehicle_DoVehicleLights ();
void HOOK_CVehicle_DoHeadLightBeam_1 ();
void HOOK_CVehicle_DoHeadLightBeam_2 ();
void HOOK_CVehicle_DoHeadLightEffect_1 ();
void HOOK_CVehicle_DoHeadLightEffect_2 ();
void HOOK_CVehicle_DoHeadLightReflectionTwin ();
void HOOK_CVehicle_DoHeadLightReflectionSingle ();

void vehicle_lights_init ( void )
{
    HookInstall(HOOKPOS_CVehicle_DoVehicleLights, (DWORD)HOOK_CVehicle_DoVehicleLights, 8 );
    HookInstall(HOOKPOS_CVehicle_DoHeadLightBeam_1, (DWORD)HOOK_CVehicle_DoHeadLightBeam_1, 6 );
    HookInstall(HOOKPOS_CVehicle_DoHeadLightBeam_2, (DWORD)HOOK_CVehicle_DoHeadLightBeam_2, 10 );
    HookInstall(HOOKPOS_CVehicle_DoHeadLightEffect_1, (DWORD)HOOK_CVehicle_DoHeadLightEffect_1, 8 );
    HookInstall(HOOKPOS_CVehicle_DoHeadLightEffect_2, (DWORD)HOOK_CVehicle_DoHeadLightEffect_2, 8 );
    HookInstall(HOOKPOS_CVehicle_DoHeadLightReflectionTwin, (DWORD)HOOK_CVehicle_DoHeadLightReflectionTwin, 8 );
    HookInstall(HOOKPOS_CVehicle_DoHeadLightReflectionSingle, (DWORD)HOOK_CVehicle_DoHeadLightReflectionSingle, 8 );

    // Allow turning on vehicle lights even if the engine is off
    memset ( (void *)0x6E1DBC, 0x90, 8 );

    // Fix vehicle back lights both using light state 3 (SA bug)
    *(BYTE *)0x6E1D4F = 2;
}


CVehicleSAInterface * pLightsVehicleInterface = NULL;
void _declspec(naked) HOOK_CVehicle_DoVehicleLights ()
{
    _asm
    {
        mov     pLightsVehicleInterface, ecx
        mov     al,byte ptr ds:[00C1CC18h] 
        sub     esp,3Ch 
        jmp     RETURN_CVehicle_DoVehicleLights
    }
}

unsigned long ulHeadLightR = 0, ulHeadLightG = 0, ulHeadLightB = 0;
void CVehicle_GetHeadLightColor ( CVehicleSAInterface * pInterface, float fR, float fG, float fB )
{
    unsigned char R = 255, G = 255, B = 255;
    CVehicle * pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD *)pInterface );
    if ( pVehicle )
    {
        RGBA color = pVehicle->GetHeadLightColor ();
        R = COLOR_RGBA_R ( color );
        G = COLOR_RGBA_G ( color );
        B = COLOR_RGBA_B ( color );
    }
    
    // Scale our color values to the defaults ..looks dodgy but its needed!
    ulHeadLightR = (unsigned char) Min ( 255.f, R * ( 1 / 255.0f ) * fR );
    ulHeadLightG = (unsigned char) Min ( 255.f, G * ( 1 / 255.0f ) * fG );
    ulHeadLightB = (unsigned char) Min ( 255.f, B * ( 1 / 255.0f ) * fB );
}

CVehicleSAInterface * pHeadLightBeamVehicleInterface = NULL;
void _declspec(naked) HOOK_CVehicle_DoHeadLightBeam_1 ()
{
    _asm
    {
        mov     pHeadLightBeamVehicleInterface, ecx
        sub     esp, 94h
        jmp     RETURN_CVehicle_DoHeadLightBeam_1
    }        
}

RwVertex * pHeadLightVerts = NULL;
unsigned int uiHeadLightNumVerts = 0;
void CVehicle_DoHeadLightBeam ()
{    
    // 255, 255, 255
    CVehicle_GetHeadLightColor ( pHeadLightBeamVehicleInterface, 255.0f, 255.0f, 255.0f );

    for ( unsigned int i = 0 ; i < uiHeadLightNumVerts ; i++ )
    {        
        unsigned char alpha = COLOR_ARGB_A ( pHeadLightVerts [ i ].color );
        pHeadLightVerts [ i ].color = COLOR_ARGB ( alpha, (unsigned char) ulHeadLightR, (unsigned char) ulHeadLightG, (unsigned char) ulHeadLightB );
    }
}

void _declspec(naked) HOOK_CVehicle_DoHeadLightBeam_2 ()
{
    _asm
    {
        mov     eax, [esp]
        mov     pHeadLightVerts, eax
        mov     eax, [esp+4]
        mov     uiHeadLightNumVerts, eax 
        pushad
    }

    CVehicle_DoHeadLightBeam ();
    
    _asm
    {
        popad
        mov     dword ptr ds:[0C4B950h],5 
        jmp     RETURN_CVehicle_DoHeadLightBeam_2
    }
}

DWORD dwCCoronas_RegisterCorona = 0x6FC580;
void _declspec(naked) HOOK_CVehicle_DoHeadLightEffect_1 ()
{
    // 160, 160, 140
    _asm pushad

    CVehicle_GetHeadLightColor ( pLightsVehicleInterface, 160.0f, 160.0f, 140.0f );

    _asm
    {
        popad
        mov     eax, ulHeadLightR
        mov     [esp+8], eax
        mov     eax, ulHeadLightG
        mov     [esp+12], eax
        mov     eax, ulHeadLightB
        mov     [esp+16], eax
        //mov     eax, ulHeadLightA
        //mov     [esp+20], eax

        call    dwCCoronas_RegisterCorona 
        add     esp,54h
        jmp     RETURN_CVehicle_DoHeadLightEffect_1
    }
}


void _declspec(naked) HOOK_CVehicle_DoHeadLightEffect_2 ()
{
    // 160, 160, 140
    _asm pushad

    CVehicle_GetHeadLightColor ( pLightsVehicleInterface, 160.0f, 160.0f, 140.0f );

    _asm
    {
        popad
        mov     eax, ulHeadLightR
        mov     [esp+8], eax
        mov     eax, ulHeadLightG
        mov     [esp+12], eax
        mov     eax, ulHeadLightB
        mov     [esp+16], eax
        //mov     eax, ulHeadLightA
        //mov     [esp+20], eax

        call    dwCCoronas_RegisterCorona 
        add     esp, 54h
        jmp     RETURN_CVehicle_DoHeadLightEffect_2
    }
}


DWORD dwCShadows_StoreCarLightShadow = 0x70C500;
void _declspec(naked) HOOK_CVehicle_DoHeadLightReflectionTwin ()
{
    // 45, 45, 45
    _asm pushad
 
    CVehicle_GetHeadLightColor ( pLightsVehicleInterface, 45.0f, 45.0f, 45.0f );

    _asm
    {
        popad
        mov     eax, ulHeadLightR
        mov     [esp+32], eax
        mov     eax, ulHeadLightG
        mov     [esp+36], eax
        mov     eax, ulHeadLightB
        mov     [esp+40], eax

        call    dwCShadows_StoreCarLightShadow
        add     esp, 4Ch
        jmp     RETURN_CVehicle_DoHeadLightReflectionTwin
    }    
}


void _declspec(naked) HOOK_CVehicle_DoHeadLightReflectionSingle ()
{
    // 45, 45, 45
    __asm pushad

    CVehicle_GetHeadLightColor ( pLightsVehicleInterface, 45.0f, 45.0f, 45.0f );

    _asm
    {
        popad
        mov     eax, ulHeadLightR
        mov     [esp+32], eax
        mov     eax, ulHeadLightG
        mov     [esp+36], eax
        mov     eax, ulHeadLightB
        mov     [esp+40], eax

        call    dwCShadows_StoreCarLightShadow
        add     esp, 30h
        jmp     RETURN_CVehicle_DoHeadLightReflectionSingle
    }
}

#endif  // ENABLE_VEHICLE_HEADLIGHT_COLOR

// ---------------------------------------------------

// Report fire damage, with correct inflictor entity

void _declspec(naked) HOOK_CWorld_SetWorldOnFire ()
{
    // Actually pass the pCreatorEntity parameter that this function receives to CFireManager::StartFire
    // (instead of a null pointer)
    _asm
    {
        push 7000
        push [esp+0x18+0x14]
        jmp RETURN_CWorld_SetWorldOnFire
    }
}

void _declspec(naked) HOOK_CTaskSimplePlayerOnFire_ProcessPed ()
{
    // Actually pass the fire's pCreatorEntity to the damage event (instead of a null pointer)
    _asm
    {
        push 3
        push 0x25
        push edx
        mov eax, [edi+0x730]    // eax = pPed->pFire
        mov eax, [eax+0x14]     // eax = pFire->pCreator
        push eax
        jmp RETURN_CTaskSimplePlayerOnFire_ProcessPed
    }
}

void _declspec(naked) HOOK_CFire_ProcessFire ()
{
    // Set the new fire's creator to the original fire's creator
    _asm
    {
        mov eax, 0x53A450       // CCreepingFire::TryToStartFireAtCoors
        call eax
        test eax, eax
        jz fail
        mov ecx, [esi+0x14] 
        mov [eax+0x14], ecx
fail:
        jmp RETURN_CFire_ProcessFire
    }
}

void _declspec(naked) HOOK_CExplosion_Update ()
{
    // Set the new fire's creator to the explosion's creator
    _asm
    {
        mov eax, 0x53A450       // CCreepingFire::TryToStartFireAtCoors
        call eax
        test eax, eax
        jz fail
        mov ecx, [esi-0x18]
        mov [eax+0x14], ecx
fail:
        jmp RETURN_CExplosion_Update
    }
}

void _declspec(naked) HOOK_CWeapon_FireAreaEffect ()
{
    // Set the new fire's creator to the weapon's owner
    _asm
    {
        mov eax, 0x53A450       // CCreepingFire::TryToStartFireAtCoors
        call eax
        test eax, eax
        jz fail
        mov ecx, [esp+0x6C+4]
        mov [eax+0x14], ecx
fail:
        jmp RETURN_CWeapon_FireAreaEffect
    }
}

// ---------------------------------------------------

// The purpose of these hooks is to divide plant (grass) rendering in two:
// rather than render *all* grass before or after the water like SA does, we render
// underwater plants before the water and above-water plants after. This way, we have
// at the same time that underwater plants do not pop up in front of water,
// and water is not drawn in front of above-water plants (eg if you're looking at a
// lake through some high grass).

void _declspec(naked) HOOK_RenderScene_Plants ()
{
    _asm
    {
        push 1                  // bRenderingBeforeWater
        movzx eax, bl           // bCamBelowWater
        push eax
        mov eax, 0x5DBAE0       // CPlantMgr::Render
        call eax
        add esp, 8
        ret
    }
}

void _declspec(naked) HOOK_RenderScene_end ()
{
    _asm
    {
        push 0                  // bRenderingBeforeWater
        movzx eax, bl           // bCamBelowWater
        push eax
        mov eax, 0x5DBAE0       // CPlantMgr::Render
        call eax
        add esp, 8

        pop ebx
        add esp, 8
        mov eax, 0x7113B0
        jmp eax
    }
}

bool _cdecl IsPlantBelowWater ( float fPlantZ, float fWaterZ )
{
    return fPlantZ + 2.0f < fWaterZ;
}

void _declspec(naked) HOOK_CPlantMgr_Render ()
{
    // (bCamBelowWater, bRenderingBeforeWater)
    _asm
    {
        sub esp, 4
        mov eax, esp
        push 0
        push 0
        push eax                // pWaterLevel
        push [ebp+8]            // Plant position
        push [ebp+4]
        push [ebp]
        mov eax, 0x6E8580       // CWaterLevel::GetWaterLevelNoWaves
        call eax
        add esp, 0x18
        add esp, 4
        test al, al
        jnz watercheck          // only compare plant.z to water level if there actually is water here
        
        xor eax, eax            // if there's no water, assume "plant above water"
        jmp rendercheck

watercheck:
        sub esp, 4
        push [ebp+8]
        call IsPlantBelowWater
        add esp, 8
        
rendercheck:
        xor eax, [esp+0x88+4]   // Decide whether or not to draw the plant right now
        cmp eax, [esp+0x88+8]
        jnz fail

        mov ax, [esi-0x10]
        mov edx, edi
        fld ds:[0x8D12C0]
        jmp RETURN_CPlantMgr_Render_success

fail:
        jmp RETURN_CPlantMgr_Render_fail
    }
}

/* This hook called from CEventHandler::ComputeKnockOffBikeResponse makes sure the
   damage calculation is made *before* CEventDamage::AffectsPed (our damage hook) is called.
   * Fixes 'falling off bike' damage being 0 in CClientGame::DamageHandler.
*/
CEventDamageSAInterface * pBikeDamageInterface;
CPedSAInterface * pBikePedInterface;
float fBikeDamage;
void CEventHandler_ComputeKnockOffBikeResponse ()
{
    CEventDamage * pEvent = pGameInterface->GetEventList ()->GetEventDamage ( pBikeDamageInterface );
    CPed * pPed = pGameInterface->GetPools ()->GetPed ( (DWORD*) pBikePedInterface );
    if ( pEvent && pPed )
    {
        CPedDamageResponse * pResponse = pEvent->GetDamageResponse ();
        if ( pResponse )
        {
            pResponse->Calculate ( ( CEntity * ) pPed, fBikeDamage, pEvent->GetWeaponUsed (), pEvent->GetPedPieceType (), true, false );
        }        
    }
    if ( pEvent ) pEvent->Destroy ();
}

DWORD dw_CEventDamage_AffectsPed = 0x4b35a0;
void _declspec(naked) HOOK_CEventHandler_ComputeKnockOffBikeResponse ()
{
    _asm
    {
        mov     pBikeDamageInterface, ecx
        mov     pBikePedInterface, edx
        mov     eax, [edi+40]
        mov     fBikeDamage, eax

        pushad
    }
    CEventHandler_ComputeKnockOffBikeResponse ();
    
    _asm
    {
        popad
        call    dw_CEventDamage_AffectsPed
        jmp     RETURN_CEventHandler_ComputeKnockOffBikeResponse
    }
}


RpClump * animationClump = NULL;
AssocGroupId animationGroup = 0;
AnimationId animationID = 0;
void _declspec(naked) HOOK_CAnimManager_AddAnimation ()
{
    _asm
    {        
        mov     eax, [esp+4]
        mov     animationClump, eax
        mov     eax, [esp+8]
        mov     animationGroup, eax
        mov     eax, [esp+12]
        mov     animationID, eax
        pushad
    }
    
    if ( m_pAddAnimationHandler  )
    {
        m_pAddAnimationHandler ( animationClump, animationGroup, animationID );
    }

    _asm
    {
        popad
        mov     eax,dword ptr [esp+0Ch] 
        mov     edx,dword ptr ds:[0B4EA34h] 
        jmp     RETURN_CAnimManager_AddAnimation
    }
}

float animationBlendDelta;
void _declspec(naked) HOOK_CAnimManager_BlendAnimation ()
{
    _asm
    {        
        mov     eax, [esp+4]
        mov     animationClump, eax
        mov     eax, [esp+8]
        mov     animationGroup, eax
        mov     eax, [esp+12]
        mov     animationID, eax
        mov     eax, [esp+16]
        mov     animationBlendDelta, eax
        pushad
    }
    
    if ( m_pBlendAnimationHandler  )
    {
        m_pBlendAnimationHandler ( animationClump, animationGroup, animationID, animationBlendDelta );
    }

    _asm
    {
        popad
        sub     esp,14h 
        mov     ecx,dword ptr [esp+18h]
        jmp     RETURN_CAnimManager_BlendAnimation
    }
}

CPedSAInterface * weaponSkillPed;
eWeaponType weaponSkillWeapon;
BYTE weaponSkill;
bool CPed_GetWeaponSkill ()
{
    CPed * pPed = pGameInterface->GetPools ()->GetPed ( (DWORD*) weaponSkillPed );
    if ( pPed )
    {
        CPed* pLocalPlayerPed = pGameInterface->GetPools ()->GetPedFromRef ( (DWORD)1 );
        if ( pPed != pLocalPlayerPed )
        {
            if ( weaponSkillWeapon >= 22 && weaponSkillWeapon <= 32 )
            {
                CPlayerPed* playerPed = dynamic_cast < CPlayerPed* > ( pPed ); 
	            if ( playerPed )
                {
		            CRemoteDataStorageSA * data = CRemoteDataSA::GetRemoteDataStorage ( playerPed );
                    float stat = data->m_stats.StatTypesFloat [ pGameInterface->GetStats ()->GetSkillStatIndex ( weaponSkillWeapon ) ];
                    
                    CWeaponInfo * pPoor = pGameInterface->GetWeaponInfo ( weaponSkillWeapon, WEAPONSKILL_POOR );
                    CWeaponInfo * pStd = pGameInterface->GetWeaponInfo ( weaponSkillWeapon, WEAPONSKILL_STD );
                    CWeaponInfo * pPro = pGameInterface->GetWeaponInfo ( weaponSkillWeapon, WEAPONSKILL_PRO );

                    if ( stat >= pPro->GetRequiredStatLevel () ) weaponSkill = WEAPONSKILL_PRO;
                    else if ( stat >= pStd->GetRequiredStatLevel () ) weaponSkill = WEAPONSKILL_STD;
                    else weaponSkill = WEAPONSKILL_POOR;
                    return true;
                }
            }
        }
    }
    return false;
}

void _declspec(naked) HOOK_CPed_GetWeaponSkill ()
{
    _asm
    {
        mov     weaponSkillPed, ecx
        mov     eax, [esp+4]
        mov     weaponSkillWeapon, eax
        pushad
    }

    if ( CPed_GetWeaponSkill () )
    {
        _asm
        {
            popad
            mov     al, weaponSkill
            retn    4
        }
    }
    else
    {
        _asm
        {
            popad
            push    esi
            mov     esi, [esp+8]
            cmp     esi, 16h
            jmp     RETURN_CPed_GetWeaponSkill
        }
    }
}