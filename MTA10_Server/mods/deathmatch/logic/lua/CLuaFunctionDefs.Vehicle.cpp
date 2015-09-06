/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefs.Vehicle.cpp
*  PURPOSE:     Lua function definitions class
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


int CLuaFunctionDefs::CreateVehicle ( lua_State* luaVM )
{
    //  vehicle createVehicle ( int model, float x, float y, float z, [float rx, float ry, float rz, string numberplate, bool bDirection, int variant1, int variant2] )
    ushort usModel; CVector vecPosition; CVector vecRotation; SString strNumberPlate; uchar ucVariant; uchar ucVariant2;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( usModel );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadVector3D ( vecRotation, CVector () );
    argStream.ReadString ( strNumberPlate, "" );
    if ( argStream.NextIsBool () )
    {
        bool bDirection;
        argStream.ReadBool ( bDirection );
    }
    argStream.ReadNumber ( ucVariant, 254 );
    argStream.ReadNumber ( ucVariant2, 254 );

    if ( !argStream.HasErrors () )
    {
        CLuaMain * pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource * pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                //if ( usModel != 570 || m_pResourceManager->GetMinClientRequirement () > "1.3.2-xx" ) // Todo: On merge: Please insert the revision
                {
                    // Create the vehicle and return its handle
                    CVehicle* pVehicle = CStaticFunctionDefinitions::CreateVehicle ( pResource, usModel, vecPosition, vecRotation, strNumberPlate, ucVariant, ucVariant2 );
                    if ( pVehicle )
                    {
                        CElementGroup * pGroup = pResource->GetElementGroup ();
                        if ( pGroup )
                        {
                            pGroup->Add ( pVehicle );
                        }
                        lua_pushelement ( luaVM, pVehicle );
                        return 1;
                    }
                }
                /*else
                m_pScriptDebugging->LogCustom ( luaVM, "Please set min_mta_version to xxx" ); // Todo*/
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleType ( lua_State* luaVM )
{
    unsigned long ulModel;

    CScriptArgReader argStream ( luaVM );

    if ( !argStream.NextIsNumber () )
    {
        CVehicle* pVehicle;
        argStream.ReadUserData ( pVehicle );

        if ( !argStream.HasErrors () )
        {
            ulModel = pVehicle->GetModel ();
        }
    }
    else
        argStream.ReadNumber ( ulModel );

    if ( !argStream.HasErrors () )
    {
        if ( ulModel >= 400 && ulModel < 610 )
        {
            lua_pushstring ( luaVM, CVehicleNames::GetVehicleTypeName ( ulModel ) );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetVehicleVariant ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    unsigned char ucVariant;
    unsigned char ucVariant2;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( ucVariant, 0xFE );
    argStream.ReadNumber ( ucVariant2, 0xFE );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleVariant ( pVehicle, ucVariant, ucVariant2 ) )
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

int CLuaFunctionDefs::GetVehicleVariant ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        unsigned char ucVariant = -1;
        unsigned char ucVariant2 = -1;
        if ( CStaticFunctionDefinitions::GetVehicleVariant ( pVehicle, ucVariant, ucVariant2 ) )
        {
            lua_pushnumber ( luaVM, ucVariant );
            lua_pushnumber ( luaVM, ucVariant2 );
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleColor ( lua_State* luaVM )
{
    //  getVehicleColor ( vehicle theVehicle[, bool bRGB ] )
    CVehicle* pVehicle;
    bool bRGB;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadBool ( bRGB, false );

    if ( !argStream.HasErrors () )
    {
        CVehicleColor color;
        if ( CStaticFunctionDefinitions::GetVehicleColor ( pVehicle, color ) )
        {

            if ( bRGB )
            {
                for ( uint i = 0; i < 4; i++ )
                {
                    SColor RGBColor = color.GetRGBColor ( i );
                    lua_pushnumber ( luaVM, RGBColor.R );
                    lua_pushnumber ( luaVM, RGBColor.G );
                    lua_pushnumber ( luaVM, RGBColor.B );
                }
                return 12;
            }
            else
            {
                lua_pushnumber ( luaVM, color.GetPaletteColor ( 0 ) );
                lua_pushnumber ( luaVM, color.GetPaletteColor ( 1 ) );
                lua_pushnumber ( luaVM, color.GetPaletteColor ( 2 ) );
                lua_pushnumber ( luaVM, color.GetPaletteColor ( 3 ) );
                return 4;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleModelFromName ( lua_State* luaVM )
{
    SString strName;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strName );

    if ( !argStream.HasErrors () )
    {
        unsigned short usModel;
        if ( CStaticFunctionDefinitions::GetVehicleModelFromName ( strName, usModel ) )
        {
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( usModel ) );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleLandingGearDown ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        if ( CVehicleManager::HasLandingGears ( pVehicle->GetModel () ) )
        {
            lua_pushboolean ( luaVM, pVehicle->IsLandingGearDown () );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetVehicleLocked ( lua_State* luaVM )
{
    CElement* pElement;
    bool bLocked;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bLocked );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleLocked ( pElement, bLocked ) )
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

int CLuaFunctionDefs::SetVehicleDoorsUndamageable ( lua_State* luaVM )
{
    CElement* pElement;
    bool bDoorsUndamageable;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bDoorsUndamageable );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleDoorsUndamageable ( pElement, bDoorsUndamageable ) )
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

int CLuaFunctionDefs::RemoveVehicleSirens ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    CVehicle* pVehicle = NULL;

    argStream.ReadUserData ( pVehicle );

    if ( argStream.HasErrors () == false )
    {
        if ( pVehicle )
        {
            if ( CStaticFunctionDefinitions::RemoveVehicleSirens ( pVehicle ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetVehicleSirens ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    CVehicle* pVehicle = NULL;
    unsigned char ucSirenID = 0;
    SSirenInfo tSirenInfo;

    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( ucSirenID );
    if ( ucSirenID > 0 && ucSirenID < 9 )
    {
        // Array indicies start at 0 so compensate here. This way all code works properly and we get nice 1-8 numbers for API
        ucSirenID--;
        argStream.ReadVector3D ( tSirenInfo.m_tSirenInfo[ucSirenID].m_vecSirenPositions );
        argStream.ReadNumber ( tSirenInfo.m_tSirenInfo[ucSirenID].m_RGBBeaconColour.R );
        argStream.ReadNumber ( tSirenInfo.m_tSirenInfo[ucSirenID].m_RGBBeaconColour.G );
        argStream.ReadNumber ( tSirenInfo.m_tSirenInfo[ucSirenID].m_RGBBeaconColour.B );
        argStream.ReadNumber ( tSirenInfo.m_tSirenInfo[ucSirenID].m_RGBBeaconColour.A, 255 );
        argStream.ReadNumber ( tSirenInfo.m_tSirenInfo[ucSirenID].m_dwMinSirenAlpha, 0 );
        if ( argStream.HasErrors () == false )
        {
            if ( pVehicle )
            {
                if ( CStaticFunctionDefinitions::SetVehicleSirens ( pVehicle, ucSirenID, tSirenInfo ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}
int CLuaFunctionDefs::GetVehicleSirenParams ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    CVehicle* pVehicle = NULL;
    SSirenInfo tSirenInfo;

    argStream.ReadUserData ( pVehicle );
    if ( argStream.HasErrors () == false )
    {
        if ( pVehicle )
        {
            tSirenInfo = pVehicle->m_tSirenBeaconInfo;// Create a new table
            lua_newtable ( luaVM );

            lua_pushstring ( luaVM, "SirenCount" );
            lua_pushnumber ( luaVM, tSirenInfo.m_ucSirenCount );
            lua_settable ( luaVM, -3 ); // End of SirenCount Property

            lua_pushstring ( luaVM, "SirenType" );
            lua_pushnumber ( luaVM, tSirenInfo.m_ucSirenType );
            lua_settable ( luaVM, -3 ); // End of SirenType Property

            lua_pushstring ( luaVM, "Flags" );
            lua_newtable ( luaVM );

            lua_pushstring ( luaVM, "360" );
            lua_pushboolean ( luaVM, tSirenInfo.m_b360Flag );
            lua_settable ( luaVM, -3 ); // End of 360 Property

            lua_pushstring ( luaVM, "DoLOSCheck" );
            lua_pushboolean ( luaVM, tSirenInfo.m_bDoLOSCheck );
            lua_settable ( luaVM, -3 ); // End of DoLOSCheck Property

            lua_pushstring ( luaVM, "UseRandomiser" );
            lua_pushboolean ( luaVM, tSirenInfo.m_bUseRandomiser );
            lua_settable ( luaVM, -3 ); // End of UseRandomiser Property

            lua_pushstring ( luaVM, "Silent" );
            lua_pushboolean ( luaVM, tSirenInfo.m_bSirenSilent );
            lua_settable ( luaVM, -3 ); // End of Silent Property

            lua_settable ( luaVM, -3 ); // End of Flags table

            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleSirens ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    CVehicle* pVehicle = NULL;
    SSirenInfo tSirenInfo;

    argStream.ReadUserData ( pVehicle );
    if ( argStream.HasErrors () == false )
    {
        if ( pVehicle )
        {
            tSirenInfo = pVehicle->m_tSirenBeaconInfo;// Create a new table
            lua_newtable ( luaVM );

            for ( int i = 0; i < tSirenInfo.m_ucSirenCount; i++ )
            {
                lua_pushnumber ( luaVM, i + 1 );
                lua_newtable ( luaVM );

                SSirenBeaconInfo info = tSirenInfo.m_tSirenInfo[i];

                lua_pushstring ( luaVM, "Min_Alpha" );
                lua_pushnumber ( luaVM, info.m_dwMinSirenAlpha );
                lua_settable ( luaVM, -3 ); // End of Min_Alpha property

                lua_pushstring ( luaVM, "Red" );
                lua_pushnumber ( luaVM, info.m_RGBBeaconColour.R );
                lua_settable ( luaVM, -3 ); // End of Red property

                lua_pushstring ( luaVM, "Green" );
                lua_pushnumber ( luaVM, info.m_RGBBeaconColour.G );
                lua_settable ( luaVM, -3 ); // End of Green property

                lua_pushstring ( luaVM, "Blue" );
                lua_pushnumber ( luaVM, info.m_RGBBeaconColour.B );
                lua_settable ( luaVM, -3 ); // End of Blue property

                lua_pushstring ( luaVM, "Alpha" );
                lua_pushnumber ( luaVM, info.m_RGBBeaconColour.A );
                lua_settable ( luaVM, -3 ); // End of Alpha property

                lua_pushstring ( luaVM, "x" );
                lua_pushnumber ( luaVM, info.m_vecSirenPositions.fX );
                lua_settable ( luaVM, -3 ); // End of X property

                lua_pushstring ( luaVM, "y" );
                lua_pushnumber ( luaVM, info.m_vecSirenPositions.fY );
                lua_settable ( luaVM, -3 ); // End of Y property

                lua_pushstring ( luaVM, "z" );
                lua_pushnumber ( luaVM, info.m_vecSirenPositions.fZ );
                lua_settable ( luaVM, -3 ); // End of Z property

                lua_settable ( luaVM, -3 ); // End of Table
            }

            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GiveVehicleSirens ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    CVehicle* pVehicle = NULL;
    unsigned char ucSirenType = 0;
    unsigned char ucSirenCount = 0;
    SSirenInfo tSirenInfo;

    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( ucSirenCount );
    argStream.ReadNumber ( ucSirenType );
    if ( ucSirenCount > 0 )
    {
        argStream.ReadBool ( tSirenInfo.m_b360Flag, false );
        argStream.ReadBool ( tSirenInfo.m_bDoLOSCheck, true );
        argStream.ReadBool ( tSirenInfo.m_bUseRandomiser, true );
        argStream.ReadBool ( tSirenInfo.m_bSirenSilent, false );
        if ( argStream.HasErrors () == false )
        {
            if ( pVehicle )
            {
                if ( CStaticFunctionDefinitions::GiveVehicleSirens ( pVehicle, ucSirenType, ucSirenCount, tSirenInfo ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleMaxPassengers ( lua_State* luaVM )
{
    unsigned int uiModel = 0;

    CScriptArgReader argStream ( luaVM );

    if ( argStream.NextIsUserData () )
    {
        CVehicle * pVehicle;
        argStream.ReadUserData ( pVehicle );

        if ( !argStream.HasErrors () )
            uiModel = pVehicle->GetModel ();
    }
    else
        argStream.ReadNumber ( uiModel );

    if ( !argStream.HasErrors () )
    {
        if ( !CVehicleManager::IsValidModel ( uiModel ) )
        {
            m_pScriptDebugging->LogBadType ( luaVM );
            lua_pushboolean ( luaVM, false );
            return 1;
        }

        unsigned int uiMaxPassengers = CVehicleManager::GetMaxPassengers ( uiModel );

        if ( uiMaxPassengers != 0xFF )
        {
            lua_pushnumber ( luaVM, uiMaxPassengers );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleName ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        SString strVehicleName;
        if ( CStaticFunctionDefinitions::GetVehicleName ( pVehicle, strVehicleName ) )
        {
            lua_pushstring ( luaVM, strVehicleName );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleNameFromModel ( lua_State* luaVM )
{
    unsigned short usModel;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( usModel );

    if ( !argStream.HasErrors () )
    {
        SString strVehicleName;

        if ( CStaticFunctionDefinitions::GetVehicleNameFromModel ( usModel, strVehicleName ) )
        {
            lua_pushstring ( luaVM, strVehicleName );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleOccupant ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    unsigned int uiSeat;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( uiSeat, 0 );

    if ( !argStream.HasErrors () )
    {
        CPed* pPed = CStaticFunctionDefinitions::GetVehicleOccupant ( pVehicle, uiSeat );
        if ( pPed )
        {
            lua_pushelement ( luaVM, pPed );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleOccupants ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        // Create a new table
        lua_newtable ( luaVM );

        // Get the maximum amount of passengers
        unsigned char ucMaxPassengers = pVehicle->GetMaxPassengers ();

        // Make sure that if the vehicle doesn't have any seats, the function returns false
        if ( ucMaxPassengers == 255 )
        {
            lua_pushboolean ( luaVM, false );
            return 1;
        }

        // Add All Occupants
        for ( unsigned char ucSeat = 0; ucSeat <= ucMaxPassengers; ++ucSeat )
        {
            CPed* pPed = pVehicle->GetOccupant ( ucSeat );
            if ( pPed )
            {
                lua_pushnumber ( luaVM, ucSeat );
                lua_pushelement ( luaVM, pPed );
                lua_settable ( luaVM, -3 );
            }
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleController ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        CPed* pPed = CStaticFunctionDefinitions::GetVehicleController ( pVehicle );
        if ( pPed )
        {
            lua_pushelement ( luaVM, pPed );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleRotation ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        CVector vecRotation;
        if ( CStaticFunctionDefinitions::GetVehicleRotation ( pVehicle, vecRotation ) )
        {
            lua_pushnumber ( luaVM, vecRotation.fX );
            lua_pushnumber ( luaVM, vecRotation.fY );
            lua_pushnumber ( luaVM, vecRotation.fZ );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleSirensOn ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        // Does the vehicle have Sirens?
        if ( CVehicleManager::HasSirens ( pVehicle->GetModel () ) || pVehicle->DoesVehicleHaveSirens () )
        {
            lua_pushboolean ( luaVM, pVehicle->IsSirenActive () );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleTurnVelocity ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        CVector vecTurnVelocity;
        if ( CStaticFunctionDefinitions::GetVehicleTurnVelocity ( pVehicle, vecTurnVelocity ) )
        {
            lua_pushnumber ( luaVM, vecTurnVelocity.fX );
            lua_pushnumber ( luaVM, vecTurnVelocity.fY );
            lua_pushnumber ( luaVM, vecTurnVelocity.fZ );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleTurretPosition ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        CVector2D vecPosition;
        if ( CStaticFunctionDefinitions::GetVehicleTurretPosition ( pVehicle, vecPosition ) )
        {
            lua_pushnumber ( luaVM, vecPosition.fX );
            lua_pushnumber ( luaVM, vecPosition.fY );
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsVehicleLocked ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        bool bLocked;
        if ( CStaticFunctionDefinitions::IsVehicleLocked ( pVehicle, bLocked ) )
        {
            lua_pushboolean ( luaVM, bLocked );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehiclesOfType ( lua_State* luaVM )
{
    unsigned int uiModel;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( uiModel );

    if ( !argStream.HasErrors () )
    {
        // Create a new table
        lua_newtable ( luaVM );

        // Add all the vehicles with a matching model
        m_pVehicleManager->GetVehiclesOfType ( uiModel, luaVM );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleUpgradeOnSlot ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    unsigned char ucSlot;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( ucSlot );

    if ( !argStream.HasErrors () )
    {
        unsigned short usUpgrade;
        if ( CStaticFunctionDefinitions::GetVehicleUpgradeOnSlot ( pVehicle, ucSlot, usUpgrade ) )
        {
            lua_pushnumber ( luaVM, usUpgrade );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleUpgradeSlotName ( lua_State* luaVM )
{
    unsigned short usNumber;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( usNumber );

    if ( !argStream.HasErrors () )
    {
        if ( usNumber < 17 )
        {
            SString strUpgradeName;
            if ( CStaticFunctionDefinitions::GetVehicleUpgradeSlotName ( static_cast < unsigned char >( usNumber ), strUpgradeName ) )
            {
                lua_pushstring ( luaVM, strUpgradeName );
                return 1;
            }
        }
        else if ( usNumber >= 1000 && usNumber <= 1193 )
        {
            SString strUpgradeName;
            if ( CStaticFunctionDefinitions::GetVehicleUpgradeSlotName ( usNumber, strUpgradeName ) )
            {
                lua_pushstring ( luaVM, strUpgradeName );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleUpgrades ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades ();
        if ( pUpgrades )
        {
            // Create a new table
            lua_newtable ( luaVM );

            // Add all the upgrades to the table
            const SSlotStates& usSlotStates = pUpgrades->GetSlotStates ();

            unsigned int uiIndex = 0;
            unsigned char ucSlot = 0;
            for ( ; ucSlot < VEHICLE_UPGRADE_SLOTS; ucSlot++ )
            {
                if ( usSlotStates[ucSlot] != 0 )
                {
                    lua_pushnumber ( luaVM, ++uiIndex );
                    lua_pushnumber ( luaVM, usSlotStates[ucSlot] );
                    lua_settable ( luaVM, -3 );
                }
            }

            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleCompatibleUpgrades ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    unsigned char ucSlot;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( ucSlot, 0xFF );

    if ( !argStream.HasErrors () )
    {
        CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades ();
        if ( pUpgrades )
        {
            // Create a new table
            lua_newtable ( luaVM );

            unsigned int uiIndex = 0;
            for ( unsigned short usUpgrade = 1000; usUpgrade <= 1193; usUpgrade++ )
            {
                if ( pUpgrades->IsUpgradeCompatible ( usUpgrade ) )
                {
                    if ( ucSlot != 0xFF )
                    {
                        unsigned char ucUpgradeSlot;
                        if ( !pUpgrades->GetSlotFromUpgrade ( usUpgrade, ucUpgradeSlot ) )
                            continue;

                        if ( ucUpgradeSlot != ucSlot )
                            continue;
                    }

                    // Add this one to a list
                    lua_pushnumber ( luaVM, ++uiIndex );
                    lua_pushnumber ( luaVM, usUpgrade );
                    lua_settable ( luaVM, -3 );
                }
            }
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleDoorState ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    unsigned char ucDoor;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( ucDoor );

    if ( !argStream.HasErrors () )
    {
        unsigned char ucState;
        if ( CStaticFunctionDefinitions::GetVehicleDoorState ( pVehicle, ucDoor, ucState ) )
        {
            lua_pushnumber ( luaVM, ucState );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleWheelStates ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        unsigned char ucFrontLeft, ucRearLeft, ucFrontRight, ucRearRight;
        if ( CStaticFunctionDefinitions::GetVehicleWheelStates ( pVehicle, ucFrontLeft, ucRearLeft, ucFrontRight, ucRearRight ) )
        {
            lua_pushnumber ( luaVM, ucFrontLeft );
            lua_pushnumber ( luaVM, ucRearLeft );
            lua_pushnumber ( luaVM, ucFrontRight );
            lua_pushnumber ( luaVM, ucRearRight );
            return 4;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleLightState ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    unsigned char ucLight;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( ucLight );

    if ( !argStream.HasErrors () )
    {
        unsigned char ucState;
        if ( CStaticFunctionDefinitions::GetVehicleLightState ( pVehicle, ucLight, ucState ) )
        {
            lua_pushnumber ( luaVM, ucState );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefs::GetVehiclePanelState ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    unsigned char ucPanel;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( ucPanel );

    if ( !argStream.HasErrors () )
    {
        unsigned char ucState;
        if ( CStaticFunctionDefinitions::GetVehiclePanelState ( pVehicle, ucPanel, ucState ) )
        {
            lua_pushnumber ( luaVM, ucState );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleOverrideLights ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        unsigned char ucLights;
        if ( CStaticFunctionDefinitions::GetVehicleOverrideLights ( pVehicle, ucLights ) )
        {
            lua_pushnumber ( luaVM, ucLights );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleTowedByVehicle ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        CVehicle* pTowedVehicle = pVehicle->GetTowedVehicle ();
        if ( pTowedVehicle )
        {
            lua_pushelement ( luaVM, pTowedVehicle );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleTowingVehicle ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        CVehicle* pTowedByVehicle = pVehicle->GetTowedByVehicle ();
        if ( pTowedByVehicle )
        {
            lua_pushelement ( luaVM, pTowedByVehicle );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehiclePaintjob ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        unsigned char ucPaintjob;
        if ( CStaticFunctionDefinitions::GetVehiclePaintjob ( pVehicle, ucPaintjob ) )
        {
            lua_pushnumber ( luaVM, ucPaintjob );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehiclePlateText ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        char szPlateText[9] = { 0 }; // 8 chars + \0
        if ( CStaticFunctionDefinitions::GetVehiclePlateText ( pVehicle, szPlateText ) )
        {
            lua_pushstring ( luaVM, szPlateText );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::IsVehicleDamageProof ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        bool bDamageProof;
        if ( CStaticFunctionDefinitions::IsVehicleDamageProof ( pVehicle, bDamageProof ) )
        {
            lua_pushboolean ( luaVM, bDamageProof );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::IsVehicleFuelTankExplodable ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        bool bExplodable;
        if ( CStaticFunctionDefinitions::IsVehicleFuelTankExplodable ( pVehicle, bExplodable ) )
        {
            lua_pushboolean ( luaVM, bExplodable );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::IsVehicleFrozen ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        bool bFrozen = false;
        if ( CStaticFunctionDefinitions::IsVehicleFrozen ( pVehicle, bFrozen ) )
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

int CLuaFunctionDefs::IsVehicleOnGround ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        bool bOnGround;
        if ( CStaticFunctionDefinitions::IsVehicleOnGround ( pVehicle, bOnGround ) )
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


int CLuaFunctionDefs::GetVehicleEngineState ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        bool bState;
        if ( CStaticFunctionDefinitions::GetVehicleEngineState ( pVehicle, bState ) )
        {
            lua_pushboolean ( luaVM, bState );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsTrainDerailed ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        bool bDerailed;
        if ( CStaticFunctionDefinitions::IsTrainDerailed ( pVehicle, bDerailed ) )
        {
            lua_pushboolean ( luaVM, bDerailed );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::IsTrainDerailable ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        bool bDerailable;
        if ( CStaticFunctionDefinitions::IsTrainDerailable ( pVehicle, bDerailable ) )
        {
            lua_pushboolean ( luaVM, bDerailable );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetTrainPosition ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        float fPosition;
        if ( CStaticFunctionDefinitions::GetTrainPosition ( pVehicle, fPosition ) )
        {
            lua_pushnumber ( luaVM, fPosition );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetTrainTrack ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        uchar ucTrack;
        if ( CStaticFunctionDefinitions::GetTrainTrack ( pVehicle, ucTrack ) )
        {
            lua_pushnumber ( luaVM, ucTrack );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetTrainDirection ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        bool bDirection;
        if ( CStaticFunctionDefinitions::GetTrainDirection ( pVehicle, bDirection ) )
        {
            lua_pushboolean ( luaVM, bDirection );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetTrainSpeed ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        float fSpeed;
        if ( CStaticFunctionDefinitions::GetTrainSpeed ( pVehicle, fSpeed ) )
        {
            lua_pushnumber ( luaVM, fSpeed );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::FixVehicle ( lua_State* luaVM )
{
    CElement* pElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::FixVehicle ( pElement ) )
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


int CLuaFunctionDefs::BlowVehicle ( lua_State* luaVM )
{
    CElement* pElement;
    bool bExplode;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bExplode, true );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::BlowVehicle ( pElement, bExplode ) )
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


int CLuaFunctionDefs::IsVehicleBlown ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::IsVehicleBlown ( pVehicle ) )
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

int CLuaFunctionDefs::GetVehicleHeadLightColor ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        SColor color;
        if ( CStaticFunctionDefinitions::GetVehicleHeadLightColor ( pVehicle, color ) )
        {
            lua_pushnumber ( luaVM, color.R );
            lua_pushnumber ( luaVM, color.G );
            lua_pushnumber ( luaVM, color.B );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetVehicleRotation ( lua_State* luaVM )
{
    CElement* pElement;
    CVector vecRotation;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadVector3D ( vecRotation );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleRotation ( pElement, vecRotation ) )
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

int CLuaFunctionDefs::SetVehicleTurnVelocity ( lua_State* luaVM )
{
    CElement* pElement;
    CVector vecTurnVelocity;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadVector3D ( vecTurnVelocity );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleTurnVelocity ( pElement, vecTurnVelocity ) )
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

int CLuaFunctionDefs::SetVehicleColor ( lua_State* luaVM )
{
    CElement* pElement;
    uchar ucParams[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    int i = 0;
    for ( ; i < 12; ++i )
    {
        if ( argStream.NextCouldBeNumber () )
        {
            argStream.ReadNumber ( ucParams[i] );
        }
        else
            break;
    }

    if ( !argStream.HasErrors () )
    {
        CVehicleColor color;

        if ( i == 4 )
        {
            // 4 args mean palette colours
            color.SetPaletteColors ( ucParams[0], ucParams[1], ucParams[2], ucParams[3] );
        }
        else if ( i % 3 == 0 )
        {
            // 3,6,9 or 12 args mean rgb colours
            color.SetRGBColors ( SColorRGBA ( ucParams[0], ucParams[1], ucParams[2], 0 ),
                SColorRGBA ( ucParams[3], ucParams[4], ucParams[5], 0 ),
                SColorRGBA ( ucParams[6], ucParams[7], ucParams[8], 0 ),
                SColorRGBA ( ucParams[9], ucParams[10], ucParams[11], 0 ) );
        }
        else
            argStream.SetCustomError ( "Incorrect number of color arguments" );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetVehicleColor ( pElement, color ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetVehicleLandingGearDown ( lua_State* luaVM )
{
    CElement* pElement;
    bool bLandingGearDown;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bLandingGearDown );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleLandingGearDown ( pElement, bLandingGearDown ) )
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


int CLuaFunctionDefs::IsVehicleTaxiLightOn ( lua_State* luaVM )
{
    CVehicle* pVehicle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors () )
    {
        lua_pushboolean ( luaVM, pVehicle->IsTaxiLightOn () );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetVehicleTaxiLightOn ( lua_State* luaVM )
{
    CElement* pElement;
    bool bTaxiLightOn;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bTaxiLightOn );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleTaxiLightOn ( pElement, bTaxiLightOn ) )
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

int CLuaFunctionDefs::SetVehicleSirensOn ( lua_State* luaVM )
{
    CElement* pElement;
    bool bSirensOn;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bSirensOn );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleSirensOn ( pElement, bSirensOn ) )
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

int CLuaFunctionDefs::AddVehicleUpgrade ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned short usUpgrade;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        if ( argStream.NextIsString () )
        {
            SString strUpgrade = "";
            argStream.ReadString ( strUpgrade );
            if ( strUpgrade == "all" )
            {
                lua_pushboolean ( luaVM, CStaticFunctionDefinitions::AddAllVehicleUpgrades ( pElement ) );
                return 1;
            }
            else
                argStream.m_iIndex--;
        }

        argStream.ReadNumber ( usUpgrade );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::AddVehicleUpgrade ( pElement, usUpgrade ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );


    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::RemoveVehicleUpgrade ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned short usUpgrade;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( usUpgrade );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::RemoveVehicleUpgrade ( pElement, usUpgrade ) )
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

int CLuaFunctionDefs::SetVehicleDoorState ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucDoor;
    unsigned char ucState;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ucDoor );
    argStream.ReadNumber ( ucState );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleDoorState ( pElement, ucDoor, ucState ) )
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


int CLuaFunctionDefs::SetVehicleWheelStates ( lua_State* luaVM )
{
    CElement* pElement;
    int iFrontLeft;
    int iRearLeft;
    int iFrontRight;
    int iRearRight;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( iFrontLeft );
    argStream.ReadNumber ( iRearLeft, -1 );
    argStream.ReadNumber ( iFrontRight, -1 );
    argStream.ReadNumber ( iRearRight, -1 );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleWheelStates ( pElement, iFrontLeft, iRearLeft, iFrontRight, iRearRight ) )
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


int CLuaFunctionDefs::SetVehicleLightState ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucLight;
    unsigned char ucState;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ucLight );
    argStream.ReadNumber ( ucState );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleLightState ( pElement, ucLight, ucState ) )
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


int CLuaFunctionDefs::SetVehiclePanelState ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucPanel;
    unsigned char ucState;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ucPanel );
    argStream.ReadNumber ( ucState );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehiclePanelState ( pElement, ucPanel, ucState ) )
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


int CLuaFunctionDefs::ToggleVehicleRespawn ( lua_State* luaVM )
{
    CElement* pElement;
    bool bRespawn;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bRespawn );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::ToggleVehicleRespawn ( pElement, bRespawn ) )
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

int CLuaFunctionDefs::SetVehicleRespawnDelay ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned long ulTime;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ulTime );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleRespawnDelay ( pElement, ulTime ) )
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

int CLuaFunctionDefs::SetVehicleRespawnPosition ( lua_State* luaVM )
{
    CElement* pElement;
    CVector vecPosition;
    CVector vecRotation;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadVector3D ( vecRotation, CVector () );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleRespawnPosition ( pElement, vecPosition, vecRotation ) )
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

int CLuaFunctionDefs::SetVehicleIdleRespawnDelay ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned long ulTime;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ulTime );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleIdleRespawnDelay ( pElement, ulTime ) )
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


int CLuaFunctionDefs::RespawnVehicle ( lua_State* luaVM )
{
    CElement* pElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::RespawnVehicle ( pElement ) )
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


int CLuaFunctionDefs::ResetVehicleExplosionTime ( lua_State* luaVM )
{
    CElement* pElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::ResetVehicleExplosionTime ( pElement ) )
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

int CLuaFunctionDefs::ResetVehicleIdleTime ( lua_State* luaVM )
{
    CElement* pElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::ResetVehicleIdleTime ( pElement ) )
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


int CLuaFunctionDefs::SpawnVehicle ( lua_State* luaVM )
{
    CElement* pElement;
    CVector vecPosition;
    CVector vecRotation;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadVector3D ( vecRotation, CVector () );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SpawnVehicle ( pElement, vecPosition, vecRotation ) )
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


int CLuaFunctionDefs::SetVehicleOverrideLights ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucLights;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ucLights );

    if ( !argStream.HasErrors () )
    {
        if ( ucLights <= 2 && CStaticFunctionDefinitions::SetVehicleOverrideLights ( pElement, ucLights ) )
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

int CLuaFunctionDefs::AttachTrailerToVehicle ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    CVehicle* pTrailer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadUserData ( pTrailer );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::AttachTrailerToVehicle ( pVehicle, pTrailer ) )
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


int CLuaFunctionDefs::DetachTrailerFromVehicle ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    CVehicle* pTrailer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadUserData ( pTrailer, NULL );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::DetachTrailerFromVehicle ( pVehicle, pTrailer ) )
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

int CLuaFunctionDefs::SetVehicleEngineState ( lua_State* luaVM )
{
    CElement* pElement;
    bool bState;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bState );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleEngineState ( pElement, bState ) )
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


int CLuaFunctionDefs::SetVehicleDirtLevel ( lua_State* luaVM )
{
    CElement* pElement;
    float fDirtLevel;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( fDirtLevel );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleDirtLevel ( pElement, fDirtLevel ) )
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

int CLuaFunctionDefs::SetVehicleDamageProof ( lua_State* luaVM )
{
    CElement* pElement;
    bool bDamageProof;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bDamageProof );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleDamageProof ( pElement, bDamageProof ) )
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


int CLuaFunctionDefs::SetVehiclePaintjob ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucPaintjob;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ucPaintjob );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehiclePaintjob ( pElement, ucPaintjob ) )
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

int CLuaFunctionDefs::SetVehicleFuelTankExplodable ( lua_State* luaVM )
{
    CElement* pElement;
    bool bExplodable;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bExplodable );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleFuelTankExplodable ( pElement, bExplodable ) )
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

int CLuaFunctionDefs::SetVehicleFrozen ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    bool bFrozen;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadBool ( bFrozen );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleFrozen ( pVehicle, bFrozen ) )
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

int CLuaFunctionDefs::SetTrainDerailed ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    bool bDerailed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadBool ( bDerailed );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetTrainDerailed ( pVehicle, bDerailed ) )
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


int CLuaFunctionDefs::SetTrainDerailable ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    bool bDerailable;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadBool ( bDerailable );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetTrainDerailable ( pVehicle, bDerailable ) )
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


int CLuaFunctionDefs::SetTrainDirection ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    bool bDirection;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadBool ( bDirection );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetTrainDirection ( pVehicle, bDirection ) )
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


int CLuaFunctionDefs::SetTrainSpeed ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    float fSpeed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( fSpeed );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetTrainSpeed ( pVehicle, fSpeed ) )
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


int CLuaFunctionDefs::SetTrainTrack ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    uchar ucTrack;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( ucTrack );

    if ( !argStream.HasErrors () )
    {
        if ( ( ucTrack >= 0 ) && ( ucTrack <= 3 ) ) {
            if ( CStaticFunctionDefinitions::SetTrainTrack ( pVehicle, ucTrack ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        m_pScriptDebugging->LogCustom ( luaVM, "track number should be between 0 and 3 inclusive" );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetTrainPosition ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    float fPosition;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( fPosition );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetTrainPosition ( pVehicle, fPosition ) )
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


int CLuaFunctionDefs::SetVehicleHeadLightColor ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    SColor color;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( color.R );
    argStream.ReadNumber ( color.G );
    argStream.ReadNumber ( color.B );
    color.A = 0xFF;

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleHeadLightColor ( pVehicle, color ) )
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

int CLuaFunctionDefs::SetVehicleTurretPosition ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    float fHorizontal;
    float fVertical;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( fHorizontal );
    argStream.ReadNumber ( fVertical );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleTurretPosition ( pVehicle, fHorizontal, fVertical ) )
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

int CLuaFunctionDefs::SetVehicleDoorOpenRatio ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucDoor;
    float fRatio;
    unsigned long ulTime;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ucDoor );
    argStream.ReadNumber ( fRatio );
    argStream.ReadNumber ( ulTime, 0 );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehicleDoorOpenRatio ( pElement, ucDoor, fRatio, ulTime ) )
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


int CLuaFunctionDefs::GetVehicleDoorOpenRatio ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    unsigned char ucDoor;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( ucDoor );

    if ( !argStream.HasErrors () )
    {
        float fRatio;

        if ( CStaticFunctionDefinitions::GetVehicleDoorOpenRatio ( pVehicle, ucDoor, fRatio ) )
        {
            lua_pushnumber ( luaVM, fRatio );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetVehiclePlateText ( lua_State* luaVM )
{
    //  bool setVehiclePlateText ( vehicle theVehicle, string plateText )
    CElement* pElement; SString strText;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadString ( strText );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetVehiclePlateText ( pElement, strText ) )
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
