/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CWeaponStatManagerSA.h
 *  PURPOSE:     header file for the custom weapon stats manager.
 *
 *****************************************************************************/

#pragma once

#include <list>
#include <game/CWeaponStatManager.h>
#include "CWeaponStatSA.h"

// Straight out of CGameSA.h
#define CLASSSIZE_WeaponInfo            112         // ##SA##
#define NUM_WeaponInfosStdSkill         WEAPONTYPE_LAST_WEAPONTYPE
#define NUM_WeaponInfosOtherSkill       11
#define NUM_WeaponInfosTotal            (NUM_WeaponInfosStdSkill + (3*NUM_WeaponInfosOtherSkill)) // std, (poor, pro, special)

#define WEAPON_STAT_MAX 112

struct sWeaponInfo
{
    eFireType fire_type;            // type - instant hit (e.g. pistol), projectile (e.g. rocket launcher), area effect (e.g. flame thrower)

    float target_range;            // max targeting range
    float weapon_range;            // absolute gun range / default melee attack range
    int   model;                   // modelinfo id
    int   model2;                  // second modelinfo id

    eWeaponSlot weapon_slot;
    int         flags;            // flags defining characteristics

    // instead of storing pointers directly to anims, use anim association groups
    // NOTE: this is used for stealth kill anims for melee weapons
    DWORD anim_group;

    //////////////////////////////////
    // Gun Data
    /////////////////////////////////
    short   maximum_clip_ammo;            // ammo in one clip
    short   damage;                       // damage inflicted per hit
    CVector fire_offset;                  // offset from weapon origin to projectile starting point

    // skill settings
    eWeaponSkill skill_level;                     // what's the skill level of this weapontype
    int          required_skill_level;            // what stat level is required for this skill level
    float        accuracy;                        // modify accuracy of weapon
    float        move_speed;                      // how fast can move with weapon

    // anim timings
    float anim_loop_start;                  // start of animation loop
    float anim_loop_stop;                   // end of animation loop
    float anim_loop_bullet_fire;            // time in animation when weapon should be fired

    float anim2_loop_start;                  // start of animation2 loop
    float anim2_loop_stop;                   // end of animation2 loop
    float anim2_loop_bullet_fire;            // time in animation2 when weapon should be fired

    float anim_breakout_time;            // time after which player can break out of attack and run off

    // projectile/area effect specific info
    float firing_speed;            // speed of projectile
    float radius;                  // radius affected
    float life_span;               // time taken for shot to dissipate
    float spread;                  // angle inside which shots are created

    short aim_offset;            // index into array of aiming offsets
    //////////////////////////////////
    // Melee Data
    /////////////////////////////////
    BYTE default_combo;               // base combo for this melee weapon
    BYTE combos_available;            // how many further combos are available
};

class CWeaponStatManagerSA : public CWeaponStatManager
{
public:
    CWeaponStatManagerSA();
    ~CWeaponStatManagerSA();
    CWeaponStat* GetWeaponStats(eWeaponType type, eWeaponSkill skill = WEAPONSKILL_STD);
    CWeaponStat* GetWeaponStatsFromSkillLevel(eWeaponType type, float fSkillLevel);
    CWeaponStat* GetOriginalWeaponStats(eWeaponType type, eWeaponSkill skill = WEAPONSKILL_STD);
    void         Init();
    void         InitLists();
    void         ResetLists();
    bool         LoadDefault(CWeaponStat* pDest, eWeaponType weaponType, eWeaponSkill skill = WEAPONSKILL_STD);
    void         CreateWeaponStat(CWeaponInfo* pInterface, eWeaponType weaponType, eWeaponSkill weaponSkill);
    CWeaponStat* CreateWeaponStatUnlisted(eWeaponType weaponType, eWeaponSkill weaponSkill);
    eWeaponSkill GetWeaponSkillFromSkillLevel(eWeaponType type, float fSkillLevel);

private:
    bool LoadDefaultInternal(CWeaponStatSA* pDest, eWeaponType weaponType, eWeaponSkill skill = WEAPONSKILL_STD);

    std::list<CWeaponStat*> m_OriginalWeaponData;
    std::list<CWeaponStat*> m_WeaponData;
    static sWeaponInfo      OriginalPoorWeaponData[WEAPONTYPE_MAX + 1];
    static sWeaponInfo      OriginalNormalWeaponData[WEAPONTYPE_MAX + 1];
    static sWeaponInfo      OriginalHitmanWeaponData[WEAPONTYPE_MAX + 1];
};
