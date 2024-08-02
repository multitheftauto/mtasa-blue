namespace EmbeddedLuaCode
{
    const char* const constantsWeapons = R"~LUA~(
--[[
    SERVER AND CLIENT.
    Defines a constant variables available for server and client.
--]]

constants.WeaponSlot = setmetatable({
    Unarmed = 0,
    Melee = 1,
    Handgun = 2,
    Shotgun = 3,
    SMG = 4,
    Rifle = 5,
    Sniper = 6,
    Heavy = 7,
    Thrown = 8,
    Special = 9,
    Gift = 10,
    Parachute = 11,
    Detonator = 12,
}, __readonly_table_meta__)

constants.WeaponSkillLevel = setmetatable({
    Poor = 0,
    Gangster = 1,
    Hitman = 2
}, __readonly_table_meta__)

constants.WeaponSkillName = setmetatable({
    Pro = 'pro',
    Gangster = 'std',
    Poor = 'poor'
}, __readonly_table_meta__)

constants.WeaponProperties = setmetatable({
    WeaponRange = 'weapon_range',
    TargetRange = 'target_range',
    Accuracy = 'accuracy',
    Damage = 'damage',
    MaximumClipAmmo = 'maximum_clip_ammo',
    MoveSpeed = 'move_speed',
    Flags = 'flags',
    FlagAimNoAuto = 'flag_aim_no_auto',
    FlagAimArm = 'flag_aim_arm',
    FlagAim1stPerson = 'flag_aim_1st_person',
    FlagAimFree = 'flag_aim_free',
    FlagMoveAndAim = 'flag_move_and_aim',
    FlagMoveAndShoot = 'flag_move_and_shoot',
    FlagTypeThrow = 'flag_type_throw',
    FlagTypeHeavy = 'flag_type_heavy',
    FlagTypeConstant = 'flag_type_constant',
    FlagTypeDual = 'flag_type_dual',
    FlagAnimReload = 'flag_anim_reload',
    FlagAnimCrouch = 'flag_anim_crouch',
    FlagAnimReloadLoop = 'flag_anim_reload_loop',
    FlagAnimReloadLong = 'flag_anim_reload_long',
    FlagShotSlows = 'flag_shot_slows',
    FlagShotRandSpeed = 'flag_shot_rand_speed',
    FlagShotAnimAbrupt = 'flag_shot_anim_abrupt',
    FlagShotExpands = 'flag_shot_expands',
    AnimLoopStart = 'anim_loop_start',
    AnimLoopStop = 'anim_loop_stop',
    AnimLoopBulletFire = 'anim_loop_bullet_fire',
    Anim2LoopStart = 'anim2_loop_start',
    Anim2LoopStop = 'anim2_loop_stop',
    Anim2LoopBulletFire = 'anim2_loop_bullet_fire',
    AnimBreakoutTime = 'anim_breakout_time',
    FireType = 'fire_type',
    Model = 'model',
    Model2 = 'model2',
    WeaponSlot = 'weapon_slot',
    AnimGroup = 'anim_group',
    SkillLevel = 'skill_level',
    RequiredSkillLevel = 'required_skill_level',
    FiringSpeed = 'firing_speed',
    Radius = 'radius',
    LifeSpan = 'life_span',
    Spread = 'spread',
    FireOffset = 'fire_offset',
    AimOffset = 'aim_offset',
    DefaultCombo = 'default_combo',
    CombosAvailable = 'combos_available',
}, __readonly_table_meta__)

    )~LUA~";
}