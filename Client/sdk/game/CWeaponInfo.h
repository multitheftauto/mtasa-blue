/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CWeaponInfo.h
 *  PURPOSE:     Weapon entity information interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Common.h"

class CVector;

enum eWeaponSkill : std::uint8_t
{
    WEAPONSKILL_POOR = 0,
    WEAPONSKILL_STD,
    WEAPONSKILL_PRO,
    WEAPONSKILL_SPECIAL,            // for cops using pistols differently for example
    WEAPONSKILL_MAX_NUMBER
};

enum eFireType
{
    FIRETYPE_MELEE,
    FIRETYPE_INSTANT_HIT,
    FIRETYPE_PROJECTILE,
    FIRETYPE_AREA_EFFECT,
    FIRETYPE_CAMERA,
    FIRETYPE_USE,

    FIRETYPE_LAST_FIRETYPE
};

enum eWeaponSlot
{
    WEAPONSLOT_TYPE_UNARMED = 0,
    WEAPONSLOT_TYPE_MELEE,
    WEAPONSLOT_TYPE_HANDGUN,
    WEAPONSLOT_TYPE_SHOTGUN,
    WEAPONSLOT_TYPE_SMG,            // 4
    WEAPONSLOT_TYPE_MG,
    WEAPONSLOT_TYPE_RIFLE,
    WEAPONSLOT_TYPE_HEAVY,
    WEAPONSLOT_TYPE_THROWN,
    WEAPONSLOT_TYPE_SPECIAL,              // 9
    WEAPONSLOT_TYPE_GIFT,                 // 10
    WEAPONSLOT_TYPE_PARACHUTE,            // 11
    WEAPONSLOT_TYPE_DETONATOR,            // 12

    WEAPONSLOT_MAX
};

enum eWeaponState
{
    WEAPONSTATE_READY,
    WEAPONSTATE_FIRING,
    WEAPONSTATE_RELOADING,
    WEAPONSTATE_OUT_OF_AMMO,
    WEAPONSTATE_MELEE_MADECONTACT
};

/**
 * Contains the weapon types/models
 */
enum eWeaponType
{
    WEAPONTYPE_UNARMED = 0,
    WEAPONTYPE_BRASSKNUCKLE,
    WEAPONTYPE_GOLFCLUB,
    WEAPONTYPE_NIGHTSTICK,
    WEAPONTYPE_KNIFE,
    WEAPONTYPE_BASEBALLBAT,
    WEAPONTYPE_SHOVEL,
    WEAPONTYPE_POOL_CUE,
    WEAPONTYPE_KATANA,
    WEAPONTYPE_CHAINSAW,

    // gifts
    WEAPONTYPE_DILDO1,            // 10
    WEAPONTYPE_DILDO2,
    WEAPONTYPE_VIBE1,
    WEAPONTYPE_VIBE2,
    WEAPONTYPE_FLOWERS,
    WEAPONTYPE_CANE,

    WEAPONTYPE_GRENADE,
    WEAPONTYPE_TEARGAS,
    WEAPONTYPE_MOLOTOV,
    WEAPONTYPE_ROCKET,
    WEAPONTYPE_ROCKET_HS,            // 20
    WEAPONTYPE_FREEFALL_BOMB,

    // FIRST SKILL WEAPON
    WEAPONTYPE_PISTOL,            // handguns
    WEAPONTYPE_PISTOL_SILENCED,
    WEAPONTYPE_DESERT_EAGLE,
    WEAPONTYPE_SHOTGUN,                    // shotguns
    WEAPONTYPE_SAWNOFF_SHOTGUN,            // one handed
    WEAPONTYPE_SPAS12_SHOTGUN,
    WEAPONTYPE_MICRO_UZI,            // submachine guns
    WEAPONTYPE_MP5,
    WEAPONTYPE_AK47,            // 30      // machine guns
    WEAPONTYPE_M4,
    WEAPONTYPE_TEC9,            // this uses stat from the micro_uzi
    // END SKILL WEAPONS

    WEAPONTYPE_COUNTRYRIFLE,            // rifles
    WEAPONTYPE_SNIPERRIFLE,
    WEAPONTYPE_ROCKETLAUNCHER,            // specials
    WEAPONTYPE_ROCKETLAUNCHER_HS,
    WEAPONTYPE_FLAMETHROWER,
    WEAPONTYPE_MINIGUN,
    WEAPONTYPE_REMOTE_SATCHEL_CHARGE,
    WEAPONTYPE_DETONATOR,            // 40 // plastic explosive
    WEAPONTYPE_SPRAYCAN,
    WEAPONTYPE_EXTINGUISHER,
    WEAPONTYPE_CAMERA,
    WEAPONTYPE_NIGHTVISION,
    WEAPONTYPE_INFRARED,
    WEAPONTYPE_PARACHUTE,
    WEAPONTYPE_LAST_WEAPONTYPE,

    WEAPONTYPE_ARMOUR,
    // these are possible ways to die
    WEAPONTYPE_RAMMEDBYCAR,
    WEAPONTYPE_RUNOVERBYCAR,            // 50
    WEAPONTYPE_EXPLOSION,
    WEAPONTYPE_UZI_DRIVEBY,
    WEAPONTYPE_DROWNING,
    WEAPONTYPE_FALL,
    WEAPONTYPE_UNIDENTIFIED,            // Used for damage being done
    WEAPONTYPE_ANYMELEE,
    WEAPONTYPE_ANYWEAPON,
    WEAPONTYPE_FLARE,

    // Added by us
    WEAPONTYPE_TANK_GRENADE,
    WEAPONTYPE_INVALID = 255,
};
enum eWeaponStats
{
    WEAPONTYPE_MIN = WEAPONTYPE_GRENADE,
    WEAPONTYPE_MAX = WEAPONTYPE_EXTINGUISHER,
};

// VERY MUCH VC, NEEDS TO BE UPDATED
enum eWeaponModel
{
    WEAPONMODEL_NONE = -1,
    WEAPONMODEL_CELLPHONE = 258,
    WEAPONMODEL_BRASSKNUCKLE,
    WEAPONMODEL_SCREWDRIVER,
    WEAPONMODEL_GOLFCLUB,
    WEAPONMODEL_NITESTICK,
    WEAPONMODEL_KNIFECUR,
    WEAPONMODEL_BASEBALL_BAT,
    WEAPONMODEL_HAMMER,
    WEAPONMODEL_CLEAVER,
    WEAPONMODEL_MACHETE,
    WEAPONMODEL_KATANA,
    WEAPONMODEL_CHAINSAW,
    WEAPONMODEL_GRENADE,
    WEAPONMODEL_TEARGAS,
    WEAPONMODEL_MOLOTOV,
    WEAPONMODEL_MISSILE,
    WEAPONMODEL_COLT45,
    WEAPONMODEL_PYTHON,
    WEAPONMODEL_RUGER,
    WEAPONMODEL_CHROMEGUN,
    WEAPONMODEL_SHOTGSPA,
    WEAPONMODEL_BUDDYSHOT,
    WEAPONMODEL_M4,
    WEAPONMODEL_TEC9,
    WEAPONMODEL_UZI,
    WEAPONMODEL_INGRAMSL,
    WEAPONMODEL_MP5LNG,
    WEAPONMODEL_SNIPER,
    WEAPONMODEL_LASER,
    WEAPONMODEL_ROCKETLA,
    WEAPONMODEL_FLAME,
    WEAPONMODEL_M60,
    WEAPONMODEL_MINIGUN,
    WEAPONMODEL_BOMB,
    WEAPONMODEL_CAMERA,
    WEAPONMODEL_FINGERS,
    WEAPONMODEL_MINIGUN2
};

// Caz - Things such as damage will only work with certain skill levels ( I found 500 to be the magic number )
class CWeaponInfo
{
public:
    virtual eWeaponModel GetModel() = 0;
    virtual eWeaponModel GetModel2() = 0;

    virtual float GetWeaponRange() = 0;
    virtual void  SetWeaponRange(float fRange) = 0;

    virtual float GetTargetRange() = 0;
    virtual void  SetTargetRange(float fRange) = 0;

    virtual CVector* GetFireOffset() = 0;
    virtual void     SetFireOffset(CVector* vecFireOffset) = 0;

    virtual short GetDamagePerHit() = 0;
    virtual void  SetDamagePerHit(short sDamagePerHit) = 0;

    virtual float GetAccuracy() = 0;
    virtual void  SetAccuracy(float fAccuracy) = 0;

    virtual short GetMaximumClipAmmo() = 0;
    virtual void  SetMaximumClipAmmo(short sAccuracy) = 0;

    virtual float GetMoveSpeed() = 0;
    virtual void  SetMoveSpeed(float fMoveSpeed) = 0;

    // projectile/areaeffect only
    virtual float GetFiringSpeed() = 0;
    virtual void  SetFiringSpeed(float fFiringSpeed) = 0;

    // area effect only
    virtual float GetRadius() = 0;
    virtual void  SetRadius(float fRadius) = 0;

    virtual float GetLifeSpan() = 0;
    virtual void  SetLifeSpan(float fLifeSpan) = 0;

    virtual float GetSpread() = 0;
    virtual void  SetSpread(float fSpread) = 0;

    virtual float GetAnimBreakoutTime() = 0;
    virtual void  SetAnimBreakoutTime(float fBreakoutTime) = 0;

    virtual eWeaponSlot GetSlot() = 0;
    virtual void        SetSlot(eWeaponSlot dwSlot) = 0;

    virtual eWeaponSkill GetSkill() = 0;
    virtual void         SetSkill(eWeaponSkill weaponSkill) = 0;

    virtual float GetRequiredStatLevel() = 0;
    virtual void  SetRequiredStatLevel(float fStatLevel) = 0;

    virtual void  SetFlags(int flags) = 0;
    virtual void  SetFlag(DWORD flag) = 0;
    virtual void  ClearFlag(DWORD flag) = 0;
    virtual bool  IsFlagSet(DWORD flag) = 0;
    virtual short GetFlags() = 0;

    virtual DWORD GetAnimGroup() = 0;
    virtual void  SetAnimGroup(DWORD dwAnimGroup) = 0;

    virtual eFireType GetFireType() = 0;

    virtual float GetWeaponAnimLoopStart() = 0;

    virtual float GetWeaponAnimLoopStop() = 0;

    virtual float GetWeaponAnimLoopFireTime() = 0;
    virtual void  SetWeaponAnimLoopFireTime(float animFireTime) = 0;

    virtual float GetWeaponAnim2LoopStart() = 0;

    virtual float GetWeaponAnim2LoopStop() = 0;

    virtual float GetWeaponAnim2LoopFireTime() = 0;

    virtual float GetWeaponAnimBreakoutTime() = 0;

    virtual float GetWeaponRadius() = 0;

    virtual short GetAimOffsetIndex() = 0;
    virtual BYTE  GetDefaultCombo() = 0;
    virtual BYTE  GetCombosAvailable() = 0;
};
