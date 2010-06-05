/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.World.cpp
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

int CLuaFunctionDefs::CreateExplosion ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
        unsigned char ucType = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );
        bool bMakeSound = true;
        float fCamShake = -1.0f;
        bool bDamaging = true;
        if ( lua_istype ( luaVM, 5, LUA_TBOOLEAN ) )
        {
            bMakeSound = ( lua_toboolean ( luaVM, 5 ) ) ? true:false;

            int iArgument6 = lua_type ( luaVM, 6 );
            if ( ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING ) )
            {
                fCamShake = static_cast < float > ( lua_tonumber ( luaVM, 6 ) );

                if ( lua_istype ( luaVM, 7, LUA_TBOOLEAN ) )
                {
                    bDamaging = ( lua_toboolean ( luaVM, 7 ) ) ? true:false;
                }
            }
        }

        if ( CStaticFunctionDefinitions::CreateExplosion ( vecPosition, ucType, bMakeSound, fCamShake, bDamaging ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "createExplosion" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::CreateFire ( lua_State* luaVM )
{
    // Grab the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );

    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
        float fSize = 1.8f;
        if ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING )
            fSize = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );

        if ( CStaticFunctionDefinitions::CreateFire ( vecPosition, fSize ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "createFire" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetTime_ ( lua_State* luaVM )
{
    // Get the time
    unsigned char ucHour, ucMinute;
    if ( CStaticFunctionDefinitions::GetTime ( ucHour, ucMinute ) )
    {
        // Return it
        lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucHour ) );
        lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucMinute ) );
        return 2;
    }

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefs::GetGroundPosition ( lua_State* luaVM )
{
    // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        CVector Vector ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );

        // Get the ground position and return it
        float fGround = g_pGame->GetWorld ()->FindGroundZFor3DPosition ( &Vector );
        lua_pushnumber ( luaVM, fGround );
        return 1;
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getGroundPositionFrom3D" );

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::ProcessLineOfSight ( lua_State * luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );
    int iArgument6 = lua_type ( luaVM, 6 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
        ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) &&
        ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING ) )
    {
        CVector vecStart ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
        CVector vecEnd (   static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 5 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 6 ) ) );

        CColPoint* pColPoint = NULL;
        CClientEntity* pColEntity = NULL;
        bool bCheckBuildings = true, bCheckVehicles = true, bCheckPeds = true, bCheckObjects = true, bCheckDummies = true, bSeeThroughStuff = false, bIgnoreSomeObjectsForCamera = false, bShootThroughStuff = false;
        CEntity* pIgnoredEntity = NULL;

        if ( lua_type ( luaVM, 7 ) == LUA_TBOOLEAN )
        {
            bCheckBuildings = ( lua_toboolean ( luaVM, 7 ) ) ? true:false;
            if ( lua_type ( luaVM, 8 ) == LUA_TBOOLEAN )
            {
                bCheckVehicles = ( lua_toboolean ( luaVM, 8 ) ) ? true:false;
                if ( lua_type ( luaVM, 9 ) == LUA_TBOOLEAN )
                {
                    bCheckPeds = ( lua_toboolean ( luaVM, 9 ) ) ? true:false;
                    if ( lua_type ( luaVM, 10 ) == LUA_TBOOLEAN )
                    {
                        bCheckObjects = ( lua_toboolean ( luaVM, 10 ) ) ? true:false;
                        if ( lua_type ( luaVM, 11 ) == LUA_TBOOLEAN )
                        {
                            bCheckDummies = ( lua_toboolean ( luaVM, 11 ) ) ? true:false;
                            if ( lua_type ( luaVM, 12 ) == LUA_TBOOLEAN )
                            {
                                bSeeThroughStuff = ( lua_toboolean ( luaVM, 12 ) ) ? true:false;
                                if ( lua_type ( luaVM, 13 ) == LUA_TBOOLEAN )
                                {
                                    bIgnoreSomeObjectsForCamera = ( lua_toboolean ( luaVM, 13 ) ) ? true:false;
                                    if ( lua_type ( luaVM, 14 ) == LUA_TBOOLEAN )
                                    {
                                        bShootThroughStuff = ( lua_toboolean ( luaVM, 14 ) ) ? true:false;

                                        if ( lua_type ( luaVM, 15 ) == LUA_TLIGHTUSERDATA )
                                        {
                                            CClientEntity* pEntity = lua_toelement ( luaVM, 15 );
                                            if ( pEntity )
                                            {
                                                switch ( pEntity->GetType () )
                                                {
                                                case CCLIENTPED:
                                                case CCLIENTPLAYER:
                                                    pIgnoredEntity = static_cast < CClientPed* > ( pEntity )->GetGamePlayer ();
                                                    break;
                                                case CCLIENTVEHICLE:
                                                    pIgnoredEntity = static_cast < CClientVehicle* > ( pEntity )->GetGameVehicle ();
                                                    break;
                                                case CCLIENTOBJECT:
                                                    pIgnoredEntity = static_cast < CClientObject* > ( pEntity )->GetGameObject ();
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        bool bCollision;
        if ( CStaticFunctionDefinitions::ProcessLineOfSight ( vecStart, vecEnd, bCollision, &pColPoint, &pColEntity, bCheckBuildings, bCheckVehicles, bCheckPeds, bCheckObjects, bCheckDummies, bSeeThroughStuff, bIgnoreSomeObjectsForCamera, bShootThroughStuff, pIgnoredEntity ) )
        {    
            // Got a collision?
            CVector vecColPosition;
            if ( pColPoint )
            {
                // Get the collision position
                vecColPosition = *pColPoint->GetPosition ();

                // Delete the colpoint
                pColPoint->Destroy ();
            }

            lua_pushboolean ( luaVM, bCollision );
            if ( bCollision )
            {
                lua_pushnumber ( luaVM, vecColPosition.fX );
                lua_pushnumber ( luaVM, vecColPosition.fY );
                lua_pushnumber ( luaVM, vecColPosition.fZ );
                if ( pColEntity )
                    lua_pushelement ( luaVM, pColEntity );
                else
                    lua_pushnil ( luaVM );
                return 5;
            }
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "processLineOfSight" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsLineOfSightClear ( lua_State * luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );
    int iArgument6 = lua_type ( luaVM, 6 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
        ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) &&
        ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING ) )
    {
        CVector vecStart ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
        CVector vecEnd (   static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 5 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 6 ) ) );

        bool bCheckBuildings = true, bCheckVehicles = true, bCheckPeds = true, bCheckObjects = true, bCheckDummies = true, bSeeThroughStuff = false, bIgnoreSomeObjectsForCamera = false, bShootThroughStuff = false;
        CEntity* pIgnoredEntity = NULL;

        if ( lua_type ( luaVM, 7 ) == LUA_TBOOLEAN )
        {
            bCheckBuildings = ( lua_toboolean ( luaVM, 7 ) ) ? true:false;
            if ( lua_type ( luaVM, 8 ) == LUA_TBOOLEAN )
            {
                bCheckVehicles = ( lua_toboolean ( luaVM, 8 ) ) ? true:false;
                if ( lua_type ( luaVM, 9 ) == LUA_TBOOLEAN )
                {
                    bCheckPeds = ( lua_toboolean ( luaVM, 9 ) ) ? true:false;
                    if ( lua_type ( luaVM, 10 ) == LUA_TBOOLEAN )
                    {
                        bCheckObjects = ( lua_toboolean ( luaVM, 10 ) ) ? true:false;
                        if ( lua_type ( luaVM, 11 ) == LUA_TBOOLEAN )
                        {
                            bCheckDummies = ( lua_toboolean ( luaVM, 11 ) ) ? true:false;
                            if ( lua_type ( luaVM, 12 ) == LUA_TBOOLEAN )
                            {
                                bSeeThroughStuff = ( lua_toboolean ( luaVM, 12 ) ) ? true:false;
                                if ( lua_type ( luaVM, 13 ) == LUA_TBOOLEAN )
                                {
                                    if ( lua_type ( luaVM, 14 ) == LUA_TLIGHTUSERDATA )
                                    {
                                        CClientEntity* pEntity = lua_toelement ( luaVM, 14 );
                                        if ( pEntity )
                                        {
                                            switch ( pEntity->GetType () )
                                            {
                                            case CCLIENTPED:
                                            case CCLIENTPLAYER:
                                                pIgnoredEntity = static_cast < CClientPed* > ( pEntity )->GetGamePlayer ();
                                                break;
                                            case CCLIENTVEHICLE:
                                                pIgnoredEntity = static_cast < CClientVehicle* > ( pEntity )->GetGameVehicle ();
                                                break;
                                            case CCLIENTOBJECT:
                                                pIgnoredEntity = static_cast < CClientObject* > ( pEntity )->GetGameObject ();
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        bool bIsClear;
        if ( CStaticFunctionDefinitions::IsLineOfSightClear ( vecStart, vecEnd, bIsClear, bCheckBuildings, bCheckVehicles, bCheckPeds, bCheckObjects, bCheckDummies, bSeeThroughStuff, bIgnoreSomeObjectsForCamera, pIgnoredEntity ) )
        {        
            lua_pushboolean ( luaVM, bIsClear );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isLineOfSightClear" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::TestLineAgainstWater ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );
    int iArgument6 = lua_type ( luaVM, 6 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
        ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) &&
        ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING ) )
    {
        CVector vecStart ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
        CVector vecEnd (   static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 5 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 6 ) ) );

        CVector vecCollision;
        if ( CStaticFunctionDefinitions::TestLineAgainstWater ( vecStart, vecEnd, vecCollision ) )
        {
            lua_pushboolean ( luaVM, true );
            lua_pushnumber ( luaVM, vecCollision.fX );
            lua_pushnumber ( luaVM, vecCollision.fY );
            lua_pushnumber ( luaVM, vecCollision.fZ );
            return 4;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "testLineAgainstWater" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::CreateWater ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        CResource* pResource = pLuaMain->GetResource ();
        if ( pResource )
        {
            int iArgument1  = lua_type ( luaVM, 1 );
            int iArgument2  = lua_type ( luaVM, 2 );
            int iArgument3  = lua_type ( luaVM, 3 );
            int iArgument4  = lua_type ( luaVM, 4 );
            int iArgument5  = lua_type ( luaVM, 5 );
            int iArgument6  = lua_type ( luaVM, 6 );
            int iArgument7  = lua_type ( luaVM, 7 );
            int iArgument8  = lua_type ( luaVM, 8 );
            int iArgument9  = lua_type ( luaVM, 9 );
            int iArgument10 = lua_type ( luaVM, 10 );
            int iArgument11 = lua_type ( luaVM, 11 );
            int iArgument12 = lua_type ( luaVM, 12 );
            if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
                ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
                ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
                ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
                ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) &&
                ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING ) &&
                ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING ) &&
                ( iArgument8 == LUA_TNUMBER || iArgument8 == LUA_TSTRING ) &&
                ( iArgument9 == LUA_TNUMBER || iArgument9 == LUA_TSTRING ) )
            {
                CVector v1 ( (float)lua_tonumber(luaVM, 1), (float)lua_tonumber(luaVM, 2), (float)lua_tonumber(luaVM, 3) );
                CVector v2 ( (float)lua_tonumber(luaVM, 4), (float)lua_tonumber(luaVM, 5), (float)lua_tonumber(luaVM, 6) );
                CVector v3 ( (float)lua_tonumber(luaVM, 7), (float)lua_tonumber(luaVM, 8), (float)lua_tonumber(luaVM, 9) );
                if ( ( iArgument10 == LUA_TNUMBER || iArgument10 == LUA_TSTRING ) &&
                    ( iArgument11 == LUA_TNUMBER || iArgument11 == LUA_TSTRING ) &&
                    ( iArgument12 == LUA_TNUMBER || iArgument12 == LUA_TSTRING ) )
                {
                    CVector v4 ( (float)lua_tonumber(luaVM, 10), (float)lua_tonumber(luaVM, 11), (float)lua_tonumber(luaVM, 12) );
                    bool bShallow = false;
                    if ( lua_type ( luaVM, 13 ) == LUA_TBOOLEAN && lua_toboolean ( luaVM, 13 ) )
                        bShallow = true;
                    lua_pushelement ( luaVM, CStaticFunctionDefinitions::CreateWater (
                        *pResource, &v1, &v2, &v3, &v4, bShallow ) );
                    return 1;
                }
                else
                {
                    bool bShallow = false;
                    if ( lua_type ( luaVM, 10 ) == LUA_TBOOLEAN && lua_toboolean ( luaVM, 10 ) )
                        bShallow = true;
                    lua_pushelement ( luaVM, CStaticFunctionDefinitions::CreateWater (
                        *pResource, &v1, &v2, &v3, NULL, bShallow ) );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadType ( luaVM, "createWater" );
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetWaterLevel ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
        bool bCheckWaves = false;
        if ( lua_type ( luaVM, 4 ) == LUA_TBOOLEAN )
            bCheckWaves = ( lua_toboolean ( luaVM, 4 ) ) ? true:false;

        float fWaterLevel;
        CVector vecUnknown;
        if ( CStaticFunctionDefinitions::GetWaterLevel ( vecPosition, fWaterLevel, bCheckWaves, vecUnknown ) )
        {
            lua_pushnumber ( luaVM, fWaterLevel );
            return 1;
        }
    }
    else if ( iArgument1 == LUA_TLIGHTUSERDATA )
    {
        CClientWater* pWater = lua_towater ( luaVM, 1 );
        if ( pWater )
        {
            float fLevel;
            if ( CStaticFunctionDefinitions::GetWaterLevel ( pWater, fLevel ) )
            {
                lua_pushnumber ( luaVM, fLevel );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getWaterLevel", "water", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getWaterLevel" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetWaterVertexPosition ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientWater* pWater = lua_towater ( luaVM, 1 );
        if ( pWater )
        {
            int iVertexIndex = static_cast < int > ( lua_tonumber ( luaVM, 2 ) );
            CVector vecPosition;
            if ( CStaticFunctionDefinitions::GetWaterVertexPosition ( pWater, iVertexIndex, vecPosition ) )
            {
                lua_pushnumber ( luaVM, vecPosition.fX );
                lua_pushnumber ( luaVM, vecPosition.fY );
                lua_pushnumber ( luaVM, vecPosition.fZ );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getWaterVertexPosition", "water", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getWaterVertexPosition" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetWaterLevel ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        CResource* pResource = pLuaMain->GetResource ();
        if ( pResource )
        {
            int iArgument1 = lua_type ( luaVM, 1 );
            int iArgument2 = lua_type ( luaVM, 2 );
            int iArgument3 = lua_type ( luaVM, 3 );
            int iArgument4 = lua_type ( luaVM, 4 );

            if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
            {
                if ( ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
                    ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
                    ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
                {
                    // (x, y, z, level)
                    CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                        static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                        static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
                    float fLevel = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );
                    if ( CStaticFunctionDefinitions::SetWaterLevel ( &vecPosition, fLevel, pResource ) )
                    {
                        lua_pushboolean ( luaVM, true );
                        return 1;
                    }
                }
                else
                {
                    // (level)
                    float fLevel = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );
                    if ( CStaticFunctionDefinitions::SetWaterLevel ( (CVector *)NULL, fLevel, pResource ) )
                    {
                        lua_pushboolean ( luaVM, true );
                        return 1;
                    }
                }
            }
            else if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
                ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
            {
                // (water, level)
                CClientWater* pWater = lua_towater ( luaVM, 1 );
                float fLevel = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
                if ( pWater )
                {
                    if ( CStaticFunctionDefinitions::SetWaterLevel ( pWater, fLevel, pResource ) )
                    {
                        lua_pushboolean ( luaVM, true );
                        return 1;
                    }
                }
                else
                    m_pScriptDebugging->LogBadPointer ( luaVM, "setWaterLevel", "water", 1 );
            }
            else
                m_pScriptDebugging->LogBadType ( luaVM, "setWaterLevel" );
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetWaterVertexPosition ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );

    if ( iArgument1 == LUA_TLIGHTUSERDATA &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
        ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) )
    {
        CClientWater* pWater = lua_towater ( luaVM, 1 );
        if ( pWater )
        {
            int iVertexIndex = static_cast < int > ( lua_tonumber ( luaVM, 2 ) );
            CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 5 ) ) );
            if ( CStaticFunctionDefinitions::SetWaterVertexPosition ( pWater, iVertexIndex, vecPosition ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setWaterVertexPosition", "water", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setWaterVertexPosition" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetWorldFromScreenPosition ( lua_State * luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        CVector vecScreen ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );

        CVector vecWorld;
        if ( CStaticFunctionDefinitions::GetWorldFromScreenPosition ( vecScreen, vecWorld ) )
        {
            lua_pushnumber ( luaVM, vecWorld.fX );
            lua_pushnumber ( luaVM, vecWorld.fY );
            lua_pushnumber ( luaVM, vecWorld.fZ );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getWorldFromScreenPosition" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetScreenFromWorldPosition ( lua_State * luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        CVector vecWorld ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );

        float fEdgeTolerance = 0;
        bool bRelative = true;

        int iArgument4 = lua_type ( luaVM, 4 );
        if ( ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
        {
            fEdgeTolerance = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );
        }

        int iArgument5 = lua_type ( luaVM, 5 );
        if ( iArgument5 == LUA_TBOOLEAN )
        {
            bRelative = ( lua_toboolean ( luaVM, 5 ) ) ? true:false;
        }

        CVector vecScreen;
        if ( CStaticFunctionDefinitions::GetScreenFromWorldPosition ( vecWorld, vecScreen, fEdgeTolerance, bRelative ) )
        {
            lua_pushnumber ( luaVM, vecScreen.fX );
            lua_pushnumber ( luaVM, vecScreen.fY );
            lua_pushnumber ( luaVM, vecScreen.fZ );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getScreenFromWorldPosition" );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefs::GetWeather ( lua_State* luaVM )
{
    unsigned char ucWeather, ucWeatherBlendingTo;
    if ( CStaticFunctionDefinitions::GetWeather ( ucWeather, ucWeatherBlendingTo ) )
    {
        lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucWeather ) );

        if ( ucWeatherBlendingTo != 0xFF )
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucWeatherBlendingTo ) );
        else
            lua_pushnil ( luaVM );

        return 2;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetZoneName ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( iArgument1 == LUA_TSTRING || iArgument1 == LUA_TNUMBER ) &&
        ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER ) &&
        ( iArgument3 == LUA_TSTRING || iArgument3 == LUA_TNUMBER ) )
    {
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );

        bool bCitiesOnly = false;
        if ( lua_type ( luaVM, 4 ) == LUA_TBOOLEAN )
            bCitiesOnly = ( lua_toboolean ( luaVM, 4 ) ) ? true:false;

        char szZoneName [ 128 ];
        if ( CStaticFunctionDefinitions::GetZoneName ( vecPosition, szZoneName, 128, bCitiesOnly ) )
        {
            lua_pushstring ( luaVM, szZoneName );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getZoneName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetGravity ( lua_State* luaVM )
{
    float fGravity;
    if ( CStaticFunctionDefinitions::GetGravity ( fGravity ) )
    {
        lua_pushnumber ( luaVM, fGravity );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetGameSpeed ( lua_State* luaVM )
{
    float fSpeed;
    if ( CStaticFunctionDefinitions::GetGameSpeed ( fSpeed ) )
    {
        lua_pushnumber ( luaVM, fSpeed );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetMinuteDuration ( lua_State* luaVM )
{
    unsigned long ulDelay;
    if ( CStaticFunctionDefinitions::GetMinuteDuration ( ulDelay ) )
    {
        lua_pushnumber ( luaVM, ulDelay );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetWaveHeight ( lua_State* luaVM )
{
    float fHeight;
    if ( CStaticFunctionDefinitions::GetWaveHeight ( fHeight ) )
    {
        lua_pushnumber ( luaVM, fHeight );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::IsGarageOpen ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER )
    {
        unsigned char ucGarageID = static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) );
        bool bIsOpen;

        if ( CStaticFunctionDefinitions::IsGarageOpen ( ucGarageID, bIsOpen ) )
        {
            lua_pushboolean ( luaVM, bIsOpen );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isGarageOpen" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetGaragePosition ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER )
    {
        unsigned char ucGarageID = static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) );
        CVector vecPosition;

        if ( CStaticFunctionDefinitions::GetGaragePosition ( ucGarageID, vecPosition ) )
        {
            lua_pushnumber ( luaVM, vecPosition.fX );
            lua_pushnumber ( luaVM, vecPosition.fY );
            lua_pushnumber ( luaVM, vecPosition.fZ );
            return 3;
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "GetGaragePosition" );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "GetGaragePosition" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetGarageSize ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER )
    {
        unsigned char ucGarageID = static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) );
        float fDepth;
        float fWidth;
        float fHeight;

        if ( CStaticFunctionDefinitions::GetGarageSize ( ucGarageID, fHeight, fWidth, fDepth ) )
        {
            lua_pushnumber ( luaVM, fHeight );
            lua_pushnumber ( luaVM, fWidth );
            lua_pushnumber ( luaVM, fDepth );
            return 3;
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "GetGarageSize" );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "GetGarageSize" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetGarageBoundingBox ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER )
    {
        unsigned char ucGarageID = static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) );
        float fLeft;
        float fRight;
        float fFront;
        float fBack;

        if ( CStaticFunctionDefinitions::GetGarageBoundingBox ( ucGarageID, fLeft, fRight, fFront, fBack ) )
        {
            lua_pushnumber ( luaVM, fLeft );
            lua_pushnumber ( luaVM, fRight );
            lua_pushnumber ( luaVM, fFront );
            lua_pushnumber ( luaVM, fBack );
            return 4;
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "GetGarageBoundingBox" );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "GetGarageBoundingBox" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetBlurLevel ( lua_State* luaVM )
{
    lua_pushnumber ( luaVM, g_pGame->GetBlurLevel () );
    return 1;
}

int CLuaFunctionDefs::SetBlurLevel ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        if ( CStaticFunctionDefinitions::SetBlurLevel ( static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) ) ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setBlurLevel" );

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetTime ( lua_State* luaVM )
{
    // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        // Set the new time
        if ( CStaticFunctionDefinitions::SetTime ( static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) ), static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) ) ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setTime" );

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetSkyGradient ( lua_State* luaVM )
{
    // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );
    int iArgument6 = lua_type ( luaVM, 6 );

    unsigned char ucTopRed = 0;
    unsigned char ucTopGreen = 0;
    unsigned char ucTopBlue = 0;
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) )
        ucTopRed = static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) );
    if ( ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
        ucTopGreen = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
    if ( ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
        ucTopBlue = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
    unsigned char ucBottomRed = 0;
    unsigned char ucBottomGreen = 0;
    unsigned char ucBottomBlue = 0;
    if ( ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
        ucBottomRed = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );
    if ( ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) )
        ucBottomGreen = static_cast < unsigned char > ( lua_tonumber ( luaVM, 5 ) );
    if ( ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING ) )
        ucBottomBlue = static_cast < unsigned char > ( lua_tonumber ( luaVM, 6 ) );
    // Set the new sky gradient
    if ( CStaticFunctionDefinitions::SetSkyGradient ( ucTopRed, ucTopGreen, ucTopBlue, ucBottomRed, ucBottomGreen, ucBottomBlue ) )
    {
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::ResetSkyGradient ( lua_State* luaVM )
{
    if ( CStaticFunctionDefinitions::ResetSkyGradient () )
    {
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetWaterColor ( lua_State* luaVM )
{
    // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );

    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) && 
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&    
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        unsigned char ucWaterRed   = 0;
        unsigned char ucWaterGreen = 0;
        unsigned char ucWaterBlue  = 0;
        unsigned char ucWaterAlpha = 200;

        ucWaterRed   = static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) );
        ucWaterGreen = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
        ucWaterBlue  = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
        if ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING )
            ucWaterAlpha = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );

        // Set the new water colour
        if ( CStaticFunctionDefinitions::SetWaterColor ( ucWaterRed, ucWaterGreen, ucWaterBlue, ucWaterAlpha ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::ResetWaterColor ( lua_State* luaVM )
{
    if ( CStaticFunctionDefinitions::ResetWaterColor () )
    {
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetWeather ( lua_State* luaVM )
{
    // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        // Set the new time
        if ( CStaticFunctionDefinitions::SetWeather ( static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) ) ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setWeather" );

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;        
}


int CLuaFunctionDefs::SetWeatherBlended ( lua_State* luaVM )
{
    // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        // Set the new time
        if ( CStaticFunctionDefinitions::SetWeatherBlended ( static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) ) ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setWeatherBlended" );

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;        
}


int CLuaFunctionDefs::SetGravity ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        float fGravity = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );
        if ( CStaticFunctionDefinitions::SetGravity ( fGravity ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setGravity" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetGameSpeed ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        float fSpeed = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );
        if ( CStaticFunctionDefinitions::SetGameSpeed ( fSpeed ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setGameSpeed" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetMinuteDuration ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        unsigned long ulDelay = static_cast < unsigned long > ( lua_tonumber ( luaVM, 1 ) );
        if ( CStaticFunctionDefinitions::SetMinuteDuration ( ulDelay ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setMinuteDuration" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetWaveHeight ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        float fHeight = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );
        if ( CStaticFunctionDefinitions::SetWaveHeight ( fHeight ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setWaveHeight" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetGarageOpen ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER && lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        unsigned char ucGarageID = static_cast < unsigned char > ( lua_tonumber( luaVM, 1 ) );
        bool bIsOpen = ( ( lua_toboolean ( luaVM, 2 ) == 0 ) ? false : true );

        if ( CStaticFunctionDefinitions::SetGarageOpen ( ucGarageID, bIsOpen ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setGarageOpen" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetJetpackMaxHeight ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER )
    {
        float fHeight = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );
        if ( CStaticFunctionDefinitions::SetJetpackMaxHeight ( fHeight ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setJetpackMaxHeight" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::IsWorldSpecialPropertyEnabled ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        lua_pushboolean ( luaVM, CStaticFunctionDefinitions::IsWorldSpecialPropertyEnabled (
            lua_tostring ( luaVM, 1 ) ) );
        return 1;
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isWorldSpecialPropertyEnabled" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetWorldSpecialPropertyEnabled ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING && lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        lua_pushboolean ( luaVM, CStaticFunctionDefinitions::SetWorldSpecialPropertyEnabled (
            lua_tostring ( luaVM, 1 ), (lua_toboolean ( luaVM, 2 ) ) ? true : false) );
        return 1;
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setWorldSpecialPropertyEnabled" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetCloudsEnabled ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TBOOLEAN )
    {
        lua_pushboolean ( luaVM, CStaticFunctionDefinitions::SetCloudsEnabled ( lua_toboolean ( luaVM, 1 ) ? true : false ) ); 
        return 1;
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setCloudsEnabled" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetCloudsEnabled ( lua_State* luaVM )
{
      lua_pushboolean ( luaVM, CStaticFunctionDefinitions::GetCloudsEnabled () );
      return 1;
}

