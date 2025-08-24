/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/WeaponProperty.h
 *  PURPOSE:     Header for common definitions
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

enum class WeaponProperty
{
    WEAPON_INVALID_PROPERTY = 0,
    WEAPON_WEAPON_RANGE,
    WEAPON_TARGET_RANGE,
    WEAPON_ACCURACY,
    WEAPON_DAMAGE,
    WEAPON_LIFE_SPAN,
    WEAPON_FIRING_SPEED,
    WEAPON_SPREAD,
    WEAPON_MAX_CLIP_AMMO,
    WEAPON_MOVE_SPEED,
    WEAPON_FLAGS,
    WEAPON_ANIM_GROUP,
    WEAPON_TYPE_SET_DISABLED,
    WEAPON_FIRETYPE,

    WEAPON_MODEL,
    WEAPON_MODEL2,

    WEAPON_SLOT,

    WEAPON_FIRE_OFFSET,

    WEAPON_SKILL_LEVEL,
    WEAPON_REQ_SKILL_LEVEL,

    WEAPON_ANIM_LOOP_START,
    WEAPON_ANIM_LOOP_STOP,
    WEAPON_ANIM_LOOP_RELEASE_BULLET_TIME,

    WEAPON_ANIM2_LOOP_START,
    WEAPON_ANIM2_LOOP_STOP,
    WEAPON_ANIM2_LOOP_RELEASE_BULLET_TIME,

    WEAPON_ANIM_BREAKOUT_TIME,

    WEAPON_SPEED,
    WEAPON_RADIUS,

    WEAPON_AIM_OFFSET,

    WEAPON_DEFAULT_COMBO,
    WEAPON_COMBOS_AVAILABLE,

    WEAPON_FLAG_AIM_NO_AUTO,            // 0x000001 - cant auto target to aim       (disable automatic up/down adjustment when firing without aiming)
    WEAPON_FLAG_FIRST = WEAPON_FLAG_AIM_NO_AUTO,
    WEAPON_FLAG_AIM_ARM,                   // 0x000002 - only needs arm to aim         (ie pistol/shotgun/tec9/uzi)
    WEAPON_FLAG_AIM_1ST_PERSON,            // 0x000004 - uses 1st person aim           (ie sniper/rpg-hs
    WEAPON_FLAG_AIM_FREE,                  // 0x000008 - can only use free aiming      (ie country sniper/flame thrower/minigun/fire extinguisher)
    WEAPON_FLAG_MOVE_AND_AIM,              // 0x000010 - can move and aim at same time
    WEAPON_FLAG_MOVE_AND_SHOOT,            // 0x000020 - can move and fire at same time
    WEAPON_FLAG_UNKNOWN_0040,
    WEAPON_FLAG_UNKNOWN_0080,
    WEAPON_FLAG_TYPE_THROW,               // 0x000100 - is a throwing weapon          (ie satchel)
    WEAPON_FLAG_TYPE_HEAVY,               // 0x000200 - heavy weapon - can't jump     (ie flame thrower/rpgs/minigun)
    WEAPON_FLAG_TYPE_CONSTANT,            // 0x000400 - fires every frame within loop (ie paint spray)
    WEAPON_FLAG_TYPE_DUAL,                // 0x000800 - can use 2x guns at same time  (ie pistol/shotgun/tec9/uzi)
    WEAPON_FLAG_ANIM_RELOAD,              // 0x001000 - weapon has reload anims       (ie everything except shotgun/snipers/flame thrower/rpgs/minigun/satchel)
    WEAPON_FLAG_ANIM_CROUCH,              // 0x002000 - weapon has crouching anims    (ie everything except flame thrower/rpgs/minigun/satchel)
    WEAPON_FLAG_ANIM_RELOAD_LOOP,            // 0x004000 - loop from end of reload to fire loop start
    WEAPON_FLAG_ANIM_RELOAD_LONG,            // 0x008000 - force a longer reload time!   (ie rpgs/snipers)
    WEAPON_FLAG_SHOT_SLOWS,                  // 0x010000 - slows down                    (ie flame thrower)
    WEAPON_FLAG_SHOT_RAND_SPEED,             // 0x020000 - random speed                  (ie flame thrower)
    WEAPON_FLAG_SHOT_ANIM_ABRUPT,            // 0x040000 - force the anim to finish player after aim/fire rather than blending out   (ie rpgs)
    WEAPON_FLAG_SHOT_EXPANDS,                // 0x080000 - expands
    WEAPON_FLAG_LAST = WEAPON_FLAG_SHOT_EXPANDS,

    WEAPON_FIRE_ROTATION,
};
