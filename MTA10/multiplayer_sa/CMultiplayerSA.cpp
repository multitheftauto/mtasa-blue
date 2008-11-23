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
#include <stdio.h>

// These includes have to be fixed!
#include "..\game_sa\CCameraSA.h"
#include "..\game_sa\CEntitySA.h"
#include "..\game_sa\CPedSA.h"
#include "..\game_sa\common.h"

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

unsigned long CMultiplayerSA::FUNC_CStreaming_Update;
unsigned long CMultiplayerSA::FUNC_CAudioEngine__DisplayRadioStationName;
unsigned long CMultiplayerSA::FUNC_CHud_Draw;

unsigned long CMultiplayerSA::ADDR_CursorHiding;

unsigned long CMultiplayerSA::FUNC_CPlayerInfoBase;


#define HOOKPOS_FxManager_CreateFxSystem 0x4A9BE0
#define HOOKPOS_FxManager_DestroyFxSystem 0x4A9810

DWORD STORE_FxManager_CreateFxSystem = 0;
DWORD STORE_FxManager_DestroyFxSystem = 0;

DWORD RETURN_FxManager_CreateFxSystem = 0x4A9BE8;
DWORD RETURN_FxManager_DestroyFxSystem = 0x4A9817;

#define HOOKPOS_CCam_ProcessFixed 0x051D470
#define HOOKPOS_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon 0x6859a0
#define HOOKPOS_CPed_IsPlayer 0x5DF8F0

DWORD RETURN_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon = 0x6859A7;
DWORD RETURN_CPed_IsPlayer = 0x5DF8F6;

DWORD STORE_CCam_ProcessFixed = 0;
DWORD STORE_FindPlayerCoors = 0;
DWORD STORE_FindPlayerCentreOfWorld = 0;
DWORD STORE_FindPlayerHeading = 0;
DWORD STORE_CStreaming_Update_Caller = 0;
DWORD STORE_CHook_Draw_Caller = 0;
DWORD STORE_CRunningScript_Process = 0;
DWORD STORE_CExplosion_AddExplosion = 0;
DWORD STORE_CRealTimeShadowManager__ReturnRealTimeShadow = 0;
DWORD STORE_CCustomRoadsignMgr__RenderRoadsignAtomic = 0;
DWORD STORE_Trailer_BreakTowLink = 0;
DWORD STORE_CRadar__DrawRadarGangOverlay = 0;
DWORD STORE_CTaskComplexJump__CreateSubTask = 0;
DWORD STORE_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon = 0;
DWORD STORE_CPed_IsPlayer = 0;

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
bool m_bExplosionsDisabled;
float fGlobalGravity = 0.008f;
float fLocalPlayerGravity = 0.008f;
float fLocalPlayerCameraRotation = 0.0f;
bool bCustomCameraRotation = false;

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

ExplosionHandler * m_pExplosionHandler; // stores our handler
BreakTowLinkHandler * m_pBreakTowLinkHandler = NULL;
DrawRadarAreasHandler * m_pDrawRadarAreasHandler = NULL;
Render3DStuffHandler * m_pRender3DStuffHandler = NULL;


VOID HOOK_FindPlayerCoors();
VOID HOOK_FindPlayerCentreOfWorld();
VOID HOOK_FindPlayerHeading();
VOID HOOK_CStreaming_Update_Caller();
VOID HOOK_CHud_Draw_Caller();
VOID HOOK_CRunningScript_Process();
VOID HOOK_CExplosion_AddExplosion();
VOID HOOK_CRealTimeShadowManager__ReturnRealTimeShadow();
VOID HOOK_CCustomRoadsignMgr__RenderRoadsignAtomic();
VOID HOOK_Trailer_BreakTowLink();
VOID HOOK_CRadar__DrawRadarGangOverlay();
VOID HOOK_CTaskComplexJump__CreateSubTask();
VOID HOOK_CWeapon_FireAreaEffect();
VOID HOOK_CBike_ProcessRiderAnims();
VOID HOOK_FxManager_CreateFxSystem ();
VOID HOOK_FxManager_DestroyFxSystem ();
VOID HOOK_CCam_ProcessFixed ();
VOID HOOK_Render3DStuff ();
VOID HOOK_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon ();
VOID HOOK_CPed_IsPlayer ();

CEntitySAInterface * dwSavedPlayerPointer = 0;
CEntitySAInterface * activeEntityForStreaming = 0; // the entity that the streaming system considers active

#define CALL_Render3DStuff 0x53EABF
#define FUNC_Render3DStuff 0x53DF40

#define CALL_CRenderer_Render 0x53EA12
#define FUNC_CRenderer_Render 0x727140

#define CALL_CWeapon_FireAreaEffect 0x73EBFE
#define FUNC_CWeapon_FireAreaEffect 0x53A450
#define CALL_CBike_ProcessRiderAnims 0x6BF425   // @ CBike::ProcessDrivingAnims
DWORD FUNC_CBike_ProcessRiderAnims = 0x6B7280;


CMultiplayerSA::CMultiplayerSA()
{
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
	bSetCenterOfWorld = false;
	bHasProcessedScript = false;

    // Set the memory areas we might change later through accessors
    DWORD oldProt, oldProt2;
    VirtualProtect((LPVOID)0x712330,1,PAGE_EXECUTE_READWRITE,&oldProt);
    VirtualProtect((LPVOID)0x716380,1,PAGE_EXECUTE_READWRITE,&oldProt);
    VirtualProtect((LPVOID)0x713950,1,PAGE_EXECUTE_READWRITE,&oldProt);
    VirtualProtect((LPVOID)0x717180,3,PAGE_EXECUTE_READWRITE,&oldProt);
    VirtualProtect((LPVOID)0x561760,1,PAGE_EXECUTE_READWRITE,&oldProt);
    VirtualProtect((LPVOID)0x6194A0,1,PAGE_EXECUTE_READWRITE,&oldProt);
    VirtualProtect((LPVOID)0x4D9888,5,PAGE_EXECUTE_READWRITE,&oldProt);
    VirtualProtect((LPVOID)0x7449F0,1,PAGE_EXECUTE_READWRITE,&oldProt);
    VirtualProtect((LPVOID)0x6E3950,1,PAGE_EXECUTE_READWRITE,&oldProt);
    VirtualProtect((LPVOID)0x5702FD,5,PAGE_EXECUTE_READWRITE,&oldProt); // CPlayerInfo__Process
    VirtualProtect((LPVOID)0x6AD75A,5,PAGE_EXECUTE_READWRITE,&oldProt); // CAutomobile__ProcessControlInputs
    VirtualProtect((LPVOID)0x6BE34B,5,PAGE_EXECUTE_READWRITE,&oldProt); // CBike__ProcessControlInputs
    VirtualProtect((LPVOID)0x67E834,5,PAGE_EXECUTE_READWRITE,&oldProt); // CTaskSimpleJetPack__ProcessControlInput
    VirtualProtect((LPVOID)0x540120,3,PAGE_EXECUTE_READWRITE,&oldProt);
    VirtualProtect((LPVOID)0x6B5A10,1,PAGE_EXECUTE_READWRITE,&oldProt);
    VirtualProtect((LPVOID)0x6B7449,3,PAGE_EXECUTE_READWRITE,&oldProt);
    VirtualProtect((LPVOID)0x6B763C,4,PAGE_EXECUTE_READWRITE,&oldProt);
    VirtualProtect((LPVOID)0x6B7617,5,PAGE_EXECUTE_READWRITE,&oldProt);
    VirtualProtect((LPVOID)0x6B62A7,1,PAGE_EXECUTE_READWRITE,&oldProt);
    VirtualProtect((LPVOID)0x6B7642,5,PAGE_EXECUTE_READWRITE,&oldProt);
    VirtualProtect((LPVOID)0x6B7449,3,PAGE_EXECUTE_READWRITE,&oldProt);
    VirtualProtect((LPVOID)0x6A90D8,5,PAGE_EXECUTE_READWRITE,&oldProt);
    VirtualProtect((LPVOID)0x72C665,6,PAGE_EXECUTE_READWRITE,&oldProt);	 // waves
    VirtualProtect((LPVOID)0x72C659,10,PAGE_EXECUTE_READWRITE,&oldProt);	//waves
    VirtualProtect((LPVOID)0x534540,3,PAGE_EXECUTE_READWRITE,&oldProt);
    VirtualProtect((LPVOID)0x96C048,10600,PAGE_EXECUTE_READWRITE,&oldProt); // Garages

	//00442DC6  |. 0F86 31090000  JBE gta_sa_u.004436FD
	//00442DC6     E9 32090000    JMP gta_sa_u.004436FD

    // increase the number of vehicles types (not actual vehicles) that can be loaded at once
    *(int *)0x8a5a84 = 127;

    // DISABLE CGameLogic::Update
	VirtualProtect((LPVOID)0x442AD0,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	memset((void *)0x442AD0, 0xC3, 1);
	VirtualProtect((LPVOID)0x442AD0,1,oldProt,&oldProt2);

    // STOP IT TRYING TO LOAD THE SCM
	VirtualProtect((LPVOID)0x468EB5,2,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x468EB5 = 0xEB;
	*(BYTE *)0x468EB6 = 0x32;
	VirtualProtect((LPVOID)0x468EB5,2,oldProt,&oldProt2);

	HookInstall(HOOKPOS_FindPlayerCoors, (DWORD)HOOK_FindPlayerCoors, &STORE_FindPlayerCoors, 6);
	HookInstall(HOOKPOS_FindPlayerCentreOfWorld, (DWORD)HOOK_FindPlayerCentreOfWorld, &STORE_FindPlayerCentreOfWorld, 6);
	HookInstall(HOOKPOS_FindPlayerHeading, (DWORD)HOOK_FindPlayerHeading, &STORE_FindPlayerHeading, 6);
	HookInstall(HOOKPOS_CStreaming_Update_Caller, (DWORD)HOOK_CStreaming_Update_Caller, &STORE_CStreaming_Update_Caller, 7);
	HookInstall(HOOKPOS_CHud_Draw_Caller, (DWORD)HOOK_CHud_Draw_Caller, &STORE_CHook_Draw_Caller, 10);
	HookInstall(HOOKPOS_CRunningScript_Process, (DWORD)HOOK_CRunningScript_Process, &STORE_CRunningScript_Process, 6);
	HookInstall(HOOKPOS_CExplosion_AddExplosion, (DWORD)HOOK_CExplosion_AddExplosion, &STORE_CExplosion_AddExplosion, 6);
    HookInstall(HOOKPOS_CRealTimeShadowManager__ReturnRealTimeShadow, (DWORD)HOOK_CRealTimeShadowManager__ReturnRealTimeShadow, &STORE_CRealTimeShadowManager__ReturnRealTimeShadow, 6);
	HookInstall(HOOKPOS_CCustomRoadsignMgr__RenderRoadsignAtomic, (DWORD)HOOK_CCustomRoadsignMgr__RenderRoadsignAtomic, &STORE_CCustomRoadsignMgr__RenderRoadsignAtomic, 6);
    HookInstall(HOOKPOS_Trailer_BreakTowLink, (DWORD)HOOK_Trailer_BreakTowLink, &STORE_Trailer_BreakTowLink, 6);
    HookInstall(HOOKPOS_CRadar__DrawRadarGangOverlay, (DWORD)HOOK_CRadar__DrawRadarGangOverlay, &STORE_CRadar__DrawRadarGangOverlay, 6);
    HookInstall(HOOKPOS_CTaskComplexJump__CreateSubTask, (DWORD)HOOK_CTaskComplexJump__CreateSubTask, &STORE_CTaskComplexJump__CreateSubTask, 6);
    HookInstall(HOOKPOS_FxManager_CreateFxSystem, (DWORD)HOOK_FxManager_CreateFxSystem, &STORE_FxManager_CreateFxSystem, 8 );
    HookInstall(HOOKPOS_FxManager_DestroyFxSystem, (DWORD)HOOK_FxManager_DestroyFxSystem, &STORE_FxManager_DestroyFxSystem, 7 );
    HookInstall(HOOKPOS_CCam_ProcessFixed, (DWORD)HOOK_CCam_ProcessFixed, &STORE_CCam_ProcessFixed, 7 );
    HookInstall(HOOKPOS_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon, (DWORD)HOOK_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon, &STORE_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon, 7 );
    HookInstall(HOOKPOS_CPed_IsPlayer, (DWORD)HOOK_CPed_IsPlayer, &STORE_CPed_IsPlayer, 6 );

    HookInstallCall ( CALL_CBike_ProcessRiderAnims, (DWORD)HOOK_CBike_ProcessRiderAnims );
    HookInstallCall ( CALL_Render3DStuff, (DWORD)HOOK_Render3DStuff );
	HookInstallCall ( CALL_CWeapon_FireAreaEffect, (DWORD)HOOK_CWeapon_FireAreaEffect);

    // Increase double link limit from 3200 ro 4000
    VirtualProtect((LPVOID)0x550F82,4,PAGE_EXECUTE_READWRITE,&oldProt);	
    *(int*)0x00550F82 = 4000;
    VirtualProtect((LPVOID)0x550F82,4,oldProt,&oldProt2); 


    // Disable GTA being able to call CAudio::StopRadio ()
    // Well this isn't really CAudio::StopRadio, it's some global class
    // func that StopRadio just jumps to.
    VirtualProtect((LPVOID)0x4E9820,3,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x4E9820 = 0xC2;
    *(BYTE *)0x4E9821 = 0x08;
    *(BYTE *)0x4E9822 = 0x00;
	VirtualProtect((LPVOID)0x4E9820,3,oldProt,&oldProt2); 

    // Disable GTA being able to call CAudio::StartRadio ()
    VirtualProtect((LPVOID)0x4DBEC0,3,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x4DBEC0 = 0xC2;
    *(BYTE *)0x4DBEC1 = 0x00;
    *(BYTE *)0x4DBEC2 = 0x00;
	VirtualProtect((LPVOID)0x4DBEC0,3,oldProt,&oldProt2); 

    VirtualProtect((LPVOID)0x4EB3C0,3,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x4EB3C0 = 0xC2;
    *(BYTE *)0x4EB3C1 = 0x10;
    *(BYTE *)0x4EB3C2 = 0x00;
	VirtualProtect((LPVOID)0x4EB3C0,3,oldProt,&oldProt2); 
    
    // DISABLE cinematic camera for trains
    VirtualProtect((LPVOID)0x52A535,1,PAGE_EXECUTE_READWRITE,&oldProt);
    *(BYTE *)0x52A535 = 0;
    VirtualProtect((LPVOID)0x52A535,1,oldProt,&oldProt2);

    // DISABLE wanted levels for military zones
    VirtualProtect((LPVOID)0x72DF0D,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x72DF0D = 0xEB;
	VirtualProtect((LPVOID)0x72DF0D,1,oldProt,&oldProt2); 

    // THROWN projectiles throw more accurately
    VirtualProtect((LPVOID)0x742685,2,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x742685 = 0x90;
    *(BYTE *)0x742686 = 0xE9;
	VirtualProtect((LPVOID)0x742685,2,oldProt,&oldProt2);    

    // DISABLE CProjectileInfo::RemoveAllProjectiles
    VirtualProtect((LPVOID)0x7399B0,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x7399B0 = 0xC3;
	VirtualProtect((LPVOID)0x7399B0,1,oldProt,&oldProt2);   

    // DISABLE CRoadBlocks::GenerateRoadblocks
    VirtualProtect((LPVOID)0x4629E0,1,PAGE_EXECUTE_READWRITE,&oldProt);
    *(BYTE *)0x4629E0 = 0xC3;
    VirtualProtect((LPVOID)0x4629E0,1,oldProt,&oldProt2);   


    // Temporary hack for disabling hand up
    /*
    VirtualProtect((LPVOID)0x62AEE7,6,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x62AEE7 = 0x90;
    *(BYTE *)0x62AEE8 = 0x90;
    *(BYTE *)0x62AEE9 = 0x90;
    *(BYTE *)0x62AEEA = 0x90;
    *(BYTE *)0x62AEEB = 0x90;
    *(BYTE *)0x62AEEC = 0x90;
	VirtualProtect((LPVOID)0x62AEE7,6,oldProt,&oldProt2);  
    */

    // DISABLE CAERadioTrackManager::CheckForMissionStatsChanges(void) (special DJ banter)
    VirtualProtect((LPVOID)0x4E8410,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x4E8410 = 0xC3; // retn
	VirtualProtect((LPVOID)0x4E8410,1,oldProt,&oldProt2);    

    // DISABLE CPopulation__AddToPopulation
    VirtualProtect((LPVOID)0x614720,3,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x614720 = 0x32; // xor al, al
    *(BYTE *)0x614721 = 0xC0;
    *(BYTE *)0x614722 = 0xC3; // retn
	VirtualProtect((LPVOID)0x614720,3,oldProt,&oldProt2);      

    // Disables deletion of RenderWare objects during unloading of ModelInfo
    // This is used so we can circumvent the limit of ~21 different vehicles by managing the RwObject ourselves
    //VirtualProtect((LPVOID)0x4C9890,1,PAGE_EXECUTE_READWRITE,&oldProt);
    //*(BYTE *)0x4C9890 = 0xC3;
    //VirtualProtect((LPVOID)0x4C9890,1,oldProt,&oldProt2);

    //VirtualProtect((LPVOID)0x408A1B,5,PAGE_EXECUTE_READWRITE,&oldProt);
    //memset ( (void*)0x408A1B, 0x90, 5 );
    //VirtualProtect((LPVOID)0x408A1B,5,oldProt,&oldProt2);

    // Hack to make the choke task use 0 time left remaining when he starts t
    // just stand there looking. So he won't do that.
    VirtualProtect((LPVOID)0x620607,2,PAGE_EXECUTE_READWRITE,&oldProt);
    *(unsigned char *)0x620607 = 0x33;
    *(unsigned char *)0x620608 = 0xC0;
    VirtualProtect((LPVOID)0x620607,2,oldProt,&oldProt2); 

    VirtualProtect((LPVOID)0x620618,5,PAGE_EXECUTE_READWRITE,&oldProt);		
    *(unsigned char *)0x620618 = 0x33;
    *(unsigned char *)0x620619 = 0xC0;
    *(unsigned char *)0x62061A = 0x90;
    *(unsigned char *)0x62061B = 0x90;
    *(unsigned char *)0x62061C = 0x90;
    VirtualProtect((LPVOID)0x620618,5,oldProt,&oldProt2); 

    // Hack to make non-local players always update their aim on akimbo weapons using camera
    // so they don't freeze when local player doesn't aim.
    VirtualProtect((LPVOID)0x61EFFE,1,PAGE_EXECUTE_READWRITE,&oldProt);
    *(BYTE *)0x61EFFE = 0xEB;   // JMP imm8 (was JZ imm8)
    VirtualProtect((LPVOID)0x61EFFE,1,oldProt,&oldProt2);    
    

    // DISABLE CGameLogic__SetPlayerWantedLevelForForbiddenTerritories
    VirtualProtect((LPVOID)0x441770,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x441770 = 0xC3;
	VirtualProtect((LPVOID)0x441770,1,oldProt,&oldProt2);    

    // DISABLE CCrime__ReportCrime
    VirtualProtect((LPVOID)0x532010,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x532010 = 0xC3;
	VirtualProtect((LPVOID)0x532010,1,oldProt,&oldProt2);    
    
	// Disables deletion of RenderWare objects during unloading of ModelInfo
	// This is used so we can circumvent the limit of ~21 different vehicles by managing the RwObject ourselves
    //VirtualProtect((LPVOID)0x4C9890,1,PAGE_EXECUTE_READWRITE,&oldProt);
	//*(BYTE *)0x4C9890 = 0xC3;
	//VirtualProtect((LPVOID)0x4C9890,1,oldProt,&oldProt2);

    /*
    004C021D   B0 00            MOV AL,0
    004C021F   90               NOP
    004C0220   90               NOP
    004C0221   90               NOP
    */
    VirtualProtect((LPVOID)0x4C01F0,5,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x4C01F0 = 0xB0;
	*(BYTE *)0x4C01F1 = 0x00;
    *(BYTE *)0x4C01F2 = 0x90;
    *(BYTE *)0x4C01F3 = 0x90;
    *(BYTE *)0x4C01F4 = 0x90;
	VirtualProtect((LPVOID)0x4C01F0,5,oldProt,&oldProt2);    

    // Disable MakePlayerSafe
	VirtualProtect((LPVOID)0x56E870,3,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x56E870 = 0xC2;
	*(BYTE *)0x56E871 = 0x08;
	*(BYTE *)0x56E872 = 0x00;
	VirtualProtect((LPVOID)0x56E870,3,oldProt,&oldProt2);    

    // Disable call to FxSystem_c__GetCompositeMatrix in CAEFireAudioEntity::UpdateParameters 
    // that was causing a crash - spent ages debugging, the crash happens if you create 40 or 
    // so vehicles that catch fire (upside down) then delete them, repeating a few times.
	VirtualProtect((LPVOID)0x4DCF87,6,PAGE_EXECUTE_READWRITE,&oldProt);		
    memset((void*)0x4DCF87,0x90,6);
	VirtualProtect((LPVOID)0x4DCF87,6,oldProt,&oldProt2);
    
    /*
    // DISABLE CPed__RemoveBodyPart
	VirtualProtect((LPVOID)0x5F0140,3,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x5F0140 = 0xC2;
    *(BYTE *)0x5F0141 = 0x08;
    *(BYTE *)0x5F0142 = 0x00;
	VirtualProtect((LPVOID)0x5F0140,3,oldProt,&oldProt2);
    */

    // ALLOW picking up of all vehicles (GTA doesn't allow picking up of 'locked' script created vehicles)
	VirtualProtect((LPVOID)0x6A436C,2,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x6A436C = 0x90;
    *(BYTE *)0x6A436D = 0x90;
	VirtualProtect((LPVOID)0x6A436C,2,oldProt,&oldProt2);

    // MAKE CEntity::GetIsOnScreen always return true, experimental
   /* VirtualProtect((LPVOID)0x534540,3,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x534540 = 0xB0;
    *(BYTE *)0x534541 = 0x01;
    *(BYTE *)0x534542 = 0xC3;
	VirtualProtect((LPVOID)0x534540,3,oldProt,&oldProt2);
*/    
    //DISABLE CPad::ReconcileTwoControllersInput
	/*VirtualProtect((LPVOID)0x53F530,3,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x53F530 = 0xC2;
    *(BYTE *)0x53F531 = 0x0C;
    *(BYTE *)0x53F532 = 0x00;
	VirtualProtect((LPVOID)0x53F530,3,oldProt,&oldProt2);

	VirtualProtect((LPVOID)0x53EF80,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x53EF80 = 0xC3;
	VirtualProtect((LPVOID)0x53EF80,1,oldProt,&oldProt2);

    VirtualProtect((LPVOID)0x541DDC,2,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x541DDC = 0xEB;
    *(BYTE *)0x541DDD = 0x60;
	VirtualProtect((LPVOID)0x541DDC,2,oldProt,&oldProt2);
*/
    // DISABLE big buildings (test)
    /*VirtualProtect((LPVOID)0x533150,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(char*)0x533150 = 0xC3;
	VirtualProtect((LPVOID)0x533150,1,oldProt,&oldProt2);*/
    
	// PREVENT THE RADIO OR ENGINE STOPPING WHEN PLAYER LEAVES VEHICLE
	// THIS ON ITS OWN will cause sounds to be left behind and other artifacts
	/*
	VirtualProtect((LPVOID)0x4FB8C0,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	memset((void *)0x4FB8C0, 0xC3, 1);
	VirtualProtect((LPVOID)0x4FB8C0,1,oldProt,&oldProt2);
	*/


/*	VirtualProtect((LPVOID)0x4FBA3E,5,PAGE_EXECUTE_READWRITE,&oldProt);		
	memset((void *)0x4FBA3E, 0x90, 5);
	VirtualProtect((LPVOID)0x4FBA3E,5,oldProt,&oldProt2);*/
	

	// DISABLE REPLAYS
/*	VirtualProtect((LPVOID)0x460500,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	memset((void *)0x460500, 0xC3, 1);
	VirtualProtect((LPVOID)0x460500,1,oldProt,&oldProt2);
*/
    // PREVENT the game from making dummy objects (may fix a crash, guesswork really)
    // This seems to work, but doesn't actually fix anything. Maybe a reason to do it in the future.
    //00615FE3     EB 09          JMP SHORT gta_sa_u.00615FEE
	/*VirtualProtect((LPVOID)0x615FE3,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	memset((void *)0x615FE3, 0xEB, 1);
	VirtualProtect((LPVOID)0x615FE3,1,oldProt,&oldProt2);*/

    // Make combines eat players *untested*
	//VirtualProtect((LPVOID)0x6A9739,6,PAGE_EXECUTE_READWRITE,&oldProt);		
	//memset ( (LPVOID)0x6A9739, 0x90, 6 );
	//VirtualProtect((LPVOID)0x6A9739,6,oldProt,&oldProt2);
    
    // Players always lean out whatever the camera mode
    // 00621983     EB 13          JMP SHORT hacked_g.00621998
	VirtualProtect((LPVOID)0x621983,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x621983 = 0xEB;
	VirtualProtect((LPVOID)0x621983,1,oldProt,&oldProt2);

    
    // Players can fire drivebys whatever camera mode
    // 627E01 - 6 bytes
	VirtualProtect((LPVOID)0x627E01,6,PAGE_EXECUTE_READWRITE,&oldProt);		
	memset ( (LPVOID)0x627E01, 0x90, 6 );
	VirtualProtect((LPVOID)0x627E01,6,oldProt,&oldProt2);

	VirtualProtect((LPVOID)0x62840D,6,PAGE_EXECUTE_READWRITE,&oldProt);		
	memset ( (LPVOID)0x62840D, 0x90, 6 );
	VirtualProtect((LPVOID)0x62840D,6,oldProt,&oldProt2);

	// Satchel crash fix
	// C89110: satchel (bomb) positions pointer?
	// C891A8+4: satchel (model) positions pointer? gets set to NULL on player death, causing an access violation
	// C891A8+12: satchel (model) disappear time (in SystemTime format). 738F99 clears the satchel when VAR_SystemTime is larger.
	VirtualProtect((LPVOID)0x738F3A,83,PAGE_EXECUTE_READWRITE,&oldProt);
	memset ( (LPVOID)0x738F3A, 0x90, 83 );
	VirtualProtect((LPVOID)0x738F3A,83,oldProt,&oldProt2);

    // Prevent gta stopping driveby players from falling off
	VirtualProtect((LPVOID)0x6B5B17,6,PAGE_EXECUTE_READWRITE,&oldProt);
	memset ( (LPVOID)0x6B5B17, 0x90, 6 );
	VirtualProtect((LPVOID)0x6B5B17,6,oldProt,&oldProt2);

    // Increase VehicleStruct pool size
	VirtualProtect((LPVOID)0x5B8FE4,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x5B8FE4 = 0x7F; // its signed so the higest you can go with this is 0x7F before it goes negative = crash
	VirtualProtect((LPVOID)0x5B8FE4,1,oldProt,&oldProt2);
    
	/*
    // CTaskSimpleCarDrive: Swaps driveby for gang-driveby for drivers
	VirtualProtect((LPVOID)0x6446A7,6,PAGE_EXECUTE_READWRITE,&oldProt);		
	memset ( (LPVOID)0x6446A7, 0x90, 6 );
	VirtualProtect((LPVOID)0x6446A7,6,oldProt,&oldProt2);
    
    // CTaskSimpleCarDrive: Swaps driveby for gang-driveby for passengers
	VirtualProtect((LPVOID)0x6446BD,6,PAGE_EXECUTE_READWRITE,&oldProt);		
	memset ( (LPVOID)0x6446BD, 0x90, 6 );
	VirtualProtect((LPVOID)0x6446BD,6,oldProt,&oldProt2);
	*/
    

	// DISABLE PLAYING REPLAYS
	VirtualProtect((LPVOID)0x460390,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	memset((void *)0x460390, 0xC3, 1);
	VirtualProtect((LPVOID)0x460390,1,oldProt,&oldProt2);

	VirtualProtect((LPVOID)0x4600F0,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	memset((void *)0x4600F0, 0xC3, 1);
	VirtualProtect((LPVOID)0x4600F0,1,oldProt,&oldProt2);

	VirtualProtect((LPVOID)0x45F050,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	memset((void *)0x45F050, 0xC3, 1);
	VirtualProtect((LPVOID)0x45F050,1,oldProt,&oldProt2);

	// DISABLE CHEATS
	VirtualProtect((LPVOID)0x439AF0,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	memset((void *)0x439AF0, 0xC3, 1);
	VirtualProtect((LPVOID)0x439AF0,1,oldProt,&oldProt2);
		
	VirtualProtect((LPVOID)0x438370,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	memset((void *)0x438370, 0xC3, 1);
	VirtualProtect((LPVOID)0x438370,1,oldProt,&oldProt2);


    // DISABLE GARAGES
    VirtualProtect((LPVOID)0x44AA89,6,PAGE_EXECUTE_READWRITE,&oldProt);		
    *(BYTE *)(0x44AA89 + 0) = 0xE9;
    *(BYTE *)(0x44AA89 + 1) = 0x28;
    *(BYTE *)(0x44AA89 + 2) = 0x01;
    *(BYTE *)(0x44AA89 + 3) = 0x00;
    *(BYTE *)(0x44AA89 + 4) = 0x00;
    *(BYTE *)(0x44AA89 + 5) = 0x90;
    VirtualProtect((LPVOID)0x44AA89,6,oldProt,&oldProt2);

    VirtualProtect((LPVOID)0x44C7E0,212,PAGE_EXECUTE_READWRITE,&oldProt);
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
    VirtualProtect((LPVOID)0x44C7E0,212,oldProt,&oldProt2);

    VirtualProtect((LPVOID)0x44C39A,6,PAGE_EXECUTE_READWRITE,&oldProt);
    *(BYTE *)(0x44C39A + 0) = 0x0F;
    *(BYTE *)(0x44C39A + 1) = 0x84;
    *(BYTE *)(0x44C39A + 2) = 0x24;
    *(BYTE *)(0x44C39A + 3) = 0x04;
    *(BYTE *)(0x44C39A + 4) = 0x00;
    *(BYTE *)(0x44C39A + 5) = 0x00;
    VirtualProtect((LPVOID)0x44C39A,6,oldProt,&oldProt2);

    // Avoid garage doors closing when you change your model
    VirtualProtect((LPVOID)0x4486F7,4,PAGE_EXECUTE_READWRITE,&oldProt);
    memset((LPVOID)0x4486F7, 0x90, 4);
    VirtualProtect((LPVOID)0x4486F7,4,oldProt,&oldProt2);
    

    // Disable CStats::IncrementStat (returns at start of function)
    VirtualProtect((LPVOID)0x55C180,1,PAGE_EXECUTE_READWRITE,&oldProt);
	*(BYTE *)0x55C180 = 0xC3;
	VirtualProtect((LPVOID)0x55C180,1,oldProt,&oldProt2); 
	/*
	VirtualProtect((LPVOID)0x55C180,648,PAGE_EXECUTE_READWRITE,&oldProt);
	memset((void *)0x55C1A9, 0x90, 14 );
	memset((void *)0x55C1DD, 0x90, 7 );
	VirtualProtect((LPVOID)0x55C180,648,oldProt,&oldProt2);
	*/

	// DISABLE STATS DECREMENTING
	VirtualProtect((LPVOID)0x559FA0,204,PAGE_EXECUTE_READWRITE,&oldProt);
	memset((void *)0x559FD5, 0x90, 7 );
	memset((void *)0x559FEB, 0x90, 7 );
	VirtualProtect((LPVOID)0x559FA0,204,oldProt,&oldProt2);

	// DISABLE STATS MESSAGES
	VirtualProtect((LPVOID)0x55B980,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	memset((void *)0x55B980, 0xC3, 1);
	VirtualProtect((LPVOID)0x55B980,1,oldProt,&oldProt2);

	VirtualProtect((LPVOID)0x559760,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	memset((void *)0x559760, 0xC3, 1);
	VirtualProtect((LPVOID)0x559760,1,oldProt,&oldProt2);

    // ALLOW more than 8 players (crash with more if this isn't done)
    //0060D64D   90               NOP
    //0060D64E   E9 9C000000      JMP gta_sa.0060D6EF
	VirtualProtect((LPVOID)0x60D64D,2,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x60D64D = 0x90;
    *(BYTE *)0x60D64E = 0xE9;
	VirtualProtect((LPVOID)0x60D64D,2,oldProt,&oldProt2);

    // PREVENT CJ smoking and drinking like an addict
    //005FBA26   EB 29            JMP SHORT gta_sa.005FBA51
	VirtualProtect((LPVOID)0x5FBA26,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x5FBA26 = 0xEB;
	VirtualProtect((LPVOID)0x5FBA26,1,oldProt,&oldProt2);

    // PREVENT the camera from messing up for drivebys for vehicle drivers
	VirtualProtect((LPVOID)0x522423,2,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x522423 = 0x90;
    *(BYTE *)0x522424 = 0x90;
	VirtualProtect((LPVOID)0x522423,2,oldProt,&oldProt2);
    
    LPVOID patchAddress = NULL;
    // ALLOW ALT+TABBING WITHOUT PAUSING
    VirtualProtect((LPVOID)0x748ADD,2,PAGE_EXECUTE_READWRITE,&oldProt);
    //if ( pGameInterface->GetGameVersion() == GAME_VERSION_US ) // won't work as pGameInterface isn't inited
    if ( *(BYTE *)0x748ADD == 0xFF && *(BYTE *)0x748ADE == 0x53 )
        patchAddress = (LPVOID)0x748A8D;
    else
        patchAddress = (LPVOID)0x748ADD;

    VirtualProtect((LPVOID)patchAddress,6,oldProt,&oldProt2);
    VirtualProtect((LPVOID)patchAddress,6,PAGE_EXECUTE_READWRITE,&oldProt);
    memset(patchAddress, 0x90, 6);
	VirtualProtect((LPVOID)patchAddress,6,oldProt,&oldProt2);

    // CENTER VEHICLE NAME and ZONE NAME messages
    // 0058B0AD   6A 02            PUSH 2 // orientation
    // VEHICLE
	VirtualProtect((LPVOID)0x58B0AE,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x58B0AE = 0x00;
	VirtualProtect((LPVOID)0x58B0AE,1,oldProt,&oldProt2);

    // ZONE
	VirtualProtect((LPVOID)0x58AD56,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x58AD56 = 0x00;
	VirtualProtect((LPVOID)0x58AD56,1,oldProt,&oldProt2);

    // 85953C needs to equal 320.0 to center the text (640.0 being the base width)
	VirtualProtect((LPVOID)0x85953C,4,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(float *)0x85953C = 320.0f;
	VirtualProtect((LPVOID)0x85953C,4,oldProt,&oldProt2);

    // 0058B147   D80D 0C958500    FMUL DWORD PTR DS:[85950C] // the text needs to be moved to the left
    //VEHICLE
	VirtualProtect((LPVOID)0x58B149,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x58B149 = 0x3C;
	VirtualProtect((LPVOID)0x58B149,1,oldProt,&oldProt2);

    //ZONE
	VirtualProtect((LPVOID)0x58AE52,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x58AE52 = 0x3C;
	VirtualProtect((LPVOID)0x58AE52,1,oldProt,&oldProt2);

	// DISABLE SAM SITES
	VirtualProtect((LPVOID)0x5A07D0,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x5A07D0 = 0xC3;
	VirtualProtect((LPVOID)0x5A07D0,1,oldProt,&oldProt2);

	// DISABLE TRAINS (AUTO GENERATED ONES)
	VirtualProtect((LPVOID)0x6F7900,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x6F7900 = 0xC3;
	VirtualProtect((LPVOID)0x6F7900,1,oldProt,&oldProt2);
    
    // Prevent TRAINS spawning with PEDs
	VirtualProtect((LPVOID)0x6F7865,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x6F7865 = 0xEB;
	VirtualProtect((LPVOID)0x6F7865,1,oldProt,&oldProt2);

	// DISABLE PLANES
	VirtualProtect((LPVOID)0x6CD2F0,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x6CD2F0 = 0xC3;
	VirtualProtect((LPVOID)0x6CD2F0,1,oldProt,&oldProt2);
	
	// DISABLE EMERGENCY VEHICLES
	VirtualProtect((LPVOID)0x42B7D0,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x42B7D0 = 0xC3;
	VirtualProtect((LPVOID)0x42B7D0,1,oldProt,&oldProt2);

	// DISABLE CAR GENERATORS
	VirtualProtect((LPVOID)0x6F3F40,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x6F3F40 = 0xC3;
	VirtualProtect((LPVOID)0x6F3F40,1,oldProt,&oldProt2);

    // DISABLE CEntryExitManager::Update (they crash when you enter anyway)
	VirtualProtect((LPVOID)0x440D10,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x440D10 = 0xC3;
	VirtualProtect((LPVOID)0x440D10,1,oldProt,&oldProt2);

	// Disable MENU AFTER alt + tab
	//0053BC72   C605 7B67BA00 01 MOV BYTE PTR DS:[BA677B],1
	VirtualProtect((LPVOID)0x53BC78,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x53BC78 = 0x00;
	VirtualProtect((LPVOID)0x53BC78,1,oldProt,&oldProt2);

	// DISABLE HUNGER MESSAGES
	VirtualProtect((LPVOID)0x56E740,5,PAGE_EXECUTE_READWRITE,&oldProt);		
	memset ( (LPVOID)0x56E740, 0x90, 5 );
	VirtualProtect((LPVOID)0x56E740,5,oldProt,&oldProt2);

	// DISABLE RANDOM VEHICLE UPGRADES
	VirtualProtect((LPVOID)0x6B0BC2,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	memset ( (LPVOID)0x6B0BC2, 0xEB, 1 );
	VirtualProtect((LPVOID)0x6B0BC2,1,oldProt,&oldProt2);	

	// DISABLE CPOPULATION::UPDATE - DOES NOT prevent vehicles - only on-foot peds
	/*VirtualProtect((LPVOID)0x616650,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x616650 = 0xC3;
    *(BYTE *)0xA43088 = 1;
	VirtualProtect((LPVOID)0x616650,1,oldProt,&oldProt2);*/

	// SORT OF HACK to make peds always walk around, even when in free-camera mode (in the editor)
	VirtualProtect((LPVOID)0x53C017,2,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x53C017 = 0x90;
	*(BYTE *)0x53C018 = 0x90;
	VirtualProtect((LPVOID)0x53C017,2,oldProt,&oldProt2);

	// DISABLE random cars
	VirtualProtect((LPVOID)0x4341C0,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	//*(BYTE *)0x4341C0 = 0xC3;
	VirtualProtect((LPVOID)0x4341C0,1,oldProt,&oldProt2);
	
	// DISABLE heat flashes
	/*VirtualProtect((LPVOID)0x6E3521,2,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x6E3521 = 0x90;
	*(BYTE *)0x6E3522 = 0xE9;
	VirtualProtect((LPVOID)0x4341C0,2,oldProt,&oldProt2);*/

	// DECREASE ROF for missiles from hydra
	// 006D462C     81E1 E8030000  AND ECX,3E8
	// 006D4632     81C1 E8030000  ADD ECX,3E8
    /*
	VirtualProtect((LPVOID)0x6D462E,12,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x6D462E = 0xE8;
	*(BYTE *)0x6D462F = 0x03;
	*(BYTE *)0x6D4634 = 0xE8;
	*(BYTE *)0x6D4635 = 0x03;
	VirtualProtect((LPVOID)0x6D462E,12,oldProt,&oldProt2);
    */

	// HACK to allow boats to be rotated
	/*
	006F2089   58               POP EAX
	006F208A   90               NOP
	006F208B   90               NOP
	006F208C   90               NOP
	006F208D   90               NOP
	*/
	VirtualProtect((LPVOID)0x6F2089,5,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x6F2089 = 0x58;
	memset((void *)0x6F208A,0x90,4);
	VirtualProtect((LPVOID)0x6F2089,5,oldProt,&oldProt2);

    // Prevent the game deleting _any_ far away vehicles - will cause issues for population vehicles in the future
    VirtualProtect((LPVOID)0x42CD10,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x42CD10 = 0xC3;
	VirtualProtect((LPVOID)0x42CD10,1,oldProt,&oldProt2);

    // DISABLE real-time shadows for peds
    VirtualProtect((LPVOID)0x5E68A0,1,PAGE_EXECUTE_READWRITE,&oldProt);		
    *(BYTE *)0x5E68A0 = 0xEB;
	VirtualProtect((LPVOID)0x5E68A0,1,oldProt,&oldProt2);

    // and some more, just to be safe
    //00542483   EB 0B            JMP SHORT gta_sa.00542490
    VirtualProtect((LPVOID)0x542483,1,PAGE_EXECUTE_READWRITE,&oldProt);		
    *(BYTE *)0x542483 = 0xEB;
	VirtualProtect((LPVOID)0x542483,1,oldProt,&oldProt2);

    // DISABLE weapon pickups
    VirtualProtect((LPVOID)0x5B47B0,1,PAGE_EXECUTE_READWRITE,&oldProt);		
    *(BYTE *)0x5B47B0 = 0xC3;
	VirtualProtect((LPVOID)0x5B47B0,1,oldProt,&oldProt2);

    // INCREASE CEntyInfoNode pool size
    //00550FB9   68 F4010000      PUSH 1F4
    VirtualProtect((LPVOID)0x550FBA,2,PAGE_EXECUTE_READWRITE,&oldProt);		
    /*
    *(BYTE *)0x550FBA = 0xE8;
    *(BYTE *)0x550FBB = 0x03;
    */
    *(BYTE *)0x550FBA = 0x00;
    *(BYTE *)0x550FBB = 0x10;
	VirtualProtect((LPVOID)0x550FBA,2,oldProt,&oldProt2);    

    
	/*VirtualProtect((LPVOID)0x469F00,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x469F00 = 0xC3;
	VirtualProtect((LPVOID)0x469F00,1,oldProt,&oldProt2);*/

	// CCAM::PROCESSFIXED remover
/*
	VirtualProtect((LPVOID)0x51D470,3,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x51D470 = 0xC2;
	*(BYTE *)0x51D471 = 0x10;
	*(BYTE *)0x51D472 = 0x00;
	VirtualProtect((LPVOID)0x51D470,3,oldProt,&oldProt2);

*/

	// HACK to prevent RealTimeShadowManager crash
	// 00542483     EB 0B          JMP SHORT gta_sa_u.00542490
	/*VirtualProtect((LPVOID)0x542483,1,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x542483 = 0xEB;
	VirtualProtect((LPVOID)0x542483,1,oldProt,&oldProt2);
*/
	
	//InitShotsyncHooks();

    //DISABLE CPad::ReconcileTwoControllersInput
    VirtualProtect((LPVOID)0x53F530,3,PAGE_EXECUTE_READWRITE,&oldProt);		
    *(BYTE *)0x53F530 = 0xC2;
    *(BYTE *)0x53F531 = 0x0C;
    *(BYTE *)0x53F532 = 0x00;
    VirtualProtect((LPVOID)0x53F530,3,oldProt,&oldProt2);

    VirtualProtect((LPVOID)0x53EF80,1,PAGE_EXECUTE_READWRITE,&oldProt);		
    *(BYTE *)0x53EF80 = 0xC3;
    VirtualProtect((LPVOID)0x53EF80,1,oldProt,&oldProt2);

    VirtualProtect((LPVOID)0x541DDC,2,PAGE_EXECUTE_READWRITE,&oldProt);		
    *(BYTE *)0x541DDC = 0xEB;
    *(BYTE *)0x541DDD = 0x60;
    VirtualProtect((LPVOID)0x541DDC,2,oldProt,&oldProt2);

    // DISABLE CWanted Helis (always return 0 from CWanted::NumOfHelisRequired)
    VirtualProtect((LPVOID)0x561FA4,2,PAGE_EXECUTE_READWRITE,&oldProt);		
    *(BYTE *)0x561FA4 = 0x90;
    *(BYTE *)0x561FA5 = 0x90;
    VirtualProtect((LPVOID)0x561FA4,2,oldProt,&oldProt2);

    // DISABLE  CWanted__UpdateEachFrame
    VirtualProtect((LPVOID)0x53BFF6,5,PAGE_EXECUTE_READWRITE,&oldProt);		
    memset( (void*)0x53BFF6, 0x90, 5 );
    VirtualProtect((LPVOID)0x53BFF6,5,oldProt,&oldProt2);

    // DISABLE CWanted__Update
    VirtualProtect((LPVOID)0x60EBCC,5,PAGE_EXECUTE_READWRITE,&oldProt);		
    memset( (void*)0x60EBCC, 0x90, 5 );
    VirtualProtect((LPVOID)0x60EBCC,5,oldProt,&oldProt2);

	// Disable armour-increase upon entering an enforcer
	VirtualProtect((LPVOID)0x6D189B,1,PAGE_EXECUTE_READWRITE,&oldProt);
	*(BYTE *)0x6D189B = 0x06;
	VirtualProtect((LPVOID)0x6D189B,1,oldProt,&oldProt2);

	// Removes the last weapon pickups from interiors as well
	VirtualProtect((LPVOID)0x591F90,1,PAGE_EXECUTE_READWRITE,&oldProt);
	*(BYTE *)0x591F90 = 0xC3;
	VirtualProtect((LPVOID)0x591F90,1,oldProt,&oldProt2);

	// Trains may infact go further than Los Santos
	VirtualProtect((LPVOID)0x4418E0,1,PAGE_EXECUTE_READWRITE,&oldProt);
	*(BYTE *)0x4418E0 = 0xC3;
	VirtualProtect((LPVOID)0x4418E0,1,oldProt,&oldProt2);

    // EXPERIMENTAL - disable unloading of cols
    //VirtualProtect((LPVOID)0x4C4EDA,10,PAGE_EXECUTE_READWRITE,&oldProt);		
   // memset( (void*)0x4C4EDA, 0x90, 10 );
    //VirtualProtect((LPVOID)0x4C4EDA,10,oldProt,&oldProt2);

    // Make CTaskComplexSunbathe::CanSunbathe always return true
    VirtualProtect((LPVOID)0x632140,3,PAGE_EXECUTE_READWRITE,&oldProt);		
	*(BYTE *)0x632140 = 0xB0;
    *(BYTE *)0x632141 = 0x01;
    *(BYTE *)0x632142 = 0xC3;
	VirtualProtect((LPVOID)0x632140,3,oldProt,&oldProt2); 
    
    // Stop CTaskSimpleCarDrive::ProcessPed from exiting passengers with CTaskComplexSequence (some timer check)
    VirtualProtect((LPVOID)0x644C18,2,PAGE_EXECUTE_READWRITE,&oldProt);	
    *(BYTE *)0x644C18 = 0x90;
	*(BYTE *)0x644C19 = 0xE9;
	VirtualProtect((LPVOID)0x644C18,2,oldProt,&oldProt2);

    // Stop CPlayerPed::ProcessControl from calling CVisibilityPlugins::SetClumpAlpha
    VirtualProtect((LPVOID)0x5E8E84,5,PAGE_EXECUTE_READWRITE,&oldProt);	
    memset ( (void*)0x5E8E84, 0x90, 5 );
	VirtualProtect((LPVOID)0x5E8E84,5,oldProt,&oldProt2);

    // Stop CVehicle::UpdateClumpAlpha from calling CVisibilityPlugins::SetClumpAlpha
    VirtualProtect((LPVOID)0x6D29CB,5,PAGE_EXECUTE_READWRITE,&oldProt);	
    memset ( (void*)0x6D29CB, 0x90, 5 );
	VirtualProtect((LPVOID)0x6D29CB,5,oldProt,&oldProt2);    

    // Disable CVehicle::DoDriveByShootings
    VirtualProtect((LPVOID)0x741FD0,3,PAGE_EXECUTE_READWRITE,&oldProt);	
    memset ( (void*)0x741FD0, 0x90, 3 );
    *(BYTE *)0x741FD0 = 0xC3;
	VirtualProtect((LPVOID)0x741FD0,3,oldProt,&oldProt2);

    // Disable CTaskSimplePlayerOnFoot::PlayIdleAnimations (ret 4)
    VirtualProtect((LPVOID)0x6872C0,3,PAGE_EXECUTE_READWRITE,&oldProt);	
    *(BYTE *)0x6872C0 = 0xC2;
    *(BYTE *)0x6872C1 = 0x04;
    *(BYTE *)0x6872C2 = 0x00;
	VirtualProtect((LPVOID)0x6872C0,3,oldProt,&oldProt2);    

    /*
    // Disable forcing of ped animations to the player one in CPlayerPed::ProcessAnimGroups
    VirtualProtect( (LPVOID)0x609A44, 21, PAGE_EXECUTE_READWRITE, &oldProt );
    memset ( (LPVOID)0x609A44, 0x90, 21 );
    VirtualProtect( (LPVOID)0x609A44, 21, oldProt, &oldProt2 );
    */
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


CRemoteDataStorage * CMultiplayerSA::AddRemoteDataStorage ( CPlayerPed * player )
{
    // Create the remote data storage and add it to the list. Return it if success
    CRemoteDataStorageSA * data = new CRemoteDataStorageSA ( player );
    if ( CRemoteDataSA::AddRemoteDataStorage ( data ) )
        return data;

    // Otherwize delete it and return NULL
    delete data;
    return NULL;
}

void CMultiplayerSA::RemoveRemoteDataStorage ( CRemoteDataStorage * data )
{
    // Remove it from the list and delete it
    CRemoteDataSA::RemoveRemoteDataStorage ( data );
    delete (CRemoteDataStorageSA*)data;
}

CPed * CMultiplayerSA::GetContextSwitchedPed ( void )
{
    return pContextSwitchedPed;
}

void CMultiplayerSA::AllowWindowsCursorShowing ( bool bAllow )
{
    DWORD oldProt, oldProt2;
    /*
    0074821D   6A 00            PUSH 0
    0074821F   FF15 EC828500    CALL DWORD PTR DS:[<&USER32.ShowCursor>] ; USER32.ShowCursor
    00748225   6A 00            PUSH 0
    00748227   FF15 9C828500    CALL DWORD PTR DS:[<&USER32.SetCursor>]  ; USER32.SetCursor
    */
    BYTE originalCode[16] = {0x6A, 0x00, 0xFF, 0x15, 0xEC, 0x82, 0x85, 0x00, 0x6A, 0x00, 0xFF, 0x15, 0x9C, 0x82, 0x85, 0x00};

	VirtualProtect( (LPVOID)ADDR_CursorHiding,16,PAGE_EXECUTE_READWRITE,&oldProt);

    if ( bAllow )
    {
	    memset ( (LPVOID)ADDR_CursorHiding, 0x90, 16 );
    }
    else
    {
        memcpy ( (LPVOID)ADDR_CursorHiding, &originalCode, 16 );
    }

	VirtualProtect((LPVOID)ADDR_CursorHiding,16,oldProt,&oldProt2);
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

void CMultiplayerSA::DisableClouds ( bool bDisabled )
{
    //volumetric clouds
	if ( bDisabled )
		*(BYTE *)0x716380 = 0xC3;
	else
		*(BYTE *)0x716380 = 0xA1;

    // normal clouds
    //0071395A     90             NOP
	if ( bDisabled )
		*(BYTE *)0x713950 = 0xC3;
	else
		*(BYTE *)0x713950 = 0x83;

    // plane trails (not really clouds, but they're sort of vapour)
	if ( bDisabled )
    {
		*(BYTE *)0x717180 = 0xC2;
        *(BYTE *)0x717181 = 0x04;
        *(BYTE *)0x717182 = 0x00;
    }
	else
    {
		*(BYTE *)0x717180 = 0x83;
        *(BYTE *)0x717181 = 0xEC;
        *(BYTE *)0x717182 = 0x08;
    }
}

void CMultiplayerSA::SetSkyColor ( unsigned char TopRed, unsigned char TopGreen, unsigned char TopBlue, unsigned char BottomRed, unsigned char BottomGreen, unsigned char BottomBlue )
{
	*(BYTE *)0x561760 = 0xC3;

    *(BYTE *)0xB7C4C4 = TopRed;
    *(BYTE *)0xB7C4C6 = TopGreen;
    *(BYTE *)0xB7C4C8 = TopBlue;

    *(BYTE *)0xB7C4CA = BottomRed;
    *(BYTE *)0xB7C4CC = BottomGreen;
    *(BYTE *)0xB7C4CE = BottomBlue;
}

void CMultiplayerSA::ResetSky ( void )
{
	*(BYTE *)0x561760 = 0xA1;
}

float CMultiplayerSA::GetWaveLevel ( void )
{
    return *(float *)0xC812E8;
}

void CMultiplayerSA::SetWaveLevel ( float fWaveLevel )
{
    if ( fWaveLevel >= 0 )
    {
        // DISABLE the game reseting the wave level
        *(BYTE *)0x72C665 = 0xDD;
        *(BYTE *)0x72C666 = 0xD8;
        memset( (void*)0x72C667, 0x90, 4 );
        memset( (void*)0x72C659, 0x90, 10 );

        *(float *)0xC812E8 = fWaveLevel;
    }
    else
    {
        *(BYTE *)0x72C665 = 0xD9;
        *(BYTE *)0x72C666 = 0x1D;
        *(BYTE *)0x72C667 = 0xE8;
        *(BYTE *)0x72C668 = 0x12;
        *(BYTE *)0x72C669 = 0xC8;
        *(BYTE *)0x72C66A = 0x00;

        *(BYTE *)0x72C659 = 0xC7;
        *(BYTE *)0x72C65A = 0x05;
        *(BYTE *)0x72C65B = 0xE8;
        *(BYTE *)0x72C65C = 0x12;
        *(BYTE *)0x72C65D = 0xC8;
        *(BYTE *)0x72C65E = 0x00;
        *(BYTE *)0x72C65F = 0x00;
        *(BYTE *)0x72C660 = 0x00;
        *(BYTE *)0x72C661 = 0x80;
        *(BYTE *)0x72C662 = 0x3F;

        *(float *)0xC812E8 = 0.6f;
    }
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

VOID _declspec(naked) HOOK_FindPlayerCoors()
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

VOID _declspec(naked) HOOK_CStreaming_Update_Caller()
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

VOID _declspec(naked) HOOK_CHud_Draw_Caller()
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

VOID _declspec(naked) HOOK_FindPlayerCentreOfWorld()
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

VOID _declspec(naked) HOOK_FindPlayerHeading()
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
VOID _declspec(naked) HOOK_CCustomRoadsignMgr__RenderRoadsignAtomic()
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
    CVehicle * pVehicle = pGameInterface->GetPools ()->GetVehicle ( vehicle );
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


CEntitySAInterface * fireAreaEffectInterface = 0;
void _declspec(naked) HOOK_CWeapon_FireAreaEffect ()
{    
    _asm
    {
		mov eax, [esp+32]
		mov fireAreaEffectInterface, eax
        mov eax, FUNC_CWeapon_FireAreaEffect
        jmp eax
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
                pExplosionCreator = pGameInterface->GetPools ()->GetVehicle ( (CVehicleSAInterface*) pInterface );
                break;
            }

            case ENTITY_TYPE_OBJECT:
            {
                //pExplosionCreator = pGameInterface->GetPools ()->GetObject ( (CObjectSAInterface*) pInterface );
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
                pExplodingEntity = dynamic_cast < CEntity * > ( pGameInterface->GetPools ()->GetPed ( (DWORD*) pExplodingEntityInterface ) );
                break;
            }

            case ENTITY_TYPE_VEHICLE:
            {
                pExplodingEntity = dynamic_cast < CEntity * > ( pGameInterface->GetPools ()->GetVehicle ( (CVehicleSAInterface*) pExplodingEntityInterface ) );
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

VOID _declspec(naked) HOOK_CExplosion_AddExplosion()
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


VOID _declspec(naked) HOOK_CRealTimeShadowManager__ReturnRealTimeShadow()
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
VOID _declspec(naked) HOOK_CTaskComplexJump__CreateSubTask()
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


void CCam_ProcessFixed ( class CCamSAInterface* pCamInterface )
{
    CCam* pCam = static_cast < CCameraSA* > ( pGameInterface->GetCamera () )->GetCam ( pCamInterface );

    if ( m_pProcessCamHandler )
    {
        m_pProcessCamHandler ( pCam );
    }
}



CCamSAInterface* CCam_ProcessFixed_pCam;

void _declspec(naked) HOOK_CCam_ProcessFixed ()
{
    _asm
    {
        // Store the CCam* and push all the registers
        mov     CCam_ProcessFixed_pCam, ecx
        pushad
    }

    CCam_ProcessFixed ( CCam_ProcessFixed_pCam );

    _asm
    {
        // Restore all the registers
        popad

        // Abort the func
        ret 10h
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

VOID _declspec(naked) HOOK_CRunningScript_Process()
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
	memset((VOID *)0x6B5A10, 0xC3, 1);

	//006B7449     E9 FF000000    JMP gta_sa.006B754D
	memset((VOID *)0x6B7449, 0xE9, 1);
	memset((VOID *)(0x6B7449+1), 0xFF, 1);
	memset((VOID *)(0x6B7449+2), 0x00, 1);

	//006B763C     E9 01010000    JMP gta_sa.006B7742
	memset((VOID *)0x6B763C, 0xE9, 1);
	memset((VOID *)(0x6B763C+1), 0x01, 1);
	memset((VOID *)(0x6B763C+2), 0x01, 1);
	memset((VOID *)(0x6B763C+3), 0x00, 1);

	//006B7617     E9 26010000    JMP gta_sa.006B7742
	memset((VOID *)0x6B7617, 0xE9, 1);
	memset((VOID *)(0x6B7617+1), 0x26, 1);
	memset((VOID *)(0x6B7617+2), 0x01, 1);
	memset((VOID *)(0x6B7617+3), 0x00, 1);
	memset((VOID *)(0x6B7617+4), 0x00, 1);

	//006B62A7     EB 74          JMP SHORT gta_sa.006B631D
	memset((VOID *)0x6B62A7, 0xEB, 1);

	//006B7642     E9 FB000000    JMP gta_sa_u.006B7742
	memset((VOID *)0x6B7642, 0xE9, 1);
	memset((VOID *)(0x6B7642+1), 0xFB, 1);
	memset((VOID *)(0x6B7642+2), 0x00, 1);
	memset((VOID *)(0x6B7642+3), 0x00, 1);
	memset((VOID *)(0x6B7642+4), 0x00, 1);

	//006B7449     E9 FF000000    JMP gta_sa_u.006B754D
	memset((VOID *)0x6B7449, 0xE9, 1);
	memset((VOID *)(0x6B7449+1), 0xFF, 1);
	memset((VOID *)(0x6B7449+2), 0x00, 1);

	// For quadbikes hitting water	
	// 006A90D8   E9 29020000      JMP gta_sa.006A9306
	memset((VOID *)0x6A90D8, 0xE9, 1);
	memset((VOID *)(0x6A90D8+1), 0x29, 1);
	memset((VOID *)(0x6A90D8+2), 0x02, 1);
	memset((VOID *)(0x6A90D8+3), 0x00, 1);
	memset((VOID *)(0x6A90D8+4), 0x00, 1);
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
