/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaFxDefs.cpp
*  PURPOSE:     Lua fx definitions class
*  DEVELOPERS:  Jax <>
*
*****************************************************************************/

#include <StdInc.h>

void CLuaFxDefs::LoadFunctions ( void )
{
    CLuaCFunctions::AddFunction ( "fxAddBlood", CLuaFxDefs::fxAddBlood );
    CLuaCFunctions::AddFunction ( "fxAddWood", CLuaFxDefs::fxAddWood );
    CLuaCFunctions::AddFunction ( "fxAddSparks", CLuaFxDefs::fxAddSparks );
    CLuaCFunctions::AddFunction ( "fxAddTyreBurst", CLuaFxDefs::fxAddTyreBurst );
    CLuaCFunctions::AddFunction ( "fxAddBulletImpact", CLuaFxDefs::fxAddBulletImpact );
    CLuaCFunctions::AddFunction ( "fxAddPunchImpact", CLuaFxDefs::fxAddPunchImpact );
    CLuaCFunctions::AddFunction ( "fxAddDebris", CLuaFxDefs::fxAddDebris );
    CLuaCFunctions::AddFunction ( "fxAddGlass", CLuaFxDefs::fxAddGlass );
    CLuaCFunctions::AddFunction ( "fxAddWaterHydrant", CLuaFxDefs::fxAddWaterHydrant );
    CLuaCFunctions::AddFunction ( "fxAddGunshot", CLuaFxDefs::fxAddGunshot );
    CLuaCFunctions::AddFunction ( "fxAddTankFire", CLuaFxDefs::fxAddTankFire );
    CLuaCFunctions::AddFunction ( "fxAddWaterSplash", CLuaFxDefs::fxAddWaterSplash );
    CLuaCFunctions::AddFunction ( "fxAddBulletSplash", CLuaFxDefs::fxAddBulletSplash );
    CLuaCFunctions::AddFunction ( "fxAddFootSplash", CLuaFxDefs::fxAddFootSplash );
}


int CLuaFxDefs::fxAddBlood ( lua_State* luaVM )
{
    // bool fxAddBlood ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ, [int count=1, float fBrightness=1.0] )

    // Verify types
    if ( ( argtype ( 1, LUA_TNUMBER ) || argtype ( 1, LUA_TSTRING ) ) &&
         ( argtype ( 2, LUA_TNUMBER ) || argtype ( 2, LUA_TSTRING ) ) &&
         ( argtype ( 3, LUA_TNUMBER ) || argtype ( 3, LUA_TSTRING ) ) &&
         ( argtype ( 4, LUA_TNUMBER ) || argtype ( 4, LUA_TSTRING ) ) &&
         ( argtype ( 5, LUA_TNUMBER ) || argtype ( 5, LUA_TSTRING ) ) &&
         ( argtype ( 6, LUA_TNUMBER ) || argtype ( 6, LUA_TSTRING ) ) )
    {
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
        CVector vecDirection ( static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                               static_cast < float > ( lua_tonumber ( luaVM, 5 ) ),
                               static_cast < float > ( lua_tonumber ( luaVM, 6 ) ) );

        int iCount = 1;
        float fBrightness = 1.0f;
        if ( argtype ( 7, LUA_TNUMBER ) || argtype ( 7, LUA_TSTRING ) )
            iCount = static_cast < int > ( lua_tonumber ( luaVM, 7 ) );
        if ( argtype ( 8, LUA_TNUMBER ) || argtype ( 8, LUA_TSTRING ) )
            fBrightness = static_cast < float > ( lua_tonumber ( luaVM, 8 ) );

        if ( CStaticFunctionDefinitions::FxAddBlood ( vecPosition, vecDirection, iCount, fBrightness ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "fxAddBlood" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddWood ( lua_State* luaVM )
{
    // bool fxAddWood ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ, [int count=1, float fBrightness=1.0] )

    // Verify types
    if ( ( argtype ( 1, LUA_TNUMBER ) || argtype ( 1, LUA_TSTRING ) ) &&
         ( argtype ( 2, LUA_TNUMBER ) || argtype ( 2, LUA_TSTRING ) ) &&
         ( argtype ( 3, LUA_TNUMBER ) || argtype ( 3, LUA_TSTRING ) ) &&
         ( argtype ( 4, LUA_TNUMBER ) || argtype ( 4, LUA_TSTRING ) ) &&
         ( argtype ( 5, LUA_TNUMBER ) || argtype ( 5, LUA_TSTRING ) ) &&
         ( argtype ( 6, LUA_TNUMBER ) || argtype ( 6, LUA_TSTRING ) ) )
    {
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
        CVector vecDirection ( static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                               static_cast < float > ( lua_tonumber ( luaVM, 5 ) ),
                               static_cast < float > ( lua_tonumber ( luaVM, 6 ) ) );

        int iCount = 1;
        float fBrightness = 1.0f;
        if ( argtype ( 7, LUA_TNUMBER ) || argtype ( 7, LUA_TSTRING ) )
            iCount = static_cast < int > ( lua_tonumber ( luaVM, 7 ) );
        if ( argtype ( 8, LUA_TNUMBER ) || argtype ( 8, LUA_TSTRING ) )
            fBrightness = static_cast < float > ( lua_tonumber ( luaVM, 8 ) );

        if ( CStaticFunctionDefinitions::FxAddWood ( vecPosition, vecDirection, iCount, fBrightness ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "fxAddWood" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddSparks ( lua_State* luaVM )
{
    // bool fxAddSparks ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ, [float force=1, int count=1, float acrossLineX=0, float acrossLineY=0, float acrossLineZ=0, bool blur=false, float spread=1, float life=1] )

    // Verify types
    if ( ( argtype ( 1, LUA_TNUMBER ) || argtype ( 1, LUA_TSTRING ) ) &&
         ( argtype ( 2, LUA_TNUMBER ) || argtype ( 2, LUA_TSTRING ) ) &&
         ( argtype ( 3, LUA_TNUMBER ) || argtype ( 3, LUA_TSTRING ) ) &&
         ( argtype ( 4, LUA_TNUMBER ) || argtype ( 4, LUA_TSTRING ) ) &&
         ( argtype ( 5, LUA_TNUMBER ) || argtype ( 5, LUA_TSTRING ) ) &&
         ( argtype ( 6, LUA_TNUMBER ) || argtype ( 6, LUA_TSTRING ) ) )
    {
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
        CVector vecDirection ( static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                               static_cast < float > ( lua_tonumber ( luaVM, 5 ) ),
                               static_cast < float > ( lua_tonumber ( luaVM, 6 ) ) );

        float fForce = 1.0f;
        int iCount = 1;
        CVector vecAcrossLine;
        bool bBlur = false;
        float fSpread = 1.0f;
        float fLife = 1.0f;
        if ( argtype ( 7, LUA_TNUMBER ) || argtype ( 7, LUA_TSTRING ) )
            fForce = static_cast < float > ( lua_tonumber ( luaVM, 7 ) );
        if ( argtype ( 8, LUA_TNUMBER ) || argtype ( 8, LUA_TSTRING ) )
            iCount = static_cast < int > ( lua_tonumber ( luaVM, 8 ) );        
        if ( argtype ( 9, LUA_TNUMBER ) || argtype ( 9, LUA_TSTRING ) )
            vecAcrossLine.fX = static_cast < float > ( lua_tonumber ( luaVM, 9 ) );
        if ( argtype ( 10, LUA_TNUMBER ) || argtype ( 10, LUA_TSTRING ) )
            vecAcrossLine.fY = static_cast < float > ( lua_tonumber ( luaVM, 10 ) );
        if ( argtype ( 11, LUA_TNUMBER ) || argtype ( 11, LUA_TSTRING ) )
            vecAcrossLine.fZ = static_cast < float > ( lua_tonumber ( luaVM, 11 ) );        
        if ( argtype ( 12, LUA_TBOOLEAN ) )
            bBlur = ( lua_toboolean ( luaVM, 12 ) ) ? true:false;       
        if ( argtype ( 13, LUA_TNUMBER ) || argtype ( 13, LUA_TSTRING ) )
            fSpread = static_cast < float > ( lua_tonumber ( luaVM, 13 ) );        
        if ( argtype ( 14, LUA_TNUMBER ) || argtype ( 14, LUA_TSTRING ) )
            fLife = static_cast < float > ( lua_tonumber ( luaVM, 14 ) );

        if ( CStaticFunctionDefinitions::FxAddSparks ( vecPosition, vecDirection, fForce, iCount, vecAcrossLine, bBlur, fSpread, fLife ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "fxAddSparks" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddTyreBurst ( lua_State* luaVM )
{
    // bool fxAddTyreBurst ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ )

    // Verify types
    if ( ( argtype ( 1, LUA_TNUMBER ) || argtype ( 1, LUA_TSTRING ) ) &&
         ( argtype ( 2, LUA_TNUMBER ) || argtype ( 2, LUA_TSTRING ) ) &&
         ( argtype ( 3, LUA_TNUMBER ) || argtype ( 3, LUA_TSTRING ) ) &&
         ( argtype ( 4, LUA_TNUMBER ) || argtype ( 4, LUA_TSTRING ) ) &&
         ( argtype ( 5, LUA_TNUMBER ) || argtype ( 5, LUA_TSTRING ) ) &&
         ( argtype ( 6, LUA_TNUMBER ) || argtype ( 6, LUA_TSTRING ) ) )
    {
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
        CVector vecDirection ( static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                               static_cast < float > ( lua_tonumber ( luaVM, 5 ) ),
                               static_cast < float > ( lua_tonumber ( luaVM, 6 ) ) );
        

        if ( CStaticFunctionDefinitions::FxAddTyreBurst ( vecPosition, vecDirection ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "fxAddTyreBurst" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddBulletImpact ( lua_State* luaVM )
{
    // bool fxAddBulletImpact ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ, [int smokeSize=1, int sparkCount=1, float fSmokeIntensity=1.0] )

    // Verify types
    if ( ( argtype ( 1, LUA_TNUMBER ) || argtype ( 1, LUA_TSTRING ) ) &&
         ( argtype ( 2, LUA_TNUMBER ) || argtype ( 2, LUA_TSTRING ) ) &&
         ( argtype ( 3, LUA_TNUMBER ) || argtype ( 3, LUA_TSTRING ) ) &&
         ( argtype ( 4, LUA_TNUMBER ) || argtype ( 4, LUA_TSTRING ) ) &&
         ( argtype ( 5, LUA_TNUMBER ) || argtype ( 5, LUA_TSTRING ) ) &&
         ( argtype ( 6, LUA_TNUMBER ) || argtype ( 6, LUA_TSTRING ) ) )
    {
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
        CVector vecDirection ( static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                               static_cast < float > ( lua_tonumber ( luaVM, 5 ) ),
                               static_cast < float > ( lua_tonumber ( luaVM, 6 ) ) );

        int iSmokeSize = 1;
        int iSparkCount = 1;
        float fSmokeIntensity = 1.0f;
        if ( argtype ( 7, LUA_TNUMBER ) || argtype ( 7, LUA_TSTRING ) )
            iSmokeSize = static_cast < int > ( lua_tonumber ( luaVM, 7 ) );
        if ( argtype ( 8, LUA_TNUMBER ) || argtype ( 8, LUA_TSTRING ) )
            iSparkCount = static_cast < int > ( lua_tonumber ( luaVM, 8 ) );
        if ( argtype ( 9, LUA_TNUMBER ) || argtype ( 9, LUA_TSTRING ) )
            fSmokeIntensity = static_cast < float > ( lua_tonumber ( luaVM, 9 ) );

        if ( CStaticFunctionDefinitions::FxAddBulletImpact ( vecPosition, vecDirection, iSmokeSize, iSparkCount, fSmokeIntensity ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "fxAddBulletImpact" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddPunchImpact ( lua_State* luaVM )
{
    // bool fxAddPunchImpact ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ )

    // Verify types
    if ( ( argtype ( 1, LUA_TNUMBER ) || argtype ( 1, LUA_TSTRING ) ) &&
         ( argtype ( 2, LUA_TNUMBER ) || argtype ( 2, LUA_TSTRING ) ) &&
         ( argtype ( 3, LUA_TNUMBER ) || argtype ( 3, LUA_TSTRING ) ) &&
         ( argtype ( 4, LUA_TNUMBER ) || argtype ( 4, LUA_TSTRING ) ) &&
         ( argtype ( 5, LUA_TNUMBER ) || argtype ( 5, LUA_TSTRING ) ) &&
         ( argtype ( 6, LUA_TNUMBER ) || argtype ( 6, LUA_TSTRING ) ) )
    {
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
        CVector vecDirection ( static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                               static_cast < float > ( lua_tonumber ( luaVM, 5 ) ),
                               static_cast < float > ( lua_tonumber ( luaVM, 6 ) ) );

        if ( CStaticFunctionDefinitions::FxAddPunchImpact ( vecPosition, vecDirection ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "fxAddPunchImpact" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddDebris ( lua_State* luaVM )
{
    // bool fxAddDebris ( float posX, float posY, float posZ, [int colorR=255, int colorG=0, int colorB=0, int colorA=255, float scale=1.0, int count=1] )

    // Verify types
    if ( ( argtype ( 1, LUA_TNUMBER ) || argtype ( 1, LUA_TSTRING ) ) &&
         ( argtype ( 2, LUA_TNUMBER ) || argtype ( 2, LUA_TSTRING ) ) &&
         ( argtype ( 3, LUA_TNUMBER ) || argtype ( 3, LUA_TSTRING ) ) )
    {
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );

        RwColor rwColor; rwColor.r = 255; rwColor.g = 0; rwColor.b = 0; rwColor.a = 255;
        float fScale = 1.0f;
        int iCount = 1;
        if ( argtype ( 4, LUA_TNUMBER ) || argtype ( 4, LUA_TSTRING ) )
            rwColor.r = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );
        if ( argtype ( 5, LUA_TNUMBER ) || argtype ( 5, LUA_TSTRING ) )
            rwColor.g = static_cast < unsigned char > ( lua_tonumber ( luaVM, 5 ) );
        if ( argtype ( 6, LUA_TNUMBER ) || argtype ( 6, LUA_TSTRING ) )
            rwColor.b = static_cast < unsigned char > ( lua_tonumber ( luaVM, 6 ) );
        if ( argtype ( 7, LUA_TNUMBER ) || argtype ( 7, LUA_TSTRING ) )
            rwColor.a = static_cast < unsigned char > ( lua_tonumber ( luaVM, 7 ) );
        if ( argtype ( 8, LUA_TNUMBER ) || argtype ( 8, LUA_TSTRING ) )
            fScale = static_cast < float > ( lua_tonumber ( luaVM, 8 ) );
        if ( argtype ( 9, LUA_TNUMBER ) || argtype ( 9, LUA_TSTRING ) )
            iCount = static_cast < int > ( lua_tonumber ( luaVM, 9 ) );

        if ( CStaticFunctionDefinitions::FxAddDebris ( vecPosition, rwColor, fScale, iCount ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "fxAddDebris" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddGlass ( lua_State* luaVM )
{
    // bool fxAddGlass ( float posX, float posY, float posZ, [int colorR=255, int colorG=0, int colorB=0, int colorA=255, float scale=1.0, int count=1] )

    // Verify types
    if ( ( argtype ( 1, LUA_TNUMBER ) || argtype ( 1, LUA_TSTRING ) ) &&
         ( argtype ( 2, LUA_TNUMBER ) || argtype ( 2, LUA_TSTRING ) ) &&
         ( argtype ( 3, LUA_TNUMBER ) || argtype ( 3, LUA_TSTRING ) ) )
    {
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );

        RwColor rwColor; rwColor.r = 255; rwColor.g = 0; rwColor.b = 0; rwColor.a = 255;
        float fScale = 1.0f;
        int iCount = 1;
        if ( argtype ( 4, LUA_TNUMBER ) || argtype ( 4, LUA_TSTRING ) )
            rwColor.r = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );
        if ( argtype ( 5, LUA_TNUMBER ) || argtype ( 5, LUA_TSTRING ) )
            rwColor.g = static_cast < unsigned char > ( lua_tonumber ( luaVM, 5 ) );
        if ( argtype ( 6, LUA_TNUMBER ) || argtype ( 6, LUA_TSTRING ) )
            rwColor.b = static_cast < unsigned char > ( lua_tonumber ( luaVM, 6 ) );
        if ( argtype ( 7, LUA_TNUMBER ) || argtype ( 7, LUA_TSTRING ) )
            rwColor.a = static_cast < unsigned char > ( lua_tonumber ( luaVM, 7 ) );
        if ( argtype ( 8, LUA_TNUMBER ) || argtype ( 8, LUA_TSTRING ) )
            fScale = static_cast < float > ( lua_tonumber ( luaVM, 8 ) );
        if ( argtype ( 9, LUA_TNUMBER ) || argtype ( 9, LUA_TSTRING ) )
            iCount = static_cast < int > ( lua_tonumber ( luaVM, 9 ) );

        if ( CStaticFunctionDefinitions::FxAddGlass ( vecPosition, rwColor, fScale, iCount ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "fxAddGlass" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddWaterHydrant ( lua_State* luaVM )
{
    // bool fxAddWaterHydrant ( float posX, float posY, float posZ )

    // Verify types
    if ( ( argtype ( 1, LUA_TNUMBER ) || argtype ( 1, LUA_TSTRING ) ) &&
         ( argtype ( 2, LUA_TNUMBER ) || argtype ( 2, LUA_TSTRING ) ) &&
         ( argtype ( 3, LUA_TNUMBER ) || argtype ( 3, LUA_TSTRING ) ) )
    {
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );

        if ( CStaticFunctionDefinitions::FxAddWaterHydrant ( vecPosition ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "fxAddWaterHydrant" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddGunshot ( lua_State* luaVM )
{
    // bool fxAddGunshot ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ, [bool includeSparks=true] )

    // Verify types
    if ( ( argtype ( 1, LUA_TNUMBER ) || argtype ( 1, LUA_TSTRING ) ) &&
         ( argtype ( 2, LUA_TNUMBER ) || argtype ( 2, LUA_TSTRING ) ) &&
         ( argtype ( 3, LUA_TNUMBER ) || argtype ( 3, LUA_TSTRING ) ) &&
         ( argtype ( 4, LUA_TNUMBER ) || argtype ( 4, LUA_TSTRING ) ) &&
         ( argtype ( 5, LUA_TNUMBER ) || argtype ( 5, LUA_TSTRING ) ) &&
         ( argtype ( 6, LUA_TNUMBER ) || argtype ( 6, LUA_TSTRING ) ) )
    {
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
        CVector vecDirection ( static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                               static_cast < float > ( lua_tonumber ( luaVM, 5 ) ),
                               static_cast < float > ( lua_tonumber ( luaVM, 6 ) ) );

        bool bIncludeSparks = true;
        if ( argtype ( 7, LUA_TBOOLEAN ) )
            bIncludeSparks = ( lua_toboolean ( luaVM, 7 ) ) ? true:false;       

        if ( CStaticFunctionDefinitions::FxAddGunshot ( vecPosition, vecDirection, bIncludeSparks ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "fxAddGunshot" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddTankFire ( lua_State* luaVM )
{
    // bool fxAddTankFire ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ )

    // Verify types
    if ( ( argtype ( 1, LUA_TNUMBER ) || argtype ( 1, LUA_TSTRING ) ) &&
         ( argtype ( 2, LUA_TNUMBER ) || argtype ( 2, LUA_TSTRING ) ) &&
         ( argtype ( 3, LUA_TNUMBER ) || argtype ( 3, LUA_TSTRING ) ) &&
         ( argtype ( 4, LUA_TNUMBER ) || argtype ( 4, LUA_TSTRING ) ) &&
         ( argtype ( 5, LUA_TNUMBER ) || argtype ( 5, LUA_TSTRING ) ) &&
         ( argtype ( 6, LUA_TNUMBER ) || argtype ( 6, LUA_TSTRING ) ) )
    {
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
        CVector vecDirection ( static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                               static_cast < float > ( lua_tonumber ( luaVM, 5 ) ),
                               static_cast < float > ( lua_tonumber ( luaVM, 6 ) ) );

        if ( CStaticFunctionDefinitions::FxAddTankFire ( vecPosition, vecDirection ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "fxAddTankFire" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddWaterSplash ( lua_State* luaVM )
{
    // bool fxAddWaterSplash ( float posX, float posY, float posZ )

    // Verify types
    if ( ( argtype ( 1, LUA_TNUMBER ) || argtype ( 1, LUA_TSTRING ) ) &&
         ( argtype ( 2, LUA_TNUMBER ) || argtype ( 2, LUA_TSTRING ) ) &&
         ( argtype ( 3, LUA_TNUMBER ) || argtype ( 3, LUA_TSTRING ) ) )
    {
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );

        if ( CStaticFunctionDefinitions::FxAddWaterSplash ( vecPosition ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "fxAddWaterSplash" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddBulletSplash ( lua_State* luaVM )
{
    // bool fxAddBulletSplash ( float posX, float posY, float posZ )

    // Verify types
    if ( ( argtype ( 1, LUA_TNUMBER ) || argtype ( 1, LUA_TSTRING ) ) &&
         ( argtype ( 2, LUA_TNUMBER ) || argtype ( 2, LUA_TSTRING ) ) &&
         ( argtype ( 3, LUA_TNUMBER ) || argtype ( 3, LUA_TSTRING ) ) )
    {
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );

        if ( CStaticFunctionDefinitions::FxAddBulletSplash ( vecPosition ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "fxAddBulletSplash" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddFootSplash ( lua_State* luaVM )
{
    // bool fxAddFootSplash ( float posX, float posY, float posZ )

    // Verify types
    if ( ( argtype ( 1, LUA_TNUMBER ) || argtype ( 1, LUA_TSTRING ) ) &&
         ( argtype ( 2, LUA_TNUMBER ) || argtype ( 2, LUA_TSTRING ) ) &&
         ( argtype ( 3, LUA_TNUMBER ) || argtype ( 3, LUA_TSTRING ) ) )
    {
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );

        if ( CStaticFunctionDefinitions::FxAddFootSplash ( vecPosition ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "fxAddFootSplash" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}