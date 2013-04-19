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
#define MIN_CLIENT_REQ_REMOVEPEDFROMVEHICLE_CLIENTSIDE  "1.3.0-9.04482"
#define MIN_CLIENT_REQ_WARPPEDINTOVEHICLE_CLIENTSIDE    "1.3.0-9.04482"

int CLuaFunctionDefs::GetPedVoice ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetPedVoice ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    SString strVoiceType = "", strVoiceBank = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadString ( strVoiceType );
    argStream.ReadString ( strVoiceBank, "" );

    if ( !argStream.HasErrors ( ) )
    {
        const char* szVoiceType = strVoiceType.c_str ( );
        const char* szVoiceBank = strVoiceBank == "" ? NULL : strVoiceBank.c_str ( );

        if ( pPed && szVoiceType )
        {
            if ( !stricmp ( szVoiceType, "PED_TYPE_DISABLED" ) )
            {
                pPed->SetSpeechEnabled ( false );
            }

            else if ( szVoiceBank )
            {
                pPed->SetSpeechEnabled ( true );
                pPed->SetVoice ( szVoiceType, szVoiceBank );
            }

            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedWeapon ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    unsigned char ucSlot = 0xFF;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadNumber ( ucSlot, 0xFF );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedWeaponSlot ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            // Grab his current slot
            int iSlot = pPed->GetCurrentWeaponSlot ();
            lua_pushnumber ( luaVM, iSlot );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedAmmoInClip ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    unsigned char ucSlot = 0xFF;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadNumber ( ucSlot, 0xFF );

    if ( !argStream.HasErrors ( ) )
    {
        // Got a ped
        if ( pPed )
        {
            // Got a second argument too (slot)?
            ucSlot = ucSlot == 0xFF ? pPed->GetCurrentWeaponSlot ( ) : ucSlot;

            CWeapon* pWeapon = pPed->GetWeapon ( (eWeaponSlot) ucSlot );
            if ( pWeapon )
            {
                unsigned short usAmmo = static_cast < unsigned short > ( pWeapon->GetAmmoInClip () );
                lua_pushnumber ( luaVM, usAmmo );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedTotalAmmo ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    unsigned char ucSlot = 0xFF;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadNumber ( ucSlot, 0xFF );

    if ( !argStream.HasErrors ( ) )
    {
        // Got the ped?
        if ( pPed )
        {
            // Got a second argument too (slot)?
            ucSlot = ucSlot == 0xFF ? pPed->GetCurrentWeaponSlot ( ) : ucSlot;

            // Grab the ammo and return
            CWeapon* pWeapon = pPed->GetWeapon ( (eWeaponSlot) ucSlot );
            if ( pWeapon )
            {
                // Keep server and client synced
                unsigned short usAmmo = 1;
                if ( CWeaponNames::DoesSlotHaveAmmo ( ucSlot ) )
                    usAmmo = static_cast < unsigned short > ( pWeapon->GetAmmoTotal () );
                
                lua_pushnumber ( luaVM, usAmmo );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedWeaponMuzzlePosition ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedOccupiedVehicle ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedTask ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    SString strPriority = "";
    unsigned int uiTaskType = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadString ( strPriority );
    argStream.ReadNumber ( uiTaskType );

    if ( !argStream.HasErrors ( ) )
    {
        // Valid ped?
        if ( pPed )
        {
            // Any priority specified?
            if ( strPriority != "" )
            {
                // Primary or secondary task grabbed?
                bool bPrimary = false;
                if ( ( bPrimary = !stricmp ( strPriority.c_str ( ), "primary" ) ) ||
                    ( !stricmp ( strPriority.c_str ( ), "secondary" ) ) )
                {
                    // Grab the taskname list and return it
                    std::vector < SString > taskHierarchy;
                    if ( CStaticFunctionDefinitions::GetPedTask ( *pPed, bPrimary, uiTaskType, taskHierarchy ) )
                    {
                        for ( uint i = 0 ; i < taskHierarchy.size () ; i++ )
                            lua_pushstring ( luaVM, taskHierarchy[i] );
                        return taskHierarchy.size ();
                    }
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedSimplestTask ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            // Grab his simplest task and return it
            const char* szTaskName = CStaticFunctionDefinitions::GetPedSimplestTask ( *pPed );
            if ( szTaskName )
            {
                lua_pushstring ( luaVM, szTaskName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsPedDoingTask ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    SString strTaskName = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadString ( strTaskName );

    if ( !argStream.HasErrors ( ) )
    {
        // Check ped
        if ( pPed )
        {
            // Check whether he's doing that task or not
            bool bIsDoingTask;
            if ( CStaticFunctionDefinitions::IsPedDoingTask ( *pPed, strTaskName.c_str ( ), bIsDoingTask ) )
            {
                lua_pushboolean ( luaVM, bIsDoingTask );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedTarget ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedTargetStart ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedTargetEnd ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedTargetRange ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            // TODO: getPedTargetRange
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedTargetCollision ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedArmor ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            // Grab the armor and return it
            float fArmor = pPed->GetArmor ();
            lua_pushnumber ( luaVM, fArmor );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedStat ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    unsigned short usStat = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadNumber ( usStat );

    if ( !argStream.HasErrors ( ) )
    {
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsPedChoking ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            // Return whether he's choking or not
            lua_pushboolean ( luaVM, pPed->IsChoking () );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefs::IsPedDucked ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            // Grab his ducked state
            bool bDucked = pPed->IsDucked ();
            lua_pushboolean ( luaVM, bDucked );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefs::IsPedInVehicle ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefs::DoesPedHaveJetPack ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            // Find out whether he has a jetpack or not and return it
            bool bHasJetPack = pPed->HasJetPack ();
            lua_pushboolean ( luaVM, bHasJetPack );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsPedOnGround ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            // Find out whether he's on the ground or not and return it
            bool bOnGround = pPed->IsOnGround ();
            lua_pushboolean ( luaVM, bOnGround );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedContactElement ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedRotation ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            float fRotation = ConvertRadiansToDegrees ( pPed->GetCurrentRotation () );
            lua_pushnumber ( luaVM, fRotation );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::CanPedBeKnockedOffBike ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            bool bCanBeKnockedOffBike = pPed->GetCanBeKnockedOffBike ();
            lua_pushboolean ( luaVM, bCanBeKnockedOffBike );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedBonePosition ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    unsigned char ucBone = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadNumber ( ucBone );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedWeaponSlot ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    int iSlot = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadNumber ( iSlot );

    if ( !argStream.HasErrors ( ) )
    {
        // Valid ped?
        if ( pPed )
        {
            // Valid slot?
            if ( iSlot >= 0 )
            {
                // Set his slot
                if ( CStaticFunctionDefinitions::SetPedWeaponSlot ( *pPed, iSlot ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedClothes ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    unsigned char ucType = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadNumber ( ucType );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            SString strTexture, strModel;
            if ( CStaticFunctionDefinitions::GetPedClothes ( *pPed, ucType, strTexture, strModel ) )
            {
                lua_pushstring ( luaVM, strTexture );
                lua_pushstring ( luaVM, strModel );
                return 2;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "player", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedControlState ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    SString strControl = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadString ( strControl );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            bool bState;
            if ( CStaticFunctionDefinitions::GetPedControlState ( *pPed, strControl.c_str ( ), bState ) )
            {
                lua_pushboolean ( luaVM, bState );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetPedAnalogControlState ( lua_State* luaVM )
{
    SString strControlState = "";
    float fState = 0.0f;
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadString ( strControlState );
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            float fState;
            if ( CStaticFunctionDefinitions::GetPedAnalogControlState ( *pPed, strControlState, fState ) )
            {
                lua_pushnumber ( luaVM, fState );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::IsPedDoingGangDriveby ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedAnalogControlState ( lua_State* luaVM )
{
    SString strControlState = "";
    float fState = 0.0f;
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadString ( strControlState );
    argStream.ReadNumber ( fState );
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedAnalogControlState ( *pEntity, strControlState, fState ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetPedAnimation ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            SString strBlockName, strAnimName;
            if ( CStaticFunctionDefinitions::GetPedAnimation ( *pPed, strBlockName, strAnimName ) )
            {
                lua_pushstring ( luaVM, strBlockName );
                lua_pushstring ( luaVM, strAnimName );
                return 2;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetPedMoveState ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            std::string strMoveState;
            if ( CStaticFunctionDefinitions::GetPedMoveState ( *pPed, strMoveState ) )
            {
                lua_pushstring ( luaVM, strMoveState.c_str() );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetPedMoveAnim ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsPedHeadless ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsPedFrozen ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsPedFootBloodEnabled ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            bool bHasFootBlood = false;
            if ( CStaticFunctionDefinitions::IsPedFootBloodEnabled ( *pPed, bHasFootBlood ) )
            {
                lua_pushboolean ( luaVM, bHasFootBlood );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedCameraRotation ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsPedOnFire ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetPedOnFire ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    bool bOnFire = false;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadBool ( bOnFire );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            if ( CStaticFunctionDefinitions::SetPedOnFire ( *pPed, bOnFire ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::WarpPedIntoVehicle ( lua_State* luaVM )
{
//  warpPedIntoVehicle ( element ped, element vehicle, int seat )
    CClientPed* pPed; CClientVehicle* pVehicle; uint uiSeat;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( uiSeat, 0 );

    MinClientReqCheck ( argStream, MIN_CLIENT_REQ_REMOVEPEDFROMVEHICLE_CLIENTSIDE, "function is being called client side" );

    if ( !argStream.HasErrors () )
    {
        if ( !pPed->IsLocalEntity () || !pVehicle->IsLocalEntity () )
            argStream.SetCustomError ( "This client side function will only work with client created peds and vehicles" );
    }

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::WarpPedIntoVehicle ( pPed, pVehicle, uiSeat ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::RemovePedFromVehicle ( lua_State* luaVM )
{
//  removePedFromVehicle ( element ped )
    CClientPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    MinClientReqCheck ( argStream, MIN_CLIENT_REQ_WARPPEDINTOVEHICLE_CLIENTSIDE, "function is being called client side" );

    if ( !argStream.HasErrors () )
    {
        if ( !pPed->IsLocalEntity ()  )
            argStream.SetCustomError ( "This client side function will only work with client created peds" );
    }

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::RemovePedFromVehicle ( pPed ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedOxygenLevel ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            float fOxygen;
            if ( CStaticFunctionDefinitions::GetPedOxygenLevel ( *pPed, fOxygen ) )
            {
                lua_pushnumber ( luaVM, fOxygen );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::AddPedClothes ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    SString strTexture = "", strModel = "";
    unsigned char ucType = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadString ( strTexture );
    argStream.ReadString ( strModel );
    argStream.ReadNumber ( ucType );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            if ( CStaticFunctionDefinitions::AddPedClothes ( *pPed, strTexture.c_str ( ), strModel.c_str ( ), ucType ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::RemovePedClothes ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    unsigned char ucType = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadNumber ( ucType );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            if ( CStaticFunctionDefinitions::RemovePedClothes ( *pPed, ucType ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedControlState ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    SString strControl = "";
    bool bState = false;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadString ( strControl );
    argStream.ReadBool ( bState );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            if ( CStaticFunctionDefinitions::SetPedControlState ( *pPed, strControl.c_str ( ), bState ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedDoingGangDriveby ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    bool bDoingGangDriveby = false;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadBool ( bDoingGangDriveby );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            if ( CStaticFunctionDefinitions::SetPedDoingGangDriveby ( *pPed, bDoingGangDriveby ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedLookAt ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CVector vecPosition;
    int iTime = 3000;
    int iBlend = 1000;
    CClientEntity * pTarget = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );
    argStream.ReadNumber ( iTime, 3000 );
    if ( argStream.NextIsUserData ( ) )
    {
        argStream.ReadUserData ( pTarget );
    }
    else
    {
        argStream.ReadNumber ( iBlend, 1000 );
        argStream.ReadUserData ( pTarget, NULL );
    }

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            if ( CStaticFunctionDefinitions::SetPedLookAt ( *pPed, vecPosition, iTime, iBlend, pTarget ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedHeadless ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    bool bHeadless = false;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadBool ( bHeadless );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            if ( CStaticFunctionDefinitions::SetPedHeadless ( *pPed, bHeadless ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedFrozen ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    bool bFrozen = false;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadBool ( bFrozen );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            if ( CStaticFunctionDefinitions::SetPedFrozen ( *pPed, bFrozen ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedFootBloodEnabled ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    bool bHasFootBlood = false;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadBool ( bHasFootBlood );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            if ( CStaticFunctionDefinitions::SetPedFootBloodEnabled ( *pPed, bHasFootBlood ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedCameraRotation ( lua_State* luaVM )
{
//  bool setPedCameraRotation ( ped thePed, float cameraRotation )
    CClientEntity* pEntity; float fRotation;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( fRotation );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPedCameraRotation ( *pEntity, fRotation ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedAimTarget ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CVector vecTarget;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadNumber ( vecTarget.fX );
    argStream.ReadNumber ( vecTarget.fY );
    argStream.ReadNumber ( vecTarget.fZ );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            if ( CStaticFunctionDefinitions::SetPedAimTarget ( *pPed, vecTarget ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedRotation ( lua_State* luaVM )
{
//  setPedRotation ( element ped, float rotation [, bool fixPedRotation = false ] )
    CClientEntity* pEntity; float fRotation; bool bNewWay;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( fRotation );
    argStream.ReadBool ( bNewWay, false );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPedRotation ( *pEntity, fRotation, bNewWay ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedCanBeKnockedOffBike ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    bool bCanBeKnockedOffBike = false;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadBool ( bCanBeKnockedOffBike );

    if ( !argStream.HasErrors ( ) )
    {
        // Valid element?
        if ( pPed )
        {
            // Set the new rotation
            if ( CStaticFunctionDefinitions::SetPedCanBeKnockedOffBike ( *pPed, bCanBeKnockedOffBike ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedAnimation ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    SString strBlockName = "";
    SString strAnimName = "";
    int iTime = -1;
    bool bLoop = true;
    bool bUpdatePosition = true;
    bool bInterruptable = true;
    bool bFreezeLastFrame = true;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadString ( strBlockName, "" );
    argStream.ReadString ( strAnimName, "" );
    argStream.ReadNumber ( iTime, -1 );
    argStream.ReadBool ( bLoop, true );
    argStream.ReadBool ( bUpdatePosition, true );
    argStream.ReadBool ( bInterruptable, true );
    argStream.ReadBool ( bFreezeLastFrame, true );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            if ( CStaticFunctionDefinitions::SetPedAnimation ( *pPed, strBlockName == "" ? NULL : strBlockName, strAnimName == "" ? NULL : strAnimName, iTime, bLoop, bUpdatePosition, bInterruptable, bFreezeLastFrame ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetPedAnimationProgress( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    SString strBlockName = "";
    SString strAnimName = "";
    float fProgress = 0.0f;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadString ( strAnimName, "" );
    argStream.ReadNumber ( fProgress, 0.0f );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            const char * szAnimName = strAnimName.c_str ( );
            float fProgress = 0.0f;
            if ( strAnimName != "" ) 
            {
                if ( CStaticFunctionDefinitions::SetPedAnimationProgress ( *pPed, szAnimName, fProgress ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
            {
                if ( CStaticFunctionDefinitions::SetPedAnimationProgress ( *pPed, NULL, fProgress ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedMoveAnim ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    unsigned int uiMoveAnim = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadNumber ( uiMoveAnim );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            if ( CStaticFunctionDefinitions::SetPedMoveAnim ( *pPed, uiMoveAnim ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedOxygenLevel ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    float fOxygen = 0.0f;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadNumber ( fOxygen );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pPed )
        {
            if ( CStaticFunctionDefinitions::SetPedOxygenLevel ( *pPed, fOxygen ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::CreatePed ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    unsigned long ulModel = 0;
    CVector vecPosition;
    float fRotation = 0.0f;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( ulModel );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );
    argStream.ReadNumber ( fRotation, 0.0f );

    if ( !argStream.HasErrors ( ) )
    {
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
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::DetonateSatchels ( lua_State* luaVM )
{
    if ( CStaticFunctionDefinitions::DetonateSatchels() )
    {
        lua_pushboolean( luaVM, true );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetWeaponProperty ( lua_State* luaVM )
{
    eWeaponSkill eWepSkill = eWeaponSkill::WEAPONSKILL_STD;
    eWeaponType eWep = eWeaponType::WEAPONTYPE_UNARMED;
    eWeaponProperty eProp = eWeaponProperty::WEAPON_INVALID_PROPERTY;
    CClientWeapon * pWeapon;
    CScriptArgReader argStream ( luaVM );

    if ( argStream.NextIsUserData () )
    {
        argStream.ReadUserData ( pWeapon );
        argStream.ReadEnumString ( eProp );

        if ( !argStream.HasErrors () )
        {
            if ( eProp == WEAPON_DAMAGE )
            {
                short sData = 0;
                if ( CStaticFunctionDefinitions::GetWeaponProperty ( pWeapon, eProp, sData ) )
                {
                    lua_pushnumber ( luaVM, sData );
                    return 1;
                }
            }
            else
            {
                float fData = 0;
                if ( CStaticFunctionDefinitions::GetWeaponProperty ( pWeapon, eProp, fData ) )
                {
                    lua_pushnumber ( luaVM, fData );
                    return 1;
                }
            }
        }
        else
            m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

        lua_pushboolean ( luaVM, false );
        return 1;
    }

    argStream.ReadEnumStringOrNumber ( eWep );
    argStream.ReadEnumStringOrNumber ( eWepSkill );
    argStream.ReadEnumString ( eProp );
    if ( !argStream.HasErrors () )
    {
        switch ( eProp )
        {
            case WEAPON_WEAPON_RANGE:
            case WEAPON_TARGET_RANGE:
            case WEAPON_ACCURACY:
            case WEAPON_FIRING_SPEED:
            case WEAPON_LIFE_SPAN:
            case WEAPON_SPREAD:
            case WEAPON_MOVE_SPEED:
            // Get only
            case WEAPON_REQ_SKILL_LEVEL:
            case WEAPON_ANIM_LOOP_START:
            case WEAPON_ANIM_LOOP_STOP:
            case WEAPON_ANIM_LOOP_RELEASE_BULLET_TIME:
            case WEAPON_ANIM2_LOOP_START:
            case WEAPON_ANIM2_LOOP_STOP:
            case WEAPON_ANIM2_LOOP_RELEASE_BULLET_TIME:
            case WEAPON_ANIM_BREAKOUT_TIME:
            case WEAPON_RADIUS:
            {
                float fWeaponInfo = 0.0f;

                if ( CStaticFunctionDefinitions::GetWeaponProperty ( eProp, eWep, eWepSkill, fWeaponInfo ) )
                {
                    lua_pushnumber ( luaVM, fWeaponInfo );
                    return 1;
                }
                break;
            }
            case WEAPON_DAMAGE:
            case WEAPON_MAX_CLIP_AMMO:
            case WEAPON_FLAGS:
            case WEAPON_ANIM_GROUP:
            case WEAPON_FIRETYPE:
            case WEAPON_MODEL:
            case WEAPON_MODEL2:
            case WEAPON_SLOT:
            case WEAPON_AIM_OFFSET:
            case WEAPON_SKILL_LEVEL:
            case WEAPON_DEFAULT_COMBO:
            case WEAPON_COMBOS_AVAILABLE:
            {
                short sWeaponInfo = 0;

                if ( CStaticFunctionDefinitions::GetWeaponProperty ( eProp, eWep, eWepSkill, sWeaponInfo ) )
                {
                    lua_pushinteger ( luaVM, sWeaponInfo );
                    return 1;
                }
                break;
            }
            case WEAPON_FIRE_OFFSET:
            {
                CVector vecWeaponInfo;

                if ( CStaticFunctionDefinitions::GetWeaponProperty ( eProp, eWep, eWepSkill, vecWeaponInfo ) )
                {
                    lua_pushnumber ( luaVM, vecWeaponInfo.fX );
                    lua_pushnumber ( luaVM, vecWeaponInfo.fY );
                    lua_pushnumber ( luaVM, vecWeaponInfo.fZ );
                    return 3;
                }
                break;
            }
            default:
            {
                argStream.SetCustomError( "unsupported weapon property at argument 3" );
                break;
            }
        }
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );


    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetOriginalWeaponProperty ( lua_State* luaVM )
{
    eWeaponSkill eWepSkill = eWeaponSkill::WEAPONSKILL_STD;
    eWeaponType eWep = eWeaponType::WEAPONTYPE_UNARMED;
    eWeaponProperty eProp = eWeaponProperty::WEAPON_INVALID_PROPERTY;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadEnumStringOrNumber ( eWep );
    argStream.ReadEnumStringOrNumber ( eWepSkill );
    argStream.ReadEnumString ( eProp );
    if ( !argStream.HasErrors () )
    {
        switch ( eProp )
        {
        case WEAPON_WEAPON_RANGE:
        case WEAPON_TARGET_RANGE:
        case WEAPON_ACCURACY:
        case WEAPON_FIRING_SPEED:
        case WEAPON_LIFE_SPAN:
        case WEAPON_SPREAD:
        case WEAPON_MOVE_SPEED:
            // Get only
        case WEAPON_REQ_SKILL_LEVEL:
        case WEAPON_ANIM_LOOP_START:
        case WEAPON_ANIM_LOOP_STOP:
        case WEAPON_ANIM_LOOP_RELEASE_BULLET_TIME:
        case WEAPON_ANIM2_LOOP_START:
        case WEAPON_ANIM2_LOOP_STOP:
        case WEAPON_ANIM2_LOOP_RELEASE_BULLET_TIME:
        case WEAPON_ANIM_BREAKOUT_TIME:
        case WEAPON_RADIUS:
            {
                float fWeaponInfo = 0.0f;

                if ( CStaticFunctionDefinitions::GetOriginalWeaponProperty ( eProp, eWep, eWepSkill, fWeaponInfo ) )
                {
                    lua_pushnumber ( luaVM, fWeaponInfo );
                    return 1;
                }
                break;
            }
        case WEAPON_DAMAGE:
        case WEAPON_MAX_CLIP_AMMO:
        case WEAPON_FLAGS:
        case WEAPON_ANIM_GROUP:
        case WEAPON_FIRETYPE:
        case WEAPON_MODEL:
        case WEAPON_MODEL2:
        case WEAPON_SLOT:
        case WEAPON_AIM_OFFSET:
        case WEAPON_SKILL_LEVEL:
        case WEAPON_DEFAULT_COMBO:
        case WEAPON_COMBOS_AVAILABLE:
            {
                short sWeaponInfo = 0;

                if ( CStaticFunctionDefinitions::GetOriginalWeaponProperty ( eProp, eWep, eWepSkill, sWeaponInfo ) )
                {
                    lua_pushinteger ( luaVM, sWeaponInfo );
                    return 1;
                }
                break;
            }
        case WEAPON_FIRE_OFFSET:
            {
                CVector vecWeaponInfo;

                if ( CStaticFunctionDefinitions::GetOriginalWeaponProperty ( eProp, eWep, eWepSkill, vecWeaponInfo ) )
                {
                    lua_pushnumber ( luaVM, vecWeaponInfo.fX );
                    lua_pushnumber ( luaVM, vecWeaponInfo.fY );
                    lua_pushnumber ( luaVM, vecWeaponInfo.fZ );
                    return 3;
                }
                break;
            }
        default:
            {
                argStream.SetCustomError( "unsupported weapon property at argument 3" );
                break;
            }
        }
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );


    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}