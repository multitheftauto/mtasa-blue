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
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

// These includes have to be fixed!
#include "..\game_sa\CCameraSA.h"
#include "..\game_sa\CEntitySA.h"
#include "..\game_sa\CBuildingSA.h"
#include "..\game_sa\CPedSA.h"
#include "..\game_sa\common.h"

using namespace std;

char* CMultiplayerSA::ms_PlayerImgCachePtr = NULL;

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
unsigned long CMultiplayerSA::HOOKPOS_CAutomobile__ProcessSwingingDoor;

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

#define CALL_CTrafficLights_GetPrimaryLightState 			0x49DB5F
#define CALL_CTrafficLights_GetSecondaryLightState 			0x49DB6D

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

#define HOOKPOS_CPed_AddGogglesModel                        0x5E3ACB
DWORD RETURN_CPed_AddGogglesModel =                         0x5E3AD4;

#define FUNC_CWorld_Remove                                  0x563280
#define FUNC_CTagManager_ShutdownForRestart                 0x49CC60
unsigned int* VAR_NumTags                                   = (unsigned int *)0xA9AD70;
DWORD** VAR_TagInfoArray                                    = (DWORD **)0xA9A8C0;

#define HOOKPOS_CPhysical_ProcessCollisionSectorList        0x54BB93
DWORD RETURN_CPhysical_ProcessCollisionSectorList =         0x54BB9A;

#define HOOKPOS_CrashFix_Misc1                              0x5D9A6E
DWORD RETURN_CrashFix_Misc1 =                               0x5D9A74;

#define HOOKPOS_CrashFix_Misc2                              0x6B18B0
DWORD RETURN_CrashFix_Misc2a =                              0x6B18B9;
DWORD RETURN_CrashFix_Misc2b =                              0x6B1F6C;

#define HOOKPOS_CrashFix_Misc3                              0x645FD9
DWORD RETURN_CrashFix_Misc3 =                               0x645FDF;
void CPlayerPed__ProcessControl_Abort();

#define HOOKPOS_CrashFix_Misc4                              0x4F02D2
DWORD RETURN_CrashFix_Misc4a =                              0x4F02D7;
DWORD RETURN_CrashFix_Misc4b =                              0x4F0B07;

#define HOOKPOS_CrashFix_Misc5                              0x5DF949
DWORD RETURN_CrashFix_Misc5a =                              0x5DF950;
DWORD RETURN_CrashFix_Misc5b =                              0x5DFCC4;

#define HOOKPOS_CrashFix_Misc6                              0x4D1750
DWORD RETURN_CrashFix_Misc6a =                              0x4D1755;
DWORD RETURN_CrashFix_Misc6b =                              0x4D1A44;

#define HOOKPOS_CrashFix_Misc7                              0x417BF8
DWORD RETURN_CrashFix_Misc7a =                              0x417BFD;
DWORD RETURN_CrashFix_Misc7b =                              0x417BFF;

#define HOOKPOS_CrashFix_Misc8                              0x73485D
DWORD RETURN_CrashFix_Misc8a =                              0x734862;
DWORD RETURN_CrashFix_Misc8b =                              0x734871;

#define HOOKPOS_CrashFix_Misc9                              0x738B64
DWORD RETURN_CrashFix_Misc9a =                              0x738B6A;
DWORD RETURN_CrashFix_Misc9b =                              0x73983A;

#define HOOKPOS_CrashFix_Misc10                             0x5334FE
DWORD RETURN_CrashFix_Misc10a =                             0x533504;
DWORD RETURN_CrashFix_Misc10b =                             0x533539;

#define HOOKPOS_CrashFix_Misc11                             0x4D2C62
DWORD RETURN_CrashFix_Misc11a =                             0x4D2C67;
DWORD RETURN_CrashFix_Misc11b =                             0x4D2E03;

#define HOOKPOS_CrashFix_Misc12                             0x4D41C5
DWORD RETURN_CrashFix_Misc12a =                             0x4D41CA;
DWORD RETURN_CrashFix_Misc12b =                             0x4D4222;

#define HOOKPOS_CrashFix_Misc13                             0x4D464E
DWORD RETURN_CrashFix_Misc13a =                             0x4D4654;
DWORD RETURN_CrashFix_Misc13b =                             0x4D4764;

#define HOOKPOS_CrashFix_Misc14                              0x4DD4B5
DWORD RETURN_CrashFix_Misc14 =                               0x4DD4BB;

#define HOOKPOS_FreezeFix_Misc15_US                         0x156CDAE
#define HOOKPOS_FreezeFix_Misc15_EU                         0x156CDEE
DWORD RETURN_FreezeFix_Misc15_US =                          0x156CDB4;
DWORD RETURN_FreezeFix_Misc15_EU =                          0x156CDF4;
DWORD RETURN_FreezeFix_Misc15_BOTH =                        0;

#define HOOKPOS_CrashFix_Misc16                              0x5E5815
DWORD RETURN_CrashFix_Misc16 =                               0x5E581B;

#define HOOKPOS_CrashFix_Misc17                              0x5E5815
DWORD RETURN_CrashFix_Misc17 =                               0x5E581B;

#define HOOKPOS_CrashFix_Misc17_US                          0x7F120E
#define HOOKPOS_CrashFix_Misc17_EU                          0x7F124E
DWORD RETURN_CrashFix_Misc17a_US =                          0x7F1214;
DWORD RETURN_CrashFix_Misc17a_EU =                          0x7F1254;
DWORD RETURN_CrashFix_Misc17a_BOTH =                        0;
DWORD RETURN_CrashFix_Misc17b_US =                          0x7F1236;
DWORD RETURN_CrashFix_Misc17b_EU =                          0x7F1276;
DWORD RETURN_CrashFix_Misc17b_BOTH =                        0;

#define HOOKPOS_CrashFix_Misc18                              0x4C7DAD
DWORD RETURN_CrashFix_Misc18 =                               0x4C7DB4;

#define HOOKPOS_CrashFix_Misc19_US                          0x7F0BF7
#define HOOKPOS_CrashFix_Misc19_EU                          0x7F0C37
DWORD RETURN_CrashFix_Misc19a_US =                          0x7F0BFD;
DWORD RETURN_CrashFix_Misc19a_EU =                          0x7F0C3D;
DWORD RETURN_CrashFix_Misc19a_BOTH =                        0;
DWORD RETURN_CrashFix_Misc19b_US =                          0x7F0C20;
DWORD RETURN_CrashFix_Misc19b_EU =                          0x7F0C60;
DWORD RETURN_CrashFix_Misc19b_BOTH =                        0;

#define HOOKPOS_CrashFix_Misc20                              0x54F3B0
DWORD RETURN_CrashFix_Misc20 =                               0x54F3B6;

#define HOOKPOS_CrashFix_Misc21                              0x648EF6
DWORD RETURN_CrashFix_Misc21 =                               0x648EFC;

#define HOOKPOS_CrashFix_Misc22                              0x4CEF08
DWORD RETURN_CrashFix_Misc22 =                               0x4CEF25;

#define HOOKPOS_CrashFix_Misc23                              0x6E3D10
DWORD RETURN_CrashFix_Misc23 =                               0x6E3D17;

#define HOOKPOS_CrashFix_Misc24_US                          0x7F0DC8
#define HOOKPOS_CrashFix_Misc24_EU                          0x7F0E08
DWORD RETURN_CrashFix_Misc24_US =                           0x7F0DCE;
DWORD RETURN_CrashFix_Misc24_EU =                           0x7F0E0E;
DWORD RETURN_CrashFix_Misc24_BOTH =                         0;

#define HOOKPOS_CheckAnimMatrix_US                          0x7C5A5C
#define HOOKPOS_CheckAnimMatrix_EU                          0x7C5A9C
DWORD RETURN_CheckAnimMatrix_US =                           0x7C5A61;
DWORD RETURN_CheckAnimMatrix_EU =                           0x7C5AA1;
DWORD RETURN_CheckAnimMatrix_BOTH =                         0;

#define HOOKPOS_VehColCB                                    0x04C838D
DWORD RETURN_VehColCB =                                     0x04C83AA;

#define HOOKPOS_VehCol                                      0x06D6603
DWORD RETURN_VehCol =                                       0x06D660C;

// Handling fix - driveType is per model
#define HOOKPOS_CHandlingData_isNotRWD              0x6A048C
DWORD RETURN_CHandlingData_isNotRWD =               0x6A0493;
#define HOOKPOS_CHandlingData_isNotFWD              0x6A04BC
DWORD RETURN_CHandlingData_isNotFWD =               0x6A04C3;
// end of handling fix
#define CALL_CAutomobile_ProcessEntityCollision             0x6AD053
#define CALL_CBike_ProcessEntityCollision1                  0x6BDF82
#define CALL_CBike_ProcessEntityCollision2                  0x6BE0D1
#define CALL_CMonsterTruck_ProcessEntityCollision           0x6C8B9E
DWORD RETURN_ProcessEntityCollision =                             0x4185C0;

#define HOOKPOS_PreHUDRender                                      0x053EAD8
DWORD RETURN_PreHUDRender =                                       0x053EADD;

#define HOOKPOS_LoadingPlayerImgDir                         0x5A69E3
DWORD RETURN_LoadingPlayerImgDira =                         0x5A69E8;
DWORD RETURN_LoadingPlayerImgDirb =                         0x5A6A06;

#define HOOKPOS_CallCStreamingInfoAddToList                 0x408962
DWORD FUNC_CStreamingInfoAddToList              = 0x407480;
DWORD RETURN_CallCStreamingInfoAddToLista       = 0x408967;
DWORD RETURN_CallCStreamingInfoAddToListb       = 0x408990;

#define HOOKPOS_CStreamingLoadRequestedModels_US            0x15670A0
#define HOOKPOS_CStreamingLoadRequestedModels_EU            0x1567090
DWORD RETURN_CStreamingLoadRequestedModelsa_US =    0x15670A5;
DWORD RETURN_CStreamingLoadRequestedModelsa_EU =    0x1567095;
DWORD RETURN_CStreamingLoadRequestedModelsa_BOTH =  0;
DWORD RETURN_CStreamingLoadRequestedModelsb_US =    0x156711B;
DWORD RETURN_CStreamingLoadRequestedModelsb_EU =    0x156710B;
DWORD RETURN_CStreamingLoadRequestedModelsb_BOTH =  0;
DWORD CStreaming__ConvertBufferToObject =           0x040C6B0;

#define HOOKPOS_CRenderer_SetupEntityVisibility     0x554230    // 8 bytes
DWORD RETURN_CRenderer_SetupEntityVisibility =      0x554238;

#define HOOKPOS_CWorldScan_ScanWorld                0x55555E    // 5 bytes
DWORD RETURN_CWorldScan_ScanWorlda =                0x555563;
DWORD RETURN_CWorldScan_ScanWorldb =                0x72CAE0;

#define HOOKPOS_CVisibilityPlugins_CalculateFadingAtomicAlpha   0x732500    // 5 bytes
DWORD RETURN_CVisibilityPlugins_CalculateFadingAtomicAlpha =    0x732505;

#define HOOKPOS_LoadIPLInstance                                    0x4061E8
DWORD CALL_LoadIPLInstance   =                                     0x538090;
DWORD RETURN_LoadIPLInstance =                                     0x04061ED;

#define HOOKPOS_CWorld_LOD_SETUP                                  0x406224
#define HOOKPOS_CWorld_LOD_SETUP2                                 0x406326
DWORD CALL_CWorld_LODSETUP   =                                    0x404C90;

#define HOOKPOS_CBuilding_DTR                                     0x404180
DWORD JMP_CBuilding_DTR   =                                       0x535E90;

#define HOOKPOS_CDummy_DTR                                        0x532566
DWORD JMP_CDummy_DTR   =                                          0x535E90;

#define HOOKPOS_CObject_DTR                                       0x59F680
DWORD JMP_CObject_DTR  =                                          0x59F686;

#define HOOKPOS_AddBuildingInstancesToWorld_CWorldAdd             0x5B5348
DWORD JMP_CWorld_Add_AddBuildingInstancesToWorld_CALL_CWorldAdd = 0x563220;
DWORD RETURN_AddBuildingInstancesToWorld_CWorldAdd =              0x5B534D;


#define HOOKPOS_CWorld_Remove_CPopulation_ConvertToDummyObject    0x6146F8

#define HOOKPOS_CWorld_ADD_CPopulation_ConvertToRealObject              0x6145C7
DWORD JMP_CWorld_Add_CPopulation_ConvertToRealObject_Retn =             0x6145CC;
DWORD JMP_CWorld_Add_CPopulation_ConvertToRealObject_CallCWorldAdd =    0x563220;

#define HOOKPOS_ConvertToObject_CPopulationManageDummy            0x616091
DWORD CALL_Convert_To_Real_Object_CPopulation_ManageDummy = 0x614580;
DWORD JMP_RETN_Called_CPopulation_ManageDummy = 0x616097;
DWORD JMP_RETN_Cancel_CPopulation_ManageDummy = 0x616098;

#define HOOKPOS_CWorld_ADD_CPopulation_ConvertToDummyObject       0x61470C
DWORD CALL_CWorld_Add_CPopulation_ConvertToDummyObject = 0x563220;
DWORD JMP_RETN_Called_CPopulation_ConvertToDummyObject = 0x614712;
DWORD JMP_RETN_Cancelled_CPopulation_ConvertToDummyObject = 0x614715;

#define HOOKPOS_CEntity_IsOnScreen_FixObjectsScale      0x534575
DWORD JMP_CEntity_IsOnScreen_FixObjectsScale = 0x53457C;

#define HOOKPOS_CEventVehicleDamageCollision                    0x6A7657
DWORD JMP_CEventVehicleDamageCollision_RETN = 0x6A765D;

#define HOOKPOS_CEventVehicleDamageCollision_Plane              0x6CC4B3
DWORD JMP_CEventVehicleDamageCollision_Plane_RETN = 0x6CC4B8;

#define HOOKPOS_CEventVehicleDamageCollision_Bike               0x6B8EC6
DWORD JMP_CEventVehicleDamageCollision_Bike_RETN = 0x6B8ECC;

#define HOOKPOS_CClothes_RebuildPlayer                      0x5A82C0
DWORD RETURN_CClothes_RebuildPlayera                        = 0x5A82C8;
DWORD RETURN_CClothes_RebuildPlayerb                        = 0x5A837F;

#define HOOKPOS_CProjectileInfo_FindPlayerPed               0x739321
#define HOOKPOS_CProjectileInfo_FindPlayerVehicle           0x739570

#define HOOKPOS_CHeli_ProcessHeliKill                       0x6DB201
DWORD RETURN_CHeli_ProcessHeliKill_RETN_Cancel = 0x6DB9E0;
DWORD RETURN_CHeli_ProcessHeliKill_RETN_Cont_Zero = 0x6DB207;

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
unsigned char ucTrafficLightState = 0;
bool bTrafficLightsBlocked = false;
bool bInteriorSoundsEnabled = true;

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
BulletImpactHandler* m_pBulletImpactHandler = NULL;
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
PreWorldProcessHandler * m_pPreWorldProcessHandler = NULL;
PostWorldProcessHandler * m_pPostWorldProcessHandler = NULL;
IdleHandler * m_pIdleHandler = NULL;
AddAnimationHandler* m_pAddAnimationHandler = NULL;
BlendAnimationHandler* m_pBlendAnimationHandler = NULL;
ProcessCollisionHandler* m_pProcessCollisionHandler = NULL;
VehicleCollisionHandler* m_pVehicleCollisionHandler = NULL;
HeliKillHandler* m_pHeliKillHandler = NULL;
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
void HOOK_CPed_AddGogglesModel ();
void HOOK_CPhysical_ProcessCollisionSectorList ();
void HOOK_CrashFix_Misc1 ();
void HOOK_CrashFix_Misc2 ();
void HOOK_CrashFix_Misc3 ();
void HOOK_CrashFix_Misc4 ();
void HOOK_CrashFix_Misc5 ();
void HOOK_CrashFix_Misc6 ();
void HOOK_CrashFix_Misc7 ();
void HOOK_CrashFix_Misc8 ();
void HOOK_CrashFix_Misc9 ();
void HOOK_CrashFix_Misc10 ();
void HOOK_CrashFix_Misc11 ();
void HOOK_CrashFix_Misc12 ();
void HOOK_CrashFix_Misc13 ();
void HOOK_CrashFix_Misc14 ();
void HOOK_FreezeFix_Misc15 ();
void HOOK_CrashFix_Misc16 ();
void HOOK_CrashFix_Misc17 ();
void HOOK_CrashFix_Misc18 ();
void HOOK_CrashFix_Misc19 ();
void HOOK_CrashFix_Misc20 ();
void HOOK_CrashFix_Misc21 ();
void HOOK_CrashFix_Misc22 ();
void HOOK_CrashFix_Misc23 ();
void HOOK_CrashFix_Misc24 ();
void HOOK_CheckAnimMatrix ();
void HOOK_VehColCB ();
void HOOK_VehCol ();
void HOOK_isVehDriveTypeNotRWD ();
void HOOK_isVehDriveTypeNotFWD ();
void HOOK_PreHUDRender();

void HOOK_CTrafficLights_GetPrimaryLightState ();
void HOOK_CTrafficLights_GetSecondaryLightState ();

void HOOK_CAutomobile__ProcessSwingingDoor ();

void HOOK_ProcessVehicleCollision ();
void HOOK_LoadingPlayerImgDir ();
void HOOK_CallCStreamingInfoAddToList ();
void HOOK_CStreamingLoadRequestedModels ();

void HOOK_CRenderer_SetupEntityVisibility ();
void HOOK_CWorldScan_ScanWorld ();
void HOOK_CVisibilityPlugins_CalculateFadingAtomicAlpha ();

void vehicle_lights_init ();

void HOOK_LoadIPLInstance ();

void HOOK_CWorld_LOD_SETUP ();

void Hook_AddBuildingInstancesToWorld ( );

void HOOK_CWorld_Remove_CPopulation_ConvertToDummyObject ( );

void HOOK_CWorld_Add_CPopulation_ConvertToDummyObject ( );

void Hook_CWorld_ADD_CPopulation_ConvertToRealObject ( );

void HOOK_ConvertToObject_CPopulationManageDummy ( );

void Hook_CBuilding_DTR ( );

void Hook_CDummy_DTR ( );

void Hook_CObject_DTR ( );

void HOOK_CEntity_IsOnScreen_FixObjectScale ();

void HOOK_CEventVehicleDamageCollision ( );

void HOOK_CEventVehicleDamageCollision_Plane ( );

void HOOK_CEventVehicleDamageCollision_Bike ( );

void HOOK_CClothes_RebuildPlayer ();

void HOOK_CProjectileInfo_Update_FindLocalPlayer_FindLocalPlayerVehicle ();

void HOOK_CHeli_ProcessHeliKill ();

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

    MemSetFast ( &localStatsData, 0, sizeof ( CStatsData ) );
    localStatsData.StatTypesFloat [ 24 ] = 569.0f; // Max Health
    m_bSuspensionEnabled = true;

    m_fAircraftMaxHeight = 800.0f;
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
    MemPutFast < int > ( 0x8a5a84, 127 );

    // DISABLE CGameLogic::Update
    MemSet ((void *)0x442AD0, 0xC3, 1);

    // STOP IT TRYING TO LOAD THE SCM
    MemPut < BYTE > ( 0x468EB5, 0xEB );
    MemPut < BYTE > ( 0x468EB6, 0x32 );

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
    HookInstall(HOOKPOS_CEventHandler_ComputeKnockOffBikeResponse, (DWORD)HOOK_CEventHandler_ComputeKnockOffBikeResponse, 7 );
    HookInstall(HOOKPOS_CAnimManager_AddAnimation, (DWORD)HOOK_CAnimManager_AddAnimation, 10 ); 
    HookInstall(HOOKPOS_CAnimManager_BlendAnimation, (DWORD)HOOK_CAnimManager_BlendAnimation, 7 );
    HookInstall(HOOKPOS_CPed_GetWeaponSkill, (DWORD)HOOK_CPed_GetWeaponSkill, 8 );
    HookInstall(HOOKPOS_CPed_AddGogglesModel, (DWORD)HOOK_CPed_AddGogglesModel, 6);
    HookInstall(HOOKPOS_CPhysical_ProcessCollisionSectorList, (DWORD)HOOK_CPhysical_ProcessCollisionSectorList, 7 );
    HookInstall(HOOKPOS_CrashFix_Misc1, (DWORD)HOOK_CrashFix_Misc1, 6 );
    HookInstall(HOOKPOS_CrashFix_Misc2, (DWORD)HOOK_CrashFix_Misc2, 9 );
    HookInstall(HOOKPOS_CrashFix_Misc3, (DWORD)HOOK_CrashFix_Misc3, 6 );
    HookInstall(HOOKPOS_CrashFix_Misc4, (DWORD)HOOK_CrashFix_Misc4, 5 );
    HookInstall(HOOKPOS_CrashFix_Misc5, (DWORD)HOOK_CrashFix_Misc5, 7 );
    HookInstall(HOOKPOS_CrashFix_Misc6, (DWORD)HOOK_CrashFix_Misc6, 5 );
    HookInstall(HOOKPOS_CrashFix_Misc7, (DWORD)HOOK_CrashFix_Misc7, 5 );
    HookInstall(HOOKPOS_CrashFix_Misc8, (DWORD)HOOK_CrashFix_Misc8, 5 );
    HookInstall(HOOKPOS_CrashFix_Misc9, (DWORD)HOOK_CrashFix_Misc9, 6 );
    HookInstall(HOOKPOS_CrashFix_Misc10, (DWORD)HOOK_CrashFix_Misc10, 6 );
    HookInstall(HOOKPOS_CrashFix_Misc11, (DWORD)HOOK_CrashFix_Misc11, 5 );
    HookInstall(HOOKPOS_CrashFix_Misc12, (DWORD)HOOK_CrashFix_Misc12, 5 );
    HookInstall(HOOKPOS_CrashFix_Misc13, (DWORD)HOOK_CrashFix_Misc13, 6 );
    HookInstall(HOOKPOS_CrashFix_Misc14, (DWORD)HOOK_CrashFix_Misc14, 6 );
    if ( version == VERSION_US_10 )
    {
        HookInstall(HOOKPOS_FreezeFix_Misc15_US, (DWORD)HOOK_FreezeFix_Misc15, 6 );
        HookInstall(HOOKPOS_CrashFix_Misc17_US, (DWORD)HOOK_CrashFix_Misc17, 6 );
        HookInstall(HOOKPOS_CrashFix_Misc19_US, (DWORD)HOOK_CrashFix_Misc19, 6 );
        HookInstall(HOOKPOS_CrashFix_Misc24_US, (DWORD)HOOK_CrashFix_Misc24, 6 );
        HookInstall(HOOKPOS_CheckAnimMatrix_US, (DWORD)HOOK_CheckAnimMatrix, 5 );
        HookInstall(HOOKPOS_CStreamingLoadRequestedModels_US, (DWORD)HOOK_CStreamingLoadRequestedModels, 5 );
        RETURN_FreezeFix_Misc15_BOTH = RETURN_FreezeFix_Misc15_US;
        RETURN_CrashFix_Misc17a_BOTH = RETURN_CrashFix_Misc17a_US;
        RETURN_CrashFix_Misc17b_BOTH = RETURN_CrashFix_Misc17b_US;
        RETURN_CrashFix_Misc19a_BOTH = RETURN_CrashFix_Misc19a_US;
        RETURN_CrashFix_Misc19b_BOTH = RETURN_CrashFix_Misc19b_US;
        RETURN_CrashFix_Misc24_BOTH = RETURN_CrashFix_Misc24_US;
        RETURN_CheckAnimMatrix_BOTH = RETURN_CheckAnimMatrix_US;
        RETURN_CStreamingLoadRequestedModelsa_BOTH = RETURN_CStreamingLoadRequestedModelsa_US;
        RETURN_CStreamingLoadRequestedModelsb_BOTH = RETURN_CStreamingLoadRequestedModelsb_US;
    }
    if ( version == VERSION_EU_10 )
    {
        HookInstall(HOOKPOS_FreezeFix_Misc15_EU, (DWORD)HOOK_FreezeFix_Misc15, 6 );
        HookInstall(HOOKPOS_CrashFix_Misc17_EU, (DWORD)HOOK_CrashFix_Misc17, 6 );
        HookInstall(HOOKPOS_CrashFix_Misc19_EU, (DWORD)HOOK_CrashFix_Misc19, 6 );
        HookInstall(HOOKPOS_CrashFix_Misc24_EU, (DWORD)HOOK_CrashFix_Misc24, 6 );
        HookInstall(HOOKPOS_CheckAnimMatrix_EU, (DWORD)HOOK_CheckAnimMatrix, 5 );
        HookInstall(HOOKPOS_CStreamingLoadRequestedModels_EU, (DWORD)HOOK_CStreamingLoadRequestedModels, 5 );
        RETURN_FreezeFix_Misc15_BOTH = RETURN_FreezeFix_Misc15_EU;
        RETURN_CrashFix_Misc17a_BOTH = RETURN_CrashFix_Misc17a_EU;
        RETURN_CrashFix_Misc17b_BOTH = RETURN_CrashFix_Misc17b_EU;
        RETURN_CrashFix_Misc19a_BOTH = RETURN_CrashFix_Misc19a_EU;
        RETURN_CrashFix_Misc19b_BOTH = RETURN_CrashFix_Misc19b_EU;
        RETURN_CrashFix_Misc24_BOTH = RETURN_CrashFix_Misc24_EU;
        RETURN_CheckAnimMatrix_BOTH = RETURN_CheckAnimMatrix_EU;
        RETURN_CStreamingLoadRequestedModelsa_BOTH = RETURN_CStreamingLoadRequestedModelsa_EU;
        RETURN_CStreamingLoadRequestedModelsb_BOTH = RETURN_CStreamingLoadRequestedModelsb_EU;
    }
    HookInstall(HOOKPOS_CrashFix_Misc16, (DWORD)HOOK_CrashFix_Misc16, 6 );
    HookInstall(HOOKPOS_CrashFix_Misc18, (DWORD)HOOK_CrashFix_Misc18, 7 );
    HookInstall(HOOKPOS_CrashFix_Misc20, (DWORD)HOOK_CrashFix_Misc20, 6 );
    HookInstall(HOOKPOS_CrashFix_Misc21, (DWORD)HOOK_CrashFix_Misc21, 6 );
    HookInstall(HOOKPOS_CrashFix_Misc22, (DWORD)HOOK_CrashFix_Misc22, 6 );
    HookInstall(HOOKPOS_CrashFix_Misc23, (DWORD)HOOK_CrashFix_Misc23, 7 );

    HookInstall(HOOKPOS_VehColCB, (DWORD)HOOK_VehColCB, 29 );
    HookInstall(HOOKPOS_VehCol, (DWORD)HOOK_VehCol, 9 );
    HookInstall(HOOKPOS_PreHUDRender, (DWORD)HOOK_PreHUDRender, 5 );
    HookInstall(HOOKPOS_CAutomobile__ProcessSwingingDoor, (DWORD)HOOK_CAutomobile__ProcessSwingingDoor, 7 );
    HookInstall(HOOKPOS_LoadingPlayerImgDir, (DWORD)HOOK_LoadingPlayerImgDir, 5 );
    HookInstall(HOOKPOS_CallCStreamingInfoAddToList, (DWORD)HOOK_CallCStreamingInfoAddToList, 5 );

    HookInstall(HOOKPOS_CHandlingData_isNotRWD, (DWORD)HOOK_isVehDriveTypeNotRWD, 7 );
    HookInstall(HOOKPOS_CHandlingData_isNotFWD, (DWORD)HOOK_isVehDriveTypeNotFWD, 7 );

    HookInstallCall ( CALL_CBike_ProcessRiderAnims, (DWORD)HOOK_CBike_ProcessRiderAnims );
    HookInstallCall ( CALL_Render3DStuff, (DWORD)HOOK_Render3DStuff );
    HookInstallCall ( CALL_VehicleCamUp, (DWORD)HOOK_VehicleCamUp );
    HookInstallCall ( CALL_VehicleLookBehindUp, (DWORD)HOOK_VehicleCamUp );
    HookInstallCall ( CALL_VehicleLookAsideUp, (DWORD)HOOK_VehicleCamUp );

    HookInstallCall ( CALL_CTrafficLights_GetPrimaryLightState, (DWORD)HOOK_CTrafficLights_GetPrimaryLightState);
    HookInstallCall ( CALL_CTrafficLights_GetSecondaryLightState, (DWORD)HOOK_CTrafficLights_GetSecondaryLightState);

    HookInstall(HOOKPOS_CEntity_IsOnScreen_FixObjectsScale, (DWORD)HOOK_CEntity_IsOnScreen_FixObjectScale, 7);



    // Start of Building removal hooks
    HookInstallCall ( HOOKPOS_LoadIPLInstance, (DWORD)HOOK_LoadIPLInstance );

    HookInstallCall ( HOOKPOS_CWorld_LOD_SETUP, (DWORD)HOOK_CWorld_LOD_SETUP );

    HookInstallCall ( HOOKPOS_CWorld_LOD_SETUP2, (DWORD)HOOK_CWorld_LOD_SETUP );

    HookInstall ( HOOKPOS_CBuilding_DTR, (DWORD)Hook_CBuilding_DTR, 5 );

    HookInstall ( HOOKPOS_CDummy_DTR, (DWORD)Hook_CDummy_DTR, 5 );

    HookInstall ( HOOKPOS_CObject_DTR, (DWORD)Hook_CObject_DTR, 6 );

    HookInstallCall ( HOOKPOS_AddBuildingInstancesToWorld_CWorldAdd, (DWORD)Hook_AddBuildingInstancesToWorld );

    HookInstallCall( HOOKPOS_CWorld_ADD_CPopulation_ConvertToRealObject, (DWORD)Hook_CWorld_ADD_CPopulation_ConvertToRealObject );

    HookInstallCall( HOOKPOS_CWorld_Remove_CPopulation_ConvertToDummyObject, (DWORD)HOOK_CWorld_Remove_CPopulation_ConvertToDummyObject );

    HookInstall ( HOOKPOS_CWorld_ADD_CPopulation_ConvertToDummyObject, (DWORD)HOOK_CWorld_Add_CPopulation_ConvertToDummyObject, 6 );
    
    HookInstall ( HOOKPOS_ConvertToObject_CPopulationManageDummy, (DWORD)HOOK_ConvertToObject_CPopulationManageDummy, 6 );
    // End of building removal hooks

    // Vehicle Collision Event Hooks
    HookInstall ( HOOKPOS_CEventVehicleDamageCollision, (DWORD)HOOK_CEventVehicleDamageCollision, 6 );

    HookInstall ( HOOKPOS_CEventVehicleDamageCollision_Plane, (DWORD)HOOK_CEventVehicleDamageCollision_Plane, 5 );

    HookInstall ( HOOKPOS_CEventVehicleDamageCollision_Bike, (DWORD)HOOK_CEventVehicleDamageCollision_Bike, 6 );
    // End of Vehicle Collision Event Hooks

    // Spider CJ fix
    HookInstall ( HOOKPOS_CClothes_RebuildPlayer, (DWORD)HOOK_CClothes_RebuildPlayer, 8 );

    // Fix for projectiles firing too fast locally.
    HookInstallCall ( (DWORD)HOOKPOS_CProjectileInfo_FindPlayerPed, (DWORD)HOOK_CProjectileInfo_Update_FindLocalPlayer_FindLocalPlayerVehicle );
    HookInstallCall ( (DWORD)HOOKPOS_CProjectileInfo_FindPlayerVehicle, (DWORD)HOOK_CProjectileInfo_Update_FindLocalPlayer_FindLocalPlayerVehicle );

    HookInstall( (DWORD)HOOKPOS_CHeli_ProcessHeliKill, (DWORD)HOOK_CHeli_ProcessHeliKill, 6);

    // Disable GTA setting g_bGotFocus to false when we minimize
    MemSet ( (void *)ADDR_GotFocus, 0x90, pGameInterface->GetGameVersion () == VERSION_EU_10 ? 6 : 10 );

    // Increase double link limit from 3200 ro 8000
    MemPut < int > ( 0x00550F82, 8000 );


    // Disable GTA being able to call CAudio::StopRadio ()
    // Well this isn't really CAudio::StopRadio, it's some global class
    // func that StopRadio just jumps to.
    MemPut < BYTE > ( 0x4E9820, 0xC2 );
    MemPut < BYTE > ( 0x4E9821, 0x08 );
    MemPut < BYTE > ( 0x4E9822, 0x00 );

    // Disable GTA being able to call CAudio::StartRadio ()
    MemPut < BYTE > ( 0x4DBEC0, 0xC2 );
    MemPut < BYTE > ( 0x4DBEC1, 0x00 );
    MemPut < BYTE > ( 0x4DBEC2, 0x00 );

    MemPut < BYTE > ( 0x4EB3C0, 0xC2 );
    MemPut < BYTE > ( 0x4EB3C1, 0x10 );
    MemPut < BYTE > ( 0x4EB3C2, 0x00 );
    
    // DISABLE cinematic camera for trains
    MemPut < BYTE > ( 0x52A535, 0 );

    // DISABLE wanted levels for military zones
    MemPut < BYTE > ( 0x72DF0D, 0xEB );

    // THROWN projectiles throw more accurately
    MemPut < BYTE > ( 0x742685, 0x90 );
    MemPut < BYTE > ( 0x742686, 0xE9 );

    // DISABLE CProjectileInfo::RemoveAllProjectiles
    MemPut < BYTE > ( 0x7399B0, 0xC3 );

    // DISABLE CRoadBlocks::GenerateRoadblocks
    MemPut < BYTE > ( 0x4629E0, 0xC3 );


    // Temporary hack for disabling hand up
    /*
    MemPut < BYTE > ( 0x62AEE7, 0x90 );
    MemPut < BYTE > ( 0x62AEE8, 0x90 );
    MemPut < BYTE > ( 0x62AEE9, 0x90 );
    MemPut < BYTE > ( 0x62AEEA, 0x90 );
    MemPut < BYTE > ( 0x62AEEB, 0x90 );
    MemPut < BYTE > ( 0x62AEEC, 0x90 );
    */

    // DISABLE CAERadioTrackManager::CheckForMissionStatsChanges(void) (special DJ banter)
    MemPut < BYTE > ( 0x4E8410, 0xC3 );

    // DISABLE CPopulation__AddToPopulation
    MemPut < BYTE > ( 0x614720, 0x32 );
    MemPut < BYTE > ( 0x614721, 0xC0 );
    MemPut < BYTE > ( 0x614722, 0xC3 );

    // Disables deletion of RenderWare objects during unloading of ModelInfo
    // This is used so we can circumvent the limit of ~21 different vehicles by managing the RwObject ourselves
    //*(BYTE *)0x4C9890 = 0xC3;

    //MemSet ( (void*)0x408A1B, 0x90, 5 );

    // Hack to make the choke task use 0 time left remaining when he starts t
    // just stand there looking. So he won't do that.
    MemPut < unsigned char > ( 0x620607, 0x33 );
    MemPut < unsigned char > ( 0x620608, 0xC0 );

    MemPut < unsigned char > ( 0x620618, 0x33 );
    MemPut < unsigned char > ( 0x620619, 0xC0 );
    MemPut < unsigned char > ( 0x62061A, 0x90 );
    MemPut < unsigned char > ( 0x62061B, 0x90 );
    MemPut < unsigned char > ( 0x62061C, 0x90 );

    // Hack to make non-local players always update their aim on akimbo weapons using camera
    // so they don't freeze when local player doesn't aim.
    MemPut < BYTE > ( 0x61EFFE, 0xEB );
    

    // DISABLE CGameLogic__SetPlayerWantedLevelForForbiddenTerritories
    MemPut < BYTE > ( 0x441770, 0xC3 );

    // DISABLE CCrime__ReportCrime
    MemPut < BYTE > ( 0x532010, 0xC3 );
    
    // Disables deletion of RenderWare objects during unloading of ModelInfo
    // This is used so we can circumvent the limit of ~21 different vehicles by managing the RwObject ourselves
    //*(BYTE *)0x4C9890 = 0xC3;

    /*
    004C021D   B0 00            MOV AL,0
    004C021F   90               NOP
    004C0220   90               NOP
    004C0221   90               NOP
    */
    MemPut < BYTE > ( 0x4C01F0, 0xB0 );
    MemPut < BYTE > ( 0x4C01F1, 0x00 );
    MemPut < BYTE > ( 0x4C01F2, 0x90 );
    MemPut < BYTE > ( 0x4C01F3, 0x90 );
    MemPut < BYTE > ( 0x4C01F4, 0x90 );

    // Disable MakePlayerSafe
    MemPut < BYTE > ( 0x56E870, 0xC2 );
    MemPut < BYTE > ( 0x56E871, 0x08 );
    MemPut < BYTE > ( 0x56E872, 0x00 );

    // Disable call to FxSystem_c__GetCompositeMatrix in CAEFireAudioEntity::UpdateParameters 
    // that was causing a crash - spent ages debugging, the crash happens if you create 40 or 
    // so vehicles that catch fire (upside down) then delete them, repeating a few times.
    MemSet ((void*)0x4DCF87,0x90,6);
    
    /*
    // DISABLE CPed__RemoveBodyPart
    MemPut < BYTE > ( 0x5F0140, 0xC2 );
    MemPut < BYTE > ( 0x5F0141, 0x08 );
    MemPut < BYTE > ( 0x5F0142, 0x00 );
    */

    // ALLOW picking up of all vehicles (GTA doesn't allow picking up of 'locked' script created vehicles)
    MemPut < BYTE > ( 0x6A436C, 0x90 );
    MemPut < BYTE > ( 0x6A436D, 0x90 );

    // MAKE CEntity::GetIsOnScreen always return true, experimental
   /*
    MemPut < BYTE > ( 0x534540, 0xB0 );
    MemPut < BYTE > ( 0x534541, 0x01 );
    MemPut < BYTE > ( 0x534542, 0xC3 );
    */

    //DISABLE CPad::ReconcileTwoControllersInput
    /*
    MemPut < BYTE > ( 0x53F530, 0xC2 );
    MemPut < BYTE > ( 0x53F531, 0x0C );
    MemPut < BYTE > ( 0x53F532, 0x00 );

    MemPut < BYTE > ( 0x53EF80, 0xC3 );

    MemPut < BYTE > ( 0x541DDC, 0xEB );
    MemPut < BYTE > ( 0x541DDD, 0x60 );
*/
    // DISABLE big buildings (test)
    /*
    MemPut < char > ( 0x533150, 0xC3 );
    */
    
    // PREVENT THE RADIO OR ENGINE STOPPING WHEN PLAYER LEAVES VEHICLE
    // THIS ON ITS OWN will cause sounds to be left behind and other artifacts
    /*
    MemPut < char > ( 0x4FB8C0, 0xC3 );
    */


/*  
    MemSet ((void *)0x4FBA3E, 0x90, 5);
    */
    

    // DISABLE REPLAYS
/*  
    MemSet ((void *)0x460500, 0xC3, 1);
*/
    // PREVENT the game from making dummy objects (may fix a crash, guesswork really)
    // This seems to work, but doesn't actually fix anything. Maybe a reason to do it in the future.
    //00615FE3     EB 09          JMP SHORT gta_sa_u.00615FEE
    /*
    MemSet ((void *)0x615FE3, 0xEB, 1);
    */

    // Make combines eat players *untested*
    //MemSet ( (LPVOID)0x6A9739, 0x90, 6 );
    
    // Players always lean out whatever the camera mode
    // 00621983     EB 13          JMP SHORT hacked_g.00621998
    MemPut < BYTE > ( 0x621983, 0xEB );

    
    // Players can fire drivebys whatever camera mode
    // 627E01 - 6 bytes
    MemSet ( (LPVOID)0x627E01, 0x90, 6 );

    MemSet ( (LPVOID)0x62840D, 0x90, 6 );

    // Satchel crash fix
    // C89110: satchel (bomb) positions pointer?
    // C891A8+4: satchel (model) positions pointer? gets set to NULL on player death, causing an access violation
    // C891A8+12: satchel (model) disappear time (in SystemTime format). 738F99 clears the satchel when VAR_SystemTime is larger.
    MemSet ( (LPVOID)0x738F3A, 0x90, 83 );

    // Prevent gta stopping driveby players from falling off
    MemSet ( (LPVOID)0x6B5B17, 0x90, 6 );

    // Increase VehicleStruct pool size
    MemPut < BYTE > ( 0x5B8FE4, 0x7F );
    
    /*
    // CTaskSimpleCarDrive: Swaps driveby for gang-driveby for drivers
    MemSet ( (LPVOID)0x6446A7, 0x90, 6 );
    
    // CTaskSimpleCarDrive: Swaps driveby for gang-driveby for passengers
    MemSet ( (LPVOID)0x6446BD, 0x90, 6 );
    */
    

    // DISABLE PLAYING REPLAYS
    MemSet ((void *)0x460390, 0xC3, 1);

    MemSet ((void *)0x4600F0, 0xC3, 1);

    MemSet ((void *)0x45F050, 0xC3, 1);

    // DISABLE CHEATS
    MemSet ((void *)0x439AF0, 0xC3, 1);
        
    MemSet ((void *)0x438370, 0xC3, 1);


    // DISABLE GARAGES
    MemPut < BYTE > ( 0x44AA89 + 0, 0xE9 );
    MemPut < BYTE > ( 0x44AA89 + 1, 0x28 );
    MemPut < BYTE > ( 0x44AA89 + 2, 0x01 );
    MemPut < BYTE > ( 0x44AA89 + 3, 0x00 );
    MemPut < BYTE > ( 0x44AA89 + 4, 0x00 );
    MemPut < BYTE > ( 0x44AA89 + 5, 0x90 );

    MemPut < DWORD > ( 0x44C7E0, 0x44C7C4 );
    MemPut < DWORD > ( 0x44C7E4, 0x44C7C4 );
    MemPut < DWORD > ( 0x44C7F8, 0x44C7C4 );
    MemPut < DWORD > ( 0x44C7FC, 0x44C7C4 );
    MemPut < DWORD > ( 0x44C804, 0x44C7C4 );
    MemPut < DWORD > ( 0x44C808, 0x44C7C4 );
    MemPut < DWORD > ( 0x44C83C, 0x44C7C4 );
    MemPut < DWORD > ( 0x44C840, 0x44C7C4 );
    MemPut < DWORD > ( 0x44C850, 0x44C7C4 );
    MemPut < DWORD > ( 0x44C854, 0x44C7C4 );
    MemPut < DWORD > ( 0x44C864, 0x44C7C4 );
    MemPut < DWORD > ( 0x44C868, 0x44C7C4 );
    MemPut < DWORD > ( 0x44C874, 0x44C7C4 );
    MemPut < DWORD > ( 0x44C878, 0x44C7C4 );
    MemPut < DWORD > ( 0x44C88C, 0x44C7C4 );
    MemPut < DWORD > ( 0x44C890, 0x44C7C4 );
    MemPut < DWORD > ( 0x44C89C, 0x44C7C4 );
    MemPut < DWORD > ( 0x44C8A0, 0x44C7C4 );
    MemPut < DWORD > ( 0x44C8AC, 0x44C7C4 );
    MemPut < DWORD > ( 0x44C8B0, 0x44C7C4 );

    MemPut < BYTE > ( 0x44C39A + 0, 0x0F );
    MemPut < BYTE > ( 0x44C39A + 1, 0x84 );
    MemPut < BYTE > ( 0x44C39A + 2, 0x24 );
    MemPut < BYTE > ( 0x44C39A + 3, 0x04 );
    MemPut < BYTE > ( 0x44C39A + 4, 0x00 );
    MemPut < BYTE > ( 0x44C39A + 5, 0x00 );

    // Avoid garage doors closing when you change your model
    MemSet ((LPVOID)0x4486F7, 0x90, 4);
    

    // Disable CStats::IncrementStat (returns at start of function)
    MemPut < BYTE > ( 0x55C180, 0xC3 );
    /*
    MemSet ((void *)0x55C1A9, 0x90, 14 );
    MemSet ((void *)0x55C1DD, 0x90, 7 );
    */

    // DISABLE STATS DECREMENTING
    MemSet ((void *)0x559FD5, 0x90, 7 );
    MemSet ((void *)0x559FEB, 0x90, 7 );

    // DISABLE STATS MESSAGES
    MemSet ((void *)0x55B980, 0xC3, 1);

    MemSet ((void *)0x559760, 0xC3, 1);

    // ALLOW more than 8 players (crash with more if this isn't done)
    //0060D64D   90               NOP
    //0060D64E   E9 9C000000      JMP gta_sa.0060D6EF
    MemPut < BYTE > ( 0x60D64D, 0x90 );
    MemPut < BYTE > ( 0x60D64E, 0xE9 );

    // PREVENT CJ smoking and drinking like an addict
    //005FBA26   EB 29            JMP SHORT gta_sa.005FBA51
    MemPut < BYTE > ( 0x5FBA26, 0xEB );

    // PREVENT the camera from messing up for drivebys for vehicle drivers
    MemPut < BYTE > ( 0x522423, 0x90 );
    MemPut < BYTE > ( 0x522424, 0x90 );
    
    LPVOID patchAddress = NULL;
    // ALLOW ALT+TABBING WITHOUT PAUSING
    //if ( pGameInterface->GetGameVersion() == GAME_VERSION_US ) // won't work as pGameInterface isn't inited
    if ( *(BYTE *)0x748ADD == 0xFF && *(BYTE *)0x748ADE == 0x53 )
        patchAddress = (LPVOID)0x748A8D;
    else
        patchAddress = (LPVOID)0x748ADD;

    MemSet (patchAddress, 0x90, 6);

    // CENTER VEHICLE NAME and ZONE NAME messages
    // 0058B0AD   6A 02            PUSH 2 // orientation
    // VEHICLE
    MemPut < BYTE > ( 0x58B0AE, 0x00 );

    // ZONE
    MemPut < BYTE > ( 0x58AD56, 0x00 );

    // 85953C needs to equal 320.0 to center the text (640.0 being the base width)
    MemPut < float > ( 0x85953C, 320.0f );

    // 0058B147   D80D 0C958500    FMUL DWORD PTR DS:[85950C] // the text needs to be moved to the left
    //VEHICLE
    MemPut < BYTE > ( 0x58B149, 0x3C );

    //ZONE
    MemPut < BYTE > ( 0x58AE52, 0x3C );

    // DISABLE SAM SITES
    MemPut < BYTE > ( 0x5A07D0, 0xC3 );

    // DISABLE TRAINS (AUTO GENERATED ONES)
    MemPut < BYTE > ( 0x6F7900, 0xC3 );
    
    // Prevent TRAINS spawning with PEDs
    MemPut < BYTE > ( 0x6F7865, 0xEB );

    // DISABLE PLANES
    MemPut < BYTE > ( 0x6CD2F0, 0xC3 );
    
    // DISABLE EMERGENCY VEHICLES
    MemPut < BYTE > ( 0x42B7D0, 0xC3 );

    // DISABLE CAR GENERATORS
    MemPut < BYTE > ( 0x6F3F40, 0xC3 );

    // DISABLE CEntryExitManager::Update (they crash when you enter anyway)
    MemPut < BYTE > ( 0x440D10, 0xC3 );

    // Disable MENU AFTER alt + tab
    //0053BC72   C605 7B67BA00 01 MOV BYTE PTR DS:[BA677B],1    
    MemPut < BYTE > ( 0x53BC78, 0x00 );

    // DISABLE HUNGER MESSAGES
    MemSet ( (LPVOID)0x56E740, 0x90, 5 );

    // DISABLE RANDOM VEHICLE UPGRADES
    MemSet ( (LPVOID)0x6B0BC2, 0xEB, 1 );

    // DISABLE CPOPULATION::UPDATE - DOES NOT prevent vehicles - only on-foot peds
    //MemPut < BYTE > ( 0x616650, 0xC3 );    Problem - Stops streetlamps being turned into collidable objects when streamed in
    // This sets the 'Replay Is Playing' flag
    //MemPutFast < BYTE > ( 0xA43088, 1 );   Problem - Stops streetlamps being turned into collidable objects when streamed in

    // SORT OF HACK to make peds always walk around, even when in free-camera mode (in the editor)
    MemPut < BYTE > ( 0x53C017, 0x90 );
    MemPut < BYTE > ( 0x53C018, 0x90 );

    // DISABLE random cars
    //*(BYTE *)0x4341C0 = 0xC3;
    
    // DISABLE heat flashes
    /*
    MemPut < BYTE > ( 0x6E3521, 0x90 );
    MemPut < BYTE > ( 0x6E3522, 0xE9 );
    */

    // DECREASE ROF for missiles from hydra
    // 006D462C     81E1 E8030000  AND ECX,3E8
    // 006D4632     81C1 E8030000  ADD ECX,3E8
    /*  
    MemPut < BYTE > ( 0x6D462E, 0xE8 );
    MemPut < BYTE > ( 0x6D462F, 0x03 );
    MemPut < BYTE > ( 0x6D4634, 0xE8 );
    MemPut < BYTE > ( 0x6D4635, 0x03 );
    */

    // HACK to allow boats to be rotated
    /*
    006F2089   58               POP EAX
    006F208A   90               NOP
    006F208B   90               NOP
    006F208C   90               NOP
    006F208D   90               NOP
    */
    MemPut < BYTE > ( 0x6F2089, 0x58 );
    MemSet ((void *)0x6F208A,0x90,4);

    // Prevent the game deleting _any_ far away vehicles - will cause issues for population vehicles in the future
    MemPut < BYTE > ( 0x42CD10, 0xC3 );

    // DISABLE real-time shadows for peds
    MemPut < BYTE > ( 0x5E68A0, 0xEB );

    // and some more, just to be safe
    //00542483   EB 0B            JMP SHORT gta_sa.00542490
    MemPut < BYTE > ( 0x542483, 0xEB );

    // DISABLE weapon pickups
    MemPut < BYTE > ( 0x5B47B0, 0xC3 );

    // INCREASE CEntyInfoNode pool size
    //00550FB9   68 F4010000      PUSH 1F4
    /*
    MemPut < BYTE > ( 0x550FBA, 0xE8 );
    MemPut < BYTE > ( 0x550FBB, 0x03 );
    */
    MemPut < BYTE > ( 0x550FBA, 0x00 );
    MemPut < BYTE > ( 0x550FBB, 0x10 );

    
    /*
    MemPut < BYTE > ( 0x469F00, 0xC3 );
    */

    // CCAM::PROCESSFIXED remover
/*
    MemPut < BYTE > ( 0x51D470, 0xC2 );
    MemPut < BYTE > ( 0x51D471, 0x10 );
    MemPut < BYTE > ( 0x51D472, 0x00 );
*/

    // HACK to prevent RealTimeShadowManager crash
    // 00542483     EB 0B          JMP SHORT gta_sa_u.00542490
    /*
    MemPut < BYTE > ( 0x542483, 0xEB );
*/
    
    //InitShotsyncHooks();

    //DISABLE CPad::ReconcileTwoControllersInput
    MemPut < BYTE > ( 0x53F530, 0xC2 );
    MemPut < BYTE > ( 0x53F531, 0x0C );
    MemPut < BYTE > ( 0x53F532, 0x00 );

    MemPut < BYTE > ( 0x53EF80, 0xC3 );

    MemPut < BYTE > ( 0x541DDC, 0xEB );
    MemPut < BYTE > ( 0x541DDD, 0x60 );

    // DISABLE CWanted Helis (always return 0 from CWanted::NumOfHelisRequired)
    MemPut < BYTE > ( 0x561FA4, 0x90 );
    MemPut < BYTE > ( 0x561FA5, 0x90 );

    // DISABLE  CWanted__UpdateEachFrame
    MemSet ( (void*)0x53BFF6, 0x90, 5 );

    // DISABLE CWanted__Update
    MemSet ( (void*)0x60EBCC, 0x90, 5 );

    // Disable armour-increase upon entering an enforcer
    MemPut < BYTE > ( 0x6D189B, 0x06 );

    // Removes the last weapon pickups from interiors as well
    MemPut < BYTE > ( 0x591F90, 0xC3 );

    // Trains may infact go further than Los Santos
    MemPut < BYTE > ( 0x4418E0, 0xC3 );

    // EXPERIMENTAL - disable unloading of cols
   // MemSet ( (void*)0x4C4EDA, 0x90, 10 );

    // Make CTaskComplexSunbathe::CanSunbathe always return true
    MemPut < BYTE > ( 0x632140, 0xB0 );
    MemPut < BYTE > ( 0x632141, 0x01 );
    MemPut < BYTE > ( 0x632142, 0xC3 );
    
    // Stop CTaskSimpleCarDrive::ProcessPed from exiting passengers with CTaskComplexSequence (some timer check)
    MemPut < BYTE > ( 0x644C18, 0x90 );
    MemPut < BYTE > ( 0x644C19, 0xE9 );

    // Stop CPlayerPed::ProcessControl from calling CVisibilityPlugins::SetClumpAlpha
    MemSet ( (void*)0x5E8E84, 0x90, 5 );

    // Stop CVehicle::UpdateClumpAlpha from calling CVisibilityPlugins::SetClumpAlpha
    MemSet ( (void*)0x6D29CB, 0x90, 5 );

    // Disable CVehicle::DoDriveByShootings
    MemSet ( (void*)0x741FD0, 0x90, 3 );
    MemPut < BYTE > ( 0x741FD0, 0xC3 );

    // Disable CTaskSimplePlayerOnFoot::PlayIdleAnimations (ret 4)
    MemPut < BYTE > ( 0x6872C0, 0xC2 );
    MemPut < BYTE > ( 0x6872C1, 0x04 );
    MemPut < BYTE > ( 0x6872C2, 0x00 );

    /*
    // Disable forcing of ped animations to the player one in CPlayerPed::ProcessAnimGroups
    MemSet ( (LPVOID)0x609A44, 0x90, 21 );
    */

    // Let us sprint everywhere (always return 0 from CSurfaceData::isSprint)
    MemPut < DWORD > ( 0x55E870, 0xC2C03366 );
    MemPut < WORD > ( 0x55E874, 0x0004 );

    // Create pickup objects in interior 0 instead of 13
    MemPut < BYTE > ( 0x59FAA3, 0x00 );

    // Don't get shotguns from police cars
    MemPut < BYTE > ( 0x6D19CD, 0xEB );

    // Don't get golf clubs from caddies
    MemPut < BYTE > ( 0x6D1A1A, 0xEB );

    // Don't get 20 health from ambulances
    MemPut < BYTE > ( 0x6D1762, 0x00 );

    // Prevent CVehicle::RecalcTrainRailPosition from changing train speed
    MemSet ((void *)0x6F701D, 0x90, 6);
    MemPut < BYTE > ( 0x6F7069, 0xEB );

    // The instanthit function for bullets ignores the first few bullets shot by
    // remote players after reloading because some flag isn't set (no bullet impact
    // graphics, no damage). Makes e.g. sawnoffs completely ineffective.
    // Remove this check so that no bullets are ignored.
    MemPut < BYTE > ( 0x73FDF9, 0xEB );

    // Allow turning on vehicle lights even if the engine is off
    MemSet ( (void *)0x6E1DBC, 0x90, 8 );

    // Fix vehicle back lights both using light state 3 (SA bug)
    MemPut < BYTE > ( 0x6E1D4F, 2 );

    // Fix for sliding over objects and vehicles (ice floor)
    MemPut < BYTE > ( 0x5E1E72, 0xE9 );
    MemPut < BYTE > ( 0x5E1E73, 0xB9 );
    MemPut < BYTE > ( 0x5E1E74, 0x00 );
    MemPut < BYTE > ( 0x5E1E77, 0x90 );

    // Avoid GTA setting vehicle first color to white after changing the paintjob
    MemSet ( (void *)0x6D65C5, 0x90, 11 );

    // Disable idle cam
    MemPut < BYTE > ( 0x522C80, 0xC3 );

    // Disable radar map hiding when pressing TAB (action key) while on foot
    MemSet ( (void *)0x58FC3E, 0x90, 14 );

    // No intro movies kthx
    if ( version == VERSION_US_10 )
    {
        MemPut < DWORD > ( 0x748EF8, 0x748AE7 );
        MemPut < DWORD > ( 0x748EFC, 0x748B08 );
        MemPut < BYTE > ( 0x748B0E, 5 );
    }
    else if ( version == VERSION_EU_10 )
    {
        MemPut < DWORD > ( 0x748F48, 0x748B37 );
        MemPut < DWORD > ( 0x748F4C, 0x748B58 );
        MemPut < BYTE > ( 0x748B5E, 5 );
    }

    // Force triggering of the damage event for players on fire
    MemSet ( (void *)0x633695, 0x90, 6 );
    MemPut < BYTE > ( 0x633720, 0 );

    // Make CCreepingFire::TryToStartFireAtCoors return the fire pointer rather than a bool
    MemPut < BYTE > ( 0x53A459, 0x33 );
    MemPut < BYTE > ( 0x53A568, 0x8B );
    MemPut < BYTE > ( 0x53A4A9, 0x33 );
    MemPut < WORD > ( 0x53A55F, 0x9090 );
    MemPut < BYTE > ( 0x73EC06, 0x85 );

    // Do not fixate camera behind spectated player if local player is dead
    MemPut < BYTE > ( 0x52A2BB, 0 );
    MemPut < BYTE > ( 0x52A4F8, 0 );

    // Disable setting players on fire when they're riding burning bmx's (see #4573)
    MemPut < BYTE > ( 0x53A982, 0xEB );

    // Disable stealth-kill aiming (holding knife up)
    MemSet ( (void *)0x685DFB, 0x90, 5 );
    MemPut < BYTE > ( 0x685DFB, 0x33 );
    MemPut < BYTE > ( 0x685DFC, 0xC0 );
    MemSet ( (void *)0x685C3E, 0x90, 5 );
    MemPut < BYTE > ( 0x685C3E, 0x33 );
    MemPut < BYTE > ( 0x685C3F, 0xC0 );
    MemSet ( (void *)0x685DC4, 0x90, 5 );
    MemPut < BYTE > ( 0x685DC4, 0x33 );
    MemPut < BYTE > ( 0x685DC5, 0xC0 );
    MemSet ( (void *)0x685DE6, 0x90, 5 );
    MemPut < BYTE > ( 0x685DE6, 0x33 );
    MemPut < BYTE > ( 0x685DE7, 0xC0 );

    // #4937, Disable stealth-kill rotation in CTaskSimpleStealthKill::ProcessPed
    // Used to face the dying ped away from the killer.
    MemSet ( (void *)0x62E63F, 0x90, 6 );
    MemPut < BYTE > ( 0x62E63F, 0xDD );
    MemPut < BYTE > ( 0x62E640, 0xD8 );
    MemSet ( (void *)0x62E659, 0x90, 6 );
    MemPut < BYTE > ( 0x62E659, 0xDD );
    MemPut < BYTE > ( 0x62E65A, 0xD8 );
    MemSet ( (void *)0x62E692, 0x90, 6 );
    MemPut < BYTE > ( 0x62E692, 0xDD );
    MemPut < BYTE > ( 0x62E693, 0xD8 );

    // Allow all screen aspect ratios
    MemPut < WORD > ( 0x745BC9, 0x9090 );

    // Allow all screen aspect ratios in multi-monitor dialog
    MemPut < WORD > ( 0x7459E1, 0x9090 );

    // Show the GTA:SA Main menu, this fixes some issues (#4374 and MAYBE #4000).
    // We are hiding the menu in "void CGameSA::Initialize ( void )".
    // 
    // - Sebas
    MemPutFast < BYTE > ( (0xBA6748)+0x5C, 1 );

    // Force the MrWhoopee music to load even if we are not the driver.
    MemPut < BYTE > ( 0x4F9CCE, 0xCE );

    // Disable re-initialization of DirectInput mouse device by the game
    MemPut < BYTE > ( 0x576CCC, 0xEB );
    MemPut < BYTE > ( 0x576EBA, 0xEB );
    MemPut < BYTE > ( 0x576F8A, 0xEB );

    // Make sure DirectInput mouse device is set non-exclusive (may not be needed?)
    MemPut < DWORD > ( 0x7469A0, 0x909000B0 );

    // Remove 14ms wait (Was done every other frame for some reason)
    MemPut < BYTE > ( 0x53E94C, 0x00 );

    // Disable the GTASA main menu.
    MemSet ( (void *)0x57BA57, 0x90, 6 );

    // Disable the loading screen tune.
    if ( version == VERSION_US_10 )
        MemSet ( (void *)0x748CF6, 0x90, 5 );
    else if ( version == VERSION_EU_10 )
        MemSet ( (void *)0x748D46, 0x90, 5 );

    // Do not render the loading screen.
    MemSet ( (void *)0x590D7C, 0x90, 5 );
    MemSet ( (void *)0x590DB3, 0x90, 5 );
    MemCpy ( (void *)0x590D9F, "\xC3\x90\x90\x90\x90", 5 );

    // Disable ped to player conversations.
    MemSet ( (void *)0x53C127, 0x90, 10 );

#if 0
    // Mute peds (would break setPedVoice).
    MemCpy ( (void *)0x5EFFE0, "\xC2\x18\x00\x90", 4 );
#endif

    // Clip camera also outside the world bounds.
    MemSet ( (void *)0x41AD12, 0x90, 2 );
    MemSet ( (void *)0x41ADA7, 0x90, 2 );
    MemSet ( (void *)0x41ADF3, 0x90, 2 );

    // Allow Player Garages to shut with players inside.
    MemSet ( (void *)0x44C6FA, 0x90, 4 );

    // Stop the loading of ambient traffic models and textures
    // by skipping CStreaming::StreamVehiclesAndPeds() and CStreaming::StreamZoneModels()
    MemPut < BYTE > ( 0x40E7DF, 0xEB );


    // Disable CPopulation::ManagePed
    MemPut < BYTE > ( 0x611FC0, 0xC3 );
    // Stop CPopulation::Update after ManagePopulation call
    MemPut < BYTE > ( 0x616698, 0x5E );
    MemPut < BYTE > ( 0x616699, 0xC3 );

    // Disable CReplay::Update
    MemPut < BYTE > ( 0x460500, 0xC3 );
    // Disable CInterestingEvents::ScanForNearbyEntities
    MemPut < BYTE > ( 0x605A30, 0xC3 );
    // Disable CGangWars::Update
    MemPut < BYTE > ( 0x446610, 0xC3 );
    // Disable CConversations::Update
    MemPut < BYTE > ( 0x43C590, 0xC3 );
    // Disable CPedToPlayerConversations::Update
    MemPut < BYTE > ( 0x43B0F0, 0xC3 );
    // Disable CCarCtrl::RemoveCarsIfThePoolGetsFull
    MemPut < BYTE > ( 0x4322B0, 0xC3 );
    // Disable CStreaming::StreamVehiclesAndPeds_Always
    MemPut < BYTE > ( 0x40B650, 0xC3 );

    // Double the size of CPlaceable matrix array to fix a crash after CMatrixLinkList::AddToList1
    MemPut < int > ( 0x54F3A1, 1800 );

    SetSuspensionEnabled ( true );

    // Aircraft Max Height checks are at 0x6D2614 and 0x6D2625 edit the check to use our own float.
    MemPut ( 0x6D2614, &m_fAircraftMaxHeight );
    MemPut ( 0x6D2625, &m_fAircraftMaxHeight );
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
        MemSet ( (LPVOID)ADDR_CursorHiding, 0x90, 16 );
    }
    else
    {
        MemCpy ( (LPVOID)ADDR_CursorHiding, &originalCode, 16 );
    }
}


CShotSyncData * CMultiplayerSA::GetLocalShotSyncData ( )
{
    return GetLocalPedShotSyncData();
}

void CMultiplayerSA::DisablePadHandler ( bool bDisabled )
{
    // DISABLE GAMEPADS (testing)
    if ( bDisabled )
        MemPut < BYTE > ( 0x7449F0, 0xC3 );
    else
        MemPut < BYTE > ( 0x7449F0, 0x8B );
}


void CMultiplayerSA::GetHeatHaze ( SHeatHazeSettings& settings )
{
    settings.ucIntensity = *(int*)0x8D50E8;
    settings.ucRandomShift = *(int*)0xC402C0;
    settings.usSpeedMin = *(int*)0x8D50EC;
    settings.usSpeedMax = *(int*)0x8D50F0;
    settings.sScanSizeX = *(int*)0xC40304;
    settings.sScanSizeY = *(int*)0xC40308;
    settings.usRenderSizeX = *(int*)0xC4030C;
    settings.usRenderSizeY = *(int*)0xC40310;
    settings.bInsideBuilding = *(bool*)0xC402BA;
}


void DoSetHeatHazePokes ( const SHeatHazeSettings& settings, int iHourStart, int iHourEnd, float fFadeSpeed, float fInsideBuildingFadeSpeed, bool bAllowAutoTypeChange  )
{
    MemPutFast < int > ( 0x8D50D4, iHourStart );
    MemPutFast < int > ( 0x8D50D8, iHourEnd );

    MemPutFast < float > ( 0x8D50DC, fFadeSpeed );
    MemPutFast < float > ( 0x8D50E0, fInsideBuildingFadeSpeed );

    MemPutFast < int > ( 0x8D50E8, settings.ucIntensity );
    MemPutFast < int > ( 0xC402C0, settings.ucRandomShift );
    MemPutFast < int > ( 0x8D50EC, settings.usSpeedMin );
    MemPutFast < int > ( 0x8D50F0, settings.usSpeedMax );
    MemPutFast < int > ( 0xC40304, settings.sScanSizeX );
    MemPutFast < int > ( 0xC40308, settings.sScanSizeY );
    MemPutFast < int > ( 0xC4030C, settings.usRenderSizeX );
    MemPutFast < int > ( 0xC40310, settings.usRenderSizeY );
    MemPutFast < bool > ( 0xC402BA, settings.bInsideBuilding );

    if ( bAllowAutoTypeChange )
        MemPut < BYTE > ( 0x701455, 0x83 ); // sub
    else
        MemPut < BYTE > ( 0x701455, 0xC3 ); // retn
}


void CMultiplayerSA::SetHeatHaze ( const SHeatHazeSettings& settings )
{
    if ( settings.ucIntensity != 0 )
        DoSetHeatHazePokes ( settings, 0, 24, 1.0f, 1.0f, false );    // 24 hrs
    else
        DoSetHeatHazePokes ( settings, 38, 39, 1.0f, 1.0f, false );   // 0 hrs
}


void CMultiplayerSA::ResetHeatHaze ( void )
{
    SHeatHazeSettings settings;
    settings.ucIntensity = 0x50;
    settings.ucRandomShift = 0x0;
    settings.usSpeedMin = 0x0C;
    settings.usSpeedMax = 0x12;
    settings.sScanSizeX = 0x4B;
    settings.sScanSizeY = 0x50;
    settings.usRenderSizeX = 0x50;
    settings.usRenderSizeY = 0x55;
    settings.bInsideBuilding = false;

    DoSetHeatHazePokes ( settings, 10, 19, 0.05f, 1.0f, true );   // defaults
}


void CMultiplayerSA::DisableAllVehicleWeapons ( bool bDisable )
{
    if ( bDisable )
        MemPut < BYTE > ( 0x6E3950, 0xC3 );
    else
        MemPut < BYTE > ( 0x6E3950, 0x83 );
}

void CMultiplayerSA::DisableZoneNames ( bool bDisabled )
{
    // Please use CHud::DisableAreaName instead
    pGameInterface->GetHud()->DisableAreaName ( bDisabled );
}

void CMultiplayerSA::DisableBirds ( bool bDisabled )
{
    if ( bDisabled )
    {
        // return out of render and update to make sure birds already created aren't rendered at least.. also no point in calling render if there is nothing to render I guess
        MemPut < BYTE > ( 0x712810, 0xC3 );
        MemPut < BYTE > ( 0x712330, 0xC3 );
    }
    else
    {
        // restore previous first bytes render and update to normal
        MemPut < BYTE > ( 0x712810, 0x64 );
        MemPut < BYTE > ( 0x712330, 0xA1 );
    }
}

void CMultiplayerSA::DisableQuickReload ( bool bDisabled )
{
    if ( bDisabled )
        MemPut < WORD > ( 0x60B4F6, 0x08EB );
    else
        MemPut < WORD > ( 0x60B4F6, 0x027C );
}
void CMultiplayerSA::DisableCloseRangeDamage ( bool bDisabled )
{
    if ( bDisabled )
    {
        // Change float comparison to 0.0f so SA doesn't use close range damage.
        MemPut < BYTE > ( 0x73B9FF, 0x50 );
        MemPut < BYTE > ( 0x73BA00, 0x8B );
    }
    else
    {
        // Change float comparison to 1.0f so SA uses close range damage.
        MemPut < BYTE > ( 0x73B9FF, 0x24 );
        MemPut < BYTE > ( 0x73BA00, 0x86 );

    }
}
bool CMultiplayerSA::GetInteriorSoundsEnabled ( )
{
    return bInteriorSoundsEnabled;
}

void CMultiplayerSA::SetInteriorSoundsEnabled ( bool bEnabled )
{
    // The function which should be restored when re-enabling interior sounds
    BYTE originalCode[6] = {0x89, 0x2d, 0xbc, 0xdc, 0xb6, 0x00};

    if ( bEnabled )
    {
        // Restore the function responsible for interior sounds
        MemCpy ( (LPVOID)0x508450, &originalCode, 6 );
        MemCpy ( (LPVOID)0x508817, &originalCode, 6 );
    }
    else
    {
        // Nop the function responsible for interior sounds
        MemSet ( (LPVOID)0x508450, 0x90, 6 );
        MemSet ( (LPVOID)0x508817, 0x90, 6 );
    }

    // Toggle the interior sound on/off, depending on what the scripter wants
    MemPutFast < bool > ( 0xB6DCBC, bEnabled );

    // If we just store it, we can always return the on/off state later on
    bInteriorSoundsEnabled = bEnabled;
}

void CMultiplayerSA::SetWindVelocity ( float fX, float fY, float fZ )
{
    //Disable
    MemPut < WORD > ( 0x72C616, 0xD8DD );
    MemPut < DWORD > ( 0x72C616 + 2, 0x90909090 );
    MemPut < WORD > ( 0x72C622, 0xD8DD );
    MemPut < DWORD > ( 0x72C622 + 2, 0x90909090 );
    MemPut < WORD > ( 0x72C636, 0xD8DD );
    MemPut < DWORD > ( 0x72C636 + 2, 0x90909090 );

    MemPut < WORD > ( 0x72C40C, 0xD8DD );
    MemPut < DWORD > ( 0x72C40C + 2, 0x90909090 );
    MemPut < WORD > ( 0x72C417, 0xD8DD );
    MemPut < DWORD > ( 0x72C417 + 2, 0x90909090 );
    MemPut < WORD > ( 0x72C4EF, 0xD8DD );
    MemPut < DWORD > ( 0x72C4EF + 2, 0x90909090 );

    //Set
    MemPutFast < float > ( 0xC813E0, fX );
    MemPutFast < float > ( 0xC813E4, fY );
    MemPutFast < float > ( 0xC813E8, fZ );
}

void CMultiplayerSA::GetWindVelocity ( float& fX, float& fY, float& fZ )
{
    fX = *(float *) 0xC813E0;
    fY = *(float *) 0xC813E4;
    fZ = *(float *) 0xC813E8;
}

void CMultiplayerSA::RestoreWindVelocity ( void )
{
    MemPut < WORD > ( 0x72C616, 0x1DD9 );
    MemPut < DWORD > ( 0x72C616 + 2, 0x00C813E0 );
    MemPut < WORD > ( 0x72C622, 0x1DD9 );
    MemPut < DWORD > ( 0x72C622 + 2, 0x00C813E4 );
    MemPut < WORD > ( 0x72C636, 0x1DD9 );
    MemPut < DWORD > ( 0x72C636 + 2, 0x00C813E8 );

    MemPut < WORD > ( 0x72C40C, 0x15D9 );
    MemPut < DWORD > ( 0x72C40C + 2, 0x00C813E0 );
    MemPut < WORD > ( 0x72C417, 0x1DD9 );
    MemPut < DWORD > ( 0x72C417 + 2, 0x00C813E4 );
    MemPut < WORD > ( 0x72C4EF, 0x1DD9 );
    MemPut < DWORD > ( 0x72C4EF + 2, 0x00C813E8 );
}

float CMultiplayerSA::GetFarClipDistance ( )
{
    return *(float *) 0xB7C4F0;
}

void CMultiplayerSA::SetFarClipDistance ( float fDistance )
{
    MemPut < BYTE > ( 0x55FCC8, 0xDD );
    MemPut < BYTE > ( 0x55FCC9, 0xD8 );
    MemPut < BYTE > ( 0x55FCCA, 0x90 );

    MemPut < BYTE > ( 0x5613A3, 0xDD );
    MemPut < BYTE > ( 0x5613A4, 0xD8 );
    MemPut < BYTE > ( 0x5613A5, 0x90 );

    MemPut < BYTE > ( 0x560A23, 0xDD );
    MemPut < BYTE > ( 0x560A24, 0xD8 );
    MemPut < BYTE > ( 0x560A25, 0x90 );

    MemPutFast < float > ( 0xB7C4F0, fDistance );
}

void CMultiplayerSA::RestoreFarClipDistance ( )
{
    BYTE originalFstp[3] = {0xD9, 0x5E, 0x50};

    MemCpy ( (LPVOID)0x55FCC8, &originalFstp, 3 );
    MemCpy ( (LPVOID)0x5613A3, &originalFstp, 3 );
    MemCpy ( (LPVOID)0x560A23, &originalFstp, 3 );
}

float CMultiplayerSA::GetFogDistance ( )
{
    return *(float *) 0xB7C4F4;
}

void CMultiplayerSA::SetFogDistance ( float fDistance )
{
    MemPut < BYTE > ( 0x55FCDB, 0xDD );
    MemPut < BYTE > ( 0x55FCDC, 0xD8 );
    MemPut < BYTE > ( 0x55FCDD, 0x90 );

    MemPutFast < float > ( 0xB7C4F4, fDistance );
}

void CMultiplayerSA::RestoreFogDistance ( )
{
    BYTE originalFstp[3] = {0xD9, 0x5E, 0x54};

    MemCpy ( (LPVOID)0x55FCDB, &originalFstp, 3 );
}

void CMultiplayerSA::GetSunColor ( unsigned char& ucCoreRed, unsigned char& ucCoreGreen, unsigned char& ucCoreBlue, unsigned char& ucCoronaRed, unsigned char& ucCoronaGreen, unsigned char& ucCoronaBlue)
{
    ucCoreRed   = *(BYTE *) 0xB7C4D0;
    ucCoreGreen = *(BYTE *) 0xB7C4D2;
    ucCoreBlue  = *(BYTE *) 0xB7C4D4;

    ucCoronaRed   = *(BYTE *) 0xB7C4D6;
    ucCoronaGreen = *(BYTE *) 0xB7C4D8;
    ucCoronaBlue  = *(BYTE *) 0xB7C4DA;
}

void CMultiplayerSA::SetSunColor ( unsigned char ucCoreRed, unsigned char ucCoreGreen, unsigned char ucCoreBlue, unsigned char ucCoronaRed, unsigned char ucCoronaGreen, unsigned char ucCoronaBlue )
{
    MemSet ( (LPVOID)0x55F9B2, 0x90, 4 );
    MemSet ( (LPVOID)0x55F9DD, 0x90, 4 );
    MemSet ( (LPVOID)0x55FA08, 0x90, 4 );
    MemSet ( (LPVOID)0x55FA33, 0x90, 4 );
    MemSet ( (LPVOID)0x55FA5E, 0x90, 4 );
    MemSet ( (LPVOID)0x55FA8D, 0x90, 4 );

    MemPutFast < BYTE > ( 0xB7C4D0, ucCoreRed );
    MemPutFast < BYTE > ( 0xB7C4D2, ucCoreGreen );
    MemPutFast < BYTE > ( 0xB7C4D4, ucCoreBlue );

    MemPutFast < BYTE > ( 0xB7C4D6, ucCoronaRed );
    MemPutFast < BYTE > ( 0xB7C4D8, ucCoronaGreen );
    MemPutFast < BYTE > ( 0xB7C4DA, ucCoronaBlue );
}

void CMultiplayerSA::ResetSunColor ( )
{
    BYTE originalMov[3] = {0x66, 0x89, 0x46};

    MemCpy ( (LPVOID)0x55F9B2, &originalMov, 3 );
    MemPut < BYTE > ( 0x55F9B5, 0x30 );
    MemCpy ( (LPVOID)0x55F9DD, &originalMov, 3 );
    MemPut < BYTE > ( 0x55F9E0, 0x32 );
    MemCpy ( (LPVOID)0x55FA08, &originalMov, 3 );
    MemPut < BYTE > ( 0x55FA0B, 0x34 );

    MemCpy ( (LPVOID)0x55FA33, &originalMov, 3 );
    MemPut < BYTE > ( 0x55FA36, 0x36 );
    MemCpy ( (LPVOID)0x55FA5E, &originalMov, 3 );
    MemPut < BYTE > ( 0x55FA61, 0x38 );
    MemCpy ( (LPVOID)0x55FA8D, &originalMov, 3 );
    MemPut < BYTE > ( 0x55FA90, 0x3A );
}

float CMultiplayerSA::GetSunSize ( )
{
    return *(float *)0xB7C4DC / 10;
}

void CMultiplayerSA::SetSunSize ( float fSize )
{
    MemPut < BYTE > ( 0x55FA9D, 0xDD );
    MemPut < BYTE > ( 0x55FA9E, 0xD8 );
    MemPut < BYTE > ( 0x55FA9F, 0x90 );

    MemPutFast < float > ( 0xB7C4DC, fSize * 10 );
}

void CMultiplayerSA::ResetSunSize ( )
{
    MemPut < BYTE > ( 0x55FA9D, 0xD9 );
    MemPut < BYTE > ( 0x55FA9E, 0x5E );
    MemPut < BYTE > ( 0x55FA9F, 0x3C );
}

void CMultiplayerSA::SetCloudsEnabled ( bool bDisabled )
{
    //volumetric clouds
    if ( bDisabled )
        MemPut < BYTE > ( 0x716380, 0xA1 );
    else
        MemPut < BYTE > ( 0x716380, 0xC3 );

    // normal clouds
    //0071395A     90             NOP
    if ( bDisabled )
        MemPut < BYTE > ( 0x713950, 0x83 );
    else
        MemPut < BYTE > ( 0x713950, 0xC3 );

    // plane trails (not really clouds, but they're sort of vapour)

    if ( bDisabled )
    {
        MemPut < BYTE > ( 0x717180, 0x83 );
        MemPut < BYTE > ( 0x717181, 0xEC );
        MemPut < BYTE > ( 0x717182, 0x08 );
    }
    else
    {
        MemPut < BYTE > ( 0x717180, 0xC2 );
        MemPut < BYTE > ( 0x717181, 0x04 );
        MemPut < BYTE > ( 0x717182, 0x00 );
    }
}

bool CMultiplayerSA::HasSkyColor ( )
{
    return bUsingCustomSkyGradient;
}

void CMultiplayerSA::GetSkyColor ( unsigned char& TopRed, unsigned char& TopGreen, unsigned char& TopBlue, unsigned char& BottomRed, unsigned char& BottomGreen, unsigned char& BottomBlue )
{
    if ( HasSkyColor ( ) )
    {
        TopRed  = ucSkyGradientTopR;
        TopGreen = ucSkyGradientTopG;
        TopBlue  = ucSkyGradientTopB;

        BottomRed   = ucSkyGradientBottomR;
        BottomGreen = ucSkyGradientBottomG;
        BottomBlue  = ucSkyGradientBottomB;
    }
    else
    {
        TopRed   = *(BYTE *)0xB7C4C4;
        TopGreen = *(BYTE *)0xB7C4C6;
        TopBlue  = *(BYTE *)0xB7C4C8;

        BottomRed   = *(BYTE *)0xB7C4CA;
        BottomGreen = *(BYTE *)0xB7C4CC;
        BottomBlue  = *(BYTE *)0xB7C4CE;
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

bool CMultiplayerSA::HasWaterColor ( )
{
    return bUsingCustomWaterColor;
}

void CMultiplayerSA::GetWaterColor ( float& fWaterRed, float& fWaterGreen, float& fWaterBlue, float& fWaterAlpha )
{
    if ( HasWaterColor ( ) )
    {
        fWaterRed   = fWaterColorR;
        fWaterGreen = fWaterColorG;
        fWaterBlue  = fWaterColorB;
        fWaterAlpha = fWaterColorA;
    }
    else
    {
        fWaterRed   = *(float *)0xB7C508;
        fWaterGreen = *(float *)0xB7C50C;
        fWaterBlue  = *(float *)0xB7C510;
        fWaterAlpha = *(float *)0xB7C514;
    }
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
    MemPutFast < BYTE > ( 0x8D5140, (BYTE)fWaterRed );
    MemPutFast < BYTE > ( 0x8D5141, (BYTE)fWaterGreen );
    MemPutFast < BYTE > ( 0x8D5142, (BYTE)fWaterBlue );
    MemPutFast < BYTE > ( 0x8D5143, (BYTE)fWaterAlpha );
    MemPut < BYTE > ( 0x7051A7, 255-(BYTE)fWaterAlpha );
    MemPut < float > ( 0x872660, 255-fWaterAlpha );
    MemPut < BYTE > ( 0x7051D7, 255-(BYTE)fWaterAlpha );
}

void CMultiplayerSA::ResetWater ( void )
{
    bUsingCustomWaterColor = false;
    MemPutFast < DWORD > ( 0x8D5140, 0x40404040 );
    MemPut < BYTE > ( 0x7051A7, 184 );
    MemPut < float > ( 0x872660, 184.0f );
    MemPut < BYTE > ( 0x7051D7, 184 );
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

void CMultiplayerSA::SetPreWorldProcessHandler ( PreWorldProcessHandler * pHandler )
{
    m_pPreWorldProcessHandler = pHandler;
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

void CMultiplayerSA::SetProcessCollisionHandler ( ProcessCollisionHandler * pHandler )
{
    m_pProcessCollisionHandler = pHandler;
}

void CMultiplayerSA::SetVehicleCollisionHandler ( VehicleCollisionHandler * pHandler )
{
    m_pVehicleCollisionHandler = pHandler;
}

void CMultiplayerSA::SetHeliKillHandler ( HeliKillHandler * pHandler )
{
    m_pHeliKillHandler = pHandler;
}
void CMultiplayerSA::HideRadar ( bool bHide )
{
    bHideRadar = bHide;
}

void CMultiplayerSA::AllowMouseMovement ( bool bAllow )
{
    if ( bAllow )
        MemPut < BYTE > ( 0x6194A0, 0xC3 );
    else
        MemPut < BYTE > ( 0x6194A0, 0xE9 );
}

void CMultiplayerSA::DoSoundHacksOnLostFocus ( bool bLostFocus )
{
    if ( bLostFocus )
        MemSet ( (void *)0x4D9888, 0x90, 5 );
    else
    {
        //004D9888   . E8 03 F1 FF FF    CALL gta_sa_u.004D8990
        MemPut < BYTE > ( 0x4D9888, 0xE8 );
        MemPut < BYTE > ( 0x4D9889, 0x03 );
        MemPut < BYTE > ( 0x4D988A, 0xF1 );
        MemPut < BYTE > ( 0x4D988B, 0xFF );
        MemPut < BYTE > ( 0x4D988C, 0xFF );
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
    /*  else
        {
            if ( !bActiveEntityForStreamingIsFakePed )
            {
                activeEntityForStreaming = new CPedSAInterface();
                MemSet (activeEntityForStreaming, 0, sizeof(CPedSAInterface));
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
        lea     esi, vecCenterOfWorld
        mov     edi, [esp+4]
        mov     eax, edi
        movsd
        movsd
        movsd           
        retn    

        // Continue. Don't replace the world center.
        dontset:
        pop     eax
        mov     eax, [esp+8]
        xor     edx, edx
        mov     ecx, CMultiplayerSA::HOOKPOS_FindPlayerCoors
        add     ecx, 6
        jmp     ecx
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
            mov     edi, CMultiplayerSA::FUNC_CPlayerInfoBase
            mov     ebx, [edi]
            mov     dwSavedPlayerPointer, ebx
            mov     ebx, activeEntityForStreaming
            mov     [edi], ebx
        }
    }

    _asm
    {
        mov     edi, eax

        // Call CMultiplayerSA::FUNC_CStreaming_Update
        mov     eax, CMultiplayerSA::FUNC_CStreaming_Update
        call    eax
    }

    // We have an entity for streaming?
    if ( activeEntityForStreaming )
    {
        _asm
        {
            // ...
            mov     edi, CMultiplayerSA::FUNC_CPlayerInfoBase
            mov     ebx, dwSavedPlayerPointer
            mov     [edi], ebx
        }
    }

    // We're no longer in streaming update
    bInStreamingUpdate = false;
    _asm
    {
        // Restore registers
        popad

        // Continue at the old func
        mov     eax, CMultiplayerSA::HOOKPOS_CStreaming_Update_Caller
        add     eax, 7
        jmp     eax
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

        mov     edx, CMultiplayerSA::FUNC_CAudioEngine__DisplayRadioStationName
        call    edx
    }

    if(!bSetCenterOfWorld)
    {
        _asm
        {
            mov     edx, CMultiplayerSA::FUNC_CHud_Draw
            call    edx
        }

    }
    else
    {
        /*if ( activeEntityForStreaming )
        {
            _asm
            {
                mov     edi, FUNC_CPlayerInfoBase
                mov     ebx, [edi]
                mov     dwSavedPlayerPointer, ebx
                mov     ebx, activeEntityForStreaming
                mov     [edi], ebx
            }
        }*/

        if (!bHideRadar)
        {
            _asm
            {
                mov     edx, 0x58A330
                call    edx
            }
        }

        /*if ( activeEntityForStreaming )
        {
            _asm
            {
                mov     edi, FUNC_CPlayerInfoBase
                mov     ebx, dwSavedPlayerPointer
                mov     [edi], ebx
            }
        }*/
    }

    _asm
    {
        popad

        mov     eax, CMultiplayerSA::HOOKPOS_CHud_Draw_Caller
        add     eax, 10
        jmp     eax
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

        mov     eax, [esp+4]
        test    eax, eax

        mov     edx, CMultiplayerSA::HOOKPOS_FindPlayerCentreOfWorld
        add     edx, 6
        jmp     edx
        

        hascenter:
        lea     eax, vecCenterOfWorld
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

        mov     ecx, [esp+4]
        mov     edx, ecx

        // Return to the hookpos
        mov     eax, CMultiplayerSA::HOOKPOS_FindPlayerHeading
        add     eax, 6
        jmp     eax

        // ..
        hascenter:
        fld     fFalseHeading
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
                pExplosionCreator = pGameInterface->GetPools ()->GetObject ( (DWORD*) pInterface );
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
                pExplodingEntity = pGameInterface->GetPools ()->GetObject ( (DWORD*) pExplodingEntityInterface );
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
        push    esi
        push    edi

        mov     esi, [esp+12]
        mov     explosionEntity, esi

        mov     esi, [esp+16]
        mov     explosionCreator, esi

        mov     esi, [esp+20]
        mov     explosionType, esi

        lea     edi, vecExplosionLocation
        mov     esi, esp
        add     esi, 24 // 3 DWORDS and RETURN address and 2 STORED REGISTERS
        movsd
        movsd
        movsd

        pop     edi
        pop     esi

        // Store registers for calling this handler
        pushad
    }

    // Call the explosion handler
    if ( !CallExplosionHandler () )
    {
        _asm    popad
        _asm    retn // if they return false from the handler, they don't want the explosion to show
    }
    else
    {
        _asm popad
    }

    _asm
    {
        noexplosionhandler:

        // Replaced code
        sub     esp, 0x1C
        push    ebx
        push    ebp
        push    esi

        // Return to the calling function and resume (do the explosion)
        mov     edx, CMultiplayerSA::HOOKPOS_CExplosion_AddExplosion
        add     edx, 6
        jmp     edx
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
        MemCpyFast ( pNewCreateFxSystem_Matrix, pCreateFxSystem_Matrix, 64 );

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

    if ( m_pProcessCamHandler && pCam )
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
            push    26
            lea     eax, szModelName
            push    eax
            push    0
            call    dwFunc
            add     esp, 12
        }

        dwFunc = 0x40EA10; // load all requested models
        _asm
        {
            push    1
            call    dwFunc
            add     esp, 4
        }

        dwFunc = 0x60D790; // setup player ped
        _asm
        {
            push    0
            call    dwFunc
            add     esp, 4
        }
        
        /*dwFunc = 0x05E47E0; // set created by
        _asm
        {
            mov     edi, 0xB7CD98
            mov     ecx, [edi]
            push    2
            call    dwFunc
        }

        dwFunc = 0x609520; // deactivate player ped
        _asm
        {
            push    0
            call    dwFunc
            add     esp, 4
        }
*/
        //_asm int 3
        dwFunc = 0x420B80; // set position
        fX = 2488.562f;
        fY = -1666.864f;
        fZ = 12.8757f;
        _asm
        {
            mov     edi, 0xB7CD98
            push    fZ
            push    fY
            push    fX
            mov     ecx, [edi]
            call    dwFunc
        }
        /*_asm int 3
        dwFunc = 0x609540; // reactivate player ped
        _asm
        {
            push    0
            call    dwFunc
            add     esp, 4
        }

        dwFunc = 0x61A5A0; // CTask::operator new
        _asm
        {
            push    28
            call    dwFunc
            add     esp, 4
        }

        dwFunc = 0x685750; // CTaskSimplePlayerOnFoot::CTaskSimplePlayerOnFoot
        _asm
        {
            mov     ecx, eax
            call    dwFunc
        }

        dwFunc = 0x681AF0; // set task
        _asm
        {
            mov     edi, 0xB7CD98
            mov     edi, [edi]
            mov     ecx, [edi+0x47C]
            add     ecx, 4
            push    0
            push    4   
            push    eax
            call    dwFunc
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
        MemPutFast < DWORD > ( 0x5332A2, dwCallback );
        MemPutFast < DWORD > ( 0x5332F3, dwCallback );

        // Call SetRwObjectAlpha
        DWORD dwFunc = FUNC_SetRwObjectAlpha;
        _asm
        {
            mov     ecx, dwEntity
            push    dwAlpha
            call    dwFunc
        }

        // Restore the GTA callbacks
        MemPutFast < DWORD > ( 0x5332A2, (DWORD)(0x533280) );
        MemPutFast < DWORD > ( 0x5332F3, (DWORD)(0x533280) );
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
        MemPutFast < DWORD > ( 0x5332D6, (DWORD)CVehicle_EAEG );
        SetEntityAlphaHooked ( dwAlphaEntity, (DWORD)HOOK_SetAlphaValues, 0 );
        MemPutFast < DWORD > ( 0x5332D6, 0x533290 );
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
static bool bObjectIsAGangTag = false;
static void SetObjectAlpha ()
{
    bEntityHasAlpha = false;
    bObjectIsAGangTag = false;

    if ( dwAlphaEntity )
    {
        CObject* pObject = pGameInterface->GetPools()->GetObject ( (DWORD *)dwAlphaEntity );
        if ( pObject )
        {
            if ( pObject->IsAGangTag () )
            {
                // For some weird reason, gang tags don't appear unsprayed
                // if we don't set their alpha to a value less than 255.
                bObjectIsAGangTag = true;
                GetAlphaAndSetNewValues ( SharedUtil::Min ( pObject->GetAlpha (), (unsigned char)254 ) );
             }
             else
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
        pushad
    }

    if ( bObjectIsAGangTag )
        goto render_a_tag;

    _asm
    {
        popad
        jmp         FUNC_CEntity_Render
    render_a_tag:
        popad
        // We simulate here the header of the CEntity::Render function
        // but then go straight to CTagManager::RenderTagForPC.
        push        ecx
        push        esi
        mov         eax, [esi+0x18]
        test        eax, eax
        jz          no_clump
        mov         eax, 0x534331
        jmp         eax
    no_clump:
        mov         eax, 0x5343EB
        jmp         eax
    }
}


void _cdecl DoEndWorldColorsPokes ()
{
    if ( bUsingCustomSkyGradient )
    {
        MemPutFast < BYTE > ( 0xB7C4C4, ucSkyGradientTopR );
        MemPutFast < BYTE > ( 0xB7C4C6, ucSkyGradientTopG );
        MemPutFast < BYTE > ( 0xB7C4C8, ucSkyGradientTopB );

        MemPutFast < BYTE > ( 0xB7C4CA, ucSkyGradientBottomR );
        MemPutFast < BYTE > ( 0xB7C4CC, ucSkyGradientBottomG );
        MemPutFast < BYTE > ( 0xB7C4CE, ucSkyGradientBottomB );
    }
    if ( bUsingCustomWaterColor )
    {
        MemPutFast < float > ( 0xB7C508, fWaterColorR );
        MemPutFast < float > ( 0xB7C50C, fWaterColorG );
        MemPutFast < float > ( 0xB7C510, fWaterColorB );
        MemPutFast < float > ( 0xB7C514, fWaterColorA );
    }
}

// Note: This hook is called at the end of the function that sets the world colours (sky gradient, water colour, etc).
void _declspec(naked) HOOK_EndWorldColors ()
{
     _asm
    {
        call DoEndWorldColorsPokes
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
    //  005400D0     32C0           XOR AL,AL
    //  005400D2     C3             RETN
    if ( !bDisabled )
    {
        // CPlayerInfo__Process
        MemPut < BYTE > ( 0x5702FD, 0xE8 );
        MemPut < BYTE > ( 0x5702FE, 0xCE );
        MemPut < BYTE > ( 0x5702FF, 0xFD );
        MemPut < BYTE > ( 0x570300, 0xFC );
        MemPut < BYTE > ( 0x570301, 0xFF );

        // CAutomobile__ProcessControlInputs
        MemPut < BYTE > ( 0x6AD75A, 0xE8 );
        MemPut < BYTE > ( 0x6AD75B, 0x71 );
        MemPut < BYTE > ( 0x6AD75C, 0x29 );
        MemPut < BYTE > ( 0x6AD75D, 0xE9 );
        MemPut < BYTE > ( 0x6AD75E, 0xFF );

        // CBike__ProcessControlInputs
        MemPut < BYTE > ( 0x6BE34B, 0xE8 );
        MemPut < BYTE > ( 0x6BE34C, 0x80 );
        MemPut < BYTE > ( 0x6BE34D, 0x1D );
        MemPut < BYTE > ( 0x6BE34E, 0xE8 );
        MemPut < BYTE > ( 0x6BE34F, 0xFF );

        // CTaskSimpleJetPack__ProcessControlInput
        MemPut < BYTE > ( 0x67E834, 0xE8 );
        MemPut < BYTE > ( 0x67E835, 0x97 );
        MemPut < BYTE > ( 0x67E836, 0x18 );
        MemPut < BYTE > ( 0x67E837, 0xEC );
        MemPut < BYTE > ( 0x67E838, 0xFF );
    }
    else
    {
        // CPlayerInfo__Process
        MemPut < BYTE > ( 0x5702FD, 0x32 );
        MemPut < BYTE > ( 0x5702FE, 0xC0 );
        MemPut < BYTE > ( 0x5702FF, 0x90 );
        MemPut < BYTE > ( 0x570300, 0x90 );
        MemPut < BYTE > ( 0x570301, 0x90 );

        // CAutomobile__ProcessControlInputs
        MemPut < BYTE > ( 0x6AD75A, 0x32 );
        MemPut < BYTE > ( 0x6AD75B, 0xC0 );
        MemPut < BYTE > ( 0x6AD75C, 0x90 );
        MemPut < BYTE > ( 0x6AD75D, 0x90 );
        MemPut < BYTE > ( 0x6AD75E, 0x90 );

        // CBike__ProcessControlInputs
        MemPut < BYTE > ( 0x6BE34B, 0x32 );
        MemPut < BYTE > ( 0x6BE34C, 0xC0 );
        MemPut < BYTE > ( 0x6BE34D, 0x90 );
        MemPut < BYTE > ( 0x6BE34E, 0x90 );
        MemPut < BYTE > ( 0x6BE34F, 0x90 );

        // CTaskSimpleJetPack__ProcessControlInput
        MemPut < BYTE > ( 0x67E834, 0x32 );
        MemPut < BYTE > ( 0x67E835, 0xC0 );
        MemPut < BYTE > ( 0x67E836, 0x90 );
        MemPut < BYTE > ( 0x67E837, 0x90 );
        MemPut < BYTE > ( 0x67E838, 0x90 );
    }
    
    // CPad__ExitVehicleJustDown
    if ( !bDisabled )
    {
        MemSet ((void *)0x540120, 0x90, 1);
        MemSet ((void *)0x540121, 0x90, 1);
        MemSet ((void *)0x540122, 0x90, 1);
    }
    else
    {
        MemSet ((void *)0x540120, 0x32, 1);
        MemSet ((void *)0x540121, 0xC0, 1);
        MemSet ((void *)0x540122, 0xC3, 1);
    }
}

void CMultiplayerSA::PreventLeavingVehicles()
{
    MemSet ((void *)0x6B5A10, 0xC3, 1);

    //006B7449     E9 FF000000    JMP gta_sa.006B754D
    MemSet ((void *)0x6B7449, 0xE9, 1);
    MemSet ((void *)(0x6B7449+1), 0xFF, 1);
    MemSet ((void *)(0x6B7449+2), 0x00, 1);

    //006B763C     E9 01010000    JMP gta_sa.006B7742
    MemSet ((void *)0x6B763C, 0xE9, 1);
    MemSet ((void *)(0x6B763C+1), 0x01, 1);
    MemSet ((void *)(0x6B763C+2), 0x01, 1);
    MemSet ((void *)(0x6B763C+3), 0x00, 1);

    //006B7617     E9 26010000    JMP gta_sa.006B7742
    MemSet ((void *)0x6B7617, 0xE9, 1);
    MemSet ((void *)(0x6B7617+1), 0x26, 1);
    MemSet ((void *)(0x6B7617+2), 0x01, 1);
    MemSet ((void *)(0x6B7617+3), 0x00, 1);
    MemSet ((void *)(0x6B7617+4), 0x00, 1);

    //006B62A7     EB 74          JMP SHORT gta_sa.006B631D
    MemSet ((void *)0x6B62A7, 0xEB, 1);

    //006B7642     E9 FB000000    JMP gta_sa_u.006B7742
    MemSet ((void *)0x6B7642, 0xE9, 1);
    MemSet ((void *)(0x6B7642+1), 0xFB, 1);
    MemSet ((void *)(0x6B7642+2), 0x00, 1);
    MemSet ((void *)(0x6B7642+3), 0x00, 1);
    MemSet ((void *)(0x6B7642+4), 0x00, 1);

    //006B7449     E9 FF000000    JMP gta_sa_u.006B754D
    MemSet ((void *)0x6B7449, 0xE9, 1);
    MemSet ((void *)(0x6B7449+1), 0xFF, 1);
    MemSet ((void *)(0x6B7449+2), 0x00, 1);

    // For quadbikes hitting water  
    // 006A90D8   E9 29020000      JMP gta_sa.006A9306
    MemSet ((void *)0x6A90D8, 0xE9, 1);
    MemSet ((void *)(0x6A90D8+1), 0x29, 1);
    MemSet ((void *)(0x6A90D8+2), 0x02, 1);
    MemSet ((void *)(0x6A90D8+3), 0x00, 1);
    MemSet ((void *)(0x6A90D8+4), 0x00, 1);
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

void CMultiplayerSA::SetBulletImpactHandler ( BulletImpactHandler* pHandler )
{
    m_pBulletImpactHandler = pHandler;
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
        MemCpyFast ( &localStats.StatTypesFloat, (void *)0xb79380, sizeof(float) * MAX_FLOAT_STATS );
        MemCpyFast ( &localStats.StatTypesInt, (void *)0xb79000, sizeof(int) * MAX_INT_STATS );
        MemCpyFast ( &localStats.StatReactionValue, (void *)0xb78f10, sizeof(float) * MAX_REACTION_STATS );

        // Change the local player's stats to the remote player's
        MemCpyFast ( (void *)0xb79380, data->m_stats.StatTypesFloat, sizeof(float) * MAX_FLOAT_STATS );
        MemCpyFast ( (void *)0xb79000, data->m_stats.StatTypesInt, sizeof(int) * MAX_INT_STATS );
        MemCpyFast ( (void *)0xb78f10, data->m_stats.StatReactionValue, sizeof(float) * MAX_REACTION_STATS );

        player->RebuildPlayer();

        // Restore the local player stats
        MemCpyFast ( (void *)0xb79380, &localStats.StatTypesFloat, sizeof(float) * MAX_FLOAT_STATS );
        MemCpyFast ( (void *)0xb79000, &localStats.StatTypesInt, sizeof(int) * MAX_INT_STATS );
        MemCpyFast ( (void *)0xb78f10, &localStats.StatReactionValue, sizeof(float) * MAX_REACTION_STATS );
    }
}


void CMultiplayerSA::SetNightVisionEnabled ( bool bEnabled )
{
    if ( bEnabled )
    {
        MemPutFast < BYTE > ( 0xC402B8, 1 );
    }
    else
    {
        MemPutFast < BYTE > ( 0xC402B8, 0 );
    }
}


void CMultiplayerSA::SetThermalVisionEnabled ( bool bEnabled )
{
    if ( bEnabled )
    {
        MemPutFast < BYTE > ( 0xC402B9, 1 );
    }
    else
    {
        MemPutFast < BYTE > ( 0xC402B9, 0 );
    }
}

bool CMultiplayerSA::IsNightVisionEnabled ( )
{
    return (*(BYTE *)0xC402B8 == 1 );
}

bool CMultiplayerSA::IsThermalVisionEnabled ( )
{
    return (*(BYTE *)0xC402B9 == 1 );
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

unsigned char ucDesignatedLightState = 0;
void _declspec(naked) HOOK_CTrafficLights_GetPrimaryLightState ()
{
    _asm pushad

    if ( ucTrafficLightState == 0 || ucTrafficLightState == 5 || ucTrafficLightState == 8 )
    {
        ucDesignatedLightState = 0; //Green
    }
    else if ( ucTrafficLightState == 1 || ucTrafficLightState == 6 || ucTrafficLightState == 7 )
    {
        ucDesignatedLightState = 1; //Amber
    }
    else if ( ucTrafficLightState == 9 )
    {
        ucDesignatedLightState = 4;  //Off
    }
    else
        ucDesignatedLightState = 2;  //Red

    _asm
    {
        popad
        mov al, ucDesignatedLightState
        retn
    }
}

void _declspec(naked) HOOK_CTrafficLights_GetSecondaryLightState ()
{
    _asm pushad

    if ( ucTrafficLightState == 3 || ucTrafficLightState == 5 || ucTrafficLightState == 7 )
    {
        ucDesignatedLightState = 0; //Green
    }
    else if ( ucTrafficLightState == 4 || ucTrafficLightState == 6 || ucTrafficLightState == 8 )
    {
        ucDesignatedLightState = 1; //Amber
    }
    else if ( ucTrafficLightState == 9 )
    {
        ucDesignatedLightState = 4; //Off
    }
    else
        ucDesignatedLightState = 2; //Red

    _asm
    {
        popad
        mov al, ucDesignatedLightState
        retn
    }
}

static CVehicleSAInterface* pHandlingDriveTypeVeh = NULL;
unsigned char ucDriveType = '4';
void GetVehicleDriveType()
{
    //Get the car drive type from the Vehicle interface
    ucDriveType = static_cast<unsigned char> ( pHandlingDriveTypeVeh->m_pVehicle->GetHandlingData()->GetCarDriveType() );
}

void _declspec(naked) HOOK_isVehDriveTypeNotRWD ()
{
    // Get the Vehicle interface from esi
    _asm
    {
         mov pHandlingDriveTypeVeh, esi
    }

    GetVehicleDriveType();

    // push our drive type into bl :)
    _asm
    {
        mov bl, ucDriveType
        jmp RETURN_CHandlingData_isNotRWD
    }
}

void _declspec(naked) HOOK_isVehDriveTypeNotFWD ()
{
    // Get the Vehicle SA interface from esi
    _asm
    {
         mov pHandlingDriveTypeVeh, esi
    }

    GetVehicleDriveType();

    // push our drive type into bl :)
    _asm
    {
        mov bl, ucDriveType
        jmp RETURN_CHandlingData_isNotFWD
    }
}

unsigned char CMultiplayerSA::GetTrafficLightState ()
{
    return ucTrafficLightState;
}

void CMultiplayerSA::SetTrafficLightState ( unsigned char ucState )
{
    ucTrafficLightState = ucState;
}

bool CMultiplayerSA::GetTrafficLightsLocked ()
{
    return bTrafficLightsBlocked;
}

void CMultiplayerSA::SetTrafficLightsLocked ( bool bLocked )
{
    bTrafficLightsBlocked = bLocked;
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
            MemCpyFast ( bufOriginalData, (void *)0x59FABC, 90 );

        bState = bOn;
        if ( bOn )
        {
            // Make created objects to have a control code, so they can be checked for vertical line test HOOK
            MemSet ( (void *)0x59FABC, 0x90, 90 );
        }
        else
        {
            // Make created objects not be checked for vertical line test HOOK
            MemCpy ( (void *)0x59FABC, bufOriginalData, 90 );
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
        MemSubFast < float > ( dwThis + 0x4C, fTimeStep * fGravity );
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

    MemPutFast < CVector > ( dwCam + 0x190, -gravcam_matGravity.vRight*cos(fPhi)*cos(fTheta) - gravcam_matGravity.vFront*sin(fPhi)*cos(fTheta) + gravcam_matGravity.vUp*sin(fTheta) );

    MemPutFast < float > ( 0x8CCEA8, fPhi );
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
    MemPutFast < CVector > ( dwCam + 0x19C, *pvecEntityPos + (gravcam_matVehicleTransform.vFront + gravcam_matGravity.vUp*0.2f)*fDistance );
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
    MemPutFast < CVector > ( dwCam + 0x19C, *pvecEntityPos + (-gravcam_matVehicleTransform.vRight*fDirectionFactor + gravcam_matGravity.vUp*0.2f)*fDistance );
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
        pushad
    }

    if ( m_pPreWorldProcessHandler ) m_pPreWorldProcessHandler ();

    _asm
    {
        popad
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
    MemSet ( (void *)0x6E1DBC, 0x90, 8 );

    // Fix vehicle back lights both using light state 3 (SA bug)
    MemPut < BYTE > ( 0x6E1D4F, 2 );
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
    SColor color = SColorRGBA ( 255, 255, 255, 255 );
    CVehicle * pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD *)pInterface );
    if ( pVehicle )
    {
        color = pVehicle->GetHeadLightColor ();
    }
    
    // Scale our color values to the defaults ..looks dodgy but its needed!
    ulHeadLightR = (unsigned char) Min ( 255.f, color.R * ( 1 / 255.0f ) * fR );
    ulHeadLightG = (unsigned char) Min ( 255.f, color.G * ( 1 / 255.0f ) * fG );
    ulHeadLightB = (unsigned char) Min ( 255.f, color.B * ( 1 / 255.0f ) * fB );
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

// When the water is not customized, use the default render order so water through glass looks better
void CMultiplayerSA::SetAltWaterOrderEnabled ( bool bEnable )
{
    // Switch
    if ( m_bEnabledAltWaterOrder == bEnable )
        return;
    m_bEnabledAltWaterOrder = bEnable;

    // Memory saved here
    static CBuffer savedMem;
    struct {
        DWORD dwAddress;
        uint uiSize;
    } memoryList[] = {  { 0x53DFF5, 1 },
                        { 0x53E133, 2 },
                        { 0x53E132, 1 },
                        { 0x53E156, 3 },
                        { 0x53DF4B, 4 },
                        { HOOKPOS_RenderScene_end, 5 },
                        { HOOKPOS_CPlantMgr_Render, 6 },
                        { CALL_RenderScene_Plants, 5 }, };

    // Enable or not?
    if ( bEnable )
    {
        // Save memory before we blat it
        CBufferWriteStream stream ( savedMem );
        for ( uint i = 0 ; i < NUMELMS( memoryList ) ; i++ )
            stream.WriteBytes ( (void*)memoryList[i].dwAddress, memoryList[i].uiSize );

        // Add hooks and things
        // Always render water after other entities (otherwise underwater LODs and trees are rendered
        // in front of it)
        MemPut < BYTE > ( 0x53DFF5, 0xEB );
        MemPut < WORD > ( 0x53E133, 0x9090 );
        // Disable some stack management instructions as we need ebx for a bit longer. We replicate
        // these in HOOK_RenderScene_end
        MemPut < BYTE > ( 0x53E132, 0x90 );
        MemSet ( (void *)0x53E156, 0x90, 3 );
        // Use 0.5 instead of 0.0 for underwater threshold
        MemPut < DWORD > ( 0x53DF4B, 0x858B8C );

        HookInstall(HOOKPOS_RenderScene_end, (DWORD)HOOK_RenderScene_end, 5);
        HookInstall(HOOKPOS_CPlantMgr_Render, (DWORD)HOOK_CPlantMgr_Render, 6);
        HookInstallCall ( CALL_RenderScene_Plants, (DWORD)HOOK_RenderScene_Plants );
    }
    else
    {
        // Restore memory
        CBufferReadStream stream ( savedMem );
        for ( uint i = 0 ; i < NUMELMS( memoryList ) ; i++ )
        {
            BYTE temp[10];
            assert ( sizeof(temp) >= memoryList[i].uiSize );
            stream.ReadBytes ( temp, memoryList[i].uiSize );
            MemCpy ( (void*)memoryList[i].dwAddress, temp, memoryList[i].uiSize );
        }
    }
}

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
            if ( weaponSkillWeapon >= WEAPONTYPE_PISTOL && weaponSkillWeapon <= WEAPONTYPE_TEC9 )
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

// Actually check if the ped putting on goggles is the local player before
// applying the visual effect
bool _cdecl CPed_AddGogglesModelCheck ( void* pPedInterface )
{
    return pGameInterface->GetPools ()->GetPed ( (DWORD *)pPedInterface ) == pGameInterface->GetPools ()->GetPedFromRef ( 1 );
}

void _declspec(naked) HOOK_CPed_AddGogglesModel ()
{
    _asm
    {
        push esi
        call CPed_AddGogglesModelCheck
        add esp, 4

        test al, al
        jz skip
        mov eax, [esp+0x10]
        mov [esi+0x500], eax
        mov byte ptr [eax], 1

    skip:
        jmp RETURN_CPed_AddGogglesModel
    }
}

void CMultiplayerSA::DeleteAndDisableGangTags ()
{
    static bool bDisabled = false;
    if ( !bDisabled )
    {
        bDisabled = true;

        // Destroy all the world tags
        DWORD dwFunc = FUNC_CWorld_Remove;

        for ( unsigned int i = 0; i < *VAR_NumTags; ++i )
        {
            DWORD* pTagInterface = VAR_TagInfoArray [ i << 1 ];
            if ( pTagInterface )
            {
                _asm
                {
                    push pTagInterface
                    call dwFunc
                    add esp, 4
                }
            }
        }

        dwFunc = FUNC_CTagManager_ShutdownForRestart;
        _asm call dwFunc
 
        // Disallow spraying gang tags
        // Nop the whole CTagManager::IsTag function and replace its body with:
        // xor eax, eax
        // ret
        // to make it always return false
        MemSet ( (void *)0x49CCE0, 0x90, 74 );
        MemPut < DWORD > ( 0x49CCE0, 0x90C3C033 );
        // Remove also some hardcoded and inlined checks for if it's a tag
        MemSet ( (void *)0x53374A, 0x90, 56 );
        MemPut < BYTE > ( 0x4C4403, 0xEB );

        // Force all tags to have zero tagged alpha
        //
        // Replaces:
        // call    CVisibilityPlugins::GetUserValue
        // push    esi
        // movzx   edi, al
        // call    CVisibilityPlugins::GetUserValue
        // movzx   eax, ax
        //
        // With:
        // push    esi
        // xor     eax, eax
        // xor     edi, edi
        //
        // No need to worry about the push esi, because at 0x49CE8E the stack is restored.
        // CVisibilityPlugins::GetUserValue is a cdecl.
        MemSet ( (void *)0x49CE58, 0x90, 5 );
        MemSet ( (void *)0x49CE5E, 0x90, 11 );
        MemPut < unsigned short > ( 0x49CE5E, 0xC033 );
        MemPut < unsigned short > ( 0x49CE60, 0xFF33 );
    }
}


CPhysicalSAInterface * pCollisionPhysicalThis, * pCollisionPhysical;
bool CPhysical_ProcessCollisionSectorList ()
{
    if ( pCollisionPhysicalThis && pCollisionPhysical )
    {
        if ( m_pProcessCollisionHandler && !m_pProcessCollisionHandler ( pCollisionPhysicalThis, pCollisionPhysical ) )
        {
            return false;
        }
    }
    return true;
}


void _declspec(naked) HOOK_CPhysical_ProcessCollisionSectorList ()
{
    _asm
    {
        mov     pCollisionPhysicalThis, esi
        mov     pCollisionPhysical, edi
        pushad
    }

    // Carry on with collision? (sets the CElement->bUsesCollision flag check)
    if ( CPhysical_ProcessCollisionSectorList () )
    {
        _asm
        {
            popad
            mov     ecx, [eax+4]
            test    byte ptr [edi+1Ch], 1
            jmp     RETURN_CPhysical_ProcessCollisionSectorList
        }
    }
    else
    {
        _asm
        {
            popad
            mov     ecx, [eax+4]
            mov     edi, 0
            test    edi, 1
            mov     edi, pCollisionPhysical
            jmp     RETURN_CPhysical_ProcessCollisionSectorList
        }
    }
}

//
// Test macros for CrashFixes
//
#ifdef MTA_DEBUG

#define SIMULATE_ERROR_BEGIN( chance ) \
{ \
    _asm { pushad } \
    if ( rand() % 100 < (chance) ) \
    { \
        _asm popad


#define SIMULATE_ERROR_END \
        _asm pushad \
    } \
    _asm popad \
}

#endif

#define TEST_CRASH_FIXES 0


//
// Support for crash stats
//
void OnCrashAverted ( uint uiId );

void _cdecl CrashAverted ( DWORD id )
{
    OnCrashAverted ( id );
}

#define CRASH_AVERTED(id) \
    } \
    _asm pushad \
    _asm mov eax, id \
    _asm push eax \
    _asm call CrashAverted \
    _asm add esp, 4 \
    _asm popad \
    _asm \
    {



void _declspec(naked) HOOK_CrashFix_Misc1 ()
{
    _asm
    {
        // Hooked from 0x5D9A6E
        mov     eax,dword ptr [esp+18h]
        test    eax,eax 
        je      cont

        mov     eax,dword ptr ds:[008D12CCh] 
        mov     ecx,dword ptr [eax+esi]     // If [eax+esi] is 0, it causes a crash
        test    ecx,ecx
        jne     cont
        CRASH_AVERTED( 1 )
        xor    ecx,ecx
    cont:
        jmp     RETURN_CrashFix_Misc1   // 0x5D9A74
    }
}


void _declspec(naked) HOOK_CrashFix_Misc2 ()
{
    _asm
    {
        // Hooked from 006B18B0
        test    eax,eax 
        je      cont        // Skip much code if eax is zero (vehicle has no colmodel)

        mov     eax,dword ptr [eax+2Ch] 
        mov     cl,byte ptr [esi+429h]
        jmp     RETURN_CrashFix_Misc2a
    cont:
        CRASH_AVERTED( 2 )
        jmp     RETURN_CrashFix_Misc2b
    }
}


void _declspec(naked) HOOK_CrashFix_Misc3 ()
{
    _asm
    {
        // Hooked from 00645FD9
        test    ecx,ecx 
        je      cont        // Skip much code if ecx is zero (ped has no something)

        mov     edx,dword ptr [ecx+384h]
        jmp     RETURN_CrashFix_Misc3
    cont:
        CRASH_AVERTED( 3 )
        jmp     CPlayerPed__ProcessControl_Abort
    }
}


void _declspec(naked) HOOK_CrashFix_Misc4 ()
{
    _asm
    {
        // Hooked from 004F02D2
        test    ecx,ecx 
        je      cont        // Skip much code if ecx is zero (avoid divide by zero in soundmanager::service)

        cdq  
        idiv    ecx  
        add     edx, ebp  
        jmp     RETURN_CrashFix_Misc4a
    cont:
        CRASH_AVERTED( 4 )
        jmp     RETURN_CrashFix_Misc4b
    }
}


void _declspec(naked) HOOK_CrashFix_Misc5 ()
{
    _asm
    {
        // Hooked from 005DF949
        mov     edi, dword ptr [ecx*4+0A9B0C8h]
        mov     edi, dword ptr [edi+5Ch]     
        test    edi, edi 
        je      cont        // Skip much code if edi is zero (ped has no model)

        mov     edi, dword ptr [ecx*4+0A9B0C8h]
        jmp     RETURN_CrashFix_Misc5a  // 005DF950
    cont:
        CRASH_AVERTED( 5 )
        pop edi
        jmp     RETURN_CrashFix_Misc5b  // 005DFCC4
    }
}


// #5465 2/2
void _declspec(naked) HOOK_CrashFix_Misc6 ()
{
    _asm
    {
        // Hooked from 4D1750  5 bytes
        test    ecx, ecx 
        je      cont        // Skip much code if ecx is zero (ped has no anim something)

        mov     eax, dword ptr [ecx+10h]
        test    eax, eax
        jmp     RETURN_CrashFix_Misc6a  // 004D1755
    cont:
        CRASH_AVERTED( 6 )
        jmp     RETURN_CrashFix_Misc6b  // 004D1A44
    }
}


// #5466
void _declspec(naked) HOOK_CrashFix_Misc7 ()
{
    _asm
    {
        // Hooked from 417BF8  5 bytes
        test    ecx, ecx 
        je      cont        // Skip much code if ecx is zero (no colmodel)

        mov     esi, dword ptr [ecx+2Ch] 
        test    esi, esi
        jmp     RETURN_CrashFix_Misc7a  // 417BFD
    cont:
        CRASH_AVERTED( 7 )
        jmp     RETURN_CrashFix_Misc7b  // 417BFF
    }
}


// #5468  1/3
void _declspec(naked) HOOK_CrashFix_Misc8 ()
{
    _asm
    {
        // Hooked from 73485D  5 bytes
        test    ecx, ecx 
        je      cont        // Skip much code if ecx is zero (no 2d effect plugin)

        mov     ecx, dword ptr [edx+ecx] 
        test    ecx, ecx 
        jmp     RETURN_CrashFix_Misc8a  // 734862
    cont:
        CRASH_AVERTED( 8 )
        jmp     RETURN_CrashFix_Misc8b  // 734871
    }
}


// #5468  2/3
void _declspec(naked) HOOK_CrashFix_Misc9 ()
{
    _asm
    {
        // Hooked from 738B64  6 bytes
        test    esi, esi 
        je      cont        // Skip much code if esi is zero (invalid projectile)

        mov     eax, dword ptr [esi+40h] 
        test    ah, 1
        jmp     RETURN_CrashFix_Misc9a  // 738B6A
    cont:
        CRASH_AVERTED( 9 )
        jmp     RETURN_CrashFix_Misc9b  // 73983A
    }
}


// #5468  3/3
void _declspec(naked) HOOK_CrashFix_Misc10 ()
{
    _asm
    {
        // Hooked from 5334FE  6 bytes
        cmp     ecx, 0x80
        jb      cont  // Skip much code if ecx is small (invalid vector pointer)

        mov     edx, dword ptr [ecx] 
        mov     dword ptr [esp], edx
        jmp     RETURN_CrashFix_Misc10a  // 533504
    cont:
        CRASH_AVERTED( 10 )
        mov     ecx, dword ptr [esp+1Ch]
        mov     dword ptr [ecx],0
        mov     dword ptr [ecx+4],0
        mov     dword ptr [ecx+8],0
        jmp     RETURN_CrashFix_Misc10b  //533539
    }
}


// #5576
void _declspec(naked) HOOK_CrashFix_Misc11 ()
{
    _asm
    {
        // Hooked from 0x4D2C62  5 bytes
        test    ecx, ecx 
        je      cont  // Skip much code if ecx is zero (invalid anim somthing)

        mov     eax, dword ptr [ecx+10h] 
        test    eax, eax 
        jmp     RETURN_CrashFix_Misc11a  // 4D2C67
    cont:
        CRASH_AVERTED( 11 )
        jmp     RETURN_CrashFix_Misc11b  // 4D2E03
    }
}


// #5530
void _declspec(naked) HOOK_CrashFix_Misc12 ()
{
    _asm
    {
        // Hooked from 0x4D41C5  5 bytes
        test    edi, edi 
        je      cont  // Skip much code if edi is zero (invalid anim somthing)

        mov     al, byte ptr [edi+0Bh] 
        test    al, al 
        jmp     RETURN_CrashFix_Misc12a  // 4D41CA
    cont:
        CRASH_AVERTED( 12 )
        jmp     RETURN_CrashFix_Misc12b  // 4D4222
    }
}


void _declspec(naked) HOOK_CrashFix_Misc13 ()
{
    _asm
    {
        // Hooked from 0x4D464E  6 bytes
        cmp     eax, 0x2480
        jb      cont  // Skip much code if eax is less than 0x480 (invalid anim)

        mov     al, byte ptr [eax+0Ah] 
        shr     al, 5
        jmp     RETURN_CrashFix_Misc13a  // 4D4654
    cont:
        CRASH_AVERTED( 13 )
        jmp     RETURN_CrashFix_Misc13b  // 4D478c
    }
}


// hooked at 4DD4B5
void _declspec(naked) HOOK_CrashFix_Misc14 ()
{
    _asm
    {
        mov     eax, dword ptr ds:[0BD00F8h]
        cmp     eax, 0
        je      cont  // Skip much code if eax is zero ( Audio event volumes table not initialized )

        sub     esp, 0D4h
        jmp     RETURN_CrashFix_Misc14  // 4DD4BB
    cont:
        CRASH_AVERTED( 14 )
        add     esp, 12
        retn    12
    }
}


void _cdecl DoWait ( HANDLE hHandle )
{
    DWORD dwWait = 4000;
    DWORD dwResult = WaitForSingleObject ( hHandle, dwWait );
    if ( dwResult == WAIT_TIMEOUT )
    {
        AddReportLog ( 6211, SString ( "WaitForSingleObject timed out with %08x and %dms", hHandle, dwWait ) );
        // This thread lock bug in GTA will have to be fixed one day.
        // Until then, a 5 second freeze should be long enough for the loading thread to have finished it's job.
#if 0
        _wassert ( _CRT_WIDE("\
            Press IGNORE  ('I' Key) \n\
            Press IGNORE  ('I' Key) \n\
            Press IGNORE  ('I' Key) \n\
            Press IGNORE  ('I' Key) \n\
            ")
         , _CRT_WIDE(__FILE__), __LINE__);
#endif
        dwResult = WaitForSingleObject ( hHandle, 1000 );
    }
}

// hooked at 0156CDAE - 6 bytes
void _declspec(naked) HOOK_FreezeFix_Misc15 ()
{
    _asm
    {
        pop eax
        pop edx
        pushad

        push eax
        call DoWait
        add esp, 4

        popad
        jmp     RETURN_FreezeFix_Misc15_BOTH  // 156CDB4
    }
}


// Handle RpAnimBlendClumpGetFirstAssociation returning NULL
// hooked at 5E5815 6 bytes
void _declspec(naked) HOOK_CrashFix_Misc16 ()
{
#if TEST_CRASH_FIXES
    SIMULATE_ERROR_BEGIN( 10 )
        _asm
        {
            mov     eax, 0
        }
    SIMULATE_ERROR_END
#endif

    _asm
    {
        cmp     eax, 0
        je      cont  // Skip much code if eax is zero ( RpAnimBlendClumpGetFirstAssociation returns NULL )

        // continue standard path
        movsx   ecx, word ptr [eax+2Ch]
        xor     edi, edi
        jmp     RETURN_CrashFix_Misc16  // 5E581B

    cont:
        CRASH_AVERTED( 16 )
        add     esp, 96
        retn
    }
}


// Handle RwFrameSetStaticPluginsSize having NULL member at 0x90
// hooked at 7F120E/7F124E 6 bytes
void _declspec(naked) HOOK_CrashFix_Misc17 ()
{
    _asm
    {
        cmp     eax, 0
        je      cont  // Skip much code if eax is zero

        // continue standard path
        mov     eax, [eax+90h]
        jmp     RETURN_CrashFix_Misc17a_BOTH  // 7F1214/7F1254

    cont:
        CRASH_AVERTED( 17 )
        jmp     RETURN_CrashFix_Misc17b_BOTH  // 7F1236/7F1276
    }
}


// Handle GetWheelPosition having wrong data
// hooked at 4C7DAD 7 bytes
void _declspec(naked) HOOK_CrashFix_Misc18 ()
{
#if TEST_CRASH_FIXES
    SIMULATE_ERROR_BEGIN( 10 )
        _asm
        {
            mov     ebp, 0
        }
    SIMULATE_ERROR_END
#endif

    _asm
    {
        cmp     ebp, 0
        je      cont  // Skip much code if ebp is zero

        // continue standard path
        mov         edx,dword ptr [ebp+40h] 
        mov         eax,dword ptr [esp+10h] 
        jmp     RETURN_CrashFix_Misc18  // 4C7DB4

    cont:
        CRASH_AVERTED( 18 )
        mov         edx,0 
        mov         eax,dword ptr [esp+10h]
        mov         dword ptr [eax],edx 
        mov         dword ptr [eax+4],edx 
        pop         esi  
        mov         dword ptr [eax+8],edx 
        pop         ebp  
        ret         0Ch  
    }
}



// Handle RwFrameCloneHierarchy having wrong data
// hooked at 7F0BF7/7F0C37 6 bytes
void _declspec(naked) HOOK_CrashFix_Misc19 ()
{
    _asm
    {
        cmp     esi, 0
        je      cont  // Skip much code if esi is zero

        // continue standard path
        mov     eax, [esi+98h] 
        jmp     RETURN_CrashFix_Misc19a_BOTH  // 7F0BFD/7F0C3D

    cont:
        CRASH_AVERTED( 19 )
        mov         edx,dword ptr [ecx+98h] 
        test        edx,edx
        jmp     RETURN_CrashFix_Misc19b_BOTH  // 7F0C20/7F0C60
    }
}


// Handle CPlaceable::RemoveMatrix having wrong data
// hooked at 54F3B0 6 bytes
void _declspec(naked) HOOK_CrashFix_Misc20 ()
{
#if TEST_CRASH_FIXES
    SIMULATE_ERROR_BEGIN( 10 )
        _asm
        {
            mov     ecx, 0
        }
    SIMULATE_ERROR_END
#endif
    _asm
    {
        cmp     ecx, 0
        je      cont        // Skip much code if ecx is zero

        // continue standard path
        sub     esp, 10h 
        mov     eax, [ecx+14h] 
        jmp     RETURN_CrashFix_Misc20      // 54F3B6

    cont:
        CRASH_AVERTED( 20 )
        retn
    }
}


// Handle CTaskSimpleCarFallOut::FinishAnimFallOutCB having wrong data
// hooked at 648EF6 6 bytes
void _declspec(naked) HOOK_CrashFix_Misc21 ()
{
#if TEST_CRASH_FIXES
    SIMULATE_ERROR_BEGIN( 10 )
        _asm
        {
            mov     ecx, 0x10
        }
    SIMULATE_ERROR_END
#endif

    _asm
    {
        cmp     ecx, 0x480
        jb      cont  // Skip much code if ecx is low

        // continue standard path
        mov     edx, [ecx+590h]
        jmp     RETURN_CrashFix_Misc21  // 648EFC

    cont:
        CRASH_AVERTED( 21 )
        retn
    }
}



// Handle CAnimBlendAssociation::Init having wrong data
// hooked at 4CEF08 6 bytes
void _declspec(naked) HOOK_CrashFix_Misc22 ()
{
    _asm
    {
        mov         edx,dword ptr [edi+0Ch] 
    }

#if TEST_CRASH_FIXES
    SIMULATE_ERROR_BEGIN( 10 )
        _asm
        {
            mov     edx, 0x10
        }
    SIMULATE_ERROR_END
#endif

    _asm
    {
        cmp     edx, 0x480
        jb      altcode  // Fill output with zeros if edx is low

        // do standard code
    lp1:
        mov         edx,dword ptr [edi+0Ch] 
        mov         edx,dword ptr [edx+eax*4] 
        mov         ebx,dword ptr [esi+10h] 
        mov         dword ptr [ebx+ecx+10h],edx 
        mov         edx,dword ptr [esi+10h] 
        mov         dword ptr [edx+ecx+14h],esi 
        movsx       edx,word ptr [esi+0Ch] 
        inc         eax  
        add         ecx,18h 
        cmp         eax,edx 
        jl          lp1 
        jmp     RETURN_CrashFix_Misc22  // 4CEF25

        // do alternate code
    altcode:
        CRASH_AVERTED( 22 )
        mov     edx,0
        mov         ebx,dword ptr [esi+10h] 
        mov         dword ptr [ebx+ecx+10h],edx 
        mov         dword ptr [ebx+ecx+14h],edx 
        movsx       edx,word ptr [esi+0Ch] 
        inc         eax  
        add         ecx,18h 
        cmp         eax,edx 
        jl          altcode 
        jmp     RETURN_CrashFix_Misc22  // 4CEF25
    }
}


// Handle CVehicleAnimGroup::ComputeAnimDoorOffsets having wrong door index
// hooked at 6E3D10 7 bytes
void _declspec(naked) HOOK_CrashFix_Misc23 ()
{
#if TEST_CRASH_FIXES
    SIMULATE_ERROR_BEGIN( 10 )
        _asm
        {
            mov     edx,0xffff0000
            mov     [esp+8], edx
        }
    SIMULATE_ERROR_END
#endif

    _asm
    {
        // Ensure door index is reasonable
        mov     edx, [esp+8]
        cmp     edx,16
        jb      ok

        // zero if out of range
        mov     edx,0
        mov     [esp+8], edx
        CRASH_AVERTED( 23 )

    ok:
        // continue standard path
        mov     edx, [esp+8]
        lea     eax, [edx+edx*2]
        jmp     RETURN_CrashFix_Misc23  // 6E3D17
    }
}


// Handle _RwFrameForAllChildren being called with NULL
// hooked at 7F0DC8/7F0E08 6 bytes
void _declspec(naked) HOOK_CrashFix_Misc24 ()
{
    _asm
    {
        cmp     ebp, 0x480
        jb      cont  // Skip code if ebp is low

        // continue standard path
        mov     eax, [ebp+98h]
        jmp     RETURN_CrashFix_Misc24_BOTH  // 7F0DCE/7F0E0E

    cont:
        CRASH_AVERTED( 24 )
        mov     ebp, 0
        mov     eax, 0
        jmp     RETURN_CrashFix_Misc24_BOTH  // 7F0DCE/7F0E0E
    }
}


// If matrix looks bad, fix it
void _cdecl CheckMatrix ( float* pMatrix )
{
    if ( abs ( pMatrix[0] ) < 1.1f )
        return;

    float scale = 0.0f;

    pMatrix[0] = scale;
    pMatrix[1] = 0;
    pMatrix[2] = 0;

    pMatrix[4] = 0;
    pMatrix[5] = scale;
    pMatrix[6] = 0;

    pMatrix[7] = 0;
    pMatrix[8] = 0;
    pMatrix[10] = scale;

    pMatrix[12] = 0;
    pMatrix[13] = 0;
    pMatrix[14] = 1;
}

// hooked at 7C5A5C/7C5A9C 5 bytes
void _declspec(naked) HOOK_CheckAnimMatrix ()
{
    _asm
    {
        // Replaced code
        lea     ecx, [esp+054h]
        pushad

        // Verify matrix
        push ecx
        call CheckMatrix
        add esp, 4

        popad
        // continue standard path
        push    eax
        jmp     RETURN_CheckAnimMatrix_BOTH      // 7C5A61/7C5AA1
    }
}



static SColor vehColors[4];

void _cdecl SaveVehColors ( DWORD dwThis )
{
    CVehicle* pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD *)dwThis );
    if ( pVehicle )
    {
        pVehicle->GetColor ( &vehColors[0], &vehColors[1], &vehColors[2], &vehColors[3], 0 );

        // 0xFF00FF and 0x00FFFF both result in black for some reason
        for ( uint i = 0 ; i < NUMELMS( vehColors ) ; i++ )
        {
            if ( vehColors[i] == 0xFF00FF )
                vehColors[i] = 0xFF01FF;
            if ( vehColors[i] == 0x00FFFF )
                vehColors[i] = 0x01FFFF;
        }
    }
}


void _declspec(naked) HOOK_VehCol ()
{
    _asm
    {
        // Get vehColors for this vehicle
        pushad
        push esi
        call SaveVehColors
        add esp, 4
        popad

        // Hooked from 006D6603  9 bytes
        mov         dl, 3
        mov         al, 2
        mov         cl, 1
        push        edx  
        xor         edx,edx 
        mov         dl,byte ptr [esi+434h] 
        mov         dl, 0

        jmp     RETURN_VehCol  // 006D660C
    }
}


void _declspec(naked) HOOK_VehColCB ()
{
    _asm
    {
        // Hooked from 004C838D  29 bytes

        // Apply vehColors for this vehicle
        mov         cl,byte ptr [esi*4+vehColors.R] 
        mov         byte ptr [eax+4],cl

        mov         cl,byte ptr [esi*4+vehColors.G] 
        mov         byte ptr [eax+5],cl

        mov         cl,byte ptr [esi*4+vehColors.B] 
        mov         byte ptr [eax+6],cl

        jmp     RETURN_VehColCB  // 004C83AA
    }
}

// Check if this vehicle is allowed to process swinging doors.
static DWORD dwSwingingDoorAutomobile;
static const DWORD dwSwingingRet1 = 0x6A9DB6;
static const DWORD dwSwingingRet2 = 0x6AA1DA;
static bool AllowSwingingDoors ()
{
    CVehicle* pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD *)dwSwingingDoorAutomobile );
    if ( pVehicle == 0 || pVehicle->AreSwingingDoorsAllowed() )
        return true;
    else
        return false;
}

void _declspec(naked) HOOK_CAutomobile__ProcessSwingingDoor ()
{
    _asm
    {
        mov     dwSwingingDoorAutomobile, esi
        mov     ecx, [esi+eax*4+0x648]
        pushad
    }

    if ( AllowSwingingDoors() )
    {
        _asm
        {
            popad
            jmp     dwSwingingRet1
        }
    }
    else
    {
        _asm
        {
            popad
            jmp     dwSwingingRet2
        }
    }
}

void* SetModelSuspensionLinesToVehiclePrivate ( CVehicleSAInterface* pVehicleIntf )
{
    // Set the per-model suspension line data of the vehicle's model to the per-vehicle
    // suspension line data so that collision processing will use that instead.
    CVehicle* pVehicle = pVehicleIntf->m_pVehicle;
    CModelInfo* pModelInfo = pGameInterface->GetModelInfo ( pVehicle->GetModelIndex () );
    return pModelInfo->SetVehicleSuspensionData ( pVehicle->GetPrivateSuspensionLines () );
}

void SetModelSuspensionLines ( CVehicleSAInterface* pVehicleIntf, void* pSuspensionLines )
{
    CModelInfo* pModelInfo = pGameInterface->GetModelInfo ( pVehicleIntf->m_pVehicle->GetModelIndex () );
    pModelInfo->SetVehicleSuspensionData ( pSuspensionLines );
}
// Some variables.
DWORD dwSuspensionChangedJump = 0x4185C0;
bool bSuspensionChanged = false;
CVehicleSAInterface* pSuspensionInterface = NULL;
bool CheckHasSuspensionChanged ( void )
{
    // Make sure we have a valid suspension interface
    if ( pSuspensionInterface )
    {
        // Check our suspension interface has a valid vehicle and return the suspension changed marker
        CVehicle* pVehicle = pSuspensionInterface->m_pVehicle;
        CModelInfo* pModelInfo = pGameInterface->GetModelInfo ( pVehicle->GetModelIndex () );
        if ( pVehicle && pModelInfo && ( pModelInfo->IsCar() || pModelInfo->IsMonsterTruck() ) )
            return pVehicle->GetHandlingData()->HasSuspensionChanged ( );
        else
            return false;
    }
    else
        return false;

}
void _declspec(naked) HOOK_ProcessVehicleCollision ()
{
    _asm
    {
        mov     pSuspensionInterface, esi
        pushad
    }

    if ( CheckHasSuspensionChanged ( ) )
    {
        // When the vehicle's collision is about to be processed, set its per-vehicle
        // suspension lines as the per-model suspension lines, and restore the per-model lines
        // afterwards
        _asm
        {
            popad
            push esi
            call SetModelSuspensionLinesToVehiclePrivate
            add esp, 4

            push eax

                push dword ptr [esp+4+0x20]
                push dword ptr [esp+8+0x1C]
                push dword ptr [esp+0xC+0x18]
                push dword ptr [esp+0x10+0x14]
                push dword ptr [esp+0x14+0x10]
                push dword ptr [esp+0x18+0xC]
                push dword ptr [esp+0x1C+8]
                push dword ptr [esp+0x20+4]
                mov eax, 0x4185C0       // CCollision::ProcessColModels
                call eax
                add esp, 0x20

            pop edx

            push eax
            
                push edx
                push esi
                call SetModelSuspensionLines
                add esp, 8

            pop eax
            ret
        }
    }
    else
    {
        // Skip our code in this case because they haven't changed anything so it'l just cause problems.
        _asm
        {
            popad
            jmp dwSuspensionChangedJump
        }
    }    
}


void OnPreHUDRender ( void );

void _declspec(naked) HOOK_PreHUDRender ()
{
    _asm
    {
        pushad
    }
        //call PreHUDRender
    OnPreHUDRender ();

    _asm
    {
        popad

        // Hooked from 0053EAD8  5 bytes
        mov     eax, ds:0B6F0B8h

        jmp     RETURN_PreHUDRender  // 0053EADD
    }
}


void CMultiplayerSA::SetSuspensionEnabled ( bool bEnabled )
{
    //if ( bEnabled )
    {
        // Hook Install
        m_bSuspensionEnabled = true;
        HookInstallCall ( CALL_CAutomobile_ProcessEntityCollision, (DWORD)HOOK_ProcessVehicleCollision );
        //HookInstallCall ( CALL_CBike_ProcessEntityCollision1, (DWORD)HOOK_ProcessVehicleCollision );
        //HookInstallCall ( CALL_CBike_ProcessEntityCollision2, (DWORD)HOOK_ProcessVehicleCollision );
        HookInstallCall ( CALL_CMonsterTruck_ProcessEntityCollision, (DWORD)HOOK_ProcessVehicleCollision );
    }
//     else
//     {
//         // Hook Uninstall
//         m_bSuspensionEnabled = false;
//         HookInstallCall ( CALL_CAutomobile_ProcessEntityCollision, RETURN_ProcessEntityCollision );
//         HookInstallCall ( CALL_CBike_ProcessEntityCollision1, RETURN_ProcessEntityCollision );
//         HookInstallCall ( CALL_CBike_ProcessEntityCollision2, RETURN_ProcessEntityCollision );
//         HookInstallCall ( CALL_CMonsterTruck_ProcessEntityCollision, RETURN_ProcessEntityCollision );
//     }
}


// Enable caching and faster loading of clothes files
void CMultiplayerSA::SetFastClothesLoading ( EFastClothesLoading fastClothesLoading )
{
    if ( m_FastClothesLoading == fastClothesLoading )
        return;

    m_FastClothesLoading = fastClothesLoading;

    // Handle auto setting
    if ( fastClothesLoading == FAST_CLOTHES_AUTO )
    {
        // Disable if less than 512MB installed ram
        long long llSystemRamKB = GetWMITotalPhysicalMemory () / 1024LL;
        if ( llSystemRamKB > 0 && llSystemRamKB < 512 * 1024 )
           fastClothesLoading = FAST_CLOTHES_OFF;
    }

    if ( fastClothesLoading != FAST_CLOTHES_OFF )
    {
        // Load and cache player.img
        SString strGTASAPath = GetCommonRegistryValue ( "", "GTA:SA Path" );
        SString strFilename = PathJoin ( strGTASAPath, "models", "player.img" );
        FileLoad ( strFilename, m_PlayerImgCache );
    }
    else
    {
        // Remove cached data
        m_PlayerImgCache.clear ();
    }

    // Update the cache pointer
    if ( !m_PlayerImgCache.empty () )
        ms_PlayerImgCachePtr = &m_PlayerImgCache[0];
    else
        ms_PlayerImgCachePtr = NULL;
}


//
// Skip loading the directory data from player.img if it has already been loaded.
// Speeds up clothes a bit, but is only part of a solution - The actual files from inside player.img are still loaded each time
//
bool _cdecl IsPlayerImgDirLoaded ( void )
{
    // When player.img dir is loaded, it looks this this:
    // 0x00BC12C0  00bbcdc8 00000226
    DWORD* ptr1 = (DWORD*)0x00BC12C0;
    if ( ptr1[0] == 0x00BBCDC8 && ptr1[1] == 0x0000226 )
    {
        return true;
    }
    return false;
}

void _declspec(naked) HOOK_LoadingPlayerImgDir()
{
// hook from 005A69E3 5 bytes
    _asm
    {
        pushad
        call    IsPlayerImgDirLoaded
        cmp     al, 0
        jnz     skip
        popad

        // Standard code to load img directory
        push    0BBCDC8h 
        jmp     RETURN_LoadingPlayerImgDira     // 5A69E8

        // Skip loading img directory
skip:
        popad
        jmp     RETURN_LoadingPlayerImgDirb     // 5A6A06
    }
}


////////////////////////////////////////////////
//
// Hook CStreaming::RequestFile
//
//
//
////////////////////////////////////////////////
namespace
{
    struct SImgGTAItemInfo
    {
        ushort    usNext;
        ushort    usPrev;

        ushort  uiUnknown1;         // Parent ?
        uchar   uiUnknown2;         // 0x12 when loading, 0x02 when finished loading
        uchar   ucImgId;

        int     iBlockOffset;
        int     iBlockCount;
        uint    uiLoadflag;         // 0-not loaded  2-requested  3-loaded  1-processed
    };

    int     iReturnFileId;
    char*   pReturnBuffer;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
// If request is for a file inside player.img (imgId 5) and uiLoadflag is 0 or 1
// then force use of our cached player.img data
//
bool _cdecl OnCallCStreamingInfoAddToList ( int flags, SImgGTAItemInfo* pImgGTAInfo )
{
    if ( !CMultiplayerSA::ms_PlayerImgCachePtr )
        return false;

    if ( pImgGTAInfo->ucImgId == 5 )
    {
        int iFileId = ((int)pImgGTAInfo - 0x08E4CC0) / 20;

        // Cached enabled setting here as it doesn't usually change
        static bool bLogClothesLoad = GetDebugIdEnabled ( 501 );
        if ( bLogClothesLoad )
        {
            SString strMessage ( "id:%d np:%04x %04x un1:%08x un2:%08x img:%d os:%d #:%d flg:%d"
                                    , iFileId
                                    , pImgGTAInfo->usNext
                                    , pImgGTAInfo->usPrev
                                    , pImgGTAInfo->uiUnknown1
                                    , pImgGTAInfo->uiUnknown2
                                    , pImgGTAInfo->ucImgId
                                    , pImgGTAInfo->iBlockOffset
                                    , pImgGTAInfo->iBlockCount
                                    , pImgGTAInfo->uiLoadflag
                                );

            LogEvent ( 501, "Clothes Load", "", strMessage );
        }

        iReturnFileId = iFileId;
        pReturnBuffer = CMultiplayerSA::ms_PlayerImgCachePtr + pImgGTAInfo->iBlockOffset * 2048;

        // Update flags
        pImgGTAInfo->uiLoadflag = 3;

        // TODO - Check ms_numModelsRequested and ms_numPriorityRequests

        // Remove priorty flag, as not counted in ms_numPriorityRequests
        pImgGTAInfo->uiUnknown2 &= ~ 0x10;

        return true;
    }

    return false;
}


void _declspec(naked) HOOK_CallCStreamingInfoAddToList()
{
// hook from 408962 5 bytes
    _asm
    {
        pushad
        push    ecx
        push    ebx
        call    OnCallCStreamingInfoAddToList
        add     esp, 4*2
        cmp     al, 0
        jnz     skip

        // Continue with standard code
        popad
        call    FUNC_CStreamingInfoAddToList
        jmp     RETURN_CallCStreamingInfoAddToLista     // 408967


        // Handle load here
skip:
        popad
        pushad

        mov     eax, 0
        push    eax
        mov     eax, iReturnFileId
        push    eax
        mov     eax, pReturnBuffer
        push    eax
        call    CStreaming__ConvertBufferToObject      // 40C6B0
        add     esp, 4*3

        popad
        add     esp, 4*1
        jmp     RETURN_CallCStreamingInfoAddToListb     // 408990
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Return true to skip
bool _cdecl ShouldSkipLoadRequestedModels ( DWORD calledFrom )
{
    if ( !CMultiplayerSA::ms_PlayerImgCachePtr )
        return false;

    // Skip LoadRequestedModels if called from:
    //      CClothesBuilder::ConstructGeometryArray      5A55A0 - 5A56B6
    //      CClothesBuilder::LoadAndPutOnClothes         5A5F70 - 5A6039
    //      CClothesBuilder::ConstructTextures           5A6040 - 5A6520
    if ( calledFrom > 0x5A55A0 && calledFrom < 0x5A6520 )
        return true;

    return false;
}


void _declspec(naked) HOOK_CStreamingLoadRequestedModels()
{
// hook from 015670A0/01567090 5 bytes
    _asm
    {
        pushad
        push    [esp+32+4*0]
        call    ShouldSkipLoadRequestedModels
        add     esp, 4*1
        cmp     al, 0
        jnz     skip

        // Continue with standard code
        popad
        mov     al,byte ptr ds:[008E4A58h]
        jmp     RETURN_CStreamingLoadRequestedModelsa_BOTH     // 015663B5/015663A5

        // Skip LoadRequestedModels
skip:
        popad
        jmp     RETURN_CStreamingLoadRequestedModelsb_BOTH     // 0156711B/0156710B
    }
}


////////////////////////////////////////////////
//
// CMultiplayerSA::EnableLODSystem
//
//
////////////////////////////////////////////////
void CMultiplayerSA::SetLODSystemEnabled ( bool bEnable )
{
    // Switch
    if ( m_bEnabledLODSystem == bEnable )
        return;
    m_bEnabledLODSystem = bEnable;

    // Memory saved here
    static CBuffer savedMem;
    struct {
        DWORD dwAddress;
        uint uiSize;
    } memoryList[] = {  { HOOKPOS_CRenderer_SetupEntityVisibility, 8 },
                        { HOOKPOS_CWorldScan_ScanWorld, 6 },
                        { HOOKPOS_CVisibilityPlugins_CalculateFadingAtomicAlpha, 5 } };

    // Enable or not?
    if ( bEnable )
    {
        // Save memory before we blat it
        CBufferWriteStream stream ( savedMem );
        for ( uint i = 0 ; i < NUMELMS( memoryList ) ; i++ )
            stream.WriteBytes ( (void*)memoryList[i].dwAddress, memoryList[i].uiSize );

        // Add hooks and things
        HookInstall(HOOKPOS_CRenderer_SetupEntityVisibility, (DWORD)HOOK_CRenderer_SetupEntityVisibility, 8 );
        HookInstall(HOOKPOS_CWorldScan_ScanWorld, (DWORD)HOOK_CWorldScan_ScanWorld, 5 );
        HookInstall(HOOKPOS_CVisibilityPlugins_CalculateFadingAtomicAlpha, (DWORD)HOOK_CVisibilityPlugins_CalculateFadingAtomicAlpha, 5 );
    }
    else
    {
        // Restore memory
        CBufferReadStream stream ( savedMem );
        for ( uint i = 0 ; i < NUMELMS( memoryList ) ; i++ )
        {
            BYTE temp[10];
            assert ( sizeof(temp) >= memoryList[i].uiSize );
            stream.ReadBytes ( temp, memoryList[i].uiSize );
            MemCpy ( (void*)memoryList[i].dwAddress, temp, memoryList[i].uiSize );
        }
    }
}


////////////////////////////////////////////////
//
// CRenderer_SetupEntityVisibility helpers
//
////////////////////////////////////////////////
namespace
{
    #define     ARRAY_ModelInfo                 0xA9B0C8
    #define     LOW_LOD_DRAW_DISTANCE_SCALE     5


    void SetGlobalDrawDistanceScale ( float fValue )
    {
        *(float*)0x858FD8 = 300.f * fValue;
    }

    float GetDrawDistanceSetting ( void )
    {
        return *(float*)0xB6F118;
    }

    float CalculateLowLodFadeAlpha ( CBaseModelInfoSAInterface* pModelInfo, float fDist )
    {
        float fModelDrawDistance = pModelInfo->fLodDistanceUnscaled * LOW_LOD_DRAW_DISTANCE_SCALE + 20.f;
        float fFadeWidth = 20.f + fModelDrawDistance / 50.f;
        float fFadeFar = fModelDrawDistance;
        float fFadeNear = fFadeFar - fFadeWidth;
        return UnlerpClamped ( fFadeFar, fDist, fFadeNear );
    }

    struct
    {
        bool bValid;
        float fLodDistanceUnscaled;
        CBaseModelInfoSAInterface* pModelInfo;
    } saved = { false, 0.f, NULL };
}


////////////////////////////////////////////////
//
// CRenderer_SetupEntityVisibility
//
////////////////////////////////////////////////
void OnMY_CRenderer_SetupEntityVisibility_Pre( CEntitySAInterface* pEntity, float& fValue )
{
    if ( pEntity->IsLowLodEntity () )
    {
        SetGlobalDrawDistanceScale ( LOW_LOD_DRAW_DISTANCE_SCALE * 2 );
        saved.pModelInfo = ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[pEntity->m_nModelIndex];
        saved.fLodDistanceUnscaled = saved.pModelInfo->fLodDistanceUnscaled;
        saved.pModelInfo->fLodDistanceUnscaled *= LOW_LOD_DRAW_DISTANCE_SCALE / GetDrawDistanceSetting ();
        saved.bValid = true;
    }
    else
        saved.bValid = false;
}

void OnMY_CRenderer_SetupEntityVisibility_Post( int result, CEntitySAInterface* pEntity, float& fDist )
{
    if ( saved.bValid )
    {
        SetGlobalDrawDistanceScale ( 1 );
        saved.pModelInfo->fLodDistanceUnscaled = saved.fLodDistanceUnscaled;
        saved.bValid = false;

        // Doing any distance fading?
        if ( result == 0 && CalculateLowLodFadeAlpha ( saved.pModelInfo, fDist ) < 1.0f )
            pEntity->bDistanceFade = 1;
        else
            pEntity->bDistanceFade = 0;

        pEntity->SetEntityVisibilityResult ( result );
    }
    else
    if ( pEntity->IsHighLodEntity () )
    {
        pEntity->SetEntityVisibilityResult ( result );
    }
}

void _declspec(naked) HOOK_CRenderer_SetupEntityVisibility()
{
// hook from 554230 8 bytes
    _asm
    {
////////////////////
        pushad
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        call    OnMY_CRenderer_SetupEntityVisibility_Pre
        add     esp, 4*2
        popad

////////////////////
        push    [esp+4*2]
        push    [esp+4*2]
        call    second
        add     esp, 4*2

////////////////////
        pushad
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        push    eax
        call    OnMY_CRenderer_SetupEntityVisibility_Post
        add     esp, 4*2+4
        popad

        retn

second:
        sub     esp,14h 
        push    esi  
        mov     esi,dword ptr [esp+1Ch] 
        jmp     RETURN_CRenderer_SetupEntityVisibility   // 0x554238
    }
}


////////////////////////////////////////////////
//
// CWorldScan_ScanWorld
//
////////////////////////////////////////////////
void OnMY_CWorldScan_ScanWorld_Pre( CVector2D* pVec, int iValue, void (__cdecl *func)(int, int) )
{
    // Scale scan area
    const CVector2D& vecOrigin = pVec[0];

    for ( uint i = 1 ; i < 5 ; i++ )
    {
        CVector2D vecDiff = pVec[i] - vecOrigin;
        vecDiff *= LOW_LOD_DRAW_DISTANCE_SCALE * 2;
        pVec[i] = vecDiff + vecOrigin;
    }
}

void _declspec(naked) HOOK_CWorldScan_ScanWorld()
{
// hook from 55555E 5 bytes
    _asm
    {
        call    second
        jmp     RETURN_CWorldScan_ScanWorlda   // 555563

second:
        pushad
        push    [esp+32+4*3]
        push    [esp+32+4*3]
        push    [esp+32+4*3]
        call    OnMY_CWorldScan_ScanWorld_Pre
        add     esp, 4*3
        popad

        jmp     RETURN_CWorldScan_ScanWorldb   // 72CAE0
    }
}


////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////
int OnMY_CVisibilityPlugins_CalculateFadingAtomicAlpha_Pre( CBaseModelInfoSAInterface* pModelInfo, CEntitySAInterface* pEntity, float fDist )
{
    if ( pEntity->IsLowLodEntity () )
    {
        // Apply custom distance fade
        float fAlpha = CalculateLowLodFadeAlpha ( pModelInfo, fDist );
        fAlpha *= pModelInfo->ucAlpha;
        return Clamp < int > ( 0, (int)fAlpha, 255 );
    }
    return -1;
}


void _declspec(naked) HOOK_CVisibilityPlugins_CalculateFadingAtomicAlpha()
{
// hook from 732500 5 bytes
    _asm
    {
        pushad
        push    [esp+32+4*3]
        push    [esp+32+4*3]
        push    [esp+32+4*3]
        call    OnMY_CVisibilityPlugins_CalculateFadingAtomicAlpha_Pre
        mov     [esp+0],eax
        add     esp, 4*3
        popad
        mov     eax,[esp-32-4*3]

        cmp     eax, 0xffffffff
        jne     usecustom

        // Use standard code
        push    [esp+4*3]
        push    [esp+4*3]
        push    [esp+4*3]
        call    second
        add     esp, 4*3

usecustom:
        retn

second:
        push    ecx
        mov     ecx, [esp+8]
        jmp     RETURN_CVisibilityPlugins_CalculateFadingAtomicAlpha                // 732505

    }
}

// Variables
SIPLInst* pEntityWorldAdd = NULL;
CEntitySAInterface * pLODInterface = NULL; 
bool bNextHookSetModel = false;

bool CheckRemovedModelNoSet ( )
{
    // Init our variables
    bNextHookSetModel = false;
    pLODInterface = NULL;
    CWorld* pWorld = pGameInterface->GetWorld();
    // You never know.
    if ( pWorld )
    {
        // Is the model in question even removed?
        if ( pWorld->IsModelRemoved ( pEntityWorldAdd->m_nModelIndex ) )
        {
            // is the replaced model in the spherical radius of any building removal
            if ( pGameInterface->GetWorld ( )->IsRemovedModelInRadius ( pEntityWorldAdd ) )
            {
                // if it is next hook remove it from the world
                return true;
            }
        }
        return false;
    }
    return false;
}
bool CheckRemovedModel ( )
{
    bNextHookSetModel = CheckRemovedModelNoSet ( );
    return bNextHookSetModel;
}
// Hook 1
void _declspec(naked) HOOK_LoadIPLInstance ()
{
    _asm
    {
        pushad
        mov pEntityWorldAdd, ecx
    }
    if ( pEntityWorldAdd )
    {
        CheckRemovedModel ( );
    }
    _asm
    {
        popad
        jmp CALL_LoadIPLInstance
        jmp RETURN_LoadIPLInstance
    }
}

void HideEntitySomehow ( )
{
    // Did we get instructed to set the model
    if ( bNextHookSetModel && pLODInterface )
    {
        // Init pInterface with the Initial model
        CEntitySAInterface * pInterface = pLODInterface;
        // Grab the removal for the interface
        SBuildingRemoval* pBuildingRemoval = pGameInterface->GetWorld ( )->GetBuildingRemoval ( pInterface );
        // Remove down the LOD tree
        if ( pBuildingRemoval && pInterface && pInterface != NULL && pInterface->bIsProcObject == 0 && ( pInterface->nType == ENTITY_TYPE_BUILDING || pInterface->nType == ENTITY_TYPE_DUMMY ) )
        {
            // Add the LOD to the list
            pBuildingRemoval->AddBinaryBuilding ( pInterface );
            // Remove the model from the world
            pGameInterface->GetWorld ( )->Remove ( pInterface, BuildingRemoval );
            // Get next LOD ( LOD's can have LOD's so we keep checking pInterface )
            //pInterface = pInterface->m_pLod;
        }
    }
    // Reset our next hook variable
    bNextHookSetModel = false;
}
// Hook 2
void _declspec(naked) HOOK_CWorld_LOD_SETUP ()
{
    _asm
    {
        pushad
        mov pLODInterface, esi
    }
    HideEntitySomehow ( );
    _asm
    {
        popad
        jmp CALL_CWorld_LODSETUP
    }
}

CEntitySAInterface * pBuildingAdd = NULL;
void StorePointerToBuilding ( )
{
    if ( pBuildingAdd != NULL )
    {
        pGameInterface->GetWorld ( )->AddDataBuilding ( pBuildingAdd );
    }
}

// Called when a data entity is added to the world (this happens once when the game loads so we just dump those in a list and we can sift through when someone tries to remove.)
void _declspec(naked) Hook_AddBuildingInstancesToWorld ( )
{
    _asm
    {
        pushad
        mov pBuildingAdd, edx
    }
    StorePointerToBuilding ( );
    _asm
    {
        popad
        jmp JMP_CWorld_Add_AddBuildingInstancesToWorld_CALL_CWorldAdd
    }
}

bool CheckForRemoval ( )
{
    // Did we get instructed to set the model
    if ( pLODInterface )
    {
        // Init pInterface with the Initial model
        CEntitySAInterface * pInterface = pLODInterface;
        // Remove down the LOD tree
        if ( pGameInterface->GetWorld ( )->IsObjectRemoved ( pInterface ) )
        {
            return true;
        }
    }
    return false;
}

// Call to CWorld::Add in CPopulation::ConvertToRealObject we just use this to get a list of pointers to valid objects for instant removal
void _declspec(naked) Hook_CWorld_ADD_CPopulation_ConvertToRealObject ( )
{
    _asm
    {
        pushad
        mov pBuildingAdd, esi
        mov pLODInterface, esi
    }
    StorePointerToBuilding ( );
    _asm
    {
        popad
        jmp JMP_CWorld_Add_CPopulation_ConvertToRealObject_CallCWorldAdd
        jmp JMP_CWorld_Add_CPopulation_ConvertToRealObject_Retn
    }
}

void RemoveObjectIfNeeded ( )
{
    SBuildingRemoval* pBuildingRemoval = pGameInterface->GetWorld ( )->GetBuildingRemoval ( pLODInterface );
    if ( pBuildingRemoval != NULL )
    {
        if ( (DWORD)(pBuildingAdd->vtbl) != VTBL_CPlaceable )
        {
            pBuildingRemoval->AddDataBuilding ( pBuildingAdd );
            pGameInterface->GetWorld ( )->Remove( pBuildingAdd, BuildingRemoval3 );
        }

        if ( (DWORD)(pLODInterface->vtbl) != VTBL_CPlaceable )
        {
            pBuildingRemoval->AddDataBuilding ( pLODInterface );
            pGameInterface->GetWorld ( )->Remove( pLODInterface, BuildingRemoval4 );
        }
    }
}

// on stream in -> create and remove it from the world just after so we can restore easily
void _declspec (naked) HOOK_ConvertToObject_CPopulationManageDummy ( )
{
    _asm
    {
        pushad
        mov pBuildingAdd, edx
        mov pLODInterface, edx
    }
    _asm
    {
        popad
        push edx
        call CALL_Convert_To_Real_Object_CPopulation_ManageDummy
        pop ecx
        mov pLODInterface, ecx
        pushad
    }
    RemoveObjectIfNeeded ( );
    _asm
    {
        popad
        jmp JMP_RETN_Cancel_CPopulation_ManageDummy
    }
}

CEntitySAInterface * pBuildingRemove = NULL;
void RemovePointerToBuilding ( )
{
    if ( pBuildingRemove->nType == ENTITY_TYPE_BUILDING || pBuildingRemove->nType == ENTITY_TYPE_DUMMY || pBuildingRemove->nType == ENTITY_TYPE_OBJECT )
    {
        pGameInterface->GetWorld ( )->RemoveWorldBuildingFromLists ( pBuildingRemove );
    }
}

DWORD dwCWorldRemove = 0x563280;
// Call to CWorld::Remove in CPopulation::ConvertToDummyObject this is called just before deleting a CObject so we remove the CObject while we are there and remove the new dummy if we need to do so before returning
void _declspec(naked) HOOK_CWorld_Remove_CPopulation_ConvertToDummyObject ( )
{
    _asm
    {
        pushad
        mov pBuildingRemove, esi
        mov pBuildingAdd, edi
        mov pLODInterface, edi
    }
    RemovePointerToBuilding ( );
    StorePointerToBuilding ( );
    RemoveObjectIfNeeded ( );
    _asm
    {
        popad
        jmp dwCWorldRemove
    }
}
// if it's replaced get rid of it
void RemoveDummyIfReplaced ( )
{
    SBuildingRemoval* pBuildingRemoval = pGameInterface->GetWorld ( )->GetBuildingRemoval ( pLODInterface );
    if ( pBuildingRemoval != NULL )
    {
        if ( (DWORD)(pBuildingAdd->vtbl) != VTBL_CPlaceable )
        {
            pBuildingRemoval->AddDataBuilding ( pBuildingAdd );
            pGameInterface->GetWorld ( )->Remove( pBuildingAdd, BuildingRemoval5 );
        }
    }
}

// Function that handles dummy -> object so we can cancel this process if need be
void _declspec(naked) HOOK_CWorld_Add_CPopulation_ConvertToDummyObject ( )
{
    _asm
    {
        pushad
        mov pLODInterface, edi
        mov pBuildingAdd, edi
    }
    StorePointerToBuilding ( );
    if ( CheckForRemoval ( ) )
    {
        _asm
        {
            popad
            jmp JMP_RETN_Cancelled_CPopulation_ConvertToDummyObject
        }
    }
    else
    {
        _asm
        {
            popad
            push edi
            call CALL_CWorld_Add_CPopulation_ConvertToDummyObject
            jmp JMP_RETN_Called_CPopulation_ConvertToDummyObject
        }
    }
}

// Destructors to catch element deletion so we can delete their entries
void _declspec(naked) Hook_CBuilding_DTR ( )
{
    _asm
    {
        pushad
        mov pBuildingRemove, ecx
    }
    RemovePointerToBuilding ( );
    _asm
    {
        popad
        jmp JMP_CBuilding_DTR
    }
}

void _declspec(naked) Hook_CDummy_DTR ( )
{
    _asm
    {
        pushad
        mov pBuildingRemove, ecx
    }
    RemovePointerToBuilding ( );
    _asm
    {
        popad
        jmp JMP_CDummy_DTR
    }
}

DWORD dwObjectVtbl = 0x866F60;
void _declspec(naked) Hook_CObject_DTR ( )
{
    _asm
    {
        pushad
        mov pBuildingRemove, esi
    }
    RemovePointerToBuilding ( );
    _asm
    {
        popad
        mov dword ptr [esi], offset dwObjectVtbl
        jmp JMP_CObject_DTR
    }
}

static DWORD dwEntityVtbl;
static DWORD dwMultResult;
void _declspec(naked) HOOK_CEntity_IsOnScreen_FixObjectScale ()
{
    _asm
    {
        push    0xB6FA74

        pushad
        mov     eax, [esi]
        mov     dwEntityVtbl, eax
    }

    if ( dwEntityVtbl == 0x866F60 )
        goto IsOnScreen_IsObject;

    _asm
    {
        popad
        mov     esi, ecx
        jmp     JMP_CEntity_IsOnScreen_FixObjectsScale

IsOnScreen_IsObject:
        popad
        fld     [eax+0x24]
        fld     [esi+0x15C]
        fmulp   st(1), st(0)
        fstp    dwMultResult
        mov     esi, dwMultResult
        jmp     JMP_CEntity_IsOnScreen_FixObjectsScale
    }
}
CVehicleSAInterface * pCollisionVehicle = NULL;
void TriggerVehicleDamageEvent ( )
{
    if ( pCollisionVehicle )
    {
        CEntitySAInterface * pEntity = pCollisionVehicle->damageEntity;
        if ( pEntity )
        {
            // Not handled because it triggers too much
            //if ( pEntity->nType != ENTITY_TYPE_BUILDING )
            {
                if ( m_pVehicleCollisionHandler )
                {
                    if ( pEntity->nType == ENTITY_TYPE_VEHICLE )
                    {
                        CVehicleSAInterface * pInterface = static_cast < CVehicleSAInterface* > ( pEntity );
                        m_pVehicleCollisionHandler ( pCollisionVehicle, pEntity, pEntity->m_nModelIndex, pCollisionVehicle->fDamageImpulseMagnitude, pInterface->fDamageImpulseMagnitude, pCollisionVehicle->byBodyPartHit, pCollisionVehicle->vecCollisionPosition, pCollisionVehicle->vecCollisionImpactVelocity );
                    }
                    else
                    {
                        m_pVehicleCollisionHandler ( pCollisionVehicle, pEntity, pEntity->m_nModelIndex, pCollisionVehicle->fDamageImpulseMagnitude, 0.0f, pCollisionVehicle->byBodyPartHit, pCollisionVehicle->vecCollisionPosition, pCollisionVehicle->vecCollisionImpactVelocity );
                    }
                }
            }
        }
    }
}

void _declspec(naked) HOOK_CEventVehicleDamageCollision ( )
{
    // .006A7657 64 A1 00 00 00 00                       mov     eax, large fs:0 < Hook >
    // .006A765D 50                                      push    eax < Jmp Back >

    // ecx = this ptr
    // pVehicle->damageEntity = damage entity
    _asm
    {
        pushad
        mov pCollisionVehicle, ecx
    }
    TriggerVehicleDamageEvent ( );
    
    // do the replaced code and return back as if nothing happened.
    _asm
    {
        popad
        mov eax, fs:0
        jmp JMP_CEventVehicleDamageCollision_RETN
    }
}

DWORD dwPlaneDamageThreadshold = 0x8D33E4;
void _declspec(naked) HOOK_CEventVehicleDamageCollision_Plane ( )
{
    // .006CC4B3 A1 E4 33 8D 00                            mov     eax, ds:?PLANE_DAMAGE_THRESHHOLD@@3MA
    //  006CC4B8 53                                        push    ebx < Jmp Back >

    // ecx = this ptr
    // pVehicle->damageEntity = damage entity
    _asm
    {
        pushad
        mov pCollisionVehicle, ecx
    }
    TriggerVehicleDamageEvent ( );

    // do the replaced code and return back as if nothing happened.
    _asm
    {
        popad
        mov eax, DWORD PTR DS:[8D33E4h]
        jmp JMP_CEventVehicleDamageCollision_Plane_RETN
    }
}

void _declspec(naked) HOOK_CEventVehicleDamageCollision_Bike ( )
{
    // .006B8EC6 DC 1D F8 9E 85 00                       fcomp   ds:__real@0000000000000000 < Hook >
    //  006B8ECC 8B F1                                   mov     esi, ecx < Jmp Back >

    // ecx = this ptr
    // pVehicle->damageEntity = damage entity
    _asm
    {
        pushad
        mov pCollisionVehicle, ecx
    }
    TriggerVehicleDamageEvent ( );

    // do the replaced code and return back as if nothing happened.
    _asm
    {
        popad
        fcomp DWORD PTR DS:[859EF8h]
        jmp JMP_CEventVehicleDamageCollision_Bike_RETN
    }
}
//////////////////////////////////////////////////////////////////////////////////////////
// Only allow rebuild player on CJ - Stops other models getting corrupted (spider CJ)
// hooked at 5A82C0 8 bytes
void _declspec(naked) HOOK_CClothes_RebuildPlayer ()
{
    _asm
    {
        push    esi
        mov     esi, [esp+8]
        movsx   eax, word ptr [esi+34]
        cmp     eax, 0
        jne     cont        // Not CJ, so skip

        // continue standard path
        mov     eax, [esi+18h]
        jmp     RETURN_CClothes_RebuildPlayera  // 005A82C8

    cont:
        jmp     RETURN_CClothes_RebuildPlayerb  // 005A837F
    }
}


void _declspec(naked) HOOK_CProjectileInfo_Update_FindLocalPlayer_FindLocalPlayerVehicle ()
{
    // 00739559 E8 B2 4C E3 FF                          call    FindPlayerPed < HOOK >
    // 00739570 E8 5B 4B E3 FF                          call    FindPlayerVehicle < HOOK >
    // Checks if the creator is the local player ped or the creator is the local player peds vehicle else decreases the velocity substantially.
    // We are forcing it to think the creator is not the local player ped or his vehicle for this specific check
    _asm
    {
        xor eax, eax
        retn
    }
}


void CMultiplayerSA::SetAutomaticVehicleStartupOnPedEnter ( bool bSet )
{
    static BYTE originalCode [ 6 ] = { 0 };
    if ( originalCode[0] == '\0' )
        MemCpyFast ( &originalCode[0], (const void *)0x64BC0D, 6 );

    if ( bSet )
        MemCpyFast ( (char *)0x64BC0D, originalCode, 6 );
    else
        MemSetFast ( (char *)0x64BC0D, 0x90, 6 );
}

// Storage
CVehicleSAInterface * pHeliKiller = NULL;
CPedSAInterface * pPedKilledByHeli = NULL;
bool CallHeliKillEvent ( )
{
    // Is our handler alive
    if ( m_pHeliKillHandler )
    {
        // Return our handlers return
        return m_pHeliKillHandler ( pHeliKiller, pPedKilledByHeli );
    }
    // Return true else
    return true;
}

void _declspec(naked) HOOK_CHeli_ProcessHeliKill ( )
{
    // 006DB201 0F 85 30 02 00 00                         jnz     loc_6DB437 < HOOK >
    // 006DB207 8B 47 14                                  mov     eax, [edi+14h] < RETURN CONTINUE >
    // 006DB9E0 8B 44 24 6C                               mov     eax, [esp+1C8h+var_15C] < RETURN CANCEL >
    // Hook is in Process Heli blades I think it's processing a Heli's blades against peds inside a collision shape.
    // We hook just after the check if he's touched the blade as before that it's just got the results of if he's near enough the heli to hit the blades
    // esi = Heli
    // edi = ped
    _asm
    {
        pushad
        mov pHeliKiller, esi
        mov pPedKilledByHeli, edi
    }
    // Call our event
    if ( CallHeliKillEvent ( ) == false )
    {
        _asm
        {
            popad
            // Go to the end of the while loop and let it start again
            jmp RETURN_CHeli_ProcessHeliKill_RETN_Cancel
        }
    }
    else
    {
        _asm
        {
            popad
            // do our JNZ
            jnz 6DB437h
            // if it failed do our continue
            jmp RETURN_CHeli_ProcessHeliKill_RETN_Cont_Zero
        }
    }
}