/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CWeaponInfoSA.h
*  PURPOSE:     Header file for weapon type information class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Chris McArthur <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_WEAPONINFO
#define __CGAMESA_WEAPONINFO

#include <game/CWeaponInfo.h>
#include <CVector.h>
#include "Common.h"

class CWeaponInfoSAInterface // 112 byte long class
{
public:
	eFireType   m_eFireType;		// type - instant hit (e.g. pistol), projectile (e.g. rocket launcher), area effect (e.g. flame thrower)

	FLOAT		m_fTargetRange;		// max targeting range
	FLOAT		m_fWeaponRange;		// absolute gun range / default melee attack range
	int			m_modelId;			// modelinfo id
	int			m_modelId2;			// second modelinfo id
	
	eWeaponSlot m_nWeaponSlot;
	int			m_nFlags;			// flags defining characteristics

	// instead of storing pointers directly to anims, use anim association groups
	// NOTE: this is used for stealth kill anims for melee weapons
	DWORD       m_animGroup;

	//////////////////////////////////
	// Gun Data
	/////////////////////////////////
	short		m_nAmmo;				// ammo in one clip
	short		m_nDamage;				// damage inflicted per hit
	CVector	m_vecFireOffset;		// offset from weapon origin to projectile starting point
	
	// skill settings
	eWeaponSkill m_SkillLevel;		    // what's the skill level of this weapontype
	int			m_nReqStatLevel;		// what stat level is required for this skill level
	FLOAT		m_fAccuracy;			// modify accuracy of weapon
	FLOAT		m_fMoveSpeed;			// how fast can move with weapon

	// anim timings
	FLOAT		m_animLoopStart;		// start of animation loop
	FLOAT		m_animLoopEnd;			// end of animation loop
	FLOAT		m_animFireTime;			// time in animation when weapon should be fired

	FLOAT		m_anim2LoopStart;		// start of animation2 loop
	FLOAT		m_anim2LoopEnd;			// end of animation2 loop
	FLOAT		m_anim2FireTime;		// time in animation2 when weapon should be fired

	FLOAT		m_animBreakoutTime;		// time after which player can break out of attack and run off
	
	// projectile/area effect specific info
	FLOAT		m_fSpeed;				// speed of projectile
	FLOAT		m_fRadius;				// radius affected
	FLOAT		m_fLifeSpan;			// time taken for shot to dissipate
	FLOAT		m_fSpread;				// angle inside which shots are created
	
	short		m_nAimOffsetIndex;		// index into array of aiming offsets
	//////////////////////////////////
	// Melee Data
	/////////////////////////////////
	BYTE		m_defaultCombo;			// base combo for this melee weapon
	BYTE		m_nCombosAvailable;		// how many further combos are available
};

class CWeaponInfoSA : public CWeaponInfo
{
private:
	eWeaponType					weaponType;
	CWeaponInfoSAInterface		* internalInterface;
public:
	// constructor
	CWeaponInfoSA(CWeaponInfoSAInterface * weaponInfoInterface, eWeaponType weaponType) { this->internalInterface = weaponInfoInterface;  this->weaponType  = weaponType; };

	// stolen from R*'s CPedIK :)
	void SetFlag(DWORD flag)		{this->internalInterface->m_nFlags |= flag;}
	void ClearFlag(DWORD flag)		{this->internalInterface->m_nFlags &= ~flag;}
    bool IsFlagSet(DWORD flag)		{return ((this->internalInterface->m_nFlags & flag) > 0 ? 1 : 0);}	

    eWeaponModel				GetModel() { return  (eWeaponModel)this->internalInterface->m_modelId; }
	CWeaponInfoSAInterface		* GetInterface() { return this->internalInterface; };
	FLOAT						GetWeaponRange() { return GetInterface()->m_fWeaponRange; };
	VOID						SetWeaponRange ( FLOAT fRange ) { GetInterface()->m_fWeaponRange = fRange; };
	FLOAT						GetTargetRange() { return GetInterface()->m_fTargetRange; };
	VOID						SetTargetRange ( FLOAT fRange ) { GetInterface()->m_fTargetRange = fRange; };
	CVector					    * GetFireOffset() { return &GetInterface()->m_vecFireOffset; };
	VOID						SetFireOffset ( CVector * vecFireOffset ) { memcpy(&GetInterface()->m_vecFireOffset, vecFireOffset, sizeof(CVector)); };
	short						GetDamagePerHit() { return GetInterface()->m_nDamage; };
	VOID						SetDamagePerHit ( short sDamagePerHit ) { GetInterface()->m_nDamage = sDamagePerHit; };
    float                       GetAccuracy ( void )                        { return internalInterface->m_fAccuracy; };
    void                        SetAccuracy ( float fAccuracy )             { internalInterface->m_fAccuracy = fAccuracy; };
	// projectile/areaeffect only
	FLOAT						GetFiringSpeed() { return GetInterface()->m_fSpeed; };
	VOID						SetFiringSpeed ( FLOAT fFiringSpeed ) { GetInterface()->m_fSpeed = fFiringSpeed; };
	// area effect only
	FLOAT						GetRadius() { return GetInterface()->m_fRadius; };
	VOID						SetRadius ( FLOAT fRadius ) { GetInterface()->m_fRadius = fRadius; };
	FLOAT						GetLifeSpan() { return GetInterface()->m_fLifeSpan; };
	VOID						SetLifeSpan ( FLOAT fLifeSpan ) { GetInterface()->m_fLifeSpan = fLifeSpan; };
	FLOAT						GetSpread() { return GetInterface()->m_fSpread; };
	VOID						SetSpread ( FLOAT fSpread ) { GetInterface()->m_fSpread = fSpread; };
	FLOAT						GetAnimBreakoutTime() { return GetInterface()->m_animBreakoutTime; };
	VOID						SetAnimBreakoutTime ( FLOAT fBreakoutTime ) { GetInterface()->m_animBreakoutTime = fBreakoutTime; };
	eWeaponSlot					GetSlot() { return (eWeaponSlot)GetInterface()->m_nWeaponSlot; };
	VOID						SetSlot ( eWeaponSlot dwSlot ) { GetInterface()->m_nWeaponSlot = (eWeaponSlot)dwSlot; };
    eWeaponSkill                GetSkill() { return GetInterface()->m_SkillLevel; }
    void                        SetSkill ( eWeaponSkill weaponSkill ) { GetInterface ()->m_SkillLevel = weaponSkill; }
    int                         GetRequiredStatLevel () { return GetInterface()->m_nReqStatLevel; }
    void                        SetRequiredStatLevel ( int iStatLevel ) { GetInterface()->m_nReqStatLevel = iStatLevel; }
    eFireType                   GetFireType () { return GetInterface()->m_eFireType; }
};

#endif