/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefs.Ped.cpp
*  PURPOSE:     Lua function definitions
*  DEVELOPERS:  Kent Simon <>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*               Ed Lyons <>
*               Oliver Brown <>
*               Jax <>
*               Chris McArthur <>
*               Kevin Whiteside <>
*               lil_Toady <>
*               Alberto Alonso <rydencillo@gmail.com>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

int CLuaFunctionDefs::GetValidPedModels ( lua_State* luaVM )
{
    int iIndex = 0;
    lua_newtable ( luaVM );
    for ( int i = 0; i <= 312; i++ )
    {
        if ( CPlayerManager::IsValidPlayerModel ( i ) )
        {
            lua_pushnumber ( luaVM, ++iIndex );
            lua_pushnumber ( luaVM, i );
            lua_settable ( luaVM, -3 );
        }
    }

    return 1;
}

int CLuaFunctionDefs::CreatePed ( lua_State* luaVM )
{
    unsigned short usModel;
    CVector vecPosition;
    float fRotation;
    bool bSynced;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( usModel );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadNumber ( fRotation, 0.0f );
    argStream.ReadBool ( bSynced, true );

    if ( !argStream.HasErrors () )
    {
        CLuaMain * pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource * pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                // Create the ped and return its handle
                CPed* pPed = CStaticFunctionDefinitions::CreatePed ( pResource, usModel, vecPosition, fRotation, bSynced );
                if ( pPed )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup ();
                    if ( pGroup )
                    {
                        pGroup->Add ( pPed );
                    }
                    lua_pushelement ( luaVM, pPed );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}




int CLuaFunctionDefs::GetPedWeapon ( lua_State* luaVM )
{
    CPed* pPed;
    unsigned char ucSlot;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadNumber ( ucSlot, 0xFF );

    if ( !argStream.HasErrors () )
    {
        if ( ucSlot == 0xFF )
            ucSlot = pPed->GetWeaponSlot ();

        CWeapon* pWeapon = pPed->GetWeapon ( ucSlot );
        if ( pWeapon )
        {
            unsigned char ucWeapon = pWeapon->ucType;
            lua_pushnumber ( luaVM, ucWeapon );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedWeaponSlot ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        unsigned char ucSlot;
        if ( CStaticFunctionDefinitions::GetPedWeaponSlot ( pPed, ucSlot ) )
        {
            lua_pushnumber ( luaVM, ucSlot );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::reloadPedWeapon ( lua_State* luaVM )
{
    CElement* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::reloadPedWeapon ( pPed ) )
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

int CLuaFunctionDefs::IsPedDoingGangDriveby ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        bool bDriveby;
        if ( CStaticFunctionDefinitions::IsPedDoingGangDriveby ( pPed, bDriveby ) )
        {
            lua_pushboolean ( luaVM, bDriveby );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsPedOnFire ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        bool bOnFire;
        if ( CStaticFunctionDefinitions::IsPedOnFire ( pPed, bOnFire ) )
        {
            lua_pushboolean ( luaVM, bOnFire );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsPedHeadless ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        bool bHeadless;
        if ( CStaticFunctionDefinitions::IsPedHeadless ( pPed, bHeadless ) )
        {
            lua_pushboolean ( luaVM, bHeadless );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsPedFrozen ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        bool bFrozen;
        if ( CStaticFunctionDefinitions::IsPedFrozen ( pPed, bFrozen ) )
        {
            lua_pushboolean ( luaVM, bFrozen );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedAnimation ( lua_State* luaVM )
{
    // bool setPedAnimation ( ped thePed [, string block=nil, string anim=nil, int time=-1, bool loop=true, bool updatePosition=true, bool interruptable=true, bool freezeLastFrame = true] )
    CElement * pPed;
    SString strBlockName, strAnimName;
    int iTime;
    bool bLoop, bUpdatePosition, bInterruptable, bFreezeLastFrame;
    bool bDummy;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    if ( argStream.NextIsBool () )
        argStream.ReadBool ( bDummy );      // Wiki used setPedAnimation(source,false) as an example
    else
        if ( argStream.NextIsNil () )
            argStream.m_iIndex++;               // Wiki docs said blockName could be nil
        else
            argStream.ReadString ( strBlockName, "" );
    argStream.ReadString ( strAnimName, "" );
    if ( argStream.NextCouldBeNumber () )    // Freeroam skips the time arg sometimes
        argStream.ReadNumber ( iTime, -1 );
    else
        iTime = -1;
    argStream.ReadBool ( bLoop, true );
    argStream.ReadBool ( bUpdatePosition, true );
    argStream.ReadBool ( bInterruptable, true );
    argStream.ReadBool ( bFreezeLastFrame, true );

    if ( !argStream.HasErrors () )
    {
        const char *szBlock, *szAnim;
        szBlock = strBlockName.empty () ? NULL : strBlockName.c_str ();
        szAnim = strAnimName.empty () ? NULL : strAnimName.c_str ();

        if ( CStaticFunctionDefinitions::SetPedAnimation ( pPed, szBlock, szAnim, iTime, bLoop, bUpdatePosition, bInterruptable, bFreezeLastFrame ) )
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

int CLuaFunctionDefs::SetPedAnimationProgress ( lua_State* luaVM )
{
    CElement* pElement;
    SString strAnimName;
    float fProgress;


    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadString ( strAnimName, "" );
    argStream.ReadNumber ( fProgress, 0.0f );

    if ( !argStream.HasErrors () )
    {
        const char *szAnim;
        szAnim = strAnimName.empty () ? NULL : strAnimName.c_str ();

        if ( CStaticFunctionDefinitions::SetPedAnimationProgress ( pElement, szAnim, fProgress ) )
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



int CLuaFunctionDefs::SetPedWeaponSlot ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucSlot;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ucSlot );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPedWeaponSlot ( pElement, ucSlot ) )
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



int CLuaFunctionDefs::SetPedOnFire ( lua_State* luaVM )
{
    CElement* pElement;
    bool bOnFire;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bOnFire );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPedOnFire ( pElement, bOnFire ) )
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



int CLuaFunctionDefs::SetPedHeadless ( lua_State* luaVM )
{
    CElement* pElement;
    bool bHeadless;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bHeadless );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPedHeadless ( pElement, bHeadless ) )
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


int CLuaFunctionDefs::SetPedFrozen ( lua_State* luaVM )
{
    CElement* pElement;
    bool bFrozen;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bFrozen );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPedFrozen ( pElement, bFrozen ) )
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

int CLuaFunctionDefs::GetPedAmmoInClip ( lua_State* luaVM )
{
    CPed* pPed;
    unsigned char ucSlot = 0;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
        argStream.ReadNumber ( ucSlot, pPed->GetWeaponSlot () );

    if ( !argStream.HasErrors () )
    {
        CWeapon* pWeapon = pPed->GetWeapon ( ucSlot );
        if ( pWeapon )
        {
            lua_pushnumber ( luaVM, pWeapon->usAmmoInClip );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );


    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedTotalAmmo ( lua_State* luaVM )
{
    CPed* pPed;
    unsigned char ucSlot = 0;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
        argStream.ReadNumber ( ucSlot, pPed->GetWeaponSlot () );

    if ( !argStream.HasErrors () )
    {
        CWeapon* pWeapon = pPed->GetWeapon ( ucSlot );
        if ( pWeapon )
        {
            lua_pushnumber ( luaVM, pWeapon->usAmmo );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );


    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedArmor ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        float fArmor;
        if ( CStaticFunctionDefinitions::GetPedArmor ( pPed, fArmor ) )
        {
            lua_pushnumber ( luaVM, fArmor );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefs::GetPedOccupiedVehicle ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        CVehicle* pVehicle = CStaticFunctionDefinitions::GetPedOccupiedVehicle ( pPed );
        if ( pVehicle )
        {
            lua_pushelement ( luaVM, pVehicle );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedOccupiedVehicleSeat ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        unsigned int uiVehicleSeat;
        if ( CStaticFunctionDefinitions::GetPedOccupiedVehicleSeat ( pPed, uiVehicleSeat ) )
        {
            lua_pushnumber ( luaVM, uiVehicleSeat );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetPedRotation ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        float fRotation;
        if ( CStaticFunctionDefinitions::GetPedRotation ( pPed, fRotation ) )
        {
            lua_pushnumber ( luaVM, fRotation );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}




int CLuaFunctionDefs::IsPedChoking ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        lua_pushboolean ( luaVM, pPed->IsChoking () );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsPedDead ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        lua_pushboolean ( luaVM, pPed->IsDead () );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsPedDucked ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        lua_pushboolean ( luaVM, pPed->IsDucked () );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefs::IsPedInVehicle ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        lua_pushboolean ( luaVM, CStaticFunctionDefinitions::GetPedOccupiedVehicle ( pPed ) != NULL );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedStat ( lua_State* luaVM )
{
    CPed* pPed;
    unsigned short usStat;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadNumber ( usStat );

    if ( !argStream.HasErrors () )
    {
        float fValue;
        if ( CStaticFunctionDefinitions::GetPedStat ( pPed, usStat, fValue ) )
        {
            lua_pushnumber ( luaVM, fValue );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedTarget ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        CElement *pTarget = CStaticFunctionDefinitions::GetPedTarget ( pPed );
        if ( pTarget )
        {
            lua_pushelement ( luaVM, pTarget );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}




int CLuaFunctionDefs::GetPedClothes ( lua_State* luaVM )
{
    CPed* pPed;
    unsigned char ucType;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadNumber ( ucType );

    if ( !argStream.HasErrors () )
    {
        SString strTexture, strModel;
        if ( CStaticFunctionDefinitions::GetPedClothes ( pPed, ucType, strTexture, strModel ) )
        {
            lua_pushstring ( luaVM, strTexture );
            lua_pushstring ( luaVM, strModel );
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::DoesPedHaveJetPack ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        bool bHasJetPack;
        if ( CStaticFunctionDefinitions::DoesPedHaveJetPack ( pPed, bHasJetPack ) )
        {
            lua_pushboolean ( luaVM, bHasJetPack );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsPedOnGround ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        bool bOnGround;
        if ( CStaticFunctionDefinitions::IsPedOnGround ( pPed, bOnGround ) )
        {
            lua_pushboolean ( luaVM, bOnGround );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}





int CLuaFunctionDefs::GetPedFightingStyle ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        unsigned char ucStyle;
        if ( CStaticFunctionDefinitions::GetPedFightingStyle ( pPed, ucStyle ) )
        {
            lua_pushnumber ( luaVM, ucStyle );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedMoveAnim ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        unsigned int uiMoveAnim;
        if ( CStaticFunctionDefinitions::GetPedMoveAnim ( pPed, uiMoveAnim ) )
        {
            lua_pushnumber ( luaVM, uiMoveAnim );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetPedGravity ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        float fGravity;
        if ( CStaticFunctionDefinitions::GetPedGravity ( pPed, fGravity ) )
        {
            lua_pushnumber ( luaVM, fGravity );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefs::GetPedContactElement ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        CElement* pContactElement = CStaticFunctionDefinitions::GetPedContactElement ( pPed );
        if ( pContactElement )
        {
            lua_pushelement ( luaVM, pContactElement );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetPedArmor ( lua_State* luaVM )
{
    CElement* pElement;
    float fArmor;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( fArmor );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPedArmor ( pElement, fArmor ) )
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


int CLuaFunctionDefs::KillPed ( lua_State* luaVM )
{
    CElement* pElement, *pKiller;
    unsigned char ucKillerWeapon;
    unsigned char ucBodyPart;
    bool bStealth;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadUserData ( pKiller, NULL );
    argStream.ReadNumber ( ucKillerWeapon, 0xFF );
    argStream.ReadNumber ( ucBodyPart, 0xFF );
    argStream.ReadBool ( bStealth, false );


    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::KillPed ( pElement, pKiller, ucKillerWeapon, ucBodyPart, bStealth ) )
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


int CLuaFunctionDefs::SetPedRotation ( lua_State* luaVM )
{
    //  setPedRotation ( element ped, float rotation [, bool fixPedRotation = false ] )
    CElement* pElement; float fRotation; bool bNewWay;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( fRotation );
    argStream.ReadBool ( bNewWay, false );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPedRotation ( pElement, fRotation, bNewWay ) )
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


int CLuaFunctionDefs::SetPedStat ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned short usStat;
    float fValue;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( usStat );
    argStream.ReadNumber ( fValue );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPedStat ( pElement, usStat, fValue ) )
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

int CLuaFunctionDefs::AddPedClothes ( lua_State* luaVM )
{
    CElement* pElement;
    SString strTexture;
    SString strModel;
    unsigned char ucType;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadString ( strTexture );
    argStream.ReadString ( strModel );
    argStream.ReadNumber ( ucType );


    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::AddPedClothes ( pElement, strTexture, strModel, ucType ) )
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

int CLuaFunctionDefs::RemovePedClothes ( lua_State* luaVM )
{
    //  bool removePedClothes ( ped thePed, int clothesType, [ string clothesTexture, string clothesModel ] )
    CElement* pElement;
    unsigned char ucType;
    SString strTexture;
    SString strModel;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ucType );
    argStream.ReadString ( strTexture, "" );
    argStream.ReadString ( strModel, "" );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::RemovePedClothes ( pElement, ucType, strTexture.empty () ? NULL : strTexture.c_str (), strModel.empty () ? NULL : strModel.c_str () ) )
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

int CLuaFunctionDefs::GivePedJetPack ( lua_State* luaVM )
{
    CElement* pElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::GivePedJetPack ( pElement ) )
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

int CLuaFunctionDefs::RemovePedJetPack ( lua_State* luaVM )
{
    CElement* pElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::RemovePedJetPack ( pElement ) )
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

int CLuaFunctionDefs::SetPedFightingStyle ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucStyle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ucStyle );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPedFightingStyle ( pElement, ucStyle ) )
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


int CLuaFunctionDefs::SetPedMoveAnim ( lua_State* luaVM )
{
    CElement* pElement;
    int iMoveAnim;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( iMoveAnim );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPedMoveAnim ( pElement, iMoveAnim ) )
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

int CLuaFunctionDefs::SetPedGravity ( lua_State* luaVM )
{
    CElement* pElement;
    float fGravity;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( fGravity );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPedGravity ( pElement, fGravity ) )
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



int CLuaFunctionDefs::SetPedChoking ( lua_State* luaVM )
{
    CElement* pElement;
    bool bChoking;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bChoking );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPedChoking ( pElement, bChoking ) )
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


int CLuaFunctionDefs::WarpPedIntoVehicle ( lua_State* luaVM )
{
    CPed* pPed;
    CVehicle* pVehicle;
    unsigned int uiSeat;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( uiSeat, 0 );

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
    CElement* pElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::RemovePedFromVehicle ( pElement ) )
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

int CLuaFunctionDefs::SetPedDoingGangDriveby ( lua_State* luaVM )
{
    CElement* pElement;
    bool bDoingGangDriveby;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bDoingGangDriveby );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPedDoingGangDriveby ( pElement, bDoingGangDriveby ) )
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


int CLuaFunctionDefs::GiveWeapon ( lua_State* luaVM )
{
    // bool giveWeapon ( ped thePlayer, int weapon [, int ammo=30, bool setAsCurrent=false ] )
    CElement* pElement; eWeaponType weaponType; ushort usAmmo; bool bSetAsCurrent;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadEnumStringOrNumber ( weaponType );
    argStream.ReadNumber ( usAmmo, 30 );
    argStream.ReadBool ( bSetAsCurrent, false );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::GiveWeapon ( pElement, weaponType, usAmmo, bSetAsCurrent ) )
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


int CLuaFunctionDefs::TakeWeapon ( lua_State* luaVM )
{
    // bool takeWeapon ( player thePlayer, int weaponId [, int ammo ] )
    CElement* pElement; eWeaponType weaponType; ushort usAmmo;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadEnumStringOrNumber ( weaponType );
    argStream.ReadNumber ( usAmmo, 9999 );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::TakeWeapon ( pElement, weaponType, usAmmo ) )
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

int CLuaFunctionDefs::TakeAllWeapons ( lua_State* luaVM )
{
    CElement* pElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::TakeAllWeapons ( pElement ) )
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

int CLuaFunctionDefs::SetWeaponAmmo ( lua_State* luaVM )
{
    // bool setWeaponAmmo ( player thePlayer, int weapon, int totalAmmo, [int ammoInClip = 0] )
    CElement* pElement;
    eWeaponType weaponType;
    ushort usAmmo;
    ushort usAmmoInClip;
    CCustomWeapon * pWeapon = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        if ( pElement->GetType () != CElement::WEAPON )
        {
            argStream.ReadEnumStringOrNumber ( weaponType );
            argStream.ReadNumber ( usAmmo );
            argStream.ReadNumber ( usAmmoInClip, 0 );

            if ( !argStream.HasErrors () )
            {
                if ( CStaticFunctionDefinitions::SetWeaponAmmo ( pElement, weaponType, usAmmo, usAmmoInClip ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );
        }
        else
        {
            pWeapon = static_cast <CCustomWeapon *> ( pElement );
            argStream.ReadNumber ( usAmmo );

            if ( !argStream.HasErrors () )
            {
                if ( CStaticFunctionDefinitions::SetWeaponAmmo ( pWeapon, usAmmo ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}
