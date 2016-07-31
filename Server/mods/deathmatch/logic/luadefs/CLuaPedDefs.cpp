/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaPedDefs.cpp
*  PURPOSE:     Lua ped definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaPedDefs::LoadFunctions ( void )
{
    // Ped funcs
    CLuaCFunctions::AddFunction ( "createPed", CreatePed );
    CLuaCFunctions::AddFunction ( "getValidPedModels", GetValidPedModels );

    // Ped get functions
    CLuaCFunctions::AddFunction ( "getPedWeaponSlot", GetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "getPedArmor", GetPedArmor );
    CLuaCFunctions::AddFunction ( "getPedRotation", GetPedRotation );
    CLuaCFunctions::AddFunction ( "isPedChoking", IsPedChoking );
    CLuaCFunctions::AddFunction ( "isPedDead", IsPedDead );
    CLuaCFunctions::AddFunction ( "isPedDucked", IsPedDucked );
    CLuaCFunctions::AddFunction ( "getPedAmmoInClip", GetPedAmmoInClip );
    CLuaCFunctions::AddFunction ( "getPedStat", GetPedStat );
    CLuaCFunctions::AddFunction ( "getPedTarget", GetPedTarget );
    CLuaCFunctions::AddFunction ( "getPedTotalAmmo", GetPedTotalAmmo );
    CLuaCFunctions::AddFunction ( "getPedWeapon", GetPedWeapon );
    CLuaCFunctions::AddFunction ( "getPedClothes", GetPedClothes );
    CLuaCFunctions::AddFunction ( "doesPedHaveJetPack", DoesPedHaveJetPack );
    CLuaCFunctions::AddFunction ( "isPedOnGround", IsPedOnGround );
    CLuaCFunctions::AddFunction ( "getPedFightingStyle", GetPedFightingStyle );
    CLuaCFunctions::AddFunction ( "getPedWalkingStyle", GetPedMoveAnim );
    CLuaCFunctions::AddFunction ( "getPedGravity", GetPedGravity );
    CLuaCFunctions::AddFunction ( "getPedContactElement", GetPedContactElement );
    CLuaCFunctions::AddFunction ( "isPedDoingGangDriveby", IsPedDoingGangDriveby );
    CLuaCFunctions::AddFunction ( "isPedOnFire", IsPedOnFire );
    CLuaCFunctions::AddFunction ( "isPedHeadless", IsPedHeadless );
    CLuaCFunctions::AddFunction ( "isPedFrozen", IsPedFrozen );
    CLuaCFunctions::AddFunction ( "getPedOccupiedVehicle", GetPedOccupiedVehicle );
    CLuaCFunctions::AddFunction ( "getPedOccupiedVehicleSeat", GetPedOccupiedVehicleSeat );
    CLuaCFunctions::AddFunction ( "isPedInVehicle", IsPedInVehicle );

    // Ped set functions
    CLuaCFunctions::AddFunction ( "setPedArmor", SetPedArmor );
    CLuaCFunctions::AddFunction ( "setPedWeaponSlot", SetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "killPed", KillPed );
    CLuaCFunctions::AddFunction ( "setPedRotation", SetPedRotation );
    CLuaCFunctions::AddFunction ( "setPedStat", SetPedStat );
    CLuaCFunctions::AddFunction ( "addPedClothes", AddPedClothes );
    CLuaCFunctions::AddFunction ( "removePedClothes", RemovePedClothes );
    CLuaCFunctions::AddFunction ( "givePedJetPack", GivePedJetPack );
    CLuaCFunctions::AddFunction ( "removePedJetPack", RemovePedJetPack );
    CLuaCFunctions::AddFunction ( "setPedFightingStyle", SetPedFightingStyle );
    CLuaCFunctions::AddFunction ( "setPedWalkingStyle", SetPedMoveAnim );
    CLuaCFunctions::AddFunction ( "setPedGravity", SetPedGravity );
    CLuaCFunctions::AddFunction ( "setPedChoking", SetPedChoking );
    CLuaCFunctions::AddFunction ( "warpPedIntoVehicle", WarpPedIntoVehicle );
    CLuaCFunctions::AddFunction ( "removePedFromVehicle", RemovePedFromVehicle );
    CLuaCFunctions::AddFunction ( "setPedDoingGangDriveby", SetPedDoingGangDriveby );
    CLuaCFunctions::AddFunction ( "setPedAnimation", SetPedAnimation );
    CLuaCFunctions::AddFunction ( "setPedAnimationProgress", SetPedAnimationProgress );
    CLuaCFunctions::AddFunction ( "setPedOnFire", SetPedOnFire );
    CLuaCFunctions::AddFunction ( "setPedHeadless", SetPedHeadless );
    CLuaCFunctions::AddFunction ( "setPedFrozen", SetPedFrozen );
    CLuaCFunctions::AddFunction ( "reloadPedWeapon", reloadPedWeapon );

    // Weapon give/take functions
    CLuaCFunctions::AddFunction ( "giveWeapon", GiveWeapon );
    CLuaCFunctions::AddFunction ( "takeWeapon", TakeWeapon );
    CLuaCFunctions::AddFunction ( "takeAllWeapons", TakeAllWeapons );
    CLuaCFunctions::AddFunction ( "giveWeaponAmmo", GiveWeapon );
    CLuaCFunctions::AddFunction ( "takeWeaponAmmo", TakeWeapon );
}


// TODO: specials
void CLuaPedDefs::AddClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "getValidModels", "getValidPedModels" );

    lua_classfunction ( luaVM, "create", "createPed" );
    lua_classfunction ( luaVM, "kill", "killPed" );
    lua_classfunction ( luaVM, "warpIntoVehicle", "warpPedIntoVehicle" );
    lua_classfunction ( luaVM, "addClothes", "addPedClothes" );
    lua_classfunction ( luaVM, "giveJetPack", "givePedJetPack" );
    lua_classfunction ( luaVM, "giveWeapon", "giveWeapon" );
    lua_classfunction ( luaVM, "takeWeapon", "takeWeapon" );
    lua_classfunction ( luaVM, "takeAllWeapons", "takeAllWeapons" );
    lua_classfunction ( luaVM, "reloadWeapon", "reloadPedWeapon" );
    lua_classfunction ( luaVM, "removeClothes", "removePedClothes" );
    lua_classfunction ( luaVM, "removeFromVehicle", "removePedFromVehicle" );
    lua_classfunction ( luaVM, "removeJetPack", "removePedJetPack" );
    lua_classfunction ( luaVM, "doesHaveJetpack", "doesPedHaveJetPack" );

    lua_classfunction ( luaVM, "isDead", "isPedDead" );
    lua_classfunction ( luaVM, "isDucked", "isPedDucked" );
    lua_classfunction ( luaVM, "isInWater", "isPedInWater" );
    lua_classfunction ( luaVM, "isOnGround", "isPedOnGround" );
    lua_classfunction ( luaVM, "isInVehicle", "isPedInVehicle" );

    lua_classfunction ( luaVM, "isOnFire", "isPedOnFire" );
    lua_classfunction ( luaVM, "isChoking", "isPedChoking" );
    lua_classfunction ( luaVM, "isDoingGangDriveby", "isPedDoingGangDriveby" );
    lua_classfunction ( luaVM, "isFrozen", "isPedFrozen" );
    lua_classfunction ( luaVM, "isHeadless", "isPedHeadless" );

    lua_classfunction ( luaVM, "getArmor", "getPedArmor" );
    lua_classfunction ( luaVM, "getFightingStyle", "getPedFightingStyle" );
    lua_classfunction ( luaVM, "getGravity", "getPedGravity" );
    lua_classfunction ( luaVM, "getStat", "getPedStat" );
    lua_classfunction ( luaVM, "getWeaponSlot", "getPedWeaponSlot" );
    lua_classfunction ( luaVM, "getWalkingStyle", "getPedWalkingStyle" );

    lua_classfunction ( luaVM, "getAmmoInClip", "getPedAmmoInClip" );
    lua_classfunction ( luaVM, "getOccupiedVehicle", "getPedOccupiedVehicle" );
    lua_classfunction ( luaVM, "getWeapon", "getPedWeapon" );
    lua_classfunction ( luaVM, "getTarget", "getPedTarget" );
    lua_classfunction ( luaVM, "getOccupiedVehicleSeat", "getPedOccupiedVehicleSeat" );
    lua_classfunction ( luaVM, "getClothes", "getPedClothes" );
    lua_classfunction ( luaVM, "getContactElement", "getPedContactElement" );
    lua_classfunction ( luaVM, "getTotalAmmo", "getPedTotalAmmo" );

    lua_classfunction ( luaVM, "setOnFire", "setPedOnFire" );
    lua_classfunction ( luaVM, "setChoking", "setPedChoking" );
    lua_classfunction ( luaVM, "setDoingGangDriveBy", "setPedDoingGangDriveby" );
    lua_classfunction ( luaVM, "setFrozen", "setPedFrozen" );
    lua_classfunction ( luaVM, "setHeadless", "setPedHeadless" );
    lua_classfunction ( luaVM, "setArmor", "setPedArmor" );
    lua_classfunction ( luaVM, "setFightingStyle", "setPedFightingStyle" );
    lua_classfunction ( luaVM, "setGravity", "setPedGravity" );
    lua_classfunction ( luaVM, "setStat", "setPedStat" );
    lua_classfunction ( luaVM, "setWeaponSlot", "setPedWeaponSlot" );
    lua_classfunction ( luaVM, "setWalkingStyle", "setPedWalkingStyle" );

    lua_classfunction ( luaVM, "setAnimation", "setPedAnimation" );
    lua_classfunction ( luaVM, "setAnimationProgress", "setPedAnimationProgress" );

    lua_classvariable ( luaVM, "inVehicle", NULL, "isPedInVehicle" );
    lua_classvariable ( luaVM, "ducked", NULL, "isPedDucked" );
    lua_classvariable ( luaVM, "inWater", NULL, "isPedInWater" );
    lua_classvariable ( luaVM, "onGround", NULL, "isPedOnGround" );
    lua_classvariable ( luaVM, "onFire", "setPedOnFire", "isPedOnFire" );
    lua_classvariable ( luaVM, "choking", "setPedChoking", "isPedChoking" );
    lua_classvariable ( luaVM, "doingGangDriveby", "setPedDoingGangDriveby", "isPedDoingGangDriveby" );
    lua_classvariable ( luaVM, "frozen", "setPedFrozen", "isPedFrozen" );
    lua_classvariable ( luaVM, "headless", "setPedHeadless", "isPedHeadless" );
    lua_classvariable ( luaVM, "armor", "setPedArmor", "getPedArmor" );
    lua_classvariable ( luaVM, "fightingStyle", "setPedFightingStyle", "getPedFightingStyle" );
    lua_classvariable ( luaVM, "gravity", "setPedGravity", "getPedGravity" );
    lua_classvariable ( luaVM, "weaponSlot", "setPedWeaponSlot", "getPedWeaponSlot" );
    lua_classvariable ( luaVM, "ammoInClipOfCurrentWeaponSlot", NULL, "getPedAmmoInClip" );
    lua_classvariable ( luaVM, "weaponInCurrentWeaponSlot", NULL, "getPedWeapon" );
    lua_classvariable ( luaVM, "target", NULL, "getPedTarget" );
    lua_classvariable ( luaVM, "vehicleSeat", NULL, "getPedOccupiedVehicleSeat" );
    lua_classvariable ( luaVM, "contactElement", NULL, "getPedContactElement" );
    lua_classvariable ( luaVM, "totalAmmoInWeaponOfCurrentWeaponSlot", NULL, "getPedTotalAmmo" );
    lua_classvariable ( luaVM, "vehicle", "warpPedIntoVehicle", "getPedOccupiedVehicle", OOP_WarpPedIntoVehicle, GetPedOccupiedVehicle ); // what about removePedFromVehicle?
    lua_classvariable ( luaVM, "dead", "killPed", "isPedDead" ); // Setting this to any value will still kill the ped. Should we special case this?
    lua_classvariable ( luaVM, "walkingStyle", "setPedWalkingStyle", "getPedWalkingStyle" );
    //lua_classvariable ( luaVM, "stats", "setPedStat", "getPedStat", CLuaOOPDefs::SetPedStat, CLuaOOPDefs::GetPedStat ); // table
    //lua_classvariable ( luaVM, "controlState", "setPedControlState", "getPedControlState", CLuaOOPDefs::SetPedControlState, CLuaOOPDefs::GetPedControlState ); // TODO: .controlState["control"] = value
    // This commented code will syntax error, a new method has to be implemented to support this
    //lua_classvariable ( luaVM, "jetpack", {"removePedJetPack","givePedJetPack}, "doesPedHaveJetPack", CLuaOOPDefs::SetJetpackActive, "doesPedHaveJetPack" ); // very specialised!

    lua_registerclass ( luaVM, "Ped", "Element" );
}



int CLuaPedDefs::GetValidPedModels ( lua_State* luaVM )
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

int CLuaPedDefs::CreatePed ( lua_State* luaVM )
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




int CLuaPedDefs::GetPedWeapon ( lua_State* luaVM )
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


int CLuaPedDefs::GetPedWeaponSlot ( lua_State* luaVM )
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

int CLuaPedDefs::reloadPedWeapon ( lua_State* luaVM )
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

int CLuaPedDefs::IsPedDoingGangDriveby ( lua_State* luaVM )
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


int CLuaPedDefs::IsPedOnFire ( lua_State* luaVM )
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


int CLuaPedDefs::IsPedHeadless ( lua_State* luaVM )
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


int CLuaPedDefs::IsPedFrozen ( lua_State* luaVM )
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


int CLuaPedDefs::SetPedAnimation ( lua_State* luaVM )
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

int CLuaPedDefs::SetPedAnimationProgress ( lua_State* luaVM )
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



int CLuaPedDefs::SetPedWeaponSlot ( lua_State* luaVM )
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



int CLuaPedDefs::SetPedOnFire ( lua_State* luaVM )
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



int CLuaPedDefs::SetPedHeadless ( lua_State* luaVM )
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


int CLuaPedDefs::SetPedFrozen ( lua_State* luaVM )
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

int CLuaPedDefs::GetPedAmmoInClip ( lua_State* luaVM )
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


int CLuaPedDefs::GetPedTotalAmmo ( lua_State* luaVM )
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


int CLuaPedDefs::GetPedArmor ( lua_State* luaVM )
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



int CLuaPedDefs::GetPedOccupiedVehicle ( lua_State* luaVM )
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


int CLuaPedDefs::GetPedOccupiedVehicleSeat ( lua_State* luaVM )
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

int CLuaPedDefs::GetPedRotation ( lua_State* luaVM )
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




int CLuaPedDefs::IsPedChoking ( lua_State* luaVM )
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


int CLuaPedDefs::IsPedDead ( lua_State* luaVM )
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


int CLuaPedDefs::IsPedDucked ( lua_State* luaVM )
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



int CLuaPedDefs::IsPedInVehicle ( lua_State* luaVM )
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


int CLuaPedDefs::GetPedStat ( lua_State* luaVM )
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


int CLuaPedDefs::GetPedTarget ( lua_State* luaVM )
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




int CLuaPedDefs::GetPedClothes ( lua_State* luaVM )
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


int CLuaPedDefs::DoesPedHaveJetPack ( lua_State* luaVM )
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


int CLuaPedDefs::IsPedOnGround ( lua_State* luaVM )
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





int CLuaPedDefs::GetPedFightingStyle ( lua_State* luaVM )
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


int CLuaPedDefs::GetPedMoveAnim ( lua_State* luaVM )
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


int CLuaPedDefs::GetPedGravity ( lua_State* luaVM )
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



int CLuaPedDefs::GetPedContactElement ( lua_State* luaVM )
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


int CLuaPedDefs::SetPedArmor ( lua_State* luaVM )
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


int CLuaPedDefs::KillPed ( lua_State* luaVM )
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


int CLuaPedDefs::SetPedRotation ( lua_State* luaVM )
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


int CLuaPedDefs::SetPedStat ( lua_State* luaVM )
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

int CLuaPedDefs::AddPedClothes ( lua_State* luaVM )
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

int CLuaPedDefs::RemovePedClothes ( lua_State* luaVM )
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

int CLuaPedDefs::GivePedJetPack ( lua_State* luaVM )
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

int CLuaPedDefs::RemovePedJetPack ( lua_State* luaVM )
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

int CLuaPedDefs::SetPedFightingStyle ( lua_State* luaVM )
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


int CLuaPedDefs::SetPedMoveAnim ( lua_State* luaVM )
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

int CLuaPedDefs::SetPedGravity ( lua_State* luaVM )
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



int CLuaPedDefs::SetPedChoking ( lua_State* luaVM )
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


int CLuaPedDefs::WarpPedIntoVehicle ( lua_State* luaVM )
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


#define MIN_CLIENT_REQ_REMOVEPEDFROMVEHICLE_CLIENTSIDE  "1.3.0-9.04482"
#define MIN_CLIENT_REQ_WARPPEDINTOVEHICLE_CLIENTSIDE    "1.3.0-9.04482"
int CLuaPedDefs::OOP_WarpPedIntoVehicle ( lua_State* luaVM )
{
    //  ped.vehicle = element vehicle
    //  ped.vehicle = nil
    CPed* pPed; CVehicle* pVehicle; uint uiSeat = 0;

    CScriptArgReader argStream ( luaVM );

    argStream.ReadUserData ( pPed );
    argStream.ReadUserData ( pVehicle, NULL );

    if ( pVehicle != NULL )
    {
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
    }
    else
    {
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
    }

    lua_pushboolean ( luaVM, false );

    return 1;
}


int CLuaPedDefs::RemovePedFromVehicle ( lua_State* luaVM )
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

int CLuaPedDefs::SetPedDoingGangDriveby ( lua_State* luaVM )
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


int CLuaPedDefs::GiveWeapon ( lua_State* luaVM )
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


int CLuaPedDefs::TakeWeapon ( lua_State* luaVM )
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

int CLuaPedDefs::TakeAllWeapons ( lua_State* luaVM )
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