/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CWeaponStatManager.h
*  PURPOSE:     source file for custom weapon stats.
*  DEVELOPERS:  Cazomino05 < Cazomino05@gmail.com >
*
*****************************************************************************/
#include "StdInc.h"

sWeaponInfo CWeaponStatManager::OriginalWeaponData [ WEAPONTYPE_MAX+1 ];
CWeaponStatManager::CWeaponStatManager ( )
{
    Init();
    CWeaponStat* pWeaponStat = NULL;
    // Make our list nicer by starting at the non-skill weapons so we don't have duplicates of STD weapon skill
    for ( int i = NUM_WeaponInfosOtherSkill + 1; i < NUM_WeaponInfosStdSkill; i++)
    {
        eWeaponType weaponType = (eWeaponType) (WEAPONTYPE_PISTOL + i);
        // Storage for Original weapon data ( for resetting )
        pWeaponStat = new CWeaponStat ( weaponType, WEAPONSKILL_STD );
        m_OriginalWeaponData.push_back ( pWeaponStat );
        // Store our defaults for this weapon.
        LoadDefault ( pWeaponStat, weaponType );

        // Storage for new weapon data ( for script use )
        pWeaponStat = new CWeaponStat ( weaponType, WEAPONSKILL_STD );
        m_WeaponData.push_back ( pWeaponStat );
        // Store our defaults for this weapon.
        LoadDefault ( pWeaponStat, weaponType );
    }

    for ( int skill = 0; skill < 3 ; skill++ )
    {
        for ( int i = 0; i < NUM_WeaponInfosOtherSkill; i++ )
        {
            eWeaponType weaponType = (eWeaponType) (WEAPONTYPE_PISTOL + i);
            // Storage for Original weapon data ( for resetting )
            pWeaponStat = new CWeaponStat ( weaponType, (eWeaponSkill)skill );
            m_OriginalWeaponData.push_back ( pWeaponStat );
            // Store our defaults for this weapon.
            LoadDefault ( pWeaponStat, weaponType );

            // Storage for new weapon data ( for script use )
            pWeaponStat = new CWeaponStat ( weaponType, (eWeaponSkill)skill );
            m_WeaponData.push_back ( pWeaponStat );
            // Store our defaults for this weapon.
            LoadDefault ( pWeaponStat, weaponType );
        }
    }
}

CWeaponStatManager::~CWeaponStatManager ( )
{
    for ( std::list < CWeaponStat* >::iterator iter = m_OriginalWeaponData.begin (); iter != m_OriginalWeaponData.end ();iter++ )
    {
        if ( (*iter ) )
            delete (*iter);
    }

    for ( std::list < CWeaponStat* >::iterator iter = m_WeaponData.begin (); iter != m_WeaponData.end ();iter++ )
    {
        if ( (*iter ) )
            delete (*iter);
    }
}


CWeaponStat* CWeaponStatManager::GetWeaponStats ( eWeaponType type, eWeaponSkill skill )
{
    for ( std::list < CWeaponStat* >::iterator iter = m_OriginalWeaponData.begin (); iter != m_OriginalWeaponData.end ();iter++ )
    {
        if ( (*iter)->GetWeaponSkillLevel() == skill && (*iter)->GetWeaponType () == type )
        {
            return (*iter);
        }
    }
    return NULL;
}
CWeaponStat* CWeaponStatManager::GetOriginalWeaponStats ( eWeaponType type, eWeaponSkill skill )
{
    for ( std::list < CWeaponStat* >::iterator iter = m_OriginalWeaponData.begin (); iter != m_OriginalWeaponData.end ();iter++ )
    {
        if ( (*iter)->GetWeaponSkillLevel() == skill && (*iter)->GetWeaponType () == type )
        {
            return (*iter);
        }
    }
    return NULL;
}
void CWeaponStatManager::Init ( void )
{
    // Load SA Defaults
    // Script to Dump:
    // Script bugs:
    // - floats mishandling i.e. 1f which will cause a compile failure
    // - 
    /*
    props = {
        "weapon_range","target_range","accuracy",
        "damage","life_span","firing_speed",
        "spread","maximum_clip_ammo",
        "move_speed","flags","anim_group",
        "fire_type","model","model2",
        "weapon_slot","fire_offset","skill_level",
        "required_skill_level","anim_loop_start","anim_loop_stop",
        "anim_loop_bullet_fire","anim2_loop_start",
        "anim2_loop_stop","anim2_loop_bullet_fire",
        "anim_breakout_time","radius","aim_offset",
        "default_combo","combos_available"
    }
    count = 22
    addCommandHandler("dump", function ()
	    hFile = fileOpen ( "dump.txt" )
	    fileSetPos(hFile, fileGetSize ( hFile ))
	    i = count
	    fileWrite(hFile, "// " .. i .. " - " .. getWeaponNameFromID( i ) )
	    fileWrite(hFile, "\r\n")
	    for k,v in ipairs(props) do
		    outputChatBox(v)
		    fileWrite(hFile, Dump(v))
		    fileWrite(hFile, "\r\n")
	    end
	    fileClose(hFile)
	    count = count + 1
    end)

    function Dump(strInfo)
	    i = count
	    -- Special case for Vectors
	    if ( strInfo == "fire_offset") then
		    fx, fy, fz = getPedWeaponInfo(getLocalPlayer(), i, strInfo)
		    return "OriginalWeaponData[" .. i .. "]." .. strInfo .. " = CVector ( " .. floatify ( fx ) .. ", " .. floatify ( fy ) .. ", " .. floatify ( fz ) .. " );"
	    end
	    -- Avoids compile errors ( Cannot convert from x to y )
	    if ( strInfo == "skill_level" ) then
		    return "OriginalWeaponData[" .. i .. "]." .. strInfo .. " = (eWeaponSkill) " .. getPedWeaponInfo(getLocalPlayer(), i, strInfo) .. ";"
	    end

	    if ( strInfo == "weapon_slot" ) then
		    return "OriginalWeaponData[" .. i .. "]." .. strInfo .. " = (eWeaponSlot) " .. getPedWeaponInfo(getLocalPlayer(), i, strInfo) .. ";"
	    end

	    if ( strInfo == "fire_type" ) then
		    return "OriginalWeaponData[" .. i .. "]." .. strInfo .. " = (eFireType) " .. getPedWeaponInfo(getLocalPlayer(), i, strInfo) .. ";"
	    end
	    -- end
	    -- Avoids Conversion from Double to float warnings.
	    if ( strInfo == "weapon_range" or strInfo == "target_range" or strInfo == "accuracy" or strInfo == "move_speed" or strInfo == "anim_loop_start" or strInfo == "anim_loop_stop" or strInfo == "anim_loop_bullet_fire" or strInfo == "anim2_loop_start" or strInfo == "anim2_loop_stop" or strInfo == "anim2_loop_bullet_fire" or strInfo == "anim_breakout_time" or strInfo == "firing_speed" or strInfo == "radius" or strInfo == "life_span" or strInfo == "spread") then
		    return "OriginalWeaponData[" .. i .. "]." .. strInfo .. " = " .. floatify ( getPedWeaponInfo(getLocalPlayer(), i, strInfo) ) .. ";"
	    end
	    -- Default
	    return "OriginalWeaponData[" .. i .. "]." .. strInfo .. " = " .. getPedWeaponInfo(getLocalPlayer(), i, strInfo) .. ";"
    end
    -- Avoids conversion from Double to float warnings.
    function floatify ( fVal )
	    if ( fVal == math.floor( fVal ) or fVal == math.ceil( fVal ) ) then
		    return fVal .. ".0f"
	    end
	    return fVal .. "f"
    end
    */
    // 22 - Colt 45
    OriginalWeaponData[22].weapon_range = 35.0f;
    OriginalWeaponData[22].target_range = 30.0f;
    OriginalWeaponData[22].accuracy = 1.0f;
    OriginalWeaponData[22].damage = 25;
    OriginalWeaponData[22].life_span = 0.0f;
    OriginalWeaponData[22].firing_speed = 0.0f;
    OriginalWeaponData[22].spread = 0.0f;
    OriginalWeaponData[22].maximum_clip_ammo = 17;
    OriginalWeaponData[22].move_speed = 0.0f;
    OriginalWeaponData[22].flags = 12339;
    OriginalWeaponData[22].anim_group = 13;
    OriginalWeaponData[22].fire_type = (eFireType) 1;
    OriginalWeaponData[22].model = 346;
    OriginalWeaponData[22].model2 = -1;
    OriginalWeaponData[22].weapon_slot = (eWeaponSlot) 2;
    OriginalWeaponData[22].fire_offset = CVector ( 0.25f, 0.050000000745058f, 0.090000003576279f );
    OriginalWeaponData[22].skill_level = (eWeaponSkill) 1;
    OriginalWeaponData[22].required_skill_level = 40;
    OriginalWeaponData[22].anim_loop_start = 0.20000001788139f;
    OriginalWeaponData[22].anim_loop_stop = 0.49399998784065f;
    OriginalWeaponData[22].anim_loop_bullet_fire = 0.20000001788139f;
    OriginalWeaponData[22].anim2_loop_start = 0.20000001788139f;
    OriginalWeaponData[22].anim2_loop_stop = 0.49399998784065f;
    OriginalWeaponData[22].anim2_loop_bullet_fire = 0.20000001788139f;
    OriginalWeaponData[22].anim_breakout_time = 3.3000001907349f;
    OriginalWeaponData[22].radius = 0.0f;
    OriginalWeaponData[22].aim_offset = 2;
    OriginalWeaponData[22].default_combo = 4;
    OriginalWeaponData[22].combos_available = 1;
    // 23 - Silenced
    OriginalWeaponData[23].weapon_range = 35.0f;
    OriginalWeaponData[23].target_range = 30.0f;
    OriginalWeaponData[23].accuracy = 1.25f;
    OriginalWeaponData[23].damage = 40;
    OriginalWeaponData[23].life_span = 0.0f;
    OriginalWeaponData[23].firing_speed = 0.0f;
    OriginalWeaponData[23].spread = 0.0f;
    OriginalWeaponData[23].maximum_clip_ammo = 17;
    OriginalWeaponData[23].move_speed = 1.2000000476837f;
    OriginalWeaponData[23].flags = 28689;
    OriginalWeaponData[23].anim_group = 18;
    OriginalWeaponData[23].fire_type = (eFireType) 1;
    OriginalWeaponData[23].model = 347;
    OriginalWeaponData[23].model2 = -1;
    OriginalWeaponData[23].weapon_slot = (eWeaponSlot) 2;
    OriginalWeaponData[23].fire_offset = CVector ( 0.40000000596046f, 0.050000000745058f, 0.15000000596046f );
    OriginalWeaponData[23].skill_level = (eWeaponSkill) 1;
    OriginalWeaponData[23].required_skill_level = 500;
    OriginalWeaponData[23].anim_loop_start = 0.20000001788139f;
    OriginalWeaponData[23].anim_loop_stop = 0.59399998188019f;
    OriginalWeaponData[23].anim_loop_bullet_fire = 0.20000001788139f;
    OriginalWeaponData[23].anim2_loop_start = 0.20000001788139f;
    OriginalWeaponData[23].anim2_loop_stop = 0.59399998188019f;
    OriginalWeaponData[23].anim2_loop_bullet_fire = 0.20000001788139f;
    OriginalWeaponData[23].anim_breakout_time = 1.3333333730698f;
    OriginalWeaponData[23].radius = 0.0f;
    OriginalWeaponData[23].aim_offset = 7;
    OriginalWeaponData[23].default_combo = 4;
    OriginalWeaponData[23].combos_available = 1;
    // 24 - Deagle
    OriginalWeaponData[24].weapon_range = 35.0f;
    OriginalWeaponData[24].target_range = 30.0f;
    OriginalWeaponData[24].accuracy = 0.0f;
    OriginalWeaponData[24].damage = 140;
    OriginalWeaponData[24].life_span = 0.0f;
    OriginalWeaponData[24].firing_speed = 0.0f;
    OriginalWeaponData[24].spread = 0.0f;
    OriginalWeaponData[24].maximum_clip_ammo = 7;
    OriginalWeaponData[24].move_speed = 1.2000000476837f;
    OriginalWeaponData[24].flags = 28689;
    OriginalWeaponData[24].anim_group = 11;
    OriginalWeaponData[24].fire_type = (eFireType) 1;
    OriginalWeaponData[24].model = 348;
    OriginalWeaponData[24].model2 = -1;
    OriginalWeaponData[24].weapon_slot = (eWeaponSlot) 2;
    OriginalWeaponData[24].fire_offset = CVector ( 0.40999999642372f, 0.029999999329448f, 0.11999999731779f );
    OriginalWeaponData[24].skill_level = (eWeaponSkill) 1;
    OriginalWeaponData[24].required_skill_level = 200;
    OriginalWeaponData[24].anim_loop_start = 0.20000001788139f;
    OriginalWeaponData[24].anim_loop_stop = 0.99399995803833f;
    OriginalWeaponData[24].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalWeaponData[24].anim2_loop_start = 0.20000001788139f;
    OriginalWeaponData[24].anim2_loop_stop = 0.91400003433228f;
    OriginalWeaponData[24].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalWeaponData[24].anim_breakout_time = 1.3333333730698f;
    OriginalWeaponData[24].radius = 0.0f;
    OriginalWeaponData[24].aim_offset = 0;
    OriginalWeaponData[24].default_combo = 4;
    OriginalWeaponData[24].combos_available = 1;
    // 25 - Shotgun
    OriginalWeaponData[25].weapon_range = 40.0f;
    OriginalWeaponData[25].target_range = 35.0f;
    OriginalWeaponData[25].accuracy = 1.2000000476837f;
    OriginalWeaponData[25].damage = 10;
    OriginalWeaponData[25].life_span = 0.0f;
    OriginalWeaponData[25].firing_speed = 0.0f;
    OriginalWeaponData[25].spread = 0.0f;
    OriginalWeaponData[25].maximum_clip_ammo = 1;
    OriginalWeaponData[25].move_speed = 1.1000000238419f;
    OriginalWeaponData[25].flags = 8209;
    OriginalWeaponData[25].anim_group = 19;
    OriginalWeaponData[25].fire_type = (eFireType) 1;
    OriginalWeaponData[25].model = 349;
    OriginalWeaponData[25].model2 = -1;
    OriginalWeaponData[25].weapon_slot = (eWeaponSlot) 3;
    OriginalWeaponData[25].fire_offset = CVector ( 0.81999999284744f, -0.019999999552965f, 0.25f );
    OriginalWeaponData[25].skill_level = (eWeaponSkill) 1;
    OriginalWeaponData[25].required_skill_level = 200;
    OriginalWeaponData[25].anim_loop_start = 0.20000001788139f;
    OriginalWeaponData[25].anim_loop_stop = 1.2539999485016f;
    OriginalWeaponData[25].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalWeaponData[25].anim2_loop_start = 0.20000001788139f;
    OriginalWeaponData[25].anim2_loop_stop = 1.3140000104904f;
    OriginalWeaponData[25].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalWeaponData[25].anim_breakout_time = 0.66666668653488f;
    OriginalWeaponData[25].radius = 0.0f;
    OriginalWeaponData[25].aim_offset = 8;
    OriginalWeaponData[25].default_combo = 4;
    OriginalWeaponData[25].combos_available = 1;
    // 26 - Sawed-off
    OriginalWeaponData[26].weapon_range = 35.0f;
    OriginalWeaponData[26].target_range = 30.0f;
    OriginalWeaponData[26].accuracy = 0.80000001192093f;
    OriginalWeaponData[26].damage = 10;
    OriginalWeaponData[26].life_span = 0.0f;
    OriginalWeaponData[26].firing_speed = 0.0f;
    OriginalWeaponData[26].spread = 0.0f;
    OriginalWeaponData[26].maximum_clip_ammo = 2;
    OriginalWeaponData[26].move_speed = 1.1000000238419f;
    OriginalWeaponData[26].flags = 12339;
    OriginalWeaponData[26].anim_group = 16;
    OriginalWeaponData[26].fire_type = (eFireType) 1;
    OriginalWeaponData[26].model = 350;
    OriginalWeaponData[26].model2 = -1;
    OriginalWeaponData[26].weapon_slot = (eWeaponSlot) 3;
    OriginalWeaponData[26].fire_offset = CVector ( 0.56000000238419f, 0.0f, 0.15000000596046f );
    OriginalWeaponData[26].skill_level = (eWeaponSkill) 1;
    OriginalWeaponData[26].required_skill_level = 200;
    OriginalWeaponData[26].anim_loop_start = 0.20000001788139f;
    OriginalWeaponData[26].anim_loop_stop = 0.49399998784065f;
    OriginalWeaponData[26].anim_loop_bullet_fire = 0.20000001788139f;
    OriginalWeaponData[26].anim2_loop_start = 0.20000001788139f;
    OriginalWeaponData[26].anim2_loop_stop = 0.49399998784065f;
    OriginalWeaponData[26].anim2_loop_bullet_fire = 0.20000001788139f;
    OriginalWeaponData[26].anim_breakout_time = 0.0f;
    OriginalWeaponData[26].radius = 0.0f;
    OriginalWeaponData[26].aim_offset = 5;
    OriginalWeaponData[26].default_combo = 4;
    OriginalWeaponData[26].combos_available = 1;
    // 27 - Combat Shotgun
    OriginalWeaponData[27].weapon_range = 40.0f;
    OriginalWeaponData[27].target_range = 40.0f;
    OriginalWeaponData[27].accuracy = 1.7999999523163f;
    OriginalWeaponData[27].damage = 15;
    OriginalWeaponData[27].life_span = 0.0f;
    OriginalWeaponData[27].firing_speed = 0.0f;
    OriginalWeaponData[27].spread = 0.0f;
    OriginalWeaponData[27].maximum_clip_ammo = 7;
    OriginalWeaponData[27].move_speed = 0.0f;
    OriginalWeaponData[27].flags = 28689;
    OriginalWeaponData[27].anim_group = 21;
    OriginalWeaponData[27].fire_type = (eFireType) 1;
    OriginalWeaponData[27].model = 351;
    OriginalWeaponData[27].model2 = -1;
    OriginalWeaponData[27].weapon_slot = (eWeaponSlot) 3;
    OriginalWeaponData[27].fire_offset = CVector ( 0.75f, -0.059999998658895f, 0.21999999880791f );
    OriginalWeaponData[27].skill_level = (eWeaponSkill) 1;
    OriginalWeaponData[27].required_skill_level = 200;
    OriginalWeaponData[27].anim_loop_start = 0.20000001788139f;
    OriginalWeaponData[27].anim_loop_stop = 0.51399999856949f;
    OriginalWeaponData[27].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalWeaponData[27].anim2_loop_start = 0.20000001788139f;
    OriginalWeaponData[27].anim2_loop_stop = 0.51399999856949f;
    OriginalWeaponData[27].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalWeaponData[27].anim_breakout_time = 1.1666667461395f;
    OriginalWeaponData[27].radius = 0.0f;
    OriginalWeaponData[27].aim_offset = 10;
    OriginalWeaponData[27].default_combo = 4;
    OriginalWeaponData[27].combos_available = 1;
    // 28 - Uzi
    OriginalWeaponData[28].weapon_range = 35.0f;
    OriginalWeaponData[28].target_range = 30.0f;
    OriginalWeaponData[28].accuracy = 1.0499999523163f;
    OriginalWeaponData[28].damage = 20;
    OriginalWeaponData[28].life_span = 0.0f;
    OriginalWeaponData[28].firing_speed = 0.0f;
    OriginalWeaponData[28].spread = 0.0f;
    OriginalWeaponData[28].maximum_clip_ammo = 50;
    OriginalWeaponData[28].move_speed = 0.0f;
    OriginalWeaponData[28].flags = 12339;
    OriginalWeaponData[28].anim_group = 13;
    OriginalWeaponData[28].fire_type = (eFireType) 1;
    OriginalWeaponData[28].model = 352;
    OriginalWeaponData[28].model2 = -1;
    OriginalWeaponData[28].weapon_slot = (eWeaponSlot) 4;
    OriginalWeaponData[28].fire_offset = CVector ( 0.25f, 0.0f, 0.10999999940395f );
    OriginalWeaponData[28].skill_level = (eWeaponSkill) 1;
    OriginalWeaponData[28].required_skill_level = 50;
    OriginalWeaponData[28].anim_loop_start = 0.20000001788139f;
    OriginalWeaponData[28].anim_loop_stop = 0.31400001049042f;
    OriginalWeaponData[28].anim_loop_bullet_fire = 0.20000001788139f;
    OriginalWeaponData[28].anim2_loop_start = 0.20000001788139f;
    OriginalWeaponData[28].anim2_loop_stop = 0.31400001049042f;
    OriginalWeaponData[28].anim2_loop_bullet_fire = 0.20000001788139f;
    OriginalWeaponData[28].anim_breakout_time = 3.3000001907349f;
    OriginalWeaponData[28].radius = 0.0f;
    OriginalWeaponData[28].aim_offset = 2;
    OriginalWeaponData[28].default_combo = 4;
    OriginalWeaponData[28].combos_available = 1;
    // 29 - MP5
    OriginalWeaponData[29].weapon_range = 45.0f;
    OriginalWeaponData[29].target_range = 40.0f;
    OriginalWeaponData[29].accuracy = 0.0f;
    OriginalWeaponData[29].damage = 25;
    OriginalWeaponData[29].life_span = 0.0f;
    OriginalWeaponData[29].firing_speed = 0.0f;
    OriginalWeaponData[29].spread = 0.0f;
    OriginalWeaponData[29].maximum_clip_ammo = 30;
    OriginalWeaponData[29].move_speed = 1.2999999523163f;
    OriginalWeaponData[29].flags = 28689;
    OriginalWeaponData[29].anim_group = 23;
    OriginalWeaponData[29].fire_type = (eFireType) 1;
    OriginalWeaponData[29].model = 353;
    OriginalWeaponData[29].model2 = -1;
    OriginalWeaponData[29].weapon_slot = (eWeaponSlot) 4;
    OriginalWeaponData[29].fire_offset = CVector ( 0.50999999046326f, -0.0099999997764826f, 0.20000000298023f );
    OriginalWeaponData[29].skill_level = (eWeaponSkill) 1;
    OriginalWeaponData[29].required_skill_level = 250;
    OriginalWeaponData[29].anim_loop_start = 0.20000001788139f;
    OriginalWeaponData[29].anim_loop_stop = 0.2940000295639f;
    OriginalWeaponData[29].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalWeaponData[29].anim2_loop_start = 0.20000001788139f;
    OriginalWeaponData[29].anim2_loop_stop = 0.2940000295639f;
    OriginalWeaponData[29].anim2_loop_bullet_fire = 0.20000001788139f;
    OriginalWeaponData[29].anim_breakout_time = 0.0f;
    OriginalWeaponData[29].radius = 0.0f;
    OriginalWeaponData[29].aim_offset = 12;
    OriginalWeaponData[29].default_combo = 4;
    OriginalWeaponData[29].combos_available = 1;
    // 30 - AK-47
    OriginalWeaponData[30].weapon_range = 70.0f;
    OriginalWeaponData[30].target_range = 45.0f;
    OriginalWeaponData[30].accuracy = 0.5f;
    OriginalWeaponData[30].damage = 30;
    OriginalWeaponData[30].life_span = 0.0f;
    OriginalWeaponData[30].firing_speed = 0.0f;
    OriginalWeaponData[30].spread = 0.0f;
    OriginalWeaponData[30].maximum_clip_ammo = 30;
    OriginalWeaponData[30].move_speed = 0.0f;
    OriginalWeaponData[30].flags = 28689;
    OriginalWeaponData[30].anim_group = 25;
    OriginalWeaponData[30].fire_type = (eFireType) 1;
    OriginalWeaponData[30].model = 355;
    OriginalWeaponData[30].model2 = -1;
    OriginalWeaponData[30].weapon_slot = (eWeaponSlot) 5;
    OriginalWeaponData[30].fire_offset = CVector ( 0.77999997138977f, -0.059999998658895f, 0.12999999523163f );
    OriginalWeaponData[30].skill_level = (eWeaponSkill) 1;
    OriginalWeaponData[30].required_skill_level = 200;
    OriginalWeaponData[30].anim_loop_start = 0.20000001788139f;
    OriginalWeaponData[30].anim_loop_stop = 0.31400001049042f;
    OriginalWeaponData[30].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalWeaponData[30].anim2_loop_start = 0.20000001788139f;
    OriginalWeaponData[30].anim2_loop_stop = 0.31400001049042f;
    OriginalWeaponData[30].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalWeaponData[30].anim_breakout_time = 3.3000001907349f;
    OriginalWeaponData[30].radius = 0.0f;
    OriginalWeaponData[30].aim_offset = 14;
    OriginalWeaponData[30].default_combo = 4;
    OriginalWeaponData[30].combos_available = 1;
    // 31 - M4
    OriginalWeaponData[31].weapon_range = 90.0f;
    OriginalWeaponData[31].target_range = 45.0f;
    OriginalWeaponData[31].accuracy = 0.64999997615814f;
    OriginalWeaponData[31].damage = 30;
    OriginalWeaponData[31].life_span = 0.0f;
    OriginalWeaponData[31].firing_speed = 0.0f;
    OriginalWeaponData[31].spread = 0.0f;
    OriginalWeaponData[31].maximum_clip_ammo = 50;
    OriginalWeaponData[31].move_speed = 0.0f;
    OriginalWeaponData[31].flags = 28689;
    OriginalWeaponData[31].anim_group = 25;
    OriginalWeaponData[31].fire_type = (eFireType) 1;
    OriginalWeaponData[31].model = 356;
    OriginalWeaponData[31].model2 = -1;
    OriginalWeaponData[31].weapon_slot = (eWeaponSlot) 5;
    OriginalWeaponData[31].fire_offset = CVector ( 0.74000000953674f, -0.03999999910593f, 0.12999999523163f );
    OriginalWeaponData[31].skill_level = (eWeaponSkill) 1;
    OriginalWeaponData[31].required_skill_level = 200;
    OriginalWeaponData[31].anim_loop_start = 0.20000001788139f;
    OriginalWeaponData[31].anim_loop_stop = 0.31400001049042f;
    OriginalWeaponData[31].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalWeaponData[31].anim2_loop_start = 0.20000001788139f;
    OriginalWeaponData[31].anim2_loop_stop = 0.31400001049042f;
    OriginalWeaponData[31].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalWeaponData[31].anim_breakout_time = 3.3000001907349f;
    OriginalWeaponData[31].radius = 0.0f;
    OriginalWeaponData[31].aim_offset = 14;
    OriginalWeaponData[31].default_combo = 4;
    OriginalWeaponData[31].combos_available = 1;
    // 32 - Tec-9
    OriginalWeaponData[32].weapon_range = 35.0f;
    OriginalWeaponData[32].target_range = 30.0f;
    OriginalWeaponData[32].accuracy = 1.0499999523163f;
    OriginalWeaponData[32].damage = 20;
    OriginalWeaponData[32].life_span = 0.0f;
    OriginalWeaponData[32].firing_speed = 0.0f;
    OriginalWeaponData[32].spread = 0.0f;
    OriginalWeaponData[32].maximum_clip_ammo = 50;
    OriginalWeaponData[32].move_speed = 0.0f;
    OriginalWeaponData[32].flags = 12339;
    OriginalWeaponData[32].anim_group = 13;
    OriginalWeaponData[32].fire_type = (eFireType) 1;
    OriginalWeaponData[32].model = 372;
    OriginalWeaponData[32].model2 = -1;
    OriginalWeaponData[32].weapon_slot = (eWeaponSlot) 4;
    OriginalWeaponData[32].fire_offset = CVector ( 0.44999998807907f, -0.050000000745058f, 0.10999999940395f );
    OriginalWeaponData[32].skill_level = (eWeaponSkill) 1;
    OriginalWeaponData[32].required_skill_level = 50;
    OriginalWeaponData[32].anim_loop_start = 0.36666667461395f;
    OriginalWeaponData[32].anim_loop_stop = 0.48066666722298f;
    OriginalWeaponData[32].anim_loop_bullet_fire = 0.40000003576279f;
    OriginalWeaponData[32].anim2_loop_start = 0.20000001788139f;
    OriginalWeaponData[32].anim2_loop_stop = 0.35400003194809f;
    OriginalWeaponData[32].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalWeaponData[32].anim_breakout_time = 3.3000001907349f;
    OriginalWeaponData[32].radius = 0.0f;
    OriginalWeaponData[32].aim_offset = 2;
    OriginalWeaponData[32].default_combo = 4;
    OriginalWeaponData[32].combos_available = 1;
    // 33 - Rifle
    OriginalWeaponData[33].weapon_range = 100.0f;
    OriginalWeaponData[33].target_range = 55.0f;
    OriginalWeaponData[33].accuracy = 0.0f;
    OriginalWeaponData[33].damage = 75;
    OriginalWeaponData[33].life_span = 0.0f;
    OriginalWeaponData[33].firing_speed = 0.0f;
    OriginalWeaponData[33].spread = 0.0f;
    OriginalWeaponData[33].maximum_clip_ammo = 1;
    OriginalWeaponData[33].move_speed = 1.2000000476837f;
    OriginalWeaponData[33].flags = -24568;
    OriginalWeaponData[33].anim_group = 27;
    OriginalWeaponData[33].fire_type = (eFireType) 1;
    OriginalWeaponData[33].model = 357;
    OriginalWeaponData[33].model2 = -1;
    OriginalWeaponData[33].weapon_slot = (eWeaponSlot) 6;
    OriginalWeaponData[33].fire_offset = CVector ( 0.69999998807907f, -0.050000000745058f, 0.20000000298023f );
    OriginalWeaponData[33].skill_level = (eWeaponSkill) 1;
    OriginalWeaponData[33].required_skill_level = 300;
    OriginalWeaponData[33].anim_loop_start = 0.20000001788139f;
    OriginalWeaponData[33].anim_loop_stop = 0.55400002002716f;
    OriginalWeaponData[33].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalWeaponData[33].anim2_loop_start = 0.20000001788139f;
    OriginalWeaponData[33].anim2_loop_stop = 0.55400002002716f;
    OriginalWeaponData[33].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalWeaponData[33].anim_breakout_time = 3.3000001907349f;
    OriginalWeaponData[33].radius = 0.0f;
    OriginalWeaponData[33].aim_offset = 16;
    OriginalWeaponData[33].default_combo = 4;
    OriginalWeaponData[33].combos_available = 1;
    // 34 - Sniper
    OriginalWeaponData[34].weapon_range = 300.0f;
    OriginalWeaponData[34].target_range = 250.0f;
    OriginalWeaponData[34].accuracy = 0.0f;
    OriginalWeaponData[34].damage = 125;
    OriginalWeaponData[34].life_span = 0.0f;
    OriginalWeaponData[34].firing_speed = 0.0f;
    OriginalWeaponData[34].spread = 0.0f;
    OriginalWeaponData[34].maximum_clip_ammo = 1;
    OriginalWeaponData[34].move_speed = 0.0f;
    OriginalWeaponData[34].flags = -24556;
    OriginalWeaponData[34].anim_group = 27;
    OriginalWeaponData[34].fire_type = (eFireType) 1;
    OriginalWeaponData[34].model = 358;
    OriginalWeaponData[34].model2 = -1;
    OriginalWeaponData[34].weapon_slot = (eWeaponSlot) 6;
    OriginalWeaponData[34].fire_offset = CVector ( 0.69999998807907f, -0.050000000745058f, 0.20000000298023f );
    OriginalWeaponData[34].skill_level = (eWeaponSkill) 1;
    OriginalWeaponData[34].required_skill_level = 300;
    OriginalWeaponData[34].anim_loop_start = 0.20000001788139f;
    OriginalWeaponData[34].anim_loop_stop = 0.55400002002716f;
    OriginalWeaponData[34].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalWeaponData[34].anim2_loop_start = 0.20000001788139f;
    OriginalWeaponData[34].anim2_loop_stop = 0.55400002002716f;
    OriginalWeaponData[34].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalWeaponData[34].anim_breakout_time = 3.3000001907349f;
    OriginalWeaponData[34].radius = 0.0f;
    OriginalWeaponData[34].aim_offset = 16;
    OriginalWeaponData[34].default_combo = 4;
    OriginalWeaponData[34].combos_available = 1;
    // 35 - Rocket Launcher
    OriginalWeaponData[35].weapon_range = 55.0f;
    OriginalWeaponData[35].target_range = 50.0f;
    OriginalWeaponData[35].accuracy = 0.0f;
    OriginalWeaponData[35].damage = 75;
    OriginalWeaponData[35].life_span = 0.0f;
    OriginalWeaponData[35].firing_speed = 0.0f;
    OriginalWeaponData[35].spread = 0.0f;
    OriginalWeaponData[35].maximum_clip_ammo = 1;
    OriginalWeaponData[35].move_speed = 0.0f;
    OriginalWeaponData[35].flags = -32236;
    OriginalWeaponData[35].anim_group = 30;
    OriginalWeaponData[35].fire_type = (eFireType) 2;
    OriginalWeaponData[35].model = 359;
    OriginalWeaponData[35].model2 = -1;
    OriginalWeaponData[35].weapon_slot = (eWeaponSlot) 7;
    OriginalWeaponData[35].fire_offset = CVector ( 0.41999998688698f, 0.0f, 0.050000000745058f );
    OriginalWeaponData[35].skill_level = (eWeaponSkill) 1;
    OriginalWeaponData[35].required_skill_level = 0;
    OriginalWeaponData[35].anim_loop_start = 0.36666667461395f;
    OriginalWeaponData[35].anim_loop_stop = 0.62066662311554f;
    OriginalWeaponData[35].anim_loop_bullet_fire = 0.40000003576279f;
    OriginalWeaponData[35].anim2_loop_start = 0.46666669845581f;
    OriginalWeaponData[35].anim2_loop_stop = 3.2806665897369f;
    OriginalWeaponData[35].anim2_loop_bullet_fire = 0.5f;
    OriginalWeaponData[35].anim_breakout_time = 3.3000001907349f;
    OriginalWeaponData[35].radius = 0.0f;
    OriginalWeaponData[35].aim_offset = 19;
    OriginalWeaponData[35].default_combo = 4;
    OriginalWeaponData[35].combos_available = 1;
    // 36 - Rocket Launcher HS
    OriginalWeaponData[36].weapon_range = 55.0f;
    OriginalWeaponData[36].target_range = 50.0f;
    OriginalWeaponData[36].accuracy = 0.0f;
    OriginalWeaponData[36].damage = 75;
    OriginalWeaponData[36].life_span = 0.0f;
    OriginalWeaponData[36].firing_speed = 0.0f;
    OriginalWeaponData[36].spread = 0.0f;
    OriginalWeaponData[36].maximum_clip_ammo = 1;
    OriginalWeaponData[36].move_speed = 0.0f;
    OriginalWeaponData[36].flags = -32236;
    OriginalWeaponData[36].anim_group = 30;
    OriginalWeaponData[36].fire_type = (eFireType) 2;
    OriginalWeaponData[36].model = 360;
    OriginalWeaponData[36].model2 = -1;
    OriginalWeaponData[36].weapon_slot = (eWeaponSlot) 7;
    OriginalWeaponData[36].fire_offset = CVector ( 0.41999998688698f, 0.0f, 0.050000000745058f );
    OriginalWeaponData[36].skill_level = (eWeaponSkill) 1;
    OriginalWeaponData[36].required_skill_level = 0;
    OriginalWeaponData[36].anim_loop_start = 0.36666667461395f;
    OriginalWeaponData[36].anim_loop_stop = 0.62066662311554f;
    OriginalWeaponData[36].anim_loop_bullet_fire = 0.40000003576279f;
    OriginalWeaponData[36].anim2_loop_start = 0.46666669845581f;
    OriginalWeaponData[36].anim2_loop_stop = 3.2806665897369f;
    OriginalWeaponData[36].anim2_loop_bullet_fire = 0.5f;
    OriginalWeaponData[36].anim_breakout_time = 3.3000001907349f;
    OriginalWeaponData[36].radius = 0.0f;
    OriginalWeaponData[36].aim_offset = 19;
    OriginalWeaponData[36].default_combo = 4;
    OriginalWeaponData[36].combos_available = 1;
    // 37 - Flamethrower
    OriginalWeaponData[37].weapon_range = 5.0999999046326f;
    OriginalWeaponData[37].target_range = 4.0f;
    OriginalWeaponData[37].accuracy = 0.0f;
    OriginalWeaponData[37].damage = 25;
    OriginalWeaponData[37].life_span = 1000.0f;
    OriginalWeaponData[37].firing_speed = 0.5f;
    OriginalWeaponData[37].spread = 0.0f;
    OriginalWeaponData[37].maximum_clip_ammo = 500;
    OriginalWeaponData[37].move_speed = 0.0f;
    OriginalWeaponData[37].flags = 568;
    OriginalWeaponData[37].anim_group = 29;
    OriginalWeaponData[37].fire_type = (eFireType) 3;
    OriginalWeaponData[37].model = 361;
    OriginalWeaponData[37].model2 = -1;
    OriginalWeaponData[37].weapon_slot = (eWeaponSlot) 7;
    OriginalWeaponData[37].fire_offset = CVector ( 0.98000001907349f, 0.0f, 0.40000000596046f );
    OriginalWeaponData[37].skill_level = (eWeaponSkill) 1;
    OriginalWeaponData[37].required_skill_level = 0;
    OriginalWeaponData[37].anim_loop_start = 0.36666667461395f;
    OriginalWeaponData[37].anim_loop_stop = 0.38066667318344f;
    OriginalWeaponData[37].anim_loop_bullet_fire = 0.36666667461395f;
    OriginalWeaponData[37].anim2_loop_start = 0.36666667461395f;
    OriginalWeaponData[37].anim2_loop_stop = 0.38066667318344f;
    OriginalWeaponData[37].anim2_loop_bullet_fire = 0.36666667461395f;
    OriginalWeaponData[37].anim_breakout_time = 1.1666667461395f;
    OriginalWeaponData[37].radius = 0.0074999998323619f;
    OriginalWeaponData[37].aim_offset = 18;
    OriginalWeaponData[37].default_combo = 4;
    OriginalWeaponData[37].combos_available = 1;
    // 38 - Minigun
    OriginalWeaponData[38].weapon_range = 75.0f;
    OriginalWeaponData[38].target_range = 65.0f;
    OriginalWeaponData[38].accuracy = 0.0f;
    OriginalWeaponData[38].damage = 140;
    OriginalWeaponData[38].life_span = 0.0f;
    OriginalWeaponData[38].firing_speed = 0.0f;
    OriginalWeaponData[38].spread = 0.0f;
    OriginalWeaponData[38].maximum_clip_ammo = 500;
    OriginalWeaponData[38].move_speed = 0.0f;
    OriginalWeaponData[38].flags = 568;
    OriginalWeaponData[38].anim_group = 29;
    OriginalWeaponData[38].fire_type = (eFireType) 1;
    OriginalWeaponData[38].model = 362;
    OriginalWeaponData[38].model2 = -1;
    OriginalWeaponData[38].weapon_slot = (eWeaponSlot) 7;
    OriginalWeaponData[38].fire_offset = CVector ( 1.1499999761581f, 0.0f, 0.40000000596046f );
    OriginalWeaponData[38].skill_level = (eWeaponSkill) 1;
    OriginalWeaponData[38].required_skill_level = 0;
    OriginalWeaponData[38].anim_loop_start = 0.36666667461395f;
    OriginalWeaponData[38].anim_loop_stop = 0.38066667318344f;
    OriginalWeaponData[38].anim_loop_bullet_fire = 0.36666667461395f;
    OriginalWeaponData[38].anim2_loop_start = 0.36666667461395f;
    OriginalWeaponData[38].anim2_loop_stop = 0.38066667318344f;
    OriginalWeaponData[38].anim2_loop_bullet_fire = 0.36666667461395f;
    OriginalWeaponData[38].anim_breakout_time = 1.1666667461395f;
    OriginalWeaponData[38].radius = 0.0f;
    OriginalWeaponData[38].aim_offset = 18;
    OriginalWeaponData[38].default_combo = 4;
    OriginalWeaponData[38].combos_available = 1;
    // 39 - Satchel
    OriginalWeaponData[39].weapon_range = 40.0f;
    OriginalWeaponData[39].target_range = 30.0f;
    OriginalWeaponData[39].accuracy = 0.0f;
    OriginalWeaponData[39].damage = 75;
    OriginalWeaponData[39].life_span = 800.0f;
    OriginalWeaponData[39].firing_speed = 0.25f;
    OriginalWeaponData[39].spread = 0.0f;
    OriginalWeaponData[39].maximum_clip_ammo = 1;
    OriginalWeaponData[39].move_speed = 0.0f;
    OriginalWeaponData[39].flags = 256;
    OriginalWeaponData[39].anim_group = 28;
    OriginalWeaponData[39].fire_type = (eFireType) 2;
    OriginalWeaponData[39].model = 363;
    OriginalWeaponData[39].model2 = 364;
    OriginalWeaponData[39].weapon_slot = (eWeaponSlot) 8;
    OriginalWeaponData[39].fire_offset = CVector ( 0.0f, 0.0f, 0.0f );
    OriginalWeaponData[39].skill_level = (eWeaponSkill) 1;
    OriginalWeaponData[39].required_skill_level = 0;
    OriginalWeaponData[39].anim_loop_start = 0.0f;
    OriginalWeaponData[39].anim_loop_stop = 3.2939999103546f;
    OriginalWeaponData[39].anim_loop_bullet_fire = 0.20000001788139f;
    OriginalWeaponData[39].anim2_loop_start = 0.0f;
    OriginalWeaponData[39].anim2_loop_stop = 3.2939999103546f;
    OriginalWeaponData[39].anim2_loop_bullet_fire = 0.30000001192093f;
    OriginalWeaponData[39].anim_breakout_time = 3.3000001907349f;
    OriginalWeaponData[39].radius = -1.0f;
    OriginalWeaponData[39].aim_offset = 17;
    OriginalWeaponData[39].default_combo = 4;
    OriginalWeaponData[39].combos_available = 1;
    // 40 - Bomb
    OriginalWeaponData[40].weapon_range = 25.0f;
    OriginalWeaponData[40].target_range = 25.0f;
    OriginalWeaponData[40].accuracy = 0.0f;
    OriginalWeaponData[40].damage = 0;
    OriginalWeaponData[40].life_span = 0.0f;
    OriginalWeaponData[40].firing_speed = 0.0f;
    OriginalWeaponData[40].spread = 0.0f;
    OriginalWeaponData[40].maximum_clip_ammo = 1;
    OriginalWeaponData[40].move_speed = 0.0f;
    OriginalWeaponData[40].flags = 0;
    OriginalWeaponData[40].anim_group = 0;
    OriginalWeaponData[40].fire_type = (eFireType) 5;
    OriginalWeaponData[40].model = 364;
    OriginalWeaponData[40].model2 = -1;
    OriginalWeaponData[40].weapon_slot = (eWeaponSlot) 12;
    OriginalWeaponData[40].fire_offset = CVector ( 0.0f, 0.0f, 0.12999999523163f );
    OriginalWeaponData[40].skill_level = (eWeaponSkill) 1;
    OriginalWeaponData[40].required_skill_level = 0;
    OriginalWeaponData[40].anim_loop_start = 0.0f;
    OriginalWeaponData[40].anim_loop_stop = 0.3139999806881f;
    OriginalWeaponData[40].anim_loop_bullet_fire = 0.1000000089407f;
    OriginalWeaponData[40].anim2_loop_start = 0.0f;
    OriginalWeaponData[40].anim2_loop_stop = 0.3139999806881f;
    OriginalWeaponData[40].anim2_loop_bullet_fire = 0.1000000089407f;
    OriginalWeaponData[40].anim_breakout_time = 3.3000001907349f;
    OriginalWeaponData[40].radius = 0.0f;
    OriginalWeaponData[40].aim_offset = 0;
    OriginalWeaponData[40].default_combo = 4;
    OriginalWeaponData[40].combos_available = 1;
    // 41 - Spraycan
    OriginalWeaponData[41].weapon_range = 6.0999999046326f;
    OriginalWeaponData[41].target_range = 4.0f;
    OriginalWeaponData[41].accuracy = 0.0f;
    OriginalWeaponData[41].damage = 1;
    OriginalWeaponData[41].life_span = 1000.0f;
    OriginalWeaponData[41].firing_speed = 0.050000000745058f;
    OriginalWeaponData[41].spread = 0.0099999997764826f;
    OriginalWeaponData[41].maximum_clip_ammo = 500;
    OriginalWeaponData[41].move_speed = 0.0f;
    OriginalWeaponData[41].flags = 1073;
    OriginalWeaponData[41].anim_group = 31;
    OriginalWeaponData[41].fire_type = (eFireType) 3;
    OriginalWeaponData[41].model = 365;
    OriginalWeaponData[41].model2 = -1;
    OriginalWeaponData[41].weapon_slot = (eWeaponSlot) 9;
    OriginalWeaponData[41].fire_offset = CVector ( 0.20000000298023f, 0.0f, -0.10000000149012f );
    OriginalWeaponData[41].skill_level = (eWeaponSkill) 1;
    OriginalWeaponData[41].required_skill_level = 0;
    OriginalWeaponData[41].anim_loop_start = 0.40000003576279f;
    OriginalWeaponData[41].anim_loop_stop = 1.9140000343323f;
    OriginalWeaponData[41].anim_loop_bullet_fire = 0.4333333671093f;
    OriginalWeaponData[41].anim2_loop_start = 0.36666667461395f;
    OriginalWeaponData[41].anim2_loop_stop = 1.9206665754318f;
    OriginalWeaponData[41].anim2_loop_bullet_fire = 0.4333333671093f;
    OriginalWeaponData[41].anim_breakout_time = 3.3000001907349f;
    OriginalWeaponData[41].radius = 0.5f;
    OriginalWeaponData[41].aim_offset = 20;
    OriginalWeaponData[41].default_combo = 4;
    OriginalWeaponData[41].combos_available = 1;
    // 42 - Fire Extinguisher
    OriginalWeaponData[42].weapon_range = 10.10000038147f;
    OriginalWeaponData[42].target_range = 10.0f;
    OriginalWeaponData[42].accuracy = 0.0f;
    OriginalWeaponData[42].damage = 1;
    OriginalWeaponData[42].life_span = 1000.0f;
    OriginalWeaponData[42].firing_speed = 0.10000000149012f;
    OriginalWeaponData[42].spread = 0.0099999997764826f;
    OriginalWeaponData[42].maximum_clip_ammo = 500;
    OriginalWeaponData[42].move_speed = 0.0f;
    OriginalWeaponData[42].flags = 56;
    OriginalWeaponData[42].anim_group = 29;
    OriginalWeaponData[42].fire_type = (eFireType) 3;
    OriginalWeaponData[42].model = 366;
    OriginalWeaponData[42].model2 = -1;
    OriginalWeaponData[42].weapon_slot = (eWeaponSlot) 9;
    OriginalWeaponData[42].fire_offset = CVector ( 0.44999998807907f, 0.10000000149012f, 0.15000000596046f );
    OriginalWeaponData[42].skill_level = (eWeaponSkill) 1;
    OriginalWeaponData[42].required_skill_level = 0;
    OriginalWeaponData[42].anim_loop_start = 0.36666667461395f;
    OriginalWeaponData[42].anim_loop_stop = 0.38066667318344f;
    OriginalWeaponData[42].anim_loop_bullet_fire = 0.36666667461395f;
    OriginalWeaponData[42].anim2_loop_start = 0.36666667461395f;
    OriginalWeaponData[42].anim2_loop_stop = 0.38066667318344f;
    OriginalWeaponData[42].anim2_loop_bullet_fire = 0.36666667461395f;
    OriginalWeaponData[42].anim_breakout_time = 1.1666667461395f;
    OriginalWeaponData[42].radius = 0.5f;
    OriginalWeaponData[42].aim_offset = 18;
    OriginalWeaponData[42].default_combo = 4;
    OriginalWeaponData[42].combos_available = 1;
}

bool CWeaponStatManager::LoadDefault ( CWeaponStat* pDest, eWeaponType weaponType )
{
    if ( pDest )
    {
        int iVal = (int)weaponType;
        if ( iVal >= 22 && iVal <= 42 )
        {
            pDest->SetWeaponRange ( OriginalWeaponData[iVal].weapon_range );
            pDest->SetTargetRange ( OriginalWeaponData[iVal].target_range );
            pDest->SetAccuracy ( OriginalWeaponData[iVal].accuracy );
            pDest->SetDamagePerHit ( OriginalWeaponData[iVal].damage );
            pDest->SetLifeSpan ( OriginalWeaponData[iVal].life_span );
            pDest->SetFiringSpeed ( OriginalWeaponData[iVal].firing_speed );
            pDest->SetSpread ( OriginalWeaponData[iVal].spread );
            pDest->SetMaximumClipAmmo ( OriginalWeaponData[iVal].maximum_clip_ammo );
            pDest->SetMoveSpeed ( OriginalWeaponData[iVal].move_speed );
            pDest->SetFlags ( OriginalWeaponData[iVal].flags );
            pDest->SetAnimGroup ( OriginalWeaponData[iVal].anim_group );
            pDest->SetFireType ( OriginalWeaponData[iVal].fire_type );
            pDest->SetModel ( OriginalWeaponData[iVal].model );
            pDest->SetModel2 ( OriginalWeaponData[iVal].model2 );
            pDest->SetSlot ( OriginalWeaponData[iVal].weapon_slot );
            pDest->SetFireOffset ( &OriginalWeaponData[iVal].fire_offset );
            pDest->SetSkill ( OriginalWeaponData[iVal].skill_level );
            pDest->SetRequiredStatLevel ( OriginalWeaponData[iVal].required_skill_level );
            pDest->SetWeaponAnimLoopStart ( OriginalWeaponData[iVal].anim_loop_start );
            pDest->SetWeaponAnimLoopStop ( OriginalWeaponData[iVal].anim_loop_stop );
            pDest->SetWeaponAnimLoopFireTime ( OriginalWeaponData[iVal].anim_loop_bullet_fire );
            pDest->SetWeaponAnim2LoopStart ( OriginalWeaponData[iVal].anim2_loop_start );
            pDest->SetWeaponAnim2LoopStop ( OriginalWeaponData[iVal].anim2_loop_stop );
            pDest->SetWeaponAnim2LoopFireTime ( OriginalWeaponData[iVal].anim2_loop_bullet_fire );
            pDest->SetWeaponAnimBreakoutTime ( OriginalWeaponData[iVal].anim_breakout_time );
            pDest->SetWeaponRadius ( OriginalWeaponData[iVal].radius );
            pDest->SetAimOffsetIndex ( OriginalWeaponData[iVal].aim_offset );
            pDest->SetDefaultCombo ( OriginalWeaponData[iVal].default_combo );
            pDest->SetCombosAvailable ( OriginalWeaponData[iVal].combos_available );
        }
    }
    return true;
}