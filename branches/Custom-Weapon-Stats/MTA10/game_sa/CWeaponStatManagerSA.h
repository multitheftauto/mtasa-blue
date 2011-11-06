/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CWeaponStatManagerSA.h
*  PURPOSE:     header file for the custom weapon stats manager.
*  DEVELOPERS:  Cazomino05 < Cazomino05@gmail.com >
*
*****************************************************************************/

#ifndef __CWEAPONSTATMANAGERSA_H
#define __CWEAPONSTATMANAGERSA_H

#include <list>

// Straight out of CGameSA.h
#define     CLASSSIZE_WeaponInfo            112         // ##SA##
#define     NUM_WeaponInfosStdSkill         WEAPONTYPE_LAST_WEAPONTYPE
#define     NUM_WeaponInfosOtherSkill       11
#define     NUM_WeaponInfosTotal            (NUM_WeaponInfosStdSkill + (3*NUM_WeaponInfosOtherSkill)) // std, (poor, pro, special)
struct sWeaponInfo
{
    eFireType   fire_type;        // type - instant hit (e.g. pistol), projectile (e.g. rocket launcher), area effect (e.g. flame thrower)

    FLOAT       target_range;     // max targeting range
    FLOAT       weapon_range;     // absolute gun range / default melee attack range
    int         model;          // modelinfo id
    int         model2;         // second modelinfo id
    
    eWeaponSlot weapon_slot;
    int         flags;           // flags defining characteristics

    // instead of storing pointers directly to anims, use anim association groups
    // NOTE: this is used for stealth kill anims for melee weapons
    DWORD       anim_group;

    //////////////////////////////////
    // Gun Data
    /////////////////////////////////
    short       maximum_clip_ammo;                // ammo in one clip
    short       damage;              // damage inflicted per hit
    CVector     fire_offset;        // offset from weapon origin to projectile starting point
    
    // skill settings
    eWeaponSkill skill_level;          // what's the skill level of this weapontype
    int         required_skill_level;        // what stat level is required for this skill level
    FLOAT       accuracy;            // modify accuracy of weapon
    FLOAT       move_speed;           // how fast can move with weapon

    // anim timings
    FLOAT       anim_loop_start;        // start of animation loop
    FLOAT       anim_loop_stop;          // end of animation loop
    FLOAT       anim_loop_bullet_fire;         // time in animation when weapon should be fired

    FLOAT       anim2_loop_start;       // start of animation2 loop
    FLOAT       anim2_loop_stop;         // end of animation2 loop
    FLOAT       anim2_loop_bullet_fire;        // time in animation2 when weapon should be fired

    FLOAT       anim_breakout_time;     // time after which player can break out of attack and run off
    
    // projectile/area effect specific info
    FLOAT       firing_speed;               // speed of projectile
    FLOAT       radius;              // radius affected
    FLOAT       life_span;            // time taken for shot to dissipate
    FLOAT       spread;              // angle inside which shots are created
    
    short       aim_offset;      // index into array of aiming offsets
    //////////////////////////////////
    // Melee Data
    /////////////////////////////////
    BYTE        default_combo;         // base combo for this melee weapon
    BYTE        combos_available;     // how many further combos are available
};

#define WEAPON_STAT_MAX 112
class CWeaponStatManagerSA : public CWeaponStatManager
{
public:
                                CWeaponStatManagerSA            ( void );
                                ~CWeaponStatManagerSA           ( void );
    CWeaponStat*                GetWeaponStats                  ( eWeaponType type, eWeaponSkill skill = WEAPONSKILL_STD );
    CWeaponStat*                GetOriginalWeaponStats          ( eWeaponType type, eWeaponSkill skill = WEAPONSKILL_STD );
    void                        Init                            ( void );
    void                        InitLists                       ( void );
    void                        ResetLists                      ( void );
    bool                        LoadDefault                     ( CWeaponStat* pDest, eWeaponType weaponType, eWeaponSkill skill = WEAPONSKILL_STD );
    void                        CreateWeaponStat                ( CWeaponInfo* pInterface, eWeaponType weaponType, eWeaponSkill weaponSkill );


private:
    bool                        LoadDefaultInternal             ( CWeaponStatSA* pDest, eWeaponType weaponType, eWeaponSkill skill = WEAPONSKILL_STD );

    std::list<CWeaponStat*>                                     m_OriginalWeaponData;
    std::list<CWeaponStat*>                                     m_WeaponData;
    static sWeaponInfo                                          OriginalPoorWeaponData [ WEAPONTYPE_MAX+1 ];
    static sWeaponInfo                                          OriginalNormalWeaponData [ WEAPONTYPE_MAX+1 ];
    static sWeaponInfo                                          OriginalHitmanWeaponData [ WEAPONTYPE_MAX+1 ];

};

#endif