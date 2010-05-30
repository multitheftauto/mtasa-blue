/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/multiplayer_shotsync.h
*  PURPOSE:     Multiplayer module shooting sync methods
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "multiplayersa_init.h"
#include <multiplayer/CMultiplayer.h>

#ifndef __CMULTIPLAYERSASHOTSYNC
#define __CMULTIPLAYERSASHOTSYNC

#include <game/CPlayerPed.h>

//#include "../game_sa/CPedIKSA.h"

#define HOOKPOS_CWeapon__Fire                           0x742300
#define HOOKPOS_CWeapon__PostFire                       0x742A02 // if the function returns true
#define HOOKPOS_CWeapon__PostFire2                      0x742423 // if the function returns false
#define HOOKPOS_CPedIK__PointGunInDirection             0x5FDC00
#define HOOKPOS_CTaskSimpleGangDriveBy__PlayerTarget    0x621A57
#define HOOKPOS_CWeapon__Fire_Sniper                    0x7424A6
#define HOOKRET_CWeapon__Fire_Sniper                    0x7424D7
#define HOOKPOS_CEventDamage__AffectsPed                0x4B35A0
#define HOOKPOS_CFireManager__StartFire                 0x539F00
#define HOOKPOS_CFireManager__StartFire_                0x53A050
#define HOOKPOS_CProjectileInfo__AddProjectile          0x737C80
#define HOOKPOS_CProjectile__CProjectile                0x5A4030
#define HOOKPOS_IKChainManager_PointArm                 0x618B66
#define HOOKPOS_IKChainManager_LookAt                   0x618970
#define HOOKPOS_IKChainManager_SkipAim                  0x62AEE7
#define HOOKPOS_CTaskSimpleUsegun_ProcessPed            0x62A380
#define HOOKPOS_CWeapon_FireInstantHit                  0x740B42
#define HOOKPOS_CWeapon_FireInstantHit_CameraMode       0x74036D
#define HOOKPOS_CWeapon_FireInstantHit_IsPlayer         0x74034C
#define HOOKPOS_CWeapon_DoBulletImpact                  0x73B550

// our stuff
VOID InitShotsyncHooks();
CShotSyncData * GetLocalPedShotSyncData ( );
VOID WriteGunDirectionDataForPed ( class CPedSAInterface * pPedInterface, float * fGunDirectionX, float * fGunDirectionY, char * cGunDirection );
bool IsLocalPlayer ( CPedSAInterface * pPedInterface );

// hooks
VOID HOOK_CWeapon__Fire();
VOID HOOK_CWeapon__PostFire ();
VOID HOOK_CWeapon__PostFire2 ();
VOID HOOK_CPedIK__PointGunInDirection ();
VOID HOOK_CTaskSimpleGangDriveBy__PlayerTarget ();
VOID HOOK_CWeapon__Fire_Sniper ();
VOID HOOK_CEventDamage__AffectsPed ();
VOID HOOK_CFireManager__StartFire ();
VOID HOOK_CFireManager__StartFire_ ();
VOID HOOK_CProjectileInfo__AddProjectile();
VOID HOOK_CProjectile__CProjectile();
VOID HOOK_IKChainManager_PointArm();
VOID HOOK_IKChainManager_LookAt();
VOID HOOK_SkipAim ();
VOID HOOK_CTaskSimpleUsegun_ProcessPed ();
VOID HOOK_CWeapon_FireInstantHit ();
VOID HOOK_CWeapon_FireInstantHit_CameraMode ();
VOID HOOK_CWeapon_FireInstantHit_IsPlayer ();
VOID HOOK_CWeapon_DoBulletImpact ();

#endif
