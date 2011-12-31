/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CWeaponStatManager.h
*  PURPOSE:     source file for the custom weapon stats manager.
*  DEVELOPERS:  Cazomino05 < Cazomino05@gmail.com >
*
*****************************************************************************/
#include "StdInc.h"

sWeaponInfo CWeaponStatManager::OriginalPoorWeaponData [ WEAPONTYPE_MAX+1 ];
sWeaponInfo CWeaponStatManager::OriginalNormalWeaponData [ WEAPONTYPE_MAX+1 ];
sWeaponInfo CWeaponStatManager::OriginalHitmanWeaponData [ WEAPONTYPE_MAX+1 ];
CWeaponStatManager::CWeaponStatManager ( )
{
    Init();
    CWeaponStat* pWeaponStat = NULL;
    // Make our list nicer by starting at the non-skill weapons so we don't have duplicates of STD weapon skill
    for ( int i = 0; i <= WEAPONTYPE_EXTINGUISHER; i++)
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
            eWeaponSkill weaponSkill = (eWeaponSkill) skill;
            eWeaponType weaponType = (eWeaponType) (WEAPONTYPE_PISTOL + i);
            // Storage for Original weapon data ( for resetting )
            pWeaponStat = new CWeaponStat ( weaponType, weaponSkill );
            m_OriginalWeaponData.push_back ( pWeaponStat );
            // Store our defaults for this weapon.
            LoadDefault ( pWeaponStat, weaponType, weaponSkill );

            // Storage for new weapon data ( for script use )
            pWeaponStat = new CWeaponStat ( weaponType, weaponSkill );
            m_WeaponData.push_back ( pWeaponStat );
            // Store our defaults for this weapon.
            LoadDefault ( pWeaponStat, weaponType, weaponSkill );
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
    for ( std::list < CWeaponStat* >::iterator iter = m_WeaponData.begin (); iter != m_WeaponData.end ();iter++ )
    {
        if ( ( (*iter)->GetWeaponSkillLevel() == skill || type > WEAPONTYPE_TEC9 ) && 
            (*iter)->GetWeaponType () == type )
        {
            return (*iter);
        }
    }
    return NULL;
}

CWeaponStat* CWeaponStatManager::GetWeaponStatsFromSkillLevel ( eWeaponType type, float fSkillLevel )
{
    CWeaponStat * pPoor = GetWeaponStats ( type, WEAPONSKILL_POOR );
    CWeaponStat * pStd = GetWeaponStats ( type, WEAPONSKILL_STD );
    CWeaponStat * pPro = GetWeaponStats ( type, WEAPONSKILL_PRO );
    if ( pStd )
    {
        if ( pPoor && pPro )
        {
            if ( fSkillLevel >= pPro->GetRequiredStatLevel () ) return pPro;
            else if ( fSkillLevel >= pStd->GetRequiredStatLevel () ) return pStd;
            else return pPoor;
        }
        else
            return pStd;
    }
    return NULL;
}

CWeaponStat* CWeaponStatManager::GetOriginalWeaponStats ( eWeaponType type, eWeaponSkill skill )
{
    for ( std::list < CWeaponStat* >::iterator iter = m_OriginalWeaponData.begin (); iter != m_OriginalWeaponData.end ();iter++ )
    {
        if ( ( (*iter)->GetWeaponSkillLevel() == skill || type > WEAPONTYPE_TEC9 ) && 
            (*iter)->GetWeaponType () == type )
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
    // - 
    /*
    count = 22
    statid = 3
    addCommandHandler("dump", function ()
	    while (count < 33) do
		    hFile = fileOpen ( "dump.txt" )
		    fileSetPos(hFile, fileGetSize ( hFile ))
		    i = count
		    fileWrite(hFile, "// " .. i .. " - " .. getWeaponNameFromID( i ) .. " Stat: " .. statid )
		    fileWrite(hFile, "\r\n")
		    for k,v in ipairs(props) do
			    outputChatBox(v)
			    fileWrite(hFile, Dump(v))
			    fileWrite(hFile, "\r\n")
		    end
		    fileClose(hFile)
		    count = count + 1
	    end
    end)
    function Array()
        if (statid == 3) then
            return "OriginalHitmanData["
        elseif ( statid == 2 ) then
            return "OriginalGangsterData["
        else
            return "OriginalWeaponData["
        end
    end
    function Dump(strInfo)
	    i = count
	    -- Special case for Vectors
	    if ( strInfo == "fire_offset") then
		    fx, fy, fz = getWeaponProperty(i, statid, strInfo)
		    return Array() .. i .. "]." .. strInfo .. " = CVector ( " .. floatify ( fx ) .. ", " .. floatify ( fy ) .. ", " .. floatify ( fz ) .. " );"
	    end
	    -- Avoids compile errors ( Cannot convert from x to y )
	    if ( strInfo == "skill_level" ) then
		    return Array() .. i .. "]." .. strInfo .. " = (eWeaponSkill) " .. getWeaponProperty(i, statid, strInfo) .. ";"
	    end

	    if ( strInfo == "weapon_slot" ) then
		    return Array() .. i .. "]." .. strInfo .. " = (eWeaponSlot) " .. getWeaponProperty(i, statid, strInfo) .. ";"
	    end

	    if ( strInfo == "fire_type" ) then
		    return Array() .. i .. "]." .. strInfo .. " = (eFireType) " .. getWeaponProperty(i, statid, strInfo) .. ";"
	    end
	    -- end
	    -- Avoids Conversion from Double to float warnings.
	    if ( strInfo == "weapon_range" or strInfo == "target_range" or strInfo == "accuracy" or strInfo == "move_speed" or strInfo == "anim_loop_start" or strInfo == "anim_loop_stop" or strInfo == "anim_loop_bullet_fire" or strInfo == "anim2_loop_start" or strInfo == "anim2_loop_stop" or strInfo == "anim2_loop_bullet_fire" or strInfo == "anim_breakout_time" or strInfo == "firing_speed" or strInfo == "radius" or strInfo == "life_span" or strInfo == "spread") then
		    return Array() .. i .. "]." .. strInfo .. " = " .. floatify ( getWeaponProperty(i, statid, strInfo) ) .. ";"
	    end
	    -- Default
	    return Array() .. i .. "]." .. strInfo .. " = " .. getWeaponProperty(i, statid, strInfo) .. ";"
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
    OriginalNormalWeaponData[22].weapon_range = 35.0f;
    OriginalNormalWeaponData[22].target_range = 30.0f;
    OriginalNormalWeaponData[22].accuracy = 1.0f;
    OriginalNormalWeaponData[22].damage = 25;
    OriginalNormalWeaponData[22].life_span = 0.0f;
    OriginalNormalWeaponData[22].firing_speed = 0.0f;
    OriginalNormalWeaponData[22].spread = 0.0f;
    OriginalNormalWeaponData[22].maximum_clip_ammo = 17;
    OriginalNormalWeaponData[22].move_speed = 1.0f;
    OriginalNormalWeaponData[22].flags = 12339;
    OriginalNormalWeaponData[22].anim_group = 13;
    OriginalNormalWeaponData[22].fire_type = (eFireType) 1;
    OriginalNormalWeaponData[22].model = 346;
    OriginalNormalWeaponData[22].model2 = -1;
    OriginalNormalWeaponData[22].weapon_slot = (eWeaponSlot) 2;
    OriginalNormalWeaponData[22].fire_offset = CVector ( 0.25f, 0.050000000745058f, 0.090000003576279f );
    OriginalNormalWeaponData[22].skill_level = (eWeaponSkill) 1;
    OriginalNormalWeaponData[22].required_skill_level = 40;
    OriginalNormalWeaponData[22].anim_loop_start = 0.20000001788139f;
    OriginalNormalWeaponData[22].anim_loop_stop = 0.49399998784065f;
    OriginalNormalWeaponData[22].anim_loop_bullet_fire = 0.20000001788139f;
    OriginalNormalWeaponData[22].anim2_loop_start = 0.20000001788139f;
    OriginalNormalWeaponData[22].anim2_loop_stop = 0.49399998784065f;
    OriginalNormalWeaponData[22].anim2_loop_bullet_fire = 0.20000001788139f;
    OriginalNormalWeaponData[22].anim_breakout_time = 3.3000001907349f;
    OriginalNormalWeaponData[22].radius = 0.0f;
    OriginalNormalWeaponData[22].aim_offset = 2;
    OriginalNormalWeaponData[22].default_combo = 4;
    OriginalNormalWeaponData[22].combos_available = 1;
    // 23 - Silenced
    OriginalNormalWeaponData[23].weapon_range = 35.0f;
    OriginalNormalWeaponData[23].target_range = 30.0f;
    OriginalNormalWeaponData[23].accuracy = 1.25f;
    OriginalNormalWeaponData[23].damage = 40;
    OriginalNormalWeaponData[23].life_span = 0.0f;
    OriginalNormalWeaponData[23].firing_speed = 0.0f;
    OriginalNormalWeaponData[23].spread = 0.0f;
    OriginalNormalWeaponData[23].maximum_clip_ammo = 17;
    OriginalNormalWeaponData[23].move_speed = 1.2000000476837f;
    OriginalNormalWeaponData[23].flags = 28689;
    OriginalNormalWeaponData[23].anim_group = 18;
    OriginalNormalWeaponData[23].fire_type = (eFireType) 1;
    OriginalNormalWeaponData[23].model = 347;
    OriginalNormalWeaponData[23].model2 = -1;
    OriginalNormalWeaponData[23].weapon_slot = (eWeaponSlot) 2;
    OriginalNormalWeaponData[23].fire_offset = CVector ( 0.40000000596046f, 0.050000000745058f, 0.15000000596046f );
    OriginalNormalWeaponData[23].skill_level = (eWeaponSkill) 1;
    OriginalNormalWeaponData[23].required_skill_level = 500;
    OriginalNormalWeaponData[23].anim_loop_start = 0.20000001788139f;
    OriginalNormalWeaponData[23].anim_loop_stop = 0.59399998188019f;
    OriginalNormalWeaponData[23].anim_loop_bullet_fire = 0.20000001788139f;
    OriginalNormalWeaponData[23].anim2_loop_start = 0.20000001788139f;
    OriginalNormalWeaponData[23].anim2_loop_stop = 0.59399998188019f;
    OriginalNormalWeaponData[23].anim2_loop_bullet_fire = 0.20000001788139f;
    OriginalNormalWeaponData[23].anim_breakout_time = 1.3333333730698f;
    OriginalNormalWeaponData[23].radius = 0.0f;
    OriginalNormalWeaponData[23].aim_offset = 7;
    OriginalNormalWeaponData[23].default_combo = 4;
    OriginalNormalWeaponData[23].combos_available = 1;
    // 24 - Deagle
    OriginalNormalWeaponData[24].weapon_range = 35.0f;
    OriginalNormalWeaponData[24].target_range = 30.0f;
    OriginalNormalWeaponData[24].accuracy = 1.0f;
    OriginalNormalWeaponData[24].damage = 140;
    OriginalNormalWeaponData[24].life_span = 0.0f;
    OriginalNormalWeaponData[24].firing_speed = 0.0f;
    OriginalNormalWeaponData[24].spread = 0.0f;
    OriginalNormalWeaponData[24].maximum_clip_ammo = 7;
    OriginalNormalWeaponData[24].move_speed = 1.2000000476837f;
    OriginalNormalWeaponData[24].flags = 28689;
    OriginalNormalWeaponData[24].anim_group = 11;
    OriginalNormalWeaponData[24].fire_type = (eFireType) 1;
    OriginalNormalWeaponData[24].model = 348;
    OriginalNormalWeaponData[24].model2 = -1;
    OriginalNormalWeaponData[24].weapon_slot = (eWeaponSlot) 2;
    OriginalNormalWeaponData[24].fire_offset = CVector ( 0.40999999642372f, 0.029999999329448f, 0.11999999731779f );
    OriginalNormalWeaponData[24].skill_level = (eWeaponSkill) 1;
    OriginalNormalWeaponData[24].required_skill_level = 200;
    OriginalNormalWeaponData[24].anim_loop_start = 0.20000001788139f;
    OriginalNormalWeaponData[24].anim_loop_stop = 0.99399995803833f;
    OriginalNormalWeaponData[24].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalNormalWeaponData[24].anim2_loop_start = 0.20000001788139f;
    OriginalNormalWeaponData[24].anim2_loop_stop = 0.91400003433228f;
    OriginalNormalWeaponData[24].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalNormalWeaponData[24].anim_breakout_time = 1.3333333730698f;
    OriginalNormalWeaponData[24].radius = 0.0f;
    OriginalNormalWeaponData[24].aim_offset = 0;
    OriginalNormalWeaponData[24].default_combo = 4;
    OriginalNormalWeaponData[24].combos_available = 1;
    // 25 - Shotgun
    OriginalNormalWeaponData[25].weapon_range = 40.0f;
    OriginalNormalWeaponData[25].target_range = 35.0f;
    OriginalNormalWeaponData[25].accuracy = 1.2000000476837f;
    OriginalNormalWeaponData[25].damage = 10;
    OriginalNormalWeaponData[25].life_span = 0.0f;
    OriginalNormalWeaponData[25].firing_speed = 0.0f;
    OriginalNormalWeaponData[25].spread = 0.0f;
    OriginalNormalWeaponData[25].maximum_clip_ammo = 1;
    OriginalNormalWeaponData[25].move_speed = 1.1000000238419f;
    OriginalNormalWeaponData[25].flags = 8209;
    OriginalNormalWeaponData[25].anim_group = 19;
    OriginalNormalWeaponData[25].fire_type = (eFireType) 1;
    OriginalNormalWeaponData[25].model = 349;
    OriginalNormalWeaponData[25].model2 = -1;
    OriginalNormalWeaponData[25].weapon_slot = (eWeaponSlot) 3;
    OriginalNormalWeaponData[25].fire_offset = CVector ( 0.81999999284744f, -0.019999999552965f, 0.25f );
    OriginalNormalWeaponData[25].skill_level = (eWeaponSkill) 1;
    OriginalNormalWeaponData[25].required_skill_level = 200;
    OriginalNormalWeaponData[25].anim_loop_start = 0.20000001788139f;
    OriginalNormalWeaponData[25].anim_loop_stop = 1.2539999485016f;
    OriginalNormalWeaponData[25].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalNormalWeaponData[25].anim2_loop_start = 0.20000001788139f;
    OriginalNormalWeaponData[25].anim2_loop_stop = 1.3140000104904f;
    OriginalNormalWeaponData[25].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalNormalWeaponData[25].anim_breakout_time = 0.66666668653488f;
    OriginalNormalWeaponData[25].radius = 0.0f;
    OriginalNormalWeaponData[25].aim_offset = 8;
    OriginalNormalWeaponData[25].default_combo = 4;
    OriginalNormalWeaponData[25].combos_available = 1;
    // 26 - Sawed-off
    OriginalNormalWeaponData[26].weapon_range = 35.0f;
    OriginalNormalWeaponData[26].target_range = 30.0f;
    OriginalNormalWeaponData[26].accuracy = 0.80000001192093f;
    OriginalNormalWeaponData[26].damage = 10;
    OriginalNormalWeaponData[26].life_span = 0.0f;
    OriginalNormalWeaponData[26].firing_speed = 0.0f;
    OriginalNormalWeaponData[26].spread = 0.0f;
    OriginalNormalWeaponData[26].maximum_clip_ammo = 2;
    OriginalNormalWeaponData[26].move_speed = 1.1000000238419f;
    OriginalNormalWeaponData[26].flags = 12339;
    OriginalNormalWeaponData[26].anim_group = 16;
    OriginalNormalWeaponData[26].fire_type = (eFireType) 1;
    OriginalNormalWeaponData[26].model = 350;
    OriginalNormalWeaponData[26].model2 = -1;
    OriginalNormalWeaponData[26].weapon_slot = (eWeaponSlot) 3;
    OriginalNormalWeaponData[26].fire_offset = CVector ( 0.56000000238419f, 0.0f, 0.15000000596046f );
    OriginalNormalWeaponData[26].skill_level = (eWeaponSkill) 1;
    OriginalNormalWeaponData[26].required_skill_level = 200;
    OriginalNormalWeaponData[26].anim_loop_start = 0.20000001788139f;
    OriginalNormalWeaponData[26].anim_loop_stop = 0.49399998784065f;
    OriginalNormalWeaponData[26].anim_loop_bullet_fire = 0.20000001788139f;
    OriginalNormalWeaponData[26].anim2_loop_start = 0.20000001788139f;
    OriginalNormalWeaponData[26].anim2_loop_stop = 0.49399998784065f;
    OriginalNormalWeaponData[26].anim2_loop_bullet_fire = 0.20000001788139f;
    OriginalNormalWeaponData[26].anim_breakout_time = 1.0f;
    OriginalNormalWeaponData[26].radius = 0.0f;
    OriginalNormalWeaponData[26].aim_offset = 5;
    OriginalNormalWeaponData[26].default_combo = 4;
    OriginalNormalWeaponData[26].combos_available = 1;
    // 27 - Combat Shotgun
    OriginalNormalWeaponData[27].weapon_range = 40.0f;
    OriginalNormalWeaponData[27].target_range = 40.0f;
    OriginalNormalWeaponData[27].accuracy = 1.7999999523163f;
    OriginalNormalWeaponData[27].damage = 15;
    OriginalNormalWeaponData[27].life_span = 0.0f;
    OriginalNormalWeaponData[27].firing_speed = 0.0f;
    OriginalNormalWeaponData[27].spread = 0.0f;
    OriginalNormalWeaponData[27].maximum_clip_ammo = 7;
    OriginalNormalWeaponData[27].move_speed = 1.0f;
    OriginalNormalWeaponData[27].flags = 28689;
    OriginalNormalWeaponData[27].anim_group = 21;
    OriginalNormalWeaponData[27].fire_type = (eFireType) 1;
    OriginalNormalWeaponData[27].model = 351;
    OriginalNormalWeaponData[27].model2 = -1;
    OriginalNormalWeaponData[27].weapon_slot = (eWeaponSlot) 3;
    OriginalNormalWeaponData[27].fire_offset = CVector ( 0.75f, -0.059999998658895f, 0.21999999880791f );
    OriginalNormalWeaponData[27].skill_level = (eWeaponSkill) 1;
    OriginalNormalWeaponData[27].required_skill_level = 200;
    OriginalNormalWeaponData[27].anim_loop_start = 0.20000001788139f;
    OriginalNormalWeaponData[27].anim_loop_stop = 0.51399999856949f;
    OriginalNormalWeaponData[27].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalNormalWeaponData[27].anim2_loop_start = 0.20000001788139f;
    OriginalNormalWeaponData[27].anim2_loop_stop = 0.51399999856949f;
    OriginalNormalWeaponData[27].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalNormalWeaponData[27].anim_breakout_time = 1.1666667461395f;
    OriginalNormalWeaponData[27].radius = 0.0f;
    OriginalNormalWeaponData[27].aim_offset = 10;
    OriginalNormalWeaponData[27].default_combo = 4;
    OriginalNormalWeaponData[27].combos_available = 1;
    // 28 - Uzi
    OriginalNormalWeaponData[28].weapon_range = 35.0f;
    OriginalNormalWeaponData[28].target_range = 30.0f;
    OriginalNormalWeaponData[28].accuracy = 1.0499999523163f;
    OriginalNormalWeaponData[28].damage = 20;
    OriginalNormalWeaponData[28].life_span = 0.0f;
    OriginalNormalWeaponData[28].firing_speed = 0.0f;
    OriginalNormalWeaponData[28].spread = 0.0f;
    OriginalNormalWeaponData[28].maximum_clip_ammo = 50;
    OriginalNormalWeaponData[28].move_speed = 1.0f;
    OriginalNormalWeaponData[28].flags = 12339;
    OriginalNormalWeaponData[28].anim_group = 13;
    OriginalNormalWeaponData[28].fire_type = (eFireType) 1;
    OriginalNormalWeaponData[28].model = 352;
    OriginalNormalWeaponData[28].model2 = -1;
    OriginalNormalWeaponData[28].weapon_slot = (eWeaponSlot) 4;
    OriginalNormalWeaponData[28].fire_offset = CVector ( 0.25f, 0.0f, 0.10999999940395f );
    OriginalNormalWeaponData[28].skill_level = (eWeaponSkill) 1;
    OriginalNormalWeaponData[28].required_skill_level = 50;
    OriginalNormalWeaponData[28].anim_loop_start = 0.20000001788139f;
    OriginalNormalWeaponData[28].anim_loop_stop = 0.31400001049042f;
    OriginalNormalWeaponData[28].anim_loop_bullet_fire = 0.20000001788139f;
    OriginalNormalWeaponData[28].anim2_loop_start = 0.20000001788139f;
    OriginalNormalWeaponData[28].anim2_loop_stop = 0.31400001049042f;
    OriginalNormalWeaponData[28].anim2_loop_bullet_fire = 0.20000001788139f;
    OriginalNormalWeaponData[28].anim_breakout_time = 3.3000001907349f;
    OriginalNormalWeaponData[28].radius = 0.0f;
    OriginalNormalWeaponData[28].aim_offset = 2;
    OriginalNormalWeaponData[28].default_combo = 4;
    OriginalNormalWeaponData[28].combos_available = 1;
    // 29 - MP5
    OriginalNormalWeaponData[29].weapon_range = 45.0f;
    OriginalNormalWeaponData[29].target_range = 40.0f;
    OriginalNormalWeaponData[29].accuracy = 1.0f;
    OriginalNormalWeaponData[29].damage = 25;
    OriginalNormalWeaponData[29].life_span = 0.0f;
    OriginalNormalWeaponData[29].firing_speed = 0.0f;
    OriginalNormalWeaponData[29].spread = 0.0f;
    OriginalNormalWeaponData[29].maximum_clip_ammo = 30;
    OriginalNormalWeaponData[29].move_speed = 1.2999999523163f;
    OriginalNormalWeaponData[29].flags = 28689;
    OriginalNormalWeaponData[29].anim_group = 23;
    OriginalNormalWeaponData[29].fire_type = (eFireType) 1;
    OriginalNormalWeaponData[29].model = 353;
    OriginalNormalWeaponData[29].model2 = -1;
    OriginalNormalWeaponData[29].weapon_slot = (eWeaponSlot) 4;
    OriginalNormalWeaponData[29].fire_offset = CVector ( 0.50999999046326f, -0.0099999997764826f, 0.20000000298023f );
    OriginalNormalWeaponData[29].skill_level = (eWeaponSkill) 1;
    OriginalNormalWeaponData[29].required_skill_level = 250;
    OriginalNormalWeaponData[29].anim_loop_start = 0.20000001788139f;
    OriginalNormalWeaponData[29].anim_loop_stop = 0.2940000295639f;
    OriginalNormalWeaponData[29].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalNormalWeaponData[29].anim2_loop_start = 0.20000001788139f;
    OriginalNormalWeaponData[29].anim2_loop_stop = 0.2940000295639f;
    OriginalNormalWeaponData[29].anim2_loop_bullet_fire = 0.20000001788139f;
    OriginalNormalWeaponData[29].anim_breakout_time = 1.0f;
    OriginalNormalWeaponData[29].radius = 0.0f;
    OriginalNormalWeaponData[29].aim_offset = 12;
    OriginalNormalWeaponData[29].default_combo = 4;
    OriginalNormalWeaponData[29].combos_available = 1;
    // 30 - AK-47
    OriginalNormalWeaponData[30].weapon_range = 70.0f;
    OriginalNormalWeaponData[30].target_range = 45.0f;
    OriginalNormalWeaponData[30].accuracy = 0.5f;
    OriginalNormalWeaponData[30].damage = 30;
    OriginalNormalWeaponData[30].life_span = 0.0f;
    OriginalNormalWeaponData[30].firing_speed = 0.0f;
    OriginalNormalWeaponData[30].spread = 0.0f;
    OriginalNormalWeaponData[30].maximum_clip_ammo = 30;
    OriginalNormalWeaponData[30].move_speed = 1.0f;
    OriginalNormalWeaponData[30].flags = 28689;
    OriginalNormalWeaponData[30].anim_group = 25;
    OriginalNormalWeaponData[30].fire_type = (eFireType) 1;
    OriginalNormalWeaponData[30].model = 355;
    OriginalNormalWeaponData[30].model2 = -1;
    OriginalNormalWeaponData[30].weapon_slot = (eWeaponSlot) 5;
    OriginalNormalWeaponData[30].fire_offset = CVector ( 0.77999997138977f, -0.059999998658895f, 0.12999999523163f );
    OriginalNormalWeaponData[30].skill_level = (eWeaponSkill) 1;
    OriginalNormalWeaponData[30].required_skill_level = 200;
    OriginalNormalWeaponData[30].anim_loop_start = 0.20000001788139f;
    OriginalNormalWeaponData[30].anim_loop_stop = 0.31400001049042f;
    OriginalNormalWeaponData[30].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalNormalWeaponData[30].anim2_loop_start = 0.20000001788139f;
    OriginalNormalWeaponData[30].anim2_loop_stop = 0.31400001049042f;
    OriginalNormalWeaponData[30].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalNormalWeaponData[30].anim_breakout_time = 3.3000001907349f;
    OriginalNormalWeaponData[30].radius = 0.0f;
    OriginalNormalWeaponData[30].aim_offset = 14;
    OriginalNormalWeaponData[30].default_combo = 4;
    OriginalNormalWeaponData[30].combos_available = 1;
    // 31 - M4
    OriginalNormalWeaponData[31].weapon_range = 90.0f;
    OriginalNormalWeaponData[31].target_range = 45.0f;
    OriginalNormalWeaponData[31].accuracy = 0.64999997615814f;
    OriginalNormalWeaponData[31].damage = 30;
    OriginalNormalWeaponData[31].life_span = 0.0f;
    OriginalNormalWeaponData[31].firing_speed = 0.0f;
    OriginalNormalWeaponData[31].spread = 0.0f;
    OriginalNormalWeaponData[31].maximum_clip_ammo = 50;
    OriginalNormalWeaponData[31].move_speed = 1.0f;
    OriginalNormalWeaponData[31].flags = 28689;
    OriginalNormalWeaponData[31].anim_group = 25;
    OriginalNormalWeaponData[31].fire_type = (eFireType) 1;
    OriginalNormalWeaponData[31].model = 356;
    OriginalNormalWeaponData[31].model2 = -1;
    OriginalNormalWeaponData[31].weapon_slot = (eWeaponSlot) 5;
    OriginalNormalWeaponData[31].fire_offset = CVector ( 0.74000000953674f, -0.03999999910593f, 0.12999999523163f );
    OriginalNormalWeaponData[31].skill_level = (eWeaponSkill) 1;
    OriginalNormalWeaponData[31].required_skill_level = 200;
    OriginalNormalWeaponData[31].anim_loop_start = 0.20000001788139f;
    OriginalNormalWeaponData[31].anim_loop_stop = 0.31400001049042f;
    OriginalNormalWeaponData[31].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalNormalWeaponData[31].anim2_loop_start = 0.20000001788139f;
    OriginalNormalWeaponData[31].anim2_loop_stop = 0.31400001049042f;
    OriginalNormalWeaponData[31].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalNormalWeaponData[31].anim_breakout_time = 3.3000001907349f;
    OriginalNormalWeaponData[31].radius = 0.0f;
    OriginalNormalWeaponData[31].aim_offset = 14;
    OriginalNormalWeaponData[31].default_combo = 4;
    OriginalNormalWeaponData[31].combos_available = 1;
    // 32 - Tec-9
    OriginalNormalWeaponData[32].weapon_range = 35.0f;
    OriginalNormalWeaponData[32].target_range = 30.0f;
    OriginalNormalWeaponData[32].accuracy = 1.0499999523163f;
    OriginalNormalWeaponData[32].damage = 20;
    OriginalNormalWeaponData[32].life_span = 0.0f;
    OriginalNormalWeaponData[32].firing_speed = 0.0f;
    OriginalNormalWeaponData[32].spread = 0.0f;
    OriginalNormalWeaponData[32].maximum_clip_ammo = 50;
    OriginalNormalWeaponData[32].move_speed = 1.0f;
    OriginalNormalWeaponData[32].flags = 12339;
    OriginalNormalWeaponData[32].anim_group = 13;
    OriginalNormalWeaponData[32].fire_type = (eFireType) 1;
    OriginalNormalWeaponData[32].model = 372;
    OriginalNormalWeaponData[32].model2 = -1;
    OriginalNormalWeaponData[32].weapon_slot = (eWeaponSlot) 4;
    OriginalNormalWeaponData[32].fire_offset = CVector ( 0.44999998807907f, -0.050000000745058f, 0.10999999940395f );
    OriginalNormalWeaponData[32].skill_level = (eWeaponSkill) 1;
    OriginalNormalWeaponData[32].required_skill_level = 50;
    OriginalNormalWeaponData[32].anim_loop_start = 0.36666667461395f;
    OriginalNormalWeaponData[32].anim_loop_stop = 0.48066666722298f;
    OriginalNormalWeaponData[32].anim_loop_bullet_fire = 0.40000003576279f;
    OriginalNormalWeaponData[32].anim2_loop_start = 0.20000001788139f;
    OriginalNormalWeaponData[32].anim2_loop_stop = 0.35400003194809f;
    OriginalNormalWeaponData[32].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalNormalWeaponData[32].anim_breakout_time = 3.3000001907349f;
    OriginalNormalWeaponData[32].radius = 0.0f;
    OriginalNormalWeaponData[32].aim_offset = 2;
    OriginalNormalWeaponData[32].default_combo = 4;
    OriginalNormalWeaponData[32].combos_available = 1;
    // 33 - Rifle
    OriginalNormalWeaponData[33].weapon_range = 100.0f;
    OriginalNormalWeaponData[33].target_range = 55.0f;
    OriginalNormalWeaponData[33].accuracy = 1.0f;
    OriginalNormalWeaponData[33].damage = 75;
    OriginalNormalWeaponData[33].life_span = 0.0f;
    OriginalNormalWeaponData[33].firing_speed = 0.0f;
    OriginalNormalWeaponData[33].spread = 0.0f;
    OriginalNormalWeaponData[33].maximum_clip_ammo = 1;
    OriginalNormalWeaponData[33].move_speed = 1.2000000476837f;
    OriginalNormalWeaponData[33].flags = -24568;
    OriginalNormalWeaponData[33].anim_group = 27;
    OriginalNormalWeaponData[33].fire_type = (eFireType) 1;
    OriginalNormalWeaponData[33].model = 357;
    OriginalNormalWeaponData[33].model2 = -1;
    OriginalNormalWeaponData[33].weapon_slot = (eWeaponSlot) 6;
    OriginalNormalWeaponData[33].fire_offset = CVector ( 0.69999998807907f, -0.050000000745058f, 0.20000000298023f );
    OriginalNormalWeaponData[33].skill_level = (eWeaponSkill) 1;
    OriginalNormalWeaponData[33].required_skill_level = 300;
    OriginalNormalWeaponData[33].anim_loop_start = 0.20000001788139f;
    OriginalNormalWeaponData[33].anim_loop_stop = 0.55400002002716f;
    OriginalNormalWeaponData[33].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalNormalWeaponData[33].anim2_loop_start = 0.20000001788139f;
    OriginalNormalWeaponData[33].anim2_loop_stop = 0.55400002002716f;
    OriginalNormalWeaponData[33].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalNormalWeaponData[33].anim_breakout_time = 3.3000001907349f;
    OriginalNormalWeaponData[33].radius = 0.0f;
    OriginalNormalWeaponData[33].aim_offset = 16;
    OriginalNormalWeaponData[33].default_combo = 4;
    OriginalNormalWeaponData[33].combos_available = 1;
    // 34 - Sniper
    OriginalNormalWeaponData[34].weapon_range = 300.0f;
    OriginalNormalWeaponData[34].target_range = 250.0f;
    OriginalNormalWeaponData[34].accuracy = 1.0f;
    OriginalNormalWeaponData[34].damage = 125;
    OriginalNormalWeaponData[34].life_span = 0.0f;
    OriginalNormalWeaponData[34].firing_speed = 0.0f;
    OriginalNormalWeaponData[34].spread = 0.0f;
    OriginalNormalWeaponData[34].maximum_clip_ammo = 1;
    OriginalNormalWeaponData[34].move_speed = 1.0f;
    OriginalNormalWeaponData[34].flags = -24556;
    OriginalNormalWeaponData[34].anim_group = 27;
    OriginalNormalWeaponData[34].fire_type = (eFireType) 1;
    OriginalNormalWeaponData[34].model = 358;
    OriginalNormalWeaponData[34].model2 = -1;
    OriginalNormalWeaponData[34].weapon_slot = (eWeaponSlot) 6;
    OriginalNormalWeaponData[34].fire_offset = CVector ( 0.69999998807907f, -0.050000000745058f, 0.20000000298023f );
    OriginalNormalWeaponData[34].skill_level = (eWeaponSkill) 1;
    OriginalNormalWeaponData[34].required_skill_level = 300;
    OriginalNormalWeaponData[34].anim_loop_start = 0.20000001788139f;
    OriginalNormalWeaponData[34].anim_loop_stop = 0.55400002002716f;
    OriginalNormalWeaponData[34].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalNormalWeaponData[34].anim2_loop_start = 0.20000001788139f;
    OriginalNormalWeaponData[34].anim2_loop_stop = 0.55400002002716f;
    OriginalNormalWeaponData[34].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalNormalWeaponData[34].anim_breakout_time = 3.3000001907349f;
    OriginalNormalWeaponData[34].radius = 0.0f;
    OriginalNormalWeaponData[34].aim_offset = 16;
    OriginalNormalWeaponData[34].default_combo = 4;
    OriginalNormalWeaponData[34].combos_available = 1;
    // 35 - Rocket Launcher
    OriginalNormalWeaponData[35].weapon_range = 55.0f;
    OriginalNormalWeaponData[35].target_range = 50.0f;
    OriginalNormalWeaponData[35].accuracy = 1.0f;
    OriginalNormalWeaponData[35].damage = 75;
    OriginalNormalWeaponData[35].life_span = 0.0f;
    OriginalNormalWeaponData[35].firing_speed = 0.0f;
    OriginalNormalWeaponData[35].spread = 0.0f;
    OriginalNormalWeaponData[35].maximum_clip_ammo = 1;
    OriginalNormalWeaponData[35].move_speed = 1.0f;
    OriginalNormalWeaponData[35].flags = -32236;
    OriginalNormalWeaponData[35].anim_group = 30;
    OriginalNormalWeaponData[35].fire_type = (eFireType) 2;
    OriginalNormalWeaponData[35].model = 359;
    OriginalNormalWeaponData[35].model2 = -1;
    OriginalNormalWeaponData[35].weapon_slot = (eWeaponSlot) 7;
    OriginalNormalWeaponData[35].fire_offset = CVector ( 0.41999998688698f, 0.0f, 0.050000000745058f );
    OriginalNormalWeaponData[35].skill_level = (eWeaponSkill) 1;
    OriginalNormalWeaponData[35].required_skill_level = 0;
    OriginalNormalWeaponData[35].anim_loop_start = 0.36666667461395f;
    OriginalNormalWeaponData[35].anim_loop_stop = 0.62066662311554f;
    OriginalNormalWeaponData[35].anim_loop_bullet_fire = 0.40000003576279f;
    OriginalNormalWeaponData[35].anim2_loop_start = 0.46666669845581f;
    OriginalNormalWeaponData[35].anim2_loop_stop = 3.2806665897369f;
    OriginalNormalWeaponData[35].anim2_loop_bullet_fire = 0.5f;
    OriginalNormalWeaponData[35].anim_breakout_time = 3.3000001907349f;
    OriginalNormalWeaponData[35].radius = 0.0f;
    OriginalNormalWeaponData[35].aim_offset = 19;
    OriginalNormalWeaponData[35].default_combo = 4;
    OriginalNormalWeaponData[35].combos_available = 1;
    // 36 - Rocket Launcher HS
    OriginalNormalWeaponData[36].weapon_range = 55.0f;
    OriginalNormalWeaponData[36].target_range = 50.0f;
    OriginalNormalWeaponData[36].accuracy = 1.0f;
    OriginalNormalWeaponData[36].damage = 75;
    OriginalNormalWeaponData[36].life_span = 0.0f;
    OriginalNormalWeaponData[36].firing_speed = 0.0f;
    OriginalNormalWeaponData[36].spread = 0.0f;
    OriginalNormalWeaponData[36].maximum_clip_ammo = 1;
    OriginalNormalWeaponData[36].move_speed = 1.0f;
    OriginalNormalWeaponData[36].flags = -32236;
    OriginalNormalWeaponData[36].anim_group = 30;
    OriginalNormalWeaponData[36].fire_type = (eFireType) 2;
    OriginalNormalWeaponData[36].model = 360;
    OriginalNormalWeaponData[36].model2 = -1;
    OriginalNormalWeaponData[36].weapon_slot = (eWeaponSlot) 7;
    OriginalNormalWeaponData[36].fire_offset = CVector ( 0.41999998688698f, 0.0f, 0.050000000745058f );
    OriginalNormalWeaponData[36].skill_level = (eWeaponSkill) 1;
    OriginalNormalWeaponData[36].required_skill_level = 0;
    OriginalNormalWeaponData[36].anim_loop_start = 0.36666667461395f;
    OriginalNormalWeaponData[36].anim_loop_stop = 0.62066662311554f;
    OriginalNormalWeaponData[36].anim_loop_bullet_fire = 0.40000003576279f;
    OriginalNormalWeaponData[36].anim2_loop_start = 0.46666669845581f;
    OriginalNormalWeaponData[36].anim2_loop_stop = 3.2806665897369f;
    OriginalNormalWeaponData[36].anim2_loop_bullet_fire = 0.5f;
    OriginalNormalWeaponData[36].anim_breakout_time = 3.3000001907349f;
    OriginalNormalWeaponData[36].radius = 0.0f;
    OriginalNormalWeaponData[36].aim_offset = 19;
    OriginalNormalWeaponData[36].default_combo = 4;
    OriginalNormalWeaponData[36].combos_available = 1;
    // 37 - Flamethrower
    OriginalNormalWeaponData[37].weapon_range = 5.0999999046326f;
    OriginalNormalWeaponData[37].target_range = 4.0f;
    OriginalNormalWeaponData[37].accuracy = 1.0f;
    OriginalNormalWeaponData[37].damage = 25;
    OriginalNormalWeaponData[37].life_span = 1000.0f;
    OriginalNormalWeaponData[37].firing_speed = 0.5f;
    OriginalNormalWeaponData[37].spread = 2.0f;
    OriginalNormalWeaponData[37].maximum_clip_ammo = 500;
    OriginalNormalWeaponData[37].move_speed = 1.0f;
    OriginalNormalWeaponData[37].flags = 568;
    OriginalNormalWeaponData[37].anim_group = 29;
    OriginalNormalWeaponData[37].fire_type = (eFireType) 3;
    OriginalNormalWeaponData[37].model = 361;
    OriginalNormalWeaponData[37].model2 = -1;
    OriginalNormalWeaponData[37].weapon_slot = (eWeaponSlot) 7;
    OriginalNormalWeaponData[37].fire_offset = CVector ( 0.98000001907349f, 0.0f, 0.40000000596046f );
    OriginalNormalWeaponData[37].skill_level = (eWeaponSkill) 1;
    OriginalNormalWeaponData[37].required_skill_level = 0;
    OriginalNormalWeaponData[37].anim_loop_start = 0.36666667461395f;
    OriginalNormalWeaponData[37].anim_loop_stop = 0.38066667318344f;
    OriginalNormalWeaponData[37].anim_loop_bullet_fire = 0.36666667461395f;
    OriginalNormalWeaponData[37].anim2_loop_start = 0.36666667461395f;
    OriginalNormalWeaponData[37].anim2_loop_stop = 0.38066667318344f;
    OriginalNormalWeaponData[37].anim2_loop_bullet_fire = 0.36666667461395f;
    OriginalNormalWeaponData[37].anim_breakout_time = 1.1666667461395f;
    OriginalNormalWeaponData[37].radius = 0.0074999998323619f;
    OriginalNormalWeaponData[37].aim_offset = 18;
    OriginalNormalWeaponData[37].default_combo = 4;
    OriginalNormalWeaponData[37].combos_available = 1;
    // 38 - Minigun
    OriginalNormalWeaponData[38].weapon_range = 75.0f;
    OriginalNormalWeaponData[38].target_range = 65.0f;
    OriginalNormalWeaponData[38].accuracy = 1.0f;
    OriginalNormalWeaponData[38].damage = 140;
    OriginalNormalWeaponData[38].life_span = 0.0f;
    OriginalNormalWeaponData[38].firing_speed = 0.0f;
    OriginalNormalWeaponData[38].spread = 0.0f;
    OriginalNormalWeaponData[38].maximum_clip_ammo = 500;
    OriginalNormalWeaponData[38].move_speed = 1.0f;
    OriginalNormalWeaponData[38].flags = 568;
    OriginalNormalWeaponData[38].anim_group = 29;
    OriginalNormalWeaponData[38].fire_type = (eFireType) 1;
    OriginalNormalWeaponData[38].model = 362;
    OriginalNormalWeaponData[38].model2 = -1;
    OriginalNormalWeaponData[38].weapon_slot = (eWeaponSlot) 7;
    OriginalNormalWeaponData[38].fire_offset = CVector ( 1.1499999761581f, 0.0f, 0.40000000596046f );
    OriginalNormalWeaponData[38].skill_level = (eWeaponSkill) 1;
    OriginalNormalWeaponData[38].required_skill_level = 0;
    OriginalNormalWeaponData[38].anim_loop_start = 0.36666667461395f;
    OriginalNormalWeaponData[38].anim_loop_stop = 0.38066667318344f;
    OriginalNormalWeaponData[38].anim_loop_bullet_fire = 0.36666667461395f;
    OriginalNormalWeaponData[38].anim2_loop_start = 0.36666667461395f;
    OriginalNormalWeaponData[38].anim2_loop_stop = 0.38066667318344f;
    OriginalNormalWeaponData[38].anim2_loop_bullet_fire = 0.36666667461395f;
    OriginalNormalWeaponData[38].anim_breakout_time = 1.1666667461395f;
    OriginalNormalWeaponData[38].radius = 0.0f;
    OriginalNormalWeaponData[38].aim_offset = 18;
    OriginalNormalWeaponData[38].default_combo = 4;
    OriginalNormalWeaponData[38].combos_available = 1;
    // 39 - Satchel
    OriginalNormalWeaponData[39].weapon_range = 40.0f;
    OriginalNormalWeaponData[39].target_range = 30.0f;
    OriginalNormalWeaponData[39].accuracy = 1.0f;
    OriginalNormalWeaponData[39].damage = 75;
    OriginalNormalWeaponData[39].life_span = 800.0f;
    OriginalNormalWeaponData[39].firing_speed = 0.25f;
    OriginalNormalWeaponData[39].spread = 1.0f;
    OriginalNormalWeaponData[39].maximum_clip_ammo = 1;
    OriginalNormalWeaponData[39].move_speed = 1.0f;
    OriginalNormalWeaponData[39].flags = 256;
    OriginalNormalWeaponData[39].anim_group = 28;
    OriginalNormalWeaponData[39].fire_type = (eFireType) 2;
    OriginalNormalWeaponData[39].model = 363;
    OriginalNormalWeaponData[39].model2 = 364;
    OriginalNormalWeaponData[39].weapon_slot = (eWeaponSlot) 8;
    OriginalNormalWeaponData[39].fire_offset = CVector ( 0.0f, 0.0f, 0.0f );
    OriginalNormalWeaponData[39].skill_level = (eWeaponSkill) 1;
    OriginalNormalWeaponData[39].required_skill_level = 0;
    OriginalNormalWeaponData[39].anim_loop_start = 0.0f;
    OriginalNormalWeaponData[39].anim_loop_stop = 3.2939999103546f;
    OriginalNormalWeaponData[39].anim_loop_bullet_fire = 0.20000001788139f;
    OriginalNormalWeaponData[39].anim2_loop_start = 0.0f;
    OriginalNormalWeaponData[39].anim2_loop_stop = 3.2939999103546f;
    OriginalNormalWeaponData[39].anim2_loop_bullet_fire = 0.30000001192093f;
    OriginalNormalWeaponData[39].anim_breakout_time = 3.3000001907349f;
    OriginalNormalWeaponData[39].radius = -1.0f;
    OriginalNormalWeaponData[39].aim_offset = 17;
    OriginalNormalWeaponData[39].default_combo = 4;
    OriginalNormalWeaponData[39].combos_available = 1;
    // 40 - Bomb
    OriginalNormalWeaponData[40].weapon_range = 25.0f;
    OriginalNormalWeaponData[40].target_range = 25.0f;
    OriginalNormalWeaponData[40].accuracy = 1.0f;
    OriginalNormalWeaponData[40].damage = 0;
    OriginalNormalWeaponData[40].life_span = 0.0f;
    OriginalNormalWeaponData[40].firing_speed = 0.0f;
    OriginalNormalWeaponData[40].spread = 0.0f;
    OriginalNormalWeaponData[40].maximum_clip_ammo = 1;
    OriginalNormalWeaponData[40].move_speed = 1.0f;
    OriginalNormalWeaponData[40].flags = 0;
    OriginalNormalWeaponData[40].anim_group = 0;
    OriginalNormalWeaponData[40].fire_type = (eFireType) 5;
    OriginalNormalWeaponData[40].model = 364;
    OriginalNormalWeaponData[40].model2 = -1;
    OriginalNormalWeaponData[40].weapon_slot = (eWeaponSlot) 12;
    OriginalNormalWeaponData[40].fire_offset = CVector ( 0.0f, 0.0f, 0.12999999523163f );
    OriginalNormalWeaponData[40].skill_level = (eWeaponSkill) 1;
    OriginalNormalWeaponData[40].required_skill_level = 0;
    OriginalNormalWeaponData[40].anim_loop_start = 0.0f;
    OriginalNormalWeaponData[40].anim_loop_stop = 0.3139999806881f;
    OriginalNormalWeaponData[40].anim_loop_bullet_fire = 0.1000000089407f;
    OriginalNormalWeaponData[40].anim2_loop_start = 0.0f;
    OriginalNormalWeaponData[40].anim2_loop_stop = 0.3139999806881f;
    OriginalNormalWeaponData[40].anim2_loop_bullet_fire = 0.1000000089407f;
    OriginalNormalWeaponData[40].anim_breakout_time = 3.3000001907349f;
    OriginalNormalWeaponData[40].radius = 0.0f;
    OriginalNormalWeaponData[40].aim_offset = 0;
    OriginalNormalWeaponData[40].default_combo = 4;
    OriginalNormalWeaponData[40].combos_available = 1;
    // 41 - Spraycan
    OriginalNormalWeaponData[41].weapon_range = 6.0999999046326f;
    OriginalNormalWeaponData[41].target_range = 4.0f;
    OriginalNormalWeaponData[41].accuracy = 1.0f;
    OriginalNormalWeaponData[41].damage = 1;
    OriginalNormalWeaponData[41].life_span = 1000.0f;
    OriginalNormalWeaponData[41].firing_speed = 0.050000000745058f;
    OriginalNormalWeaponData[41].spread = 0.0099999997764826f;
    OriginalNormalWeaponData[41].maximum_clip_ammo = 500;
    OriginalNormalWeaponData[41].move_speed = 1.0f;
    OriginalNormalWeaponData[41].flags = 1073;
    OriginalNormalWeaponData[41].anim_group = 31;
    OriginalNormalWeaponData[41].fire_type = (eFireType) 3;
    OriginalNormalWeaponData[41].model = 365;
    OriginalNormalWeaponData[41].model2 = -1;
    OriginalNormalWeaponData[41].weapon_slot = (eWeaponSlot) 9;
    OriginalNormalWeaponData[41].fire_offset = CVector ( 0.20000000298023f, 0.0f, -0.10000000149012f );
    OriginalNormalWeaponData[41].skill_level = (eWeaponSkill) 1;
    OriginalNormalWeaponData[41].required_skill_level = 0;
    OriginalNormalWeaponData[41].anim_loop_start = 0.40000003576279f;
    OriginalNormalWeaponData[41].anim_loop_stop = 1.9140000343323f;
    OriginalNormalWeaponData[41].anim_loop_bullet_fire = 0.4333333671093f;
    OriginalNormalWeaponData[41].anim2_loop_start = 0.36666667461395f;
    OriginalNormalWeaponData[41].anim2_loop_stop = 1.9206665754318f;
    OriginalNormalWeaponData[41].anim2_loop_bullet_fire = 0.4333333671093f;
    OriginalNormalWeaponData[41].anim_breakout_time = 3.3000001907349f;
    OriginalNormalWeaponData[41].radius = 0.5f;
    OriginalNormalWeaponData[41].aim_offset = 20;
    OriginalNormalWeaponData[41].default_combo = 4;
    OriginalNormalWeaponData[41].combos_available = 1;
    // 42 - Fire Extinguisher
    OriginalNormalWeaponData[42].weapon_range = 10.10000038147f;
    OriginalNormalWeaponData[42].target_range = 10.0f;
    OriginalNormalWeaponData[42].accuracy = 1.0f;
    OriginalNormalWeaponData[42].damage = 1;
    OriginalNormalWeaponData[42].life_span = 1000.0f;
    OriginalNormalWeaponData[42].firing_speed = 0.10000000149012f;
    OriginalNormalWeaponData[42].spread = 0.0099999997764826f;
    OriginalNormalWeaponData[42].maximum_clip_ammo = 500;
    OriginalNormalWeaponData[42].move_speed = 1.0f;
    OriginalNormalWeaponData[42].flags = 56;
    OriginalNormalWeaponData[42].anim_group = 29;
    OriginalNormalWeaponData[42].fire_type = (eFireType) 3;
    OriginalNormalWeaponData[42].model = 366;
    OriginalNormalWeaponData[42].model2 = -1;
    OriginalNormalWeaponData[42].weapon_slot = (eWeaponSlot) 9;
    OriginalNormalWeaponData[42].fire_offset = CVector ( 0.44999998807907f, 0.10000000149012f, 0.15000000596046f );
    OriginalNormalWeaponData[42].skill_level = (eWeaponSkill) 1;
    OriginalNormalWeaponData[42].required_skill_level = 0;
    OriginalNormalWeaponData[42].anim_loop_start = 0.36666667461395f;
    OriginalNormalWeaponData[42].anim_loop_stop = 0.38066667318344f;
    OriginalNormalWeaponData[42].anim_loop_bullet_fire = 0.36666667461395f;
    OriginalNormalWeaponData[42].anim2_loop_start = 0.36666667461395f;
    OriginalNormalWeaponData[42].anim2_loop_stop = 0.38066667318344f;
    OriginalNormalWeaponData[42].anim2_loop_bullet_fire = 0.36666667461395f;
    OriginalNormalWeaponData[42].anim_breakout_time = 1.1666667461395f;
    OriginalNormalWeaponData[42].radius = 0.5f;
    OriginalNormalWeaponData[42].aim_offset = 18;
    OriginalNormalWeaponData[42].default_combo = 4;
    OriginalNormalWeaponData[42].combos_available = 1;

    // End of STD skill level weapons
    // Start of Skill Level Weapons
    // 23 - Silenced Stat: 0
    OriginalPoorWeaponData[23].weapon_range = 30.0f;
    OriginalPoorWeaponData[23].target_range = 25.0f;
    OriginalPoorWeaponData[23].accuracy = 1.0f;
    OriginalPoorWeaponData[23].damage = 40;
    OriginalPoorWeaponData[23].life_span = 0.0f;
    OriginalPoorWeaponData[23].firing_speed = 0.0f;
    OriginalPoorWeaponData[23].spread = 0.0f;
    OriginalPoorWeaponData[23].maximum_clip_ammo = 17;
    OriginalPoorWeaponData[23].move_speed = 1.0f;
    OriginalPoorWeaponData[23].flags = 28673;
    OriginalPoorWeaponData[23].anim_group = 18;
    OriginalPoorWeaponData[23].fire_type = (eFireType) 1;
    OriginalPoorWeaponData[23].model = 347;
    OriginalPoorWeaponData[23].model2 = -1;
    OriginalPoorWeaponData[23].weapon_slot = (eWeaponSlot) 2;
    OriginalPoorWeaponData[23].fire_offset = CVector ( 0.40000000596046f, 0.050000000745058f, 0.15000000596046f );
    OriginalPoorWeaponData[23].skill_level = (eWeaponSkill) 0;
    OriginalPoorWeaponData[23].required_skill_level = 0;
    OriginalPoorWeaponData[23].anim_loop_start = 0.20000001788139f;
    OriginalPoorWeaponData[23].anim_loop_stop = 0.65399998426437f;
    OriginalPoorWeaponData[23].anim_loop_bullet_fire = 0.20000001788139f;
    OriginalPoorWeaponData[23].anim2_loop_start = 0.20000001788139f;
    OriginalPoorWeaponData[23].anim2_loop_stop = 0.65399998426437f;
    OriginalPoorWeaponData[23].anim2_loop_bullet_fire = 0.20000001788139f;
    OriginalPoorWeaponData[23].anim_breakout_time = 1.3333333730698f;
    OriginalPoorWeaponData[23].radius = 0.0f;
    OriginalPoorWeaponData[23].aim_offset = 7;
    OriginalPoorWeaponData[23].default_combo = 4;
    OriginalPoorWeaponData[23].combos_available = 1;
    // 24 - Deagle Stat: 0
    OriginalPoorWeaponData[24].weapon_range = 30.0f;
    OriginalPoorWeaponData[24].target_range = 25.0f;
    OriginalPoorWeaponData[24].accuracy = 0.75f;
    OriginalPoorWeaponData[24].damage = 70;
    OriginalPoorWeaponData[24].life_span = 0.0f;
    OriginalPoorWeaponData[24].firing_speed = 0.0f;
    OriginalPoorWeaponData[24].spread = 0.0f;
    OriginalPoorWeaponData[24].maximum_clip_ammo = 7;
    OriginalPoorWeaponData[24].move_speed = 1.0f;
    OriginalPoorWeaponData[24].flags = 28673;
    OriginalPoorWeaponData[24].anim_group = 12;
    OriginalPoorWeaponData[24].fire_type = (eFireType) 1;
    OriginalPoorWeaponData[24].model = 348;
    OriginalPoorWeaponData[24].model2 = -1;
    OriginalPoorWeaponData[24].weapon_slot = (eWeaponSlot) 2;
    OriginalPoorWeaponData[24].fire_offset = CVector ( 0.40999999642372f, 0.029999999329448f, 0.11999999731779f );
    OriginalPoorWeaponData[24].skill_level = (eWeaponSkill) 0;
    OriginalPoorWeaponData[24].required_skill_level = 0;
    OriginalPoorWeaponData[24].anim_loop_start = 0.23333334922791f;
    OriginalPoorWeaponData[24].anim_loop_stop = 1.1873333454132f;
    OriginalPoorWeaponData[24].anim_loop_bullet_fire = 0.26666668057442f;
    OriginalPoorWeaponData[24].anim2_loop_start = 0.20000001788139f;
    OriginalPoorWeaponData[24].anim2_loop_stop = 0.99399995803833f;
    OriginalPoorWeaponData[24].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalPoorWeaponData[24].anim_breakout_time = 1.3333333730698f;
    OriginalPoorWeaponData[24].radius = 0.0f;
    OriginalPoorWeaponData[24].aim_offset = 1;
    OriginalPoorWeaponData[24].default_combo = 4;
    OriginalPoorWeaponData[24].combos_available = 1;
    // 22 - Colt 45 Stat: 0
    OriginalPoorWeaponData[22].weapon_range = 30.0f;
    OriginalPoorWeaponData[22].target_range = 25.0f;
    OriginalPoorWeaponData[22].accuracy = 0.75f;
    OriginalPoorWeaponData[22].damage = 25;
    OriginalPoorWeaponData[22].life_span = 0.0f;
    OriginalPoorWeaponData[22].firing_speed = 0.0f;
    OriginalPoorWeaponData[22].spread = 0.0f;
    OriginalPoorWeaponData[22].maximum_clip_ammo = 17;
    OriginalPoorWeaponData[22].move_speed = 1.0f;
    OriginalPoorWeaponData[22].flags = 12339;
    OriginalPoorWeaponData[22].anim_group = 13;
    OriginalPoorWeaponData[22].fire_type = (eFireType) 1;
    OriginalPoorWeaponData[22].model = 346;
    OriginalPoorWeaponData[22].model2 = -1;
    OriginalPoorWeaponData[22].weapon_slot = (eWeaponSlot) 2;
    OriginalPoorWeaponData[22].fire_offset = CVector ( 0.25f, 0.050000000745058f, 0.090000003576279f );
    OriginalPoorWeaponData[22].skill_level = (eWeaponSkill) 0;
    OriginalPoorWeaponData[22].required_skill_level = 0;
    OriginalPoorWeaponData[22].anim_loop_start = 0.20000001788139f;
    OriginalPoorWeaponData[22].anim_loop_stop = 0.55400002002716f;
    OriginalPoorWeaponData[22].anim_loop_bullet_fire = 0.20000001788139f;
    OriginalPoorWeaponData[22].anim2_loop_start = 0.20000001788139f;
    OriginalPoorWeaponData[22].anim2_loop_stop = 0.51399999856949f;
    OriginalPoorWeaponData[22].anim2_loop_bullet_fire = 0.20000001788139f;
    OriginalPoorWeaponData[22].anim_breakout_time = 3.3000001907349f;
    OriginalPoorWeaponData[22].radius = 0.0f;
    OriginalPoorWeaponData[22].aim_offset = 2;
    OriginalPoorWeaponData[22].default_combo = 4;
    OriginalPoorWeaponData[22].combos_available = 1;
    // 23 - Silenced Stat: 0
    OriginalPoorWeaponData[23].weapon_range = 30.0f;
    OriginalPoorWeaponData[23].target_range = 25.0f;
    OriginalPoorWeaponData[23].accuracy = 1.0f;
    OriginalPoorWeaponData[23].damage = 40;
    OriginalPoorWeaponData[23].life_span = 0.0f;
    OriginalPoorWeaponData[23].firing_speed = 0.0f;
    OriginalPoorWeaponData[23].spread = 0.0f;
    OriginalPoorWeaponData[23].maximum_clip_ammo = 17;
    OriginalPoorWeaponData[23].move_speed = 1.0f;
    OriginalPoorWeaponData[23].flags = 28673;
    OriginalPoorWeaponData[23].anim_group = 18;
    OriginalPoorWeaponData[23].fire_type = (eFireType) 1;
    OriginalPoorWeaponData[23].model = 347;
    OriginalPoorWeaponData[23].model2 = -1;
    OriginalPoorWeaponData[23].weapon_slot = (eWeaponSlot) 2;
    OriginalPoorWeaponData[23].fire_offset = CVector ( 0.40000000596046f, 0.050000000745058f, 0.15000000596046f );
    OriginalPoorWeaponData[23].skill_level = (eWeaponSkill) 0;
    OriginalPoorWeaponData[23].required_skill_level = 0;
    OriginalPoorWeaponData[23].anim_loop_start = 0.20000001788139f;
    OriginalPoorWeaponData[23].anim_loop_stop = 0.65399998426437f;
    OriginalPoorWeaponData[23].anim_loop_bullet_fire = 0.20000001788139f;
    OriginalPoorWeaponData[23].anim2_loop_start = 0.20000001788139f;
    OriginalPoorWeaponData[23].anim2_loop_stop = 0.65399998426437f;
    OriginalPoorWeaponData[23].anim2_loop_bullet_fire = 0.20000001788139f;
    OriginalPoorWeaponData[23].anim_breakout_time = 1.3333333730698f;
    OriginalPoorWeaponData[23].radius = 0.0f;
    OriginalPoorWeaponData[23].aim_offset = 7;
    OriginalPoorWeaponData[23].default_combo = 4;
    OriginalPoorWeaponData[23].combos_available = 1;
    // 24 - Deagle Stat: 0
    OriginalPoorWeaponData[24].weapon_range = 30.0f;
    OriginalPoorWeaponData[24].target_range = 25.0f;
    OriginalPoorWeaponData[24].accuracy = 0.75f;
    OriginalPoorWeaponData[24].damage = 70;
    OriginalPoorWeaponData[24].life_span = 0.0f;
    OriginalPoorWeaponData[24].firing_speed = 0.0f;
    OriginalPoorWeaponData[24].spread = 0.0f;
    OriginalPoorWeaponData[24].maximum_clip_ammo = 7;
    OriginalPoorWeaponData[24].move_speed = 1.0f;
    OriginalPoorWeaponData[24].flags = 28673;
    OriginalPoorWeaponData[24].anim_group = 12;
    OriginalPoorWeaponData[24].fire_type = (eFireType) 1;
    OriginalPoorWeaponData[24].model = 348;
    OriginalPoorWeaponData[24].model2 = -1;
    OriginalPoorWeaponData[24].weapon_slot = (eWeaponSlot) 2;
    OriginalPoorWeaponData[24].fire_offset = CVector ( 0.40999999642372f, 0.029999999329448f, 0.11999999731779f );
    OriginalPoorWeaponData[24].skill_level = (eWeaponSkill) 0;
    OriginalPoorWeaponData[24].required_skill_level = 0;
    OriginalPoorWeaponData[24].anim_loop_start = 0.23333334922791f;
    OriginalPoorWeaponData[24].anim_loop_stop = 1.1873333454132f;
    OriginalPoorWeaponData[24].anim_loop_bullet_fire = 0.26666668057442f;
    OriginalPoorWeaponData[24].anim2_loop_start = 0.20000001788139f;
    OriginalPoorWeaponData[24].anim2_loop_stop = 0.99399995803833f;
    OriginalPoorWeaponData[24].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalPoorWeaponData[24].anim_breakout_time = 1.3333333730698f;
    OriginalPoorWeaponData[24].radius = 0.0f;
    OriginalPoorWeaponData[24].aim_offset = 1;
    OriginalPoorWeaponData[24].default_combo = 4;
    OriginalPoorWeaponData[24].combos_available = 1;
    // 25 - Shotgun Stat: 0
    OriginalPoorWeaponData[25].weapon_range = 40.0f;
    OriginalPoorWeaponData[25].target_range = 30.0f;
    OriginalPoorWeaponData[25].accuracy = 1.0f;
    OriginalPoorWeaponData[25].damage = 10;
    OriginalPoorWeaponData[25].life_span = 0.0f;
    OriginalPoorWeaponData[25].firing_speed = 0.0f;
    OriginalPoorWeaponData[25].spread = 0.0f;
    OriginalPoorWeaponData[25].maximum_clip_ammo = 1;
    OriginalPoorWeaponData[25].move_speed = 1.0f;
    OriginalPoorWeaponData[25].flags = 8193;
    OriginalPoorWeaponData[25].anim_group = 20;
    OriginalPoorWeaponData[25].fire_type = (eFireType) 1;
    OriginalPoorWeaponData[25].model = 349;
    OriginalPoorWeaponData[25].model2 = -1;
    OriginalPoorWeaponData[25].weapon_slot = (eWeaponSlot) 3;
    OriginalPoorWeaponData[25].fire_offset = CVector ( 0.81999999284744f, -0.019999999552965f, 0.23000000417233f );
    OriginalPoorWeaponData[25].skill_level = (eWeaponSkill) 0;
    OriginalPoorWeaponData[25].required_skill_level = 0;
    OriginalPoorWeaponData[25].anim_loop_start = 0.20000001788139f;
    OriginalPoorWeaponData[25].anim_loop_stop = 1.3140000104904f;
    OriginalPoorWeaponData[25].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalPoorWeaponData[25].anim2_loop_start = 0.20000001788139f;
    OriginalPoorWeaponData[25].anim2_loop_stop = 1.3140000104904f;
    OriginalPoorWeaponData[25].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalPoorWeaponData[25].anim_breakout_time = 0.93333339691162f;
    OriginalPoorWeaponData[25].radius = 0.0f;
    OriginalPoorWeaponData[25].aim_offset = 9;
    OriginalPoorWeaponData[25].default_combo = 4;
    OriginalPoorWeaponData[25].combos_available = 1;
    // 26 - Sawed-off Stat: 0
    OriginalPoorWeaponData[26].weapon_range = 30.0f;
    OriginalPoorWeaponData[26].target_range = 25.0f;
    OriginalPoorWeaponData[26].accuracy = 0.69999998807907f;
    OriginalPoorWeaponData[26].damage = 10;
    OriginalPoorWeaponData[26].life_span = 0.0f;
    OriginalPoorWeaponData[26].firing_speed = 0.0f;
    OriginalPoorWeaponData[26].spread = 0.0f;
    OriginalPoorWeaponData[26].maximum_clip_ammo = 2;
    OriginalPoorWeaponData[26].move_speed = 1.0f;
    OriginalPoorWeaponData[26].flags = 12339;
    OriginalPoorWeaponData[26].anim_group = 16;
    OriginalPoorWeaponData[26].fire_type = (eFireType) 1;
    OriginalPoorWeaponData[26].model = 350;
    OriginalPoorWeaponData[26].model2 = -1;
    OriginalPoorWeaponData[26].weapon_slot = (eWeaponSlot) 3;
    OriginalPoorWeaponData[26].fire_offset = CVector ( 0.56000000238419f, 0.0f, 0.15000000596046f );
    OriginalPoorWeaponData[26].skill_level = (eWeaponSkill) 0;
    OriginalPoorWeaponData[26].required_skill_level = 0;
    OriginalPoorWeaponData[26].anim_loop_start = 0.20000001788139f;
    OriginalPoorWeaponData[26].anim_loop_stop = 0.49399998784065f;
    OriginalPoorWeaponData[26].anim_loop_bullet_fire = 0.20000001788139f;
    OriginalPoorWeaponData[26].anim2_loop_start = 0.20000001788139f;
    OriginalPoorWeaponData[26].anim2_loop_stop = 0.49399998784065f;
    OriginalPoorWeaponData[26].anim2_loop_bullet_fire = 0.20000001788139f;
    OriginalPoorWeaponData[26].anim_breakout_time = 1.0f;
    OriginalPoorWeaponData[26].radius = 0.0f;
    OriginalPoorWeaponData[26].aim_offset = 5;
    OriginalPoorWeaponData[26].default_combo = 4;
    OriginalPoorWeaponData[26].combos_available = 1;
    // 27 - Combat Shotgun Stat: 0
    OriginalPoorWeaponData[27].weapon_range = 40.0f;
    OriginalPoorWeaponData[27].target_range = 35.0f;
    OriginalPoorWeaponData[27].accuracy = 1.3999999761581f;
    OriginalPoorWeaponData[27].damage = 15;
    OriginalPoorWeaponData[27].life_span = 0.0f;
    OriginalPoorWeaponData[27].firing_speed = 0.0f;
    OriginalPoorWeaponData[27].spread = 0.0f;
    OriginalPoorWeaponData[27].maximum_clip_ammo = 7;
    OriginalPoorWeaponData[27].move_speed = 1.0f;
    OriginalPoorWeaponData[27].flags = 28673;
    OriginalPoorWeaponData[27].anim_group = 21;
    OriginalPoorWeaponData[27].fire_type = (eFireType) 1;
    OriginalPoorWeaponData[27].model = 351;
    OriginalPoorWeaponData[27].model2 = -1;
    OriginalPoorWeaponData[27].weapon_slot = (eWeaponSlot) 3;
    OriginalPoorWeaponData[27].fire_offset = CVector ( 0.75f, -0.059999998658895f, 0.21999999880791f );
    OriginalPoorWeaponData[27].skill_level = (eWeaponSkill) 0;
    OriginalPoorWeaponData[27].required_skill_level = 0;
    OriginalPoorWeaponData[27].anim_loop_start = 0.20000001788139f;
    OriginalPoorWeaponData[27].anim_loop_stop = 0.59399998188019f;
    OriginalPoorWeaponData[27].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalPoorWeaponData[27].anim2_loop_start = 0.20000001788139f;
    OriginalPoorWeaponData[27].anim2_loop_stop = 0.59399998188019f;
    OriginalPoorWeaponData[27].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalPoorWeaponData[27].anim_breakout_time = 1.1666667461395f;
    OriginalPoorWeaponData[27].radius = 0.0f;
    OriginalPoorWeaponData[27].aim_offset = 10;
    OriginalPoorWeaponData[27].default_combo = 4;
    OriginalPoorWeaponData[27].combos_available = 1;
    // 28 - Uzi Stat: 0
    OriginalPoorWeaponData[28].weapon_range = 30.0f;
    OriginalPoorWeaponData[28].target_range = 25.0f;
    OriginalPoorWeaponData[28].accuracy = 0.75f;
    OriginalPoorWeaponData[28].damage = 20;
    OriginalPoorWeaponData[28].life_span = 0.0f;
    OriginalPoorWeaponData[28].firing_speed = 0.0f;
    OriginalPoorWeaponData[28].spread = 0.0f;
    OriginalPoorWeaponData[28].maximum_clip_ammo = 50;
    OriginalPoorWeaponData[28].move_speed = 1.0f;
    OriginalPoorWeaponData[28].flags = 12307;
    OriginalPoorWeaponData[28].anim_group = 13;
    OriginalPoorWeaponData[28].fire_type = (eFireType) 1;
    OriginalPoorWeaponData[28].model = 352;
    OriginalPoorWeaponData[28].model2 = -1;
    OriginalPoorWeaponData[28].weapon_slot = (eWeaponSlot) 4;
    OriginalPoorWeaponData[28].fire_offset = CVector ( 0.25f, 0.0f, 0.10999999940395f );
    OriginalPoorWeaponData[28].skill_level = (eWeaponSkill) 0;
    OriginalPoorWeaponData[28].required_skill_level = 0;
    OriginalPoorWeaponData[28].anim_loop_start = 0.20000001788139f;
    OriginalPoorWeaponData[28].anim_loop_stop = 0.31400001049042f;
    OriginalPoorWeaponData[28].anim_loop_bullet_fire = 0.20000001788139f;
    OriginalPoorWeaponData[28].anim2_loop_start = 0.20000001788139f;
    OriginalPoorWeaponData[28].anim2_loop_stop = 0.31400001049042f;
    OriginalPoorWeaponData[28].anim2_loop_bullet_fire = 0.20000001788139f;
    OriginalPoorWeaponData[28].anim_breakout_time = 3.3000001907349f;
    OriginalPoorWeaponData[28].radius = 0.0f;
    OriginalPoorWeaponData[28].aim_offset = 2;
    OriginalPoorWeaponData[28].default_combo = 4;
    OriginalPoorWeaponData[28].combos_available = 1;
    // 29 - MP5 Stat: 0
    OriginalPoorWeaponData[29].weapon_range = 40.0f;
    OriginalPoorWeaponData[29].target_range = 35.0f;
    OriginalPoorWeaponData[29].accuracy = 0.75f;
    OriginalPoorWeaponData[29].damage = 25;
    OriginalPoorWeaponData[29].life_span = 0.0f;
    OriginalPoorWeaponData[29].firing_speed = 0.0f;
    OriginalPoorWeaponData[29].spread = 0.0f;
    OriginalPoorWeaponData[29].maximum_clip_ammo = 30;
    OriginalPoorWeaponData[29].move_speed = 1.1000000238419f;
    OriginalPoorWeaponData[29].flags = 28673;
    OriginalPoorWeaponData[29].anim_group = 24;
    OriginalPoorWeaponData[29].fire_type = (eFireType) 1;
    OriginalPoorWeaponData[29].model = 353;
    OriginalPoorWeaponData[29].model2 = -1;
    OriginalPoorWeaponData[29].weapon_slot = (eWeaponSlot) 4;
    OriginalPoorWeaponData[29].fire_offset = CVector ( 0.50999999046326f, -0.0099999997764826f, 0.20000000298023f );
    OriginalPoorWeaponData[29].skill_level = (eWeaponSkill) 0;
    OriginalPoorWeaponData[29].required_skill_level = 0;
    OriginalPoorWeaponData[29].anim_loop_start = 0.20000001788139f;
    OriginalPoorWeaponData[29].anim_loop_stop = 0.2940000295639f;
    OriginalPoorWeaponData[29].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalPoorWeaponData[29].anim2_loop_start = 0.20000001788139f;
    OriginalPoorWeaponData[29].anim2_loop_stop = 0.2940000295639f;
    OriginalPoorWeaponData[29].anim2_loop_bullet_fire = 0.20000001788139f;
    OriginalPoorWeaponData[29].anim_breakout_time = 1.0f;
    OriginalPoorWeaponData[29].radius = 0.0f;
    OriginalPoorWeaponData[29].aim_offset = 13;
    OriginalPoorWeaponData[29].default_combo = 4;
    OriginalPoorWeaponData[29].combos_available = 1;
    // 30 - AK-47 Stat: 0
    OriginalPoorWeaponData[30].weapon_range = 70.0f;
    OriginalPoorWeaponData[30].target_range = 40.0f;
    OriginalPoorWeaponData[30].accuracy = 0.40000000596046f;
    OriginalPoorWeaponData[30].damage = 30;
    OriginalPoorWeaponData[30].life_span = 0.0f;
    OriginalPoorWeaponData[30].firing_speed = 0.0f;
    OriginalPoorWeaponData[30].spread = 0.0f;
    OriginalPoorWeaponData[30].maximum_clip_ammo = 30;
    OriginalPoorWeaponData[30].move_speed = 0.89999997615814f;
    OriginalPoorWeaponData[30].flags = 28673;
    OriginalPoorWeaponData[30].anim_group = 26;
    OriginalPoorWeaponData[30].fire_type = (eFireType) 1;
    OriginalPoorWeaponData[30].model = 355;
    OriginalPoorWeaponData[30].model2 = -1;
    OriginalPoorWeaponData[30].weapon_slot = (eWeaponSlot) 5;
    OriginalPoorWeaponData[30].fire_offset = CVector ( 0.77999997138977f, -0.059999998658895f, 0.12999999523163f );
    OriginalPoorWeaponData[30].skill_level = (eWeaponSkill) 0;
    OriginalPoorWeaponData[30].required_skill_level = 0;
    OriginalPoorWeaponData[30].anim_loop_start = 0.20000001788139f;
    OriginalPoorWeaponData[30].anim_loop_stop = 0.31400001049042f;
    OriginalPoorWeaponData[30].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalPoorWeaponData[30].anim2_loop_start = 0.20000001788139f;
    OriginalPoorWeaponData[30].anim2_loop_stop = 0.31400001049042f;
    OriginalPoorWeaponData[30].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalPoorWeaponData[30].anim_breakout_time = 3.3000001907349f;
    OriginalPoorWeaponData[30].radius = 0.0f;
    OriginalPoorWeaponData[30].aim_offset = 15;
    OriginalPoorWeaponData[30].default_combo = 4;
    OriginalPoorWeaponData[30].combos_available = 1;
    // 31 - M4 Stat: 0
    OriginalPoorWeaponData[31].weapon_range = 90.0f;
    OriginalPoorWeaponData[31].target_range = 40.0f;
    OriginalPoorWeaponData[31].accuracy = 0.44999998807907f;
    OriginalPoorWeaponData[31].damage = 30;
    OriginalPoorWeaponData[31].life_span = 0.0f;
    OriginalPoorWeaponData[31].firing_speed = 0.0f;
    OriginalPoorWeaponData[31].spread = 0.0f;
    OriginalPoorWeaponData[31].maximum_clip_ammo = 50;
    OriginalPoorWeaponData[31].move_speed = 0.89999997615814f;
    OriginalPoorWeaponData[31].flags = 28673;
    OriginalPoorWeaponData[31].anim_group = 26;
    OriginalPoorWeaponData[31].fire_type = (eFireType) 1;
    OriginalPoorWeaponData[31].model = 356;
    OriginalPoorWeaponData[31].model2 = -1;
    OriginalPoorWeaponData[31].weapon_slot = (eWeaponSlot) 5;
    OriginalPoorWeaponData[31].fire_offset = CVector ( 0.74000000953674f, -0.03999999910593f, 0.12999999523163f );
    OriginalPoorWeaponData[31].skill_level = (eWeaponSkill) 0;
    OriginalPoorWeaponData[31].required_skill_level = 0;
    OriginalPoorWeaponData[31].anim_loop_start = 0.20000001788139f;
    OriginalPoorWeaponData[31].anim_loop_stop = 0.31400001049042f;
    OriginalPoorWeaponData[31].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalPoorWeaponData[31].anim2_loop_start = 0.20000001788139f;
    OriginalPoorWeaponData[31].anim2_loop_stop = 0.31400001049042f;
    OriginalPoorWeaponData[31].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalPoorWeaponData[31].anim_breakout_time = 3.3000001907349f;
    OriginalPoorWeaponData[31].radius = 0.0f;
    OriginalPoorWeaponData[31].aim_offset = 15;
    OriginalPoorWeaponData[31].default_combo = 4;
    OriginalPoorWeaponData[31].combos_available = 1;
    // 32 - Tec-9 Stat: 0
    OriginalPoorWeaponData[32].weapon_range = 30.0f;
    OriginalPoorWeaponData[32].target_range = 25.0f;
    OriginalPoorWeaponData[32].accuracy = 0.75f;
    OriginalPoorWeaponData[32].damage = 20;
    OriginalPoorWeaponData[32].life_span = 0.0f;
    OriginalPoorWeaponData[32].firing_speed = 0.0f;
    OriginalPoorWeaponData[32].spread = 0.0f;
    OriginalPoorWeaponData[32].maximum_clip_ammo = 50;
    OriginalPoorWeaponData[32].move_speed = 1.0f;
    OriginalPoorWeaponData[32].flags = 12307;
    OriginalPoorWeaponData[32].anim_group = 13;
    OriginalPoorWeaponData[32].fire_type = (eFireType) 1;
    OriginalPoorWeaponData[32].model = 372;
    OriginalPoorWeaponData[32].model2 = -1;
    OriginalPoorWeaponData[32].weapon_slot = (eWeaponSlot) 4;
    OriginalPoorWeaponData[32].fire_offset = CVector ( 0.44999998807907f, -0.050000000745058f, 0.10999999940395f );
    OriginalPoorWeaponData[32].skill_level = (eWeaponSkill) 0;
    OriginalPoorWeaponData[32].required_skill_level = 0;
    OriginalPoorWeaponData[32].anim_loop_start = 0.36666667461395f;
    OriginalPoorWeaponData[32].anim_loop_stop = 0.48066666722298f;
    OriginalPoorWeaponData[32].anim_loop_bullet_fire = 0.40000003576279f;
    OriginalPoorWeaponData[32].anim2_loop_start = 0.20000001788139f;
    OriginalPoorWeaponData[32].anim2_loop_stop = 0.35400003194809f;
    OriginalPoorWeaponData[32].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalPoorWeaponData[32].anim_breakout_time = 3.3000001907349f;
    OriginalPoorWeaponData[32].radius = 0.0f;
    OriginalPoorWeaponData[32].aim_offset = 2;
    OriginalPoorWeaponData[32].default_combo = 4;
    OriginalPoorWeaponData[32].combos_available = 1;
    // 22 - Colt 45 Stat: 2
    OriginalHitmanWeaponData[22].weapon_range = 35.0f;
    OriginalHitmanWeaponData[22].target_range = 35.0f;
    OriginalHitmanWeaponData[22].accuracy = 1.25f;
    OriginalHitmanWeaponData[22].damage = 25;
    OriginalHitmanWeaponData[22].life_span = 0.0f;
    OriginalHitmanWeaponData[22].firing_speed = 0.0f;
    OriginalHitmanWeaponData[22].spread = 0.0f;
    OriginalHitmanWeaponData[22].maximum_clip_ammo = 34;
    OriginalHitmanWeaponData[22].move_speed = 1.0f;
    OriginalHitmanWeaponData[22].flags = 14387;
    OriginalHitmanWeaponData[22].anim_group = 15;
    OriginalHitmanWeaponData[22].fire_type = (eFireType) 1;
    OriginalHitmanWeaponData[22].model = 346;
    OriginalHitmanWeaponData[22].model2 = -1;
    OriginalHitmanWeaponData[22].weapon_slot = (eWeaponSlot) 2;
    OriginalHitmanWeaponData[22].fire_offset = CVector ( 0.25f, 0.050000000745058f, 0.090000003576279f );
    OriginalHitmanWeaponData[22].skill_level = (eWeaponSkill) 2;
    OriginalHitmanWeaponData[22].required_skill_level = 999;
    OriginalHitmanWeaponData[22].anim_loop_start = 0.20000001788139f;
    OriginalHitmanWeaponData[22].anim_loop_stop = 0.59399998188019f;
    OriginalHitmanWeaponData[22].anim_loop_bullet_fire = 0.20000001788139f;
    OriginalHitmanWeaponData[22].anim2_loop_start = 0.20000001788139f;
    OriginalHitmanWeaponData[22].anim2_loop_stop = 0.55400002002716f;
    OriginalHitmanWeaponData[22].anim2_loop_bullet_fire = 0.20000001788139f;
    OriginalHitmanWeaponData[22].anim_breakout_time = 3.3000001907349f;
    OriginalHitmanWeaponData[22].radius = 0.0f;
    OriginalHitmanWeaponData[22].aim_offset = 4;
    OriginalHitmanWeaponData[22].default_combo = 4;
    OriginalHitmanWeaponData[22].combos_available = 1;
    // 23 - Silenced Stat: 2
    OriginalHitmanWeaponData[23].weapon_range = 35.0f;
    OriginalHitmanWeaponData[23].target_range = 35.0f;
    OriginalHitmanWeaponData[23].accuracy = 1.5f;
    OriginalHitmanWeaponData[23].damage = 40;
    OriginalHitmanWeaponData[23].life_span = 0.0f;
    OriginalHitmanWeaponData[23].firing_speed = 0.0f;
    OriginalHitmanWeaponData[23].spread = 0.0f;
    OriginalHitmanWeaponData[23].maximum_clip_ammo = 17;
    OriginalHitmanWeaponData[23].move_speed = 1.5f;
    OriginalHitmanWeaponData[23].flags = 28721;
    OriginalHitmanWeaponData[23].anim_group = 18;
    OriginalHitmanWeaponData[23].fire_type = (eFireType) 1;
    OriginalHitmanWeaponData[23].model = 347;
    OriginalHitmanWeaponData[23].model2 = -1;
    OriginalHitmanWeaponData[23].weapon_slot = (eWeaponSlot) 2;
    OriginalHitmanWeaponData[23].fire_offset = CVector ( 0.40000000596046f, 0.050000000745058f, 0.15000000596046f );
    OriginalHitmanWeaponData[23].skill_level = (eWeaponSkill) 2;
    OriginalHitmanWeaponData[23].required_skill_level = 999;
    OriginalHitmanWeaponData[23].anim_loop_start = 0.20000001788139f;
    OriginalHitmanWeaponData[23].anim_loop_stop = 0.55400002002716f;
    OriginalHitmanWeaponData[23].anim_loop_bullet_fire = 0.20000001788139f;
    OriginalHitmanWeaponData[23].anim2_loop_start = 0.20000001788139f;
    OriginalHitmanWeaponData[23].anim2_loop_stop = 0.55400002002716f;
    OriginalHitmanWeaponData[23].anim2_loop_bullet_fire = 0.20000001788139f;
    OriginalHitmanWeaponData[23].anim_breakout_time = 1.3333333730698f;
    OriginalHitmanWeaponData[23].radius = 0.0f;
    OriginalHitmanWeaponData[23].aim_offset = 7;
    OriginalHitmanWeaponData[23].default_combo = 4;
    OriginalHitmanWeaponData[23].combos_available = 1;
    // 24 - Deagle Stat: 2
    OriginalHitmanWeaponData[24].weapon_range = 35.0f;
    OriginalHitmanWeaponData[24].target_range = 35.0f;
    OriginalHitmanWeaponData[24].accuracy = 1.25f;
    OriginalHitmanWeaponData[24].damage = 140;
    OriginalHitmanWeaponData[24].life_span = 0.0f;
    OriginalHitmanWeaponData[24].firing_speed = 0.0f;
    OriginalHitmanWeaponData[24].spread = 0.0f;
    OriginalHitmanWeaponData[24].maximum_clip_ammo = 7;
    OriginalHitmanWeaponData[24].move_speed = 1.5f;
    OriginalHitmanWeaponData[24].flags = 28721;
    OriginalHitmanWeaponData[24].anim_group = 11;
    OriginalHitmanWeaponData[24].fire_type = (eFireType) 1;
    OriginalHitmanWeaponData[24].model = 348;
    OriginalHitmanWeaponData[24].model2 = -1;
    OriginalHitmanWeaponData[24].weapon_slot = (eWeaponSlot) 2;
    OriginalHitmanWeaponData[24].fire_offset = CVector ( 0.40999999642372f, 0.029999999329448f, 0.11999999731779f );
    OriginalHitmanWeaponData[24].skill_level = (eWeaponSkill) 2;
    OriginalHitmanWeaponData[24].required_skill_level = 999;
    OriginalHitmanWeaponData[24].anim_loop_start = 0.20000001788139f;
    OriginalHitmanWeaponData[24].anim_loop_stop = 0.91400003433228f;
    OriginalHitmanWeaponData[24].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalHitmanWeaponData[24].anim2_loop_start = 0.20000001788139f;
    OriginalHitmanWeaponData[24].anim2_loop_stop = 0.91400003433228f;
    OriginalHitmanWeaponData[24].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalHitmanWeaponData[24].anim_breakout_time = 1.3333333730698f;
    OriginalHitmanWeaponData[24].radius = 0.0f;
    OriginalHitmanWeaponData[24].aim_offset = 0;
    OriginalHitmanWeaponData[24].default_combo = 4;
    OriginalHitmanWeaponData[24].combos_available = 1;
    // 25 - Shotgun Stat: 2
    OriginalHitmanWeaponData[25].weapon_range = 40.0f;
    OriginalHitmanWeaponData[25].target_range = 40.0f;
    OriginalHitmanWeaponData[25].accuracy = 1.3999999761581f;
    OriginalHitmanWeaponData[25].damage = 10;
    OriginalHitmanWeaponData[25].life_span = 0.0f;
    OriginalHitmanWeaponData[25].firing_speed = 0.0f;
    OriginalHitmanWeaponData[25].spread = 0.0f;
    OriginalHitmanWeaponData[25].maximum_clip_ammo = 1;
    OriginalHitmanWeaponData[25].move_speed = 1.2999999523163f;
    OriginalHitmanWeaponData[25].flags = 8241;
    OriginalHitmanWeaponData[25].anim_group = 19;
    OriginalHitmanWeaponData[25].fire_type = (eFireType) 1;
    OriginalHitmanWeaponData[25].model = 349;
    OriginalHitmanWeaponData[25].model2 = -1;
    OriginalHitmanWeaponData[25].weapon_slot = (eWeaponSlot) 3;
    OriginalHitmanWeaponData[25].fire_offset = CVector ( 0.81999999284744f, -0.019999999552965f, 0.25f );
    OriginalHitmanWeaponData[25].skill_level = (eWeaponSkill) 2;
    OriginalHitmanWeaponData[25].required_skill_level = 999;
    OriginalHitmanWeaponData[25].anim_loop_start = 0.20000001788139f;
    OriginalHitmanWeaponData[25].anim_loop_stop = 1.2539999485016f;
    OriginalHitmanWeaponData[25].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalHitmanWeaponData[25].anim2_loop_start = 0.20000001788139f;
    OriginalHitmanWeaponData[25].anim2_loop_stop = 1.3140000104904f;
    OriginalHitmanWeaponData[25].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalHitmanWeaponData[25].anim_breakout_time = 0.83333337306976f;
    OriginalHitmanWeaponData[25].radius = 0.0f;
    OriginalHitmanWeaponData[25].aim_offset = 8;
    OriginalHitmanWeaponData[25].default_combo = 4;
    OriginalHitmanWeaponData[25].combos_available = 1;
    // 26 - Sawed-off Stat: 2
    OriginalHitmanWeaponData[26].weapon_range = 35.0f;
    OriginalHitmanWeaponData[26].target_range = 35.0f;
    OriginalHitmanWeaponData[26].accuracy = 0.89999997615814f;
    OriginalHitmanWeaponData[26].damage = 10;
    OriginalHitmanWeaponData[26].life_span = 0.0f;
    OriginalHitmanWeaponData[26].firing_speed = 0.0f;
    OriginalHitmanWeaponData[26].spread = 0.0f;
    OriginalHitmanWeaponData[26].maximum_clip_ammo = 4;
    OriginalHitmanWeaponData[26].move_speed = 1.2999999523163f;
    OriginalHitmanWeaponData[26].flags = 14387;
    OriginalHitmanWeaponData[26].anim_group = 17;
    OriginalHitmanWeaponData[26].fire_type = (eFireType) 1;
    OriginalHitmanWeaponData[26].model = 350;
    OriginalHitmanWeaponData[26].model2 = -1;
    OriginalHitmanWeaponData[26].weapon_slot = (eWeaponSlot) 3;
    OriginalHitmanWeaponData[26].fire_offset = CVector ( 0.56000000238419f, 0.0f, 0.15000000596046f );
    OriginalHitmanWeaponData[26].skill_level = (eWeaponSkill) 2;
    OriginalHitmanWeaponData[26].required_skill_level = 999;
    OriginalHitmanWeaponData[26].anim_loop_start = 0.20000001788139f;
    OriginalHitmanWeaponData[26].anim_loop_stop = 0.49399998784065f;
    OriginalHitmanWeaponData[26].anim_loop_bullet_fire = 0.20000001788139f;
    OriginalHitmanWeaponData[26].anim2_loop_start = 0.20000001788139f;
    OriginalHitmanWeaponData[26].anim2_loop_stop = 0.49399998784065f;
    OriginalHitmanWeaponData[26].anim2_loop_bullet_fire = 0.20000001788139f;
    OriginalHitmanWeaponData[26].anim_breakout_time = 1.0f;
    OriginalHitmanWeaponData[26].radius = 0.0f;
    OriginalHitmanWeaponData[26].aim_offset = 6;
    OriginalHitmanWeaponData[26].default_combo = 4;
    OriginalHitmanWeaponData[26].combos_available = 1;
    // 27 - Combat Shotgun Stat: 2
    OriginalHitmanWeaponData[27].weapon_range = 40.0f;
    OriginalHitmanWeaponData[27].target_range = 40.0f;
    OriginalHitmanWeaponData[27].accuracy = 2.0f;
    OriginalHitmanWeaponData[27].damage = 15;
    OriginalHitmanWeaponData[27].life_span = 0.0f;
    OriginalHitmanWeaponData[27].firing_speed = 0.0f;
    OriginalHitmanWeaponData[27].spread = 0.0f;
    OriginalHitmanWeaponData[27].maximum_clip_ammo = 7;
    OriginalHitmanWeaponData[27].move_speed = 1.2000000476837f;
    OriginalHitmanWeaponData[27].flags = 28721;
    OriginalHitmanWeaponData[27].anim_group = 21;
    OriginalHitmanWeaponData[27].fire_type = (eFireType) 1;
    OriginalHitmanWeaponData[27].model = 351;
    OriginalHitmanWeaponData[27].model2 = -1;
    OriginalHitmanWeaponData[27].weapon_slot = (eWeaponSlot) 3;
    OriginalHitmanWeaponData[27].fire_offset = CVector ( 0.75f, -0.059999998658895f, 0.21999999880791f );
    OriginalHitmanWeaponData[27].skill_level = (eWeaponSkill) 2;
    OriginalHitmanWeaponData[27].required_skill_level = 999;
    OriginalHitmanWeaponData[27].anim_loop_start = 0.20000001788139f;
    OriginalHitmanWeaponData[27].anim_loop_stop = 0.51399999856949f;
    OriginalHitmanWeaponData[27].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalHitmanWeaponData[27].anim2_loop_start = 0.20000001788139f;
    OriginalHitmanWeaponData[27].anim2_loop_stop = 0.51399999856949f;
    OriginalHitmanWeaponData[27].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalHitmanWeaponData[27].anim_breakout_time = 1.1666667461395f;
    OriginalHitmanWeaponData[27].radius = 0.0f;
    OriginalHitmanWeaponData[27].aim_offset = 10;
    OriginalHitmanWeaponData[27].default_combo = 4;
    OriginalHitmanWeaponData[27].combos_available = 1;
    // 28 - Uzi Stat: 2
    OriginalHitmanWeaponData[28].weapon_range = 35.0f;
    OriginalHitmanWeaponData[28].target_range = 35.0f;
    OriginalHitmanWeaponData[28].accuracy = 1.1000000238419f;
    OriginalHitmanWeaponData[28].damage = 20;
    OriginalHitmanWeaponData[28].life_span = 0.0f;
    OriginalHitmanWeaponData[28].firing_speed = 0.0f;
    OriginalHitmanWeaponData[28].spread = 0.0f;
    OriginalHitmanWeaponData[28].maximum_clip_ammo = 100;
    OriginalHitmanWeaponData[28].move_speed = 1.0f;
    OriginalHitmanWeaponData[28].flags = 14387;
    OriginalHitmanWeaponData[28].anim_group = 15;
    OriginalHitmanWeaponData[28].fire_type = (eFireType) 1;
    OriginalHitmanWeaponData[28].model = 352;
    OriginalHitmanWeaponData[28].model2 = -1;
    OriginalHitmanWeaponData[28].weapon_slot = (eWeaponSlot) 4;
    OriginalHitmanWeaponData[28].fire_offset = CVector ( 0.25f, 0.0f, 0.10999999940395f );
    OriginalHitmanWeaponData[28].skill_level = (eWeaponSkill) 2;
    OriginalHitmanWeaponData[28].required_skill_level = 999;
    OriginalHitmanWeaponData[28].anim_loop_start = 0.20000001788139f;
    OriginalHitmanWeaponData[28].anim_loop_stop = 0.31400001049042f;
    OriginalHitmanWeaponData[28].anim_loop_bullet_fire = 0.20000001788139f;
    OriginalHitmanWeaponData[28].anim2_loop_start = 0.20000001788139f;
    OriginalHitmanWeaponData[28].anim2_loop_stop = 0.31400001049042f;
    OriginalHitmanWeaponData[28].anim2_loop_bullet_fire = 0.20000001788139f;
    OriginalHitmanWeaponData[28].anim_breakout_time = 3.3000001907349f;
    OriginalHitmanWeaponData[28].radius = 0.0f;
    OriginalHitmanWeaponData[28].aim_offset = 4;
    OriginalHitmanWeaponData[28].default_combo = 4;
    OriginalHitmanWeaponData[28].combos_available = 1;
    // 29 - MP5 Stat: 2
    OriginalHitmanWeaponData[29].weapon_range = 45.0f;
    OriginalHitmanWeaponData[29].target_range = 45.0f;
    OriginalHitmanWeaponData[29].accuracy = 1.2000000476837f;
    OriginalHitmanWeaponData[29].damage = 25;
    OriginalHitmanWeaponData[29].life_span = 0.0f;
    OriginalHitmanWeaponData[29].firing_speed = 0.0f;
    OriginalHitmanWeaponData[29].spread = 0.0f;
    OriginalHitmanWeaponData[29].maximum_clip_ammo = 30;
    OriginalHitmanWeaponData[29].move_speed = 1.5f;
    OriginalHitmanWeaponData[29].flags = 28721;
    OriginalHitmanWeaponData[29].anim_group = 23;
    OriginalHitmanWeaponData[29].fire_type = (eFireType) 1;
    OriginalHitmanWeaponData[29].model = 353;
    OriginalHitmanWeaponData[29].model2 = -1;
    OriginalHitmanWeaponData[29].weapon_slot = (eWeaponSlot) 4;
    OriginalHitmanWeaponData[29].fire_offset = CVector ( 0.50999999046326f, -0.0099999997764826f, 0.20000000298023f );
    OriginalHitmanWeaponData[29].skill_level = (eWeaponSkill) 2;
    OriginalHitmanWeaponData[29].required_skill_level = 999;
    OriginalHitmanWeaponData[29].anim_loop_start = 0.20000001788139f;
    OriginalHitmanWeaponData[29].anim_loop_stop = 0.2940000295639f;
    OriginalHitmanWeaponData[29].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalHitmanWeaponData[29].anim2_loop_start = 0.20000001788139f;
    OriginalHitmanWeaponData[29].anim2_loop_stop = 0.2940000295639f;
    OriginalHitmanWeaponData[29].anim2_loop_bullet_fire = 0.20000001788139f;
    OriginalHitmanWeaponData[29].anim_breakout_time = 1.0f;
    OriginalHitmanWeaponData[29].radius = 0.0f;
    OriginalHitmanWeaponData[29].aim_offset = 12;
    OriginalHitmanWeaponData[29].default_combo = 4;
    OriginalHitmanWeaponData[29].combos_available = 1;
    // 30 - AK-47 Stat: 2
    OriginalHitmanWeaponData[30].weapon_range = 70.0f;
    OriginalHitmanWeaponData[30].target_range = 50.0f;
    OriginalHitmanWeaponData[30].accuracy = 0.60000002384186f;
    OriginalHitmanWeaponData[30].damage = 30;
    OriginalHitmanWeaponData[30].life_span = 0.0f;
    OriginalHitmanWeaponData[30].firing_speed = 0.0f;
    OriginalHitmanWeaponData[30].spread = 0.0f;
    OriginalHitmanWeaponData[30].maximum_clip_ammo = 30;
    OriginalHitmanWeaponData[30].move_speed = 1.1000000238419f;
    OriginalHitmanWeaponData[30].flags = 28721;
    OriginalHitmanWeaponData[30].anim_group = 25;
    OriginalHitmanWeaponData[30].fire_type = (eFireType) 1;
    OriginalHitmanWeaponData[30].model = 355;
    OriginalHitmanWeaponData[30].model2 = -1;
    OriginalHitmanWeaponData[30].weapon_slot = (eWeaponSlot) 5;
    OriginalHitmanWeaponData[30].fire_offset = CVector ( 0.77999997138977f, -0.059999998658895f, 0.12999999523163f );
    OriginalHitmanWeaponData[30].skill_level = (eWeaponSkill) 2;
    OriginalHitmanWeaponData[30].required_skill_level = 999;
    OriginalHitmanWeaponData[30].anim_loop_start = 0.20000001788139f;
    OriginalHitmanWeaponData[30].anim_loop_stop = 0.31400001049042f;
    OriginalHitmanWeaponData[30].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalHitmanWeaponData[30].anim2_loop_start = 0.20000001788139f;
    OriginalHitmanWeaponData[30].anim2_loop_stop = 0.31400001049042f;
    OriginalHitmanWeaponData[30].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalHitmanWeaponData[30].anim_breakout_time = 3.3000001907349f;
    OriginalHitmanWeaponData[30].radius = 0.0f;
    OriginalHitmanWeaponData[30].aim_offset = 14;
    OriginalHitmanWeaponData[30].default_combo = 4;
    OriginalHitmanWeaponData[30].combos_available = 1;
    // 31 - M4 Stat: 2
    OriginalHitmanWeaponData[31].weapon_range = 90.0f;
    OriginalHitmanWeaponData[31].target_range = 50.0f;
    OriginalHitmanWeaponData[31].accuracy = 0.80000001192093f;
    OriginalHitmanWeaponData[31].damage = 30;
    OriginalHitmanWeaponData[31].life_span = 0.0f;
    OriginalHitmanWeaponData[31].firing_speed = 0.0f;
    OriginalHitmanWeaponData[31].spread = 0.0f;
    OriginalHitmanWeaponData[31].maximum_clip_ammo = 50;
    OriginalHitmanWeaponData[31].move_speed = 1.1000000238419f;
    OriginalHitmanWeaponData[31].flags = 28721;
    OriginalHitmanWeaponData[31].anim_group = 25;
    OriginalHitmanWeaponData[31].fire_type = (eFireType) 1;
    OriginalHitmanWeaponData[31].model = 356;
    OriginalHitmanWeaponData[31].model2 = -1;
    OriginalHitmanWeaponData[31].weapon_slot = (eWeaponSlot) 5;
    OriginalHitmanWeaponData[31].fire_offset = CVector ( 0.74000000953674f, -0.03999999910593f, 0.12999999523163f );
    OriginalHitmanWeaponData[31].skill_level = (eWeaponSkill) 2;
    OriginalHitmanWeaponData[31].required_skill_level = 999;
    OriginalHitmanWeaponData[31].anim_loop_start = 0.20000001788139f;
    OriginalHitmanWeaponData[31].anim_loop_stop = 0.31400001049042f;
    OriginalHitmanWeaponData[31].anim_loop_bullet_fire = 0.23333334922791f;
    OriginalHitmanWeaponData[31].anim2_loop_start = 0.20000001788139f;
    OriginalHitmanWeaponData[31].anim2_loop_stop = 0.31400001049042f;
    OriginalHitmanWeaponData[31].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalHitmanWeaponData[31].anim_breakout_time = 3.3000001907349f;
    OriginalHitmanWeaponData[31].radius = 0.0f;
    OriginalHitmanWeaponData[31].aim_offset = 14;
    OriginalHitmanWeaponData[31].default_combo = 4;
    OriginalHitmanWeaponData[31].combos_available = 1;
    // 32 - Tec-9 Stat: 2
    OriginalHitmanWeaponData[32].weapon_range = 35.0f;
    OriginalHitmanWeaponData[32].target_range = 35.0f;
    OriginalHitmanWeaponData[32].accuracy = 1.1000000238419f;
    OriginalHitmanWeaponData[32].damage = 20;
    OriginalHitmanWeaponData[32].life_span = 0.0f;
    OriginalHitmanWeaponData[32].firing_speed = 0.0f;
    OriginalHitmanWeaponData[32].spread = 0.0f;
    OriginalHitmanWeaponData[32].maximum_clip_ammo = 100;
    OriginalHitmanWeaponData[32].move_speed = 1.0f;
    OriginalHitmanWeaponData[32].flags = 14387;
    OriginalHitmanWeaponData[32].anim_group = 15;
    OriginalHitmanWeaponData[32].fire_type = (eFireType) 1;
    OriginalHitmanWeaponData[32].model = 372;
    OriginalHitmanWeaponData[32].model2 = -1;
    OriginalHitmanWeaponData[32].weapon_slot = (eWeaponSlot) 4;
    OriginalHitmanWeaponData[32].fire_offset = CVector ( 0.44999998807907f, -0.050000000745058f, 0.10999999940395f );
    OriginalHitmanWeaponData[32].skill_level = (eWeaponSkill) 2;
    OriginalHitmanWeaponData[32].required_skill_level = 999;
    OriginalHitmanWeaponData[32].anim_loop_start = 0.36666667461395f;
    OriginalHitmanWeaponData[32].anim_loop_stop = 0.48066666722298f;
    OriginalHitmanWeaponData[32].anim_loop_bullet_fire = 0.40000003576279f;
    OriginalHitmanWeaponData[32].anim2_loop_start = 0.20000001788139f;
    OriginalHitmanWeaponData[32].anim2_loop_stop = 0.35400003194809f;
    OriginalHitmanWeaponData[32].anim2_loop_bullet_fire = 0.23333334922791f;
    OriginalHitmanWeaponData[32].anim_breakout_time = 3.3000001907349f;
    OriginalHitmanWeaponData[32].radius = 0.0f;
    OriginalHitmanWeaponData[32].aim_offset = 4;
    OriginalHitmanWeaponData[32].default_combo = 4;
    OriginalHitmanWeaponData[32].combos_available = 1;
    // End of Skill Level Weapons
}

bool CWeaponStatManager::LoadDefault ( CWeaponStat* pDest, eWeaponType weaponType, eWeaponSkill weaponSkill )
{
    if ( pDest )
    {
        int iVal = (int)weaponType;
        if ( iVal >= 22 && iVal <= 42 )
        {
            switch (weaponSkill)
            {
            case WEAPONSKILL_POOR:
                {
                    pDest->SetWeaponSkillLevel( weaponSkill );
                    pDest->SetWeaponRange ( OriginalPoorWeaponData[iVal].weapon_range );
                    pDest->SetTargetRange ( OriginalPoorWeaponData[iVal].target_range );
                    pDest->SetAccuracy ( OriginalPoorWeaponData[iVal].accuracy );
                    pDest->SetDamagePerHit ( OriginalPoorWeaponData[iVal].damage );
                    pDest->SetLifeSpan ( OriginalPoorWeaponData[iVal].life_span );
                    pDest->SetFiringSpeed ( OriginalPoorWeaponData[iVal].firing_speed );
                    pDest->SetSpread ( OriginalPoorWeaponData[iVal].spread );
                    pDest->SetMaximumClipAmmo ( OriginalPoorWeaponData[iVal].maximum_clip_ammo );
                    pDest->SetMoveSpeed ( OriginalPoorWeaponData[iVal].move_speed );
                    pDest->SetFlags ( OriginalPoorWeaponData[iVal].flags );
                    pDest->SetAnimGroup ( OriginalPoorWeaponData[iVal].anim_group );
                    pDest->SetFireType ( OriginalPoorWeaponData[iVal].fire_type );
                    pDest->SetModel ( OriginalPoorWeaponData[iVal].model );
                    pDest->SetModel2 ( OriginalPoorWeaponData[iVal].model2 );
                    pDest->SetSlot ( OriginalPoorWeaponData[iVal].weapon_slot );
                    pDest->SetFireOffset ( &OriginalPoorWeaponData[iVal].fire_offset );
                    pDest->SetSkill ( OriginalPoorWeaponData[iVal].skill_level );
                    pDest->SetRequiredStatLevel ( OriginalPoorWeaponData[iVal].required_skill_level );
                    pDest->SetWeaponAnimLoopStart ( OriginalPoorWeaponData[iVal].anim_loop_start );
                    pDest->SetWeaponAnimLoopStop ( OriginalPoorWeaponData[iVal].anim_loop_stop );
                    pDest->SetWeaponAnimLoopFireTime ( OriginalPoorWeaponData[iVal].anim_loop_bullet_fire );
                    pDest->SetWeaponAnim2LoopStart ( OriginalPoorWeaponData[iVal].anim2_loop_start );
                    pDest->SetWeaponAnim2LoopStop ( OriginalPoorWeaponData[iVal].anim2_loop_stop );
                    pDest->SetWeaponAnim2LoopFireTime ( OriginalPoorWeaponData[iVal].anim2_loop_bullet_fire );
                    pDest->SetWeaponAnimBreakoutTime ( OriginalPoorWeaponData[iVal].anim_breakout_time );
                    pDest->SetWeaponRadius ( OriginalPoorWeaponData[iVal].radius );
                    pDest->SetAimOffsetIndex ( OriginalPoorWeaponData[iVal].aim_offset );
                    pDest->SetDefaultCombo ( OriginalPoorWeaponData[iVal].default_combo );
                    pDest->SetCombosAvailable ( OriginalPoorWeaponData[iVal].combos_available );
                    break;
                }
            case WEAPONSKILL_PRO:
                {
                    pDest->SetWeaponSkillLevel( weaponSkill );
                    pDest->SetWeaponRange ( OriginalHitmanWeaponData[iVal].weapon_range );
                    pDest->SetTargetRange ( OriginalHitmanWeaponData[iVal].target_range );
                    pDest->SetAccuracy ( OriginalHitmanWeaponData[iVal].accuracy );
                    pDest->SetDamagePerHit ( OriginalHitmanWeaponData[iVal].damage );
                    pDest->SetLifeSpan ( OriginalHitmanWeaponData[iVal].life_span );
                    pDest->SetFiringSpeed ( OriginalHitmanWeaponData[iVal].firing_speed );
                    pDest->SetSpread ( OriginalHitmanWeaponData[iVal].spread );
                    pDest->SetMaximumClipAmmo ( OriginalHitmanWeaponData[iVal].maximum_clip_ammo );
                    pDest->SetMoveSpeed ( OriginalHitmanWeaponData[iVal].move_speed );
                    pDest->SetFlags ( OriginalHitmanWeaponData[iVal].flags );
                    pDest->SetAnimGroup ( OriginalHitmanWeaponData[iVal].anim_group );
                    pDest->SetFireType ( OriginalHitmanWeaponData[iVal].fire_type );
                    pDest->SetModel ( OriginalHitmanWeaponData[iVal].model );
                    pDest->SetModel2 ( OriginalHitmanWeaponData[iVal].model2 );
                    pDest->SetSlot ( OriginalHitmanWeaponData[iVal].weapon_slot );
                    pDest->SetFireOffset ( &OriginalHitmanWeaponData[iVal].fire_offset );
                    pDest->SetSkill ( OriginalHitmanWeaponData[iVal].skill_level );
                    pDest->SetRequiredStatLevel ( OriginalHitmanWeaponData[iVal].required_skill_level );
                    pDest->SetWeaponAnimLoopStart ( OriginalHitmanWeaponData[iVal].anim_loop_start );
                    pDest->SetWeaponAnimLoopStop ( OriginalHitmanWeaponData[iVal].anim_loop_stop );
                    pDest->SetWeaponAnimLoopFireTime ( OriginalHitmanWeaponData[iVal].anim_loop_bullet_fire );
                    pDest->SetWeaponAnim2LoopStart ( OriginalHitmanWeaponData[iVal].anim2_loop_start );
                    pDest->SetWeaponAnim2LoopStop ( OriginalHitmanWeaponData[iVal].anim2_loop_stop );
                    pDest->SetWeaponAnim2LoopFireTime ( OriginalHitmanWeaponData[iVal].anim2_loop_bullet_fire );
                    pDest->SetWeaponAnimBreakoutTime ( OriginalHitmanWeaponData[iVal].anim_breakout_time );
                    pDest->SetWeaponRadius ( OriginalHitmanWeaponData[iVal].radius );
                    pDest->SetAimOffsetIndex ( OriginalHitmanWeaponData[iVal].aim_offset );
                    pDest->SetDefaultCombo ( OriginalHitmanWeaponData[iVal].default_combo );
                    pDest->SetCombosAvailable ( OriginalHitmanWeaponData[iVal].combos_available );
                    break;
                }
            case WEAPONSKILL_STD:
                {
                    pDest->SetWeaponSkillLevel( weaponSkill );
                    pDest->SetWeaponRange ( OriginalNormalWeaponData[iVal].weapon_range );
                    pDest->SetTargetRange ( OriginalNormalWeaponData[iVal].target_range );
                    pDest->SetAccuracy ( OriginalNormalWeaponData[iVal].accuracy );
                    pDest->SetDamagePerHit ( OriginalNormalWeaponData[iVal].damage );
                    pDest->SetLifeSpan ( OriginalNormalWeaponData[iVal].life_span );
                    pDest->SetFiringSpeed ( OriginalNormalWeaponData[iVal].firing_speed );
                    pDest->SetSpread ( OriginalNormalWeaponData[iVal].spread );
                    pDest->SetMaximumClipAmmo ( OriginalNormalWeaponData[iVal].maximum_clip_ammo );
                    pDest->SetMoveSpeed ( OriginalNormalWeaponData[iVal].move_speed );
                    pDest->SetFlags ( OriginalNormalWeaponData[iVal].flags );
                    pDest->SetAnimGroup ( OriginalNormalWeaponData[iVal].anim_group );
                    pDest->SetFireType ( OriginalNormalWeaponData[iVal].fire_type );
                    pDest->SetModel ( OriginalNormalWeaponData[iVal].model );
                    pDest->SetModel2 ( OriginalNormalWeaponData[iVal].model2 );
                    pDest->SetSlot ( OriginalNormalWeaponData[iVal].weapon_slot );
                    pDest->SetFireOffset ( &OriginalNormalWeaponData[iVal].fire_offset );
                    pDest->SetSkill ( OriginalNormalWeaponData[iVal].skill_level );
                    pDest->SetRequiredStatLevel ( OriginalNormalWeaponData[iVal].required_skill_level );
                    pDest->SetWeaponAnimLoopStart ( OriginalNormalWeaponData[iVal].anim_loop_start );
                    pDest->SetWeaponAnimLoopStop ( OriginalNormalWeaponData[iVal].anim_loop_stop );
                    pDest->SetWeaponAnimLoopFireTime ( OriginalNormalWeaponData[iVal].anim_loop_bullet_fire );
                    pDest->SetWeaponAnim2LoopStart ( OriginalNormalWeaponData[iVal].anim2_loop_start );
                    pDest->SetWeaponAnim2LoopStop ( OriginalNormalWeaponData[iVal].anim2_loop_stop );
                    pDest->SetWeaponAnim2LoopFireTime ( OriginalNormalWeaponData[iVal].anim2_loop_bullet_fire );
                    pDest->SetWeaponAnimBreakoutTime ( OriginalNormalWeaponData[iVal].anim_breakout_time );
                    pDest->SetWeaponRadius ( OriginalNormalWeaponData[iVal].radius );
                    pDest->SetAimOffsetIndex ( OriginalNormalWeaponData[iVal].aim_offset );
                    pDest->SetDefaultCombo ( OriginalNormalWeaponData[iVal].default_combo );
                    pDest->SetCombosAvailable ( OriginalNormalWeaponData[iVal].combos_available );
                    break;
                }
            default:
                    break;
            }
        }
    }
    return true;
}

unsigned short CWeaponStatManager::GetSkillStatIndex ( eWeaponType eWeapon )
{
    switch ( eWeapon )
    {
        case WEAPONTYPE_PISTOL:
        {
            return 46;
            break;
        }
        case WEAPONTYPE_PISTOL_SILENCED:
        {
            return 47;
            break;
        }
        case WEAPONTYPE_DESERT_EAGLE:
        {
            return 48;
            break;
        }
        case WEAPONTYPE_SHOTGUN:
        {
            return 49;
            break;
        }
        case WEAPONTYPE_SAWNOFF_SHOTGUN:
        {
            return 50;
            break;
        }
        case WEAPONTYPE_SPAS12_SHOTGUN:
        {
            return 51;
            break;
        }
        case WEAPONTYPE_MICRO_UZI:
        case WEAPONTYPE_TEC9:
        {
            return 52;
            break;
        }
        case WEAPONTYPE_MP5:
        {
            return 53;
            break;
        }
        case WEAPONTYPE_M4:
        {
            return 54;
            break;
        }
        case WEAPONTYPE_AK47:
        {
            return 55;
            break;
        }
        case WEAPONTYPE_SNIPERRIFLE:
        case WEAPONTYPE_COUNTRYRIFLE:
        {
            return 56;
            break;
        }
        default:
        {
            return 0;
        }
    }
}

float CWeaponStatManager::GetWeaponRangeFromSkillLevel ( eWeaponType eWeapon, float fSkillLevel )
{
    float fWeaponRange = 0.0f;
    CWeaponStat* pWeaponStat = GetWeaponStatsFromSkillLevel ( eWeapon, fSkillLevel );
    if ( pWeaponStat )
    {
        fWeaponRange = pWeaponStat->GetWeaponRange ( );
    }
    return fWeaponRange;
}