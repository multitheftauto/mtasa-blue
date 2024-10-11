/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/multiplayer_sa/CMultiplayerSA.cpp
 *  PURPOSE:     Multiplayer module class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/CWorld.h>
#include <game/CBuildingRemoval.h>
#include <game/CAnimBlendAssocGroup.h>
#include <game/CPedDamageResponse.h>
#include <game/CEventList.h>
#include <game/CEventDamage.h>

class CEventDamageSAInterface;

extern CCoreInterface* g_pCore;
extern CMultiplayerSA* pMultiplayer;

using namespace std;

char* CMultiplayerSA::ms_PlayerImgCachePtr = NULL;

extern CGame* pGameInterface;

#define HOOKPOS_FindPlayerCoors                          0x56E010
#define HOOKPOS_FindPlayerCentreOfWorld                  0x56E250
#define HOOKPOS_FindPlayerHeading                        0x56E450
#define HOOKPOS_CStreaming_Update_Caller                 0x53BF09
#define HOOKPOS_CHud_Draw_Caller                         0x53E4FA
#define HOOKPOS_CRunningScript_Process                   0x469F00
#define HOOKPOS_CExplosion_AddExplosion                  0x736A50
#define HOOKPOS_CCustomRoadsignMgr__RenderRoadsignAtomic 0x6FF35B
#define HOOKPOS_Trailer_BreakTowLink                     0x6E0027
#define HOOKPOS_CRadar__DrawRadarGangOverlay             0x586650
#define HOOKPOS_CTaskComplexJump__CreateSubTask          0x67DABE
#define HOOKPOS_CTrain_ProcessControl_Derail             0x6F8DBA
#define HOOKPOS_CVehicle_SetupRender                     0x6D6512
#define HOOKPOS_CVehicle_ResetAfterRender                0x6D0E3E
#define HOOKPOS_CObject_Render                           0x59F1ED
#define HOOKPOS_EndWorldColors                           0x561795
#define HOOKPOS_CWorld_ProcessVerticalLineSectorList     0x563357
#define HOOKPOS_ComputeDamageResponse_StartChoking       0x4C05B9
#define HOOKPOS_CAutomobile__ProcessSwingingDoor         0x6A9DAF

#define FUNC_CStreaming_Update                           0x40E670
#define FUNC_CAudioEngine__DisplayRadioStationName       0x507030
#define FUNC_CHud_Draw                                   0x58FAE0
#define FUNC_CPlayerInfoBase                             0xB7CD98

#define ADDR_CursorHiding                                0x7481CD
#define ADDR_GotFocus                                    0x748054

#define HOOKPOS_FxManager_CreateFxSystem                    0x4A9BE0
#define HOOKPOS_FxManager_DestroyFxSystem                   0x4A9810

DWORD RETURN_FxManager_CreateFxSystem = 0x4A9BE8;
DWORD RETURN_FxManager_DestroyFxSystem = 0x4A9817;

#define HOOKPOS_CCam_ProcessFixed                           0x51D470
#define HOOKPOS_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon 0x6859a0
#define HOOKPOS_CPed_IsPlayer                               0x5DF8F0

DWORD RETURN_CCam_ProcessFixed = 0x51D475;
DWORD RETURN_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon = 0x6859A7;
DWORD RETURN_CPed_IsPlayer = 0x5DF8F6;

#define VAR_CollisionStreamRead_ModelInfo                   0x9689E0
#define HOOKPOS_CollisionStreamRead                         0x41B1D0
DWORD RETURN_CollisionStreamRead = 0x41B1D6;

#define CALL_Render3DStuff                                  0x53EABF
#define FUNC_Render3DStuff                                  0x53DF40

#define CALL_CRenderer_Render                               0x53EA12
#define FUNC_CRenderer_Render                               0x727140

#define CALL_CTrafficLights_GetPrimaryLightState             0x49DB5F
#define CALL_CTrafficLights_GetSecondaryLightState             0x49DB6D
#define HOOKPOS_CTrafficLights_DisplayActualLight           0x49E1D9
DWORD RETURN_CTrafficLights_DisplayActualLight = 0x49E1FF;

#define HOOKPOS_CGame_Process                               0x53C095
DWORD RETURN_CGame_Process = 0x53C09F;

#define HOOKPOS_Idle                                        0x53E981
DWORD RETURN_Idle = 0x53E98B;

DWORD FUNC_CEntity_Render = 0x534310;

#define HOOKPOS_VehicleCamStart                             0x5245ED
DWORD RETURN_VehicleCamStart_success = 0x5245F3;
DWORD RETURN_VehicleCamStart_failure = 0x525E43;
#define HOOKPOS_VehicleCamTargetZTweak                      0x524A68
DWORD RETURN_VehicleCamTargetZTweak = 0x524AA4;
#define HOOKPOS_VehicleCamLookDir1                          0x524DF1
DWORD RETURN_VehicleCamLookDir1 = 0x524DF6;
#define HOOKPOS_VehicleCamLookDir2                          0x525B0E
DWORD RETURN_VehicleCamLookDir2 = 0x525B73;
#define HOOKPOS_VehicleCamHistory                           0x525C56
DWORD RETURN_VehicleCamHistory = 0x525D4A;
#define HOOKPOS_VehicleCamColDetect                         0x525D8D
DWORD RETURN_VehicleCamColDetect = 0x525D92;
#define CALL_VehicleCamUp                                   0x525E1F
#define HOOKPOS_VehicleCamEnd                               0x525E3C
DWORD RETURN_VehicleCamEnd = 0x525E42;

#define HOOKPOS_VehicleLookBehind                           0x5207E3
DWORD RETURN_VehicleLookBehind = 0x520891;
#define CALL_VehicleLookBehindUp                            0x520E2A
#define HOOKPOS_VehicleLookAside                            0x520F70
DWORD RETURN_VehicleLookAside = 0x520FDC;
#define CALL_VehicleLookAsideUp                             0x5211E0

#define HOOKPOS_OccupiedVehicleBurnCheck                    0x570C84
DWORD RETURN_OccupiedVehicleBurnCheck = 0x570C8A;
#define HOOKPOS_UnoccupiedVehicleBurnCheck                  0x6A76DC
DWORD RETURN_UnoccupiedVehicleBurnCheck = 0x6A76E4;
#define HOOKPOS_ApplyCarBlowHop                             0x6B3816
DWORD RETURN_ApplyCarBlowHop = 0x6B3831;

#define HOOKPOS_CVehicle_ApplyBoatWaterResistance           0x6D2771
DWORD RETURN_CVehicle_ApplyBoatWaterResistance = 0x6D2777;

#define HOOKPOS_CPhysical_ApplyGravity                      0x543081
DWORD RETURN_CPhysical_ApplyGravity = 0x543093;

#define HOOKPOS_CWorld_SetWorldOnFire                       0x56B983
DWORD RETURN_CWorld_SetWorldOnFire = 0x56B989;
#define HOOKPOS_CTaskSimplePlayerOnFire_ProcessPed          0x6336DA
DWORD RETURN_CTaskSimplePlayerOnFire_ProcessPed = 0x6336E0;
#define HOOKPOS_CFire_ProcessFire                           0x53AC1A
DWORD RETURN_CFire_ProcessFire = 0x53AC1F;
#define HOOKPOS_CExplosion_Update                           0x7377D3
DWORD RETURN_CExplosion_Update = 0x7377D8;
#define HOOKPOS_CWeapon_FireAreaEffect                      0x73EBFE
DWORD RETURN_CWeapon_FireAreaEffect = 0x73EC03;

#define CALL_RenderScene_Plants                             0x53E103
#define HOOKPOS_RenderScene_end                             0x53E159
#define HOOKPOS_CPlantMgr_Render                            0x5DBC4C
DWORD RETURN_CPlantMgr_Render_success = 0x5DBC52;
DWORD RETURN_CPlantMgr_Render_fail = 0x5DBDAA;

#define HOOKPOS_CEventHandler_ComputeKnockOffBikeResponse   0x4BA06F
DWORD RETURN_CEventHandler_ComputeKnockOffBikeResponse = 0x4BA076;

#define HOOKPOS_CAnimBlendAssociation_SetCurrentTime        0x4CEA80
#define HOOKPOS_RpAnimBlendClumpUpdateAnimations            0x4D34F0
#define HOOKPOS_CAnimBlendAssoc_destructor                  0x4CECF0
#define HOOKPOS_CAnimBlendAssocGroupCopyAnimation           0x4CE130
#define HOOKPOS_CAnimManager_AddAnimation                   0x4d3aa0
#define HOOKPOS_CAnimManager_AddAnimationAndSync            0x4D3B30
#define HOOKPOS_CAnimManager_BlendAnimation_Hierarchy       0x4D453E

#define HOOKPOS_CPed_GetWeaponSkill                         0x5e3b60
DWORD RETURN_CPed_GetWeaponSkill = 0x5E3B68;

#define HOOKPOS_CPed_AddGogglesModel                        0x5E3ACB
DWORD RETURN_CPed_AddGogglesModel = 0x5E3AD4;

#define FUNC_CWorld_Remove                                  0x563280
#define FUNC_CTagManager_ShutdownForRestart                 0x49CC60
unsigned int* VAR_NumTags = (unsigned int*)0xA9AD70;
DWORD**       VAR_TagInfoArray = (DWORD**)0xA9A8C0;

#define HOOKPOS_CPhysical_ProcessCollisionSectorList        0x54BB93
DWORD RETURN_CPhysical_ProcessCollisionSectorList = 0x54BB9A;

#define HOOKPOS_CheckAnimMatrix                          	0x7C5A5C
DWORD RETURN_CheckAnimMatrix = 0x7C5A61;

#define HOOKPOS_VehColCB                                    0x04C838D
DWORD RETURN_VehColCB = 0x04C83AA;

#define HOOKPOS_VehCol                                      0x06D6603
DWORD RETURN_VehCol = 0x06D660C;

#define HOOKPOS_Transmission_CalculateDriveAcceleration 0x6D05E0
DWORD RETURN_Transmission_CalculateDriveAcceleration = 0x6D05E6;
// Handling fix - driveType is per model
#define HOOKPOS_CHandlingData_isNotRWD              0x6A048C
DWORD RETURN_CHandlingData_isNotRWD = 0x6A0493;
#define HOOKPOS_CHandlingData_isNotFWD              0x6A04BC
DWORD RETURN_CHandlingData_isNotFWD = 0x6A04C3;
// end of handling fix
#define CALL_CAutomobile_ProcessEntityCollision             0x6AD053
#define CALL_CMonsterTruck_ProcessEntityCollision           0x6C8B9E
DWORD RETURN_ProcessEntityCollision = 0x4185C0;

#define HOOKPOS_PreFxRender                                     0x049E650
DWORD RETURN_PreFxRender = 0x0404D1E;

#define HOOKPOS_PostColorFilterRender                             0x705099
DWORD RETURN_PostColorFilterRender = 0x70509E;

#define HOOKPOS_PreHUDRender                                      0x053EAD8
DWORD RETURN_PreHUDRender = 0x053EADD;

#define HOOKPOS_LoadIPLInstance                                    0x4061E8
DWORD CALL_LoadIPLInstance = 0x538090;
DWORD RETURN_LoadIPLInstance = 0x04061ED;

#define HOOKPOS_CWorld_LOD_SETUP                                  0x406224
#define HOOKPOS_CWorld_LOD_SETUP2                                 0x406326
DWORD CALL_CWorld_LODSETUP = 0x404C90;

#define HOOKPOS_CBuilding_DTR                                     0x404180
DWORD JMP_CBuilding_DTR = 0x535E90;

#define HOOKPOS_CDummy_DTR                                        0x532566
DWORD JMP_CDummy_DTR = 0x535E90;

#define HOOKPOS_CObject_DTR                                       0x59F680
DWORD JMP_CObject_DTR = 0x59F686;

#define HOOKPOS_AddBuildingInstancesToWorld_CWorldAdd             0x5B5348
DWORD JMP_CWorld_Add_AddBuildingInstancesToWorld_CALL_CWorldAdd = 0x563220;
DWORD RETURN_AddBuildingInstancesToWorld_CWorldAdd = 0x5B534D;

#define HOOKPOS_CWorld_Remove_CPopulation_ConvertToDummyObject    0x6146F8

#define HOOKPOS_CWorld_ADD_CPopulation_ConvertToRealObject              0x6145C7
DWORD JMP_CWorld_Add_CPopulation_ConvertToRealObject_Retn = 0x6145CC;
DWORD JMP_CWorld_Add_CPopulation_ConvertToRealObject_CallCWorldAdd = 0x563220;

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

#define HOOKPOS_CClothes_RebuildPlayer                      0x5A82C0
DWORD RETURN_CClothes_RebuildPlayera = 0x5A82C8;
DWORD RETURN_CClothes_RebuildPlayerb = 0x5A837F;

#define HOOKPOS_CProjectileInfo_FindPlayerPed               0x739321
#define HOOKPOS_CProjectileInfo_FindPlayerVehicle           0x739570

#define HOOKPOS_CHeli_ProcessHeliKill                       0x6DB201
DWORD RETURN_CHeli_ProcessHeliKill_RETN_Cancel = 0x6DB9E0;
DWORD RETURN_CHeli_ProcessHeliKill_RETN_Cont_Zero = 0x6DB207;
DWORD RETURN_CHeli_ProcessHeliKill_6DB437h = 0x6DB437;

#define HOOKPOS_CObject_ProcessBreak                        0x5A0F0F
DWORD RETURN_CObject_ProcessBreak = 0x5A0F14;
#define HOOKPOS_CObject_ProcessDamage                       0x5A0E0D
DWORD RETURN_CObject_ProcessDamage = 0x5A0E13;
DWORD RETURN_CObject_ProcessDamage_Cancel = 0x5A1241;
#define HOOKPOS_CObject_ProcessCollision                    0x548DC7
DWORD RETURN_CObject_ProcessCollision = 0x548DD1;
DWORD JMP_DynamicObject_Cond_Zero = 0x548E98;
#define HOOKPOS_CGlass_WindowRespondsToCollision           0x71BC40
DWORD RETURN_CGlass_WindowRespondsToCollision = 0x71BC48;
#define HOOKPOS_CGlass__BreakGlassPhysically                0x71D14B
DWORD RETURN_CGlass__BreakGlassPhysically = 0x71D150;
#define HOOKPOS_CGlass_WindowRespondsToExplosion            0x71C255
DWORD RETURN_CGlass_WindowRespondsToExplosion = 0x71C25A;
constexpr const DWORD CALL_FROM_CGlass_WindowRespondsToExplosion = 0x71C28E;
constexpr const DWORD CALL_FROM_CGlass_WasGlassHitByBullet = 0x71C192;
constexpr const DWORD CALL_FROM_CPhysical_ApplyCollision = 0x548F39;
constexpr const DWORD CALL_FROM_CPhysical_ApplyCollision_2 = 0x5490AE;
constexpr const DWORD CALL_FROM_CPhysical_ApplySoftCollision = 0x54A816;

#define HOOKPOS_FxManager_c__DestroyFxSystem                0x4A989A

#define HOOKPOS_CTaskSimplyGangDriveBy__ProcessPed          0x62D5A7
DWORD RETURN_CTaskSimplyGangDriveBy__ProcessPed = 0x62D5AC;

#define HOOKPOS_CAERadioTrackManager__ChooseMusicTrackIndex 0x4EA296
DWORD RETURN_CAERadioTrackManager__ChooseMusicTrackIndex = 0x4EA2A0;
DWORD RETURN_CAERadioTrackManager__ChooseMusicTrackIndex_Regenerate = 0x04EA286;

#define HOOKPOS_CAEVEhicleAudioEntity__ProcessDummyHeli     0x4FE9B9
DWORD RETURN_CAEVEhicleAudioEntity__ProcessDummyHeli = 0x4FEDFB;
DWORD dwFUNC_CAEVehicleAudioEntity__ProcessAIHeli = FUNC_CAEVehicleAudioEntity__ProcessAIHeli;

#define HOOKPOS_CAEVEhicleAudioEntity__ProcessDummyProp     0x4FD96D
DWORD RETURN_CAEVEhicleAudioEntity__ProcessDummyProp = 0x4FDFAB;
DWORD dwFUNC_CAEVehicleAudioEntity__ProcessAIProp = FUNC_CAEVehicleAudioEntity__ProcessAIProp;

#define HOOKPOS_CTaskSimpleSwim_ProcessSwimmingResistance   0x68A4EF
DWORD       RETURN_CTaskSimpleSwim_ProcessSwimmingResistance = 0x68A50E;
const DWORD HOOKPOS_Idle_CWorld_ProcessPedsAfterPreRender = 0x53EA03;
const DWORD RETURN_Idle_CWorld_ProcessPedsAfterPreRender = 0x53EA08;

#define HOOKPOS_CAEAmbienceTrackManager__UpdateAmbienceTrackAndVolume_StartRadio    0x4D7198
#define HOOKPOS_CAEAmbienceTrackManager__UpdateAmbienceTrackAndVolume_StopRadio     0x4D71E7

#define HOOKPOS_CAutomobile__dmgDrawCarCollidingParticles 0x6A6FF0

#define HOOKPOS_CWeapon__TakePhotograph 0x73C26E

#define HOOKPOS_CCollision__CheckCameraCollisionObjects 0x41AB8E

CPed*         pContextSwitchedPed = 0;
CVector       vecCenterOfWorld;
FLOAT         fFalseHeading;
bool          bSetCenterOfWorld;
DWORD         dwVectorPointer;
bool          bInStreamingUpdate;
bool          bHideRadar;
bool          bHasProcessedScript;
float         fX, fY, fZ;
DWORD         RoadSignFixTemp;
bool          m_bExplosionsDisabled;
float         fGlobalGravity = 0.008f;
float         fLocalPlayerGravity = 0.008f;
float         fLocalPlayerCameraRotation = 0.0f;
bool          bCustomCameraRotation = false;
unsigned char ucTrafficLightState = 0;
bool          bTrafficLightsBlocked = false;
bool          bInteriorSoundsEnabled = true;
bool          bInteriorFurnitureStates[5] = {true, true, true, true, true};

bool  bUsingCustomSkyGradient = false;
BYTE  ucSkyGradientTopR = 0;
BYTE  ucSkyGradientTopG = 0;
BYTE  ucSkyGradientTopB = 0;
BYTE  ucSkyGradientBottomR = 0;
BYTE  ucSkyGradientBottomG = 0;
BYTE  ucSkyGradientBottomB = 0;

bool  bUsingCustomAmbientColor = false;
float fAmbientColorR = 0.0F;
float fAmbientColorG = 0.0F;
float fAmbientColorB = 0.0F;

bool  bUsingCustomAmbientObjectColor = false;
float fAmbientObjectColorR = 0.0F;
float fAmbientObjectColorG = 0.0F;
float fAmbientObjectColorB = 0.0F;

bool  bUsingCustomDirectionalColor = false;
float fDirectionalColorR = 0.0F;
float fDirectionalColorG = 0.0F;
float fDirectionalColorB = 0.0F;

bool  bUsingCustomLowCloudsColor = false;
int16 iLowCloudsColorR = 0;
int16 iLowCloudsColorG = 0;
int16 iLowCloudsColorB = 0;

bool  bUsingCustomBottomCloudsColor = false;
int16 iBottomCloudsColorR = 0;
int16 iBottomCloudsColorG = 0;
int16 iBottomCloudsColorB = 0;

bool  bUsingCustomWaterColor = false;
float fWaterColorR = 0.0F;
float fWaterColorG = 0.0F;
float fWaterColorB = 0.0F;
float fWaterColorA = 0.0F;

CStatsData  localStatsData;
bool        bLocalStatsStatic = true;
extern bool bWeaponFire;
float       fDuckingHealthThreshold;

static const std::array<uint32_t, 16> shadowAddr{
    0x6FAD5D,            // CRegisteredCorona::Update
    0x7041DB,            // CPostEffects::Fog
    0x7085A9,            // CShadows::RenderStaticShadows
    0x709B2F,            // CShadows::CastShadowEntityXY
    0x709B8E,            // CShadows::CastShadowEntityXY
    0x709BC7,            // CShadows::CastShadowEntityXY
    0x709BF6,            // CShadows::CastShadowEntityXY
    0x709C93,            // CShadows::CastShadowEntityXY
    0x709E9E,            // IntersectEntityRenderTriangleCB
    0x709EBC,            // IntersectEntityRenderTriangleCB
    0x709ED7,            // IntersectEntityRenderTriangleCB
    0x70B221,            // CShadows::RenderStoredShadows
    0x70B373,            // CShadows::RenderStoredShadows
    0x70B4D1,            // CShadows::RenderStoredShadows
    0x70B635,            // CShadows::RenderStoredShadows
    0x73A48F             // CWeapon::AddGunshell
};

PreContextSwitchHandler*                   m_pPreContextSwitchHandler = NULL;
PostContextSwitchHandler*                  m_pPostContextSwitchHandler = NULL;
PreWeaponFireHandler*                      m_pPreWeaponFireHandler = NULL;
PostWeaponFireHandler*                     m_pPostWeaponFireHandler = NULL;
BulletImpactHandler*                       m_pBulletImpactHandler = NULL;
BulletFireHandler*                         m_pBulletFireHandler = NULL;
DamageHandler*                             m_pDamageHandler = NULL;
DeathHandler*                              m_pDeathHandler = NULL;
FireHandler*                               m_pFireHandler = NULL;
ProjectileHandler*                         m_pProjectileHandler = NULL;
ProjectileStopHandler*                     m_pProjectileStopHandler = NULL;
ProcessCamHandler*                         m_pProcessCamHandler = NULL;
ChokingHandler*                            m_pChokingHandler = NULL;
ExplosionHandler*                          m_pExplosionHandler = NULL;
BreakTowLinkHandler*                       m_pBreakTowLinkHandler = NULL;
DrawRadarAreasHandler*                     m_pDrawRadarAreasHandler = NULL;
Render3DStuffHandler*                      m_pRender3DStuffHandler = NULL;
PreWorldProcessHandler*                    m_pPreWorldProcessHandler = NULL;
PostWorldProcessHandler*                   m_pPostWorldProcessHandler = NULL;
PostWorldProcessPedsAfterPreRenderHandler* m_postWorldProcessPedsAfterPreRenderHandler = nullptr;
IdleHandler*                               m_pIdleHandler = NULL;
PreFxRenderHandler*                        m_pPreFxRenderHandler = NULL;
PostColorFilterRenderHandler*              m_pPostColorFilterRenderHandler = nullptr;
PreHudRenderHandler*                       m_pPreHudRenderHandler = NULL;
ProcessCollisionHandler*                   m_pProcessCollisionHandler = NULL;
HeliKillHandler*                           m_pHeliKillHandler = NULL;
ObjectDamageHandler*                       m_pObjectDamageHandler = NULL;
ObjectBreakHandler*                        m_pObjectBreakHandler = NULL;
FxSystemDestructionHandler*                m_pFxSystemDestructionHandler = NULL;
DrivebyAnimationHandler*                   m_pDrivebyAnimationHandler = NULL;
AudioZoneRadioSwitchHandler*               m_pAudioZoneRadioSwitchHandler = NULL;

CEntitySAInterface* dwSavedPlayerPointer = 0;
CEntitySAInterface* activeEntityForStreaming = 0;            // the entity that the streaming system considers active

void HOOK_FindPlayerCoors();
void HOOK_FindPlayerCentreOfWorld();
void HOOK_FindPlayerHeading();
void HOOK_CStreaming_Update_Caller();
void HOOK_CHud_Draw_Caller();
void HOOK_CRunningScript_Process();
void HOOK_CExplosion_AddExplosion();
void HOOK_CCustomRoadsignMgr__RenderRoadsignAtomic();
void HOOK_Trailer_BreakTowLink();
void HOOK_CRadar__DrawRadarGangOverlay();
void HOOK_CTaskComplexJump__CreateSubTask();
void HOOK_FxManager_CreateFxSystem();
void HOOK_FxManager_DestroyFxSystem();
void HOOK_CCam_ProcessFixed();
void HOOK_Render3DStuff();
void HOOK_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon();
void HOOK_CPed_IsPlayer();
void HOOK_CTrain_ProcessControl_Derail();
void HOOK_CVehicle_SetupRender();
void HOOK_CVehicle_ResetAfterRender();
void HOOK_CObject_Render();
void HOOK_EndWorldColors();
void HOOK_CWorld_ProcessVerticalLineSectorList();
void HOOK_ComputeDamageResponse_StartChoking();
void HOOK_CollisionStreamRead();
void HOOK_CVehicle_ApplyBoatWaterResistance();
void HOOK_CPhysical_ApplyGravity();
void HOOK_VehicleCamStart();
void HOOK_VehicleCamTargetZTweak();
void HOOK_VehicleCamLookDir1();
void HOOK_VehicleCamLookDir2();
void HOOK_VehicleCamHistory();
void HOOK_VehicleCamUp();
void HOOK_VehicleCamEnd();
void HOOK_VehicleLookBehind();
void HOOK_VehicleLookAside();
void HOOK_OccupiedVehicleBurnCheck();
void HOOK_UnoccupiedVehicleBurnCheck();
void HOOK_ApplyCarBlowHop();
void HOOK_CWorld_SetWorldOnFire();
void HOOK_CTaskSimplePlayerOnFire_ProcessPed();
void HOOK_CFire_ProcessFire();
void HOOK_CExplosion_Update();
void HOOK_CWeapon_FireAreaEffect();
void HOOK_CGame_Process();
void HOOK_Idle();
void HOOK_RenderScene_Plants();
void HOOK_RenderScene_end();
void HOOK_CPlantMgr_Render();
void HOOK_CEventHandler_ComputeKnockOffBikeResponse();
void HOOK_CAnimBlendAssociation_SetCurrentTime();
void HOOK_RpAnimBlendClumpUpdateAnimations();
void HOOK_CAnimBlendAssoc_destructor();
void HOOK_CAnimManager_AddAnimation();
void HOOK_CAnimManager_AddAnimationAndSync();
void HOOK_CAnimManager_BlendAnimation_Hierarchy();
void HOOK_CPed_GetWeaponSkill();
void HOOK_CPed_AddGogglesModel();
void HOOK_CPhysical_ProcessCollisionSectorList();
void HOOK_CrashFix_Misc1();
void HOOK_CrashFix_Misc2();
void HOOK_CrashFix_Misc3();
void HOOK_CrashFix_Misc4();
void HOOK_CrashFix_Misc5();
void HOOK_CrashFix_Misc6();
void HOOK_CrashFix_Misc7();
void HOOK_CrashFix_Misc8();
void HOOK_CrashFix_Misc9();
void HOOK_CrashFix_Misc10();
void HOOK_CrashFix_Misc11();
void HOOK_CrashFix_Misc12();
void HOOK_CrashFix_Misc13();
void HOOK_CrashFix_Misc14();
void HOOK_FreezeFix_Misc15();
void HOOK_CrashFix_Misc16();
void HOOK_CrashFix_Misc17();
void HOOK_CrashFix_Misc18();
void HOOK_CrashFix_Misc19();
void HOOK_CrashFix_Misc20();
void HOOK_CrashFix_Misc21();
void HOOK_CrashFix_Misc22();
void HOOK_CrashFix_Misc23();
void HOOK_CrashFix_Misc24();
void HOOK_CheckAnimMatrix();
void HOOK_VehColCB();
void HOOK_VehCol();
void HOOK_Transmission_CalculateDriveAcceleration();
void HOOK_isVehDriveTypeNotRWD();
void HOOK_isVehDriveTypeNotFWD();
void HOOK_PreFxRender();
void HOOK_PostColorFilterRender();
void HOOK_PreHUDRender();

void HOOK_CTrafficLights_GetPrimaryLightState();
void HOOK_CTrafficLights_GetSecondaryLightState();
void HOOK_CTrafficLights_DisplayActualLight();

void HOOK_CAutomobile__ProcessSwingingDoor();

void vehicle_lights_init();

void HOOK_LoadIPLInstance();

void HOOK_CWorld_LOD_SETUP();

void Hook_AddBuildingInstancesToWorld();

void HOOK_CWorld_Remove_CPopulation_ConvertToDummyObject();

void HOOK_CWorld_Add_CPopulation_ConvertToDummyObject();

void Hook_CWorld_ADD_CPopulation_ConvertToRealObject();

void HOOK_ConvertToObject_CPopulationManageDummy();

void Hook_CBuilding_DTR();

void Hook_CDummy_DTR();

void Hook_CObject_DTR();

void HOOK_CEntity_IsOnScreen_FixObjectScale();

void HOOK_CClothes_RebuildPlayer();

void HOOK_CProjectileInfo_Update_FindLocalPlayer_FindLocalPlayerVehicle();

void HOOK_CHeli_ProcessHeliKill();

void HOOK_CObject_ProcessDamage();
void HOOK_CObject_ProcessBreak();
void HOOK_CObject_ProcessCollision();
void HOOK_CGlass_WindowRespondsToCollision();
void HOOK_CGlass__BreakGlassPhysically();

void HOOK_FxManager_c__DestroyFxSystem();

void HOOK_CTaskSimpleGangDriveBy__ProcessPed();

void HOOK_CAERadioTrackManager__ChooseMusicTrackIndex();

void HOOK_CAEVehicleAudioEntity__ProcessDummyHeli();
void HOOK_CAEVehicleAudioEntity__ProcessDummyProp();

void HOOK_CTaskSimpleSwim_ProcessSwimmingResistance();
void HOOK_Idle_CWorld_ProcessPedsAfterPreRender();

void HOOK_CAEAmbienceTrackManager__UpdateAmbienceTrackAndVolume_StartRadio();
void HOOK_CAEAmbienceTrackManager__UpdateAmbienceTrackAndVolume_StopRadio();

void HOOK_CAutomobile__dmgDrawCarCollidingParticles();

void HOOK_CWeapon__TakePhotograph();

void HOOK_CCollision__CheckCameraCollisionObjects();

CMultiplayerSA::CMultiplayerSA()
{
    // Unprotect all of the GTASA code at once and leave it that way
    SetInitialVirtualProtect();

    CRemoteDataSA::Init();

    m_bExplosionsDisabled = false;
    m_pExplosionHandler = NULL;
    m_pBreakTowLinkHandler = NULL;
    m_pDrawRadarAreasHandler = NULL;
    m_pDamageHandler = NULL;
    m_pFireHandler = NULL;
    m_pProjectileHandler = NULL;
    m_pProjectileStopHandler = NULL;

    MemSetFast(&localStatsData, 0, sizeof(CStatsData));
    localStatsData.StatTypesFloat[24] = 569.0f;            // Max Health

    m_fAircraftMaxHeight = 800.0f;

    m_fAircraftMaxVelocity = 1.5f;
    m_fAircraftMaxVelocity_Sq = m_fAircraftMaxVelocity * m_fAircraftMaxVelocity;

    m_bHeatHazeEnabled = true;
    m_bHeatHazeCustomized = false;
    m_fMaddDoggPoolLevel = 1082.73f;
    m_dwLastStaticAnimGroupID = eAnimGroup::ANIM_GROUP_DEFAULT;
    m_dwLastStaticAnimID = eAnimID::ANIM_ID_WALK;
}

void CMultiplayerSA::InitHooks()
{
    InitKeysyncHooks();
    InitShotsyncHooks();
    vehicle_lights_init();
    bSetCenterOfWorld = false;
    bHasProcessedScript = false;
    m_fNearClipDistance = DEFAULT_NEAR_CLIP_DISTANCE;

    bSetCenterOfWorld = false;

    // 00442DC6  |. 0F86 31090000  JBE gta_sa_u.004436FD
    // 00442DC6     E9 32090000    JMP gta_sa_u.004436FD

    // increase the number of vehicles types (not actual vehicles) that can be loaded at once
    MemPutFast<int>(0x8a5a84, 127);

    // DISABLE CGameLogic::Update
    MemSet((void*)0x442AD0, 0xC3, 1);

    // STOP IT TRYING TO LOAD THE SCM
    MemPut<BYTE>(0x468EB5, 0xEB);
    MemPut<BYTE>(0x468EB6, 0x32);

    HookInstall(HOOKPOS_FindPlayerCoors, (DWORD)HOOK_FindPlayerCoors, 6);
    HookInstall(HOOKPOS_FindPlayerCentreOfWorld, (DWORD)HOOK_FindPlayerCentreOfWorld, 6);
    HookInstall(HOOKPOS_FindPlayerHeading, (DWORD)HOOK_FindPlayerHeading, 6);
    HookInstall(HOOKPOS_CStreaming_Update_Caller, (DWORD)HOOK_CStreaming_Update_Caller, 7);
    HookInstall(HOOKPOS_CHud_Draw_Caller, (DWORD)HOOK_CHud_Draw_Caller, 10);
    HookInstall(HOOKPOS_CRunningScript_Process, (DWORD)HOOK_CRunningScript_Process, 6);
    HookInstall(HOOKPOS_CExplosion_AddExplosion, (DWORD)HOOK_CExplosion_AddExplosion, 6);
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
    HookInstall(HOOKPOS_CVehicle_ApplyBoatWaterResistance, (DWORD)HOOK_CVehicle_ApplyBoatWaterResistance, 6);
    HookInstall(HOOKPOS_CPhysical_ApplyGravity, (DWORD)HOOK_CPhysical_ApplyGravity, 6);
    HookInstall(HOOKPOS_OccupiedVehicleBurnCheck, (DWORD)HOOK_OccupiedVehicleBurnCheck, 6);
    HookInstall(HOOKPOS_UnoccupiedVehicleBurnCheck, (DWORD)HOOK_UnoccupiedVehicleBurnCheck, 5);
    HookInstall(HOOKPOS_ApplyCarBlowHop, (DWORD)HOOK_ApplyCarBlowHop, 6);
    HookInstall(HOOKPOS_CWorld_SetWorldOnFire, (DWORD)HOOK_CWorld_SetWorldOnFire, 5);
    HookInstall(HOOKPOS_CTaskSimplePlayerOnFire_ProcessPed, (DWORD)HOOK_CTaskSimplePlayerOnFire_ProcessPed, 5);
    HookInstall(HOOKPOS_CFire_ProcessFire, (DWORD)HOOK_CFire_ProcessFire, 5);
    HookInstall(HOOKPOS_CExplosion_Update, (DWORD)HOOK_CExplosion_Update, 5);
    HookInstall(HOOKPOS_CWeapon_FireAreaEffect, (DWORD)HOOK_CWeapon_FireAreaEffect, 5);
    HookInstall(HOOKPOS_CGame_Process, (DWORD)HOOK_CGame_Process, 10);
    HookInstall(HOOKPOS_Idle, (DWORD)HOOK_Idle, 10);
    HookInstall(HOOKPOS_CEventHandler_ComputeKnockOffBikeResponse, (DWORD)HOOK_CEventHandler_ComputeKnockOffBikeResponse, 7);
    HookInstall(HOOKPOS_CAnimBlendAssociation_SetCurrentTime, (DWORD)HOOK_CAnimBlendAssociation_SetCurrentTime, 8);
    HookInstall(HOOKPOS_RpAnimBlendClumpUpdateAnimations, (DWORD)HOOK_RpAnimBlendClumpUpdateAnimations, 8);
    HookInstall(HOOKPOS_CAnimBlendAssoc_destructor, (DWORD)HOOK_CAnimBlendAssoc_destructor, 6);
    HookInstall(HOOKPOS_CPed_GetWeaponSkill, (DWORD)HOOK_CPed_GetWeaponSkill, 8);
    HookInstall(HOOKPOS_CPed_AddGogglesModel, (DWORD)HOOK_CPed_AddGogglesModel, 6);
    HookInstall(HOOKPOS_CPhysical_ProcessCollisionSectorList, (DWORD)HOOK_CPhysical_ProcessCollisionSectorList, 7);
    HookInstall(HOOKPOS_CheckAnimMatrix, (DWORD)HOOK_CheckAnimMatrix, 5);


    HookInstall(HOOKPOS_VehColCB, (DWORD)HOOK_VehColCB, 29);
    HookInstall(HOOKPOS_VehCol, (DWORD)HOOK_VehCol, 9);
    HookInstall(HOOKPOS_PreFxRender, (DWORD)HOOK_PreFxRender, 5);
    HookInstall(HOOKPOS_PostColorFilterRender, (DWORD)HOOK_PostColorFilterRender, 5);
    HookInstall(HOOKPOS_PreHUDRender, (DWORD)HOOK_PreHUDRender, 5);
    HookInstall(HOOKPOS_CAutomobile__ProcessSwingingDoor, (DWORD)HOOK_CAutomobile__ProcessSwingingDoor, 7);

    HookInstall(HOOKPOS_Transmission_CalculateDriveAcceleration, (DWORD)HOOK_Transmission_CalculateDriveAcceleration, 5);

    HookInstall(HOOKPOS_CHandlingData_isNotRWD, (DWORD)HOOK_isVehDriveTypeNotRWD, 7);
    HookInstall(HOOKPOS_CHandlingData_isNotFWD, (DWORD)HOOK_isVehDriveTypeNotFWD, 7);

    HookInstallCall(CALL_Render3DStuff, (DWORD)HOOK_Render3DStuff);
    HookInstallCall(CALL_VehicleCamUp, (DWORD)HOOK_VehicleCamUp);
    HookInstallCall(CALL_VehicleLookBehindUp, (DWORD)HOOK_VehicleCamUp);
    HookInstallCall(CALL_VehicleLookAsideUp, (DWORD)HOOK_VehicleCamUp);

    HookInstallCall(CALL_CTrafficLights_GetPrimaryLightState, (DWORD)HOOK_CTrafficLights_GetPrimaryLightState);
    HookInstallCall(CALL_CTrafficLights_GetSecondaryLightState, (DWORD)HOOK_CTrafficLights_GetSecondaryLightState);
    HookInstall(HOOKPOS_CTrafficLights_DisplayActualLight, (DWORD)HOOK_CTrafficLights_DisplayActualLight, 36);

    HookInstall(HOOKPOS_CEntity_IsOnScreen_FixObjectsScale, (DWORD)HOOK_CEntity_IsOnScreen_FixObjectScale, 7);

    // Start of Building removal hooks
    HookInstallCall(HOOKPOS_LoadIPLInstance, (DWORD)HOOK_LoadIPLInstance);

    HookInstallCall(HOOKPOS_CWorld_LOD_SETUP, (DWORD)HOOK_CWorld_LOD_SETUP);

    HookInstallCall(HOOKPOS_CWorld_LOD_SETUP2, (DWORD)HOOK_CWorld_LOD_SETUP);

    HookInstall(HOOKPOS_CBuilding_DTR, (DWORD)Hook_CBuilding_DTR, 5);

    HookInstall(HOOKPOS_CDummy_DTR, (DWORD)Hook_CDummy_DTR, 5);

    HookInstall(HOOKPOS_CObject_DTR, (DWORD)Hook_CObject_DTR, 6);

    HookInstallCall(HOOKPOS_AddBuildingInstancesToWorld_CWorldAdd, (DWORD)Hook_AddBuildingInstancesToWorld);

    HookInstallCall(HOOKPOS_CWorld_ADD_CPopulation_ConvertToRealObject, (DWORD)Hook_CWorld_ADD_CPopulation_ConvertToRealObject);

    HookInstallCall(HOOKPOS_CWorld_Remove_CPopulation_ConvertToDummyObject, (DWORD)HOOK_CWorld_Remove_CPopulation_ConvertToDummyObject);

    HookInstall(HOOKPOS_CWorld_ADD_CPopulation_ConvertToDummyObject, (DWORD)HOOK_CWorld_Add_CPopulation_ConvertToDummyObject, 6);

    HookInstall(HOOKPOS_ConvertToObject_CPopulationManageDummy, (DWORD)HOOK_ConvertToObject_CPopulationManageDummy, 6);
    // End of building removal hooks

    // Spider CJ fix
    HookInstall(HOOKPOS_CClothes_RebuildPlayer, (DWORD)HOOK_CClothes_RebuildPlayer, 8);

    // Fix for projectiles firing too fast locally.
    HookInstallCall((DWORD)HOOKPOS_CProjectileInfo_FindPlayerPed, (DWORD)HOOK_CProjectileInfo_Update_FindLocalPlayer_FindLocalPlayerVehicle);
    HookInstallCall((DWORD)HOOKPOS_CProjectileInfo_FindPlayerVehicle, (DWORD)HOOK_CProjectileInfo_Update_FindLocalPlayer_FindLocalPlayerVehicle);

    HookInstall((DWORD)HOOKPOS_CHeli_ProcessHeliKill, (DWORD)HOOK_CHeli_ProcessHeliKill, 6);

    // Hooks for object break events
    HookInstall(HOOKPOS_CObject_ProcessDamage, (DWORD)HOOK_CObject_ProcessDamage, 6);
    HookInstall(HOOKPOS_CObject_ProcessBreak, (DWORD)HOOK_CObject_ProcessBreak, 5);
    HookInstall(HOOKPOS_CObject_ProcessCollision, (DWORD)HOOK_CObject_ProcessCollision, 10);
    HookInstall(HOOKPOS_CGlass_WindowRespondsToCollision, (DWORD)HOOK_CGlass_WindowRespondsToCollision, 8);
    HookInstall(HOOKPOS_CGlass__BreakGlassPhysically, (DWORD)HOOK_CGlass__BreakGlassPhysically, 5);
    
    // Post-destruction hook for FxSystems
    HookInstall(HOOKPOS_FxManager_c__DestroyFxSystem, (DWORD)HOOK_FxManager_c__DestroyFxSystem, 5);

    // CTaskSimpleGangDriveBy::ProcessPed hook for disabling certain animations
    HookInstall(HOOKPOS_CTaskSimplyGangDriveBy__ProcessPed, (DWORD)HOOK_CTaskSimpleGangDriveBy__ProcessPed, 5);

    SString strTrakLkupMd5 = CMD5Hasher::CalculateHexString(PathJoin(GetLaunchPath(), "audio", "CONFIG", "TrakLkup.dat"));
    if (strTrakLkupMd5 != "528E75D663B8BAE072A01351081A2145")
    {
        // CAERadioTrackManager::ChooseMusicTrackIndex hook for fixing a crash with the steam audio files
        HookInstall(HOOKPOS_CAERadioTrackManager__ChooseMusicTrackIndex, (DWORD)HOOK_CAERadioTrackManager__ChooseMusicTrackIndex, 10);
    }

    HookInstall(HOOKPOS_CAEVEhicleAudioEntity__ProcessDummyHeli, (DWORD)HOOK_CAEVehicleAudioEntity__ProcessDummyHeli, 5);
    HookInstall(HOOKPOS_CAEVEhicleAudioEntity__ProcessDummyProp, (DWORD)HOOK_CAEVehicleAudioEntity__ProcessDummyProp, 5);

    // Fix GTA:SA swimming speed problem on higher fps
    HookInstall(HOOKPOS_CTaskSimpleSwim_ProcessSwimmingResistance, (DWORD)HOOK_CTaskSimpleSwim_ProcessSwimmingResistance, 6);
    HookInstall(HOOKPOS_Idle_CWorld_ProcessPedsAfterPreRender, (DWORD)HOOK_Idle_CWorld_ProcessPedsAfterPreRender, 5);

    HookInstall(HOOKPOS_CAnimManager_AddAnimation, (DWORD)HOOK_CAnimManager_AddAnimation, 10);
    HookInstall(HOOKPOS_CAnimManager_AddAnimationAndSync, (DWORD)HOOK_CAnimManager_AddAnimationAndSync, 10);
    HookInstall(HOOKPOS_CAnimManager_BlendAnimation_Hierarchy, (DWORD)HOOK_CAnimManager_BlendAnimation_Hierarchy, 5);    

    HookInstall(HOOKPOS_CAEAmbienceTrackManager__UpdateAmbienceTrackAndVolume_StartRadio,
                (DWORD)HOOK_CAEAmbienceTrackManager__UpdateAmbienceTrackAndVolume_StartRadio, 5);
    HookInstall(HOOKPOS_CAEAmbienceTrackManager__UpdateAmbienceTrackAndVolume_StopRadio,
                (DWORD)HOOK_CAEAmbienceTrackManager__UpdateAmbienceTrackAndVolume_StopRadio, 5);

    HookInstall(HOOKPOS_CAutomobile__dmgDrawCarCollidingParticles, (DWORD)HOOK_CAutomobile__dmgDrawCarCollidingParticles, 0x91);

    HookInstall(HOOKPOS_CWeapon__TakePhotograph, (DWORD)HOOK_CWeapon__TakePhotograph, 3 + 2);

    HookInstall(HOOKPOS_CCollision__CheckCameraCollisionObjects, (DWORD)HOOK_CCollision__CheckCameraCollisionObjects, 6 + 4);

    // Disable GTA setting g_bGotFocus to false when we minimize
    MemSet((void*)ADDR_GotFocus, 0x90, 10);

    // Disable GTA being able to call CAudio::StopRadio ()
    // Well this isn't really CAudio::StopRadio, it's some global class
    // func that StopRadio just jumps to.
    MemPut<BYTE>(0x4E9820, 0xC2);
    MemPut<BYTE>(0x4E9821, 0x08);
    MemPut<BYTE>(0x4E9822, 0x00);

    // Disable GTA being able to call CAudio::StartRadio ()
    MemPut<BYTE>(0x4DBEC0, 0xC2);
    MemPut<BYTE>(0x4DBEC1, 0x00);
    MemPut<BYTE>(0x4DBEC2, 0x00);

    MemPut<BYTE>(0x4EB3C0, 0xC2);
    MemPut<BYTE>(0x4EB3C1, 0x10);
    MemPut<BYTE>(0x4EB3C2, 0x00);

    // Disable automatic switching cinematic camera for trains
    MemPut<WORD>(0x52A50B, 0x29EB);

    // Enable camera view mode switching in trains
    MemPut<BYTE>(0x528152, 0x12);
    MemPut<WORD>(0x52815B, 0x03EB);

    // DISABLE wanted levels for military zones
    MemPut<BYTE>(0x72DF0D, 0xEB);

    // THROWN projectiles throw more accurately
    MemPut<BYTE>(0x742685, 0x90);
    MemPut<BYTE>(0x742686, 0xE9);

    // DISABLE CProjectileInfo::RemoveAllProjectiles
    MemPut<BYTE>(0x7399B0, 0xC3);

    // DISABLE CRoadBlocks::GenerateRoadblocks
    MemPut<BYTE>(0x4629E0, 0xC3);

    // Disable CPopulation::RemovePed
    MemPut<BYTE>(0x610F20, 0xC3);

    // Temporary hack for disabling hand up
    /*
    MemPut < BYTE > ( 0x62AEE7, 0x90 );
    MemPut < BYTE > ( 0x62AEE8, 0x90 );
    MemPut < BYTE > ( 0x62AEE9, 0x90 );
    MemPut < BYTE > ( 0x62AEEA, 0x90 );
    MemPut < BYTE > ( 0x62AEEB, 0x90 );
    MemPut < BYTE > ( 0x62AEEC, 0x90 );
    */

    // DISABLE CAERadioTrackManager::CheckForMissionStatsChanges() (special DJ banter)
    MemPut<BYTE>(0x4E8410, 0xC3);

    // DISABLE CPopulation__AddToPopulation
    MemPut<BYTE>(0x614720, 0x32);
    MemPut<BYTE>(0x614721, 0xC0);
    MemPut<BYTE>(0x614722, 0xC3);

    // Disables deletion of RenderWare objects during unloading of ModelInfo
    // This is used so we can circumvent the limit of ~21 different vehicles by managing the RwObject ourselves
    //*(BYTE *)0x4C9890 = 0xC3;

    // MemSet ( (void*)0x408A1B, 0x90, 5 );

    // Hack to make the choke task use 0 time left remaining when he starts t
    // just stand there looking. So he won't do that.
    MemPut<unsigned char>(0x620607, 0x33);
    MemPut<unsigned char>(0x620608, 0xC0);

    MemPut<unsigned char>(0x620618, 0x33);
    MemPut<unsigned char>(0x620619, 0xC0);
    MemPut<unsigned char>(0x62061A, 0x90);
    MemPut<unsigned char>(0x62061B, 0x90);
    MemPut<unsigned char>(0x62061C, 0x90);

    // Hack to make non-local players always update their aim on akimbo weapons using camera
    // so they don't freeze when local player doesn't aim.
    MemPut<BYTE>(0x61EFFE, 0xEB);

    // DISABLE CGameLogic__SetPlayerWantedLevelForForbiddenTerritories
    MemPut<BYTE>(0x441770, 0xC3);

    // DISABLE CCrime__ReportCrime
    MemPut<BYTE>(0x532010, 0xC3);

    // Disables deletion of RenderWare objects during unloading of ModelInfo
    // This is used so we can circumvent the limit of ~21 different vehicles by managing the RwObject ourselves
    //*(BYTE *)0x4C9890 = 0xC3;

    /*
    004C021D   B0 00            MOV AL,0
    004C021F   90               NOP
    004C0220   90               NOP
    004C0221   90               NOP
    */
    MemPut<BYTE>(0x4C01F0, 0xB0);
    MemPut<BYTE>(0x4C01F1, 0x00);
    MemPut<BYTE>(0x4C01F2, 0x90);
    MemPut<BYTE>(0x4C01F3, 0x90);
    MemPut<BYTE>(0x4C01F4, 0x90);

    // Disable MakePlayerSafe
    MemPut<BYTE>(0x56E870, 0xC2);
    MemPut<BYTE>(0x56E871, 0x08);
    MemPut<BYTE>(0x56E872, 0x00);

    // Disable call to FxSystem_c__GetCompositeMatrix in CAEFireAudioEntity::UpdateParameters
    // that was causing a crash - spent ages debugging, the crash happens if you create 40 or
    // so vehicles that catch fire (upside down) then delete them, repeating a few times.
    // MemSet ((void*)0x4DCF87,0x90,6);
    //
    // The above MemSet was commented out because of mantis#8590, gh#124, see c20d2adc5

    /*
    // DISABLE CPed__RemoveBodyPart
    MemPut < BYTE > ( 0x5F0140, 0xC2 );
    MemPut < BYTE > ( 0x5F0141, 0x08 );
    MemPut < BYTE > ( 0x5F0142, 0x00 );
    */

    // ALLOW picking up of all vehicles (GTA doesn't allow picking up of 'locked' script created vehicles)
    MemPut<BYTE>(0x6A436C, 0x90);
    MemPut<BYTE>(0x6A436D, 0x90);

    // MAKE CEntity::GetIsOnScreen always return true, experimental
    /*
     MemPut < BYTE > ( 0x534540, 0xB0 );
     MemPut < BYTE > ( 0x534541, 0x01 );
     MemPut < BYTE > ( 0x534542, 0xC3 );
     */

    // DISABLE CPad::ReconcileTwoControllersInput
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
    // 00615FE3     EB 09          JMP SHORT gta_sa_u.00615FEE
    /*
    MemSet ((void *)0x615FE3, 0xEB, 1);
    */

    // Make combines eat players *untested*
    // MemSet ( (LPVOID)0x6A9739, 0x90, 6 );

    // Players always lean out whatever the camera mode
    // 00621983     EB 13          JMP SHORT hacked_g.00621998
    MemPut<BYTE>(0x621983, 0xEB);

    // Players can fire drivebys whatever camera mode
    // 627E01 - 6 bytes
    MemSet((LPVOID)0x627E01, 0x90, 6);

    MemSet((LPVOID)0x62840D, 0x90, 6);

    // Satchel crash fix
    // C89110: satchel (bomb) positions pointer?
    // C891A8+4: satchel (model) positions pointer? gets set to NULL on player death, causing an access violation
    // C891A8+12: satchel (model) disappear time (in SystemTime format). 738F99 clears the satchel when VAR_SystemTime is larger.
    MemSet((LPVOID)0x738F3A, 0x90, 83);

    // Prevent gta stopping driveby players from falling off
    MemSet((LPVOID)0x6B5B17, 0x90, 6);

    // Increase VehicleStruct pool size
    MemPut<BYTE>(0x5B8342 + 0, 0x33);            // xor eax, eax
    MemPut<BYTE>(0x5B8342 + 1, 0xC0);
    MemPut<BYTE>(0x5B8342 + 2, 0xB0);            // mov al, 0xFF
    MemPut<BYTE>(0x5B8342 + 3, 0xFF);
    MemPut<BYTE>(0x5B8342 + 4, 0x8B);            // mov edi, eax
    MemPut<BYTE>(0x5B8342 + 5, 0xF8);

    /*
    // CTaskSimpleCarDrive: Swaps driveby for gang-driveby for drivers
    MemSet ( (LPVOID)0x6446A7, 0x90, 6 );

    // CTaskSimpleCarDrive: Swaps driveby for gang-driveby for passengers
    MemSet ( (LPVOID)0x6446BD, 0x90, 6 );
    */

    // DISABLE PLAYING REPLAYS
    MemSet((void*)0x460390, 0xC3, 1);

    MemSet((void*)0x4600F0, 0xC3, 1);

    MemSet((void*)0x45F050, 0xC3, 1);

    // DISABLE CHEATS
    MemSet((void*)0x439AF0, 0xC3, 1);

    MemSet((void*)0x438370, 0xC3, 1);

    // DISABLE GARAGES
    MemPut<BYTE>(0x44AA89 + 0, 0xE9);
    MemPut<BYTE>(0x44AA89 + 1, 0x28);
    MemPut<BYTE>(0x44AA89 + 2, 0x01);
    MemPut<BYTE>(0x44AA89 + 3, 0x00);
    MemPut<BYTE>(0x44AA89 + 4, 0x00);
    MemPut<BYTE>(0x44AA89 + 5, 0x90);

    MemPut<DWORD>(0x44C7E0, 0x44C7C4);
    MemPut<DWORD>(0x44C7E4, 0x44C7C4);
    MemPut<DWORD>(0x44C7F8, 0x44C7C4);
    MemPut<DWORD>(0x44C7FC, 0x44C7C4);
    MemPut<DWORD>(0x44C804, 0x44C7C4);
    MemPut<DWORD>(0x44C808, 0x44C7C4);
    MemPut<DWORD>(0x44C83C, 0x44C7C4);
    MemPut<DWORD>(0x44C840, 0x44C7C4);
    MemPut<DWORD>(0x44C850, 0x44C7C4);
    MemPut<DWORD>(0x44C854, 0x44C7C4);
    MemPut<DWORD>(0x44C864, 0x44C7C4);
    MemPut<DWORD>(0x44C868, 0x44C7C4);
    MemPut<DWORD>(0x44C874, 0x44C7C4);
    MemPut<DWORD>(0x44C878, 0x44C7C4);
    MemPut<DWORD>(0x44C88C, 0x44C7C4);
    MemPut<DWORD>(0x44C890, 0x44C7C4);
    MemPut<DWORD>(0x44C89C, 0x44C7C4);
    MemPut<DWORD>(0x44C8A0, 0x44C7C4);
    MemPut<DWORD>(0x44C8AC, 0x44C7C4);
    MemPut<DWORD>(0x44C8B0, 0x44C7C4);

    MemPut<BYTE>(0x44C39A + 0, 0x0F);
    MemPut<BYTE>(0x44C39A + 1, 0x84);
    MemPut<BYTE>(0x44C39A + 2, 0x24);
    MemPut<BYTE>(0x44C39A + 3, 0x04);
    MemPut<BYTE>(0x44C39A + 4, 0x00);
    MemPut<BYTE>(0x44C39A + 5, 0x00);

    // Avoid garage doors closing when you change your model
    MemSet((LPVOID)0x4486F7, 0x90, 4);

    // Disable CStats::IncrementStat (returns at start of function)
    MemPut<BYTE>(0x55C180, 0xC3);
    /*
    MemSet ((void *)0x55C1A9, 0x90, 14 );
    MemSet ((void *)0x55C1DD, 0x90, 7 );
    */

    // DISABLE STATS DECREMENTING
    MemSet((void*)0x559FD5, 0x90, 7);
    MemSet((void*)0x559FEB, 0x90, 7);

    // DISABLE STATS MESSAGES
    MemSet((void*)0x55B980, 0xC3, 1);

    MemSet((void*)0x559760, 0xC3, 1);

    // ALLOW more than 8 players (crash with more if this isn't done)
    // 0060D64D   90               NOP
    // 0060D64E   E9 9C000000      JMP gta_sa.0060D6EF
    MemPut<BYTE>(0x60D64D, 0x90);
    MemPut<BYTE>(0x60D64E, 0xE9);

    // PREVENT CJ smoking and drinking like an addict
    // 005FBA26   EB 29            JMP SHORT gta_sa.005FBA51
    MemPut<BYTE>(0x5FBA26, 0xEB);

    // PREVENT the camera from messing up for drivebys for vehicle drivers
    MemPut<BYTE>(0x522423, 0x90);
    MemPut<BYTE>(0x522424, 0x90);

    // ALLOW ALT+TABBING WITHOUT PAUSING
    MemSet((void*)0x748A8D, 0x90, 6);

    // CENTER VEHICLE NAME and ZONE NAME messages
    // 0058B0AD   6A 02            PUSH 2 // orientation
    // VEHICLE
    MemPut<BYTE>(0x58B0AE, 0x00);

    // ZONE
    MemPut<BYTE>(0x58AD56, 0x00);

    // 85953C needs to equal 320.0 to center the text (640.0 being the base width)
    MemPut<float>(0x85953C, 320.0f);

    // 0058B147   D80D 0C958500    FMUL DWORD PTR DS:[85950C] // the text needs to be moved to the left
    // VEHICLE
    MemPut<BYTE>(0x58B149, 0x3C);

    // ZONE
    MemPut<BYTE>(0x58AE52, 0x3C);

    // DISABLE SAM SITES
    MemPut<BYTE>(0x5A07D0, 0xC3);

    // DISABLE TRAINS (AUTO GENERATED ONES)
    MemPut<BYTE>(0x6F7900, 0xC3);

    // Prevent TRAINS spawning with PEDs
    MemPut<BYTE>(0x6F7865, 0xEB);
    MemPut<BYTE>(0x6F8E7B, 0xE9);
    MemPut<DWORD>(0x6F8E7C, 0x109);            // jmp to 0x6F8F89
    MemPut<BYTE>(0x6F8E80, 0x90);

    // DISABLE PLANES
    MemPut<BYTE>(0x6CD2F0, 0xC3);

    // DISABLE EMERGENCY VEHICLES
    MemPut<BYTE>(0x42B7D0, 0xC3);

    // DISABLE CAR GENERATORS
    MemPut<BYTE>(0x6F3F40, 0xC3);

    // DISABLE CEntryExitManager::Update (they crash when you enter anyway)
    MemPut<BYTE>(0x440D10, 0xC3);

    // Disable MENU AFTER alt + tab
    // 0053BC72   C605 7B67BA00 01 MOV BYTE PTR DS:[BA677B],1
    MemPut<BYTE>(0x53BC78, 0x00);

    // DISABLE HUNGER MESSAGES
    MemSet((LPVOID)0x56E740, 0x90, 5);

    // DISABLE RANDOM VEHICLE UPGRADES
    MemSet((LPVOID)0x6B0BC2, 0xEB, 1);

    // DISABLE CPOPULATION::UPDATE - DOES NOT prevent vehicles - only on-foot peds
    // MemPut < BYTE > ( 0x616650, 0xC3 );    Problem - Stops streetlamps being turned into collidable objects when streamed in
    // This sets the 'Replay Is Playing' flag
    // MemPutFast < BYTE > ( 0xA43088, 1 );   Problem - Stops streetlamps being turned into collidable objects when streamed in

    // SORT OF HACK to make peds always walk around, even when in free-camera mode (in the editor)
    MemPut<BYTE>(0x53C017, 0x90);
    MemPut<BYTE>(0x53C018, 0x90);

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
    MemPut<BYTE>(0x6F2089, 0x58);
    MemSet((void*)0x6F208A, 0x90, 4);

    // Prevent the game deleting _any_ far away vehicles - will cause issues for population vehicles in the future
    MemPut<BYTE>(0x42CD10, 0xC3);

    // DISABLE weapon pickups
    MemPut<BYTE>(0x5B47B0, 0xC3);

    /*
    MemPut < BYTE > ( 0x469F00, 0xC3 );
    */

    // CCAM::PROCESSFIXED remover
    /*
        MemPut < BYTE > ( 0x51D470, 0xC2 );
        MemPut < BYTE > ( 0x51D471, 0x10 );
        MemPut < BYTE > ( 0x51D472, 0x00 );
    */

    // InitShotsyncHooks();

    // DISABLE CPad::ReconcileTwoControllersInput
    MemPut<BYTE>(0x53F530, 0xC2);
    MemPut<BYTE>(0x53F531, 0x0C);
    MemPut<BYTE>(0x53F532, 0x00);

    MemPut<BYTE>(0x53EF80, 0xC3);

    MemPut<BYTE>(0x541DDC, 0xEB);
    MemPut<BYTE>(0x541DDD, 0x60);

    // DISABLE CWanted Helis (always return 0 from CWanted::NumOfHelisRequired)
    MemPut<BYTE>(0x561FA4, 0x90);
    MemPut<BYTE>(0x561FA5, 0x90);

    // DISABLE  CWanted__UpdateEachFrame
    MemSet((void*)0x53BFF6, 0x90, 5);

    // DISABLE CWanted__Update
    MemSet((void*)0x60EBCC, 0x90, 5);

    // Disable armour-increase upon entering an enforcer
    MemPut<BYTE>(0x6D189B, 0x06);

    // Removes the last weapon pickups from interiors as well
    MemPut<BYTE>(0x591F90, 0xC3);

    // Trains may infact go further than Los Santos
    MemPut<BYTE>(0x4418E0, 0xC3);

    // EXPERIMENTAL - disable unloading of cols
    // MemSet ( (void*)0x4C4EDA, 0x90, 10 );

    // Make CTaskComplexSunbathe::CanSunbathe always return true
    MemPut<BYTE>(0x632140, 0xB0);
    MemPut<BYTE>(0x632141, 0x01);
    MemPut<BYTE>(0x632142, 0xC3);

    // Stop CTaskSimpleCarDrive::ProcessPed from exiting passengers with CTaskComplexSequence (some timer check)
    MemPut<BYTE>(0x644C18, 0x90);
    MemPut<BYTE>(0x644C19, 0xE9);

    // Stop CPlayerPed::ProcessControl from calling CVisibilityPlugins::SetClumpAlpha
    MemSet((void*)0x5E8E84, 0x90, 5);

    // Stop CVehicle::UpdateClumpAlpha from calling CVisibilityPlugins::SetClumpAlpha
    MemSet((void*)0x6D29CB, 0x90, 5);

    // Disable CVehicle::DoDriveByShootings
    MemSet((void*)0x741FD0, 0x90, 3);
    MemPut<BYTE>(0x741FD0, 0xC3);

    // Disable CTaskSimplePlayerOnFoot::PlayIdleAnimations (ret 4)
    MemPut<BYTE>(0x6872C0, 0xC2);
    MemPut<BYTE>(0x6872C1, 0x04);
    MemPut<BYTE>(0x6872C2, 0x00);

    /*
    // Disable forcing of ped animations to the player one in CPlayerPed::ProcessAnimGroups
    MemSet ( (LPVOID)0x609A44, 0x90, 21 );
    */

    // Let us sprint everywhere (always return 0 from CSurfaceData::isSprint)
    MemPut<DWORD>(0x55E870, 0xC2C03366);
    MemPut<WORD>(0x55E874, 0x0004);

    // Create pickup objects in interior 0 instead of 13
    MemPut<BYTE>(0x59FAA3, 0x00);

    // Don't get shotguns from police cars
    MemPut<BYTE>(0x6D19CD, 0xEB);

    // Don't get golf clubs from caddies
    MemPut<BYTE>(0x6D1A1A, 0xEB);

    // Don't get 20 health from ambulances
    MemPut<BYTE>(0x6D1762, 0x00);

    // Prevent CVehicle::RecalcTrainRailPosition from changing train speed
    MemSet((void*)0x6F701D, 0x90, 6);
    MemPut<BYTE>(0x6F7069, 0xEB);

    // The instanthit function for bullets ignores the first few bullets shot by
    // remote players after reloading because some flag isn't set (no bullet impact
    // graphics, no damage). Makes e.g. sawnoffs completely ineffective.
    // Remove this check so that no bullets are ignored.
    MemPut<BYTE>(0x73FDF9, 0xEB);

    // Allow turning on vehicle lights even if the engine is off
    MemSet((void*)0x6E1DBC, 0x90, 8);

    // Fix vehicle back lights both using light state 3 (SA bug)
    MemPut<BYTE>(0x6E1D4F, 2);

    // Fix for sliding over objects and vehicles (ice floor)
    MemPut<BYTE>(0x5E1E72, 0xE9);
    MemPut<BYTE>(0x5E1E73, 0xB9);
    MemPut<BYTE>(0x5E1E74, 0x00);
    MemPut<BYTE>(0x5E1E77, 0x90);

    // Avoid GTA setting vehicle first color to white after changing the paintjob
    MemSet((void*)0x6D65C5, 0x90, 11);

    // Disable GTA vehicle detachment at rotation awkwardness
    MemPut<BYTE>(0x547441, 0xE9);
    MemPut<BYTE>(0x547442, 0xFA);
    MemPut<BYTE>(0x547443, 0x02);
    MemPut<BYTE>(0x547444, 0x00);
    MemPut<BYTE>(0x547445, 0x00);

    // Disable idle cam
    MemPut<BYTE>(0x522C80, 0xC3);

    // Ignore camera fade state in rendering routine
    MemSet((void*)0x53E9C6, 0x90, 6);

    // Disable radar map hiding when pressing TAB (action key) while on foot
    MemSet((void*)0x58FC3E, 0x90, 14);

    // No intro movies kthx
    MemPut<DWORD>(0x748EF8, 0x748AE7);
    MemPut<DWORD>(0x748EFC, 0x748B08);
    MemPut<BYTE>(0x748B0E, 5);

    // Force triggering of the damage event for players on fire
    MemSet((void*)0x633695, 0x90, 6);
    MemPut<BYTE>(0x633720, 0);

    // Make CCreepingFire::TryToStartFireAtCoors return the fire pointer rather than a bool
    MemPut<BYTE>(0x53A459, 0x33);
    MemPut<BYTE>(0x53A568, 0x8B);
    MemPut<BYTE>(0x53A4A9, 0x33);
    MemPut<WORD>(0x53A55F, 0x9090);
    MemPut<BYTE>(0x73EC06, 0x85);

    // Do not fixate camera behind spectated player if local player is dead
    MemPut<BYTE>(0x52A2BB, 0);
    MemPut<BYTE>(0x52A4F8, 0);

    // Disable setting players on fire when they're riding burning bmx's (see #4573)
    MemPut<BYTE>(0x53A982, 0xEB);

    // Disable stealth-kill aiming (holding knife up)
    MemSet((void*)0x685DFB, 0x90, 5);
    MemPut<BYTE>(0x685DFB, 0x33);
    MemPut<BYTE>(0x685DFC, 0xC0);
    MemSet((void*)0x685C3E, 0x90, 5);
    MemPut<BYTE>(0x685C3E, 0x33);
    MemPut<BYTE>(0x685C3F, 0xC0);
    MemSet((void*)0x685DC4, 0x90, 5);
    MemPut<BYTE>(0x685DC4, 0x33);
    MemPut<BYTE>(0x685DC5, 0xC0);
    MemSet((void*)0x685DE6, 0x90, 5);
    MemPut<BYTE>(0x685DE6, 0x33);
    MemPut<BYTE>(0x685DE7, 0xC0);

    // #4937, Disable stealth-kill rotation in CTaskSimpleStealthKill::ProcessPed
    // Used to face the dying ped away from the killer.
    MemSet((void*)0x62E63F, 0x90, 6);
    MemPut<BYTE>(0x62E63F, 0xDD);
    MemPut<BYTE>(0x62E640, 0xD8);
    MemSet((void*)0x62E659, 0x90, 6);
    MemPut<BYTE>(0x62E659, 0xDD);
    MemPut<BYTE>(0x62E65A, 0xD8);
    MemSet((void*)0x62E692, 0x90, 6);
    MemPut<BYTE>(0x62E692, 0xDD);
    MemPut<BYTE>(0x62E693, 0xD8);

    // Allow all screen aspect ratios
    MemPut<WORD>(0x745BC9, 0x9090);

    // Allow all screen aspect ratios in multi-monitor dialog
    MemPut<WORD>(0x7459E1, 0x9090);

    // Show the GTA:SA Main menu, this fixes some issues (#4374 and MAYBE #4000).
    // We are hiding the menu in "void CGameSA::Initialize ( void )".
    //
    // - Sebas
    MemPutFast<BYTE>((0xBA6748) + 0x5C, 1);

    // Force the MrWhoopee music to load even if we are not the driver.
    MemPut<BYTE>(0x4F9CCE, 0xCE);

    // Disable re-initialization of DirectInput mouse device by the game
    MemPut<BYTE>(0x576CCC, 0xEB);
    MemPut<BYTE>(0x576EBA, 0xEB);
    MemPut<BYTE>(0x576F8A, 0xEB);

    // Make sure DirectInput mouse device is set non-exclusive (may not be needed?)
    MemPut<DWORD>(0x7469A0, 0x909000B0);

    // Remove 14ms wait (Was done every other frame for some reason)
    MemPut<BYTE>(0x53E94C, 0x00);

    // Disable the GTASA main menu.
    MemSet((void*)0x57BA57, 0x90, 6);

    // Disable the loading screen tune.
    MemSet((void*)0x748CF6, 0x90, 5);
	
    // Do not render the loading screen.
    MemSet((void*)0x590D7C, 0x90, 5);
    MemSet((void*)0x590DB3, 0x90, 5);
    MemCpy((void*)0x590D9F, "\xC3\x90\x90\x90\x90", 5);

    // Disable ped to player conversations.
    MemSet((void*)0x53C127, 0x90, 10);

#if 0
    // Mute peds (would break setPedVoice).
    MemCpy ( (void *)0x5EFFE0, "\xC2\x18\x00\x90", 4 );
#endif

    // Clip camera also outside the world bounds.
    MemSet((void*)0x41AD12, 0x90, 2);
    MemSet((void*)0x41AD5E, 0x90, 2);
    MemSet((void*)0x41ADA7, 0x90, 2);
    MemSet((void*)0x41ADF3, 0x90, 2);

    // Fix melee doesn't work outside the world bounds.
    MemSet((void*)0x5FFAEE, 0x90, 2);
    MemSet((void*)0x5FFB4B, 0x90, 2);
    MemSet((void*)0x5FFBA2, 0x90, 5);
    MemSet((void*)0x5FFC00, 0x90, 5);

    // Fix shooting sniper doesn't work outside the world bounds (for local player).
    MemSet((void*)0x7361BF, 0x90, 6);
    MemSet((void*)0x7361D4, 0x90, 6);
    MemSet((void*)0x7361E9, 0x90, 6);
    MemSet((void*)0x7361FE, 0x90, 6);

    // Fix heli blades lacks collision outside the world bounds.
    MemSet((void*)0x6E2FBC, 0x90, 2);
    MemSet((void*)0x6E301C, 0x90, 2);
    MemSet((void*)0x6E3075, 0x90, 2);
    MemSet((void*)0x6E30D6, 0x90, 2);

    // Allow Player Garages to shut with players inside.
    MemSet((void*)0x44C6FA, 0x90, 4);

    // Stop the loading of ambient traffic models and textures
    // by skipping CStreaming::StreamVehiclesAndPeds() and CStreaming::StreamZoneModels()
    MemPut<BYTE>(0x40E7DF, 0xEB);

    // Disable CPopulation::ManagePed
    MemPut<BYTE>(0x611FC0, 0xC3);
    // Stop CPopulation::Update after ManagePopulation call
    MemPut<BYTE>(0x616698, 0x5E);
    MemPut<BYTE>(0x616699, 0xC3);

    // Disable CReplay::Update
    MemPut<BYTE>(0x460500, 0xC3);
    // Disable CInterestingEvents::ScanForNearbyEntities
    MemPut<BYTE>(0x605A30, 0xC3);
    // Disable CGangWars::Update
    MemPut<BYTE>(0x446610, 0xC3);
    // Disable CConversations::Update
    MemPut<BYTE>(0x43C590, 0xC3);
    // Disable CPedToPlayerConversations::Update
    MemPut<BYTE>(0x43B0F0, 0xC3);
    // Disable CCarCtrl::RemoveCarsIfThePoolGetsFull
    MemPut<BYTE>(0x4322B0, 0xC3);
    // Disable CStreaming::StreamVehiclesAndPeds_Always
    MemPut<BYTE>(0x40B650, 0xC3);

    UpdateVehicleSuspension();

    // Aircraft Max Height checks are at 0x6D2614 and 0x6D2625 edit the check to use our own float.
    MemPut(0x6D2614, &m_fAircraftMaxHeight);
    MemPut(0x6D2625, &m_fAircraftMaxHeight);

    // Aircraft Max Velocity
    MemPut(0x6DADDF, &m_fAircraftMaxVelocity_Sq);
    MemPut(0x6DADEF, &m_fAircraftMaxVelocity);
    MemPut(0x6DADF8, &m_fAircraftMaxVelocity);
    MemPut(0x6DAE01, &m_fAircraftMaxVelocity);

    // Disable calls to CFireManager::ExtinguishPoint and CWorld::ExtinguishAllCarFiresInArea
    // from CWorld::ClearExcitingStuffFromArea
    MemSet((void*)0x56A404, 0x90, 0x56A446 - 0x56A404);

    // Disable setting the occupied's vehicles health to 75.0f when a burning ped enters it
    // in CFire::ProcessFire
    MemSet((void*)0x53A651, 0x90, 0xA);

    // Prevent money change (+12$) when entering a taxi/cabbie (fix for #8332)
    MemSet((void*)0x6D1741, 0x90, 0x6D175F - 0x6D1741);

    // Increase intensity of vehicle tail light corona
    MemPut<BYTE>(0x6E1A22, 0xF0);

    // Do not change visibility flag for water areas above level 950 (fix for #9159)
    // do it only for Madd Dogg's mansion pool instead
    MemPut(0x6E5869, &m_fMaddDoggPoolLevel);
    MemPut(0x6E58BD, &m_fMaddDoggPoolLevel);
    MemPut(0x6E594B, &m_fMaddDoggPoolLevel);
    MemPut(0x6E5995, &m_fMaddDoggPoolLevel);

    MemCpy((void*)0x6E5871, "\x40\x74", 2);
    MemCpy((void*)0x6E58C5, "\x40\x74", 2);
    MemCpy((void*)0x6E5951, "\x40\x74", 2);
    MemCpy((void*)0x6E599D, "\x40\x74", 2);

    // Skip vehicle type check in CVehicle::SetupRender & CVehicle::ResetAfterRender (fix for #8158)
    MemSet((void*)0x6D6517, 0x90, 2);
    MemSet((void*)0x6D0E43, 0x90, 2);

    // Fix killing ped during car jacking (#4319)
    // by using CTaskComplexLeaveCar instead of CTaskComplexLeaveCarAndDie
    MemPut<BYTE>(0x63F576, 0xEB);

    // Disable vehicle audio driver logic so MTA can reimplement it (#9681)
    // Disable updating m_bPlayerDriver in CAEVehicleAudioEntity::Service
    MemSetFast((void*)0x5023B2, 0x90, 6);
    // Disable call to CAEVehicleAudioEntity::JustGotInVehicleAsDriver
    MemSetFast((void*)0x5023E1, 0x90, 5);
    // Disable call to CAEVehicleAudioEntity::JustGotOutOfVehicleAsDriver
    MemSetFast((void*)0x502341, 0x90, 5);

    // Allow to switch weapons while glued
    MemSetFast((void*)0x60D861, 0x90, 14);

    // Allow water cannon to hit objects and players visually
    MemSet((void*)0x72925D, 0x1, 1);            // objects
    MemSet((void*)0x729263, 0x1, 1);            // players

    // Allow crouching with 1HP
    MemPut((void*)0x6943AD, &fDuckingHealthThreshold);
    fDuckingHealthThreshold = 0;

    // Lower the GTA shadows offset closer to ground/floor level
    m_fShadowsOffset = DEFAULT_SHADOWS_OFFSET;
    for (auto uiAddr : shadowAddr)
        MemPut(uiAddr, &m_fShadowsOffset);

    // Fix corona rain reflections aren't rendering (#2345)
    // By using zBufferFar instead of zBufferNear for corona position
    MemPut<BYTE>(0x6FB9A0, 0x1C);

    // Skip check for disabled HUD
    MemSet((void*)0x58FBC4, 0x90, 9);

    // Show muzzle flash for last bullet in magazine
    MemSet((void*)0x61ECD2, 0x90, 20);

    // Disable camera photos creation since we reimplement it (to have better quality)
    MemSet((void*)0x705331, 0x90, 0x7053AF - 0x705331);

    // Fix ped real time shadows by processing them always like for a non player ped
    // Change JZ to JMP instruction in CRealTimeShadowManager::GetRealTimeShadow()
    MemPut<BYTE>(0x7069F5, 0xEB);

    // Modify CRealTimeShadowManager::GetRealTimeShadow()
    // Start iterating over shadow array from 0 instead 1 (so we will have max 16 shadows, not 15)
    // Originally, first shadow from the shadow array is used for player ped only
    // Array: CRealTimeShadowManager::pShadowData[16]
    // Array size: 0x40 (16 elements)
    // Array element size: 0x04
    MemPut<BYTE>(0x7069FE, 0x08);

    // Fix ped real time shadows do not render on some objects
    // by skipping some entity flag check in CShadows::CastRealTimeShadowSectorList()
    MemSet((void*)0x70A83B, 0x90, 6);

    // Fix vehicle blob shadows and light textures do not render on some objects when vehicle is empty
    // by skipping some entity flag check in CShadows::CastPlayerShadowSectorList()
    MemSet((void*)0x70A4CB, 0x90, 6);

    // Allow vertical camera movement during a camera fade (#411)
    MemPut<BYTE>(0x524084, 0xFF);
    MemPut<BYTE>(0x524089, 0xFF);

    // Allow change alpha for arrow & checkpoint markers (#1860)
    MemSet((void*)0x7225F5, 0x90, 4);
    MemCpy((void*)0x725DDE, "\xFF\x76\xB\x90\x90", 5);

    // Allow switch weapon during jetpack task (#3569)
    MemSetFast((void*)0x60D86F, 0x90, 19);

    // Fix invisible vehicle windows when lights are on (#2936)
    MemPut<BYTE>(0x6E1425, 1);

    // Allow alpha change for helicopter rotor (#523)
    MemSet((void*)0x6C444B, 0x90, 6);
    MemSet((void*)0x6C4453, 0x90, 0x68);
    
    InitHooks_CrashFixHacks();

    // Init our 1.3 hooks.
    Init_13();
    InitHooks_LicensePlate();
    InitHooks_Direct3D();
    InitHooks_FixLineOfSightArgs();
    InitHooks_VehicleDamage();
    InitHooks_VehicleLights();
    InitHooks_VehicleWeapons();

    InitHooks_Streaming();
    InitHooks_FrameRateFixes();
    InitHooks_ProjectileCollisionFix();
    InitHooks_ObjectStreamerOptimization();

    InitHooks_Postprocess();
}

// Used to store copied pointers for explosions in the FxSystem

std::list<DWORD*> Pointers_FxSystem;

void AddFxSystemPointer(DWORD* pPointer)
{
    Pointers_FxSystem.push_front(pPointer);
}

void RemoveFxSystemPointer(DWORD* pPointer)
{
    // Look through our list for the pointer
    std::list<DWORD*>::iterator iter = Pointers_FxSystem.begin();
    for (; iter != Pointers_FxSystem.end(); ++iter)
    {
        // It exists in our list?
        if (*iter == pPointer)
        {
            // Remove it from the list over our copied matrices
            Pointers_FxSystem.erase(iter);

            // Delete the pointer itself
            free(pPointer);
            return;
        }
    }
}

CRemoteDataStorage* CMultiplayerSA::CreateRemoteDataStorage()
{
    return new CRemoteDataStorageSA();
}

void CMultiplayerSA::DestroyRemoteDataStorage(CRemoteDataStorage* pData)
{
    delete (CRemoteDataStorageSA*)pData;
}

void CMultiplayerSA::AddRemoteDataStorage(CPlayerPed* pPed, CRemoteDataStorage* pData)
{
    CRemoteDataSA::AddRemoteDataStorage(pPed, pData);
}

CRemoteDataStorage* CMultiplayerSA::GetRemoteDataStorage(CPlayerPed* pPed)
{
    return CRemoteDataSA::GetRemoteDataStorage(pPed);
}

void CMultiplayerSA::RemoveRemoteDataStorage(CPlayerPed* pPed)
{
    CRemoteDataSA::RemoveRemoteDataStorage(pPed);
}

CPed* CMultiplayerSA::GetContextSwitchedPed()
{
    return pContextSwitchedPed;
}

void CMultiplayerSA::AllowWindowsCursorShowing(bool bAllow)
{
    /*
    0074821D   6A 00            PUSH 0
    0074821F   FF15 EC828500    CALL DWORD PTR DS:[<&USER32.ShowCursor>] ; USER32.ShowCursor
    00748225   6A 00            PUSH 0
    00748227   FF15 9C828500    CALL DWORD PTR DS:[<&USER32.SetCursor>]  ; USER32.SetCursor
    */
    BYTE originalCode[16] = {0x6A, 0x00, 0xFF, 0x15, 0xEC, 0x82, 0x85, 0x00, 0x6A, 0x00, 0xFF, 0x15, 0x9C, 0x82, 0x85, 0x00};

    if (bAllow)
    {
        MemSet((LPVOID)ADDR_CursorHiding, 0x90, 16);
    }
    else
    {
        MemCpy((LPVOID)ADDR_CursorHiding, &originalCode, 16);
    }
}

CShotSyncData* CMultiplayerSA::GetLocalShotSyncData()
{
    return GetLocalPedShotSyncData();
}

void CMultiplayerSA::DisablePadHandler(bool bDisabled)
{
    // DISABLE GAMEPADS (testing)
    if (bDisabled)
        MemPut<BYTE>(0x7449F0, 0xC3);
    else
        MemPut<BYTE>(0x7449F0, 0x8B);
}

void CMultiplayerSA::GetHeatHaze(SHeatHazeSettings& settings)
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

void CMultiplayerSA::ResetColorFilter()
{
    if (*(BYTE*)0x7036EC == 0xB8)
    {
        static BYTE DefaultBytes[5] = {0xC1, 0xE0, 0x08, 0x0B, 0xC1};            // shl     eax, 8
                                                                                 // or      eax, ecx
        MemCpy((void*)0x7036EC, DefaultBytes, sizeof(DefaultBytes));
        MemCpy((void*)0x70373D, DefaultBytes, sizeof(DefaultBytes));
    }
}

void CMultiplayerSA::SetColorFilter(DWORD dwPass0Color, DWORD dwPass1Color)
{
    const bool bEnabled = *(BYTE*)0x7036EC == 0xB8;

    // Update a pass0 color if needed
    if (!bEnabled || *(DWORD*)0x7036ED != dwPass0Color)
    {
        MemPut<BYTE>(0x7036EC, 0xB8);            // mov eax
        MemPut<DWORD>(0x7036ED, dwPass0Color);
    }

    // Update a pass1 color if needed
    if (!bEnabled || *(DWORD*)0x70373E != dwPass1Color)
    {
        MemPut<BYTE>(0x70373D, 0xB8);            // mov eax
        MemPut<DWORD>(0x70373E, dwPass1Color);
    }
}

void CMultiplayerSA::GetColorFilter(DWORD& dwPass0Color, DWORD& dwPass1Color, bool isOriginal)
{
    // GTASA PC has 2 color filters, one of them is static color filter, and another one is blended by time cycle
    bool bUseTimeCycle = *(BYTE*)0x7036EC == 0xC1;
    if (bUseTimeCycle || isOriginal){ //If we are using color filter from time cycle or we specified color filter from time cycle
        SColorRGBA pass0SColor(*(float*)0xB7C518, *(float*)0xB7C51C, *(float*)0xB7C520, *(float*)0xB7C524);
        SColorRGBA pass1SColor(*(float*)0xB7C528, *(float*)0xB7C52C, *(float*)0xB7C530, *(float*)0xB7C534);
        dwPass0Color = pass0SColor.ulARGB;
        dwPass1Color = pass1SColor.ulARGB;
    }
    else
    {
        dwPass0Color = *(DWORD*)0x7036ED;
        dwPass1Color = *(DWORD*)0x70373E;
    }
}

void DoSetHeatHazePokes(const SHeatHazeSettings& settings, int iHourStart, int iHourEnd, float fFadeSpeed, float fInsideBuildingFadeSpeed,
                        bool bAllowAutoTypeChange)
{
    MemPutFast<int>(0x8D50D4, iHourStart);
    MemPutFast<int>(0x8D50D8, iHourEnd);

    MemPutFast<float>(0x8D50DC, fFadeSpeed);
    MemPutFast<float>(0x8D50E0, fInsideBuildingFadeSpeed);

    MemPutFast<int>(0x8D50E8, settings.ucIntensity);
    MemPutFast<int>(0xC402C0, settings.ucRandomShift);
    MemPutFast<int>(0x8D50EC, settings.usSpeedMin);
    MemPutFast<int>(0x8D50F0, settings.usSpeedMax);
    MemPutFast<int>(0xC40304, settings.sScanSizeX);
    MemPutFast<int>(0xC40308, settings.sScanSizeY);
    MemPutFast<int>(0xC4030C, settings.usRenderSizeX);
    MemPutFast<int>(0xC40310, settings.usRenderSizeY);
    MemPutFast<bool>(0xC402BA, settings.bInsideBuilding);

    if (bAllowAutoTypeChange)
        MemPut<BYTE>(0x701455, 0x83);            // sub
    else
        MemPut<BYTE>(0x701455, 0xC3);            // retn
}

void CMultiplayerSA::SetHeatHaze(const SHeatHazeSettings& settings)
{
    if (settings.ucIntensity != 0)
        DoSetHeatHazePokes(settings, 0, 24, 1.0f, 1.0f, false);            // 24 hrs
    else
        DoSetHeatHazePokes(settings, 38, 39, 1.0f, 1.0f, false);            // 0 hrs

    m_bHeatHazeCustomized = true;
    ApplyHeatHazeEnabled();
}

void CMultiplayerSA::ResetHeatHaze()
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

    DoSetHeatHazePokes(settings, 10, 19, 0.05f, 1.0f, true);            // defaults

    m_bHeatHazeCustomized = false;
    ApplyHeatHazeEnabled();
}

void CMultiplayerSA::SetHeatHazeEnabled(bool bEnabled)
{
    m_bHeatHazeEnabled = bEnabled;
    ApplyHeatHazeEnabled();
}

void CMultiplayerSA::ApplyHeatHazeEnabled()
{
    // Enable heat haze if user allows it or scripts have customized it
    if (m_bHeatHazeEnabled || m_bHeatHazeCustomized)
        MemPut<BYTE>(0x701780, 0x83);
    else
        MemPut<BYTE>(0x701780, 0xC3);
}

void CMultiplayerSA::DisableAllVehicleWeapons(bool bDisable)
{
    if (bDisable)
        MemPut<BYTE>(0x6E3950, 0xC3);
    else
        MemPut<BYTE>(0x6E3950, 0x83);
}

void CMultiplayerSA::DisableBirds(bool bDisabled)
{
    if (bDisabled)
    {
        // return out of render and update to make sure birds already created aren't rendered at least.. also no point in calling render if there is nothing to
        // render I guess
        MemPut<BYTE>(0x712810, 0xC3);
        MemPut<BYTE>(0x712330, 0xC3);
    }
    else
    {
        // restore previous first bytes render and update to normal
        MemPut<BYTE>(0x712810, 0x64);
        MemPut<BYTE>(0x712330, 0xA1);
    }
}

void CMultiplayerSA::DisableQuickReload(bool bDisabled)
{
    if (bDisabled)
        MemPut<WORD>(0x60B4F6, 0x08EB);
    else
        MemPut<WORD>(0x60B4F6, 0x027C);
}
void CMultiplayerSA::DisableCloseRangeDamage(bool bDisabled)
{
    if (bDisabled)
    {
        // Change float comparison to 0.0f so SA doesn't use close range damage.
        MemPut<BYTE>(0x73B9FF, 0x50);
        MemPut<BYTE>(0x73BA00, 0x8B);
    }
    else
    {
        // Change float comparison to 1.0f so SA uses close range damage.
        MemPut<BYTE>(0x73B9FF, 0x24);
        MemPut<BYTE>(0x73BA00, 0x86);
    }
}
bool CMultiplayerSA::GetInteriorSoundsEnabled()
{
    return bInteriorSoundsEnabled;
}

void CMultiplayerSA::SetInteriorSoundsEnabled(bool bEnabled)
{
    // The function which should be restored when re-enabling interior sounds
    BYTE originalCode[6] = {0x89, 0x2d, 0xbc, 0xdc, 0xb6, 0x00};

    if (bEnabled)
    {
        // Restore the function responsible for interior sounds
        MemCpy((LPVOID)0x508450, &originalCode, 6);
        MemCpy((LPVOID)0x508817, &originalCode, 6);
    }
    else
    {
        // Nop the function responsible for interior sounds
        MemSet((LPVOID)0x508450, 0x90, 6);
        MemSet((LPVOID)0x508817, 0x90, 6);
    }

    // Toggle the interior sound on/off, depending on what the scripter wants
    MemPutFast<bool>(0xB6DCBC, bEnabled);

    // If we just store it, we can always return the on/off state later on
    bInteriorSoundsEnabled = bEnabled;
}

bool CMultiplayerSA::GetInteriorFurnitureEnabled(char cRoomId)
{
    assert(cRoomId >= 0 && cRoomId <= 4);
    return bInteriorFurnitureStates[cRoomId];
}

void CMultiplayerSA::SetInteriorFurnitureEnabled(char cRoomId, bool bEnabled)
{
    assert(cRoomId >= 0 && cRoomId <= 4);

    // 0 = Shop; 1 = Office; 2 = Lounge; 3 = Bedroom; 4 = Kitchen
    DWORD originalCodeAddresses[] = {0x593D00, 0x593D0E, 0x593D1C, 0x593D2A, 0x593D38};
    BYTE  originalCodes[][5] = {
        {0xE8, 0x8B, 0x6A, 0x0, 0x0}, {0xE8, 0xDD, 0x5D, 0x0, 0x0}, {0xE8, 0x1F, 0x3A, 0x0, 0x0}, {0xE8, 0x91, 0x2, 0x0, 0x0}, {0xE8, 0x73, 0x33, 0x0, 0x0}};

    if (bEnabled)
    {
        MemCpy((void*)originalCodeAddresses[cRoomId], &originalCodes[cRoomId], 5);

        if (cRoomId == 0)
            MemPut<BYTE>(0x593CFD, 0x50);
    }
    else
    {
        MemSet((void*)originalCodeAddresses[cRoomId], 0x90, 5);

        if (cRoomId == 0)
            MemPut<BYTE>(0x593CFD, 0x90);
    }

    bInteriorFurnitureStates[cRoomId] = bEnabled;
}

void CMultiplayerSA::SetWindVelocity(float fX, float fY, float fZ)
{
    // Disable
    MemPut<WORD>(0x72C616, 0xD8DD);
    MemPut<DWORD>(0x72C616 + 2, 0x90909090);
    MemPut<WORD>(0x72C622, 0xD8DD);
    MemPut<DWORD>(0x72C622 + 2, 0x90909090);
    MemPut<WORD>(0x72C636, 0xD8DD);
    MemPut<DWORD>(0x72C636 + 2, 0x90909090);

    MemPut<WORD>(0x72C40C, 0xD8DD);
    MemPut<DWORD>(0x72C40C + 2, 0x90909090);
    MemPut<WORD>(0x72C417, 0xD8DD);
    MemPut<DWORD>(0x72C417 + 2, 0x90909090);
    MemPut<WORD>(0x72C4EF, 0xD8DD);
    MemPut<DWORD>(0x72C4EF + 2, 0x90909090);

    // Set
    MemPutFast<float>(0xC813E0, fX);
    MemPutFast<float>(0xC813E4, fY);
    MemPutFast<float>(0xC813E8, fZ);
}

void CMultiplayerSA::GetWindVelocity(float& fX, float& fY, float& fZ)
{
    fX = *(float*)0xC813E0;
    fY = *(float*)0xC813E4;
    fZ = *(float*)0xC813E8;
}

void CMultiplayerSA::RestoreWindVelocity()
{
    MemPut<WORD>(0x72C616, 0x1DD9);
    MemPut<DWORD>(0x72C616 + 2, 0x00C813E0);
    MemPut<WORD>(0x72C622, 0x1DD9);
    MemPut<DWORD>(0x72C622 + 2, 0x00C813E4);
    MemPut<WORD>(0x72C636, 0x1DD9);
    MemPut<DWORD>(0x72C636 + 2, 0x00C813E8);

    MemPut<WORD>(0x72C40C, 0x15D9);
    MemPut<DWORD>(0x72C40C + 2, 0x00C813E0);
    MemPut<WORD>(0x72C417, 0x1DD9);
    MemPut<DWORD>(0x72C417 + 2, 0x00C813E4);
    MemPut<WORD>(0x72C4EF, 0x1DD9);
    MemPut<DWORD>(0x72C4EF + 2, 0x00C813E8);
}

float CMultiplayerSA::GetFarClipDistance()
{
    return *(float*)0xB7C4F0;
}

void CMultiplayerSA::SetFarClipDistance(float fDistance)
{
    BYTE newFstp[3] = {0xDD, 0xD8, 0x90};
    if (*(BYTE*)0x55FCC8 != newFstp[0])
    {
        MemCpy((LPVOID)0x55FCC8, &newFstp, 3);
        MemCpy((LPVOID)0x5613A3, &newFstp, 3);
        MemCpy((LPVOID)0x560A23, &newFstp, 3);
        MemCpy((LPVOID)0x560D3B, &newFstp, 3);
        MemCpy((LPVOID)0x560EDD, &newFstp, 3);
        MemCpy((LPVOID)0x560F18, &newFstp, 3);
    }

    MemPutFast<float>(0xB7C4F0, fDistance);
}

void CMultiplayerSA::RestoreFarClipDistance()
{
    BYTE originalFstp[3] = {0xD9, 0x5E, 0x50};
    if (*(BYTE*)0x55FCC8 != originalFstp[0])
    {
        MemCpy((LPVOID)0x55FCC8, &originalFstp, 3);
        MemCpy((LPVOID)0x5613A3, &originalFstp, 3);
        MemCpy((LPVOID)0x560A23, &originalFstp, 3);
        MemCpy((LPVOID)0x560D3B, &originalFstp, 3);
        MemCpy((LPVOID)0x560EDD, &originalFstp, 3);
        MemCpy((LPVOID)0x560F18, &originalFstp, 3);
    }
}

float CMultiplayerSA::GetNearClipDistance()
{
    return m_fNearClipDistance;
}

void CMultiplayerSA::SetNearClipDistance(float fDistance)
{
    m_fNearClipDistance = Clamp(0.1f, fDistance, 20.f);
}

void CMultiplayerSA::RestoreNearClipDistance()
{
    m_fNearClipDistance = DEFAULT_NEAR_CLIP_DISTANCE;
}

float CMultiplayerSA::GetFogDistance()
{
    return *(float*)0xB7C4F4;
}

void CMultiplayerSA::SetFogDistance(float fDistance)
{
    BYTE newFstp[3] = {0xDD, 0xD8, 0x90};
    if (*(BYTE*)0x55FCDB != newFstp[0])
    {
        MemCpy((LPVOID)0x55FCDB, &newFstp, 3);
        MemCpy((LPVOID)0x560D60, &newFstp, 3);
    }

    MemPutFast<float>(0xB7C4F4, fDistance);
}

void CMultiplayerSA::RestoreFogDistance()
{
    BYTE originalFstp[3] = {0xD9, 0x5E, 0x54};
    if (*(BYTE*)0x55FCDB != originalFstp[0])
    {
        MemCpy((LPVOID)0x55FCDB, &originalFstp, 3);
        MemCpy((LPVOID)0x560D60, &originalFstp, 3);
    }
}

void CMultiplayerSA::GetSunColor(unsigned char& ucCoreRed, unsigned char& ucCoreGreen, unsigned char& ucCoreBlue, unsigned char& ucCoronaRed,
                                 unsigned char& ucCoronaGreen, unsigned char& ucCoronaBlue)
{
    ucCoreRed = *(BYTE*)0xB7C4D0;
    ucCoreGreen = *(BYTE*)0xB7C4D2;
    ucCoreBlue = *(BYTE*)0xB7C4D4;

    ucCoronaRed = *(BYTE*)0xB7C4D6;
    ucCoronaGreen = *(BYTE*)0xB7C4D8;
    ucCoronaBlue = *(BYTE*)0xB7C4DA;
}

void CMultiplayerSA::SetSunColor(unsigned char ucCoreRed, unsigned char ucCoreGreen, unsigned char ucCoreBlue, unsigned char ucCoronaRed,
                                 unsigned char ucCoronaGreen, unsigned char ucCoronaBlue)
{
    MemSet((LPVOID)0x55F9B2, 0x90, 4);
    MemSet((LPVOID)0x55F9DD, 0x90, 4);
    MemSet((LPVOID)0x55FA08, 0x90, 4);
    MemSet((LPVOID)0x55FA33, 0x90, 4);
    MemSet((LPVOID)0x55FA5E, 0x90, 4);
    MemSet((LPVOID)0x55FA8D, 0x90, 4);

    MemPutFast<BYTE>(0xB7C4D0, ucCoreRed);
    MemPutFast<BYTE>(0xB7C4D2, ucCoreGreen);
    MemPutFast<BYTE>(0xB7C4D4, ucCoreBlue);

    MemPutFast<BYTE>(0xB7C4D6, ucCoronaRed);
    MemPutFast<BYTE>(0xB7C4D8, ucCoronaGreen);
    MemPutFast<BYTE>(0xB7C4DA, ucCoronaBlue);
}

void CMultiplayerSA::ResetSunColor()
{
    BYTE originalMov[3] = {0x66, 0x89, 0x46};

    MemCpy((LPVOID)0x55F9B2, &originalMov, 3);
    MemPut<BYTE>(0x55F9B5, 0x30);
    MemCpy((LPVOID)0x55F9DD, &originalMov, 3);
    MemPut<BYTE>(0x55F9E0, 0x32);
    MemCpy((LPVOID)0x55FA08, &originalMov, 3);
    MemPut<BYTE>(0x55FA0B, 0x34);

    MemCpy((LPVOID)0x55FA33, &originalMov, 3);
    MemPut<BYTE>(0x55FA36, 0x36);
    MemCpy((LPVOID)0x55FA5E, &originalMov, 3);
    MemPut<BYTE>(0x55FA61, 0x38);
    MemCpy((LPVOID)0x55FA8D, &originalMov, 3);
    MemPut<BYTE>(0x55FA90, 0x3A);
}

float CMultiplayerSA::GetSunSize()
{
    return *(float*)0xB7C4DC / 10;
}

void CMultiplayerSA::SetSunSize(float fSize)
{
    MemPut<BYTE>(0x55FA9D, 0xDD);
    MemPut<BYTE>(0x55FA9E, 0xD8);
    MemPut<BYTE>(0x55FA9F, 0x90);

    MemPutFast<float>(0xB7C4DC, fSize * 10);
}

void CMultiplayerSA::ResetSunSize()
{
    MemPut<BYTE>(0x55FA9D, 0xD9);
    MemPut<BYTE>(0x55FA9E, 0x5E);
    MemPut<BYTE>(0x55FA9F, 0x3C);
}

void CMultiplayerSA::SetCloudsEnabled(bool bEnabled)
{
    // volumetric clouds
    if (bEnabled)
        MemPut<BYTE>(0x716380, 0xA1);
    else
        MemPut<BYTE>(0x716380, 0xC3);

    // bottom clouds
    if (bEnabled)
        MemPut<BYTE>(0x7154B0, 0x83);
    else
        MemPut<BYTE>(0x7154B0, 0xC3);

    // skyline clouds
    if (bEnabled)
    {
        MemPut<BYTE>(0x71411A, 0xD9);
        MemPut<BYTE>(0x71411B, 0x41);
        MemPut<BYTE>(0x71411C, 0x08);
    }
    else
    {
        MemSet((void*)0x71411A, 0x90, 3);
    }

    // plane trails (not really clouds, but they're sort of vapour)

    if (bEnabled)
    {
        MemPut<BYTE>(0x717180, 0x83);
        MemPut<BYTE>(0x717181, 0xEC);
        MemPut<BYTE>(0x717182, 0x08);
    }
    else
    {
        MemPut<BYTE>(0x717180, 0xC2);
        MemPut<BYTE>(0x717181, 0x04);
        MemPut<BYTE>(0x717182, 0x00);
    }
}

bool CMultiplayerSA::HasSkyColor()
{
    return bUsingCustomSkyGradient;
}

void CMultiplayerSA::GetSkyColor(unsigned char& TopRed, unsigned char& TopGreen, unsigned char& TopBlue, unsigned char& BottomRed, unsigned char& BottomGreen,
                                 unsigned char& BottomBlue)
{
    if (HasSkyColor())
    {
        TopRed = ucSkyGradientTopR;
        TopGreen = ucSkyGradientTopG;
        TopBlue = ucSkyGradientTopB;

        BottomRed = ucSkyGradientBottomR;
        BottomGreen = ucSkyGradientBottomG;
        BottomBlue = ucSkyGradientBottomB;
    }
    else
    {
        TopRed = *(BYTE*)0xB7C4C4;
        TopGreen = *(BYTE*)0xB7C4C6;
        TopBlue = *(BYTE*)0xB7C4C8;

        BottomRed = *(BYTE*)0xB7C4CA;
        BottomGreen = *(BYTE*)0xB7C4CC;
        BottomBlue = *(BYTE*)0xB7C4CE;
    }
}

void CMultiplayerSA::SetSkyColor(unsigned char TopRed, unsigned char TopGreen, unsigned char TopBlue, unsigned char BottomRed, unsigned char BottomGreen,
                                 unsigned char BottomBlue)
{
    bUsingCustomSkyGradient = true;
    ucSkyGradientTopR = TopRed;
    ucSkyGradientTopG = TopGreen;
    ucSkyGradientTopB = TopBlue;
    ucSkyGradientBottomR = BottomRed;
    ucSkyGradientBottomG = BottomGreen;
    ucSkyGradientBottomB = BottomBlue;
}

void CMultiplayerSA::ResetSky()
{
    bUsingCustomSkyGradient = false;
}

void CMultiplayerSA::SetMoonSize(int iSize)
{
    MemPutFast<BYTE>(0x8D4B60, iSize);
}

int CMultiplayerSA::GetMoonSize()
{
    return *(BYTE*)0x8D4B60;
}

void CMultiplayerSA::ResetMoonSize()
{
    // Set default moon size 3
    SetMoonSize(3);
}

bool CMultiplayerSA::HasWaterColor()
{
    return bUsingCustomWaterColor;
}

void CMultiplayerSA::GetWaterColor(float& fWaterRed, float& fWaterGreen, float& fWaterBlue, float& fWaterAlpha)
{
    if (HasWaterColor())
    {
        fWaterRed = fWaterColorR;
        fWaterGreen = fWaterColorG;
        fWaterBlue = fWaterColorB;
        fWaterAlpha = fWaterColorA;
    }
    else
    {
        fWaterRed = *(float*)0xB7C508;
        fWaterGreen = *(float*)0xB7C50C;
        fWaterBlue = *(float*)0xB7C510;
        fWaterAlpha = *(float*)0xB7C514;
    }
}

void CMultiplayerSA::SetWaterColor(float fWaterRed, float fWaterGreen, float fWaterBlue, float fWaterAlpha)
{
    bUsingCustomWaterColor = true;
    // Water surface
    fWaterColorR = fWaterRed;
    fWaterColorG = fWaterGreen;
    fWaterColorB = fWaterBlue;
    fWaterColorA = fWaterAlpha;
    // Underwater
    MemPutFast<BYTE>(0x8D5140, (BYTE)fWaterRed);
    MemPutFast<BYTE>(0x8D5141, (BYTE)fWaterGreen);
    MemPutFast<BYTE>(0x8D5142, (BYTE)fWaterBlue);
    MemPutFast<BYTE>(0x8D5143, (BYTE)fWaterAlpha);
    MemPut<BYTE>(0x7051A7, 255 - (BYTE)fWaterAlpha);
    MemPut<float>(0x872660, 255 - fWaterAlpha);
    MemPut<BYTE>(0x7051D7, 255 - (BYTE)fWaterAlpha);
}

void CMultiplayerSA::ResetWater()
{
    bUsingCustomWaterColor = false;
    MemPutFast<DWORD>(0x8D5140, 0x40404040);
    MemPut<BYTE>(0x7051A7, 184);
    MemPut<float>(0x872660, 184.0f);
    MemPut<BYTE>(0x7051D7, 184);
}

void CMultiplayerSA::GetAmbientColor(float& red, float& green, float& blue) const
{
    if (bUsingCustomAmbientColor)
        red = fAmbientColorR, green = fAmbientColorG, blue = fAmbientColorB;
    else
        red = *(float*)0xB7C4A0, green = *(float*)0xB7C4A4, blue = *(float*)0xB7C4A8;
}

bool CMultiplayerSA::SetAmbientColor(float red, float green, float blue)
{
    bUsingCustomAmbientColor = true;
    fAmbientColorR = red;
    fAmbientColorG = green;
    fAmbientColorB = blue;
    return true;
}

bool CMultiplayerSA::ResetAmbientColor()
{
    bUsingCustomAmbientColor = false;
    return true;
}

void CMultiplayerSA::GetAmbientObjectColor(float& red, float& green, float& blue) const
{
    if (bUsingCustomAmbientObjectColor)
        red = fAmbientObjectColorR, green = fAmbientObjectColorG, blue = fAmbientObjectColorB;
    else
        red = *(float*)0xB7C4AC, green = *(float*)0xB7C4B0, blue = *(float*)0xB7C4B4;
}

bool CMultiplayerSA::SetAmbientObjectColor(float red, float green, float blue)
{
    bUsingCustomAmbientObjectColor = true;
    fAmbientObjectColorR = red;
    fAmbientObjectColorG = green;
    fAmbientObjectColorB = blue;
    return true;
}

bool CMultiplayerSA::ResetAmbientObjectColor()
{
    bUsingCustomAmbientObjectColor = false;
    return true;
}

void CMultiplayerSA::GetDirectionalColor(float& red, float& green, float& blue) const
{
    if (bUsingCustomDirectionalColor)
        red = fDirectionalColorR, green = fDirectionalColorG, blue = fDirectionalColorB;
    else
        red = *(float*)0xB7C4B8, green = *(float*)0xB7C4BC, blue = *(float*)0xB7C4C0;
}

bool CMultiplayerSA::SetDirectionalColor(float red, float green, float blue)
{
    bUsingCustomDirectionalColor = true;
    fDirectionalColorR = red;
    fDirectionalColorG = green;
    fDirectionalColorB = blue;
    return true;
}

bool CMultiplayerSA::ResetDirectionalColor()
{
    bUsingCustomDirectionalColor = false;
    return true;
}

float CMultiplayerSA::GetSpriteSize() const
{
    return *(float*)0xB7C4E0;
}

bool CMultiplayerSA::SetSpriteSize(float size)
{
    MemPut<BYTE>(0x55FC21, 0xDD);
    MemPut<BYTE>(0x55FC22, 0xD8);
    MemPut<BYTE>(0x55FC23, 0x90);

    MemPutFast<float>(0xB7C4E0, size);
    return true;
}

bool CMultiplayerSA::ResetSpriteSize()
{
    MemPut<BYTE>(0x55FC21, 0xD9);
    MemPut<BYTE>(0x55FC22, 0x5E);
    MemPut<BYTE>(0x55FC23, 0x40);
    return true;
}

float CMultiplayerSA::GetSpriteBrightness() const
{
    return *(float*)0xB7C4E4;
}

bool CMultiplayerSA::SetSpriteBrightness(float brightness)
{
    MemPut<BYTE>(0x55FC34, 0xDD);
    MemPut<BYTE>(0x55FC35, 0xD8);
    MemPut<BYTE>(0x55FC36, 0x90);

    MemPutFast<float>(0xB7C4E4, brightness);
    return true;
}

bool CMultiplayerSA::ResetSpriteBrightness()
{
    MemPut<BYTE>(0x55FC34, 0xD9);
    MemPut<BYTE>(0x55FC35, 0x5E);
    MemPut<BYTE>(0x55FC36, 0x44);
    return true;
}

int16 CMultiplayerSA::GetPoleShadowStrength() const
{
    return *(int16*)0xB7C4EC;
}

bool CMultiplayerSA::SetPoleShadowStrength(int16 strength)
{
    MemSet((LPVOID)0x55FCB8, 0x90, 4);
    MemSet((LPVOID)(0x56023A + 2), 0x90, 3);
    MemSet((LPVOID)(0x5602A6 + 2), 0x90, 3);

    MemPutFast<int16>(0xB7C4EC, strength);
    return true;
}

bool CMultiplayerSA::ResetPoleShadowStrength()
{
    BYTE originalMov[4] = {0x66, 0x89, 0x46, 0x4C};
    MemCpy((LPVOID)0x55FCB8, &originalMov, 4);

    BYTE originalCodes[3] = {0xEC, 0xC4, 0xB7};
    MemCpy((LPVOID)(0x56023A + 2), &originalCodes, 3);
    MemCpy((LPVOID)(0x5602A6 + 2), &originalCodes, 3);
    return true;
}

int16 CMultiplayerSA::GetShadowStrength() const
{
    return *(int16*)0xB7C4E8;
}

bool CMultiplayerSA::SetShadowStrength(int16 strength)
{
    MemSet((LPVOID)0x55FC5E, 0x90, 4);
    MemSet((LPVOID)(0x56022E + 2), 0x90, 3);
    MemSet((LPVOID)(0x560234 + 2), 0x90, 3);
    MemSet((LPVOID)(0x56029A + 2), 0x90, 3);
    MemSet((LPVOID)(0x5602A0 + 2), 0x90, 3);

    MemPutFast<int16>(0xB7C4E8, strength);
    return true;
}

bool CMultiplayerSA::ResetShadowStrength()
{
    BYTE originalMov[4] = {0x66, 0x89, 0x46, 0x48};
    MemCpy((LPVOID)0x55FC5E, &originalMov, 4);

    BYTE originalCodes[3] = {0xE8, 0xC4, 0xB7};
    MemCpy((LPVOID)(0x56022E + 2), &originalCodes, 3);
    MemCpy((LPVOID)(0x560234 + 2), &originalCodes, 3);
    MemCpy((LPVOID)(0x56029A + 2), &originalCodes, 3);
    MemCpy((LPVOID)(0x5602A0 + 2), &originalCodes, 3);
    return true;
}

float CMultiplayerSA::GetShadowsOffset() const
{
    return m_fShadowsOffset;
}

bool CMultiplayerSA::SetShadowsOffset(float offset)
{
    m_fShadowsOffset = offset;
    return true;
}

bool CMultiplayerSA::ResetShadowsOffset()
{
    m_fShadowsOffset = DEFAULT_SHADOWS_OFFSET;
    return true;
}

float CMultiplayerSA::GetLightsOnGroundBrightness() const
{
    return *(float*)0xB7C4F8;
}

bool CMultiplayerSA::SetLightsOnGroundBrightness(float brightness)
{
    MemPut<BYTE>(0x55FDBC, 0xDD);
    MemPut<BYTE>(0x55FDBD, 0xD8);
    MemPut<BYTE>(0x55FDBE, 0x90);
    MemSet((LPVOID)(0x5602AC + 2), 0x90, 3);

    MemPutFast<float>(0xB7C4F8, brightness);
    return true;
}

bool CMultiplayerSA::ResetLightsOnGroundBrightness()
{
    BYTE originalFstp[3] = {0xD9, 0x5E, 0x58};
    MemCpy((LPVOID)0x55FDBC, &originalFstp, 3);

    BYTE originalCodes[3] = {0xF8, 0xC4, 0xB7};
    MemCpy((LPVOID)(0x5602AC + 2), &originalCodes, 3);
    return true;
}

void CMultiplayerSA::GetLowCloudsColor(int16& red, int16& green, int16& blue) const
{
    if (bUsingCustomLowCloudsColor)
        red = iLowCloudsColorR, green = iLowCloudsColorG, blue = iLowCloudsColorB;
    else
        red = *(int16*)0xB7C4FC, green = *(int16*)0xB7C4FE, blue = *(int16*)0xB7C500;
}

bool CMultiplayerSA::SetLowCloudsColor(int16 red, int16 green, int16 blue)
{
    bUsingCustomLowCloudsColor = true;
    iLowCloudsColorR = red;
    iLowCloudsColorG = green;
    iLowCloudsColorB = blue;
    return true;
}

bool CMultiplayerSA::ResetLowCloudsColor()
{
    bUsingCustomLowCloudsColor = false;
    return true;
}

void CMultiplayerSA::GetBottomCloudsColor(int16& red, int16& green, int16& blue) const
{
    if (bUsingCustomBottomCloudsColor)
        red = iBottomCloudsColorR, green = iBottomCloudsColorG, blue = iBottomCloudsColorB;
    else
        red = *(int16*)0xB7C502, green = *(int16*)0xB7C504, blue = *(int16*)0xB7C506;
}

bool CMultiplayerSA::SetBottomCloudsColor(int16 red, int16 green, int16 blue)
{
    bUsingCustomBottomCloudsColor = true;
    iBottomCloudsColorR = red;
    iBottomCloudsColorG = green;
    iBottomCloudsColorB = blue;
    return true;
}

bool CMultiplayerSA::ResetBottomCloudsColor()
{
    bUsingCustomBottomCloudsColor = false;
    return true;
}

float CMultiplayerSA::GetCloudsAlpha1() const
{
    return *(float*)0xB7C538;
}

bool CMultiplayerSA::SetCloudsAlpha1(float alpha)
{
    MemPut<BYTE>(0x55FDD5, 0xD8);
    MemPutFast<float>(0xB7C538, alpha);
    return true;
}

bool CMultiplayerSA::ResetCloudsAlpha1()
{
    MemPut<BYTE>(0x55FDD5, 0xD9);
    return true;
}

float CMultiplayerSA::GetIllumination() const
{
    return *(float*)0xB7C544;
}

bool CMultiplayerSA::SetIllumination(float illumination)
{
    MemPut<BYTE>(0x55FE46, 0xD8);
    MemPutFast<float>(0xB7C544, illumination);
    return true;
}

bool CMultiplayerSA::ResetIllumination()
{
    MemPut<BYTE>(0x55FE46, 0xD9);
    return true;
}

bool CMultiplayerSA::GetExplosionsDisabled()
{
    return m_bExplosionsDisabled;
}

void CMultiplayerSA::DisableExplosions(bool bDisabled)
{
    m_bExplosionsDisabled = bDisabled;
}

void CMultiplayerSA::SetExplosionHandler(ExplosionHandler* pExplosionHandler)
{
    m_pExplosionHandler = pExplosionHandler;
}

void CMultiplayerSA::SetProjectileHandler(ProjectileHandler* pProjectileHandler)
{
    m_pProjectileHandler = pProjectileHandler;
}

void CMultiplayerSA::SetProjectileStopHandler(ProjectileStopHandler* pProjectileHandler)
{
    m_pProjectileStopHandler = pProjectileHandler;
}

void CMultiplayerSA::SetBreakTowLinkHandler(BreakTowLinkHandler* pBreakTowLinkHandler)
{
    m_pBreakTowLinkHandler = pBreakTowLinkHandler;
}

void CMultiplayerSA::SetDrawRadarAreasHandler(DrawRadarAreasHandler* pRadarAreasHandler)
{
    m_pDrawRadarAreasHandler = pRadarAreasHandler;
}

void CMultiplayerSA::SetRender3DStuffHandler(Render3DStuffHandler* pHandler)
{
    m_pRender3DStuffHandler = pHandler;
}

void CMultiplayerSA::SetDamageHandler(DamageHandler* pDamageHandler)
{
    m_pDamageHandler = pDamageHandler;
}

void CMultiplayerSA::SetDeathHandler(DeathHandler* pDeathHandler)
{
    m_pDeathHandler = pDeathHandler;
}

void CMultiplayerSA::SetFireHandler(FireHandler* pFireHandler)
{
    m_pFireHandler = pFireHandler;
}

void CMultiplayerSA::SetProcessCamHandler(ProcessCamHandler* pProcessCamHandler)
{
    m_pProcessCamHandler = pProcessCamHandler;
}

void CMultiplayerSA::SetChokingHandler(ChokingHandler* pChokingHandler)
{
    m_pChokingHandler = pChokingHandler;
}

void CMultiplayerSA::SetPreWorldProcessHandler(PreWorldProcessHandler* pHandler)
{
    m_pPreWorldProcessHandler = pHandler;
}

void CMultiplayerSA::SetPostWorldProcessHandler(PostWorldProcessHandler* pHandler)
{
    m_pPostWorldProcessHandler = pHandler;
}

void CMultiplayerSA::SetPostWorldProcessPedsAfterPreRenderHandler(PostWorldProcessPedsAfterPreRenderHandler* pHandler)
{
    m_postWorldProcessPedsAfterPreRenderHandler = pHandler;
}

void CMultiplayerSA::SetIdleHandler(IdleHandler* pHandler)
{
    m_pIdleHandler = pHandler;
}

void CMultiplayerSA::SetPreFxRenderHandler(PreFxRenderHandler* pHandler)
{
    m_pPreFxRenderHandler = pHandler;
}

void CMultiplayerSA::SetPostColorFilterRenderHandler(PostColorFilterRenderHandler* pHandler)
{
    m_pPostColorFilterRenderHandler = pHandler;
}

void CMultiplayerSA::SetPreHudRenderHandler(PreHudRenderHandler* pHandler)
{
    m_pPreHudRenderHandler = pHandler;
}

void CMultiplayerSA::SetProcessCollisionHandler(ProcessCollisionHandler* pHandler)
{
    m_pProcessCollisionHandler = pHandler;
}

void CMultiplayerSA::SetHeliKillHandler(HeliKillHandler* pHandler)
{
    m_pHeliKillHandler = pHandler;
}

void CMultiplayerSA::SetObjectDamageHandler(ObjectDamageHandler* pHandler)
{
    m_pObjectDamageHandler = pHandler;
}

void CMultiplayerSA::SetObjectBreakHandler(ObjectBreakHandler* pHandler)
{
    m_pObjectBreakHandler = pHandler;
}

void CMultiplayerSA::SetFxSystemDestructionHandler(FxSystemDestructionHandler* pHandler)
{
    m_pFxSystemDestructionHandler = pHandler;
}

void CMultiplayerSA::SetDrivebyAnimationHandler(DrivebyAnimationHandler* pHandler)
{
    m_pDrivebyAnimationHandler = pHandler;
}

void CMultiplayerSA::SetAudioZoneRadioSwitchHandler(AudioZoneRadioSwitchHandler* pHandler)
{
    m_pAudioZoneRadioSwitchHandler = pHandler;
}

// What we do here is check if the idle handler has been set
bool CMultiplayerSA::IsConnected()
{
    return m_pIdleHandler != NULL;
}

void CMultiplayerSA::HideRadar(bool bHide)
{
    bHideRadar = bHide;
}

void CMultiplayerSA::AllowMouseMovement(bool bAllow)
{
    if (bAllow)
        MemPut<BYTE>(0x6194A0, 0xC3);
    else
        MemPut<BYTE>(0x6194A0, 0xE9);
}

void CMultiplayerSA::DoSoundHacksOnLostFocus(bool bLostFocus)
{
    if (bLostFocus)
        MemSet((void*)0x4D9888, 0x90, 5);
    else
    {
        // 004D9888   . E8 03 F1 FF FF    CALL gta_sa_u.004D8990
        MemPut<BYTE>(0x4D9888, 0xE8);
        MemPut<BYTE>(0x4D9889, 0x03);
        MemPut<BYTE>(0x4D988A, 0xF1);
        MemPut<BYTE>(0x4D988B, 0xFF);
        MemPut<BYTE>(0x4D988C, 0xFF);
    }
}

void CMultiplayerSA::SetCenterOfWorld(CEntity* entity, CVector* vecPosition, FLOAT fHeading)
{
    if (vecPosition)
    {
        bInStreamingUpdate = false;

        vecCenterOfWorld.fX = vecPosition->fX;
        vecCenterOfWorld.fY = vecPosition->fY;
        vecCenterOfWorld.fZ = vecPosition->fZ;

        if (entity)
        {
            CEntitySA* pEntitySA = dynamic_cast<CEntitySA*>(entity);
            if (pEntitySA)
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

        // DWORD dwCurrentValue = *(DWORD *)FUNC_CPlayerInfoBase;
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
        mov     ecx, HOOKPOS_FindPlayerCoors
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
    if (activeEntityForStreaming)
    {
        // Do something...
        _asm
        {
            mov     edi, FUNC_CPlayerInfoBase
            mov     ebx, [edi]
            mov     dwSavedPlayerPointer, ebx
            mov     ebx, activeEntityForStreaming
            mov     [edi], ebx
        }
    }

    _asm
    {
        mov     edi, eax

        // Call FUNC_CStreaming_Update
        mov     eax, FUNC_CStreaming_Update
        call    eax
    }

    // We have an entity for streaming?
    if (activeEntityForStreaming)
    {
        _asm
        {
            // ...
            mov     edi, FUNC_CPlayerInfoBase
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
        mov     eax, HOOKPOS_CStreaming_Update_Caller
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

        mov     edx, FUNC_CAudioEngine__DisplayRadioStationName
        call    edx
    }

    if (!bSetCenterOfWorld)
    {
        _asm
        {
            mov     edx, FUNC_CHud_Draw
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

        mov     eax, HOOKPOS_CHud_Draw_Caller
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

        mov     edx, HOOKPOS_FindPlayerCentreOfWorld
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
        mov     eax, HOOKPOS_FindPlayerHeading
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
        mov     edx, HOOKPOS_CCustomRoadsignMgr__RenderRoadsignAtomic
        add     edx, 6
        jmp     edx
no_render:
        mov     edx, 0x6FF40B
        jmp     edx
    }
}

bool CallBreakTowLinkHandler(CVehicleSAInterface* vehicle)
{
    SClientEntity<CVehicleSA>* pVehicleClientEntity = pGameInterface->GetPools()->GetVehicle((DWORD*)vehicle);
    CVehicle*                  pVehicle = pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
    if (pVehicle && m_pBreakTowLinkHandler)
    {
        return m_pBreakTowLinkHandler(pVehicle);
    }
    return true;
}

void _declspec(naked) HOOK_CRadar__DrawRadarGangOverlay()
{
    _asm
    {
        pushad
    }

    if (m_pDrawRadarAreasHandler) m_pDrawRadarAreasHandler();

    _asm
    {
        popad
        retn
    }
}

CVehicleSAInterface* towingVehicle;

void _declspec(naked) HOOK_Trailer_BreakTowLink()
{
    _asm
    {
        mov     towingVehicle, ecx
        pushad
    }

    if (CallBreakTowLinkHandler(towingVehicle))
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
        mov     ecx, HOOKPOS_Trailer_BreakTowLink
        add     ecx, 6
        jmp     ecx
    }
}

eExplosionType explosionType;
CVector        vecExplosionLocation;
DWORD          explosionCreator = 0;
DWORD          explosionEntity = 0;

bool CallExplosionHandler()
{
    // Find out who the creator is
    CEntity*            pExplosionCreator = NULL;
    CEntity*            pExplodingEntity = NULL;
    CEntitySAInterface* pInterface = (CEntitySAInterface*)explosionCreator;
    CEntitySAInterface* pExplodingEntityInterface = (CEntitySAInterface*)explosionEntity;

    if (pInterface)
    {
        pExplosionCreator = pGameInterface->GetPools()->GetEntity((DWORD*)pInterface);
    }

    if (pExplodingEntityInterface)
    {
        pExplodingEntity = pGameInterface->GetPools()->GetEntity((DWORD*)pExplodingEntityInterface);
    }

    return m_pExplosionHandler(pExplodingEntity, pExplosionCreator, vecExplosionLocation, explosionType);
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
    if (!CallExplosionHandler())
    {
        _asm
        {
            popad
            retn // if they return false from the handler, they don't want the explosion to show
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
        noexplosionhandler:

        // Replaced code
        sub     esp, 0x1C
        push    ebx
        push    ebp
        push    esi

        // Return to the calling function and resume (do the explosion)
        mov     edx, HOOKPOS_CExplosion_AddExplosion
        add     edx, 6
        jmp     edx
    }
}

CEntitySAInterface* entity;
float*              entityEdgeHeight;
float               edgeHeight;
CVector*            pedPosition;

bool processGrab()
{
    if (entity->nType == ENTITY_TYPE_OBJECT)
    {
        // CObjectSA * object = (CObjectSA*)entity;
        // CModelInfo * info = pGameInterface->GetModelInfo(entity->m_nModelIndex);
        if (entity->Placeable.matrix)
            edgeHeight = *entityEdgeHeight + entity->Placeable.matrix->vPos.fZ;
        else
            edgeHeight = *entityEdgeHeight + entity->Placeable.m_transform.m_translate.fZ;
    }
    else
        edgeHeight = *entityEdgeHeight;

    if (edgeHeight - pedPosition->fZ >= 1.4f)
        return true;
    return false;
}

// 0x67DABE
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

    if (processGrab())
    {
        _asm
        {
            popad
            mov     eax, 0x67DAD6
            jmp     eax
        }
    }
    else
    {
        _asm
        {
            popad
            mov     eax, 0x67DAD1
            jmp     eax
        }
    }
}

char*  szCreateFxSystem_ExplosionType = 0;
DWORD* pCreateFxSystem_Matrix = 0;
DWORD* pNewCreateFxSystem_Matrix = 0;

void _declspec(naked) HOOK_FxManager_CreateFxSystem()
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
    if (pCreateFxSystem_Matrix != 0 && strncmp(szCreateFxSystem_ExplosionType, "explosion", 9) == 0)
    {
        // Copy the matrix so we don't crash if the owner of this matrix is deleted
        pNewCreateFxSystem_Matrix = (DWORD*)malloc(64);
        MemCpyFast(pNewCreateFxSystem_Matrix, pCreateFxSystem_Matrix, 64);

        // Add it to the list over FxSystem matrices we've copied
        AddFxSystemPointer(pNewCreateFxSystem_Matrix);
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
        mov         eax, [esp+16]
        mov         edx, [esp+8]

        // Jump back to the rest of the function we hooked
        jmp         RETURN_FxManager_CreateFxSystem
    }
}

DWORD  dwDestroyFxSystem_Pointer = 0;
DWORD* pDestroyFxSystem_Matrix = 0;

void _declspec(naked) HOOK_FxManager_DestroyFxSystem()
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
    pDestroyFxSystem_Matrix = *((DWORD**)(dwDestroyFxSystem_Pointer + 12));

    // Delete it if it's in our list
    RemoveFxSystemPointer(pDestroyFxSystem_Matrix);

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

bool CCam_ProcessFixed(class CCamSAInterface* pCamInterface)
{
    CCam* pCam = static_cast<CCameraSA*>(pGameInterface->GetCamera())->GetCam(pCamInterface);

    if (m_pProcessCamHandler && pCam)
    {
        return m_pProcessCamHandler(pCam);
    }
    return false;
}

CCamSAInterface* CCam_ProcessFixed_pCam;

void _declspec(naked) HOOK_CCam_ProcessFixed()
{
    _asm
    {
        mov CCam_ProcessFixed_pCam, ecx
    }

    if (CCam_ProcessFixed(CCam_ProcessFixed_pCam))
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

void _declspec(naked) HOOK_Render3DStuff()
{
    _asm
    {
        pushad
    }
    if (m_pRender3DStuffHandler) m_pRender3DStuffHandler();

    _asm
    {
        popad
        mov eax, FUNC_Render3DStuff
        jmp eax
    }
}

CPedSAInterface* pProcessPlayerWeaponPed = NULL;
bool             ProcessPlayerWeapon()
{
    if (IsLocalPlayer(pProcessPlayerWeaponPed))
        return true;

    SClientEntity<CPedSA>* pPedClientEntity = pGameInterface->GetPools()->GetPed((DWORD*)pProcessPlayerWeaponPed);
    CPlayerPed*            pPed = pPedClientEntity ? dynamic_cast<CPlayerPed*>(pPedClientEntity->pEntity) : nullptr;
    if (pPed)
    {
        CRemoteDataStorageSA* pData = CRemoteDataSA::GetRemoteDataStorage(pPed);
        if (pData)
        {
            if (pData->ProcessPlayerWeapon())
            {
                return true;
            }
        }
    }
    return false;
}

void _declspec(naked) HOOK_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon()
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
    if (ProcessPlayerWeapon())
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

CPedSAInterface* pIsPlayerPed = NULL;
bool             IsPlayer()
{
    return true;
}

void _declspec(naked) HOOK_CPed_IsPlayer()
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
    if (IsPlayer())
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

void CRunningScript_Process()
{
    if (!bHasProcessedScript)
    {
        CCamera* pCamera = pGameInterface->GetCamera();
        pCamera->SetFadeColor(0, 0, 0);
        pCamera->Fade(0.0f, FADE_OUT);

        DWORD dwFunc = 0x409D10;            // RequestSpecialModel

        char szModelName[64];
        strcpy(szModelName, "player");
        _asm
        {
            push    26
            lea     eax, szModelName
            push    eax
            push    0
            call    dwFunc
            add     esp, 12
        }

        dwFunc = 0x40EA10;            // load all requested models
        _asm
        {
            push    1
            call    dwFunc
            add     esp, 4
        }

        dwFunc = 0x60D790;            // setup player ped
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
        dwFunc = 0x420B80;            // set position
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
        /*
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
        }
*/

        bHasProcessedScript = true;
    }
}

void _declspec(naked) HOOK_CRunningScript_Process()
{
    _asm
    {
        pushad
    }

    CRunningScript_Process();

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
    if (pDerailingTrain->m_pVehicle->IsDerailable())
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
static DWORD         dwAlphaEntity = 0;
static bool          bEntityHasAlpha = false;
static unsigned char ucCurrentAlpha[1024];
static uint          uiAlphaIdx = 0;

static void SetEntityAlphaHooked(DWORD dwEntity, DWORD dwCallback, DWORD dwAlpha)
{
    if (dwEntity)
    {
        // Alpha setting of SetRwObjectAlpha function is done by
        // iterating all materials of a clump and its atoms, and
        // calling a given callback. We temporarily overwrite that
        // callback with our own callback and then restore it.
        MemPutFast<DWORD>(0x5332A2, dwCallback);
        MemPutFast<DWORD>(0x5332F3, dwCallback);

        // Call SetRwObjectAlpha
        DWORD dwFunc = FUNC_SetRwObjectAlpha;
        _asm
        {
            mov     ecx, dwEntity
            push    dwAlpha
            call    dwFunc
        }

        // Restore the GTA callbacks
        MemPutFast<DWORD>(0x5332A2, (DWORD)(0x533280));
        MemPutFast<DWORD>(0x5332F3, (DWORD)(0x533280));
    }
}

static RpMaterial* HOOK_GetAlphaValues(RpMaterial* pMaterial, unsigned char ucAlpha)
{
    ucCurrentAlpha[uiAlphaIdx] = pMaterial->color.a;
    uiAlphaIdx = std::min(uiAlphaIdx + 1, NUMELMS(ucCurrentAlpha) - 1);

    return pMaterial;
}
static RpMaterial* HOOK_SetAlphaValues(RpMaterial* pMaterial, unsigned char ucAlpha)
{
    pMaterial->color.a = static_cast<unsigned char>((float)(pMaterial->color.a) * (float)ucAlpha / 255.0f);

    return pMaterial;
}
static RpMaterial* HOOK_RestoreAlphaValues(RpMaterial* pMaterial, unsigned char ucAlpha)
{
    pMaterial->color.a = ucCurrentAlpha[uiAlphaIdx];
    uiAlphaIdx = std::min(uiAlphaIdx + 1, NUMELMS(ucCurrentAlpha) - 1);

    return pMaterial;
}

static void GetAlphaAndSetNewValues(unsigned char ucAlpha)
{
    if (ucAlpha < 255)
    {
        bEntityHasAlpha = true;
        uiAlphaIdx = 0;
        SetEntityAlphaHooked(dwAlphaEntity, (DWORD)HOOK_GetAlphaValues, 0);
        SetEntityAlphaHooked(dwAlphaEntity, (DWORD)HOOK_SetAlphaValues, ucAlpha);
    }
    else
        bEntityHasAlpha = false;
}
static void RestoreAlphaValues()
{
    if (bEntityHasAlpha)
    {
        uiAlphaIdx = 0;
        SetEntityAlphaHooked(dwAlphaEntity, (DWORD)HOOK_RestoreAlphaValues, 0);
    }
}

/**
 ** Vehicles
 **/

static void SetVehicleAlpha()
{
    CVehicleSAInterface* pInterface = ((CVehicleSAInterface*)dwAlphaEntity);
    unsigned char        ucAlpha = pInterface->m_pVehicle->GetAlpha();

    if (ucAlpha < 255)
        GetAlphaAndSetNewValues(ucAlpha);
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

    SetVehicleAlpha();

    _asm
    {
        popad
        add     esp, 0x8
        test    eax, eax
        jmp     dwCVehicle_SetupRender_ret
    }
}

static DWORD dwCVehicle_ResetAfterRender_ret = 0x6D0E43;
void _declspec(naked) HOOK_CVehicle_ResetAfterRender()
{
    _asm
    {
        pushad
    }

    RestoreAlphaValues();

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
static void SetObjectAlpha()
{
    bEntityHasAlpha = false;
    bObjectIsAGangTag = false;

    if (dwAlphaEntity)
    {
        SClientEntity<CObjectSA>* pObjectClientEntity = pGameInterface->GetPools()->GetObject((DWORD*)dwAlphaEntity);
        CObject*                  pObject = pObjectClientEntity ? pObjectClientEntity->pEntity : nullptr;
        if (pObject)
        {
            if (pObject->IsAGangTag())
            {
                // For some weird reason, gang tags don't appear unsprayed
                // if we don't set their alpha to a value less than 255.
                bObjectIsAGangTag = true;
                GetAlphaAndSetNewValues(std::min(pObject->GetAlpha(), (unsigned char)254));
            }
            else
                GetAlphaAndSetNewValues(pObject->GetAlpha());
        }
    }
}

DWORD dwCObjectRenderRet = 0;
void _declspec(naked) HOOK_CObject_PostRender()
{
    _asm
    {
        pushad
    }

    TIMING_CHECKPOINT("-ObjRndr");
    RestoreAlphaValues();

    _asm
    {
        popad
        mov         edx, dwCObjectRenderRet
        jmp         edx
    }
}

// Note: This hook is also called for world objects (light poles, wooden fences, etc).
void _declspec(naked) HOOK_CObject_Render()
{
    _asm
    {
        mov         dwAlphaEntity, ecx
        pushad
    }

    TIMING_CHECKPOINT("+ObjRndr");
    SetObjectAlpha();

    _asm
    {
        popad
        mov         edx, [esp]
        mov         dwCObjectRenderRet, edx
        mov         edx, HOOK_CObject_PostRender
        mov         [esp], edx
        pushad
    }

    if (bObjectIsAGangTag) goto render_a_tag;

    _asm
    {
        popad
        jmp         FUNC_CEntity_Render
    }

render_a_tag:
    _asm
    {
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

void _cdecl DoEndWorldColorsPokes()
{
    if (bUsingCustomSkyGradient)
    {
        MemPutFast<BYTE>(0xB7C4C4, ucSkyGradientTopR);
        MemPutFast<BYTE>(0xB7C4C6, ucSkyGradientTopG);
        MemPutFast<BYTE>(0xB7C4C8, ucSkyGradientTopB);

        MemPutFast<BYTE>(0xB7C4CA, ucSkyGradientBottomR);
        MemPutFast<BYTE>(0xB7C4CC, ucSkyGradientBottomG);
        MemPutFast<BYTE>(0xB7C4CE, ucSkyGradientBottomB);
    }
    if (bUsingCustomAmbientColor)
    {
        MemPutFast<float>(0xB7C4A0, fAmbientColorR);
        MemPutFast<float>(0xB7C4A4, fAmbientColorG);
        MemPutFast<float>(0xB7C4A8, fAmbientColorB);
    }
    if (bUsingCustomAmbientObjectColor)
    {
        MemPutFast<float>(0xB7C4AC, fAmbientObjectColorR);
        MemPutFast<float>(0xB7C4B0, fAmbientObjectColorG);
        MemPutFast<float>(0xB7C4B4, fAmbientObjectColorB);
    }
    if (bUsingCustomDirectionalColor)
    {
        MemPutFast<float>(0xB7C4B8, fDirectionalColorR);
        MemPutFast<float>(0xB7C4BC, fDirectionalColorG);
        MemPutFast<float>(0xB7C4C0, fDirectionalColorB);
    }
    if (bUsingCustomLowCloudsColor)
    {
        MemPutFast<int16>(0xB7C4FC, iLowCloudsColorR);
        MemPutFast<int16>(0xB7C4FE, iLowCloudsColorG);
        MemPutFast<int16>(0xB7C500, iLowCloudsColorB);
    }
    if (bUsingCustomBottomCloudsColor)
    {
        MemPutFast<int16>(0xB7C502, iBottomCloudsColorR);
        MemPutFast<int16>(0xB7C504, iBottomCloudsColorG);
        MemPutFast<int16>(0xB7C506, iBottomCloudsColorB);
    }
    if (bUsingCustomWaterColor)
    {
        MemPutFast<float>(0xB7C508, fWaterColorR);
        MemPutFast<float>(0xB7C50C, fWaterColorG);
        MemPutFast<float>(0xB7C510, fWaterColorB);
        MemPutFast<float>(0xB7C514, fWaterColorA);
    }
}

// Note: This hook is called at the end of the function that sets the world colours (sky gradient, water colour, etc).
void _declspec(naked) HOOK_EndWorldColors()
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
void _declspec(naked) HOOK_CWorld_ProcessVerticalLineSectorList()
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
static DWORD         dwChokingChoke = 0x4C05C1;
static DWORD         dwChokingDontchoke = 0x4C0620;
static unsigned char ucChokingWeaponType = 0;
void _declspec(naked) HOOK_ComputeDamageResponse_StartChoking()
{
    _asm
    {
        pushad
        mov     al, [esp+0x8C]
        mov     ucChokingWeaponType, al
    }

    if (m_pChokingHandler && m_pChokingHandler(ucChokingWeaponType) == false) goto dont_choke;

    _asm
    {
        popad
        mov     ecx, [edi]
        mov     eax, [ecx+0x47C]
        jmp     dwChokingChoke
    }

dont_choke:
    _asm
    {
        popad
        jmp     dwChokingDontchoke
    }
}

void CMultiplayerSA::DisableEnterExitVehicleKey(bool bDisabled)
{
    // PREVENT THE PLAYER LEAVING THEIR VEHICLE
    //  005400D0     32C0           XOR AL,AL
    //  005400D2     C3             RETN
    if (!bDisabled)
    {
        // CPlayerInfo__Process
        MemPut<BYTE>(0x5702FD, 0xE8);
        MemPut<BYTE>(0x5702FE, 0xCE);
        MemPut<BYTE>(0x5702FF, 0xFD);
        MemPut<BYTE>(0x570300, 0xFC);
        MemPut<BYTE>(0x570301, 0xFF);

        // CAutomobile__ProcessControlInputs
        MemPut<BYTE>(0x6AD75A, 0xE8);
        MemPut<BYTE>(0x6AD75B, 0x71);
        MemPut<BYTE>(0x6AD75C, 0x29);
        MemPut<BYTE>(0x6AD75D, 0xE9);
        MemPut<BYTE>(0x6AD75E, 0xFF);

        // CBike__ProcessControlInputs
        MemPut<BYTE>(0x6BE34B, 0xE8);
        MemPut<BYTE>(0x6BE34C, 0x80);
        MemPut<BYTE>(0x6BE34D, 0x1D);
        MemPut<BYTE>(0x6BE34E, 0xE8);
        MemPut<BYTE>(0x6BE34F, 0xFF);

        // CTaskSimpleJetPack__ProcessControlInput
        MemPut<BYTE>(0x67E834, 0xE8);
        MemPut<BYTE>(0x67E835, 0x97);
        MemPut<BYTE>(0x67E836, 0x18);
        MemPut<BYTE>(0x67E837, 0xEC);
        MemPut<BYTE>(0x67E838, 0xFF);
    }
    else
    {
        // CPlayerInfo__Process
        MemPut<BYTE>(0x5702FD, 0x32);
        MemPut<BYTE>(0x5702FE, 0xC0);
        MemPut<BYTE>(0x5702FF, 0x90);
        MemPut<BYTE>(0x570300, 0x90);
        MemPut<BYTE>(0x570301, 0x90);

        // CAutomobile__ProcessControlInputs
        MemPut<BYTE>(0x6AD75A, 0x32);
        MemPut<BYTE>(0x6AD75B, 0xC0);
        MemPut<BYTE>(0x6AD75C, 0x90);
        MemPut<BYTE>(0x6AD75D, 0x90);
        MemPut<BYTE>(0x6AD75E, 0x90);

        // CBike__ProcessControlInputs
        MemPut<BYTE>(0x6BE34B, 0x32);
        MemPut<BYTE>(0x6BE34C, 0xC0);
        MemPut<BYTE>(0x6BE34D, 0x90);
        MemPut<BYTE>(0x6BE34E, 0x90);
        MemPut<BYTE>(0x6BE34F, 0x90);

        // CTaskSimpleJetPack__ProcessControlInput
        MemPut<BYTE>(0x67E834, 0x32);
        MemPut<BYTE>(0x67E835, 0xC0);
        MemPut<BYTE>(0x67E836, 0x90);
        MemPut<BYTE>(0x67E837, 0x90);
        MemPut<BYTE>(0x67E838, 0x90);
    }

    // CPad__ExitVehicleJustDown
    if (!bDisabled)
    {
        MemSet((void*)0x540120, 0x90, 1);
        MemSet((void*)0x540121, 0x90, 1);
        MemSet((void*)0x540122, 0x90, 1);
    }
    else
    {
        MemSet((void*)0x540120, 0x32, 1);
        MemSet((void*)0x540121, 0xC0, 1);
        MemSet((void*)0x540122, 0xC3, 1);
    }
}

void CMultiplayerSA::PreventLeavingVehicles()
{
    MemSet((void*)0x6B5A10, 0xC3, 1);

    // 006B7449     E9 FF000000    JMP gta_sa.006B754D
    MemSet((void*)0x6B7449, 0xE9, 1);
    MemSet((void*)(0x6B7449 + 1), 0xFF, 1);
    MemSet((void*)(0x6B7449 + 2), 0x00, 1);

    // 006B763C     E9 01010000    JMP gta_sa.006B7742
    MemSet((void*)0x6B763C, 0xE9, 1);
    MemSet((void*)(0x6B763C + 1), 0x01, 1);
    MemSet((void*)(0x6B763C + 2), 0x01, 1);
    MemSet((void*)(0x6B763C + 3), 0x00, 1);

    // 006B7617     E9 26010000    JMP gta_sa.006B7742
    MemSet((void*)0x6B7617, 0xE9, 1);
    MemSet((void*)(0x6B7617 + 1), 0x26, 1);
    MemSet((void*)(0x6B7617 + 2), 0x01, 1);
    MemSet((void*)(0x6B7617 + 3), 0x00, 1);
    MemSet((void*)(0x6B7617 + 4), 0x00, 1);

    // 006B62A7     EB 74          JMP SHORT gta_sa.006B631D
    MemSet((void*)0x6B62A7, 0xEB, 1);

    // 006B7642     E9 FB000000    JMP gta_sa_u.006B7742
    MemSet((void*)0x6B7642, 0xE9, 1);
    MemSet((void*)(0x6B7642 + 1), 0xFB, 1);
    MemSet((void*)(0x6B7642 + 2), 0x00, 1);
    MemSet((void*)(0x6B7642 + 3), 0x00, 1);
    MemSet((void*)(0x6B7642 + 4), 0x00, 1);

    // 006B7449     E9 FF000000    JMP gta_sa_u.006B754D
    MemSet((void*)0x6B7449, 0xE9, 1);
    MemSet((void*)(0x6B7449 + 1), 0xFF, 1);
    MemSet((void*)(0x6B7449 + 2), 0x00, 1);

    // For quadbikes hitting water
    // 006A90D8   E9 29020000      JMP gta_sa.006A9306
    MemSet((void*)0x6A90D8, 0xE9, 1);
    MemSet((void*)(0x6A90D8 + 1), 0x29, 1);
    MemSet((void*)(0x6A90D8 + 2), 0x02, 1);
    MemSet((void*)(0x6A90D8 + 3), 0x00, 1);
    MemSet((void*)(0x6A90D8 + 4), 0x00, 1);
}

void CMultiplayerSA::SetPreContextSwitchHandler(PreContextSwitchHandler* pHandler)
{
    m_pPreContextSwitchHandler = pHandler;
}

void CMultiplayerSA::SetPostContextSwitchHandler(PostContextSwitchHandler* pHandler)
{
    m_pPostContextSwitchHandler = pHandler;
}

void CMultiplayerSA::SetPreWeaponFireHandler(PreWeaponFireHandler* pHandler)
{
    m_pPreWeaponFireHandler = pHandler;
}

void CMultiplayerSA::SetPostWeaponFireHandler(PostWeaponFireHandler* pHandler)
{
    m_pPostWeaponFireHandler = pHandler;
}

void CMultiplayerSA::SetBulletImpactHandler(BulletImpactHandler* pHandler)
{
    m_pBulletImpactHandler = pHandler;
}

void CMultiplayerSA::SetBulletFireHandler(BulletFireHandler* pHandler)
{
    m_pBulletFireHandler = pHandler;
}

void CMultiplayerSA::Reset()
{
    bHideRadar = false;
    m_pExplosionHandler = NULL;
    m_pPreContextSwitchHandler = NULL;
    m_pPostContextSwitchHandler = NULL;
    m_pBreakTowLinkHandler = NULL;
    m_pDrawRadarAreasHandler = NULL;
    DisableAllVehicleWeapons(false);
    m_pDamageHandler = NULL;
    m_pDeathHandler = NULL;
    m_pFireHandler = NULL;
    m_pRender3DStuffHandler = NULL;
    m_pFxSystemDestructionHandler = NULL;
}

void CMultiplayerSA::ConvertEulerAnglesToMatrix(CMatrix& Matrix, float fX, float fY, float fZ)
{
    CMatrix_Padded  matrixPadded(Matrix);
    CMatrix_Padded* pMatrixPadded = &matrixPadded;
    DWORD           dwFunc = FUNC_CMatrix__ConvertFromEulerAngles;
    int             iUnknown = 21;
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
    matrixPadded.ConvertToMatrix(Matrix);
}

void CMultiplayerSA::ConvertMatrixToEulerAngles(const CMatrix& Matrix, float& fX, float& fY, float& fZ)
{
    // Convert the given matrix to a padded matrix
    CMatrix_Padded matrixPadded(Matrix);

    // Grab its pointer and call gta's func
    CMatrix_Padded* pMatrixPadded = &matrixPadded;
    DWORD           dwFunc = FUNC_CMatrix__ConvertToEulerAngles;

    float* pfX = &fX;
    float* pfY = &fY;
    float* pfZ = &fZ;
    int    iUnknown = 21;
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

void CMultiplayerSA::RebuildMultiplayerPlayer(CPed* player)
{
    TIMING_CHECKPOINT("+RebuldMulplrPlr");

    CPlayerPed*           playerPed = dynamic_cast<CPlayerPed*>(player);
    CRemoteDataStorageSA* data = NULL;

    if (playerPed)
        data = CRemoteDataSA::GetRemoteDataStorage(playerPed);

    if (data)
    {
        CStatsData localStats;

        // Store the local player stats
        MemCpyFast(&localStats.StatTypesFloat, (void*)0xb79380, sizeof(float) * MAX_FLOAT_STATS);
        MemCpyFast(&localStats.StatTypesInt, (void*)0xb79000, sizeof(int) * MAX_INT_STATS);
        MemCpyFast(&localStats.StatReactionValue, (void*)0xb78f10, sizeof(float) * MAX_REACTION_STATS);

        // Change the local player's stats to the remote player's
        MemCpyFast((void*)0xb79380, data->m_stats.StatTypesFloat, sizeof(float) * MAX_FLOAT_STATS);
        MemCpyFast((void*)0xb79000, data->m_stats.StatTypesInt, sizeof(int) * MAX_INT_STATS);
        MemCpyFast((void*)0xb78f10, data->m_stats.StatReactionValue, sizeof(float) * MAX_REACTION_STATS);

        player->RebuildPlayer();

        // Restore the local player stats
        MemCpyFast((void*)0xb79380, &localStats.StatTypesFloat, sizeof(float) * MAX_FLOAT_STATS);
        MemCpyFast((void*)0xb79000, &localStats.StatTypesInt, sizeof(int) * MAX_INT_STATS);
        MemCpyFast((void*)0xb78f10, &localStats.StatReactionValue, sizeof(float) * MAX_REACTION_STATS);
    }
    TIMING_CHECKPOINT("-RebuldMulplrPlr");
}

float CMultiplayerSA::GetGlobalGravity()
{
    return fGlobalGravity;
}

void CMultiplayerSA::SetGlobalGravity(float fGravity)
{
    fGlobalGravity = fGravity;
}

float CMultiplayerSA::GetLocalPlayerGravity()
{
    return fLocalPlayerGravity;
}

void CMultiplayerSA::SetLocalPlayerGravity(float fGravity)
{
    fLocalPlayerGravity = fGravity;
}

void CMultiplayerSA::SetLocalStatValue(unsigned short usStat, float fValue)
{
    if (usStat < MAX_FLOAT_STATS)
        localStatsData.StatTypesFloat[usStat] = fValue;
    else if (usStat >= STATS_OFFSET && usStat < MAX_INT_FLOAT_STATS)
        localStatsData.StatTypesInt[usStat - STATS_OFFSET] = (int)fValue;
}

float CMultiplayerSA::GetLocalStatValue(unsigned short usStat)
{
    if (usStat < MAX_FLOAT_STATS)
        return localStatsData.StatTypesFloat[usStat];
    else if (usStat >= STATS_OFFSET && usStat < MAX_INT_FLOAT_STATS)
        return (float)localStatsData.StatTypesInt[usStat - STATS_OFFSET];
    return 0;
}

void CMultiplayerSA::SetLocalStatsStatic(bool bStatic)
{
    bLocalStatsStatic = bStatic;
}

void CMultiplayerSA::SetLocalCameraRotation(float fRotation)
{
    fLocalPlayerCameraRotation = fRotation;
}

bool CMultiplayerSA::IsCustomCameraRotationEnabled()
{
    return bCustomCameraRotation;
}

void CMultiplayerSA::SetCustomCameraRotationEnabled(bool bEnabled)
{
    bCustomCameraRotation = bEnabled;
}

void CMultiplayerSA::SetDebugVars(float f1, float f2, float f3)
{
}

void _declspec(naked) HOOK_CollisionStreamRead()
{
    if (*(DWORD*)VAR_CollisionStreamRead_ModelInfo)
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
void _declspec(naked) HOOK_CTrafficLights_GetPrimaryLightState()
{
    _asm
    {
        pushad
    }

    if (ucTrafficLightState == 0 || ucTrafficLightState == 5 || ucTrafficLightState == 8)
    {
        ucDesignatedLightState = 0;            // Green
    }
    else if (ucTrafficLightState == 1 || ucTrafficLightState == 6 || ucTrafficLightState == 7)
    {
        ucDesignatedLightState = 1;            // Amber
    }
    else if (ucTrafficLightState == 9)
    {
        ucDesignatedLightState = 4;            // Off
    }
    else ucDesignatedLightState = 2;            // Red

    _asm
    {
        popad
        mov al, ucDesignatedLightState
        retn
    }
}

void _declspec(naked) HOOK_CTrafficLights_GetSecondaryLightState()
{
    _asm
    {
        pushad
    }

    if (ucTrafficLightState == 3 || ucTrafficLightState == 5 || ucTrafficLightState == 7)
    {
        ucDesignatedLightState = 0;            // Green
    }
    else if (ucTrafficLightState == 4 || ucTrafficLightState == 6 || ucTrafficLightState == 8)
    {
        ucDesignatedLightState = 1;            // Amber
    }
    else if (ucTrafficLightState == 9)
    {
        ucDesignatedLightState = 4;            // Off
    }
    else ucDesignatedLightState = 2;            // Red

    _asm
    {
        popad
        mov al, ucDesignatedLightState
        retn
    }
}

void _declspec(naked) HOOK_CTrafficLights_DisplayActualLight()
{
    _asm
    {
        pushad
    }

    if (ucTrafficLightState == 2)
    {
        ucDesignatedLightState = 0;
    }
    else if (ucTrafficLightState == 9) { ucDesignatedLightState = 1; }
    else { ucDesignatedLightState = 2; }

    _asm
    {
        popad
        movzx eax, ucDesignatedLightState
        jmp RETURN_CTrafficLights_DisplayActualLight
    }
}

static CVehicleSAInterface* pHandlingDriveTypeVeh = NULL;
unsigned char               ucDriveType = '4';
void                        GetVehicleDriveType()
{
    // Get the car drive type from the Vehicle interface
    ucDriveType = static_cast<unsigned char>(pHandlingDriveTypeVeh->m_pVehicle->GetHandlingData()->GetCarDriveType());
}

static CTransmission* pCurTransmission = nullptr;
static ::byte*        pCurGear = nullptr;

void CheckVehicleMaxGear()
{
    if (*pCurGear > pCurTransmission->numOfGears)
    {
        *pCurGear = pCurTransmission->numOfGears;
    }
}

void _declspec(naked) HOOK_Transmission_CalculateDriveAcceleration()
{
    _asm
    {
        push eax
        mov pCurTransmission, ecx
        mov eax, [esp+0xC]
        mov pCurGear, eax
        pop eax
        pushad
    }

    CheckVehicleMaxGear();

    _asm
    {
        popad
        mov eax, [esp+0x10]
        mov edx, [eax]
        jmp RETURN_Transmission_CalculateDriveAcceleration
    }
}

void _declspec(naked) HOOK_isVehDriveTypeNotRWD()
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

void _declspec(naked) HOOK_isVehDriveTypeNotFWD()
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

unsigned char CMultiplayerSA::GetTrafficLightState()
{
    return ucTrafficLightState;
}

void CMultiplayerSA::SetTrafficLightState(unsigned char ucState)
{
    ucTrafficLightState = ucState;
}

bool CMultiplayerSA::GetTrafficLightsLocked()
{
    return bTrafficLightsBlocked;
}

void CMultiplayerSA::SetTrafficLightsLocked(bool bLocked)
{
    bTrafficLightsBlocked = bLocked;
}

// Allowing a created object into the vertical line test makes getGroundPosition, jetpacks and molotovs to work.
// Not allowing a created object into the vertical line test makes the breakable animation work.
void CMultiplayerSA::AllowCreatedObjectsInVerticalLineTest(bool bOn)
{
    static BYTE bufOriginalData[90] = {0};
    static bool bState = false;

    // Change required?
    if (bState != bOn)
    {
        // Done initialization?
        if (bufOriginalData[0] == 0)
            MemCpyFast(bufOriginalData, (void*)0x59FABC, 90);

        bState = bOn;
        if (bOn)
        {
            // Make created objects to have a control code, so they can be checked for vertical line test HOOK
            MemSet((void*)0x59FABC, 0x90, 90);
        }
        else
        {
            // Make created objects not be checked for vertical line test HOOK
            MemCpy((void*)0x59FABC, bufOriginalData, 90);
        }
    }
}

// ---------------------------------------------------

void _cdecl CPhysical_ApplyGravity(DWORD dwThis)
{
    DWORD dwType;
    _asm
    {
        mov ecx, dwThis
        mov eax, 0x46A2C0       // CEntity::GetType
        call eax
        mov dwType, eax
    }

    float fTimeStep = *(float*)0xB7CB5C;
    float fGravity = *(float*)0x863984;
    if (dwType == 2)
    {
        // It's a vehicle, use the gravity vector
        SClientEntity<CVehicleSA>* pVehicleClientEntity = pGameInterface->GetPools()->GetVehicle((DWORD*)dwThis);
        CVehicle*                  pVehicle = pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
        if (!pVehicle)
            return;

        CVector vecGravity, vecMoveSpeed;
        pVehicle->GetGravity(&vecGravity);
        pVehicle->GetMoveSpeed(&vecMoveSpeed);
        vecMoveSpeed += vecGravity * fTimeStep * fGravity;
        pVehicle->SetMoveSpeed(&vecMoveSpeed);
    }
    else
    {
        // It's something else, apply regular downward gravity (+0x4C == m_vecMoveSpeed.fZ)
        MemSubFast<float>(dwThis + 0x4C, fTimeStep * fGravity);
    }
}

const float kfTimeStepOrg = 5.0f / 3.0f;
void _declspec(naked) HOOK_CVehicle_ApplyBoatWaterResistance()
{
    _asm
    {
        fmul    ds : 0x871DDC   // Original constant used in code
        fmul    ds : 0xB7CB5C   // Multiply by current timestep
        fdiv    kfTimeStepOrg   // Divide by desired timestep, used at 30fps
        jmp     RETURN_CVehicle_ApplyBoatWaterResistance
    }
}

void _declspec(naked) HOOK_CPhysical_ApplyGravity()
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

void GetMatrixForGravity(const CVector& vecGravity, CMatrix& mat)
{
    // Calculates a basis where the z axis is the inverse of the gravity
    if (vecGravity.Length() > 0.0001f)
    {
        mat.vUp = -vecGravity;
        mat.vUp.Normalize();
        if (fabs(mat.vUp.fX) > 0.0001f || fabs(mat.vUp.fZ) > 0.0001f)
        {
            CVector y(0.0f, 1.0f, 0.0f);
            mat.vFront = vecGravity;
            mat.vFront.CrossProduct(&y);
            mat.vFront.CrossProduct(&vecGravity);
            mat.vFront.Normalize();
        }
        else
        {
            mat.vFront = CVector(0.0f, 0.0f, vecGravity.fY);
        }
        mat.vRight = mat.vFront;
        mat.vRight.CrossProduct(&mat.vUp);
    }
    else
    {
        // No gravity, use default axes
        mat.vRight = CVector(1.0f, 0.0f, 0.0f);
        mat.vFront = CVector(0.0f, 1.0f, 0.0f);
        mat.vUp = CVector(0.0f, 0.0f, 1.0f);
    }
}

// ---------------------------------------------------

CMatrix gravcam_matGravity;
CMatrix gravcam_matInvertGravity;
CMatrix gravcam_matVehicleTransform;
CVector gravcam_vecVehicleVelocity;

bool _cdecl VehicleCamStart(DWORD dwCam, DWORD pVehicleInterface)
{
    // Inverse transform some things so that they match a downward pointing gravity.
    // This way SA's gravity-goes-downward assumptive code can calculate the camera
    // spherical coords correctly. Of course we restore these after the camera function
    // completes.
    SClientEntity<CVehicleSA>* pVehicleClientEntity = pGameInterface->GetPools()->GetVehicle((DWORD*)pVehicleInterface);
    CVehicle*                  pVehicle = pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
    if (!pVehicle)
        return false;

    CVector vecGravity;
    pVehicle->GetGravity(&vecGravity);

    GetMatrixForGravity(vecGravity, gravcam_matGravity);
    gravcam_matInvertGravity = gravcam_matGravity;
    gravcam_matInvertGravity.Invert();

    pVehicle->GetMatrix(&gravcam_matVehicleTransform);
    CMatrix matVehicleInverted = gravcam_matInvertGravity * gravcam_matVehicleTransform;
    matVehicleInverted.vPos = gravcam_matVehicleTransform.vPos;
    pVehicle->SetMatrix(&matVehicleInverted);

    pVehicle->GetMoveSpeed(&gravcam_vecVehicleVelocity);
    CVector vecVelocityInverted = gravcam_matInvertGravity * gravcam_vecVehicleVelocity;
    pVehicle->SetMoveSpeed(&vecVelocityInverted);
    return true;
}

void _declspec(naked) HOOK_VehicleCamStart()
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

void _cdecl VehicleCamTargetZTweak(CVector* pvecCamTarget, float fTargetZTweak)
{
    // Replacement for "vecCamTarget = vecCarPosition + (0, 0, 1)*fZTweak"
    *pvecCamTarget += gravcam_matGravity.vUp * fTargetZTweak;
}

void _declspec(naked) HOOK_VehicleCamTargetZTweak()
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

void _cdecl VehicleCamLookDir1(DWORD dwCam, DWORD pVehicleInterface)
{
    // For the same reason as in VehicleCamStart, inverse transform the camera's lookdir
    // at this point
    CVector* pvecLookDir = (CVector*)(dwCam + 0x190);
    *pvecLookDir = gravcam_matInvertGravity * (*pvecLookDir);
}

void _declspec(naked) HOOK_VehicleCamLookDir1()
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

bool _cdecl VehicleCamLookDir2(DWORD dwCam)
{
    // Calculates the look direction vector for the vehicle camera. This vector
    // is later multiplied by a factor and added to the vehicle position by SA
    // to obtain the final camera position.
    float fPhi = *(float*)(dwCam + 0xBC);
    float fTheta = *(float*)(dwCam + 0xAC);

    MemPutFast<CVector>(dwCam + 0x190, -gravcam_matGravity.vRight * cos(fPhi) * cos(fTheta) - gravcam_matGravity.vFront * sin(fPhi) * cos(fTheta) +
                                           gravcam_matGravity.vUp * sin(fTheta));

    MemPutFast<float>(0x8CCEA8, fPhi);
    return true;
}

void _declspec(naked) HOOK_VehicleCamLookDir2()
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

void _cdecl VehicleCamHistory(DWORD dwCam, CVector* pvecTarget, float fTargetTheta, float fRadius, float fZoom)
{
    float   fPhi = *(float*)(dwCam + 0xBC);
    CVector vecDir = -gravcam_matGravity.vRight * cos(fPhi) * cos(fTargetTheta) - gravcam_matGravity.vFront * sin(fPhi) * cos(fTargetTheta) +
                     gravcam_matGravity.vUp * sin(fTargetTheta);
    ((CVector*)(dwCam + 0x1D8))[0] = *pvecTarget - vecDir * fRadius;
    ((CVector*)(dwCam + 0x1D8))[1] = *pvecTarget - vecDir * fZoom;
}

void _declspec(naked) HOOK_VehicleCamHistory()
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

void _cdecl VehicleCamUp(DWORD dwCam)
{
    // Calculates the up vector for the vehicle camera.
    CVector* pvecUp = (CVector*)(dwCam + 0x1B4);
    CVector* pvecLookDir = (CVector*)(dwCam + 0x190);

    pvecLookDir->Normalize();
    *pvecUp = *pvecLookDir;
    pvecUp->CrossProduct(&gravcam_matGravity.vUp);
    pvecUp->CrossProduct(pvecLookDir);
    pvecUp->Normalize();
}

void _declspec(naked) HOOK_VehicleCamUp()
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

void _cdecl VehicleCamEnd(DWORD pVehicleInterface)
{
    // Restore the things that we inverse transformed in VehicleCamStart
    SClientEntity<CVehicleSA>* pVehicleClientEntity = pGameInterface->GetPools()->GetVehicle((DWORD*)pVehicleInterface);
    CVehicle*                  pVehicle = pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
    if (!pVehicle)
        return;

    pVehicle->SetMatrix(&gravcam_matVehicleTransform);
    pVehicle->SetMoveSpeed(&gravcam_vecVehicleVelocity);
}

void _declspec(naked) HOOK_VehicleCamEnd()
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

void _cdecl VehicleLookBehind(DWORD dwCam, CVector* pvecEntityPos, float fDistance)
{
    // Custom calculation of the camera position when looking behind while in
    // vehicle cam mode, taking in account custom gravity
    MemPutFast<CVector>(dwCam + 0x19C, *pvecEntityPos + (gravcam_matVehicleTransform.vFront + gravcam_matGravity.vUp * 0.2f) * fDistance);
}

void _declspec(naked) HOOK_VehicleLookBehind()
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

void _cdecl VehicleLookAside(DWORD dwCam, CVector* pvecEntityPos, float fDirectionFactor, float fDistance)
{
    // Custom calculation of the camera position when looking left/right while in
    // vehicle cam mode, taking in account custom gravity
    MemPutFast<CVector>(dwCam + 0x19C, *pvecEntityPos + (-gravcam_matVehicleTransform.vRight * fDirectionFactor + gravcam_matGravity.vUp * 0.2f) * fDistance);
}

void _declspec(naked) HOOK_VehicleLookAside()
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

float _cdecl VehicleBurnCheck(DWORD pVehicleInterface)
{
    // To check if a vehicle is lying upside down on its roof, SA checks if the z coordinate
    // of the vehicle's up vector is negative. We replace this z by the dot product of the up vector
    // and the negated gravity vector.
    SClientEntity<CVehicleSA>* pVehicleClientEntity = pGameInterface->GetPools()->GetVehicle((DWORD*)pVehicleInterface);
    CVehicle*                  pVehicle = pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
    if (!pVehicle)
        return 1.0f;

    CVector vecGravity;
    CMatrix matVehicle;
    pVehicle->GetGravity(&vecGravity);
    pVehicle->GetMatrix(&matVehicle);
    vecGravity = -vecGravity;
    return matVehicle.vUp.DotProduct(&vecGravity);
}

void _declspec(naked) HOOK_OccupiedVehicleBurnCheck()
{
    _asm
    {
        push eax
        call VehicleBurnCheck
        add esp, 4
        jmp RETURN_OccupiedVehicleBurnCheck
    }
}

void _declspec(naked) HOOK_UnoccupiedVehicleBurnCheck()
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

void _cdecl ApplyVehicleBlowHop(DWORD pVehicleInterface)
{
    // Custom application of the little jump that vehicles make when they blow up,
    // taking into account custom gravity
    SClientEntity<CVehicleSA>* pVehicleClientEntity = pGameInterface->GetPools()->GetVehicle((DWORD*)pVehicleInterface);
    CVehicle*                  pVehicle = pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
    if (!pVehicle)
        return;

    CVector vecGravity, vecVelocity;
    pVehicle->GetGravity(&vecGravity);
    pVehicle->GetMoveSpeed(&vecVelocity);
    vecVelocity -= vecGravity * 0.13f;
    pVehicle->SetMoveSpeed(&vecVelocity);
}

void _declspec(naked) HOOK_ApplyCarBlowHop()
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
void _declspec(naked) HOOK_CGame_Process()
{
    _asm
    {
        pushad
    }

    TIMING_CHECKPOINT("+CWorld_Process");
    if (m_pPreWorldProcessHandler)
        m_pPreWorldProcessHandler();

    _asm
    {
        popad
        call    CALL_CWorld_Process
        mov     ecx, 0B72978h
        pushad
    }

    if (m_pPostWorldProcessHandler) m_pPostWorldProcessHandler();

    TIMING_CHECKPOINT("-CWorld_Process");

    _asm
    {
        popad
        jmp     RETURN_CGame_Process;
    }
}

void __cdecl HandleIdle()
{
    static bool bAnimGroupArrayAddressLogged = false;
    if (!bAnimGroupArrayAddressLogged)
    {
        bAnimGroupArrayAddressLogged = true;
        DWORD dwAnimGroupArrayAddress = 0xb4ea34;
        LogEvent(567, "aAnimAssocGroups", "CAnimManager::ms_aAnimAssocGroups Address",
                 SString("CAnimManager::ms_aAnimAssocGroups = %#.8x", *(DWORD*)dwAnimGroupArrayAddress), 567);
    }
    m_pIdleHandler();
}

DWORD CALL_CGame_Process = 0x53BEE0;
void _declspec(naked) HOOK_Idle()
{
    TIMING_CHECKPOINT("+CGame_Process");
    _asm
    {
        call    CALL_CGame_Process
        pushad
    }

    TIMING_CHECKPOINT("-CGame_Process");

    TIMING_CHECKPOINT("+Idle");
    if (m_pIdleHandler)
        HandleIdle();
    TIMING_CHECKPOINT("-Idle");

    _asm
    {
        popad
        mov     ecx, 0B6BC90h
        jmp     RETURN_Idle
    }
}

// Hooked from 0049E650 5 bytes
void _declspec(naked) HOOK_PreFxRender()
{
    _asm
    {
        pushad
        mov     eax,[esp+32+4*2]
        cmp     eax,0
        jne skip
    }

    if (m_pPreFxRenderHandler) m_pPreFxRenderHandler();

    _asm
    {
skip:
        popad
        jmp     RETURN_PreFxRender  // 00404D1E
    }
}

// Hooked from 00705099  5 bytes
void _declspec(naked) HOOK_PostColorFilterRender()
{
    _asm
    {
        pushad
    }

    if (m_pPostColorFilterRenderHandler) m_pPostColorFilterRenderHandler();

    _asm
    {
        popad
        mov al, ds:0C402BAh
        jmp     RETURN_PostColorFilterRender  // 0070509E
    }
}

// Hooked from 0053EAD8  5 bytes
void _declspec(naked) HOOK_PreHUDRender()
{
    _asm
    {
        pushad
    }

    if (m_pPreHudRenderHandler) m_pPreHudRenderHandler();

    _asm
    {
        popad
        mov     eax, ds:0B6F0B8h
        jmp     RETURN_PreHUDRender  // 0053EADD
    }
}

// ---------------------------------------------------

#define ENABLE_VEHICLE_HEADLIGHT_COLOR 1

#if !ENABLE_VEHICLE_HEADLIGHT_COLOR

// Dummy stub
void vehicle_lights_init()
{
}

#else

#define HOOKPOS_CVehicle_DoVehicleLights                    0x6e1a60
DWORD RETURN_CVehicle_DoVehicleLights = 0x6e1a68;

#define HOOKPOS_CVehicle_DoHeadLightBeam_1                  0x6E0E20
DWORD RETURN_CVehicle_DoHeadLightBeam_1 = 0x6E0E26;

#define HOOKPOS_CVehicle_DoHeadLightBeam_2                  0x6E13A4
DWORD RETURN_CVehicle_DoHeadLightBeam_2 = 0x6E13AE;

#define HOOKPOS_CVehicle_DoHeadLightEffect_1                0x6E0D01
DWORD RETURN_CVehicle_DoHeadLightEffect_1 = 0x6E0D09;

#define HOOKPOS_CVehicle_DoHeadLightEffect_2                0x6E0DF7
DWORD RETURN_CVehicle_DoHeadLightEffect_2 = 0x6E0DFF;

#define HOOKPOS_CVehicle_DoHeadLightReflectionTwin          0x6E170F
DWORD RETURN_CVehicle_DoHeadLightReflectionTwin = 0x6E1717;

#define HOOKPOS_CVehicle_DoHeadLightReflectionSingle        0x6E15E2
DWORD RETURN_CVehicle_DoHeadLightReflectionSingle = 0x6E15EA;

void HOOK_CVehicle_DoVehicleLights();
void HOOK_CVehicle_DoHeadLightBeam_1();
void HOOK_CVehicle_DoHeadLightBeam_2();
void HOOK_CVehicle_DoHeadLightEffect_1();
void HOOK_CVehicle_DoHeadLightEffect_2();
void HOOK_CVehicle_DoHeadLightReflectionTwin();
void HOOK_CVehicle_DoHeadLightReflectionSingle();

void vehicle_lights_init()
{
    HookInstall(HOOKPOS_CVehicle_DoVehicleLights, (DWORD)HOOK_CVehicle_DoVehicleLights, 8);
    HookInstall(HOOKPOS_CVehicle_DoHeadLightBeam_1, (DWORD)HOOK_CVehicle_DoHeadLightBeam_1, 6);
    HookInstall(HOOKPOS_CVehicle_DoHeadLightBeam_2, (DWORD)HOOK_CVehicle_DoHeadLightBeam_2, 10);
    HookInstall(HOOKPOS_CVehicle_DoHeadLightEffect_1, (DWORD)HOOK_CVehicle_DoHeadLightEffect_1, 8);
    HookInstall(HOOKPOS_CVehicle_DoHeadLightEffect_2, (DWORD)HOOK_CVehicle_DoHeadLightEffect_2, 8);
    HookInstall(HOOKPOS_CVehicle_DoHeadLightReflectionTwin, (DWORD)HOOK_CVehicle_DoHeadLightReflectionTwin, 8);
    HookInstall(HOOKPOS_CVehicle_DoHeadLightReflectionSingle, (DWORD)HOOK_CVehicle_DoHeadLightReflectionSingle, 8);

    // Allow turning on vehicle lights even if the engine is off
    MemSet((void*)0x6E1DBC, 0x90, 8);

    // Fix vehicle back lights both using light state 3 (SA bug)
    MemPut<BYTE>(0x6E1D4F, 2);
}

CVehicleSAInterface* pLightsVehicleInterface = NULL;
void _declspec(naked) HOOK_CVehicle_DoVehicleLights()
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
void          CVehicle_GetHeadLightColor(CVehicleSAInterface* pInterface, float fR, float fG, float fB)
{
    SColor                     color = SColorRGBA(255, 255, 255, 255);
    SClientEntity<CVehicleSA>* pVehicleClientEntity = pGameInterface->GetPools()->GetVehicle((DWORD*)pInterface);
    CVehicle*                  pVehicle = pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
    if (pVehicle)
    {
        color = pVehicle->GetHeadLightColor();
    }

    // Scale our color values to the defaults ..looks dodgy but its needed!
    ulHeadLightR = (unsigned char)std::min(255.f, color.R * (1 / 255.0f) * fR);
    ulHeadLightG = (unsigned char)std::min(255.f, color.G * (1 / 255.0f) * fG);
    ulHeadLightB = (unsigned char)std::min(255.f, color.B * (1 / 255.0f) * fB);
}

CVehicleSAInterface* pHeadLightBeamVehicleInterface = NULL;
void _declspec(naked) HOOK_CVehicle_DoHeadLightBeam_1()
{
    _asm
    {
        mov     pHeadLightBeamVehicleInterface, ecx
        sub     esp, 94h
        jmp     RETURN_CVehicle_DoHeadLightBeam_1
    }
}

RwVertex*    pHeadLightVerts = NULL;
unsigned int uiHeadLightNumVerts = 0;
void         CVehicle_DoHeadLightBeam()
{
    // 255, 255, 255
    CVehicle_GetHeadLightColor(pHeadLightBeamVehicleInterface, 255.0f, 255.0f, 255.0f);

    for (unsigned int i = 0; i < uiHeadLightNumVerts; i++)
    {
        unsigned char alpha = COLOR_ARGB_A(pHeadLightVerts[i].color);
        pHeadLightVerts[i].color = COLOR_ARGB(alpha, (unsigned char)ulHeadLightR, (unsigned char)ulHeadLightG, (unsigned char)ulHeadLightB);
    }
}

void _declspec(naked) HOOK_CVehicle_DoHeadLightBeam_2()
{
    _asm
    {
        mov     eax, [esp]
        mov     pHeadLightVerts, eax
        mov     eax, [esp+4]
        mov     uiHeadLightNumVerts, eax
        pushad
    }

    CVehicle_DoHeadLightBeam();

    _asm
    {
        popad
        mov     dword ptr ds:[0C4B950h],5
        jmp     RETURN_CVehicle_DoHeadLightBeam_2
    }
}

DWORD dwCCoronas_RegisterCorona = 0x6FC580;
void _declspec(naked) HOOK_CVehicle_DoHeadLightEffect_1()
{
    _asm
    {
        pushad
    }

    CVehicle_GetHeadLightColor(pLightsVehicleInterface, 160.0f, 160.0f, 140.0f);

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

void _declspec(naked) HOOK_CVehicle_DoHeadLightEffect_2()
{
    _asm
    {
        pushad
    }

    CVehicle_GetHeadLightColor(pLightsVehicleInterface, 160.0f, 160.0f, 140.0f);

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
void _declspec(naked) HOOK_CVehicle_DoHeadLightReflectionTwin()
{
    _asm
    {
        pushad
    }

    CVehicle_GetHeadLightColor(pLightsVehicleInterface, 45.0f, 45.0f, 45.0f);

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

void _declspec(naked) HOOK_CVehicle_DoHeadLightReflectionSingle()
{
    _asm
    {
        pushad
    }

    CVehicle_GetHeadLightColor(pLightsVehicleInterface, 45.0f, 45.0f, 45.0f);

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

void _declspec(naked) HOOK_CWorld_SetWorldOnFire()
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

void _declspec(naked) HOOK_CTaskSimplePlayerOnFire_ProcessPed()
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

void _declspec(naked) HOOK_CFire_ProcessFire()
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

void _declspec(naked) HOOK_CExplosion_Update()
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

void _declspec(naked) HOOK_CWeapon_FireAreaEffect()
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
void CMultiplayerSA::SetAltWaterOrderEnabled(bool bEnable)
{
    // Switch
    if (m_bEnabledAltWaterOrder == bEnable)
        return;
    m_bEnabledAltWaterOrder = bEnable;

    // Memory saved here
    static CBuffer savedMem;
    struct
    {
        DWORD dwAddress;
        uint  uiSize;
    } memoryList[] = {
        {0x53DFF5, 1},
        {0x53E133, 2},
        {0x53E132, 1},
        {0x53E156, 3},
        {0x53DF4B, 4},
        {HOOKPOS_RenderScene_end, 5},
        {HOOKPOS_CPlantMgr_Render, 6},
        {CALL_RenderScene_Plants, 5},
    };

    // Enable or not?
    if (bEnable)
    {
        // Save memory before we blat it
        CBufferWriteStream stream(savedMem);
        for (uint i = 0; i < NUMELMS(memoryList); i++)
            stream.WriteBytes((void*)memoryList[i].dwAddress, memoryList[i].uiSize);

        // Add hooks and things
        // Always render water after other entities (otherwise underwater LODs and trees are rendered
        // in front of it)
        MemPut<BYTE>(0x53DFF5, 0xEB);
        MemPut<WORD>(0x53E133, 0x9090);
        // Disable some stack management instructions as we need ebx for a bit longer. We replicate
        // these in HOOK_RenderScene_end
        MemPut<BYTE>(0x53E132, 0x90);
        MemSet((void*)0x53E156, 0x90, 3);
        // Use 0.5 instead of 0.0 for underwater threshold
        MemPut<DWORD>(0x53DF4B, 0x858B8C);

        HookInstall(HOOKPOS_RenderScene_end, (DWORD)HOOK_RenderScene_end, 5);
        HookInstall(HOOKPOS_CPlantMgr_Render, (DWORD)HOOK_CPlantMgr_Render, 6);
        HookInstallCall(CALL_RenderScene_Plants, (DWORD)HOOK_RenderScene_Plants);
    }
    else
    {
        // Restore memory
        CBufferReadStream stream(savedMem);
        for (uint i = 0; i < NUMELMS(memoryList); i++)
        {
            BYTE temp[10];
            assert(sizeof(temp) >= memoryList[i].uiSize);
            stream.ReadBytes(temp, memoryList[i].uiSize);
            MemCpy((void*)memoryList[i].dwAddress, temp, memoryList[i].uiSize);
        }
    }
}

//
// Notify core when rendering grass so we can do optimal things
//
void CPlantMgr_Render_Pre()
{
    g_pCore->NotifyRenderingGrass(true);
}

void CPlantMgr_Render_Post()
{
    g_pCore->NotifyRenderingGrass(false);
}

// The purpose of these hooks is to divide plant (grass) rendering in two:
// rather than render *all* grass before or after the water like SA does, we render
// underwater plants before the water and above-water plants after. This way, we have
// at the same time that underwater plants do not pop up in front of water,
// and water is not drawn in front of above-water plants (eg if you're looking at a
// lake through some high grass).

void _declspec(naked) HOOK_RenderScene_Plants()
{
    _asm
    {
        pushad
        call    CPlantMgr_Render_Pre
        popad

        push 1                  // bRenderingBeforeWater
        movzx eax, bl           // bCamBelowWater
        push eax
        mov eax, 0x5DBAE0       // CPlantMgr::Render
        call eax
        add esp, 8

        pushad
        call    CPlantMgr_Render_Post
        popad
        ret
    }
}

void _declspec(naked) HOOK_RenderScene_end()
{
    _asm
    {
        pushad
        call    CPlantMgr_Render_Pre
        popad

        push 0                  // bRenderingBeforeWater
        movzx eax, bl           // bCamBelowWater
        push eax
        mov eax, 0x5DBAE0       // CPlantMgr::Render
        call eax
        add esp, 8

        pushad
        call    CPlantMgr_Render_Post
        popad

        pop ebx
        add esp, 8
        mov eax, 0x7113B0
        jmp eax
    }
}

bool _cdecl IsPlantBelowWater(float fPlantZ, float fWaterZ)
{
    return fPlantZ + 2.0f < fWaterZ;
}

void _declspec(naked) HOOK_CPlantMgr_Render()
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
        // NOTE: Causes some foliage not generating in certain places when uncommented (see also: PR #2679)
        // xor eax, [esp+0x88+4]   // Decide whether or not to draw the plant right now
        // cmp eax, [esp+0x88+8]
        // jnz fail

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
CEventDamageSAInterface* pBikeDamageInterface;
CPedSAInterface*         pBikePedInterface;
float                    fBikeDamage;
void                     CEventHandler_ComputeKnockOffBikeResponse()
{
    CEventDamage*          pEvent = pGameInterface->GetEventList()->GetEventDamage(pBikeDamageInterface);
    SClientEntity<CPedSA>* pPedClientEntity = pGameInterface->GetPools()->GetPed((DWORD*)pBikePedInterface);
    CPed*                  pPed = pPedClientEntity ? pPedClientEntity->pEntity : nullptr;
    if (pEvent && pPed)
    {
        CPedDamageResponse* pResponse = pEvent->GetDamageResponse();
        if (pResponse)
        {
            pResponse->Calculate((CEntity*)pPed, fBikeDamage, pEvent->GetWeaponUsed(), pEvent->GetPedPieceType(), true, false);
        }
    }
    if (pEvent)
        pEvent->Destroy();
}

DWORD dw_CEventDamage_AffectsPed = 0x4b35a0;
void _declspec(naked) HOOK_CEventHandler_ComputeKnockOffBikeResponse()
{
    _asm
    {
        mov     pBikeDamageInterface, ecx
        mov     pBikePedInterface, edx
        mov     eax, [edi+40]
        mov     fBikeDamage, eax

        pushad
    }
    CEventHandler_ComputeKnockOffBikeResponse();

    _asm
    {
        popad
        call    dw_CEventDamage_AffectsPed
        jmp     RETURN_CEventHandler_ComputeKnockOffBikeResponse
    }
}

CPedSAInterface* weaponSkillPed;
eWeaponType      weaponSkillWeapon;
BYTE             weaponSkill;
bool             CPed_GetWeaponSkill()
{
    SClientEntity<CPedSA>* pPedClientEntity = pGameInterface->GetPools()->GetPed((DWORD*)weaponSkillPed);
    CPed*                  pPed = pPedClientEntity ? pPedClientEntity->pEntity : nullptr;
    if (pPed)
    {
        CPed* pLocalPlayerPed = pGameInterface->GetPools()->GetPedFromRef((DWORD)1);
        if (pPed != pLocalPlayerPed)
        {
            if (weaponSkillWeapon >= WEAPONTYPE_PISTOL && weaponSkillWeapon <= WEAPONTYPE_TEC9)
            {
                CPlayerPed* playerPed = dynamic_cast<CPlayerPed*>(pPed);
                if (playerPed)
                {
                    CRemoteDataStorageSA* data = CRemoteDataSA::GetRemoteDataStorage(playerPed);
                    if (data)
                    {
                        float stat = data->m_stats.StatTypesFloat[pGameInterface->GetStats()->GetSkillStatIndex(weaponSkillWeapon)];

                        CWeaponInfo* pPoor = pGameInterface->GetWeaponInfo(weaponSkillWeapon, WEAPONSKILL_POOR);
                        CWeaponInfo* pStd = pGameInterface->GetWeaponInfo(weaponSkillWeapon, WEAPONSKILL_STD);
                        CWeaponInfo* pPro = pGameInterface->GetWeaponInfo(weaponSkillWeapon, WEAPONSKILL_PRO);

                        if (stat >= pPro->GetRequiredStatLevel())
                            weaponSkill = WEAPONSKILL_PRO;
                        else if (stat >= pStd->GetRequiredStatLevel())
                            weaponSkill = WEAPONSKILL_STD;
                        else
                            weaponSkill = WEAPONSKILL_POOR;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void _declspec(naked) HOOK_CPed_GetWeaponSkill()
{
    _asm
    {
        mov     weaponSkillPed, ecx
        mov     eax, [esp+4]
        mov     weaponSkillWeapon, eax
        pushad
    }

    if (CPed_GetWeaponSkill())
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
bool _cdecl CPed_AddGogglesModelCheck(void* pPedInterface)
{
    SClientEntity<CPedSA>* pPedClientEntity = pGameInterface->GetPools()->GetPed((DWORD*)pPedInterface);
    CPed*                  pPed = pPedClientEntity ? pPedClientEntity->pEntity : nullptr;
    return pPed == pGameInterface->GetPools()->GetPedFromRef(1);
}

void _declspec(naked) HOOK_CPed_AddGogglesModel()
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

void CMultiplayerSA::DeleteAndDisableGangTags()
{
    static bool bDisabled = false;
    if (!bDisabled)
    {
        bDisabled = true;

        // Destroy all the world tags
        DWORD dwFunc = FUNC_CWorld_Remove;

        for (unsigned int i = 0; i < *VAR_NumTags; ++i)
        {
            DWORD* pTagInterface = VAR_TagInfoArray[i << 1];
            if (pTagInterface)
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
        _asm
        {
            call dwFunc
        }

        // Disallow spraying gang tags
        // Nop the whole CTagManager::IsTag function and replace its body with:
        // xor eax, eax
        // ret
        // to make it always return false
        MemSet((void*)0x49CCE0, 0x90, 74);
        MemPut<DWORD>(0x49CCE0, 0x90C3C033);
        // Remove also some hardcoded and inlined checks for if it's a tag
        MemSet((void*)0x53374A, 0x90, 56);
        MemPut<BYTE>(0x4C4403, 0xEB);

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
        MemSet((void*)0x49CE58, 0x90, 5);
        MemSet((void*)0x49CE5E, 0x90, 11);
        MemPut<unsigned short>(0x49CE5E, 0xC033);
        MemPut<unsigned short>(0x49CE60, 0xFF33);
    }
}

CPhysicalSAInterface *pCollisionPhysicalThis, *pCollisionPhysical;
bool                  CPhysical_ProcessCollisionSectorList()
{
    if (pCollisionPhysicalThis && pCollisionPhysical)
    {
        if (m_pProcessCollisionHandler && !m_pProcessCollisionHandler(pCollisionPhysicalThis, pCollisionPhysical))
        {
            return false;
        }
    }
    return true;
}

void _declspec(naked) HOOK_CPhysical_ProcessCollisionSectorList()
{
    _asm
    {
        mov     pCollisionPhysicalThis, esi
        mov     pCollisionPhysical, edi
        pushad
    }

    // Carry on with collision? (sets the CElement->bUsesCollision flag check)
    if (CPhysical_ProcessCollisionSectorList())
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

// Ped animation matrix array gets corrupted sometimes by unknown thing
// Hack fix for now is to validate each matrix before it is used
void _cdecl CheckMatrix(float* pMatrix)
{
    // Peek at IEEE 754 float data to quickly check if any element is outside range of -2 to 2 or is NaN
    int* p = (int*)pMatrix;
    int  RotBits = p[0] | p[1] | p[2] | p[4] | p[5] | p[6] | p[8] | p[9] | p[10];

    int PosBits = p[12] | p[13] | p[14];

    // If rotational part is outside -2 to 2 range, then flag fix
    bool bFix = (RotBits & 0x40000000) != 0;

    // If positional part is outside -2 to 2 range, then do further check for -10 to 10 range
    if (PosBits & 0x40000000)
    {
        for (uint i = 12; i < 15; i++)
        {
            float f = pMatrix[i];
            if (f < -10 || f > 10 || std::isnan(f))
                bFix = true;
        }
    }

    // Fix if required
    if (bFix)
    {
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
}

// hooked at 7C5A5C 5 bytes
void _declspec(naked) HOOK_CheckAnimMatrix()
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
        jmp     RETURN_CheckAnimMatrix      // 7C5A61
    }
}

static SColor vehColors[4];

void _cdecl SaveVehColors(DWORD dwThis)
{
    SClientEntity<CVehicleSA>* pVehicleClientEntity = pGameInterface->GetPools()->GetVehicle((DWORD*)dwThis);
    CVehicle*                  pVehicle = pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
    if (pVehicle)
    {
        pVehicle->GetColor(&vehColors[0], &vehColors[1], &vehColors[2], &vehColors[3], true);
    }
}

void _declspec(naked) HOOK_VehCol()
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

void _declspec(naked) HOOK_VehColCB()
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
static DWORD       dwSwingingDoorAutomobile;
static const DWORD dwSwingingRet1 = 0x6A9DB6;
static const DWORD dwSwingingRet2 = 0x6AA1DA;
static bool        AllowSwingingDoors()
{
    SClientEntity<CVehicleSA>* pVehicleClientEntity = pGameInterface->GetPools()->GetVehicle((DWORD*)dwSwingingDoorAutomobile);
    CVehicle*                  pVehicle = pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
    if (pVehicle == 0 || pVehicle->AreSwingingDoorsAllowed())
        return true;
    else
        return false;
}

void _declspec(naked) HOOK_CAutomobile__ProcessSwingingDoor()
{
    _asm
    {
        mov     dwSwingingDoorAutomobile, esi
        mov     ecx, [esi+eax*4+0x648]
        pushad
    }

    if (AllowSwingingDoors())
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

void* SetModelSuspensionLinesToVehiclePrivate(CVehicleSAInterface* pVehicleIntf)
{
    // Set the per-model suspension line data of the vehicle's model to the per-vehicle
    // suspension line data so that collision processing will use that instead.
    CVehicle*   pVehicle = pVehicleIntf->m_pVehicle;
    CModelInfo* pModelInfo = pGameInterface->GetModelInfo(pVehicle->GetModelIndex());
    return pModelInfo->SetVehicleSuspensionData(pVehicle->GetPrivateSuspensionLines());
}

void SetModelSuspensionLines(CVehicleSAInterface* pVehicleIntf, void* pSuspensionLines)
{
    CModelInfo* pModelInfo = pGameInterface->GetModelInfo(pVehicleIntf->m_pVehicle->GetModelIndex());
    pModelInfo->SetVehicleSuspensionData(pSuspensionLines);
}
// Some variables.
DWORD                dwSuspensionChangedJump = 0x4185C0;
bool                 bSuspensionChanged = false;
CVehicleSAInterface* pSuspensionInterface = NULL;
bool                 CheckHasSuspensionChanged()
{
    // Make sure we have a valid suspension interface
    if (pSuspensionInterface)
    {
        // Check our suspension interface has a valid vehicle and return the suspension changed marker
        CVehicle* pVehicle = pSuspensionInterface->m_pVehicle;
        if (!pVehicle)
            return false;

        CModelInfo* pModelInfo = pGameInterface->GetModelInfo(pVehicle->GetModelIndex());
        if (pModelInfo && (pModelInfo->IsCar() || pModelInfo->IsMonsterTruck()))
            return true;
        else
            return false;
    }
    else
        return false;
}
void _declspec(naked) HOOK_ProcessVehicleCollision()
{
    _asm
    {
        mov     pSuspensionInterface, esi
        pushad
    }

    if (CheckHasSuspensionChanged())
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

void CMultiplayerSA::UpdateVehicleSuspension() noexcept
{
    HookInstallCall(CALL_CAutomobile_ProcessEntityCollision, reinterpret_cast<DWORD>(HOOK_ProcessVehicleCollision));
    HookInstallCall(CALL_CMonsterTruck_ProcessEntityCollision, reinterpret_cast<DWORD>(HOOK_ProcessVehicleCollision));
}

// Variables
SIPLInst*           pEntityWorldAdd = NULL;
CEntitySAInterface* pLODInterface = NULL;
bool                bNextHookSetModel = false;
bool                bCodePathCheck = false;

bool CheckRemovedModelNoSet()
{
    // Init our variables
    bNextHookSetModel = false;
    bCodePathCheck = bNextHookSetModel;
    pLODInterface = NULL;
    CBuildingRemoval* pBuildingRemoval = pGameInterface->GetBuildingRemoval();
    // You never know.
    if (pBuildingRemoval)
    {
        // Is the model in question even removed?
        if (pBuildingRemoval->IsModelRemoved(pEntityWorldAdd->m_nModelIndex))
        {
            // is the replaced model in the spherical radius of any building removal
            if (pGameInterface->GetBuildingRemoval()->IsRemovedModelInRadius(pEntityWorldAdd))
            {
                // if it is next hook remove it from the world
                return true;
            }
        }
        return false;
    }
    return false;
}
// Binary
bool CheckRemovedModel()
{
    TIMING_CHECKPOINT("+CheckRemovedModel");
    bNextHookSetModel = CheckRemovedModelNoSet();
    TIMING_CHECKPOINT("-CheckRemovedModel");
    bCodePathCheck = true;
    return bNextHookSetModel;
}

// Binary
// Hook 1
void _declspec(naked) HOOK_LoadIPLInstance()
{
    _asm
    {
        pushad
        mov pEntityWorldAdd, ecx
    }
    if (pEntityWorldAdd)
    {
        CheckRemovedModel();
    }
    _asm
    {
        popad
        jmp CALL_LoadIPLInstance
        jmp RETURN_LoadIPLInstance
    }
}
static bool bTest = false;
// Binary
void HideEntitySomehow()
{
    TIMING_CHECKPOINT("+HideEntitySomehow");
    // Did we get instructed to set the model
    if (bNextHookSetModel && pLODInterface)
    {
        // Init pInterface with the Initial model
        CEntitySAInterface* pInterface = pLODInterface;
        // Grab the removal for the interface
        SBuildingRemoval* pBuildingRemoval = pGameInterface->GetBuildingRemoval()->GetBuildingRemoval(pInterface);
        // Remove down the LOD tree
        if (pBuildingRemoval && pInterface && pInterface != NULL && pInterface->bIsProcObject == 0 &&
            (pInterface->nType == ENTITY_TYPE_BUILDING || pInterface->nType == ENTITY_TYPE_DUMMY))
        {
            // Add the LOD to the list
            pBuildingRemoval->AddBinaryBuilding(pInterface);
            // Remove the model from the world
            pGameInterface->GetWorld()->Remove(pInterface, BuildingRemoval);
            // Get next LOD ( LOD's can have LOD's so we keep checking pInterface )
            // pInterface = pInterface->m_pLod;
        }
    }
    else if (bCodePathCheck && pLODInterface)
    {
        // Init pInterface with the Initial model
        CEntitySAInterface* pInterface = pLODInterface;
        if (pInterface && pInterface != NULL && pInterface->bIsProcObject == 0 &&
            (pInterface->nType == ENTITY_TYPE_BUILDING || pInterface->nType == ENTITY_TYPE_DUMMY))
        {
            pGameInterface->GetBuildingRemoval()->AddBinaryBuilding(pInterface);
        }
    }
    // Reset our next hook variable
    bNextHookSetModel = false;
    bCodePathCheck = bNextHookSetModel;
    TIMING_CHECKPOINT("-HideEntitySomehow");
}
// Binary
// Hook 2
void _declspec(naked) HOOK_CWorld_LOD_SETUP()
{
    _asm
    {
        pushad
        mov pLODInterface, esi
    }
    HideEntitySomehow();
    _asm
    {
        popad
        jmp CALL_CWorld_LODSETUP
    }
}

CEntitySAInterface* pBuildingAdd = NULL;
void                StorePointerToBuilding()
{
    if (pBuildingAdd != NULL)
    {
        pGameInterface->GetBuildingRemoval()->AddDataBuilding(pBuildingAdd);
    }
}

// Called when a data entity is added to the world (this happens once when the game loads so we just dump those in a list and we can sift through when someone
// tries to remove.)
void _declspec(naked) Hook_AddBuildingInstancesToWorld()
{
    _asm
    {
        pushad
        mov pBuildingAdd, edx
    }
    StorePointerToBuilding();
    _asm
    {
        popad
        jmp JMP_CWorld_Add_AddBuildingInstancesToWorld_CALL_CWorldAdd
    }
}

bool CheckForRemoval()
{
    // Did we get instructed to set the model
    if (pLODInterface)
    {
        // Init pInterface with the Initial model
        CEntitySAInterface* pInterface = pLODInterface;
        // Remove down the LOD tree
        if (pGameInterface->GetBuildingRemoval()->IsObjectRemoved(pInterface))
        {
            return true;
        }
    }
    return false;
}

// Call to CWorld::Add in CPopulation::ConvertToRealObject we just use this to get a list of pointers to valid objects for instant removal
void _declspec(naked) Hook_CWorld_ADD_CPopulation_ConvertToRealObject()
{
    _asm
    {
        pushad
        mov pBuildingAdd, esi
        mov pLODInterface, esi
    }
    StorePointerToBuilding();
    _asm
    {
        popad
        jmp JMP_CWorld_Add_CPopulation_ConvertToRealObject_CallCWorldAdd
        jmp JMP_CWorld_Add_CPopulation_ConvertToRealObject_Retn
    }
}

void RemoveObjectIfNeeded()
{
    TIMING_CHECKPOINT("+RemoveObjectIfNeeded");
    SBuildingRemoval* pBuildingRemoval = pGameInterface->GetBuildingRemoval()->GetBuildingRemoval(pLODInterface);
    if (pBuildingRemoval != NULL)
    {
        if ((DWORD)(pBuildingAdd->vtbl) != VTBL_CPlaceable)
        {
            pBuildingRemoval->AddDataBuilding(pBuildingAdd);
            pGameInterface->GetWorld()->Remove(pBuildingAdd, BuildingRemoval3);
        }

        if ((DWORD)(pLODInterface->vtbl) != VTBL_CPlaceable)
        {
            pBuildingRemoval->AddDataBuilding(pLODInterface);
            pGameInterface->GetWorld()->Remove(pLODInterface, BuildingRemoval4);
        }
    }
    TIMING_CHECKPOINT("-RemoveObjectIfNeeded");
}

// on stream in -> create and remove it from the world just after so we can restore easily
void _declspec(naked) HOOK_ConvertToObject_CPopulationManageDummy()
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
    RemoveObjectIfNeeded();
    _asm
    {
        popad
        jmp JMP_RETN_Cancel_CPopulation_ManageDummy
    }
}

CEntitySAInterface* pBuildingRemove = NULL;
void                RemovePointerToBuilding()
{
    if (pBuildingRemove->nType == ENTITY_TYPE_BUILDING || pBuildingRemove->nType == ENTITY_TYPE_DUMMY || pBuildingRemove->nType == ENTITY_TYPE_OBJECT)
    {
        pGameInterface->GetBuildingRemoval()->RemoveWorldBuildingFromLists(pBuildingRemove);
    }
}

DWORD dwCWorldRemove = 0x563280;
// Call to CWorld::Remove in CPopulation::ConvertToDummyObject this is called just before deleting a CObject so we remove the CObject while we are there and
// remove the new dummy if we need to do so before returning
void _declspec(naked) HOOK_CWorld_Remove_CPopulation_ConvertToDummyObject()
{
    _asm
    {
        pushad
        mov pBuildingRemove, esi
        mov pBuildingAdd, edi
        mov pLODInterface, edi
    }
    TIMING_CHECKPOINT("+RemovePointerToBuilding");
    RemovePointerToBuilding();
    StorePointerToBuilding();
    RemoveObjectIfNeeded();
    TIMING_CHECKPOINT("-RemovePointerToBuilding");
    _asm
    {
        popad
        jmp dwCWorldRemove
    }
}
// if it's replaced get rid of it
void RemoveDummyIfReplaced()
{
    SBuildingRemoval* pBuildingRemoval = pGameInterface->GetBuildingRemoval()->GetBuildingRemoval(pLODInterface);
    if (pBuildingRemoval != NULL)
    {
        if ((DWORD)(pBuildingAdd->vtbl) != VTBL_CPlaceable)
        {
            pBuildingRemoval->AddDataBuilding(pBuildingAdd);
            pGameInterface->GetWorld()->Remove(pBuildingAdd, BuildingRemoval5);
        }
    }
}

// Function that handles dummy -> object so we can cancel this process if need be
void _declspec(naked) HOOK_CWorld_Add_CPopulation_ConvertToDummyObject()
{
    _asm
    {
        pushad
        mov pLODInterface, edi
        mov pBuildingAdd, edi
    }

    TIMING_CHECKPOINT("+CheckForRemoval");
    StorePointerToBuilding();
    if (CheckForRemoval())
    {
        TIMING_CHECKPOINT("-CheckForRemoval");
        _asm
        {
            popad
            jmp JMP_RETN_Cancelled_CPopulation_ConvertToDummyObject
        }
    }
    else
    {
        TIMING_CHECKPOINT("-CheckForRemoval");
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
void _declspec(naked) Hook_CBuilding_DTR()
{
    _asm
    {
        pushad
        mov pBuildingRemove, ecx
    }
    RemovePointerToBuilding();
    _asm
    {
        popad
        jmp JMP_CBuilding_DTR
    }
}

void _declspec(naked) Hook_CDummy_DTR()
{
    _asm
    {
        pushad
        mov pBuildingRemove, ecx
    }
    RemovePointerToBuilding();
    _asm
    {
        popad
        jmp JMP_CDummy_DTR
    }
}

DWORD dwObjectVtbl = 0x866F60;
void _declspec(naked) Hook_CObject_DTR()
{
    _asm
    {
        pushad
        mov pBuildingRemove, esi
    }
    RemovePointerToBuilding();
    _asm
    {
        popad
        mov dword ptr [esi], offset dwObjectVtbl
        jmp JMP_CObject_DTR
    }
}

static DWORD dwEntityVtbl;
static DWORD dwMultResult;
void _declspec(naked) HOOK_CEntity_IsOnScreen_FixObjectScale()
{
    _asm
    {
        push    0xB6FA74

        pushad
        mov     eax, [esi]
        mov     dwEntityVtbl, eax
    }

    if (dwEntityVtbl == 0x866F60) goto IsOnScreen_IsObject;

    _asm
    {
        popad
        mov     esi, ecx
        jmp     JMP_CEntity_IsOnScreen_FixObjectsScale
    }

IsOnScreen_IsObject:
    _asm
    {
        popad
        fld     [eax+0x24]
        fld     [esi+0x15C]
        fmulp   st(1), st(0)
        fstp    dwMultResult
        mov     esi, dwMultResult
        jmp     JMP_CEntity_IsOnScreen_FixObjectsScale
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
// Only allow rebuild player on CJ - Stops other models getting corrupted (spider CJ)
// hooked at 5A82C0 8 bytes
void _declspec(naked) HOOK_CClothes_RebuildPlayer()
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

void _declspec(naked) HOOK_CProjectileInfo_Update_FindLocalPlayer_FindLocalPlayerVehicle()
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

void CMultiplayerSA::SetAutomaticVehicleStartupOnPedEnter(bool bSet)
{
    static BYTE originalCode[6] = {0};
    if (originalCode[0] == '\0')
        MemCpyFast(&originalCode[0], (const void*)0x64BC0D, 6);

    if (bSet)
        MemCpy((char*)0x64BC0D, originalCode, 6);
    else
        MemSet((char*)0x64BC0D, 0x90, 6);
}

// Storage
CVehicleSAInterface* pHeliKiller = NULL;
CEntitySAInterface*  pHitByHeli = NULL;
bool                 CallHeliKillEvent()
{
    // Is our handler alive
    if (m_pHeliKillHandler)
    {
        // Return our handlers return
        return m_pHeliKillHandler(pHeliKiller, pHitByHeli);
    }
    // Return true else
    return true;
}

void _declspec(naked) HOOK_CHeli_ProcessHeliKill()
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
        pushfd
        pushad
        mov pHeliKiller, esi
        mov pHitByHeli, edi
    }
    //   Call our event
    if (CallHeliKillEvent() == false)
    {
        _asm
        {
            popad
            popfd
            // Go to the end of the while loop and let it start again
            jmp RETURN_CHeli_ProcessHeliKill_RETN_Cancel
        }
    }
    else
    {
        _asm
        {
            popad
            popfd
            // do our JNZ
            jnz lp1
            // if it failed do our continue
            jmp RETURN_CHeli_ProcessHeliKill_RETN_Cont_Zero

lp1:        jmp RETURN_CHeli_ProcessHeliKill_6DB437h
        }
    }
}

CObjectSAInterface* pDamagedObject = NULL;
CEntitySAInterface* pObjectAttacker = NULL;
float               fNewObjectHealth = NULL;
bool                bObjectDamaged = true;

bool TriggerObjectDamageEvent()
{
    if (m_pObjectDamageHandler && pDamagedObject && fNewObjectHealth)
    {
        float fHealth = *(float*)((DWORD)pDamagedObject + 340);
        float fLoss = fHealth - fNewObjectHealth;

        return m_pObjectDamageHandler(pDamagedObject, fLoss, pObjectAttacker);
    }
    return true;
}

void _declspec(naked) HOOK_CObject_ProcessDamage()
{
    // .text:005A0DF7                 mov     ecx, [esi+160h]
    // .text:005A0DFD                 fld     [esp+0D4h+arg_0]
    // .text:005A0E04                 fmul    dword ptr [ecx+18h]
    // .text:005A0E07                 fsubr   dword ptr [esi+154h]
    // .text:005A0E0D                 fst     dword ptr [esi+154h]

    _asm
    {
        pushad
        mov     pDamagedObject, esi
        mov     pObjectAttacker, edi
        fst     dword ptr fNewObjectHealth
    }
    if (TriggerObjectDamageEvent())
    {
        bObjectDamaged = true;
        _asm
        {
            popad
            fst     dword ptr [esi+154h]
            jmp     RETURN_CObject_ProcessDamage
        }
    }
    else
    {
        bObjectDamaged = false;
        _asm
        {
            popad
            ffree   st(0)
            fdecstp
            jmp     RETURN_CObject_ProcessDamage_Cancel
        }
    }
}

unsigned char ucColDamageEffect = NULL;
bool          TriggerObjectBreakEvent()
{
    if (m_pObjectBreakHandler && pDamagedObject)
    {
        return m_pObjectBreakHandler(pDamagedObject, pObjectAttacker);
    }
    return true;
}

void _declspec(naked) HOOK_CObject_ProcessBreak()
{
    _asm
    {
        pushad
    }
    ucColDamageEffect = *(unsigned char*)((DWORD)pDamagedObject + 324);

    if (ucColDamageEffect != NULL)
    {
        if (ucColDamageEffect == 0xC8 || ucColDamageEffect == 0xCA || ucColDamageEffect == 1 || ucColDamageEffect == 0x14 || ucColDamageEffect == 0x15)
        {
            if (!TriggerObjectBreakEvent())
            {
                bObjectDamaged = false;
                _asm
                {
                    popad
                    jmp     RETURN_CObject_ProcessDamage_Cancel
                }
            }
        }
    }

    _asm
    {
        popad
        cmp     eax, 0C9h
        jmp     RETURN_CObject_ProcessBreak
    }
}

void _declspec(naked) HOOK_CObject_ProcessCollision()
{
    if (bObjectDamaged)
    {
        _asm
        {
            test    byte ptr [esi+1Ch], 1
            jnz     checkfordynamic
            jmp     RETURN_CObject_ProcessCollision

        checkfordynamic:
            jmp     JMP_DynamicObject_Cond_Zero
        }
    }
    else
    {
        _asm
        {
            jmp     RETURN_CObject_ProcessCollision
        }
    }
}

DWORD WindowRespondsToCollision_CalledFrom = 0;
void _declspec(naked) HOOK_CGlass_WindowRespondsToCollision()
{
    _asm
    {
        push eax
        mov eax, [esp + 4]
        mov WindowRespondsToCollision_CalledFrom, eax
        pop eax
    }

    pObjectAttacker = nullptr;

    if (WindowRespondsToCollision_CalledFrom != CALL_FROM_CGlass_WindowRespondsToExplosion)
    {
        _asm
        {
            mov pDamagedObject, esi
        }
    }

    // Get attacker for the glass break
    if (WindowRespondsToCollision_CalledFrom == CALL_FROM_CPhysical_ApplyCollision ||
        WindowRespondsToCollision_CalledFrom == CALL_FROM_CPhysical_ApplyCollision_2 ||
        WindowRespondsToCollision_CalledFrom == CALL_FROM_CPhysical_ApplySoftCollision)
    {
        _asm
        {
            mov pObjectAttacker, edi
        }
    }

    if (WindowRespondsToCollision_CalledFrom == CALL_FROM_CGlass_WasGlassHitByBullet)
    {
        _asm
        {
            mov pObjectAttacker, ebx // WasGlassHitByBullet called from CWeapon::DoBulletImpact
        }

        if (!pObjectAttacker || (pObjectAttacker && !pObjectAttacker->m_pRwObject)) // WasGlassHitByBullet called from CBulletInfo::Update
        {
            _asm
            {
                push ecx
                mov ecx, [edi]
                mov pObjectAttacker, ecx
                pop ecx
            }
        }
    }

    if (WindowRespondsToCollision_CalledFrom == CALL_FROM_CGlass_WindowRespondsToExplosion)
    {
        _asm
        {
            mov pDamagedObject, edx
            mov pObjectAttacker, ebp
        }
    }

    if (pObjectAttacker && !pObjectAttacker->m_pRwObject) // Still wrong?
        pObjectAttacker = nullptr;

    if (TriggerObjectBreakEvent())
    {
        _asm
        {
            sub esp, 68h
            push esi
            mov esi, [esp+6Ch+4]
            jmp RETURN_CGlass_WindowRespondsToCollision
        }
    }
    else
    {
        _asm
        {
            retn
        }
    }
}

// Called when glass object is being broken by ped melee attack
DWORD dummy_404350 = 0x404350;
void _declspec(naked) HOOK_CGlass__BreakGlassPhysically()
{
    _asm
    {
        mov     pDamagedObject, esi
        push    ecx
        mov     ecx, [esp+4]
        mov     pObjectAttacker, ecx
        pop     ecx
    }

    if (TriggerObjectBreakEvent())
    {
        _asm
        {
            // restore replaced part
            push    dummy_404350
            // jump outside of the hook
            jmp     RETURN_CGlass__BreakGlassPhysically
        }
    }
    else
    {
        _asm
        {
            pop     edi
            pop     esi
            pop     ebp
            pop     ebx
            add     esp, 0BCh
            retn
        }
    }
}

void* pFxSystemToBeDestroyed;
void  FxManager_c__DestroyFxSystem()
{
    if (m_pFxSystemDestructionHandler)
    {
        m_pFxSystemDestructionHandler(pFxSystemToBeDestroyed);
    }
}

void _declspec(naked) HOOK_FxManager_c__DestroyFxSystem()
{
    _asm
    {
        mov pFxSystemToBeDestroyed, edi
        pushad
    }

    FxManager_c__DestroyFxSystem();

    _asm
    {
        popad

        // Replaced code
        add esp, 4
        pop edi
        pop ebx
        pop ecx
        retn 4
    }
}

DWORD pProcessedGangDriveBySimpleTask;
void  CTaskSimpleGangDriveBy__ProcessPed()
{
    AnimationId* pRequiredAnim = ((AnimationId*)(pProcessedGangDriveBySimpleTask + 0x24));
    AssocGroupId requiredAnimGroup = *((AssocGroupId*)(pProcessedGangDriveBySimpleTask + 0x28));

    if (m_pDrivebyAnimationHandler != NULL)
        *pRequiredAnim = m_pDrivebyAnimationHandler(*pRequiredAnim, requiredAnimGroup);
}

DWORD RETURN_CTaskSimpleGangDriveBy_ProcessPed_Cancel = 0x62D5C1;
void _declspec(naked) HOOK_CTaskSimpleGangDriveBy__ProcessPed()
{
    // esi contains 'this'
    _asm
    {
        mov pProcessedGangDriveBySimpleTask, esi
        pushad
    }
    CTaskSimpleGangDriveBy__ProcessPed();
    _asm
    {
        popad
        // Replaced code
        cmp[esi + 28h], edi                // .text:0062D5A7
        jnz GangDriveBy_ProcessPed_Cancel  // .text:0062D5AA
        // Return to original code
        jmp RETURN_CTaskSimplyGangDriveBy__ProcessPed

    GangDriveBy_ProcessPed_Cancel:
        jmp RETURN_CTaskSimpleGangDriveBy_ProcessPed_Cancel
    }
}

eRadioStationID dwStationID = UNKNOWN;
BYTE            bTrackID = 0;
DWORD           dwNumberOfTracks = 0;

const DWORD pTrackNumbers[] = {
    0x2,             // radio off, somewhere 2 is subtracted from this so that's why it's 2
    0xB,             // playback fm
    0xF,             // k-rose
    0xF,             // k-dst
    0xE,             // bounce fm
    0x10,            // sf-ur
    0xE,             // rls
    0xD,             // radio x
    0xD,             // csr
    0xE,             // k-jah
    0xC,             // master sounds
    0x1F,
};

bool ChooseMusicTrackIndex_SteamFix()
{
    // update the number of tracks from the array above as it has the new values
    dwNumberOfTracks = pTrackNumbers[dwStationID];

    // switch contains all radio stations and music that has been removed from the game

    switch (dwStationID)
    {
        case Playback_FM:
        {
            // disable "Critical Beatdown"
            if (bTrackID == 9)
            {
                return true;
            }
        }
        break;
        case K_Rose:
            break;
        case K_DST:
        {
            // disable "Running Down A Dream"
            if (bTrackID == 0)
            {
                return true;
            }
            // disable "Woman To Woman"
            else if (bTrackID == 2)
            {
                return true;
            }
        }
        break;
        case BOUNCE_FM:
        {
            // disable "You Dropped A Bomb On Me"
            if (bTrackID == 3)
            {
                return true;
            }
            // disable "Yum Yum"
            else if (bTrackID == 8)
            {
                return true;
            }
            // disable "Running Away"
            else if (bTrackID == 15)
            {
                return true;
            }
        }
        break;
        case SF_UR:
            break;
        case RLS:
        {
            // "I Don't Give A f*ck"
            if (bTrackID == 1)
            {
                return true;
            }
            // disable "Express Yourself"
            else if (bTrackID == 6)
            {
                return true;
            }
        }
        break;
        case RADIO_X:
        {
            // disable "Hellraiser"
            if (bTrackID == 6)
            {
                return true;
            }
            // disable "Killing in the Name of"
            else if (bTrackID == 7)
            {
                return true;
            }
        }
        break;
        case CSR_1039:
            break;
        case K_JAH_WEST:
        {
            // disable "Ring My Bell"
            if (bTrackID == 2)
            {
                return true;
            }
            // disable "Don't Let It Go To Your Head"
            else if (bTrackID == 3)
            {
                return true;
            }
        }
        break;
        case Master_Sounds:
        {
            // disable "Express Yourself"
            if (bTrackID == 0)
            {
                return true;
            }
            // disable "Rock Creek Park"
            else if (bTrackID == 5)
            {
                return true;
            }
            // disable "Funky President"
            else if (bTrackID == 7)
            {
                return true;
            }
            // disable "Grunt"
            else if (bTrackID == 8)
            {
                return true;
            }
            // disable "Soul power"
            else if (bTrackID == 11)
            {
                return true;
            }
            // disable "The payback"
            else if (bTrackID == 16)
            {
                return true;
            }
        }
        break;
        case WCTR:
            break;
    }
#ifdef PRINT_SONGID
    OutputDebugLine(SString("%i %i", dwStationID, bTrackID));
#endif
    // song is allowed
    return false;
}
/*
    This hook is ultra important as of 09/11/2014 as steam has released a patch which changes the audio files and causes the following consequenses:
        1) a division by zero when trying to start deleted songs due to the track list being malformed
        2) the game getting stuck on a specific song and never carrying on as the game tries to play 15 songs before restarting on a channel that only has 12

    These are as a result of the fact that steam updated gta-sa.exe and gta_sa.exe is our old exe which contains the arrays the game had originally for audio
   files All the files related to the deleted audio are zeroed and decompress to 5kb 0 length files which includes intros and outros.
*/
void _declspec(naked) HOOK_CAERadioTrackManager__ChooseMusicTrackIndex()
{
    // esi is our station id
    // al has the random number picked (music id the game wants to play)

    _asm
    {
        add esp, 8              // fix the stack from the function call above as we overrote this instruction
        pushad                  // save our registers
        mov dwStationID, esi    // save esi, we need the station ID above
        mov bTrackID, al        // save our track ID which we need to figure out if we can play it.
    }

    // returns true if this is a restricted song
    if (ChooseMusicTrackIndex_SteamFix())
    {
        _asm
        {
            // pop the stack
            popad
            // go back to generating a number again, this is so that we don't get stuck in an infinite loop if we tried to increment or decrement
            // SA tries to avoid playing the same songs close together so it won't play anything that's already played once until we have done a full loop
            // as such generating a new ID is better than trying to fix it (this is how the game naturally works anyway)
            jmp RETURN_CAERadioTrackManager__ChooseMusicTrackIndex_Regenerate
        }
    }
    // looks good, carry on
    _asm
    {
        // pop the stack
        popad
        // this number of tracks needs fixing because we need the game to know about the deletions here as it is used for the wrap around logic of radio
        mov ecx, dwNumberOfTracks
        // jump back to normal processing
        jmp RETURN_CAERadioTrackManager__ChooseMusicTrackIndex
    }
}

// Use AI heli rotor sound if player sound bank is not loaded
void _declspec(naked) HOOK_CAEVehicleAudioEntity__ProcessDummyHeli()
{
    _asm
    {
        // push our argument
        push    [esp+8Ch+4]
        mov     ecx, esi
        // call twin function
        call    dwFUNC_CAEVehicleAudioEntity__ProcessAIHeli
        // go back
        jmp     RETURN_CAEVEhicleAudioEntity__ProcessDummyHeli
    }
}

// Use AI plane propeller sound if player sound bank is not loaded
void _declspec(naked) HOOK_CAEVehicleAudioEntity__ProcessDummyProp()
{
    _asm
    {
        // push our argument
        push    [esp+98h+4]
        mov     ecx, esi
        // call twin function
        call    dwFUNC_CAEVehicleAudioEntity__ProcessAIProp
        // go back
        jmp     RETURN_CAEVEhicleAudioEntity__ProcessDummyProp
    }
}

const float kfTimeStepOriginal = 1.66f;
void _declspec(naked) HOOK_CTaskSimpleSwim_ProcessSwimmingResistance()
{
    _asm
    {
        fsub    st, st(1)

        fld     dword ptr[esp + 16]
        lea     eax, [esi + 44h]
        mov     ecx, eax
        fmul    st, st(1)

        fdiv    ds : 0xB7CB5C
        fmul    kfTimeStepOriginal

        fstp    dword ptr[esp + 28]

        fld     dword ptr[esp + 20]
        fmul    st, st(1)

        fdiv    ds : 0xB7CB5C
        fmul    kfTimeStepOriginal

        fstp    dword ptr[esp + 32]
        fmul    dword ptr[esp + 24]

        fdiv    ds : 0xB7CB5C
        fmul    kfTimeStepOriginal

        jmp     RETURN_CTaskSimpleSwim_ProcessSwimmingResistance
    }
}

void PostCWorld_ProcessPedsAfterPreRender()
{
    if (m_postWorldProcessPedsAfterPreRenderHandler)
        m_postWorldProcessPedsAfterPreRenderHandler();

    // Scale the object entities
    CPools* pools = pGameInterface->GetPools();
    for (std::uint32_t i = 0; i < MAX_OBJECTS; i++)
    {
        CObject* objectEntity = pools->GetObjectFromIndex(i);
        if (!objectEntity)
            continue;
        auto objectInterface = objectEntity->GetObjectInterface();
        if (objectInterface && objectEntity->GetPreRenderRequired())
        {
            if (objectInterface->fScale != 1.0f || objectInterface->bUpdateScale)
            {
                objectEntity->SetScaleInternal(*objectEntity->GetScale());
                objectInterface->bUpdateScale = false;
            }
            RpClump* clump = objectInterface->m_pRwObject;
            if (clump && clump->object.type == RP_TYPE_CLUMP)
                objectEntity->UpdateRpHAnim();
            objectEntity->SetPreRenderRequired(false);
        }
    }
}

const DWORD CWorld_ProcessPedsAfterPreRender = 0x563430;
void _declspec(naked) HOOK_Idle_CWorld_ProcessPedsAfterPreRender()
{
    __asm
    {
       call CWorld_ProcessPedsAfterPreRender
       call PostCWorld_ProcessPedsAfterPreRender
       jmp RETURN_Idle_CWorld_ProcessPedsAfterPreRender
    }
}

DWORD dwLastRequestedStation = -1;
void  CAEAmbienceTrackManager__UpdateAmbienceTrackAndVolume_ChangeStation(DWORD dwStationID)
{
    if (dwLastRequestedStation != dwStationID)
    {
        if (m_pAudioZoneRadioSwitchHandler)
        {
            m_pAudioZoneRadioSwitchHandler(dwStationID);
        }
    }
    dwLastRequestedStation = dwStationID;
}

// Start radio after entering audio zone
void _declspec(naked) HOOK_CAEAmbienceTrackManager__UpdateAmbienceTrackAndVolume_StartRadio()
{
    _asm
    {
        push    [esi+3]
        call    CAEAmbienceTrackManager__UpdateAmbienceTrackAndVolume_ChangeStation
        add     esp, 4
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        add     esp, 36
        retn
    }
}

// Stop radio after leaving audio zone
void _declspec(naked) HOOK_CAEAmbienceTrackManager__UpdateAmbienceTrackAndVolume_StopRadio()
{
    _asm
    {
        push    0
        call    CAEAmbienceTrackManager__UpdateAmbienceTrackAndVolume_ChangeStation
        add     esp, 4
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        add     esp, 36
        retn
    }
}

static void AddVehicleColoredDebris(CAutomobileSAInterface* pVehicleInterface, CVector& vecPosition, int count)
{
    SClientEntity<CVehicleSA>* pVehicleClientEntity = pGameInterface->GetPools()->GetVehicle((DWORD*)pVehicleInterface);
    CVehicle*                  pVehicle = pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
    if (pVehicle)
    {
        SColor colors[4];
        pVehicle->GetColor(&colors[0], &colors[1], &colors[2], &colors[3], false);

        RwColor color = {colors[0].R * pVehicleInterface->m_fLighting, colors[0].G * pVehicleInterface->m_fLighting,
                         colors[0].B * pVehicleInterface->m_fLighting, 0xFF};

        // Fx_c::AddDebris
        ((void(__thiscall*)(int, CVector&, RwColor&, float, int))0x49F750)(CLASS_CFx, vecPosition, color, 0.06f, count / 100 + 1);
    }
}

const DWORD RETURN_CAutomobile__dmgDrawCarCollidingParticles = 0x6A7081;
void _declspec(naked) HOOK_CAutomobile__dmgDrawCarCollidingParticles()
{
    _asm
    {
        lea eax, [esp + 0x1C]
        push ebp                // count
        push eax                // pos
        push edi                // vehicle
        call AddVehicleColoredDebris
        add esp, 12

        jmp RETURN_CAutomobile__dmgDrawCarCollidingParticles
    }
}

// Reimplement camera photo creation
// to have better photo quality
static void TakePhotograph()
{
    g_pCore->InitiateScreenShot(true);
}

const DWORD RETURN_CWeapon__TakePhotograph = 0x73C273;
void _declspec(naked) HOOK_CWeapon__TakePhotograph()
{
    _asm
    {
        // Restore instructions replaced by hook
        add     esp, 8
        test    edi, edi

        call    TakePhotograph

        // Go back
        jmp     RETURN_CWeapon__TakePhotograph
    }
}

// Disable camera collisions for projectiles and detached vehicle parts
const DWORD RETURN_CCollision__CheckCameraCollisionObjects = 0x41AB98;
const DWORD RETURN_CCollision__CheckCameraCollisionObjects_2 = 0x41AC26;

bool CanEntityCollideWithCamera(CEntitySAInterface* pEntity)
{
    switch (pEntity->m_nModelIndex)
    {
        // projectiles
        case 342:            // grenade
        case 343:            // teargas
        case 344:            // molotov
        case 363:            // satchel

        // vehicle parts
        case 374:            // car_door
        case 375:            // car_bumper
        case 376:            // car_panel
        case 377:            // car_bonnet
        case 378:            // car_boot
        case 379:            // car_wheel
            return false;
    }

    return true;
}

void _declspec(naked) HOOK_CCollision__CheckCameraCollisionObjects()
{
    _asm
    {
        // Restore instructions replaced by hook
        jz      out2
        movsx   edx, word ptr [esi+22h]

        // Store all registers
        pushad

        // Do our stuff
        push    esi // pEntity
        call    CanEntityCollideWithCamera
        add     esp, 4
        test    al, al

        // Restore registers
        popad

        jnz     out1
        jmp     RETURN_CCollision__CheckCameraCollisionObjects_2

    out1: jmp   RETURN_CCollision__CheckCameraCollisionObjects
    out2: jmp   RETURN_CCollision__CheckCameraCollisionObjects_2
    }
}
