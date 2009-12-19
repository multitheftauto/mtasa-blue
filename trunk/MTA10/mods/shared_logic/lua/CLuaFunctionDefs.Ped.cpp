/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Ped.cpp
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Derek Abdine <>
*               Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"

int CLuaFunctionDefs::GetPedVoice ( lua_State* luaVM )
{
    // Right type?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            if ( !pPed->IsSpeechEnabled () )
            {
                lua_pushstring ( luaVM, "PED_TYPE_DISABLED" );
                return 1;
            }
            else
            {
                const char* szVoiceType = 0;
                const char* szVoiceBank = 0;
                pPed->GetVoice ( &szVoiceType, &szVoiceBank );
                if ( szVoiceType && szVoiceBank )
                {
                    lua_pushstring ( luaVM, szVoiceType );
                    lua_pushstring ( luaVM, szVoiceBank );
                    return 2;
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedVoice", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedVoice" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetPedVoice ( lua_State* luaVM )
{
    // Right type?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) && 
        lua_istype ( luaVM, 2, LUA_TSTRING ) )
    {
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        const char* szVoiceType = lua_tostring ( luaVM, 2 );
        const char* szVoiceBank = NULL;
        if ( lua_istype ( luaVM, 3, LUA_TSTRING ) ) szVoiceBank = lua_tostring ( luaVM, 3 );        

        if ( pPed && szVoiceType )
        {
            if ( !stricmp ( szVoiceType, "PED_TYPE_DISABLED" ) ) pPed->SetSpeechEnabled ( false );
            else if ( szVoiceBank )
            {
                pPed->SetSpeechEnabled ( true );
                pPed->SetVoice ( szVoiceType, szVoiceBank );
            }

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else if ( !pPed )
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedVoice", "ped", 1 );
        else if ( !szVoiceType )
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedVoice", "voicetype", 1 );
        else if ( !szVoiceBank )
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedVoice", "voicebank", 1 );

    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedVoice" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedWeapon ( lua_State* luaVM )
{
    // Right type?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Grab the slot if specified
            unsigned char ucSlot = 0xFF;
            if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) || lua_istype ( luaVM, 2, LUA_TSTRING ) )
                ucSlot = ( unsigned char ) lua_tonumber ( luaVM, 2 );

            if ( ucSlot == 0xFF )
                ucSlot = pPed->GetCurrentWeaponSlot ();

            CWeapon* pWeapon = pPed->GetWeapon ( ( eWeaponSlot ) ucSlot );
            if ( pWeapon )
            {
                unsigned char ucWeapon = pWeapon->GetType ();
                lua_pushnumber ( luaVM, ucWeapon );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedWeapon", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedWeapon" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedWeaponSlot ( lua_State* luaVM )
{
    // Right type?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Grab his current slot
            int iSlot = pPed->GetCurrentWeaponSlot ();
            lua_pushnumber ( luaVM, iSlot );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedWeaponSlot", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedWeaponSlot" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedAmmoInClip ( lua_State* luaVM )
{
    // Right types?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped and the slot provided
        CClientPed* pPed = lua_toped ( luaVM, 1 );

        // Got a ped
        if ( pPed )
        {
            // Got a second argument too (slot)?
            unsigned char ucSlot = pPed->GetCurrentWeaponSlot ();
            int iArgument2 = lua_type ( luaVM, 2 );
            if ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER )
            {
                ucSlot = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            }

            CWeapon* pWeapon = pPed->GetWeapon ( (eWeaponSlot) ucSlot );
            if ( pWeapon )
            {
                unsigned short usAmmo = static_cast < unsigned short > ( pWeapon->GetAmmoInClip () );
                lua_pushnumber ( luaVM, usAmmo );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedAmmoInClip", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedAmmoInClip" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedTotalAmmo ( lua_State* luaVM )
{
    // Right types?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped and the slot
        CClientPed* pPed = lua_toped ( luaVM, 1 );

        // Got the ped?
        if ( pPed )
        {
            // Got a slot argument too?
            unsigned char ucSlot = pPed->GetCurrentWeaponSlot ();
            int iArgument2 = lua_type ( luaVM, 2 );
            if ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER )
            {
                ucSlot = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            }

            // Grab the ammo and return
            CWeapon* pWeapon = pPed->GetWeapon ( (eWeaponSlot) ucSlot );
            if ( pWeapon )
            {
                unsigned short usAmmo = static_cast < unsigned short > ( pWeapon->GetAmmoTotal () );
                lua_pushnumber ( luaVM, usAmmo );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedTotalAmmo", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedTotalAmmo" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedWeaponMuzzlePosition ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            CVector vecMuzzlePos;
            if ( CStaticFunctionDefinitions::GetPedWeaponMuzzlePosition ( *pPed, vecMuzzlePos ) )
            {
                lua_pushnumber ( luaVM, vecMuzzlePos.fX );
                lua_pushnumber ( luaVM, vecMuzzlePos.fY );
                lua_pushnumber ( luaVM, vecMuzzlePos.fZ );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedWeaponMuzzlePosition", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedWeaponMuzzlePosition" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedOccupiedVehicle ( lua_State* luaVM )
{
    // Correct type?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Grab his occupied vehicle
            CClientVehicle* pVehicle = pPed->GetOccupiedVehicle ();
            if ( pVehicle )
            {                
                lua_pushelement ( luaVM, pVehicle );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedOccupiedVehicle", "ped", 1 );
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedTask ( lua_State* luaVM )
{
    // Check types
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TSTRING ) &&
        ( iArgument3 == LUA_TSTRING || iArgument3 == LUA_TNUMBER ) )
    {
        // Grab the ped, the priority and the task type
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        const char* szPriority = lua_tostring ( luaVM, 2 );
        unsigned int uiTaskType = static_cast < unsigned int > ( lua_tonumber ( luaVM, 3 ) );
        int iIndex = 0;

        // Grab the index if provided
        int iArgument4 = lua_type ( luaVM, 4 );
        if ( iArgument4 == LUA_TSTRING || iArgument4 == LUA_TNUMBER )
            iIndex = static_cast < int > ( lua_tonumber ( luaVM, 4 ) );

        // Valid ped?
        if ( pPed )
        {
            // Any priority specified?
            if ( szPriority && szPriority [ 0 ] )
            {
                // Primary or secondary task grabbed?
                bool bPrimary = false;
                if ( ( bPrimary = !stricmp ( szPriority, "primary" ) ) ||
                    ( !stricmp ( szPriority, "secondary" ) ) )
                {
                    // Grab the taskname and return it
                    char* szTaskName = CStaticFunctionDefinitions::GetPedTask ( *pPed, bPrimary, uiTaskType, iIndex );
                    if ( szTaskName )
                    {
                        lua_pushstring ( luaVM, szTaskName );
                        return 1;
                    }
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedTask", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedTask" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedSimplestTask ( lua_State* luaVM )
{
    // Check types
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Grab his simplest task and return it
            char* szTaskName = CStaticFunctionDefinitions::GetPedSimplestTask ( *pPed );
            if ( szTaskName )
            {
                lua_pushstring ( luaVM, szTaskName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedSimplestTask", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedSimplestTask" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsPedDoingTask ( lua_State* luaVM )
{
    // Check types
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TSTRING ) )
    {
        // Grab the ped and the task name to check
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        const char* szTaskName = lua_tostring ( luaVM, 2 );

        // Check ped
        if ( pPed )
        {
            // Check whether he's doing that task or not
            bool bIsDoingTask;
            if ( CStaticFunctionDefinitions::IsPedDoingTask ( *pPed, szTaskName, bIsDoingTask ) )
            {
                lua_pushboolean ( luaVM, bIsDoingTask );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPedDoingTask", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPedDoingTask" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedTarget ( lua_State* luaVM )
{
    // Check types
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Grab his target element
            CClientEntity* pEntity = CStaticFunctionDefinitions::GetPedTarget ( *pPed );
            if ( pEntity )
            {
                lua_pushelement ( luaVM, pEntity );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedTarget", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedTarget" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedTargetStart ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Grab his start aim position and return it
            CVector vecStart;
            pPed->GetShotData ( &vecStart );

            lua_pushnumber ( luaVM, vecStart.fX );
            lua_pushnumber ( luaVM, vecStart.fY );
            lua_pushnumber ( luaVM, vecStart.fZ );
            return 3;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedTargetStart", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedTargetStart" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedTargetEnd ( lua_State* luaVM )
{
    // Check types
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Grab the ped end target position and return it
            CVector vecEnd;
            pPed->GetShotData ( NULL, &vecEnd );

            lua_pushnumber ( luaVM, vecEnd.fX );
            lua_pushnumber ( luaVM, vecEnd.fY );
            lua_pushnumber ( luaVM, vecEnd.fZ );
            return 3;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedTargetEnd", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedTargetEnd" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedTargetRange ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // TODO: getPedTargetRange
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedTargetRange", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedTargetRange" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedTargetCollision ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Grab his target collision and return it
            CVector vecCollision;
            if ( CStaticFunctionDefinitions::GetPedTargetCollision ( *pPed, vecCollision ) )
            {
                lua_pushnumber ( luaVM, vecCollision.fX );
                lua_pushnumber ( luaVM, vecCollision.fY );
                lua_pushnumber ( luaVM, vecCollision.fZ );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedTargetCollision", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedTargetCollision" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedArmor ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Grab the armor and return it
            float fArmor = pPed->GetArmor ();
            lua_pushnumber ( luaVM, fArmor );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedArmor", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedArmor" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedStat ( lua_State* luaVM )
{
    // Check the types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
        ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER ) )
    {
        // Grab the ped and the stat argument
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        unsigned short usStat = static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) );

        // Valid ped?
        if ( pPed )
        {
            // Check the stat
            if ( usStat < NUM_PLAYER_STATS )
            {
                float fValue = pPed->GetStat ( usStat );
                lua_pushnumber ( luaVM, fValue );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedStat", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedStat" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsPedChoking ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped and verify him
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Return whether he's choking or not
            lua_pushboolean ( luaVM, pPed->IsChoking () );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPedChoking", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPedChoking" );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefs::IsPedDucked ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Grab his ducked state
            bool bDucked = pPed->IsDucked ();
            lua_pushboolean ( luaVM, bDucked );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPedDucked", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPedDucked" );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefs::IsPedInVehicle ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Find out whether he's in a vehicle or not
            bool bInVehicle;
            if ( CStaticFunctionDefinitions::IsPedInVehicle ( *pPed, bInVehicle ) )
            {
                // Return that state
                lua_pushboolean ( luaVM, bInVehicle );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPedInVehicle", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPedInVehicle" );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefs::DoesPedHaveJetPack ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Find out whether he has a jetpack or not and return it
            bool bHasJetPack = pPed->HasJetPack ();
            lua_pushboolean ( luaVM, bHasJetPack );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "doesPedHaveJetPack", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "doesPedHaveJetPack" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsPedOnGround ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Find out whether he's on the ground or not and return it
            bool bOnGround = pPed->IsOnGround ();
            lua_pushboolean ( luaVM, bOnGround );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPedOnGround", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPedOnGround" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedContactElement ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) )
    {
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            CClientEntity* pEntity = pPed->GetContactEntity ();
            if ( pEntity )
            {
                lua_pushelement ( luaVM, pEntity );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedContactElement", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedContactElement" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedRotation ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) )
    {
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            float fRotation = ConvertRadiansToDegrees ( pPed->GetCurrentRotation () );
            lua_pushnumber ( luaVM, fRotation );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedRotation", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedRotation" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::CanPedBeKnockedOffBike ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) )
    {
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            bool bCanBeKnockedOffBike = pPed->GetCanBeKnockedOffBike ();
            lua_pushboolean ( luaVM, bCanBeKnockedOffBike );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "canPedBeKnockedOffBike", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "canPedBeKnockedOffBike" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedBonePosition ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            unsigned char ucBone = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            if ( ucBone <= BONE_RIGHTFOOT )
            {
                eBone bone = ( eBone ) ucBone;
                CVector vecPosition;
                if ( CStaticFunctionDefinitions::GetPedBonePosition ( *pPed, bone, vecPosition ) )
                {
                    lua_pushnumber ( luaVM, vecPosition.fX );
                    lua_pushnumber ( luaVM, vecPosition.fY );
                    lua_pushnumber ( luaVM, vecPosition.fZ );
                    return 3;
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedBonePosition", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedBonePosition" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedWeaponSlot ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        // Grab the arguments
        CClientEntity* pElement = lua_toelement ( luaVM, 1 );
        int iSlot = static_cast < int > ( lua_tonumber ( luaVM, 2 ) );

        // Valid ped?
        if ( pElement )
        {
            // Valid slot?
            if ( iSlot >= 0 )
            {
                // Set his slot
                if ( CStaticFunctionDefinitions::SetPedWeaponSlot ( *pElement, iSlot ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedWeaponSlot", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedWeaponSlot" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedClothes ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CClientPed * pPed = lua_toped ( luaVM, 1 );
        unsigned char ucType = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
        if ( pPed )
        {
            char szTexture [ 128 ], szModel [ 128 ];
            if ( CStaticFunctionDefinitions::GetPedClothes ( *pPed, ucType, szTexture, szModel ) )
            {
                lua_pushstring ( luaVM, szTexture );
                lua_pushstring ( luaVM, szModel );
                return 2;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedClothes", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedClothes" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedControlState ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( lua_type ( luaVM, 2 ) == LUA_TSTRING ) )
    {
        CClientPed * pPed = lua_toped ( luaVM, 1 );
        const char * szControl = lua_tostring ( luaVM, 2 );
        if ( pPed )
        {
            bool bState;
            if ( CStaticFunctionDefinitions::GetPedControlState ( *pPed, szControl, bState ) )
            {
                lua_pushboolean ( luaVM, bState );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedControlState", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedControlState" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsPedDoingGangDriveby ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) )
    {
        CClientPed * pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            bool bDoingGangDriveby;
            if ( CStaticFunctionDefinitions::IsPedDoingGangDriveby ( *pPed, bDoingGangDriveby ) )
            {
                lua_pushboolean ( luaVM, bDoingGangDriveby );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPedDoingGangDriveby", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPedDoingGangDriveby" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedAnimation ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) )
    {
        CClientPed * pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            char szBlockName [ 128 ], szAnimName [ 128 ];
            if ( CStaticFunctionDefinitions::GetPedAnimation ( *pPed, szBlockName, szAnimName, 128 ))
            {
                lua_pushstring ( luaVM, szBlockName );
                lua_pushstring ( luaVM, szAnimName );
                return 2;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedAnimation", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedAnimation" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedMoveAnim ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) )
    {
        CClientPed * pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            unsigned int iMoveAnim;
            if ( CStaticFunctionDefinitions::GetPedMoveAnim ( *pPed, iMoveAnim ) )
            {
                lua_pushnumber ( luaVM, iMoveAnim );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedWalkingStyle", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedWalkingStyle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsPedHeadless ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) )
    {
        CClientPed * pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            bool bHeadless;
            if ( CStaticFunctionDefinitions::IsPedHeadless ( *pPed, bHeadless ) )
            {
                lua_pushboolean ( luaVM, bHeadless );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPedHeadless", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPedHeadless" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsPedFrozen ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) )
    {
        CClientPed * pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            bool bFrozen;
            if ( CStaticFunctionDefinitions::IsPedFrozen ( *pPed, bFrozen ) )
            {
                lua_pushboolean ( luaVM, bFrozen );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPedFrozen", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPedFrozen" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedCameraRotation ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) )
    {
        CClientPed * pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            float fRotation = 0.0f;
            if ( CStaticFunctionDefinitions::GetPedCameraRotation ( *pPed, fRotation ) )
            {
                lua_pushnumber ( luaVM, fRotation );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedCameraRotation", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedCameraRotation" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsPedOnFire ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) )
    {
        CClientPed * pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            bool bOnFire;
            if ( CStaticFunctionDefinitions::IsPedOnFire ( *pPed, bOnFire ) )
            {
                lua_pushboolean ( luaVM, bOnFire );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPedOnFire", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPedOnFire" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetPedOnFire ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        bool bOnFire = ( ( lua_toboolean ( luaVM, 2 ) ) ? true:false );

        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedOnFire ( *pEntity, bOnFire ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedOnFire", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedOnFire" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::AddPedClothes ( lua_State* luaVM )
{
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( lua_type ( luaVM, 2 ) == LUA_TSTRING ) &&
        ( lua_type ( luaVM, 3 ) == LUA_TSTRING ) &&
        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        const char* szTexture = lua_tostring ( luaVM, 2 );
        const char* szModel = lua_tostring ( luaVM, 3 );
        unsigned char ucType = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );

        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::AddPedClothes ( *pEntity, const_cast < char* > ( szTexture ), const_cast < char* > ( szModel ), ucType ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "addPedClothes", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "addPedClothes" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::RemovePedClothes ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        unsigned char ucType = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );

        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::RemovePedClothes ( *pEntity, ucType ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "removePedClothes", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "removePedClothes" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedControlState ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( lua_type ( luaVM, 2 ) == LUA_TSTRING ) &&
        ( lua_type ( luaVM, 3 ) == LUA_TBOOLEAN ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        const char * szControl = lua_tostring ( luaVM, 2 );
        bool bState = ( lua_toboolean ( luaVM, 3 ) ) ? true:false;
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedControlState ( *pEntity, szControl, bState ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedControlState", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedControlState" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedDoingGangDriveby ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( lua_type ( luaVM, 2 ) == LUA_TBOOLEAN ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        bool bDoingGangDriveby = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedDoingGangDriveby ( *pEntity, bDoingGangDriveby ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedDoingGangDriveby", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedDoingGangDriveby" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedLookAt ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        CVector vecPosition;
        vecPosition.fX = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
        vecPosition.fY = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );
        vecPosition.fZ = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );
        int iTime = 3000;
        CClientEntity * pTarget = NULL;

        int iArgument5 = lua_type ( luaVM, 5 );
        if ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING )
        {
            iTime = static_cast < int > ( lua_tonumber ( luaVM, 5 ) );

            if ( lua_type ( luaVM, 6 ) == LUA_TLIGHTUSERDATA )
                pTarget = lua_toelement ( luaVM, 6 );
        }

        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedLookAt ( *pEntity, vecPosition, iTime, pTarget ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedLookAt", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedLookAt" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedHeadless ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( lua_type ( luaVM, 2 ) == LUA_TBOOLEAN ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        bool bHeadless = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedHeadless ( *pEntity, bHeadless ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedHeadless", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedHeadless" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedFrozen ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( lua_type ( luaVM, 2 ) == LUA_TBOOLEAN ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        bool bFrozen = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedFrozen ( *pEntity, bFrozen ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedFrozen", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedFrozen" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetPedCameraRotation ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        float fRotation = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedCameraRotation ( *pEntity, fRotation ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedCameraRotation", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedCameraRotation" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedAimTarget ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );

        if ( pEntity )
        {
            CVector vecTarget;
            vecTarget.fX = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            vecTarget.fY = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );
            vecTarget.fZ = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );

            if ( CStaticFunctionDefinitions::SetPedAimTarget ( *pEntity, vecTarget ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedAimTarget", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedAimTarget" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedRotation ( lua_State* luaVM )
{
    // Check types
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        // Grab the entity
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        float fRotation = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );

        // Valid element?
        if ( pEntity )
        {
            // Set the new rotation
            if ( CStaticFunctionDefinitions::SetPedRotation ( *pEntity, fRotation ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedRotation", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedRotation" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedCanBeKnockedOffBike ( lua_State* luaVM )
{
    // Check types
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
    {
        // Grab the entity
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        bool bCanBeKnockedOffBike = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;

        // Valid element?
        if ( pEntity )
        {
            // Set the new rotation
            if ( CStaticFunctionDefinitions::SetPedCanBeKnockedOffBike ( *pEntity, bCanBeKnockedOffBike ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedCanBeKnockedOffBike", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedCanBeKnockedOffBike" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedAnimation ( lua_State* luaVM )
{
    // Check types
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the element
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            const char * szBlockName = NULL;
            const char * szAnimName = NULL;
            int iTime = -1;
            bool bLoop = true;
            bool bUpdatePosition = true;
            bool bInterruptable = true;
            if ( lua_type ( luaVM, 2 ) == LUA_TSTRING ) szBlockName = lua_tostring ( luaVM, 2 );
            if ( lua_type ( luaVM, 3 ) == LUA_TSTRING ) szAnimName = lua_tostring ( luaVM, 3 );
            int iArgument4 = lua_type ( luaVM, 4 );
            if ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING )
                iTime = static_cast < int > ( lua_tonumber ( luaVM, 4 ) );
            if ( lua_type ( luaVM, 5 ) == LUA_TBOOLEAN )
                bLoop = ( lua_toboolean ( luaVM, 5 ) ) ? true:false;
            if ( lua_type ( luaVM, 6 ) == LUA_TBOOLEAN )
                bUpdatePosition = ( lua_toboolean ( luaVM, 6 ) ) ? true:false;
            if ( lua_type ( luaVM, 6 ) == LUA_TBOOLEAN )
                bInterruptable = ( lua_toboolean ( luaVM, 7 ) ) ? true:false;

            if ( CStaticFunctionDefinitions::SetPedAnimation ( *pEntity, szBlockName, szAnimName, iTime, bLoop, bUpdatePosition, bInterruptable ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedAnimation", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedAnimation" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedMoveAnim ( lua_State* luaVM )
{
    // Check types
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( lua_type ( luaVM, 2 ) == LUA_TNUMBER ) )
    {
        // Grab the element
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            unsigned int iMoveAnim = static_cast < unsigned int > ( lua_tonumber ( luaVM, 2 ) );

            if ( CStaticFunctionDefinitions::SetPedMoveAnim ( *pEntity, iMoveAnim ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedWalkingStyle", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedWalkingStyle" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::CreatePed ( lua_State* luaVM )
{
    // Valid types?
    if ( ( lua_type ( luaVM, 1 ) == LUA_TNUMBER || lua_type ( luaVM, 1 ) == LUA_TSTRING ) &&
        ( lua_type ( luaVM, 2 ) == LUA_TNUMBER || lua_type ( luaVM, 2 ) == LUA_TSTRING ) &&
        ( lua_type ( luaVM, 3 ) == LUA_TNUMBER || lua_type ( luaVM, 3 ) == LUA_TSTRING ) &&
        ( lua_type ( luaVM, 4 ) == LUA_TNUMBER || lua_type ( luaVM, 4 ) == LUA_TSTRING ) )
    {
        // Grab arguments
        CVector vecPosition;
        unsigned long ulModel = static_cast < unsigned long > ( lua_tonumber ( luaVM, 1 ) );
        vecPosition.fX = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
        vecPosition.fY = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );
        vecPosition.fZ = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );

        float fRotation = 0.0f;
        if ( lua_type ( luaVM, 5 ) == LUA_TNUMBER || lua_type ( luaVM, 5 ) == LUA_TSTRING )
        {
            fRotation = static_cast < float > ( lua_tonumber ( luaVM, 5 ) );
        }

        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource * pResource = pLuaMain->GetResource();
            if ( pResource )
            {
                // Create it
                CClientPed* pPed = CStaticFunctionDefinitions::CreatePed ( *pResource, ulModel, vecPosition, fRotation );
                if ( pPed )
                {
                    // Return it
                    lua_pushelement ( luaVM, pPed );
                    return 1;
                }
            }
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}
