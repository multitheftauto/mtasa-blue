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
    CLuaCFunctions::AddFunction ( "createEffect", CLuaFxDefs::CreateEffect );
    CLuaCFunctions::AddFunction ( "setEffectSpeed", CLuaFxDefs::SetEffectSpeed );
    CLuaCFunctions::AddFunction ( "getEffectSpeed", CLuaFxDefs::GetEffectSpeed );
    CLuaCFunctions::AddFunction ( "setEffectDensity", CLuaFxDefs::SetEffectDensity );
    CLuaCFunctions::AddFunction ( "getEffectDensity", CLuaFxDefs::GetEffectDensity );
}


int CLuaFxDefs::fxAddBlood ( lua_State* luaVM )
{
    // bool fxAddBlood ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ, [int count=1, float fBrightness=1.0] )

    CVector vecPosition, vecDirection;
    int iCount = 1;
    float fBrightness = 1.0f;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadVector3D ( vecDirection );
    argStream.ReadNumber ( iCount, 1 );
    argStream.ReadNumber ( fBrightness, 1.0f );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddBlood ( vecPosition, vecDirection, iCount, fBrightness ) )
        {
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


int CLuaFxDefs::fxAddWood ( lua_State* luaVM )
{
    // bool fxAddWood ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ, [int count=1, float fBrightness=1.0] )

    CVector vecPosition, vecDirection;
    int iCount = 1;
    float fBrightness = 1.0f;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadVector3D ( vecDirection );
    argStream.ReadNumber ( iCount, 1 );
    argStream.ReadNumber ( fBrightness, 1.0f );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddWood ( vecPosition, vecDirection, iCount, fBrightness ) )
        {
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


int CLuaFxDefs::fxAddSparks ( lua_State* luaVM )
{
    // bool fxAddSparks ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ, [float force=1, int count=1, float acrossLineX=0, float acrossLineY=0, float acrossLineZ=0, bool blur=false, float spread=1, float life=1] )

    // Verify types
    CVector vecPosition, vecDirection;
    float fForce = 1.0f;
    int iCount = 1;
    CVector vecAcrossLine;
    bool bBlur = false;
    float fSpread = 1.0f;
    float fLife = 1.0f;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadVector3D ( vecDirection );
    argStream.ReadNumber ( fForce, 1.0f );
    argStream.ReadNumber ( iCount, 1 );
    argStream.ReadVector3D ( vecAcrossLine, vecAcrossLine );
    argStream.ReadBool ( bBlur, false );
    argStream.ReadNumber ( fSpread, 1.0f );
    argStream.ReadNumber ( fLife, 1.0f );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddSparks ( vecPosition, vecDirection, fForce, iCount, vecAcrossLine, bBlur, fSpread, fLife ) )
        {
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


int CLuaFxDefs::fxAddTyreBurst ( lua_State* luaVM )
{
    // bool fxAddTyreBurst ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ )

    CVector vecPosition, vecDirection;
    int iCount = 1;
    float fBrightness = 1.0f;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadVector3D ( vecDirection );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddTyreBurst ( vecPosition, vecDirection ) )
        {
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


int CLuaFxDefs::fxAddBulletImpact ( lua_State* luaVM )
{
    // bool fxAddBulletImpact ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ, [int smokeSize=1, int sparkCount=1, float fSmokeIntensity=1.0] )

    // Verify types
    CVector vecPosition, vecDirection;
    int iSmokeSize = 1;
    int iSparkCount = 1;
    float fSmokeIntensity = 1.0f;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadVector3D ( vecDirection );
    argStream.ReadNumber ( iSmokeSize, 1 );
    argStream.ReadNumber ( iSparkCount, 1 );
    argStream.ReadNumber ( fSmokeIntensity, 1.0f );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddBulletImpact ( vecPosition, vecDirection, iSmokeSize, iSparkCount, fSmokeIntensity ) )
        {
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


int CLuaFxDefs::fxAddPunchImpact ( lua_State* luaVM )
{
    // bool fxAddPunchImpact ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ )

    // Verify types
    CVector vecPosition, vecDirection;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadVector3D ( vecDirection );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddPunchImpact ( vecPosition, vecDirection ) )
        {
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


int CLuaFxDefs::fxAddDebris ( lua_State* luaVM )
{
    // bool fxAddDebris ( float posX, float posY, float posZ, [int colorR=255, int colorG=0, int colorB=0, int colorA=255, float scale=1.0, int count=1] )

    // Verify types
    CVector vecPosition;
    RwColor rwColor; 
    rwColor.r = 255; 
    rwColor.g = 0; 
    rwColor.b = 0; 
    rwColor.a = 255;
    float fScale = 1.0f;
    int iCount = 1;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadNumber ( rwColor.r, 255 );
    argStream.ReadNumber ( rwColor.g, 0 );
    argStream.ReadNumber ( rwColor.b, 0 );
    argStream.ReadNumber ( rwColor.a, 255 );
    argStream.ReadNumber ( fScale, 1.0f );
    argStream.ReadNumber ( iCount, 1 );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddDebris ( vecPosition, rwColor, fScale, iCount ) )
        {
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


int CLuaFxDefs::fxAddGlass ( lua_State* luaVM )
{
    // bool fxAddGlass ( float posX, float posY, float posZ, [int colorR=255, int colorG=0, int colorB=0, int colorA=255, float scale=1.0, int count=1] )

    // Verify types
    CVector vecPosition;
    RwColor rwColor; 
    rwColor.r = 255; 
    rwColor.g = 0; 
    rwColor.b = 0; 
    rwColor.a = 255;
    float fScale = 1.0f;
    int iCount = 1;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadNumber ( rwColor.r, 255 );
    argStream.ReadNumber ( rwColor.g, 0 );
    argStream.ReadNumber ( rwColor.b, 0 );
    argStream.ReadNumber ( rwColor.a, 255 );
    argStream.ReadNumber ( fScale, 1.0f );
    argStream.ReadNumber ( iCount, 1 );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddGlass ( vecPosition, rwColor, fScale, iCount ) )
        {
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


int CLuaFxDefs::fxAddWaterHydrant ( lua_State* luaVM )
{
    // bool fxAddWaterHydrant ( float posX, float posY, float posZ )

    // Verify types
    CVector vecPosition;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddWaterHydrant ( vecPosition ) )
        {
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


int CLuaFxDefs::fxAddGunshot ( lua_State* luaVM )
{
    // bool fxAddGunshot ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ, [bool includeSparks=true] )

    // Verify types
    CVector vecPosition, vecDirection;
    bool bIncludeSparks = true;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadVector3D ( vecDirection );
    argStream.ReadBool ( bIncludeSparks, true );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddGunshot ( vecPosition, vecDirection, bIncludeSparks ) )
        {
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


int CLuaFxDefs::fxAddTankFire ( lua_State* luaVM )
{
    // bool fxAddTankFire ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ )

    // Verify types
    CVector vecPosition, vecDirection;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadVector3D ( vecDirection );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddTankFire ( vecPosition, vecDirection ) )
        {
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


int CLuaFxDefs::fxAddWaterSplash ( lua_State* luaVM )
{
    // bool fxAddWaterSplash ( float posX, float posY, float posZ )

    // Verify types
    CVector vecPosition;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddWaterSplash ( vecPosition ) )
        {
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


int CLuaFxDefs::fxAddBulletSplash ( lua_State* luaVM )
{
    // bool fxAddBulletSplash ( float posX, float posY, float posZ )

    CVector vecPosition;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddBulletSplash ( vecPosition ) )
        {
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


int CLuaFxDefs::fxAddFootSplash ( lua_State* luaVM )
{
    // bool fxAddFootSplash ( float posX, float posY, float posZ )

    CVector vecPosition;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddFootSplash ( vecPosition ) )
        {
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



int CLuaFxDefs::CreateEffect ( lua_State* luaVM )
{
    // bool createEffect ( string fxName, float posX, float posY, float posZ[, float rotX, float rotY, float rotZ] )

    CVector vecPosition;
    CVector vecRotation;
    SString strFxName;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strFxName );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadVector3D ( vecRotation, CVector(0, 0, 0) );

    if ( !argStream.HasErrors ( ) )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource();
            if ( pResource )
            {
                // Create it and return it
                CClientEffect * pFx = CStaticFunctionDefinitions::CreateEffect ( *pResource, strFxName, vecPosition );
                if ( pFx != NULL )
                {
                    pFx->SetRotationDegrees ( vecRotation );
                    lua_pushelement ( luaVM, pFx );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFxDefs::GetEffectSpeed ( lua_State* luaVM )
{
    // float getEffectSpeed ( effect theEffect )

    CClientEffect * pEffect;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData( pEffect );

    if ( !argStream.HasErrors ( ) )
    {
        lua_pushnumber( luaVM, pEffect->GetEffectSpeed() );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::SetEffectSpeed ( lua_State* luaVM )
{
    // void setEffectSpeed ( effect theEffect, float fSpeed )

    CClientEffect * pEffect;
    float fSpeed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData( pEffect );
    argStream.ReadNumber ( fSpeed );

    if ( !argStream.HasErrors ( ) )
    {
        pEffect->SetEffectSpeed ( fSpeed );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::GetEffectDensity ( lua_State* luaVM )
{
    // float getEffectDensity ( effect theEffect )

    CClientEffect * pEffect;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData( pEffect );

    if ( !argStream.HasErrors ( ) )
    {
        lua_pushnumber( luaVM, pEffect->GetEffectDensity() );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::SetEffectDensity ( lua_State* luaVM )
{
    // void setEffectDensity ( effect theEffect, float fDensity )

    CClientEffect * pEffect;
    float fDensity;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData( pEffect );
    argStream.ReadNumber ( fDensity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEffect->SetEffectDensity ( fDensity ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom ( luaVM, "effect density beyond bounds" );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}
